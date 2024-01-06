#include "Node.h"

Define_Module(Node);

void Node::initialize()
{
    // Initialize parameters
    int WS = getParentModule()->par("WS");

    parameters.TO = getParentModule()->par("TO");
    parameters.PT = getParentModule()->par("PT");
    parameters.TD = getParentModule()->par("TD");
    parameters.ED = getParentModule()->par("ED");
    parameters.DD = getParentModule()->par("DD");
    parameters.LP = getParentModule()->par("LP");

    node_id = getNodeId(getName());

    go_back_N = new GoBackN(WS, parameters, node_id, this);
}

void Node::handleMessage(cMessage* msg)
{
    Event event;
    Frame_Base* frame = dynamic_cast<Frame_Base*>(msg);

    if (frame != nullptr)
        event = Event::FRAME_ARRIVAL;

    else if (msg->getKind() == (short)MsgType::SEND_FRAME)
        event = Event::NETWORK_LAYER_READY; 

    else if (msg->getKind() == (short)MsgType::TIMEOUT)
        event = Event::TIMEOUT;
        
    bool network_layer_enabled = go_back_N->protocol(event, frame);

    // Send a self msg after PT to send the next frame, only if the network_layer is enabled to avoid exceeding the window size. 
    if (network_layer_enabled) {
        // the next frame should be sent after finishing the processing of the current msg. 
        Time time;
        if(event ==  Event::FRAME_ARRIVAL)
            time = simTime().dbl();
        else if (event == Event::NETWORK_LAYER_READY)
            time = simTime().dbl() + parameters.PT;
        else 
            return;  // Time out, this wont happen, but for safezone.
        cMessage* msg = new cMessage();
        msg->setKind((short)MsgType::SEND_FRAME);
        scheduleAt(time, msg);
    }
} 

Node::~Node()
{
    delete this->go_back_N;
}
