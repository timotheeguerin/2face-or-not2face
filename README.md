2face-or-not2face
=================


# Run

```bash
# Feed the server with training data
$ ./2face -s ./imgs/
```

# Build On Ubuntu 14.04

Make sure you are using g++-4.9 to have access to c++11 features.
On ubuntu you can get it from the test toolchain.
`libopencv-dev` takes around 250MB to download.

```bash
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt-get -qq update
sudo apt-get -qq install g++-4.9
sudo apt-get -qq install cmake libopencv-dev
```

On Ubuntu 14.04:

```bash
vagrant@vagrant-ubuntu-trusty-64$ CXX=/usr/bin/g++-4.9 cmake --target facerecognition --build .
-- Configuring done
-- Generating done
-- Build files have been written to: /vagrant/2face-or-not2face
vagrant@vagrant-ubuntu-trusty-64$ make
[ 25%] Building CXX object CMakeFiles/2face.dir/src/main.cpp.o
[ 50%] Building CXX object CMakeFiles/2face.dir/src/face_matcher.cpp.o
[ 75%] Building CXX object CMakeFiles/2face.dir/src/helper.cpp.o
[100%] Building CXX object CMakeFiles/2face.dir/vendor/docopt.cpp/docopt.cpp.o
Linking CXX executable 2face
[100%] Built target 2face
```


# Sample Output

Server side:
```bash
$ ./2face -s ./imgs/
Launching server...
de: save.xml
Server training done...
Incoming connection from 127.0.0.1 - sending welcome
Receiving: 
./2face ./imgs/1_2_.jpg
Predicting for image: ./imgs/1_2_.jpg
```

Client side:
```bash
$ ./2face ./imgs/1_2_.jpg
1
```

