#include "catch.hpp"

#include "environment.hpp"
#include "semantic_error.hpp"

#include <cmath>

TEST_CASE( "Test default constructor", "[environment]" ) {

  Environment env;

  REQUIRE(env.is_known(Atom("pi")));
  REQUIRE(env.is_exp(Atom("pi")));

  REQUIRE(env.is_known(Atom("-pi")));
  REQUIRE(env.is_exp(Atom("-pi")));

  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));

  REQUIRE(env.is_known(Atom("e")));
  REQUIRE(env.is_exp(Atom("e")));

  REQUIRE(env.is_known(Atom("-e")));
  REQUIRE(env.is_exp(Atom("-e")));

  REQUIRE(env.is_known(Atom("I")));
  REQUIRE(env.is_exp(Atom("I")));

  REQUIRE(env.is_known(Atom("-I")));
  REQUIRE(env.is_exp(Atom("-I")));



  REQUIRE(env.is_proc(Atom("+")));
  REQUIRE(env.is_proc(Atom("-")));
  REQUIRE(env.is_proc(Atom("*")));
  REQUIRE(env.is_proc(Atom("/")));
  REQUIRE(!env.is_proc(Atom("op")));
  REQUIRE(env.is_proc(Atom("sqrt")));
  REQUIRE(env.is_proc(Atom("^")));
  REQUIRE(env.is_proc(Atom("ln")));
  REQUIRE(env.is_proc(Atom("sin")));
  REQUIRE(env.is_proc(Atom("cos")));
  REQUIRE(env.is_proc(Atom("tan")));
  REQUIRE(env.is_proc(Atom("real")));
  REQUIRE(env.is_proc(Atom("imag")));
  REQUIRE(env.is_proc(Atom("arg")));
  REQUIRE(env.is_proc(Atom("conj")));
  REQUIRE(env.is_proc(Atom("mag")));
  REQUIRE(!env.is_proc(Atom("not a procedure")));
}

TEST_CASE( "Test get expression", "[environment]" ) {
  Environment env;

  REQUIRE(env.get_exp(Atom("pi")) == Expression(std::atan2(0, -1)));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test add expression", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  REQUIRE(env.is_known(Atom("one")));
  REQUIRE(env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == a);

  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);
  REQUIRE(env.is_known(Atom("hi")));
  REQUIRE(env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == b);

  REQUIRE_THROWS_AS(env.add_exp(Atom(1.0), b), SemanticError);
}

