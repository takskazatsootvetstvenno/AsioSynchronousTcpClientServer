cd ..
if exist build rmdir /s /q build
if exist install rmdir /s /q install
mkdir build
conan install AsioSynchronousTcpClientServer -if build --build missing -s build_type=Debug -r conancenter
cd build
cmake ../AsioSynchronousTcpClientServer -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
cmake --install .
