// definition of each variables, asamy el bytes w keda.

#ifndef DEFS_H
#define DEFS_H

#include <bitset>
#include <vector>
#include "frame_m.h"

#define FLAG '$'
#define ESC '/'

typedef double Time;
typedef unsigned int SeqNum;
typedef std::bitset<4> FrameErrorCode;
typedef std::bitset<8> Byte;
typedef std::vector<Byte> ByteStream;

enum class FrameType
{
    NACK,
    ACK,
    DATA,
};

enum class Event
{
    FRAME_ARRIVAL,
    // CHECKSUM_ERR,
    TIMEOUT,
    NETWORK_LAYER_READY,
};

// To be used in self messages in msg->kind
// Add any you need
enum class MsgType
{
    SEND_FRAME, // initial send_frame msg will be sent by coordinator
    TIMEOUT,
};

struct NetworkParameters
{
    int WS;
    int TO;
    double PT;
    double TD;
    double ED;
    double DD;
    double LP;
};

// Add any data you need from protocol -> node
struct ProtocolResponse
{
    Frame_Base *frame = nullptr;
    bool timer = false;
    bool more_msgs = true;
};

// Overloading the << operator of FrameType for logging purposes
std::ostream &operator<<(std::ostream &os, FrameType frame_type);

#endif
