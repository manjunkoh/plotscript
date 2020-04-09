#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <thread>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){
  
  std::istringstream iss(program);
    
  Interpreter interp;
    
  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl; 
  }
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

TEST_CASE( "Test Interpreter parser with expected input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);
 
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == true);
}

TEST_CASE( "Test Interpreter parser with numerical literals", "[interpreter]" ) {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};
  
  for(auto program : programs){
    std::istringstream iss(program);
 
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == true);
  }

  {
    std::istringstream iss("(define x 1abc)");
    
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with truncated input", "[interpreter]" ) {

  {
    std::string program = "(f";
    std::istringstream iss(program);
  
    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
  
  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with extra input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with single non-keyword", "[interpreter]" ) {

  std::string program = "hello";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty input", "[interpreter]" ) {

  std::string program;
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty expression", "[interpreter]" ) {

  std::string program = "( )";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with bad number string", "[interpreter]" ) {

  std::string program = "(1abc)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with incorrect input. Regression Test", "[interpreter]" ) {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE("simple semantic error cases", "[interpreter]")
{
	Interpreter interp;

	std::string program = "(define 1 1)";
	std::istringstream iss(program);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program2 = "(define define 1)";
	std::istringstream iss2(program2);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);


}

TEST_CASE( "Test Interpreter result with literal expressions", "[interpreter]" ) {
  
  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(4.));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(atan2(0, -1)));
  }

  {
	  std::string program = "(e)";
	  Expression result = run(program);
	  REQUIRE(result == Expression(std::exp(1)));
  }

  {
	  std::complex<double> I(0, 1);
	  std::string program = "(I)";
	  Expression result = run(program);
	  REQUIRE(result == Expression(I));
  }

}

TEST_CASE( "Test Interpreter result with simple procedures (add)", "[interpreter]" ) {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }
  
  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(6.));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(21.));
  }
}
  
TEST_CASE( "Test Interpreter special forms: begin and define", "[interpreter]" ) {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }
  
  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(20.));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

TEST_CASE( "Test a medium-sized expression", "[interpreter]" ) {

  {
    std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
    Expression result = run(program);
    REQUIRE(result == Expression(43.));
  }
}

TEST_CASE( "Test arithmetic procedures", "[interpreter]" ) {

  {
    std::vector<std::string> programs = {"(+ 1 -2)",
					 "(+ -3 1 1)",
					 "(- 1)",
					 "(- 1 2)",
					 "(* 1 -1)",
					 "(* 1 1 -1)",
					 "(/ -1 1)",
					 "(/ 1 -1)"};

    for(auto s : programs){
      Expression result = run(s);
      REQUIRE(result == Expression(-1.));
    }
  }
}


TEST_CASE("Test lambda functions", "[interpreter]")
{
	Interpreter interp;
	
	std::string program = "(lambda (x) (* 2 x))"; 
	std::istringstream iss(program);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	interp.evaluate();


	std::vector<std::string> programs = { "(define f (lambda (x) (+ 2 x)))",
											"(f 2)"};
	int tempSize = programs.size();
	for (int i =0; i < tempSize; i++) 
	{
		std::istringstream iss(programs[i]);
		ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}

	std::vector<std::string> programs2 = { "(define a 1)", "(define x 100)",
		"(define f (lambda (x) (begin (define b 12) (+ a b x))))" };
	tempSize = programs2.size();
	for (int i = 0; i < tempSize; i++)
	{
		std::istringstream iss(programs2[i]);
		ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}


}
TEST_CASE("Semantic errors for set-property", "[interpreter]")
{
	Interpreter interp;

	std::string program = "(set-property \"number\" \"three\" (3))";
	std::istringstream iss(program);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	interp.evaluate();

	std::string program2 = "(set-property \"label\" \"foo\" (lambda (x) (* 3 (+ 2 x))))";
	std::istringstream iss2(program2);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	interp.evaluate();

	std::string program3 = "(set-property (+ 1 2) \"number\" \"three\")";
	std::istringstream iss3(program3);
	ok = interp.parseStream(iss3);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program4 = "(set-property (+ 1 2) \"number\" \"three\" 3)";
	std::istringstream iss4(program4);
	ok = interp.parseStream(iss4);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

}

