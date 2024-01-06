#include "go_back.h"

GoBackN::GoBackN()
{
    // empty default constructor.
}

GoBackN::GoBackN(int WS, NetworkParameters parameters, int node_id, cSimpleModule *node_ptr)
{
    MAX_SEQUENCE = WS; // as stated in the document, the maximum seq number is the window size.
    par = parameters;
    this->node_id = node_id;

    buffer = std::vector<Frame_Base *>(WS + 1);
    timers = std::vector<cMessage *>(WS + 1);
    error_codes = std::vector<FrameErrorCode>(WS + 1);

    network_layer = new NetworkLayer("../input/input" + std::to_string(node_id) + ".txt");
    logger = Logger::GetLogger("../log/out.log");

    // initially all of them are 0.
    frame_expected = 0;
    next_frame_to_send = 0;
    ack_expected = 1;
    num_outstanding_frames = 0;
    more_msgs = true;
    node = node_ptr;
}

GoBackN::~GoBackN()
{
}

/*
    This function is responsible for applying binary addition as a utility for the checkSum
    After each byte wise addition, if there was any carry, we create a new byte of value 1, and apply the
    binary addition once again. (the algorithm work in this way).
    TESTED
*/
Byte GoBackN::binaryAddition(std::deque<Byte> bytes)
{
    if (bytes.empty())
        return Byte(0);
    Byte result = bytes[0];
    // EV << result.to_string() << endl;
    bytes.pop_front();
    if (bytes.empty())
        return result;

    for (auto byte : bytes)
    {
        // EV << byte.to_string() << endl;
        int carry = 0;
        for (int i = 0; i < 8; i++)
        {
            // this is the full adder logic.
            int sum = result[i] + byte[i] + carry;
            carry = sum / 2;
            result[i] = sum % 2;
        }
        // this is the logic in the check sum, when there is a carry at the last bit.
        // we create new byte of weight = 1, and add its value to the result.
        // EV << result.to_string() << "  is the result \n";
        if (carry)
        {
            Byte overFlowByte(1);
            result = binaryAddition({result, overFlowByte});
            // EV << result.to_string() << "  is the result after adding carry\n";
        }
    }
    return result;
}

/*
    This function is responsible for creating the check sum on the byte stuffed payload.
    this is done by dividing the message into k blocks each of size one char.
    then applying binary addition.
    if there were any carry, we should add it to the result.
    then after the binary addition, we should apply 1's complement.
    then append the checksum at the trailer of the frame.

    TESTED
*/
char GoBackN::createCheckSum(std::string Payload)
{
    // 1. convert all the characters into bytes.
    std::deque<Byte> bytes;
    for (auto c : Payload)
        bytes.push_back(Byte(c));

    // 2. apply binary addition on them
    //! we need to be careful for the carry.
    Byte summation = binaryAddition(bytes);

    // 3. apply the one's complement
    Byte onesComplement = ~summation;

    // 4. convert the checkSum into char.
    return (char)onesComplement.to_ulong();
}

/*
    This function is responsible for validating the message using the checksum
    TESTED
*/
bool GoBackN::validateCheckSum(Frame_Base *frame)
{
    std::string Payload = frame->getPayload();

    // EV << "recieved string is : " << Payload << endl;
    // 1. get the checksum in form of byte
    Byte checkSum(frame->getTrailer());

    // 2. convert the payLoad into bytes, and append the checkSum with them
    std::deque<Byte> bytes;
    // bytes.push_back(checkSum);
    for (auto c : Payload)
        bytes.push_back(Byte(c));

    // 3. apply binary addition
    Byte summation = binaryAddition(bytes);

    // 4. the result should be all ones, so simply we can get the ones complement, and it must be 0
    Byte onesComplement = ~summation;

    // 5. if 0 return true, else return false.
    // EV << checkSum.to_string() << endl
    //     << onesComplement.to_string() << endl;
    return checkSum == onesComplement;
}

/*
    Utility function used to remove the start and end flags.
    and remove the stuffed bytes.
*/
std::string GoBackN::deFraming(Frame_Base *recievedFrame)
{
    string recievedPayload = recievedFrame->getPayload();

    int len = recievedPayload.length();
    std::string unFramedPayload = "";

    // just to remove the start and the end flags.
    for (int i = 1; i < len - 1; i++)
        unFramedPayload += recievedPayload[i];

    string unStuffedPayload = removeByteStuffing(unFramedPayload);
    return unStuffedPayload;
}

/*
    This function applies the byte stuffing algorithm, by iterating over each character in the original payload
    then if the char was flag or esc, it append the esc before it.
    TESTED.
*/
std::string GoBackN::applyByteStuffing(std::string Payload)
{
    std::string newPayload;
    for (auto c : Payload)
    {
        // append the ESC before any Flag or ESC
        if (c == FLAG || c == ESC)
            newPayload += ESC;
        // always append the character to the newPayload.
        newPayload += c;
    }
    return newPayload;
}

