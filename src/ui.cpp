#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include "connection.h"
#include "dpforam.h"
#include "inslbl.h"
#include "protocol.h"
#include "simple_socket.h"
#include "ssot.h"

using namespace std;
using namespace CryptoPP;

int main(int argc, const char* argv[]) {
	if (argc != 3) {
		cout << "Usage: dpforam [party] [protocol]" << endl;
		return 0;
	}

	const char* party = argv[1];
	const char* proto = argv[2];

	const char* host = "127.0.0.1";
	int port = 8000;

	connection* cons[2] = { new simple_socket(), new simple_socket() };
	AutoSeededRandomPool rnd;
	CTR_Mode<AES>::Encryption prgs[2];
	uchar bytes[96];
	for (uint i = 0; i < 96; i++) {
		bytes[i] = i;
	}
	uint offset_DE = 0;
	uint offset_CE = 32;
	uint offset_CD = 64;

	if (strcmp(party, "eddie") == 0) {
		cout << "Establishing connection with debbie... " << flush;
		cons[0]->init_server(port);
		cout << "done" << endl;

		cout << "Establishing connection with charlie... " << flush;
		cons[1]->init_server(port + 1);
		cout << "done" << endl;

		prgs[0].SetKeyWithIV(bytes + offset_DE, 16, bytes + offset_DE + 16);
		prgs[1].SetKeyWithIV(bytes + offset_CE, 16, bytes + offset_CE + 16);

	} else if (strcmp(party, "debbie") == 0) {
		cout << "Connecting with eddie... " << flush;
		cons[1]->init_client(host, port);
		cout << "done" << endl;

		cout << "Establishing connection with charlie... " << flush;
		cons[0]->init_server(port + 2);
		cout << "done" << endl;

		prgs[0].SetKeyWithIV(bytes + offset_CD, 16, bytes + offset_CD + 16);
		prgs[1].SetKeyWithIV(bytes + offset_DE, 16, bytes + offset_DE + 16);

	} else if (strcmp(party, "charlie") == 0) {
		cout << "Connecting with eddie... " << flush;
		cons[0]->init_client(host, port + 1);
		cout << "done" << endl;

		cout << "Connecting with debbie... " << flush;
		cons[1]->init_client(host, port + 2);
		cout << "done" << endl;

		prgs[0].SetKeyWithIV(bytes + offset_CE, 16, bytes + offset_CE + 16);
		prgs[1].SetKeyWithIV(bytes + offset_CD, 16, bytes + offset_CD + 16);

	} else {
		cerr << "Incorrect party: " << party << endl;
		delete cons[0];
		delete cons[1];
		return 0;
	}

	uint tau = 3;
	uint logN = 11;
	uint DBytes = 4;

	protocol* test_proto = NULL;
	if (strcmp(proto, "ssot") == 0) {
		test_proto = new ssot(party, cons, &rnd, prgs);
	} else if (strcmp(proto, "inslbl") == 0) {
		test_proto = new inslbl(party, cons, &rnd, prgs);
	} else if (strcmp(proto, "dpforam") == 0) {
		test_proto = new dpforam(party, cons, &rnd, prgs, tau, logN, DBytes,
				true);
	} else {
		cerr << "Incorrect protocol: " << proto << endl;
	}

	if (test_proto != NULL) {
		test_proto->sync();
		test_proto->test();
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
