#ifndef _LOGGER_H_
#define _LOGGER_H_

// LogSink is a backend consuming preformatted messages
// there can be several different instances depending on where
// to send the data
//
// Credit http://stackoverflow.com/a/8338089/710408
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>


//
// Logger
//

enum class Level {
  Info,
  Debug,
  Warn,
  Error,
  Fatal
};

class LogSink;

class Logger {
  
public:
  
  Logger(Level l, LogSink& ls);
  

  
  void operator()(std::string const& message,
                  char const* function,
                  std::string fileName,
                  int line);
  static std::string fileName(std::string path);
  
private:
    Level level_;
  LogSink& sink_;
};


#define LOG(Logger_, Message_)                  \
  Logger_(                                      \
    static_cast<std::ostringstream&>(           \
      std::ostringstream().flush() << Message_  \
    ).str(),                                    \
    __FUNCTION__,                               \
    Logger::fileName(__FILE__),                 \
    __LINE__                                    \
);

Logger& Debug();
Logger& Info();
Logger& Error();


#ifdef NDEBUG
#  define LOG_DEBUG(_) do {} while(0);
#else
#  define LOG_DEBUG(Message_) LOG(Debug(), Message_)
#endif

#define LOG_ERROR(Message_) LOG(Error(), Message_)

#define LOG_INFO(Message_) LOG(Info(), Message_)

//
// Sink
//

class LogSink
{
public:
  ~LogSink();
  LogSink(std::ostream& os);
  
  void fileSink(std::string const& logFile);
  void streamSink(std::ostream& os);
  
  template <typename T>
  inline
  LogSink& operator<<(T const& message);
  void flush();
  
private:
  std::ostream* os_;
  std::ofstream fs_;
};

template <typename T>
inline
LogSink& LogSink::operator<<(T const& message) {
  (*os_) << message;
  return *this;
}

LogSink& DebugSink(std::ostream& os = std::cerr);
LogSink& InfoSink(std::ostream& os = std::cerr);


#endif /* _LOGGER_H_ */
