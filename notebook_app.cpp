#include "notebook_app.hpp"

NotebookApp::NotebookApp(QWidget *parent) :QWidget(parent)
{
	inputWidget = new InputWidget;
	outputWidget = new OutputWidget;

	inputWidget->setObjectName("input");
	outputWidget->setObjectName("output");

	startButton = new QPushButton("Start Kernel");
	stopButton = new QPushButton("Stop Kernel");
	resetButton = new QPushButton("Reset Kernel");
	interruptButton = new QPushButton("Interrupt Kernel");

	startButton->setObjectName("start");
	stopButton->setObjectName("stop");
	resetButton->setObjectName("reset");
	interruptButton->setObjectName("interrupt");
	//twoThreadsRunning = true;

	

	// put make-point/line/text procedure into env map after reading in the file
	eval_from_file(STARTUP_FILE);
	interp.interrupt();
	resetInterp = interp;
	guiThread = new std::thread(&Interpreter::Kernal, &interp);

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(startButton, 0, 0);
	layout->addWidget(stopButton, 0, 1);
	layout->addWidget(resetButton, 0, 2);
	layout->addWidget(interruptButton, 0, 3);
	layout->addWidget(inputWidget, 1, 0,1,4);//,1,1,Qt::AlignJustify);
	layout->addWidget(outputWidget, 2, 0,1,4);//,1,1,Qt::AlignJustify);
	setLayout(layout);

	connectSlots();
}

NotebookApp::~NotebookApp() {

	if (guiThread != nullptr) {
		MessageQueue<std::string> &inputMessage = MessageQueue<std::string>::getInstance();
		inputMessage.push("%exit");
		guiThread->join();
		delete guiThread;
	}
	guiThread = nullptr;

}

void NotebookApp::handle_PlotScript(QString input)
{
	
	emit(initializeScreen());
	std::string inputStr = input.toStdString();
	//qDebug() << inputStr;
	std::istringstream argumentStream(inputStr); 

	MessageQueue<std::string> &inputMessage = MessageQueue<std::string>::getInstance();
	MessageQueue<messageOut> &outputMessage = MessageQueue<messageOut>::getInstance();
	//MessageQueue<Expression> &interruptMessage = MessageQueue<Expression>::getInstance();

	messageOut output;

	if (inputStr != "%start" && guiThread != nullptr) {
		inputMessage.push(inputStr);
		qDebug() << "message pushed";
	}
	QString test = QString::fromStdString(inputStr);
	qDebug() << test;
	if (inputStr == "%start") {
		if (guiThread == nullptr) {
			guiThread = new std::thread(&Interpreter::Kernal, &interp);
			//twoThreadsRunning = true;
		}
	}
	else if (inputStr == "%stop" && guiThread != nullptr) {
		if (guiThread->joinable()) {
			guiThread->join();
			guiThread = nullptr;
			//twoThreadsRunning = false;
		}
	}
	else if (inputStr == "%reset") {
		if (guiThread != nullptr) {
			guiThread->join();
			guiThread = nullptr;
		}

		interp.clear();
		interp = resetInterp;
		guiThread = new std::thread(&Interpreter::Kernal, &interp);
		
	}
	else {
		if (guiThread != nullptr) {
			outputMessage.wait_and_pop(output);
			if (output.isExpression == true) {
				whichSignal(output.expMsg);
			}
			else {
				qDebug() << QString::fromStdString(output.error);
				error(output.error);
			}
		}
		else {
			if(inputStr != "%stop" && inputStr != "%reset")
				error("Error: interpreter kernel not running");
		}
	}
}

void NotebookApp::start() {
	QString input("%start");
	handle_PlotScript(input);
}

void NotebookApp::stop() {
	QString input("%stop");
	handle_PlotScript(input);
}

void NotebookApp::reset() {
	QString input("%reset");
	handle_PlotScript(input);
}

void NotebookApp::interrupt() {
	QString input("%interrupt");
	handle_PlotScript(input);
}

void NotebookApp::connectSlots()
{
	connect(inputWidget, SIGNAL(sendToPlotScript(QString)), this, SLOT(handle_PlotScript(QString)));
	
	//data ready to output 
	connect(this, SIGNAL(dataReady(QString)), outputWidget, SLOT(showResult(QString)));
	connect(this, SIGNAL(makePointReady(std::vector<double>)), outputWidget, SLOT(plotPoint(std::vector<double>)));
	connect(this, SIGNAL(makeLineReady(std::vector<double>)), outputWidget, SLOT(plotLine(std::vector<double>)));
	connect(this, SIGNAL(makeTextReady(QString, double, double,double,double)), outputWidget, SLOT(showText(QString, double, double,double,double)));
	connect(this, SIGNAL(initializeScreen()), outputWidget, SLOT(clearScreen()));
	connect(startButton, SIGNAL(released()), this, SLOT(start()));
	connect(stopButton, SIGNAL(released()), this, SLOT(stop()));
	connect(resetButton, SIGNAL(released()), this, SLOT(reset()));
	connect(interruptButton, SIGNAL(released()), this, SLOT(interrupt()));
}

