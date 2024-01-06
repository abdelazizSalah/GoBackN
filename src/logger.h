#ifndef LOGGER_H
#define LOGGER_H

#include "defs.h"
#include <fstream>

enum class LogType
{
    PROCESSING,
    SENDING,
    TIME_OUT,
    CONTROL,
    RECEIVING
};

struct LogData
{
    Time time;
    int node;

    SeqNum seq_num;
    std::string payload;
    FrameType frame_type;
    FrameErrorCode error_code;
    Byte trailer;

    int modified;
    bool lost;
    int duplicate;
    Time delay;
};

// A class for logging
// Implements the Singleton desing pattern which means that only one object can be instantiated
class Logger
{

private:
    std::ofstream logfile;

    // A pointer to the unique logger object
    static Logger* logger;

    // The constructor should be private to avoid instantiation
    Logger(std::string filename);

public:
    void log(LogType type, LogData data);

    static Logger* GetLogger(const std::string& filename);

    // Delete copy constructor and assignment operator because the object should be singleton
    Logger(Logger& other) = delete;
    void operator=(const Logger&) = delete;

    ~Logger();
};

#endif
