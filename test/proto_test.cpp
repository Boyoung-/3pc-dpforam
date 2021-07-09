#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <omp.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

#include "cxxopts.hpp"
#include "dpforam.h"
#include "inslbl.h"
#include "simple_socket.h"
#include "ssot.h"
#include "util.h"

using namespace CryptoPP;
using namespace std;

int main(int argc, char *argv[]) {
    cxxopts::Options options(argv[0], "3PC DPF-ORAM IMPLEMENTATION");
    options.positional_help("[optional args]").show_positional_help();
    options.add_options()("par", "Party:[eddie|debbie|charlie]", cxxopts::value<string>())("proto", "Protocol:[dpforam|ssot|inslbl]", cxxopts::value<string>()->default_value("dpforam"))("eip", "Eddie's ip", cxxopts::value<string>()->default_value("127.0.0.1"))("dip", "Debbie's ip", cxxopts::value<string>()->default_value("127.0.0.1"))("tau", "Tau", cxxopts::value<uint>()->default_value("3"))("logn", "LogN", cxxopts::value<uint>()->default_value("12"))("db", "DBytes", cxxopts::value<uint>()->default_value("4"))("thr", "Threads", cxxopts::value<uint>()->default_value("1"))("iter", "Iterations", cxxopts::value<uint>()->default_value("100"));

    auto result = options.parse(argc, argv);
    if (result.count("par") == 0) {
        cout << "No party specified" << endl;
        cout << options.help({"", "Group"}) << endl;
        return 0;
    }
    string party = result["par"].as<string>();
    string proto = result["proto"].as<string>();
    string eddie_ip = result["eip"].as<string>();
    string debbie_ip = result["dip"].as<string>();
    uint tau = result["tau"].as<uint>();
    uint logN = result["logn"].as<uint>();
    uint DBytes = result["db"].as<uint>();
    uint threads = result["thr"].as<uint>();
    uint iters = result["iter"].as<uint>();

    omp_set_num_threads(threads);
    int port = 8000;

    connection *cons[2] = {new simple_socket(), new simple_socket()};
    AutoSeededRandomPool rnd;
    CTR_Mode<AES>::Encryption prgs[2];
    uchar bytes[96];
    for (uint i = 0; i < 96; i++) {
        bytes[i] = i;
    }
    uint offset_DE = 0;
    uint offset_CE = 32;
    uint offset_CD = 64;

    if (party == "eddie") {
        cout << "Establishing connection with debbie... " << flush;
        cons[0]->init_server(port);
        cout << "done" << endl;

        cout << "Establishing connection with charlie... " << flush;
        cons[1]->init_server(port + 1);
        cout << "done" << endl;

        prgs[0].SetKeyWithIV(bytes + offset_DE, 16, bytes + offset_DE + 16);
        prgs[1].SetKeyWithIV(bytes + offset_CE, 16, bytes + offset_CE + 16);
    } else if (party == "debbie") {
        cout << "Connecting with eddie... " << flush;
        cons[1]->init_client(eddie_ip.c_str(), port);
        cout << "done" << endl;

        cout << "Establishing connection with charlie... " << flush;
        cons[0]->init_server(port + 2);
        cout << "done" << endl;

        prgs[0].SetKeyWithIV(bytes + offset_CD, 16, bytes + offset_CD + 16);
        prgs[1].SetKeyWithIV(bytes + offset_DE, 16, bytes + offset_DE + 16);
    } else if (party == "charlie") {
        cout << "Connecting with eddie... " << flush;
        cons[0]->init_client(eddie_ip.c_str(), port + 1);
        cout << "done" << endl;

        cout << "Connecting with debbie... " << flush;
        cons[1]->init_client(debbie_ip.c_str(), port + 2);
        cout << "done" << endl;

        prgs[0].SetKeyWithIV(bytes + offset_CE, 16, bytes + offset_CE + 16);
        prgs[1].SetKeyWithIV(bytes + offset_CD, 16, bytes + offset_CD + 16);
    } else {
        cout << "Incorrect party: " << party << endl;
        delete cons[0];
        delete cons[1];
        return 0;
    }

    protocol *test_proto = NULL;
    if (proto == "ssot") {
        test_proto = new ssot(party.c_str(), cons, &rnd, prgs);
    } else if (proto == "inslbl") {
        test_proto = new inslbl(party.c_str(), cons, &rnd, prgs);
    } else if (proto == "dpforam") {
        unsigned long init_wc = current_timestamp();
        test_proto = new dpforam(party.c_str(), cons, &rnd, prgs, tau, logN,
                                 DBytes, true);
        init_wc = current_timestamp() - init_wc;
        std::cout << "Init Wallclock(microsec): " << init_wc << std::endl;
    } else {
        cout << "Incorrect protocol: " << proto << endl;
    }

    if (test_proto != NULL) {
        test_proto->sync();
        test_proto->test(iters);
        test_proto->sync();
        delete test_proto;
    }

    cout << "Closing connections... " << flush;
    sleep(1);
    cons[0]->close();
    cons[1]->close();
    delete cons[0];
    delete cons[1];
    cout << "done" << endl;

    return 0;
}
