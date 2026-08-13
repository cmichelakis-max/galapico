# Adding Scramble game to Galapico - Pi Pico 2 Arcade Emulator

This is a fork of https://github.com/Beaumotplage/galapico Pi Pico 2 Arcade Emulator

That is a port of the excellent Galagino ESP32 emulator to the Pi Pico 2, with 15kHz outputs to a CRT TV/Monitor. 
See here for the Galagino emulator: https://github.com/harbaum/galagino

Issues :
1. Graphics issues
2. Scroll not working 
3. Sound not implemented (2nd Z80 CPU)
4. No stars in background
5. DIP switches need addressing

This is a SINGLE MACHINE build

Instructions :
#Tested on raspberry pi 5 running Linux  Trixie

#From terminal in Linux 

# Install tools for raspberry pico from git

cd ~/

sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib

#  Install pico sdk from git

git clone https://github.com/raspberrypi/pico-sdk.git

cd ~/pico-sdk

# Install some additional modules from git

git submodule update --init 

export PICO_SDK_PATH=~/pico-sdk

cd ~/

# Download Scramble version of galapico from github

git clone https://github.com/cmichelakis-max/galapico.git

cd ~/galapico

mkdir build

cd build 

cmake -DPICO_PLATFORM=rp2350-arm-s -DPICO_BOARD=pico2 ..

cd ~/galapico/romconv

# Download z80 emulator zip file from https://fms.komkon.org/EMUL8/Z80-081707.zip
# into ~/roms directory
# Search internet for scramble arcade ROMS 
# Must be the following set of files:
# For older mame

# colour ROM
 c01s.6e
 
# graphics ROMS
 c1.5h
 c2.5f
 
# audio CPU ROMS
 ot1.5c
 ot2.5d
 ot3.5e
 
# main  CPU ROMS 
 s1.2d
 s2.2e
 s3.2f
 s4.2h
 s5.2j
 s6.2l
 s7.2m
 s8.2p

# for newer mame

 # colour ROM
 82s123.6e
 
 # graphics ROMS
 5f.k
 5h.k
  
 # audio CPU ROMS
 5c
 5d
 5e
 
 # main  CPU ROMS 
 2d.k 
 2e.k
 2f.k 
 2h.k
 2j.k
 2l.k 
 2m.k
 2p.k

# if using newer mame roms conv.sh script must be changed to suit the names
# run conversion roms
chmod 777 ./conv.sh

./conv.sh

cd ~/galapico/galagino
mv Z80.c z80.cpp 

cd ~/galapico/build

make

# Should build now

# Flash ~/build/Galapico.uf2 onto pico2 board usual way
                                                                                             




