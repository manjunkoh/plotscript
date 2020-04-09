#include "expression.hpp"

#include <sstream>
#include <list>

#include "environment.hpp"
#include "semantic_error.hpp"
#include <limits>

Expression::Expression() {}

Expression::Expression(const Atom & a) {

	m_head = a;
}


// recursive copy
Expression::Expression(const Expression & a) {

	m_head = a.m_head;
	for (auto e : a.m_tail) {
		m_tail.push_back(e);
	}
	m_property.clear();
	m_property = a.m_property;
}

Expression & Expression::operator=(const Expression & a) {

	// prevent self-assignment
	if (this != &a) {
		m_head = a.m_head;
		m_tail.clear();
		for (auto e : a.m_tail) {
			m_tail.push_back(e);
		}
		m_property.clear();
		m_property = a.m_property;
	}

	return *this;
}


Atom & Expression::head() {
	return m_head;
}

const Atom & Expression::head() const {
	return m_head;
}

bool Expression::isHeadNumber() const noexcept {
	return m_head.isNumber();
}

bool Expression::isHeadSymbol() const noexcept {
	return m_head.isSymbol();
}


void Expression::append(const Atom & a) {
	m_tail.emplace_back(a);
}

void Expression::addToTail(const Expression & a) {
	m_tail.emplace_back(a);
}

bool Expression::isListEmpty() const noexcept {
	return m_tail.empty();
}

int Expression::listLength() const noexcept {
	return m_tail.size();
}


Expression * Expression::tail() {
	Expression * ptr = nullptr;

	if (m_tail.size() > 0) {
		ptr = &m_tail.back();
	}

	return ptr;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept {
	return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept {
	return m_tail.cend();
}

Expression apply(const Atom & op, const std::vector<Expression> & args, const Environment & env) {

	// head must be a symbol
	if (!op.isSymbol()) {
		throw SemanticError("Error during evaluation: procedure name not symbol");
	}

	// must map to a proc
	if (!env.is_proc(op)) {
		throw SemanticError("Error during evaluation: symbol does not name a procedure");
	}

	//if(op.asSymbol() == "lambda")

	// map from symbol to proc
	Procedure proc = env.get_proc(op);

	// call proc with args
	return proc(args);
}

Expression Expression::handle_lookup(const Atom & head, const Environment & env) {
	if (head.isSymbol()) { // if symbol is in env return value
		if (env.is_exp(head)) {
			return env.get_exp(head);
		}
		else {
			throw SemanticError("Error during evaluation: unknown symbol");
		}
	}
	else if (head.isNumber() || head.isComplexNumber() || head.isStringLiteral()) {
		return Expression(head);
	}
	else {
		throw SemanticError("Error during evaluation: Invalid type in terminal expression");
	}
}

Expression Expression::handle_begin(Environment & env) {

	if (m_tail.size() == 0) {
		throw SemanticError("Error during evaluation: zero arguments to begin");
	}

	// evaluate each arg from tail, return the last
	Expression result;
	for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
		result = it->eval(env);
	}

	return result;
}

Expression Expression::handle_define(Environment & env) {


	// tail must have size 3 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to define");
	}

	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to define not symbol");
	}

	// but tail[0] must not be a special-form or procedure
	std::string s = m_tail[0].head().asSymbol();
	if ((s == "define") || (s == "begin")) {
		throw SemanticError("Error during evaluation: attempt to redefine a special-form");
	}

	if (env.is_proc(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}

	// eval tail[1]
	Expression result = m_tail[1].eval(env);

	if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}

	//and add to env
	env.add_exp(m_tail[0].head(), result);

	return result;
}

// Milestone 1 task 3: anonymous(Lambda) functions 
Expression Expression::handle_lambda(Environment & env)
{
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to lambda");
	}
	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to lambda not symbol");
	}

	Atom head("list");
	Expression lambda_list(head);
	if (env.is_proc(m_tail[0].head().asSymbol())) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}
	lambda_list.addToTail(Expression(m_tail[0].head()));
	for (auto a = m_tail[0].tailConstBegin(); a != m_tail[0].tailConstEnd(); ++a) {
		if (env.is_proc(a->head().asSymbol())) {
			throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
		}
		else
			lambda_list.addToTail(*a);
	}

	Atom headEx("lambda");
	Expression result(headEx);
	result.addToTail(lambda_list);
	//m_tail[1].eval(env);
	result.addToTail(m_tail[1]);

	return result;
}

