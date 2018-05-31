#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <iostream>
#include <string.h>

#include "connection.h"
#include "simple_socket.h"
#include "ssot.h"

using namespace std;
using namespace CryptoPP;

int main(int argc, const char* argv[]) {
	if (argc != 2) {
		cout << "Usage: ui [party]" << endl;
		return 0;
	}

	const char* party = argv[1];
	const char* host = "127.0.0.1";
	int port = 8000;

	connection* cons[2] = { new simple_socket(), new simple_socket() };
	CTR_Mode<AES>::Encryption prgs[2];
	unsigned char bytes[96];
	for (int i = 0; i < 96; i++) {
		bytes[i] = i;
	}
	int offset_DE = 0;
	int offset_CE = 32;
	int offset_CD = 64;

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
		cout << "Incorrect party: " << party << endl;
	}

	ssot test_ssot(party, cons, prgs);
	test_ssot.test();

	cout << "Closing connections... " << flush;
	cons[0]->close();
	cons[1]->close();
	delete cons[0];
	delete cons[1];
	cout << "done" << endl;

	return 0;
}
