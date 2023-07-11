#ifndef __connectivity
#define __connectivity__
#include "blockchain.hpp"
#include <fstream>
#include <iostream>

void reset();
void sendBlockToNetwork(Block);
void sendToNetwork(std::string);
std::string recieveFromNetwork();
Block recieveBlockFromNetwork();

#endif