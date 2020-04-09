[33mcommit 82d7ddb2f105a21787d0d67b499dd8f7068d9d40[m[33m ([m[1;36mHEAD -> [m[1;32mmaster[m[33m, [m[1;33mtag: task3-1[m[33m)[m
Author: junniboi <junniboi@vt.edu>
Date:   Wed Aug 29 17:24:13 2018 -0400

    Worked on task 3 : added e, sqrt, exponents, natural log.

[1mdiff --git a/environment.cpp b/environment.cpp[m
[1mindex 22a7c8d..42f883d 100644[m
[1m--- a/environment.cpp[m
[1m+++ b/environment.cpp[m
[36m@@ -104,6 +104,72 @@[m [mExpression div(const std::vector<Expression> & args){[m
   return Expression(result);[m
 };[m
 [m
[32m+[m[32m//compute the exponent. 1st input is base and 2nd is the exponent[m[41m [m
[32m+[m[32mExpression exponent(const std::vector<Expression>& args) {[m
[32m+[m[41m	[m
[32m+[m	[32mdouble result = 0;[m
[32m+[m
[32m+[m	[32mif (nargs_equal(args, 2))[m
[32m+[m	[32m{[m
[32m+[m		[32mif ((args[0].isHeadNumber()) && (args[1].isHeadNumber()))[m
[32m+[m			[32mresult = pow(args[0].head().asNumber(), args[1].head().asNumber());[m
[32m+[m		[32melse[m
[32m+[m			[32mthrow SemanticError("Error in call to exponent: invalid argument.");[m
[32m+[m	[32m}[m
[32m+[m	[32melse[m
[32m+[m		[32mthrow SemanticError("Error in call to exponent: invalid number of arguments.");[m
[32m+[m
[32m+[m	[32mreturn Expression(result);[m
[32m+[m[32m};[m
[32m+[m
[32m+[m[32mExpression sqrt(const std::vector<Expression> & args) {[m
[32m+[m
[32m+[m	[32mdouble result = 0;[m
[32m+[m
[32m+[m	[32mif (nargs_equal(args, 1))[m[41m [m
[32m+[m	[32m{[m
[32m+[m		[32mif (args[0].isHeadNumber())[m
[32m+[m		[32m{[m
[32m+[m			[32mif (args[0].head().asNumber() >= 0)[m
[32m+[m				[32mresult = sqrt(args[0].head().asNumber());[m
[32m+[m
[32m+[m			[32melse[m
[32m+[m				[32mthrow SemanticError("Error in call to square root: input has to be positive.");[m
[32m+[m
[32m+[m		[32m}[m
[32m+[m		[32melse[m
[32m+[m			[32mthrow SemanticError("Error in call to sqaure root: number argument must be one.");[m
[32m+[m	[32m}[m
[32m+[m	[32melse[m
[32m+[m		[32mthrow SemanticError("Error in call to sqaure root: invalid argument.");[m
[32m+[m
[32m+[m	[32mreturn Expression(result);[m
[32m+[m[32m};[m
[32m+[m
[32m+[m[32m// computes the natural log[m
[32m+[m[32mExpression ln(const std::vector<Expression> & args) {[m
[32m+[m
[32m+[m	[32mdouble result = 0;[m
[32m+[m
[32m+[m	[32mif (nargs_equal(args, 1))[m
[32m+[m	[32m{[m
[32m+[m		[32mif (args[0].isHeadNumber())[m
[32m+[m		[32m{[m
[32m+[m			[32mif (args[0].head().asNumber() >= 0)[m
[32m+[m				[32mresult = log(args[0].head().asNumber());[m
[32m+[m
[32m+[m			[32melse[m
[32m+[m				[32mthrow SemanticError("Error in call to natural log: input has to be positive.");[m
[32m+[m		[32m}[m
[32m+[m		[32melse[m
[32m+[m			[32mthrow SemanticError("Error in call to natural log: number argument must be one.");[m
[32m+[m	[32m}[m
[32m+[m	[32melse[m
[32m+[m		[32mthrow SemanticError("Error in call to natural log: invalid argument.");[m
[32m+[m
[32m+[m	[32mreturn Expression(result);[m
[32m+[m[32m};[m
[32m+[m
 const double PI = std::atan2(0, -1);[m
 const double EXP = std::exp(1);[m
 [m
[36m@@ -196,4 +262,17 @@[m [mvoid Environment::reset(){[m
 [m
   // Procedure: div;[m
   envmap.emplace("/", EnvResult(ProcedureType, div)); [m
[32m+[m
[32m+[m[32m  //milestone 0[m
[32m+[m[32m  // task 3-1 Built-in value of e = exp(1)[m
[32m+[m[32m  envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));[m
[32m+[m
[32m+[m[32m  // task 3-2 procedure: sqrt[m
[32m+[m[32m  envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));[m
[32m+[m
[32m+[m[32m  // task 3-3 precedure: exponent[m
[32m+[m[32m  envmap.emplace("^", EnvResult(ProcedureType, exponent));[m
[32m+[m
[32m+[m[32m  // task 3-4 procedure: natural log[m[41m [m
[32m+[m[32m  envmap.emplace("ln", EnvResult(ProcedureType, ln));[m
 }[m
