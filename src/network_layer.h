#ifndef NETWORK_LAYER_H
#define NETWORK_LAYER_H

#include "defs.h"
#include "fstream"

// This class simulates the network layer as a simple text file
class NetworkLayer {
    std::ifstream file;
public:
//    just read the message from the input file.
    NetworkLayer(std::string filename);
    bool getMsg(FrameErrorCode& error_code, std::string& payload);
    ~NetworkLayer();
};

#endif
