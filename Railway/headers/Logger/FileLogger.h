#ifndef FILELOGGER_HPP
#define FILELOGGER_HPP

#include <fstream>

namespace ige {
    class FileLogger {
    public:
        enum class e_logType { LOG_ERROR, LOG_WARNING, LOG_INFO };
        explicit FileLogger(const char* fname = "ige_log.txt")
            : numWarnings(0U),
            numErrors(0U)
        {
            myFile.open(fname);
            if (myFile.is_open()) {
                myFile << "Log file created" << std::endl << std::endl;
            } 
        }

        ~FileLogger() {

            if (myFile.is_open()) {
                myFile << std::endl << std::endl;
                myFile << numWarnings << " warnings" << std::endl;
                myFile << numErrors << " errors" << std::endl;

                myFile.close();
            }
        }
        friend FileLogger& operator << (FileLogger& logger, const e_logType l_type) {
            switch (l_type) {
            case e_logType::LOG_ERROR:
                logger.myFile << "[ERROR]: ";
                ++logger.numErrors;
                break;

            case e_logType::LOG_WARNING:
                logger.myFile << "[WARNING]: ";
                ++logger.numWarnings;
                break;

            default:
                logger.myFile << "[INFO]: ";
                break;
            }
            return logger;
        }
        friend FileLogger& operator << (FileLogger& logger, const char* text) {
            logger.myFile << text << std::endl;
            return logger;
        }
        friend FileLogger& operator << (FileLogger& logger, const std::string& text) {
            logger.myFile << text << std::endl;
            return logger;
        }
        FileLogger(const FileLogger&) = delete;
        FileLogger& operator= (const FileLogger&) = delete;
    private:
        std::ofstream           myFile;
        unsigned int            numWarnings;
        unsigned int            numErrors;
    };

}


#endif
