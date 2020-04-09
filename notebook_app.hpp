#ifndef NOTEBOOKAPP_H
#define NOTEBOOKAPP_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>

#include "input_widget.hpp"
#include "output_widget.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"  // for the file for make-point,line,text 

class NotebookApp : public QWidget {

	Q_OBJECT 

public:

	NotebookApp(QWidget * parent = nullptr);
	~NotebookApp();

signals:

	void dataReady(QString data);
	void makePointReady(std::vector<double> parameters);
	void makeLineReady(std::vector<double> parameters);
	void makeTextReady(QString text, double x, double y, double scale, double phi);
	void makeLabelReady(QString text, double x, double y, double scale, double phi);
	void initializeScreen();

	public slots: 

	void handle_PlotScript(QString input);

	private slots:
	void start();
	void stop();
	void reset();
	void interrupt();
	
	


private: 

	QWidget * inputWidget;
	QWidget * outputWidget;

	QPushButton * startButton;
	QPushButton * stopButton;
	QPushButton * resetButton;
	QPushButton * interruptButton;

	Interpreter interp;
	Interpreter resetInterp;


	//Graphics parameters
	double x = 0;
	double y = 0;
	double size = 0;
	double thickness = 0;
	double scale = 0; //scale factor
	double phi = 0; //text angle
	std::vector<double> plotParameters; 
	std::thread *guiThread;
	//bool twoThreadsRunning;

	void connectSlots();

	// functions to run plotscript before emitting the signal 
	void error(const std::string & err_str);
	void eval_from_stream(std::istream & stream);
	void eval_from_file(std::string filename);
	void whichSignal(Expression exp);

};

#endif