Expression Expression::handle_apply(Environment & env)
{

	if (m_tail.size() != 2) {
		throw SemanticError("Error: during evaluation: invalid number of arguments to apply");
	}
	if (!m_tail[0].isHeadSymbol())
		throw SemanticError("Error in apply: first atgument not a procedure");

	if (m_tail[1].head().asSymbol() != "list")
		throw SemanticError("Error: in apply: second argument not a list");

	if (env.is_proc(m_tail[0].head()))
	{
		if (m_tail[0].m_tail.size() != 0)
			throw SemanticError("Error during evaluation: first argument to apply not a procedure");

		Expression post_apply_eval(m_tail[0].head());
		int tempSize = m_tail[1].m_tail.size();
		for (int i = 0; i < tempSize; ++i)
		{
			post_apply_eval.addToTail(m_tail[1].m_tail[i]);
		}
		Expression result(m_tail[1].head());
		result = post_apply_eval.eval(env);
		return result;

	}
	else
	{
		Expression grablambda = env.get_exp(m_tail[0].head());

		//if (grablambda.head().asSymbol() == "lambda") {
			Expression lambdaEval = m_tail[1].eval(env);
			Expression tree(m_tail[0].m_head.asSymbol());
			int tempSize = lambdaEval.m_tail.size();
			for (int i = 0; i < tempSize; i++)
			{
				tree.addToTail(lambdaEval.m_tail[i].eval(env));
			}
			Expression result(m_tail[1].head());
			result = tree.eval(env);
			return result;
		//}
	}


}
Expression Expression::handle_map(Environment & env)
{
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to map");
	}
	Expression arg = m_tail[1].eval(env);
	if (arg.head().asSymbol() != "list")
		throw SemanticError("Error in map: second argument not a list");

	if (env.is_proc(m_tail[0].head()))
	{
		if (m_tail[0].m_tail.size() != 0)
			throw SemanticError("Error during evaluation: first argument to map not a procedure");

		Expression result(Atom("list"));
		int tempSize = arg.m_tail.size();
		for (int i = 0; i < tempSize; ++i)
		{
			Expression post_apply_eval(m_tail[0].head());
			post_apply_eval.addToTail(arg.m_tail[i]);
			result.addToTail(post_apply_eval.eval((env)));
		}
		return result;
	}
	else if (env.is_exp(m_tail[0].head()))
	{
		Expression grablambda = env.get_exp(m_tail[0].head());

		if (grablambda.head().asSymbol() == "lambda")
		{
			Expression lambdaEval = m_tail[1].eval(env);
			Expression tree(m_tail[0].m_head.asSymbol());
			Expression result(Atom("list"));
			for (unsigned int i = 0; i != lambdaEval.m_tail.size(); i++)
			{
				tree.addToTail(lambdaEval.m_tail[i].eval(env));
				result.addToTail(tree.eval(env));
				tree.m_tail.clear();
			}
			return result;

		}
		else
			throw SemanticError("Error in map: expression not lambda");

	}
	else
		throw SemanticError("Error during evaluation: first argument to map not a procedure");
}

Expression Expression::handle_setProperty(Environment & env)
{
	if (m_tail.size() != 3)
		throw SemanticError("Error in set-property: invalid number of arguments");

	std::string tag;
	if (m_tail[0].head().isStringLiteral())
		tag = m_tail[0].head().asStringLiteral();
	else
		throw SemanticError("Error in set-property: first argument not a string");

	Expression property = m_tail[1].eval(env);

	Expression value = m_tail[2].eval(env);

	m_tail[2].m_property[tag] = property;
	value.m_property[tag] = property;
	//value.eval(env);

	//m_tail[2].eval(env);

	return value;
}

