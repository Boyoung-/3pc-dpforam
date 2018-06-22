#!/bin/bash

sudo apt update
sudo apt install g++ cmake libssl-dev -y
git clone https://github.com/weidai11/cryptopp.git
cd cryptopp
make
sudo make install
cd ..
rm -rf cryptopp
mkdir build
cd build
cmake ../src
make
