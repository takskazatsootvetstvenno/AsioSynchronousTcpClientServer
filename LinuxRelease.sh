#!/bin/bash
sudo pip install conan
sudo apt-get update
sudo apt-get -y install cmake
sudo apt-get upgrade cmake
cd ..
rm -r build
rm -r install
mkdir build
conan install AsioSynchronousTcpClientServer -if build --build missing -s build_type=Release -r conancenter
cd build
cmake ../AsioSynchronousTcpClientServer -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cmake --install .
