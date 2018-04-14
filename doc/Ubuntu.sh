# This is roughly how to build this on Ubuntu.

apt update
apt install git wget snappy build-essential software-properties-common libgl-dev

cd
git clone --recursive https://github.com/Timmmm/robocut.git
mkdir /opt/cmake
mkdir cmake
cd cmake
wget https://cmake.org/files/v3.10/cmake-3.10.3-Linux-x86_64.sh
sh cmake-3.10.3-linux-x86_64.sh --prefix=/opt/cmake
ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake

add-apt-repository ppa:beineri/opt-qt-5.10.1-xenial
apt update
apt install qt510-meta-minimal qt510svg qt510imageformats qt510tools

cd
cd robocut
mkdir build
cd build
cmake ..
make -j4

apt install snapcraft

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