TEST_CASE("test get property", "[interpreter]")
{
	Interpreter interp;

	std::string program = "(get-property \"number\" \"three\" (3))";
	std::istringstream iss(program);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program3 = "(get-property (+ 1 2) \"number\" \"three\")";
	std::istringstream iss3(program3);
	ok = interp.parseStream(iss3);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program4 = "(get-property (+ 1 2) \"number\" \"three\" 3)";
	std::istringstream iss4(program4);
	ok = interp.parseStream(iss4);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::vector<std::string> programs = { "(define a (+ 1 I))"," (define b (set-property \"note\" \"a complex number\" a))",
		"(get-property \"note\" b)" };
	int tempSize = programs.size();
	for (int i = 0; i < tempSize; i++)
	{

		std::istringstream iss(programs[i]);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}


}

TEST_CASE("Semantic errors for lambda", "[interpreter]")
{
	Interpreter interp;

	std::string program = "(lambda (x) (* 2 x) (+ 2 x))";
	std::istringstream iss(program);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program2 = "(lambda (5) (* 2 x))";
	std::istringstream iss2(program2);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program3 = "(lambda (+) (* 2 x))";
	std::istringstream iss3(program3);
	ok = interp.parseStream(iss3);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

}

TEST_CASE("test apply", "[interpreter]")
{
	std::string program = "(apply + (list 1 2 3 4))";
	
	Expression result = run(program);
	REQUIRE(result == Expression(10.));

	std::vector<std::string> programs = { "(define complexAsList (lambda (x) (list (real x) (imag x))))",
										"(apply complexAsList (list (+ 1 (* 3 I))))" };
	for (auto s : programs)
	{
		
		Interpreter interp;
		std::istringstream iss(s);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		//Expression result = run(s);
		//std::cout << result << std::endl;
		
	}
	//std::cout << interp.evaluate() << std::endl;
}

TEST_CASE("combine apply and lambda", "[interpreter]")
{
	std::vector<std::string> programs = { "(define complexAsList (lambda (x) (list (real x) (imag x))))",
											"(apply complexAsList (list (+ 1 (* 3 I))))" };
	Interpreter interp;
	int tempSize = programs.size();
	for (int i =0; i < tempSize; i++)
	{

		std::istringstream iss(programs[i]);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}

	std::vector<std::string> programs2 = { "(define linear (lambda (a b x) (+ (* a x) b)))",
											"(apply linear (list 3 4 5))" };
	tempSize = programs2.size();
	for (int i = 0; i < tempSize; i++)
	{
		std::istringstream iss(programs2[i]);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}
}

TEST_CASE("Semantic Errors for apply procedure", "[interpreter]")
{
	std::string program1 = "(apply + 3)";
	Interpreter interp;

	std::istringstream iss1(program1);

	bool ok = interp.parseStream(iss1);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program2 = "(apply (+ z I) (list 0))";
	std::istringstream iss2(program2);

	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program3 = "(apply / (list 1 2 4))";
	std::istringstream iss3(program3);

	ok = interp.parseStream(iss3);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program4 = "(apply + (list 1 2 3) (list 1 2 3))";
	std::istringstream iss4(program4);
	ok = interp.parseStream(iss4);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program5 = "(apply pi (list 1 2 4))";
	std::istringstream iss5(program5);

	ok = interp.parseStream(iss5);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

}

TEST_CASE("test map procedure", "[interpreter]")
{
	Interpreter interp;

	// test 1 
	std::string program = "(map / (list 1 2 4))";
	std::vector<Expression> args = { Expression(1.), Expression(0.5), Expression(0.25)};
	Expression resultList = list(args); 
	Expression result = run(program);
	REQUIRE(result == resultList);

	//test 2
	std::vector<std::string> programs = { "(define f (lambda (x) (sin x)))",
										"(map f (list (- pi) (/ (- pi) 2) 0 (/ pi 2) pi))" };

	int tempSize = programs.size();
	for (int i = 0; i < tempSize; i++)
	{
		std::istringstream iss(programs[i]);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}
}

