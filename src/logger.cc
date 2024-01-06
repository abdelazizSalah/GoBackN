#include "logger.h"
#include <iomanip>
#include <iostream>
#include <omnetpp.h>

using namespace omnetpp;

Logger* Logger::logger = nullptr;

Logger::Logger(std::string filename)
{
    logfile.open(filename);
    if (!logfile.is_open()) {
        std::cerr << "Error opening the log file." << std::endl;
    }
}

Logger* Logger::GetLogger(const std::string& filename) {
    if (logger == nullptr) {
        logger = new Logger(filename);
    }
    return logger;
}

/*
    This function is responsible for writing in the file.
*/
void Logger::log(LogType type, LogData data)
{
    switch (type) {
        //     in each case, we need to duplicate the same msg using EV, before the break.
    case LogType::PROCESSING:
        // Writing in the file
        logfile << "At time [" << data.time << "], Node[" << data.node << "]: "
            << "Introducing channel error with code = [" << data.error_code << "]." << std::endl << std::endl;
        // Writing in the console
        EV << "At time [" << data.time << "], Node[" << data.node << "]: "
            << "Introducing channel error with code = [" << data.error_code << "]." << std::endl << std::endl;
        break;

    case LogType::SENDING:
        // Writing in the file
        logfile << "At time [" << data.time << "], Node[" << data.node << "]: "
            << "[sent] frame with seq_num=[" << data.seq_num << "], payload=[" << data.payload << "], trailer=["
            << data.trailer << "], "
            << "Modified [" << data.modified << "], Lost [" << std::boolalpha << data.lost
            << "], Duplicate [" << data.duplicate << "], Delay [" << data.delay << "]." << std::endl << std::endl;

        // Writing in the console
        EV << "At time [" << data.time << "], Node[" << data.node << "]: "
            << "[sent] frame with seq_num=[" << data.seq_num << "], payload=[" << data.payload << "], trailer=["
            << data.trailer << "], "
            << "Modified [" << data.modified << "], Lost [" << std::boolalpha << data.lost
            << "], Duplicate [" << data.duplicate << "], Delay [" << data.delay << "]." << std::endl << std::endl;
        break;

    case LogType::TIME_OUT:
        // Writing in the file
        logfile << "Time out event at time [" << data.time << "], Node[" << data.node << "], "
            << "for frame with seq_num=[" << data.seq_num << "]." << std::endl << std::endl;
        // Writing in the console
        EV << "Time out event at time [" << data.time << "], Node[" << data.node << "], "
            << "for frame with seq_num=[" << data.seq_num << "]." << std::endl << std::endl;
        break;

    case LogType::CONTROL:
        // Writing in the file
        logfile << "At time [" << data.time << "], Node[" << data.node << "]: "
            << "Sending["
            << data.frame_type << "] with number [" << data.seq_num << "], loss ["
            << std::boolalpha << data.lost << "]." << std::endl << std::endl;
        // Writing in the console
        EV << "At time [" << data.time << "], Node[" << data.node << "]: "
            << "Sending["
            << data.frame_type << "] with number [" << data.seq_num << "], loss ["
            << std::boolalpha << data.lost << "]." << std::endl << std::endl;
        break;

    case LogType::RECEIVING:
        // Writing in the file
        logfile << "At time [" << data.time << "], Node[" << data.node << "]: "
            << "Uploading payload=["
            << data.payload << "] and seq_num=[" << data.seq_num << "] to the network layer." << std::endl << std::endl;
        // Writing in the console
        EV << "At time [" << data.time << "], Node[" << data.node << "]: "
            << "Uploading payload=["
            << data.payload << "] and seq_num=[" << data.seq_num << "] to the network layer." << std::endl << std::endl;
        break;
    }
}

Logger::~Logger() {
    logfile.close();
}