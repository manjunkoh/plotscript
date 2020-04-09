#include "catch.hpp"

#include "atom.hpp"

TEST_CASE( "Test constructors", "[atom]" ) {

  {
    INFO("Default Constructor");
    Atom a;

    REQUIRE(a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(!a.isSymbol());
	REQUIRE(!a.isComplexNumber());
  }

  {
    INFO("Number Constructor");
    Atom a(1.0);

    REQUIRE(!a.isNone());
    REQUIRE(a.isNumber());
    REQUIRE(!a.isSymbol());
	REQUIRE(!a.isComplexNumber());
  }

  {
    INFO("Symbol Constructor");
    Atom a("hi");

    REQUIRE(!a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(a.isSymbol());
	REQUIRE(!a.isComplexNumber());
  }

  {
	  INFO("Complex Number Constructor");
	  std::complex<double> I(0, 1);
	  std::complex<double> Z;
	  Atom a(I);

	  REQUIRE(!a.isNone());
	  REQUIRE(!a.isNumber());
	  REQUIRE(!a.isSymbol());
	  REQUIRE(a.isComplexNumber());

	  Atom b(Z);
	  REQUIRE(b.isComplexNumber());
  }


  {
    INFO("Token Constructor");
    Token t("hi");
    Atom a(t);

    REQUIRE(!a.isNone());
    REQUIRE(!a.isNumber());
	REQUIRE(!a.isComplexNumber());
    REQUIRE(a.isSymbol());

  }

  {
    INFO("Copy Constructor");
    Atom a("hi");
    Atom b(1.0);
	std::complex<double> I(0, 1);
	Atom e(I);
    
    Atom c = a;
    REQUIRE(!a.isNone());
    REQUIRE(!c.isNumber());
	REQUIRE(!c.isComplexNumber());
    REQUIRE(c.isSymbol());

    Atom d = b;
    REQUIRE(!a.isNone());
    REQUIRE(d.isNumber());
    REQUIRE(!d.isSymbol());
	REQUIRE(!d.isComplexNumber());

	Atom f = e;
	REQUIRE(!f.isNone());
	REQUIRE(!f.isNumber());
	REQUIRE(!f.isSymbol());
	REQUIRE(f.isComplexNumber());
  }
}

TEST_CASE( "Test assignment", "[atom]" ) {

  {
    INFO("default to default");
    Atom a;
    Atom b;
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
	REQUIRE(!b.isComplexNumber());
  }

  {
    INFO("default to number");
    Atom a;
    Atom b(1.0);
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
	REQUIRE(!b.isComplexNumber());
  }

  {
	  INFO("default to symbol");
	  Atom a;
	  Atom b("hi");
	  b = a;
	  REQUIRE(b.isNone());
	  REQUIRE(!b.isNumber());
	  REQUIRE(!b.isSymbol());
	  REQUIRE(!b.isComplexNumber());
  }

  {
	INFO("default to complex number");
	Atom a;
	std::complex<double> z(6, 9);
	Atom b(z);
	b = a;
	REQUIRE(b.isNone());
	REQUIRE(!b.isNumber());
	REQUIRE(!b.isSymbol());
	REQUIRE(!b.isComplexNumber());
	
	

  }

  {
	  INFO("default to string literal");

	  Atom b("foo");
	  b.setStringLiteral();
	  REQUIRE(b.asStringLiteral() == "foo");
	  

	  REQUIRE(!b.isNone());
	  REQUIRE(!b.isNumber());
	  REQUIRE(!b.isSymbol());
	  REQUIRE(!b.isComplexNumber());
	  REQUIRE(b.isStringLiteral());


  }

  {
    INFO("number to default");
    Atom a(1.0);
    Atom b;
    b = a;
    REQUIRE(b.isNumber());
    REQUIRE(b.asNumber() == 1.0);
  }

  {
    INFO("number to number");
    Atom a(1.0);
    Atom b(2.0);
    b = a;
    REQUIRE(b.isNumber());
    REQUIRE(b.asNumber() == 1.0);
  }

  {
    INFO("number to symbol");
    Atom a(1.0);
    Atom b("hi");
    b = a;
	REQUIRE(b.isNumber());
	REQUIRE(b.asNumber() == 1.0);
  }

  {
	  INFO("number to complex number");
	  Atom a(1.0);
	  std::complex<double> z(6, 9);
	  Atom b(z);
	  b = a;
	  REQUIRE(b.isNumber());
	  REQUIRE(b.asNumber() == 1.0);
  }

  {
    INFO("symbol to default");
    Atom a("hi");
    Atom b;
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("symbol to number");
    Atom a("hi");
    Atom b(1.0);
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
	INFO("symbol to complex number");
	Atom a("hi");
	std::complex<double> z(6, 9);
	Atom b(z);
	b = a;
	REQUIRE(b.isSymbol());
	REQUIRE(b.asSymbol() == "hi");

  }

  {
    INFO("symbol to symbol");
    Atom a("hi");
    Atom b("bye");
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
	  INFO("complex number to default");
	  std::complex<double> z(6, 9);
	  Atom a(z);
	  Atom b;
	  b = a;

	  REQUIRE(b.isComplexNumber());
	  REQUIRE(b.asComplexNumber() == z);
  }

  {
	  INFO("Complex number to number");
	  std::complex<double> z(6, 9);
	  Atom a(z);
	  Atom b(1);
	  b = a;

	  REQUIRE(b.isComplexNumber());
	  REQUIRE(b.asComplexNumber() == z);
  }

  {
	  INFO("complex number to symbol");
	  std::complex<double> z(6, 9);
	  Atom a(z);
	  Atom b("test");
	  b = a;
	  REQUIRE(b.isComplexNumber());
	  REQUIRE(b.asComplexNumber() == z);
  }

  {
	INFO("complex number to complex number");
	std::complex<double> z1(6, 9);
	std::complex<double> z2(1, 1);
	Atom a(z1);
	Atom b(z2);
	b = a;
	REQUIRE(b.isComplexNumber());
	REQUIRE(b.asComplexNumber() == z1);

  }
}

TEST_CASE( "test comparison", "[atom]" ) {

  {
    INFO("compare default to default");
    Atom a;
    Atom b;
    REQUIRE(a == b);
  }

  {
    INFO("compare default to number");
    Atom a;
    Atom b(1.0);
    REQUIRE(a != b);
  }

  {
    INFO("compare default to symbol");
    Atom a;
    Atom b("hi");
    REQUIRE(a != b);
  }

  {
	INFO("compare default to complex number");
	Atom a;
	std::complex<double> z(6, 9);
	Atom b(z);
	REQUIRE(a != b);
  }

  {
    INFO("compare number to default");
    Atom a(1.0);
    Atom b;
    REQUIRE(a != b);
  }

  {
    INFO("compare number to number");
    Atom a(1.0);
    Atom b(1.0);
    Atom c(2.0);
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

  {
    INFO("compare number to symbol");
    Atom a(1.0);
    Atom b("hi");
    REQUIRE(a != b);
  }

  {
	INFO("compare number to complex number");
	Atom a(1.0);
	std::complex<double> z(6, 9);
	Atom b(z);
	REQUIRE(a != b);
  }

  {
    INFO("compare symbol to default");
    Atom a("hi");
    Atom b;
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to number");
    Atom a("hi");
    Atom b(1.0);
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to symbol");
    Atom a("hi");
    Atom b("hi");
    Atom c("bye");
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

  {
	INFO("compare symbol to complex number");
	Atom a("hi");
	std::complex<double> z(6, 9);
	Atom b(z);
	REQUIRE(a != b);
  }

  {
	INFO("compare complex number to default");
	std::complex<double> z(6, 9);
	Atom a(z);
	Atom b;
	REQUIRE(a != b);
  }

  {
	INFO("compare complex number to number");
	std::complex<double> z(6, 9);
	Atom a(z);
	Atom b(1.0);
	REQUIRE(a != b);
  }

  {
	INFO("compare complex number to symbol");
	std::complex<double> z(6, 9);
	Atom a(z);
	Atom b("hi");
	REQUIRE(a != b);
  }

  {
	  INFO("compare complex number to complex number");
	  std::complex<double> z1(6, 9);
	  std::complex<double> z2(6, 9);
	  std::complex<double> z3(1, 1);
	  Atom a(z1);
	  Atom b(z2);
	  Atom c(z3);
	  REQUIRE(a == b);
	  REQUIRE(a != c);
	  REQUIRE(b != c);
  }

}