TEST_CASE( "Test get built-in procedure", "[environment]" ) {
  Environment env;


  INFO("default procedure")
  Procedure p1 = env.get_proc(Atom("doesnotexist"));
  Procedure p2 = env.get_proc(Atom("alsodoesnotexist"));
  REQUIRE(p1 == p2);
  std::vector<Expression> args;
  REQUIRE(p1(args) == Expression());
  REQUIRE(p2(args) == Expression());

  INFO("trying add procedure")
  Procedure padd = env.get_proc(Atom("+"));
  args.emplace_back(1.0);
  args.emplace_back(2.0);
  REQUIRE(padd(args) == Expression(3.0));
  args.emplace_back(3.0);
  REQUIRE(padd(args) == Expression(6.0));
  
  std::complex<double> z(0, 9);
  args.emplace_back(z);
  std::complex<double> test;
  test = z + 6.0;
  REQUIRE(padd(args) == Expression(test));
  args.emplace_back(0.0); 
  REQUIRE(padd(args) == Expression(test));
  std::complex<double> I(0, 1);
  args.emplace_back(Expression(I));
  test = test + I;
  REQUIRE(padd(args) == Expression(test));
  args.emplace_back(env.get_exp((Atom("e"))));
  test = test + std::exp(1);
  REQUIRE(padd(args) == Expression(test));
  args.emplace_back(env.get_exp((Atom("pi"))));
  test = test + std::atan2(0, -1);
  REQUIRE(padd(args) == Expression(test));

  INFO("testing subtract procedure");
  std::vector<Expression> args_sub;
  Procedure subTot = env.get_proc(Atom("-"));
  args_sub.emplace_back(1.0);
  REQUIRE(subTot(args_sub) == Expression(-1.0));
  args_sub.emplace_back(1.0);
  REQUIRE(subTot(args_sub) == Expression(0));

  //empty the vector and test the procedure with complex numbers 
  args_sub.erase(args_sub.begin(),args_sub.begin() + 2);
  args_sub.emplace_back(I);
  REQUIRE(subTot(args_sub) == Expression(-I));
  args_sub.emplace_back(I);
  REQUIRE(subTot(args_sub) == Expression(I-I));
  args_sub.erase(args_sub.begin() + 1);
  args_sub.emplace_back(69.0);
  REQUIRE(subTot(args_sub) == Expression(-69.0 - I));

  INFO("testing the multiplication procedure");
  std::vector<Expression> args_mult;
  Procedure multTot = env.get_proc(Atom("*"));
  args_mult.emplace_back(1.0);
  args_mult.emplace_back(1.0);
  REQUIRE(multTot(args_mult) == Expression(1.0));
  args_mult.emplace_back(69.0);
  REQUIRE(multTot(args_mult) == Expression(69.0));
  args_mult.emplace_back(I + 1.0);
  REQUIRE(multTot(args_mult) == Expression(69.0 + 69.0*I));
  args_mult.erase(args_mult.begin(), args_mult.begin() + 4);
  args_mult.emplace_back(I);
  args_mult.emplace_back(I);
  REQUIRE(multTot(args_mult) == Expression(I*I));
  
  INFO("testing the division procedure");
  std::vector<Expression> args_div;
  Procedure divResult = env.get_proc(Atom("/"));
  args_div.emplace_back(6.0);
  REQUIRE(divResult(args_div) == Expression(1 / 6.0));
  args_div.emplace_back(9.0);
  REQUIRE(divResult(args_div) == Expression(6.0 / 9.0));
  args_div.erase(args_div.begin() + 1);
  args_div.emplace_back(I);
  REQUIRE(divResult(args_div) == Expression(6.0 / I));
  args_div.clear();
  args_div.emplace_back(I);
  args_div.emplace_back(I);
  REQUIRE(divResult(args_div) == Expression(I/I));

  INFO("testing exponent procedure");
  std::vector<Expression> args_exp;
  Procedure expResult = env.get_proc(Atom("^"));
  args_exp.emplace_back(2.0);
  args_exp.emplace_back(10.0);
  REQUIRE(expResult(args_exp) == Expression(1024.0));
  args_exp.clear();
  args_exp.emplace_back(env.get_exp(Atom("e")));
  args_exp.emplace_back(I * std::atan2(0,-1));
  std::complex<double> euler(-1, 0);
  REQUIRE(expResult(args_exp) == Expression(euler));

  INFO("testing square root procedure");
  std::vector<Expression> args_sqrt;
  Procedure sqrtResult = env.get_proc(Atom("sqrt"));
  args_sqrt.emplace_back(9.0);
  REQUIRE(sqrtResult(args_sqrt) == Expression(3.0));
  args_sqrt.clear();
  args_sqrt.emplace_back(-1);
  REQUIRE(sqrtResult(args_sqrt) == Expression(I));
  args_sqrt.clear();
  args_sqrt.emplace_back(I);
  REQUIRE(sqrtResult(args_sqrt) == Expression(std::sqrt(I)));

  INFO("testing natural log");
  std::vector<Expression> args_ln;
  Procedure lnResult = env.get_proc(Atom("ln"));
  args_ln.emplace_back(env.get_exp(Atom("e")));
  REQUIRE(lnResult(args_ln) == Expression(1.0));
  
  INFO("testing sin");
  std::vector<Expression> args_sin;
  Procedure sinResult = env.get_proc(Atom("sin"));
  args_sin.emplace_back(env.get_exp(Atom("pi")));
  REQUIRE(sinResult(args_sin) == Expression(0));

  INFO("testing cos");
  std::vector<Expression> args_cos;
  Procedure cosResult = env.get_proc(Atom("cos"));
  args_cos.emplace_back(env.get_exp(Atom("pi")));
  REQUIRE(cosResult(args_cos) == Expression(-1.0));

  INFO("testing tan");
  std::vector<Expression> args_tan;
  Procedure tanResult = env.get_proc(Atom("tan"));
  args_tan.emplace_back(env.get_exp(Atom("pi")));
  REQUIRE(tanResult(args_tan) == Expression(0));
  args_tan.clear();
  args_tan.emplace_back(std::atan2(0, -1)/2);
  REQUIRE(tanResult(args_tan) == Expression(std::tan(std::atan2(0, -1) / 2)));

  INFO("testing real,imag,arg,conj,mag");
  std::vector<Expression> argComplex;
  Procedure real = env.get_proc(Atom("real"));
  Procedure imag = env.get_proc(Atom("imag"));
  Procedure arg = env.get_proc(Atom("arg"));
  Procedure conj = env.get_proc(Atom("conj"));
  Procedure mag = env.get_proc(Atom("mag"));

  std::complex<double> Z(6, 9);
  argComplex.emplace_back(Z);
  REQUIRE(real(argComplex) == Expression(6.0));
  REQUIRE(imag(argComplex) == Expression(9.0));
  REQUIRE(arg(argComplex) == Expression(std::arg(Z)));
  REQUIRE(conj(argComplex) == Expression(std::conj(Z)));
  REQUIRE(mag(argComplex) == Expression(std::abs(Z)));

  INFO("testing list"); 
  std::vector<Expression> argList; 
  Procedure list = env.get_proc(Atom("list"));
  argList.emplace_back(Expression(1.0));
  argList.emplace_back(Expression(2.0));
  argList.emplace_back(Expression(3.0));
  Expression resultList = list(argList);
  REQUIRE(resultList.head().asSymbol() == "list");
  REQUIRE(env.is_list(argList) == false);

  Expression copyList = resultList;
  REQUIRE(resultList == copyList);

  INFO("testing first");
  std::vector<Expression> argsFirst;
  Procedure first = env.get_proc(Atom("first"));
  Expression listFirst = resultList;
  argsFirst.emplace_back(listFirst);
  Expression resultFirst = first(argsFirst);
  //REQUIRE(resultFirst.head().asSymbol() == "list");
  REQUIRE(resultFirst.tailConstBegin() == resultFirst.tailConstEnd());

  INFO("testing rest");
  std::vector<Expression> argsRest;
  Procedure rest = env.get_proc(Atom("rest"));
  Expression listRest = resultList;
  argsRest.emplace_back(listRest);
  Expression resultRest = rest(argsRest);
  REQUIRE(*resultRest.tailConstBegin() == argList[1]);
  REQUIRE(resultRest.listLength() == 2);

  INFO("testing length procedure");
  std::vector<Expression> argsLength;
  Procedure length = env.get_proc(Atom("length"));
  argsLength.emplace_back(resultList);
  Expression resultLength = length(argsLength);
  REQUIRE(resultLength == Expression(resultList.listLength()));

  INFO("testing append procedure");
  std::vector<Expression> argsAppend;
  Procedure append = env.get_proc(Atom("append"));
  argsAppend.emplace_back(resultList);
  argsAppend.emplace_back(1.0);
  Expression resultAppend = append(argsAppend);
  REQUIRE(resultAppend.listLength() == 4);

  INFO("testing join procedure");
  std::vector<Expression> argsJoin;
  Procedure join = env.get_proc(Atom("join"));
  argsJoin.emplace_back(resultList);
  argsJoin.emplace_back(resultList);
  Expression resultJoin = join(argsJoin);
  REQUIRE(resultJoin.listLength() == 6);

  INFO("testing range procedure");
  std::vector<Expression> argsRange;
  Procedure range = env.get_proc(Atom("range"));
  argsRange.emplace_back(0.0);
  argsRange.emplace_back(5.0);
  argsRange.emplace_back(1.0);
  Expression resultRange = range(argsRange);
  REQUIRE(resultRange.listLength() == 6);
}

