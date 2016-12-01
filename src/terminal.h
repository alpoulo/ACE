#ifndef ACCELCOMPENG_TERMINAL_H
#define ACCELCOMPENG_TERMINAL_H
#include <string>
#include "exception.h"
namespace AccelCompEng
{



/// @brief Terminal interface class.
///
/// Defines the behavior of the terminal interface this program uses as basic
/// console based input and output. Some functions are not virtual, either be
/// direct or indirect wraparounds to virtual functions.
class Terminal
{
public:
   struct SystemError : public Exception { using Exception::Exception; };
   struct InvalidUse : public Exception { using Exception::Exception; };
   Terminal() = default;
   Terminal(const Terminal&) = delete;
   Terminal& operator=(const Terminal&) = delete;
   Terminal(Terminal&&) = delete;
   Terminal& operator=(Terminal&&) = delete;
   virtual ~Terminal() = default;
   static Terminal& newline(Terminal&);
   static Terminal& endl(Terminal&);
   static Terminal& flush(Terminal&);
   static Terminal& general(Terminal&);
   static Terminal& warning(Terminal&);
   static Terminal& error(Terminal&);
   Terminal& print(short);
   Terminal& print(unsigned short);
   Terminal& print(int);
   Terminal& print(unsigned int);
   Terminal& print(long);
   Terminal& print(unsigned long);
   Terminal& print(float);
   Terminal& print(double);
   Terminal& print(const char*);
   Terminal& print(const std::string&);
   Terminal& operator<<(Terminal& (*pf)(Terminal&));
   /// Must set the header of the terminal interface to the string provided.
   /// @param newHeader New header
   virtual void header(const std::string& newHeader) = 0;
   /// Must set the precision for all proceeding floating point outputs to the precision given.
   /// @param prec The precision defined by the number of digits displayed past the point.
   virtual void precision(int prec) = 0;
   virtual Terminal& operator<<(short n) = 0;
   virtual Terminal& operator<<(unsigned short n) = 0;
   virtual Terminal& operator<<(int n) = 0;
   virtual Terminal& operator<<(unsigned int n) = 0;
   virtual Terminal& operator<<(long n) = 0;
   virtual Terminal& operator<<(unsigned long n) = 0;
   virtual Terminal& operator<<(float n) = 0;
   virtual Terminal& operator<<(double n) = 0;
   virtual Terminal& operator<<(const char* str) = 0;
   virtual Terminal& operator<<(const std::string& str) = 0;
   /// Must read a single line of input from the user terminal. Must also be blocking until a new
   /// line of input has been read.
   /// @param input Will be overwritten with new line of input from user.
   virtual void operator>>(std::string& input) = 0;
protected:
   /// Defines all possible operators for terminal output.
   enum class Ops
   {
      newline, ///< Make new line to output.
      flush, ///< Flush any buffered output.
      general, ///< Set proceeding output as general message.
      warning, ///< Set proceeding output as warning message.
      error ///< Set proceeding output as error message.
   };
   /// Must take any operator from the enumerated type Ops and process it to output of the terminal.
   /// Some operators change state information which must be saved after call of this function.
   /// @param oper Specific operator being given to output of terminal.
   virtual void set_ops(Ops oper) = 0;
};



}
#endif