// plotscript functions 

void NotebookApp::error(const std::string & err_str) {
	QString output = QString::fromStdString(err_str);
	emit dataReady(output);
}


void NotebookApp::eval_from_stream(std::istream & stream) {

	if (!interp.parseStream(stream)) {
		error("Error: invalid expression. Could not parse.");
	}
	else {
		try {
			Expression exp = interp.evaluate();
			whichSignal(exp);
			}
		catch (const SemanticError & ex) {
			error(ex.what());
		}
	}

}

void NotebookApp::eval_from_file(std::string filename) {

	std::ifstream ifs(filename);

	if (!ifs) {
		error("Could not open file for reading.");
	}

	qDebug() << "input file successfully implemented";

	eval_from_stream(ifs);
}

void NotebookApp::whichSignal(Expression exp)
{
	std::vector<Expression> tempParameters; // for graphics

	// if the input is make-point: takes 3 inputs (x,y,size) and emit those as signal 
	if (exp.get_property("object-name").second.head().asStringLiteral() == "point")
	{
		plotParameters.clear();
		size = exp.get_property("size").second.head().asNumber();
		for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e)
			tempParameters.push_back(*e);
		
		x = tempParameters[0].head().asNumber();
		y = tempParameters[1].head().asNumber();
		
		if (size < 0)
			error("Error in make-point: size must be non-negative");

		plotParameters.push_back(x);
		plotParameters.push_back(y);
		plotParameters.push_back(size);

		for (int i = 0; i < 3; i++)
			qDebug() << "final parameters: " << plotParameters[i];

		emit(makePointReady(plotParameters));

	}
	// if the input is make-line: takes 5 inputs (x1,y1,x2,y2,thickness) and emit those as signal
	else if (exp.get_property("object-name").second.head().asStringLiteral() == "line")
	{
		plotParameters.clear();
		thickness = exp.get_property("thickness").second.head().asNumber();

		if (thickness < 0)
			error("Error: in make-line: thickness must be positive");

		for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e)
		{
			Expression points = *e;
			for (auto a = points.tailConstBegin(); a != points.tailConstEnd(); ++a)
				tempParameters.push_back(*a);
		}
		qDebug() << "in line part of which signal function, size: " << tempParameters.size();

		for (int i = 0; i < 4; i++)
			plotParameters.push_back(tempParameters[i].head().asNumber());
		plotParameters.push_back(thickness);

		for (int i = 0; i < 5; i++)
			qDebug() <<"final parameters: " << plotParameters[i];
		emit(makeLineReady(plotParameters));
	}
	else if (exp.get_property("object-name").second.head().asStringLiteral() == "text")
	{
		QString text = QString::fromStdString(exp.head().asStringLiteral());


		//obtain the position of the text 
		Expression position = exp.get_property("position").second;

		if (position.get_property("object-name").second.head().asStringLiteral() != "point")
			error("Error in make-text: object-name of position property not a point");

		for (auto e = position.tailConstBegin(); e != position.tailConstEnd(); ++e)
			tempParameters.push_back(*e);

		x = tempParameters[0].head().asNumber();
		y = tempParameters[1].head().asNumber();

		Expression textScale = exp.get_property("text-scale").second;

		if (textScale.head().asNumber() < 0)
			scale = 1;
		else
			scale = textScale.head().asNumber();

		Expression textRotation = exp.get_property("text-rotation").second;

		if (!textRotation.isHeadNumber())
			phi = 0;
		else
			phi = textRotation.head().asNumber();

		QString test("this is a test");

		emit(makeTextReady(text, x, y,scale,phi));

	}
	//recursion for evaluating list of properties 
	else if (exp.head().asSymbol() == "list") 
	{
		for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e)
			whichSignal(*e);
	}
	else // regular expression 
	{
		if (exp.head().asSymbol() != "lambda")
		{
			std::stringstream stream;
			stream << exp;
			std::string output = stream.str();
			emit(dataReady(QString::fromStdString(output)));
		}
	}
	
}

