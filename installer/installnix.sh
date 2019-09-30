#!/bin/sh

#

# If Debian-based, install the .deb file with the dependencies.
if [ -x "$(command -v "apt")" ]; then 
	sudo apt install git libpoco-dev qt5-default
elif [ -x "$(command -v "apt-get")" ]; then 
	sudo apt-get install git libpoco-dev qt5-default
fi

# If RPM-bsed, install the .rpm file with the dependencies.
# if [ -x "$(command -v "rpm")" ]; then 
	# sudo #
# fi


# Fetch the needed repositories.
# TODO: check whether they exist already: if so, pull new commits instead.
git clone https://github.com/MayaPosch/ByteBauble.git
git clone https://github.com/MayaPosch/NymphMQTT.git
git clone https://github.com/MayaPosch/MQTTCute.git

# Compile and install the dependencies.
make -C ByteBauble/src/cpp/ lib
sudo cp ByteBauble/src/cpp/lib/libbytebauble.a /usr/local/lib/.
sudo cp ByteBauble/src/cpp/src/*.h /usr/local/include/.

make -C NymphMQTT/src/ lib
sudo cp NymphMQTT/src/lib/libnymphmqtt.a /usr/local/lib/.
sudo mkdir /usr/local/include/nymphmqtt
sudo cp NymphMQTT/src/cpp/*.h /usr/local/include/nymphmqtt/.

mkdir -p MQTTCute/build && cd MQTTCute/build && qmake .. && make

# Install
sudo cp MQTTCute/build/MQTTCute /usr/local/bin/.
sudo chmod +x  /usr/local/bin/MQTTCute
