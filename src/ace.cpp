#include <iostream>
#include "../ace.h"
namespace AccelCompEng
{
using namespace std;



int run(const char* header, Factory& factory, int argc, char* argv[], void (*callBack)(Console&))
{
   try
   {
      cout << "Initializing data manager...\n";
      DataMap dataMap(factory);
      cout << "Initializing terminal...\n";
      LinuxTerm::stty_raw();
      LinuxTerm terminal;
      terminal << "Initializing console system...\n";
      Console console(argc,argv,terminal,factory,dataMap,header);
      if ( callBack )
      {
         callBack(console);
      }
      else
      {
         console.run();
      }
   }
   catch(Exception e)
   {
      std::cout << "Fatal Exception Caught!\n";
      std::cout << e.what() << "(" << e.function() << ":" << e.line() <<  ")";
      if ( e.detail() )
      {
         std::cout << ": " << e.detail();
      }
      std::cout << "\n";
      LinuxTerm::stty_cooked();
      return -1;
   }
   catch (std::exception& e)
   {
      std::cout << "Fatal STD Exception Caught!\n";
      std::cout << e.what() << "\n";
      LinuxTerm::stty_cooked();
      return -1;
   }
   catch (...)
   {
      LinuxTerm::stty_cooked();
      throw;
   }
   LinuxTerm::stty_cooked();
   return 0;
}



}