void Expression::setProperty(std::string tag, Expression property)
{
	this->m_property[tag] = property;
}

Expression Expression::handle_getProperty(Environment & env)
{
	if (m_tail.size() != 2)
		throw SemanticError("Error in get-property: invalid number of arguments");
	if (!m_tail[0].head().isStringLiteral())
		throw SemanticError("Error in get-property: input for tag not a string literal");

	Expression evalValue;
	evalValue = m_tail[1].eval(env);
	auto property = evalValue.m_property.find(m_tail[0].head().asStringLiteral());

	if (property == evalValue.m_property.end())
		return Expression();
	else
	{
		Expression result = property->second;
		return result;
	}
}

// helper function for discrete plots 
std::map<std::string, double> getValueProperties(Expression exp)
{

	std::map<double, double> coordinates;
	std::vector<Expression> tempParameters;
	std::map<std::string, double> properties;

	for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e)
	{
		
			Expression points = *e;
			for (auto a = points.tailConstBegin(); a != points.tailConstEnd(); ++a)
				tempParameters.push_back(*a);
	}

	double xMin, xMax, yMin, yMax;
	xMin = std::numeric_limits<double>::max();
	yMin = std::numeric_limits<double>::max();
	xMax = std::numeric_limits<double>::lowest();
	yMax = std::numeric_limits<double>::lowest();

	for (unsigned int i = 0; i != tempParameters.size(); i += 2)
	{
		if (xMax < tempParameters[i].head().asNumber())
			xMax = tempParameters[i].head().asNumber();

		if (yMax < tempParameters[i + 1].head().asNumber())
			yMax = tempParameters[i + 1].head().asNumber();

		if (xMin > tempParameters[i].head().asNumber())
			xMin = tempParameters[i].head().asNumber();
		if (yMin > tempParameters[i + 1].head().asNumber())
			yMin = tempParameters[i + 1].head().asNumber();
	}

	properties["max x"] = xMax;
	properties["min x"] = xMin;
	properties["max y"] = yMax;
	properties["min y"] = yMin;
	properties["x scale"] = 20 / (xMax - xMin);
	properties["y scale"] = 20 / (yMax - yMin);

	return properties;
}

Expression makePoint(double x, double y, double size)
{
	Expression point(Atom("list"));
	Atom p("point");
	p.setStringLiteral();
	point.setProperty("object-name", Expression(p));
	point.addToTail(Atom(x));
	point.addToTail(Atom(y));
	point.setProperty("size", Expression(Atom(size)));
	return point;
}

Expression makeLine(Expression p1, Expression p2, double thickness)
{
	Expression line(Atom("list"));
	Atom l("line");
	l.setStringLiteral();
	line.setProperty("object-name", Expression(l));
	line.addToTail(p1);
	line.addToTail(p2);
	line.setProperty("thickness", Expression(Atom(thickness)));
	return line;
}

Expression makeText(std::string text, double x, double y, double scale, double phi)
{
	Atom head(text);
	head.setStringLiteral();
	Expression txt(head);
	Atom t("text");
	t.setStringLiteral();
	txt.setProperty("object-name", Expression(t));
	Expression point = makePoint(x, y, 0.0);
	txt.setProperty("position", point);
	txt.setProperty("text-scale", Expression(Atom(scale)));
	txt.setProperty("text-rotation", Expression(Atom(phi)));
	return txt;
}

