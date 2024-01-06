#include "coordinator.h"
#include <fstream>

Define_Module(Coordinator);

void Coordinator::initialize()
{
    std::ifstream file("../input/coordinator.txt");

    if (!file.is_open())
    {
        std::cerr << "Error opening the coordinator file." << std::endl;
    }

    int starting_node;
    float starting_time;
    file >> starting_node >> starting_time;

    // send the initial send_frame msg to the sender at the required starting time
    cMessage *start_msg = new cMessage();
    start_msg->setKind((short int)MsgType::SEND_FRAME);
    sendDelayed(start_msg, starting_time, "out", starting_node);
}

void Coordinator::handleMessage(cMessage *msg) {}
