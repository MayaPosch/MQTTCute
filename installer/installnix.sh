#!/bin/sh

#

# If Debian-based, use APT.
if [ -x "$(command -v "apt")" ]; then 
	sudo apt install git libpoco-dev qt5-default
elif [ -x "$(command -v "apt-get")" ]; then 
	sudo apt-get install git libpoco-dev qt5-default
fi

# If RPM/YUM-based, use its package manager
if [ -x "$(command -v "yum")" ]; then 
	sudo yum install qt5-qtbase-devel poco-devel
fi

# If RPM/DNF-based, use its package manager
if [ -x "$(command -v "dnf")" ]; then 
	sudo dnf install qt5-devel poco-devel
fi

# If Arch-based, use Pacman.
if [ -x "$(command -v "pacman")" ]; then 
	sudo pacman -S qt5-base poco
fi

# If MSYS2, use Pacman (no sudo).
# if [ -x "$(command -v "rpm")" ]; then 
	# sudo #
# fi


# Fetch the needed repositories.
# TODO: check whether they exist already: if so, only pull new commits instead.
git clone https://github.com/MayaPosch/ByteBauble.git
git clone https://github.com/MayaPosch/NymphMQTT.git
git clone https://github.com/MayaPosch/MQTTCute.git

# Compile and install the dependencies.
# TODO: If updating, clean and make lib if git pull found changes.
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
