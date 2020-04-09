#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>
#include<chrono>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"

// This is an example of how to to trap Cntrl-C in a cross-platform manner
// it creates a simple REPL event loop and shows how to interrupt it.

#include <csignal>
#include <cstdlib>

// This global is needed for communication between the signal handler
// and the rest of the code. This atomic integer counts the number of times
// Cntl-C has been pressed by not reset by the REPL code.
volatile sig_atomic_t global_status_flag = 0;

// *****************************************************************************
// install a signal handler for Cntl-C on Windows
// *****************************************************************************
#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>

// this function is called when a signal is sent to the process
BOOL WINAPI interrupt_handler(DWORD fdwCtrlType) {

	switch (fdwCtrlType) {
	case CTRL_C_EVENT: // handle Cnrtl-C
					   // if not reset since last call, exit
		if (global_status_flag > 0) {
			exit(EXIT_FAILURE);
		}
		++global_status_flag;
		return TRUE;

	default:
		return FALSE;
	}
}

// install the signal handler
inline void install_handler() { SetConsoleCtrlHandler(interrupt_handler, TRUE); }
// *****************************************************************************

// *****************************************************************************
// install a signal handler for Cntl-C on Unix/Posix
// *****************************************************************************
#elif defined(__APPLE__) || defined(__linux) || defined(__unix) ||             \
    defined(__posix)
#include <unistd.h>

// this function is called when a signal is sent to the process
void interrupt_handler(int signal_num) {

	if (signal_num == SIGINT) { // handle Cnrtl-C
								// if not reset since last call, exit
		if (global_status_flag > 0) {
			exit(EXIT_FAILURE);
		}
		++global_status_flag;
	}
}

// install the signal handler
inline void install_handler() {

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = interrupt_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
}
#endif


void prompt(){
  std::cout << "\nplotscript> ";
}

std::string readline(){
  std::string line;
  std::getline(std::cin, line);

  return line;
}

void error(const std::string & err_str){
  std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str){
  std::cout << "Info: " << err_str << std::endl;
}

int eval_from_stream(std::istream & stream, Interpreter *interp){
  
  if(!interp->parseStream(stream)){
    error("Invalid Program. Could not parse.");
    return EXIT_FAILURE;
  }
  else{
    try{
      Expression exp = interp->evaluate();
      std::cout << exp << std::endl;
    }
    catch(const SemanticError & ex){
      std::cerr << ex.what() << std::endl;
      return EXIT_FAILURE;
    }	
  }

  return EXIT_SUCCESS;
}

int eval_from_file(std::string filename, Interpreter *interp){
      
  std::ifstream ifs(filename);
  
  if(!ifs){
    error("Could not open file for reading.");
    return EXIT_FAILURE;
  }
  
  return eval_from_stream(ifs,interp);
}

int eval_from_command(std::string argexp, Interpreter *interp){

  std::istringstream expression(argexp);

  return eval_from_stream(expression,interp);
}

// A REPL is a repeated read-eval-print loop
void repl(Interpreter *interp, bool &threadReset){

	install_handler();
	
	std::thread *thr1;
	if (threadReset == true) {
		eval_from_file(STARTUP_FILE, interp);
		thr1 = new std::thread(&Interpreter::Kernal, interp);
	}
	
	Interpreter * newInterp = interp;
	//newInterp = interp; // replacing with new interpreter for resetting  

	while (!std::cin.eof()) {

		global_status_flag = 0;
		
		MessageQueue<std::string> &inputMessage = MessageQueue<std::string>::getInstance();
		MessageQueue<messageOut> &outputMessage = MessageQueue<messageOut>::getInstance();
		messageOut output;

		
		prompt();
		std::string line = readline();
		if (line.empty()) continue;

		if (threadReset == false) {
			if (line == "%start") {
				thr1 = new std::thread(&Interpreter::Kernal, interp);
				threadReset = true;
				continue;
			}
			else {
				error("interpreter kernel not running");
				continue;
			}
		}

		//if(line != "%start")
			inputMessage.push(line);

		if (line == "%stop") {
			while (!thr1->joinable()) {
			}
			thr1->join();
			threadReset = false;
			continue;
		}
		else if (line == "%exit") {
			thr1->join();
			exit(EXIT_SUCCESS);
		}
		else if (line == "%reset") {
			if (threadReset == true) {
				while (!thr1->joinable()) {
				}
				thr1->join();
			}
			interp->clear();
			interp = newInterp;
			thr1 = new std::thread(&Interpreter::Kernal, interp);
			continue;
		}
		while (!outputMessage.try_pop(output)) {
			if (global_status_flag > 0) {
				thr1->detach();
				thr1->~thread();
				interp->clear();
				interp = new Interpreter;
				interp = newInterp;
				thr1 = new std::thread(&Interpreter::Kernal, interp);
				error("interpreter kernel interrupted");
				break;
			}
		}
		//outputMessage.wait_and_pop(output);
		if (line != "%start") {
			if (output.isExpression == true) {
				std::cout << output.expMsg << std::endl;
			}
			else {
				std::cerr << output.error << std::endl;
			}
		}
	}
	thr1->join();
		
}

int main(int argc, char *argv[])
{  
	Interpreter *interp = new Interpreter;
	bool threadReset = true;

	if(argc == 2){
    return eval_from_file(argv[1],interp);
  }
  else if(argc == 3){
    if(std::string(argv[1]) == "-e"){
      return eval_from_command(argv[2],interp);
    }
    else{
      error("Incorrect number of command line arguments.");
    }
  }
  else{
    repl(interp,threadReset);
  }
    
  return EXIT_SUCCESS;
}
