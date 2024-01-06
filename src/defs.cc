#include "defs.h"
#include <fstream>

// Overloading the << operator for logging purposes
std::ostream& operator<<(std::ostream& os, FrameType frame_type) {
    switch (frame_type) {
    case FrameType::ACK:
        os << "ACK";
        break;
    case FrameType::NACK:
        os << "NACK";
        break;
    case FrameType::DATA:
        os << "DATA";
        break;
    }

    return os;
}