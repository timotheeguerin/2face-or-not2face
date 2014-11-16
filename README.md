2face-or-not2face
=================


# Run

```bash
# Feed the server with training data
$ ./2face -s ./imgs/
```

# Build

```bash
$ cmake
$ make
-- Configuring done
You have changed variables that require your cache to be deleted.
Configure will be re-run and you may have to reset some variables.
The following variables have changed:
CMAKE_CXX_COMPILER= /usr/bin/g++-4.9

-- The C compiler identification is GNU 4.8.2
-- The CXX compiler identification is GNU 4.9.2
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/g++-4.9
-- Check for working CXX compiler: /usr/bin/g++-4.9 -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Configuring done
-- Generating done
-- Build files have been written to: /vagrant/2face-or-not2face
Scanning dependencies of target 2face
make[2]: Warning: File `CMakeFiles/2face.dir/depend.make' has modification time 0.0018 s in the future
[ 20%] Building CXX object CMakeFiles/2face.dir/src/face_matcher.cpp.o
[ 40%] Building CXX object CMakeFiles/2face.dir/src/facerecognition.cpp.o
[ 60%] Building CXX object CMakeFiles/2face.dir/src/helper.cpp.o
[ 80%] Building CXX object CMakeFiles/2face.dir/vendor/docopt.cpp/docopt.cpp.o
[100%] Building CXX object CMakeFiles/2face.dir/main.cpp.o
Linking CXX executable 2face
make[2]: warning:  Clock skew detected.  Your build may be incomplete.
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
 ./2face ./imgs/1_2_.jpg
1
```

