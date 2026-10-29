#!/bin/sh
set -e
g++ -std=c++11 -O2 -Wall -o ascii_game main.cpp
echo "Built ./ascii_game"