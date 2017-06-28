#!/bin/bash

sudo apt-get install build-essential cmake libv4l-dev pkg-config
sudo apt-get install libgtk2.0-dev libtiff5-dev libjasper-dev libjpeg-dev libpng-dev
sudo apt-get install libgl-dev

cd ~/Downloads
wget http://downloads.sourceforge.net/project/opencvlibrary/opencv-unix/3.2.0/opencv-3.2.0.zip
unzip opencv-3.2.0.zip
cd opencv-3.2.0/
mkdir release
cd release

cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
make
sudo make install
sudo ldconfig

echo "OPENCV LIBS: $(pkg-config opencv --libs)"
echo "Download and instal Qt now" 

