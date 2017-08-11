#!/bin/bash
LB="\033[1;34m"
NC='\033[0m'
cd `dirname "$0"`

printf "${LB}>> Begin installing dependencies\n${NC}"
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential -y
printf "${LB}>> All dependencies installed\n${NC}"

# installing TORCS
printf "${LB}>> Begin installing TORCS\n${NC}"
sudo apt-get build-dep torcs -y
cd torcs-1.3.6
make
sudo make install
sudo make datainstall
printf "${LB}>> TORCS installed\n${NC}"