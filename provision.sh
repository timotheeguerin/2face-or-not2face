#!/bin/bash
 
echo "Provisioning virtual machine..."

echo "Adding ppa to install g++-4.9..."
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test

echo "Updating sources..."
sudo apt-get -qq update

echo "Installing dependencies..."
sudo apt-get -qq install g++-4.9 > /dev/null
sudo apt-get -qq install cmake libopencv-dev > /dev/null

echo "Moving files to /Codejam..."
sudo mkdir /Codejam
cp -rf /vagrant/ /Codejam/

echo "Compiling facerecognition..."
cd /Codejam/
make clean
CXX=/usr/bin/g++-4.9 cmake .
make && \

echo "Adding to global bin folder..."
sudo ln -s /vagrant/facerecognition /usr/local/bin/

echo "Adding facial recognition server to startup..."
#ln -s /vagrant/bootstrap-facerecognition /etc/init.d/

#echo "Adding facial recognition server to profile..."
#echo "cd /Codejam && ./facerecognition --server >> ~/facerecognition.log &" >> ~/.profile


echo "Done Done Done."