Expression createPlotLayout(std::map<std::string, double> values)
{
	// extract the max,min,and scale value
	double xscaler = values.find("x scale")->second;
	double yscaler = values.find("y scale")->second;

	// let the bounding box be size N x N and set max x y coordinates of the box
	double NxMax = xscaler * values.find("max x")->second;
	double NxMin = xscaler * values.find("min x")->second;
	double NyMax = yscaler * values.find("max y")->second;
	double NyMin = yscaler * values.find("min y")->second;

	bool xAxisExists = false, yAxisExists = false;

	if (NxMax > 0 && NxMin < 0)
		yAxisExists =  true;

	if (NyMax > 0 && NyMin < 0)
		xAxisExists = true;

	// draw the box from the top going CWthen axes if they are within box range 
	Expression boxLayout(Atom("list"));
	Expression p1 = makePoint(NxMin, -NyMax, 0.0);
	Expression p2 = makePoint(NxMax, -NyMax, 0.0);
	boxLayout.addToTail(makeLine(p1, p2, 0.0));

	p1 = makePoint(NxMax, -NyMax, 0.0);
	p2 = makePoint(NxMax, -NyMin, 0.0);
	boxLayout.addToTail(makeLine(p1, p2, 0.0));

	p1 = makePoint(NxMax, -NyMin, 0.0);
	p2 = makePoint(NxMin, -NyMin, 0.0);
	boxLayout.addToTail(makeLine(p1, p2, 0.0));

	p1 = makePoint(NxMin, -NyMin, 0.0);
	p2 = makePoint(NxMin, -NyMax, 0.0);
	boxLayout.addToTail(makeLine(p1, p2, 0.0));

	// draw the axes if they are within box range
	if (xAxisExists == true)
	{
		p1 = makePoint(NxMin, 0.0,0.0);
		p2 = makePoint(NxMax, 0.0, 0.0);
		boxLayout.addToTail(makeLine(p1, p2, 0.0));
	}
	if (yAxisExists == true)
	{
		p1 = makePoint(0.0,-NyMin, 0.0);
		p2 = makePoint(0.0, -NyMax, 0.0);
		boxLayout.addToTail(makeLine(p1, p2, 0.0));
	}

	return boxLayout;
} 

Expression createPlotLabels(std::map<std::string, double> values, Expression exp)
{
	// extract the max,min,and scale value
	double xscaler = values.find("x scale")->second;
	double yscaler = values.find("y scale")->second;
	double xMax = values.find("max x")->second;
	double xMin = values.find("min x")->second;
	double yMax = values.find("max y")->second;
	double yMin = values.find("min y")->second;

	// let the bounding box be size N x N and set max x y coordinates of the box
	double NxMax = xscaler * xMax;
	double NxMin = xscaler * xMin;
	double NyMax = yscaler * yMax;
	double NyMin = yscaler * yMin;

	// create labels using make-text title, x label, y label respectively
	Expression Labels(Atom("list"));
	double scale = 1;

	if (exp.head().asSymbol() != "no labels")
	{
		std::vector<Expression> temp;
		std::map<std::string, Expression> labels;

		for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e)
		{
			Expression points = *e;
			for (auto a = points.tailConstBegin(); a != points.tailConstEnd(); ++a)
				temp.push_back(*a);
		}

		for (unsigned int i = 0; i < temp.size(); i += 2)
		{
			labels[temp[i].head().asStringLiteral()] = temp[i + 1];
		}

		if (labels.find("text-scale") != labels.end())
			scale = labels.find("text-scale")->second.head().asNumber();
		else
			scale = 1;

		auto title = labels.find("title");
		if (title != labels.end())
			Labels.addToTail(makeText(title->second.head().asStringLiteral(), NxMin + 10, -NyMax - 3, scale, 0.0));

		auto xlabel = labels.find("abscissa-label");
		if (xlabel != labels.end())
			Labels.addToTail(makeText(xlabel->second.head().asStringLiteral(), NxMin + 10, -NyMin + 3, scale, 0.0));

		auto ylabel = labels.find("ordinate-label");
		if (ylabel != labels.end())
			Labels.addToTail(makeText(ylabel->second.head().asStringLiteral(), NxMin - 3, -NyMin - 10, scale, -std::atan2(0, -1) / 2));
	}
	// create labels for tic marks in roder of the vector below 
	std::vector<double> rangeValues = { yMax,yMin,xMin,xMax };
	std::vector<std::string> tickMark;
	std::stringstream stream;

	for (int i = 0; i < 4; i++)
	{
		stream << std::setprecision(2) << rangeValues[i];
		tickMark.push_back(stream.str());
		stream.str(std::string());
	}
	
	Labels.addToTail(makeText(tickMark[0], NxMin - 2, -NyMax, scale, 0.0));
	Labels.addToTail(makeText(tickMark[1], NxMin - 2, -NyMin, scale, 0.0));
	Labels.addToTail(makeText(tickMark[2], NxMin, -NyMin + 2, scale, 0.0));
	Labels.addToTail(makeText(tickMark[3], NxMax, -NyMin + 2, scale, 0.0));

	return Labels;

}

