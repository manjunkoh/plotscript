#include "atom.hpp"

#include <sstream>
#include <cctype>
#include <cmath>
#include <limits>

Atom::Atom(): m_type(NoneKind) {}

Atom::Atom(double value){

  setNumber(value);
}

Atom::Atom(std::complex<double> value) {

	setComplexNumber(value);
}

Atom::Atom(const Token & token): Atom(){
  
  // is token a number?
  double temp;
  std::istringstream iss(token.asString());
  if(iss >> temp){
    // check for trailing characters if >> succeeds
    if(iss.rdbuf()->in_avail() == 0){
      setNumber(temp);
    }
  }
  else{ // else assume symbol
    // make sure does not start with number
    if(!std::isdigit(token.asString()[0])){
      setSymbol(token.asString());
    }
  }
}

Atom::Atom(const std::string & value): Atom() {
  
  setSymbol(value);
}

Atom::Atom(const Atom & x): Atom(){
  if(x.isNumber()){
    setNumber(x.numberValue);
  }
  else if(x.isSymbol()){
    setSymbol(x.stringValue);
  }
  else if (x.isStringLiteral()) {
	  setString(x.stringValue);
  }
  else if (x.isComplexNumber()) {
	  setComplexNumber(x.complexValue);
  }

}

Atom & Atom::operator=(const Atom & x){

  if(this != &x){
	  if (x.m_type == NoneKind) {
		  m_type = NoneKind;
	  }
	  else if (x.m_type == NumberKind) {
		  setNumber(x.numberValue);
	  }
	  else if (x.m_type == SymbolKind) {
		  setSymbol(x.stringValue);
	  }
	  else if (x.m_type == ComplexKind) {
		  setComplexNumber(x.complexValue);
	  }
	  else if (x.m_type == StringKind)
	  {
		  m_type = StringKind;
		  new (&stringValue) std::string(x.stringValue);
	  }
  }
  return *this;
}
  
Atom::~Atom(){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind || m_type == StringKind){
    stringValue.~basic_string();
  }
}

bool Atom::isNone() const noexcept{
  return m_type == NoneKind;
}

bool Atom::isNumber() const noexcept{
  return m_type == NumberKind;
}

bool Atom::isComplexNumber() const noexcept {
	return m_type == ComplexKind;
}

bool Atom::isSymbol() const noexcept{
  return m_type == SymbolKind;
}  

bool Atom::isStringLiteral() const noexcept {
	return m_type == StringKind;
}


void Atom::setNumber(double value){

  m_type = NumberKind;
  numberValue = value;
}

void Atom::setComplexNumber(std::complex<double> value) {

	m_type = ComplexKind;
	complexValue = value;
}

void Atom::setSymbol(const std::string & value){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind){
    stringValue.~basic_string();
  }
    
  m_type = SymbolKind;

  // copy construct in place
  new (&stringValue) std::string(value);
}

void Atom::setString(const std::string & value) {

	// we need to ensure the destructor of the symbol string is called
	if (m_type == SymbolKind) {
		stringValue.~basic_string();
	}

	m_type = StringKind;

	// copy construct in place
	new (&stringValue) std::string(value);
}



void Atom::setStringLiteral() {

	m_type = StringKind;
}

double Atom::asNumber() const noexcept{

  return (m_type == NumberKind) ? numberValue : 0.0;  
}

std::complex<double> Atom::asComplexNumber() const noexcept {

	return (m_type == ComplexKind) ? complexValue : 0.0;
}

std::string Atom::asSymbol() const noexcept{

  std::string result;

  if(m_type == SymbolKind){
    result = stringValue;
  }

  return result;
}

std::string Atom::asStringLiteral() const noexcept {

	std::string result;

	if (m_type == StringKind)
		result = stringValue;

	return result;
}

bool Atom::operator==(const Atom & right) const noexcept{
  
  if(m_type != right.m_type) return false;

  switch(m_type){
  case NoneKind:
    if(right.m_type != NoneKind) return false;
    break;
  case NumberKind:
    {
      if(right.m_type != NumberKind) return false;
      double dleft = numberValue;
      double dright = right.numberValue;
      double diff = fabs(dleft - dright);
      if(std::isnan(diff) ||
	 (diff > std::numeric_limits<double>::epsilon())) return false;
    }
    break;
  case ComplexKind:
  {
	  if (right.m_type != ComplexKind) return false;
	  std::complex<double> cdleft = complexValue;
	  std::complex<double> cdright = right.complexValue;
	  double cdiff = abs(cdleft - cdright);
	  if (std::isnan(cdiff) || (cdiff > std::numeric_limits<double>::epsilon()))
		return false;
  }
  break;
  case SymbolKind:
    {
      if(right.m_type != SymbolKind) return false;

      return stringValue == right.stringValue;
    }
    break;
  default:
    return false;
  }

  return true;
}

bool operator!=(const Atom & left, const Atom & right) noexcept{
  
  return !(left == right);
}


std::ostream & operator<<(std::ostream & out, const Atom & a){

  if (a.isNone())
	out << "NONE";

  if(a.isNumber()){
    out << a.asNumber();
  }
  if(a.isSymbol()){
    out << a.asSymbol();
  }
  if (a.isComplexNumber()) {
	  std::complex<double> value = a.asComplexNumber();
	  out << real(value) << "," << imag(value);
  }
  if (a.isStringLiteral())
	  out << a.asStringLiteral();

  return out;
}