/*
    This is a utility function used to apply deframing at the reciever side.
    TESTED
*/
std::string GoBackN::removeByteStuffing(std::string payload)
{
    bool findEsc = false;
    std::string newPayload = "";

    for (char c : payload)
    {
        if (findEsc)
        {
            findEsc = false;
            newPayload += c;
            continue;
        }
        if (c == ESC)
        {
            findEsc = true;
            continue;
        }
        newPayload += c;
    }
    return newPayload;
}

/*
    This function is responsible for applying the framing on the payload only
    this is done by appending the flag at the begining, then append the payload after applying the byte stuffing
    Then at the end append the trailing flag.
    TESTED
*/

Frame_Base *GoBackN::framing(std::string Payload)
{
    Frame_Base *frame = new Frame_Base();
    // we always send a data.
    frame->setFrameType((int)FrameType::DATA);

    std::string newPayload;

    // first we append the starting flag
    newPayload += FLAG;

    // then apply byte stuffing
    Payload = applyByteStuffing(Payload);
    newPayload += Payload;

    // finally we append the ending flag
    newPayload += FLAG;

    // append the checksum
    char checkSum = createCheckSum(newPayload);

    frame->setPayload(newPayload.c_str());

    // append the sequence number in the header.
    frame->setHeader(next_frame_to_send);

    // Append the checkSum in the trailer.
    frame->setTrailer(checkSum);

    return frame;
}

void GoBackN::startTimer(SeqNum frame_num, Time delay)
{
    cMessage *timer_msg = new cMessage();
    timer_msg->setKind((short)MsgType::TIMEOUT);

    timers[frame_num] = timer_msg;

    if (simTime().dbl() + par.TO + delay == 25.5)
    {
        EV << buffer[frame_num]->getPayload() << ' ' << frame_num << endl;
    }
    node->scheduleAt(simTime().dbl() + par.TO + delay, timer_msg);
}

/*
    This function is us
*/
void GoBackN::stopTimer(SeqNum frame_num)
{
    // cMessage *timer_msg = timers[frame_num];

    // This is used to cancel the timer msg previously sent, thus canceling the timer
    node->cancelEvent(timers[frame_num]);
    // delete timer_msg;
    delete timers[frame_num];
}

void GoBackN::send(SeqNum frame_num, Time delay, bool error)
{
    // A duplicate is sent not the original frame to avoid two parties owning the same frame object
    Frame_Base *frame = buffer[frame_num]->dup();

    FrameErrorCode error_code = error_codes[frame_num];
    bool modification = error_code[3] && error;
    bool loss = error_code[2] && error;
    bool duplication = error_code[1] && error;
    bool error_delay = error_code[0] && error;

    int modified_bit;
    std::string payload = frame->getPayload();
    if (modification)
    {
        std::string modified_payload = addRandomError(payload, modified_bit);
        frame->setPayload(modified_payload.c_str());
    }

    // Logging
    LogData logdata = {
        .time = simTime().dbl() + (delay - par.TD),
        .node = node_id,
        .seq_num = frame_num,
        .payload = frame->getPayload(),
        .trailer = frame->getTrailer(),
        .modified = modification ? modified_bit : -1,
        .lost = loss,
        .duplicate = duplication ? 1 : 0,
        .delay = error_delay ? par.ED : 0,
    };
    logger->log(LogType::SENDING, logdata);

    if (error_delay)
        delay += par.ED;

    if (!loss)
        node->sendDelayed(frame, delay, "out");

    if (duplication)
    {
        Frame_Base *dup_frame = frame->dup();

        // Logging
        logdata.duplicate = 2;
        logdata.time += par.DD;
        logger->log(LogType::SENDING, logdata);

        if (!loss)
            node->sendDelayed(dup_frame, delay + par.DD, "out");
    }

    // return the original payload before modification
    // frame->setPayload(payload.c_str());
}

/*
    utility function used to increment the value of the sequnce number,
    and get it back to 0 if it exceeded the maximum sequnce value.
*/
void GoBackN::increment(SeqNum &seq)
{
    seq = Mod(seq + 1, MAX_SEQUENCE + 1);
}

