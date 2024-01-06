#include "network_layer.h"
#include <iostream>

NetworkLayer::NetworkLayer(std::string filename)
{
    file.open(filename);

    if (!file.is_open())
    {
        std::cerr << "Error opening the input file." << std::endl;
    }
}

// this function reads only one line, and if there is no more lines, it just returns false.
/*
    this return the error code and the payload.
*/
bool NetworkLayer::getMsg(FrameErrorCode &error_code, std::string &payload)
{
    // check if the file is now finshed or empty. 
    if (file.eof())
        return false;
    
    // read the first four bits. 
    std::string error;
    file >> error >> std::ws;
    error_code = FrameErrorCode(error);

    // read the msg. 
    std::getline(file, payload);

    return true;
}

// to use it -> While (getMsg) ...

NetworkLayer::~NetworkLayer()
{
    file.close();
}