Expression Expression::handle_discretePlot(Environment & env)
{
	Expression rawData = m_tail[0].eval(env);
	Expression labelProperties = m_tail[1].eval(env);

	if (rawData.head().asSymbol() != "list")
		throw SemanticError("Error in discrete plot: first argument not a list");
	if (labelProperties.head().asSymbol() != "list")
		throw SemanticError("Error in discrete plot: second argument not a list");

	std::map<std::string, double> properties = getValueProperties(rawData);
	Expression plotLayout = createPlotLayout(properties);
	Expression plotLabels = createPlotLabels(properties, labelProperties);

	// extract the  ymax, ymin and scale value
	double xscaler = properties.find("x scale")->second;
	double yscaler = properties.find("y scale")->second;

	// let the bounding box be size N x N and set max x y coordinates of the box
	double NyMax = yscaler * properties.find("max y")->second;
	double NyMin = yscaler * properties.find("min y")->second;

	bool xAxisExists = false;

	if (NyMax > 0 && NyMin < 0)
		xAxisExists = true;

	std::vector<Expression> temp;
	std::map<double, double> points;
	Expression plot(Atom("list"));

	for (auto e = rawData.tailConstBegin(); e != rawData.tailConstEnd(); ++e)
	{
		Expression point = *e;
		for (auto a = point.tailConstBegin(); a != point.tailConstEnd(); ++a)
			temp.push_back(*a);
	}

	for (unsigned int i = 0; i < temp.size(); i += 2)
		points[xscaler*temp[i].head().asNumber()] = yscaler * temp[i + 1].head().asNumber();

	for (auto it = points.begin(); it != points.end(); it++)
	{
		Expression basePoint;
		if (xAxisExists == true)
			basePoint = makePoint(it->first, 0.0, 0);
		else if (xAxisExists == false && NyMin > 0)
			basePoint = makePoint(it->first, -NyMin, 0.0);
		else if (xAxisExists == false && NyMax < 0)
			basePoint = makePoint(it->first, -NyMax, 0.0);
		Expression actualPoint = makePoint(it->first, -it->second, 0.0);
		Expression line = makeLine(basePoint, actualPoint, 0.0);
		Expression lollipop = makePoint(it->first, -it->second, 0.5);
		plot.addToTail(line);
		plot.addToTail(lollipop);
	}
	
	// combine  all the parameters in to one list
	Expression discretePlot(Atom("list"));
	for (auto e = plotLayout.tailConstBegin(); e != plotLayout.tailConstEnd(); ++e)
		discretePlot.addToTail(*e);
	for (auto e = plotLabels.tailConstBegin(); e != plotLabels.tailConstEnd(); ++e)
		discretePlot.addToTail(*e);
	for (auto e = plot.tailConstBegin(); e != plot.tailConstEnd(); ++e)
		discretePlot.addToTail(*e);

	return discretePlot;
}