bool GoBackN::protocol(Event event, Frame_Base *frame)
{
    bool network_layer_enabled = false;
    FrameErrorCode error_code;
    std::string payLoad = "";
    LogData logdata;
    /*
      sender
      next_frame_to_Send = 4; [1,2,3,4]
      expected_akc = 2
      send(4).
      startTimer(4).
      next_frame_toSend = 5.
      -------------------------
      getting ack 4
      (between(2,..,5){
        stopTimer(expected_ack - 1). 2
        stopTimer(expected_ack - 1). 3
        stopTimer(expected_ack - 1). 4
      }

    */
    switch (event)
    {
    case Event::NETWORK_LAYER_READY:
        more_msgs = network_layer->getMsg(error_code, payLoad);

        if (more_msgs)
        {
            // Logging
            logdata = {.time = simTime().dbl(), .node = node_id, .error_code = error_code};
            logger->log(LogType::PROCESSING, logdata);

            Frame_Base *newFrame = framing(payLoad);
            buffer[next_frame_to_send] = newFrame;
            error_codes[next_frame_to_send] = error_code;

            Time delay = par.PT + par.TD;
            send(next_frame_to_send, delay);
            startTimer(next_frame_to_send, par.PT);

            num_outstanding_frames = num_outstanding_frames + 1;
            increment(next_frame_to_send);

            if (num_outstanding_frames < MAX_SEQUENCE)
                network_layer_enabled = true;
        }

        break;
    case Event::FRAME_ARRIVAL:

        if (frame->getFrameType() == (int)FrameType::DATA)
        {
            SeqNum recieved_seq_num = frame->getHeader();

            if (recieved_seq_num == frame_expected)
            {
                // The ack frame is lost with probability par.LP
                //! TODO, check on the uniform real as it returns a constant value.
                bool lost = uniform_real(0, 1) < par.LP;

                // EV << frame->getPayload() << endl;
                bool valid = validateCheckSum(frame);
                if (valid)
                { // +ve ACK
                    std::string received_payload = deFraming(frame);

                    // Logging
                    logdata = {
                        .time = simTime().dbl(),
                        .node = node_id,
                        .seq_num = recieved_seq_num,
                        .payload = received_payload,
                    };
                    logger->log(LogType::RECEIVING, logdata);

                    if (!lost)
                    {
                        // create ack frame
                        Frame_Base *ack_frame = new Frame_Base();
                        // ack on expected + 1
                        ack_frame->setAckNum(Mod(frame_expected + 1, MAX_SEQUENCE + 1));
                        ack_frame->setFrameType((int)FrameType::ACK);
                        node->sendDelayed(ack_frame, par.PT + par.TD, "out");
                    }

                    increment(frame_expected);
                }
                else
                { // -ve ACK
                    if (!lost)
                    {
                        Frame_Base *nack_frame = new Frame_Base();
                        EV << "NACK seqNo: " << frame_expected << endl;
                        nack_frame->setAckNum(frame_expected);
                        nack_frame->setFrameType((int)FrameType::NACK);
                        node->sendDelayed(nack_frame, par.PT + par.TD, "out");
                        // node->sendDelayed(nack_frame, 0.001 + par.TD, "out");
                    }
                }

                // Logging
                logdata = {
                    .time = simTime().dbl() + par.PT,
                    .node = node_id,
                    .seq_num = frame_expected,
                    .frame_type = valid ? FrameType::ACK : FrameType::NACK,
                    .lost = lost,
                };
                logger->log(LogType::CONTROL, logdata);
            }
        }

        // Sender
        else if (frame->getFrameType() == (int)FrameType::ACK)
        {
            SeqNum ack_received = frame->getAckNum();

            // If the window was previously full and a valid ack was received, then reenable the network_layer
            // given that they're more msgs to send.
            bool valid_ack = between(ack_expected, ack_received, next_frame_to_send);
            bool full_window = num_outstanding_frames == MAX_SEQUENCE;
            if (valid_ack && full_window && more_msgs)
                network_layer_enabled = true;

            // Accumulative ack
            while (between(ack_expected, ack_received, next_frame_to_send))
            {
                num_outstanding_frames = num_outstanding_frames - 1;
                stopTimer(Mod(ack_expected - 1, MAX_SEQUENCE + 1));
                increment(ack_expected);
            }
        }
        else
        {
            SeqNum nack_received = frame->getAckNum();
            // stop the previous timer.
            stopTimer(nack_received);
            Time delay = par.PT + 0.001 + par.TD;
            send(nack_received, delay, false);
            startTimer(nack_received, par.PT + 0.001);
        }

        break;

    case Event::TIMEOUT:
        // return the pointer to the start of the window.
        next_frame_to_send = Mod(ack_expected - 1, MAX_SEQUENCE + 1);

        // Logging
        logdata = {.time = simTime().dbl(), .node = node_id, .seq_num = next_frame_to_send};
        logger->log(LogType::TIME_OUT, logdata);

        for (int i = 1; i <= num_outstanding_frames; i++)
        {
            // The first frame that caused the timeout should be send error free
            bool error = (i == 1) ? false : true;

            // Time delay = i * par.PT + par.TD;
            Time delay = i * par.PT + par.TD + 0.001;

            // because we are now at the case which caused the timeout.
            if (i != 1)
                stopTimer(next_frame_to_send);

            send(next_frame_to_send, delay, error);
            startTimer(next_frame_to_send, i * par.PT + 0.001);

            increment(next_frame_to_send);
        }
    }

    return network_layer_enabled;
}
