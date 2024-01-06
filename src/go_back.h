#ifndef GO_BACK_H
#define GO_BACK_H

#include "defs.h"
#include "frame_m.h"
#include "logger.h"
#include "network_layer.h"
#include <deque>
#include <omnetpp.h>
#include "utils.h"

using namespace std;
using namespace omnetpp;

/* CONVENTIONS

    The main convention is: "The ack is sent for the next seq num"

    ack_expected -> The ack that I'm expected to recieve (e.g., ack_expected = 2, means the first frame in window has seqnum 1)
    ack_recieved -> Ack for seqnum + 1 (e.g., ack_recieved = 2, means frame with seqnum 1 is recieved correctly)
    next_frame_to_send -> The seq num of the next frame, it hasn't been sent yet

    so the sender window is between [ack_expected-1, next_frame_to_send) 
    (e.g., ack_expected = 2, next_frame_to_send = 7, means the window contains [1,2,3,4,5,6])

    No accumulative ack case:
    if (ack_recieved == ack_expected)
        valid ack

    accumulative ack case:
    if (ack_expected <= ack_recieved <= next_frame_to_send)
        valid ack

=======================================================================================================================================
    
    Initial values:
    ack_expected = 1
    next_frame_to_send = 0

=======================================================================================================================================
    
    WS = MAX_SEQUENCE
        Thus frames inside the window can have seq numbers from 0 -> (MAX_SEQUENCE - 1)
    The sequence numbers go from 0 -> MAX_SEQUENCE
         Which means we have (MAX_SEQUENCE + 1) possible seq numbers
         Buffer size is MAX_SEQUENCE + 1

======================================================================================================================================

    Timing Conventions

    Frames are sent
*/

class GoBackN
{
    NetworkParameters par;
    int node_id;

    int MAX_SEQUENCE;
    vector<Frame_Base *> buffer;
    vector<cMessage *> timers;
    vector<FrameErrorCode> error_codes;

    NetworkLayer *network_layer;
    Logger *logger;

    // This variable represents the receiver window (of size 1)
    SeqNum frame_expected; // The sequence number expected by the receiver

    // The sender window is between [ack_expected, next_frame_to_send]
    SeqNum next_frame_to_send;  // The sequence number of the frame to be send
    SeqNum ack_expected;        // The sequence number of the first unacknowledged frame
    int num_outstanding_frames; // The number of frames currently in buffer (must be <= MAX_SEQUENCE)
    
    bool more_msgs; // A flag to indicate that the network layer doesn't have any more msgs to send

    cSimpleModule *node;

public:
    GoBackN(int WS, NetworkParameters parameters, int node_id, cSimpleModule *node_ptr);
    GoBackN();
    ~GoBackN();
    // ProtocolResponse protocol(Event event, Frame_Base *frame = nullptr);
    bool protocol(Event event, Frame_Base *frame = nullptr);

    // Abdelazizs' functions
private:
    Frame_Base* framing(std::string Payload);
    std::string deFraming(Frame_Base* framedPayload);
    std::string applyByteStuffing(std::string Payload);
    std::string removeByteStuffing(std::string Payload);
    char createCheckSum(std::string Payload);
    Byte binaryAddition(std::deque<Byte> bytes);
    bool validateCheckSum(Frame_Base *frame);

    void increment(SeqNum &seq);
    void startTimer(SeqNum frame_num, Time delay);
    void stopTimer(SeqNum frame_num);
    void send(SeqNum frame_num, Time frame_delay, bool error = true);
};

#endif