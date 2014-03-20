//
//  logger.cc
//  logger
//
//  Created by Daniel James on 10/10/2012.
//  Copyright (c) 2012 Daniel James. All rights reserved.
//

#include <unistd.h>

#include "logger.hh"
#include <sstream>
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>

//
// Logger
//
std::string Logger::fileName(std::string path) {
  auto lsl(path.rfind('/'));
  return lsl == std::string::npos ? path : path.substr(lsl+1, path.size());
}

Logger::Logger(Level l, LogSink& ls) : sink_(ls) {
  if (l == Level::Info) {
    ls.fileSink("info.log");
  }
}

void Logger::operator()(std::string const& message,
                        char const* function,
                        std::string fileName,
                        int line) {
  
  typedef std::chrono::system_clock sys_clock;
  std::time_t t = sys_clock::to_time_t(sys_clock::now());
  std::tm tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%F %T")
  << " (" << getpid() << ", " << std::this_thread::get_id() << ")"
  << " " << function
  << " " << fileName << ':' << line
  << " \"" << message << "\"";
  sink_ << oss.str() << '\n';
  sink_.flush();
}

Logger& Debug() {
  static Logger logger(Level::Debug, DebugSink());
  return logger;
}
Logger& Error() {
  static Logger logger(Level::Error, DebugSink());
  return logger;
}
Logger& Info() {
  static Logger logger(Level::Info, InfoSink());
  return logger;
}

//
// Sink
//

LogSink::LogSink(std::ostream& os) : os_(&os) { }

LogSink::~LogSink() { }

void LogSink::fileSink(const std::string &logFile) {
  fs_ = std::ofstream(logFile, std::ios_base::app);
  if (!fs_.good()) {
    std::string func(__FILE__);
    std::string file(__FILE__);
    int line(__LINE__);
    std::ostringstream info;
    info << "ERROR: " << file << ' ' << func << ':' << line;
    info << " !std::ifstream.good() for filename: " << logFile << '\n';
    
    throw (std::runtime_error(info.str()));
  } else {
    os_ = &fs_;
  }
}
void LogSink::flush() {
  os_->flush();
}
void LogSink::streamSink(std::ostream &os) {
  os_ = &os;
}
LogSink& InfoSink(std::ostream& os) {
  static LogSink sink(os);
  return sink;
};
LogSink& DebugSink(std::ostream& os) {
  static LogSink sink(os);
  return sink;
};