TEST_CASE( "Test reset", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);

  env.reset();
  REQUIRE(!env.is_known(Atom("one")));
  REQUIRE(!env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == Expression());
  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test semeantic errors", "[environment]" ) {

  Environment env;

  {
    Expression exp(Atom("begin"));
    REQUIRE_THROWS_AS(exp.eval(env), SemanticError);
  }
  {
	  INFO("add errors");
	  std::vector<Expression> args;
	  Procedure padd = env.get_proc(Atom("+"));
	  args.emplace_back(1.0);
	  args.emplace_back(Atom("yo"));
	  REQUIRE_THROWS_AS(padd(args), SemanticError);
  }
  {
	  INFO("subtract errors");
	  std::vector<Expression> args;
	  Procedure sub = env.get_proc(Atom("-"));
	  args.emplace_back(Atom("yo im an error"));
	  REQUIRE_THROWS_AS(sub(args), SemanticError);
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(sub(args), SemanticError);
	  args.clear();
	  args.emplace_back(1.0);
	  args.emplace_back(2.0);
	  args.emplace_back(69.0);
	  REQUIRE_THROWS_AS(sub(args), SemanticError);
  }
  {
	  INFO("multiplication errors");
	  std::vector<Expression> args;
	  Procedure mul = env.get_proc(Atom("*"));
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(mul(args), SemanticError);
	  args.emplace_back(Atom("im an error and I will beat u this time"));
	  REQUIRE_THROWS_AS(mul(args), SemanticError);
  }
  {
	  INFO("division errors");
	  std::vector<Expression> args;
	  Procedure div = env.get_proc(Atom("/"));
	  args.emplace_back(1.0);
	  args.emplace_back(Atom("ok maybe I cant"));
	  args.emplace_back(Atom("ok maybe I cant"));
	  REQUIRE_THROWS_AS(div(args), SemanticError);
  }
  {
	  INFO("exponent errors");
	  std::vector<Expression> args;
	  Procedure exp = env.get_proc(Atom("^"));
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(exp(args), SemanticError);
	  args.emplace_back(Atom("u win but next time"));
	  REQUIRE_THROWS_AS(exp(args), SemanticError);
  }
  {
	  INFO("sqrt errors");
	  std::vector<Expression> args;
	  Procedure sqrt = env.get_proc(Atom("sqrt"));
	  args.emplace_back(Atom("tru"));
	  REQUIRE_THROWS_AS(sqrt(args), SemanticError);
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(sqrt(args), SemanticError);
  }
  {
	  INFO("natural log errors");
	  std::vector<Expression> args;
	  Procedure ln = env.get_proc(Atom("ln"));
	  args.emplace_back(Atom("tru"));
	  REQUIRE_THROWS_AS(ln(args), SemanticError);
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(ln(args), SemanticError);
	  args.clear();
	  args.emplace_back(-1.0);
	  REQUIRE_THROWS_AS(ln(args), SemanticError);
  }
  {
	  INFO("sin errors");
	  std::vector<Expression> args;
	  Procedure sin = env.get_proc(Atom("sin"));
	  args.emplace_back(Atom("tru"));
	  REQUIRE_THROWS_AS(sin(args), SemanticError);
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(sin(args), SemanticError);
  }
  {
	  INFO("cos errors");
	  std::vector<Expression> args;
	  Procedure cos = env.get_proc(Atom("cos"));
	  args.emplace_back(Atom("tru"));
	  REQUIRE_THROWS_AS(cos(args), SemanticError);
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(cos(args), SemanticError);
  }
  {
	  INFO("tan errors");
	  std::vector<Expression> args;
	  Procedure tan = env.get_proc(Atom("sin"));
	  args.emplace_back(Atom("tru"));
	  REQUIRE_THROWS_AS(tan(args), SemanticError);
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(tan(args), SemanticError);
  }
  {
	  INFO("real,imag,arg,conj,mag errors");
	  std::vector<Expression> args;
	  Procedure real = env.get_proc(Atom("real"));
	  Procedure imag = env.get_proc(Atom("imag"));
	  Procedure arg = env.get_proc(Atom("arg"));
	  Procedure conj = env.get_proc(Atom("conj"));
	  Procedure mag = env.get_proc(Atom("mag"));
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(real(args), SemanticError);
	  REQUIRE_THROWS_AS(imag(args), SemanticError);
	  REQUIRE_THROWS_AS(arg(args), SemanticError);
	  REQUIRE_THROWS_AS(conj(args), SemanticError);
	  REQUIRE_THROWS_AS(mag(args), SemanticError);
  }
  {
	  INFO("first procedure errors");
	  Procedure first = env.get_proc(Atom("first"));
	  std::vector<Expression> args;
	  Expression emptyList = list(args);
	  args.emplace_back(emptyList);
	  REQUIRE_THROWS_AS(first(args), SemanticError);
	  args.clear();
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(first(args), SemanticError);
	  args.emplace_back(2.0);
	  REQUIRE_THROWS_AS(first(args), SemanticError);

  }
  {
	  INFO("Rest procedure errors");
	  Procedure rest = env.get_proc(Atom("rest"));
	  Procedure list = env.get_proc(Atom("list"));
	  std::vector<Expression> args;
	  Expression emptyList = list(args);
	  args.emplace_back(emptyList);
	  REQUIRE_THROWS_AS(rest(args), SemanticError);
	  args.clear();
	  args.emplace_back(Atom("not a list"));
	  REQUIRE_THROWS_AS(rest(args), SemanticError);
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(rest(args), SemanticError);
  }
  {
	  INFO("length procedure errors");
	  Procedure length = env.get_proc(Atom("length"));
	  std::vector<Expression> args;
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(length(args), SemanticError);
	  args.emplace_back(2.0);
	  REQUIRE_THROWS_AS(length(args), SemanticError);
  }
  {
	  INFO("append procedure errors");
	  Procedure append = env.get_proc(Atom("append"));
	  std::vector<Expression> args;
	  args.emplace_back(1.0);
	  args.emplace_back(2.0);
	  REQUIRE_THROWS_AS(append(args), SemanticError);
	  args.emplace_back(3.0);
	  REQUIRE_THROWS_AS(append(args), SemanticError);
  }
  {
	  INFO("join procedure erros");
	  Procedure join = env.get_proc(Atom("join"));
	  Procedure list = env.get_proc(Atom("list"));
	  std::vector<Expression> args;
	  args.emplace_back(1.0);
	  REQUIRE_THROWS_AS(join(args), SemanticError);
	  args.emplace_back(2.0);
	  args.emplace_back(3.0);
	  Expression list1 = list(args);
	  args.clear();
	  args.emplace_back(list1);
	  args.emplace_back(Atom("not a list"));
	  REQUIRE_THROWS_AS(join(args), SemanticError);
  }
  {
	  INFO("range procedure erorrs");
	  Procedure range = env.get_proc(Atom("range"));
	  std::vector<Expression> args;
	  args.emplace_back(Atom("not a number"));
	  REQUIRE_THROWS_AS(range(args), SemanticError);
	  args.clear();
	  args.emplace_back(1.0);
	  args.emplace_back(Atom("yo"));
	  REQUIRE_THROWS_AS(range(args), SemanticError);
	  args.pop_back();
	  args.emplace_back(2.0);
	  args.emplace_back(Atom("yo"));
	  REQUIRE_THROWS_AS(range(args), SemanticError);
	  args.clear();
	  args.emplace_back(5.0);
	  args.emplace_back(1.0);
	  args.emplace_back(2.0); 
	  REQUIRE_THROWS_AS(range(args), SemanticError);
	  args.clear();
	  args.emplace_back(1.0);
	  args.emplace_back(5.0);
	  args.emplace_back(-1.0);
	  REQUIRE_THROWS_AS(range(args), SemanticError);

  }

}