Expression Expression::handle_continousPlot(Environment & env)
{
	// extract the plot bounds from input
	Expression xleft = m_tail[1].m_tail[0];
	Expression xright = m_tail[1].m_tail[1];
	
	// compute the scalar to generate exactly 50 samples for plotting
	double xboundScaler = (xright.head().asNumber() - xleft.head().asNumber())/50;
	Expression xPoints(Atom("list"));
	Expression rawData(Atom("list"));

	for (double i = xleft.head().asNumber(); i <= xright.head().asNumber(); i += xboundScaler)
		xPoints.addToTail(Expression(Atom(i)));
	xPoints.addToTail(xright);

	for (auto e = xPoints.tailConstBegin(); e != xPoints.tailConstEnd(); ++e)
	{
		Expression currentData(Atom("list"));
		Expression yPoints(m_tail[0]);
		yPoints.addToTail(*e);

		currentData.addToTail(*e);
		currentData.addToTail(yPoints.eval(env));
		rawData.addToTail(currentData);
	}


	// now generate continuous plot

	std::map<std::string, double> properties = getValueProperties(rawData);
	Expression plotLayout = createPlotLayout(properties);
	Expression labelProperties(Atom("no labels"));

	if (m_tail.size() == 3)
		labelProperties = m_tail[2].eval(env);

	Expression plotLabels = createPlotLabels(properties, labelProperties);
	// extract the  ymax, ymin and scale value
	double xscaler = properties.find("x scale")->second;
	double yscaler = properties.find("y scale")->second;

	std::vector<Expression> temp;
	Expression plot(Atom("list"));

	for (auto e = rawData.tailConstBegin(); e != rawData.tailConstEnd(); ++e)
	{
		Expression point = *e;
		for (auto a = point.tailConstBegin(); a != point.tailConstEnd(); ++a)
			temp.push_back(*a);
	}

	int maxIter = 0;
	double v1x,v2x,v1y,v2y,V1, V2,theta,dotProduct; // compute the angle between two vectors for smoothing 
	double x1, x2, x3, y1, y2, y3;
	Expression point1;
	Expression smoothPoint;
	Expression point2;
	Expression point3;
	Expression line1;
	Expression line2;
	Expression smoothLine;

	double oneEightyOverPI = 180 / std::atan2(0, -1); // conversion
	for (unsigned int i = 0; i < temp.size() - 4; i += 2) {
		
		x1 = temp[i].head().asNumber();
		y1 = temp[i+1].head().asNumber();
		x2 = temp[i+2].head().asNumber();
		y2 = temp[i+3].head().asNumber();
		x3 = temp[i+4].head().asNumber();
		y3 = temp[i+5].head().asNumber();
		
		v1x = x2 - x1;
		v1y = y2 - y1;
		v2x = x3 - x2;
		v2y = y3 - y2;

		V1 = std::sqrt(v1x*v1x + v1y * v1y); //magnitude of vector 
		V2 = std::sqrt(v2x*v2x + v2y * v2y);
		dotProduct = v1x * v2x + v1y * v2y;
		
		theta = std::acos(dotProduct / (V1*V2)) * oneEightyOverPI; // compute theta in degrees

		point1 = makePoint(xscaler*x1, -yscaler * y1, 0.0);
		point2 = makePoint(xscaler*x2, -yscaler * y2, 0.0);
		point3 = makePoint(xscaler*x3, -yscaler * y3, 0.0);

		line1 = makeLine(point1, point2, 0.5);
		plot.addToTail(line1);


		if (theta >5 && theta < 175 && maxIter < 10) {
			
			maxIter++;
			Expression newyPoints(m_tail[0]);
			newyPoints.addToTail(Expression(Atom(temp[i].head().asNumber() + 0.5*(v1x - v2x))));
			Expression newy = newyPoints.eval(env);
			smoothPoint = makePoint(xscaler*(x1 + 0.5*(v1x - v2x)), -yscaler * (newy.head().asNumber()), 0.0);
			
			smoothLine = makeLine(smoothPoint, point2, 0.5);
			plot.addToTail(smoothLine);
			
		}

		line2 = makeLine(point2, point3, 0.5);
		plot.addToTail(line2);
	}


	/*for (unsigned int i = 0; i < temp.size()-2; i += 2)
	{
		
		Expression point1 = makePoint(xscaler*temp[i].head().asNumber(), -yscaler * temp[i + 1].head().asNumber(),0.0);
		Expression point2 = makePoint(xscaler*temp[i+2].head().asNumber(), -yscaler * temp[i + 3].head().asNumber(),0.0);
		Expression line = makeLine(point1, point2, 0.5);
		plot.addToTail(line);
	}*/


	// combine  all the parameters in to one list
	Expression continuousPlot(Atom("list"));
	for (auto e = plotLayout.tailConstBegin(); e != plotLayout.tailConstEnd(); ++e)
		continuousPlot.addToTail(*e);
	for (auto e = plotLabels.tailConstBegin(); e != plotLabels.tailConstEnd(); ++e)
		continuousPlot.addToTail(*e);
	for (auto e = plot.tailConstBegin(); e != plot.tailConstEnd(); ++e)
		continuousPlot.addToTail(*e);

	return continuousPlot;
}


