#include "environment.hpp"
#include "expression.hpp"

#include <cassert>
#include <cmath>
//#include <complex>
#include <iostream>

#include "environment.hpp"
#include "semantic_error.hpp"

/***********************************************************************
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs) {
	return args.size() == nargs;
}

/***********************************************************************
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args) {
	args.size(); // make compiler happy we used this parameter
	return Expression();
};

// constants 
const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> I(0, 1);

Expression add(const std::vector<Expression> & args) {

	// check all aruments are numbers, while adding
	double realSum = 0, imagSum = 0;
	bool flag = false;
	for (auto & a : args) {

		if (a.isHeadSymbol()) {
			throw SemanticError("Error in call to add, argument not a number");
		}
		else {

			if (a.isHeadNumber())
				realSum += a.head().asNumber();
			else if (a.head().isComplexNumber()) {
				flag = true;
				realSum += real(a.head().asComplexNumber());
				imagSum += imag(a.head().asComplexNumber());
			}
		}
	}
	if (flag == false)
		return Expression(realSum);

	std::complex<double> result(realSum, imagSum);

	return Expression(result);
};

Expression mul(const std::vector<Expression> & args) {

	// check all aruments are numbers, while multiplying
	std::complex<double> result(1, 0);
	bool flag = false;

	if (nargs_equal(args, 1))
		throw SemanticError("Error in call to mul, invalid number of arguments");

	for (auto & a : args) {
		if (a.isHeadSymbol())
			throw SemanticError("Error in call to mul, argument not a number");
		else
		{
			if (a.head().isComplexNumber())
			{
				flag = true;
				result *= a.head().asComplexNumber();
			}
			else
				result *= a.head().asNumber();
		}

	}

	if (flag == false)
		return Expression(result.real());
	else
		return Expression(result);
};

Expression subneg(const std::vector<Expression> & args) {

	double realResult = 0, imagResult = 0;
	bool flag = true;

	// preconditions
	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			flag = false;
			realResult = -args[0].head().asNumber();
		}
		else if (args[0].head().isComplexNumber()) {
			realResult = -real(args[0].head().asComplexNumber());
			imagResult = -imag(args[0].head().asComplexNumber());
		}
		else {
			throw SemanticError("Error in call to negate: invalid argument.");
		}
	}
	else if (nargs_equal(args, 2)) {

		if ((args[0].isHeadSymbol()) || (args[1].isHeadSymbol())) {
			throw SemanticError("Error in call to subtraction: invalid argument.");
		}
		else {

			if (args[0].isHeadNumber() && args[1].head().isComplexNumber())
			{
				realResult = args[0].head().asNumber() - real(args[1].head().asComplexNumber());
				imagResult = -std::imag(args[1].head().asComplexNumber());
			}
			else if (args[1].isHeadNumber() && args[0].head().isComplexNumber())
			{
				realResult = std::real(args[0].head().asComplexNumber()) - args[1].head().asNumber();
				imagResult = -std::imag(args[0].head().asComplexNumber());
			}
			else if (args[0].head().isComplexNumber() && args[1].head().isComplexNumber())
			{
				realResult = std::real(args[0].head().asComplexNumber()) - real(args[1].head().asComplexNumber());
				imagResult = std::imag(args[0].head().asComplexNumber()) - imag(args[1].head().asComplexNumber());
			}
			else
			{
				flag = false;
				realResult = args[0].head().asNumber() - args[1].head().asNumber();
			}
		}
	}
	else {
		throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
	}

	std::complex<double> result(realResult, imagResult);

	if (!flag)
		return Expression(realResult);
	else
		return Expression(result);
};

Expression div(const std::vector<Expression> & args) {

	std::complex<double> result(0, 0);
	bool flag = true;
	
	if (nargs_equal(args,1))
	{

		if (args[0].isHeadNumber())
		{
			flag = false;
			result = 1.0 / args[0].head().asNumber();
		}
		else if (args[0].head().isComplexNumber())
			result = 1.0 / args[0].head().asComplexNumber();
		
	}
	else if (nargs_equal(args, 2))
	{
		if (args[0].isHeadNumber() && args[1].head().isComplexNumber())
		{
			result = args[0].head().asNumber() / args[1].head().asComplexNumber();
		}
		else if (args[1].isHeadNumber() && args[0].head().isComplexNumber())
		{
			result = args[0].head().asComplexNumber() / args[1].head().asNumber();
		}
		else if (args[0].head().isComplexNumber() && args[1].head().isComplexNumber())
		{
			result = args[0].head().asComplexNumber() / args[1].head().asComplexNumber();
		}
		else
		{
			flag = false;
			result = args[0].head().asNumber() / args[1].head().asNumber();
		}
	}
	else
		throw SemanticError("Error in call to div: incorrect num of args");

	if (!flag)
		return Expression(result.real());
	else
		return Expression(result);
};

//compute the exponent. 1st input is base and 2nd is the exponent 
Expression exponent(const std::vector<Expression>& args) {

	std::complex<double> result(0, 0);
	bool flag = true;

	if (nargs_equal(args, 2))
	{
		if ((args[0].isHeadNumber()) && (args[1].isHeadNumber()))
		{
			flag = false;
			result = std::pow(args[0].head().asNumber(), args[1].head().asNumber());
		}
		else if (args[1].isHeadNumber() && args[0].head().isComplexNumber())
		{
			result = std::pow(args[0].head().asComplexNumber(), args[1].head().asNumber());
		}
		else if (args[0].head().isComplexNumber() && args[1].head().isComplexNumber())
		{
			result = std::pow(args[0].head().asComplexNumber(), args[1].head().asComplexNumber());
		}
		else if (args[0].isHeadNumber() && args[1].head().isComplexNumber())
		{
			result = std::pow(args[0].head().asNumber(), args[1].head().asComplexNumber());
		}
		else
			throw SemanticError("Error in call to exponent: invalid argument.");
	}
	else
		throw SemanticError("Error in call to exponent: invalid number of arguments.");

	if (!flag)
		return Expression(result.real());
	else
		return Expression(result);
};

Expression sqrt(const std::vector<Expression> & args) {

	std::complex<double> result(0,0);
	bool flag = true;

	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadNumber())
		{
			if (args[0].head().asNumber() >= 0)
			{
				flag = false;
				result = std::sqrt(args[0].head().asNumber());
			}
			else
				result = std::sqrt(-args[0].head().asNumber()) * I;
		}
		else if (args[0].head().isComplexNumber())
			result = std::sqrt(args[0].head().asComplexNumber());
		else
			throw SemanticError("Error in call to sqaure root: invalid argument.");
	}
	else
		throw SemanticError("Error in call to sqaure root: number argument must be one.");

	if (!flag)
		return Expression(result.real());
	else
		return Expression(result);
};

// computes the natural log
Expression ln(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadNumber())
		{
			if (args[0].head().asNumber() >= 0)
				result = std::log(args[0].head().asNumber());
			else
				throw SemanticError("Error in call to natural log: input has to be positive.");
		}
		else
			throw SemanticError("Error in call to natural log: invalid argument.");
	}
	else
		throw SemanticError("Error in call to natural log: number of argument must be one.");

	return Expression(result);
};


// computes the sine
Expression sin(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadNumber())
			result = std::sin(args[0].head().asNumber());
		else
			throw SemanticError("Error in call to sine: invalid argument");
	}
	else
		throw SemanticError("Error in call to sine: invalid argument.");

	return Expression(result);
};

// computes the cosine
Expression cos(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadNumber())
			result = std::cos(args[0].head().asNumber());
		else
			throw SemanticError("Error in call to cosine: invalid argument.");
	}
	else
		throw SemanticError("Error in call to cosine: invalid argument.");

	return Expression(result);
};

// computes the tangent
Expression tan(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1))
	{
		if (args[0].isHeadNumber())
			result = std::tan(args[0].head().asNumber());
		else
			throw SemanticError("Error in call to tangent: invalid argument.");
	}
	else
		throw SemanticError("Error in call to tangent: invalid argument.");

	return Expression(result);
};

// returns real part of a complex number
Expression real(const std::vector<Expression> & args) {

	if (nargs_equal(args, 1) && args[0].head().isComplexNumber())
		return Expression(std::real(args[0].head().asComplexNumber()));
	else
		throw SemanticError("Error in call to real: argument not a complex number.");
};

// returns imaginary part of a complex number
Expression imaginary(const std::vector<Expression> & args) {

	if (nargs_equal(args, 1) && args[0].head().isComplexNumber())
		return Expression(std::imag(args[0].head().asComplexNumber()));
	else
		throw SemanticError("Error in call to real: argument not a complex number.");
};

// returns phase angle of a complex number in radians 
Expression arg(const std::vector<Expression> & args) {

	if (nargs_equal(args, 1) && args[0].head().isComplexNumber())
		return Expression(std::arg(args[0].head().asComplexNumber()));
	else
		throw SemanticError("Error in call to real: argument not a complex number.");
};

// returns the conjugate of a complex number 
Expression conj(const std::vector<Expression> & args) {

	if (nargs_equal(args, 1) && args[0].head().isComplexNumber())
		return Expression(std::conj(args[0].head().asComplexNumber()));
	else
		throw SemanticError("Error in call to real: argument not a complex number.");
};

// returns the magnitude of a complex number 
Expression mag(const std::vector<Expression> & args) {

	if (nargs_equal(args, 1) && args[0].head().isComplexNumber())
		return Expression(std::abs(args[0].head().asComplexNumber()));
	else
		throw SemanticError("Error in call to real: argument not a complex number.");
};

// Milestone 1 task 2
// returns list of the arguments 
Expression list(const std::vector<Expression> & args) {

	Atom list_head("list");
	Expression list(list_head);

	for (unsigned int i = 0; i < args.size(); i++) {
		list.addToTail(args[i]);
	}
	 return list;
};

// this functions returns the first expression of the list 
Expression first(const std::vector<Expression> &args) 
{
	Expression firstList(Atom("list"));
	if (nargs_equal(args, 1)) {
		if (args[0].tailConstBegin() != args[0].tailConstEnd()) {
			if (args[0].head().asSymbol() == "list")
				firstList = *(args[0].tailConstBegin());
			else
				throw SemanticError("Error in call to first: argument not a list");
		}
		else
			throw SemanticError("Error: argument to first empty list");
	}
	else
		throw SemanticError("Error: invalid number of arguments");
	return firstList;
}


Expression rest(const std::vector<Expression> &args) {

	if (args[0].head().asSymbol() != "list")
	{
		throw SemanticError("Error: argument to rest is not a list.");

	}
		if (nargs_equal(args, 1))
		{
			if (args[0].head().asSymbol() != "list")
				throw SemanticError("Error: argument to rest is not a list.");

			if (args[0].isListEmpty())
				throw SemanticError("Error: argument to rest is an empty list.");
			
			Expression rest(Atom("list"));
			for (auto  a = args[0].tailConstBegin() + 1; a != args[0].tailConstEnd(); ++a)
				rest.addToTail(*a);

			return rest;
			
		}
		else
			throw SemanticError("Error: more than one argument in call to rest.");

}

Expression length(const std::vector<Expression> &args)
{

		if (nargs_equal(args, 1))
		{
			if (args[0].head() != Expression(Atom("list")))
				throw SemanticError("Error: argument to length is not a list.");

			return Expression(Atom(args[0].listLength()));
		}
		else
			throw SemanticError("Error: more than one argument in call to length.");
}

Expression append(const std::vector<Expression> &args)
{
	if (nargs_equal(args, 2))
	{
		if (args[0].head() != Expression(Atom("list")))
			throw SemanticError("Error: first argument to append not a list.");

		Expression newList(Atom("list"));
		for (auto a = args[0].tailConstBegin(); a != args[0].tailConstEnd(); ++a)
			newList.addToTail(*a);
		newList.addToTail(args[1]);
		return newList;
	}
	else
		throw SemanticError("Error in append: invalid number of arguments");
}

Expression join(const std::vector<Expression> &args)
{
	if (args[0].head() != Expression(Atom("list")) || args[1].head() != Expression(Atom("list")))
		throw SemanticError("Error: first argument to join not a list.");
	else
	{
		Expression newList(Atom("list"));
		for (auto a = args[0].tailConstBegin(); a != args[0].tailConstEnd(); ++a)
			newList.addToTail(*a);
		for (auto a = args[1].tailConstBegin(); a != args[1].tailConstEnd(); ++a)
			newList.addToTail(*a);
		return newList;
	}
}

Expression range(const std::vector<Expression> &args)
{
	if (!args[0].isHeadNumber() || !args[1].isHeadNumber() || !args[2].isHeadNumber())
		throw SemanticError("Error: argument not a number.");
	else
	{
		if (args[0].head().asNumber() > args[1].head().asNumber())
			throw SemanticError("Error: begin greater than end in range.");
		else if (args[2].head().asNumber() <= 0)
			throw SemanticError("Error: negative or zero increment in range.");
		else
		{
			Expression rangeList(Atom("list"));
			for (double a = args[0].head().asNumber(); a <= args[1].head().asNumber(); a += args[2].head().asNumber())
			{
				rangeList.addToTail(Expression(a));
				//a += args[2].head().asNumber();
			}
			return rangeList;
		}
	}
}


Environment::Environment() {

	reset();
}

bool Environment::is_known(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const {

	Expression exp;

	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if ((result != envmap.end()) && (result->second.type == ExpressionType)) {
			exp = result->second.exp;
		}
	}

	return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp) {

	if (!sym.isSymbol()) {
		throw SemanticError("Attempt to add non-symbol to environment");
	}

	// overwrite the symbol map if lambda is used 
	if (envmap.find(sym.asSymbol()) != envmap.end()) {
		envmap[sym.asSymbol()] = EnvResult(ExpressionType, exp);
	}

	envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp));
}

bool Environment::is_proc(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == ProcedureType);
}

bool Environment::is_list(const std::vector<Expression> & exp) const
{
	return (Expression(exp[0].head()) == Expression(Atom("list")));
}

Procedure Environment::get_proc(const Atom & sym) const {

	//Procedure proc = default_proc;

	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if ((result != envmap.end()) && (result->second.type == ProcedureType)) {
			return result->second.proc;
		}
	}

	return default_proc;
}

/*void Environment::add_MessageQueue(MessageQueue<std::string> *msg) {

}*/

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset() {

	envmap.clear();

	// Built-In value of pi
	envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));
	envmap.emplace("-pi", EnvResult(ExpressionType, Expression(-PI)));

	// Procedure: add;
	envmap.emplace("+", EnvResult(ProcedureType, add));

	// Procedure: subneg;
	envmap.emplace("-", EnvResult(ProcedureType, subneg));

	// Procedure: mul;
	envmap.emplace("*", EnvResult(ProcedureType, mul));

	// Procedure: div;
	envmap.emplace("/", EnvResult(ProcedureType, div));

	//milestone 0
	// task 3-1 Built-in value of e = exp(1)
	envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));
	envmap.emplace("-e", EnvResult(ExpressionType, Expression(-EXP)));

	// task 3-2 procedure: sqrt
	envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));

	// task 3-3 precedure: exponent
	envmap.emplace("^", EnvResult(ProcedureType, exponent));

	// task 3-4 procedure: natural log 
	envmap.emplace("ln", EnvResult(ProcedureType, ln));

	// task 3-5~7 procedure: trig functions 
	envmap.emplace("sin", EnvResult(ProcedureType, sin));
	envmap.emplace("cos", EnvResult(ProcedureType, cos));
	envmap.emplace("tan", EnvResult(ProcedureType, tan));

	//task 4-1 create built-in expression I 
	envmap.emplace("I", EnvResult(ExpressionType, Expression(I)));
	envmap.emplace("-I", EnvResult(ExpressionType, Expression(-I)));

	//task 4-4  real, imaginary, arg, conj procedures implemented 

	envmap.emplace("real", EnvResult(ProcedureType, real));
	envmap.emplace("imag", EnvResult(ProcedureType, imaginary));
	envmap.emplace("arg",  EnvResult(ProcedureType, arg));
	envmap.emplace("conj", EnvResult(ProcedureType, conj));
	envmap.emplace("mag", EnvResult(ProcedureType, mag));

	//milestone 1 task 1 creating a list 
	envmap.emplace("list", EnvResult(ProcedureType, list));
	envmap.emplace("first", EnvResult(ProcedureType, first));
	envmap.emplace("rest", EnvResult(ProcedureType, rest));
	envmap.emplace("length", EnvResult(ProcedureType, length));
	envmap.emplace("append", EnvResult(ProcedureType, append));
	envmap.emplace("join", EnvResult(ProcedureType, join));
	envmap.emplace("range", EnvResult(ProcedureType, range));
	
}
