#ifndef UTILS_H
#define UTILS_H

#include "defs.h"

int getNodeId(const char *name);
bool between(SeqNum a, SeqNum b, SeqNum c);
std::string addRandomError(std::string payload, int& modified_bit);
float uniform_real(float a, float b);
int Mod (int n, int m) ;
#endif