std::pair<std::string, Expression> Expression::get_property(std::string const &property)
{
	//if (m_property.empty())
		//throw SemanticError("Error: there is no assigned property");

	auto targetProperty = m_property.find(property);
	if (targetProperty == m_property.end())
		return std::pair<std::string, Expression>("", Expression());
	else
		return std::pair<std::string, Expression>(targetProperty->first, targetProperty->second);
}


// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env)
{
	Environment copyEnv(env);

	if (m_tail.empty() && m_head.asSymbol() != "list") {
		return handle_lookup(m_head, env);
	}
	// handle begin special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "begin") {
		return handle_begin(env);
	}
	// handle define special-form
	else if (m_head.isSymbol() && m_head.asSymbol() == "define") {
		//defineUsed = true;
		return handle_define(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "lambda") {
		return handle_lambda(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "apply") {
		return handle_apply(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "map") {
		return handle_map(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "set-property") {
		return handle_setProperty(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "get-property") {
		return handle_getProperty(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "discrete-plot") {
		return handle_discretePlot(env);
	}
	else if (m_head.isSymbol() && m_head.asSymbol() == "continuous-plot") {
		return handle_continousPlot(env);
	}
	// else attempt to treat as procedure
	else
	{
		// temporary empy expression 
		Expression temp = env.get_exp(m_head);

		// evaluating expression after lambda 	
		if (env.is_known(m_head) && temp.m_head.asSymbol() == "lambda")
		{
			//unsigned in = temp.m_tail[0].listLength();
			//if (temp.m_tail[0].listLength() != 1 && in != m_tail.size())
			//	throw SemanticError("Error in Lambda: number of arguments don't match the number of parameters");

			int tempSize = m_tail.size();
			for (int input = 0; input < tempSize; ++input)
			{
				Expression post_Lamb_eval(Atom("define"));
				post_Lamb_eval.addToTail(temp.m_tail[0].m_tail[input]);
				post_Lamb_eval.addToTail(m_tail[input]);
				post_Lamb_eval.eval(copyEnv);
			}

			return temp.m_tail[1].eval(copyEnv);
		} // end of lambda evaluation 

		std::vector<Expression> results;
		for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
			results.push_back(it->eval(env));
		}

		return apply(m_head, results, env);
	}
}


std::ostream & operator<<(std::ostream & out, const Expression & exp) {

	Environment env;
	if (!exp.head().isNone())
		out << "(";

	if (exp.head().isStringLiteral())
		out << "\"";

	if (exp.head().asSymbol() != "lambda" && exp.head().asSymbol() != "list"
		&& exp.head().asSymbol() != "define" && exp.head().asSymbol() != "begin")
		out << exp.head();


	if (env.is_proc(exp.head()) && exp.head().asSymbol() != "lambda" && exp.head().asSymbol() != "list"
		&& exp.head().asSymbol() != "define" && exp.head().asSymbol() != "begin")
		out << " ";

	for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd();) {

		out << *e;
		++e;
		if (e != exp.tailConstEnd())
			out << " ";
	}

	if (exp.head().isStringLiteral())
		out << "\"";

	if (!exp.head().isNone())
		out << ")";

	return out;
}

bool Expression::operator==(const Expression & exp) const noexcept {

	bool result = (m_head == exp.m_head);

	result = result && (m_tail.size() == exp.m_tail.size());

	if (result) {
		for (auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
			(lefte != m_tail.end()) && (righte != exp.m_tail.end());
			++lefte, ++righte) {
			result = result && (*lefte == *righte);
		}
	}

	return result;
}

bool operator!=(const Expression & left, const Expression & right) noexcept {

	return !(left == right);
}
