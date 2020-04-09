#include "interpreter.hpp"

// system includes
#include <stdexcept>

// module includes
#include "token.hpp"
#include "parse.hpp"
#include "expression.hpp"
#include "environment.hpp"
#include "semantic_error.hpp"

bool Interpreter::parseStream(std::istream & expression) noexcept {

	TokenSequenceType tokens = tokenize(expression);


	ast = parse(tokens);

	return (ast != Expression());
};


Expression Interpreter::evaluate() {

	return ast.eval(env);
}

void Interpreter::clear() {
	env.reset();
}

void Interpreter::interrupt() {
	//MessageQueue<Expression> &interruptMessage = MessageQueue<Expression>::getInstance();
	//Expression interruptMsg;
	//interruptMessage.wait_and_pop(interruptMsg);
}


void Interpreter::Kernal()
{

	MessageQueue<std::string> &inputMessage = MessageQueue<std::string>::getInstance();
	MessageQueue<messageOut> &outputMessage = MessageQueue<messageOut>::getInstance();
	messageOut outputMsg;

	while (1)
	{
		std::string inputMsg;
		inputMessage.wait_and_pop(inputMsg);
		std::istringstream expression(inputMsg);


		if (inputMsg == "%stop" || inputMsg == "%exit" || inputMsg == "%reset")
		{
			break;
		}

		if (!parseStream(expression)) {
			outputMsg.isExpression = false;
			outputMsg.error = "Error: Invalid Expression. Could not parse.";
			outputMessage.push(outputMsg);

		}
		else
		{
			try {
				outputMsg.expMsg = evaluate();
				outputMsg.isExpression = true;
				outputMessage.push(outputMsg);
				continue;

			}
			catch (const SemanticError & ex)
			{
				outputMsg.isExpression = false;
				outputMsg.error = ex.what();
				outputMessage.push(outputMsg);

			}
		}
	}
}