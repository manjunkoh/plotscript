#include "catch.hpp"

#include "expression.hpp"

TEST_CASE( "Test default expression", "[expression]" ) {

  Expression exp;

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
  REQUIRE(!exp.head().isComplexNumber());
}

TEST_CASE( "Test double expression", "[expression]" ) {

  Expression exp(6.023);

  REQUIRE(exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
  REQUIRE(!exp.head().isComplexNumber());
  
}


TEST_CASE( "Test symbol expression", "[expression]" ) {

  Expression exp(Atom("asymbol"));

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(exp.isHeadSymbol());
  REQUIRE(!exp.head().isComplexNumber());
}

TEST_CASE("Test complex number expression", "[expression]")
{
  std::complex<double> z;
  Expression exp(z);

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
  REQUIRE(exp.head().isComplexNumber());
}

/*TEST_CASE("Test value properties", "[expression]")
{
	Expression

	REQUIRE(!exp.isHeadNumber());
	REQUIRE(!exp.isHeadSymbol());
	REQUIRE(exp.head().isComplexNumber());
}*/

