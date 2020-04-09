#include "parse.hpp"

#include <stack>

bool setHead(Expression &exp, const Token &token, bool &isStringLiteral) {

  Atom a(token);

  if (isStringLiteral)
	  a.setStringLiteral();
 
  exp.head() = a;

  return !a.isNone();
}

bool append(Expression *exp, const Token &token, bool &isStringKind) {

  Atom a(token);

  if (isStringKind)
	  a.setStringLiteral();

  exp->append(a);

  return !a.isNone();
}

Expression parse(const TokenSequenceType &tokens) noexcept {

  Expression ast;

  // cannot parse empty
  if (tokens.empty())
    return Expression();

  bool athead = false;
  bool stringLiteral = false;

  // stack tracks the last node created
  std::stack<Expression *> stack;

  std::size_t num_tokens_seen = 0;
  std::size_t num_quotes_seen = 0;

  for (auto &t : tokens) {

    if (t.type() == Token::OPEN) {
      athead = true;
    } 
	else if (t.type() == Token::CLOSE) {
      if (stack.empty() || num_quotes_seen%2 == 1) {
        return Expression();
      }
      stack.pop();

      if (stack.empty()) {
        num_tokens_seen += 1;
        break;
      }
    }
	else if (t.type() == Token::STRINGLITERAL)
	{
		stringLiteral = true;
		num_quotes_seen++;
		if (num_quotes_seen >= 2 && num_quotes_seen%2 == 0)
			stringLiteral = false;
	}
	else {

      if (athead) {
        if (stack.empty()) {
          if (!setHead(ast, t,stringLiteral)) { // is this a non kind
            return Expression();
          }
          stack.push(&ast);
        } 
		else {
          if (stack.empty()) {
            return Expression();
          }
          if (!append(stack.top(), t,stringLiteral)) {
            return Expression();
          }
          stack.push(stack.top()->tail());
        }
        athead = false;
      } 
	  else {
        if (stack.empty()) {
          return Expression();
        }

        if (!append(stack.top(), t,stringLiteral)) {
          return Expression();
        }
      }
    }
    num_tokens_seen += 1;
  }

  if (stack.empty() && (num_tokens_seen == tokens.size())) {
    return ast;
  }

  return Expression();
};