TEST_CASE("Semantic Errors for map procedure", "[interpreter]")
{
	std::string program1 = "(map + 3)";
	Interpreter interp;

	std::istringstream iss1(program1);
	bool ok = interp.parseStream(iss1);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program2 = "(map 3 (list 1 2 3))";
	std::istringstream iss2(program2);
	ok = interp.parseStream(iss2);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program3 = "(map 3 (list 1 2 3) (list 1 2 3))";
	std::istringstream iss3(program3);
	ok = interp.parseStream(iss3);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::string program4 = "(map pi (list 1 2 3))";
	std::istringstream iss4(program4);
	ok = interp.parseStream(iss4);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

	std::vector<std::string> programs = { "(define addtwo (lambda (x y) (+ x y)))",
		"(map addtwo (list 1 2 3))" };
	int tempSize = programs.size();
	
	for (int i = 0; i < tempSize; i++)
	{
		std::istringstream iss3(programs[i]);
		bool ok = interp.parseStream(iss3);
		REQUIRE(ok == true);

		if (i == 1)
			REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
		else
			interp.evaluate();
	
	}
}

TEST_CASE("test discrete plot", "[interpreter]") {

	Interpreter interp;
	std::vector<std::string> programs = { "(define f (lambda (x) (list x (+ (* 2 x) 1))))",
		"(discrete-plot (map f (range -2 2 0.5)) (list (list \"title\" \"The Data\") (list \"abscissa-label\" \"X Label\") (list \"ordinate-label\" \"Y Label\") (list \"text-scale\" 1)))" };

	int tempSize = programs.size();
	for (int i = 0; i < tempSize; i++)
	{
		std::istringstream iss(programs[i]);
		bool ok = interp.parseStream(iss);
		REQUIRE(ok == true);
		interp.evaluate();
	}
}

TEST_CASE("test continuous plot", "[interpreter]") {

	Interpreter interp;
	
	std::string programs = "(begin (define f (lambda(x) (+ (* 2 x) 1))) (continuous-plot f (list - 2 2)))";

	std::istringstream iss(programs);
	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	interp.evaluate();
	
}

TEST_CASE("interpreter reset", "[interpreter]") {
	Interpreter interp;
	interp.clear();
}

TEST_CASE( "Test some semantically invalid expresions", "[interpreter]" ) {
  
  std::vector<std::string> programs = {"(@ none)", // so such procedure
				       "(- 1 1 2)", // too many arguments
				       "(define begin 1)", // redefine special form
				       "(define pi 3.14)"}; // redefine builtin symbol
    for(auto s : programs){
      Interpreter interp;

      std::istringstream iss(s);
      
      bool ok = interp.parseStream(iss);
      REQUIRE(ok == true);
      
      //REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
    }
}

TEST_CASE( "Test for exceptions from semantically incorrect input", "[interpreter]" ) {

  std::string input = R"(
(+ 1 a)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {

    std::string input = R"(
(define a 1 2)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test using number as procedure", "[interpreter]" ) {
    std::string input = R"(
(1 2 3)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("test thread Message queue", "[interpreter]") {

	//Interpreter *interp;
	//std::thread *testThread = new std::thread(&Interpreter::Kernal, interp);
	MessageQueue<std::string> &inputMessage = MessageQueue<std::string>::getInstance();
	MessageQueue<messageOut> &outputMessage = MessageQueue<messageOut>::getInstance();
	messageOut output;

	std::string dummy;

	std::string parseError = "(parse error";
	output.error = parseError;
	output.isExpression = false;
	inputMessage.push(parseError);
	REQUIRE(inputMessage.empty() == false);
	REQUIRE(inputMessage.try_pop(dummy) == true);
	REQUIRE(inputMessage.try_pop(dummy) == false);
	outputMessage.push(output);
	outputMessage.wait_and_pop(output);

	/*outputMessage.wait_and_pop(output);
	
	std::string semanticError = "(define f)";
	inputMessage.push(semanticError); s
	outputMessage.wait_and_pop(output);
	*/
	//testThread->join();



}
