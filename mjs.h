
#ifndef _mjs_h
#define _mjs_h

#include <string>
#include <vector>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctype.h>
#include <vector>
#include <type_traits>
#include <functional>
#include <cstring>
#include <map>

using namespace std;




namespace mjs {

// class VM {
// public:
//     VM();

//     Context context;
    
//     std::string run(std::string script);
// }

// class Context {
// public:
//     std::vector<Object> stack;
// }

// class Object {
// }

// class Variable : public Object {
//     std::string type;
//     std::string value;
// }

// class Function : public Object {

// }

// class AST {
// public:
//     AST();

//     Node parse(std::vector<Token> tokens);
// }

// class Node {
// public:
//     std::string id;
    
//     void run(Context & ctx) = 0;
// }

// class Statement : public Node {
//     void run(Context & ctx) = 0;
// }

// class FunctionDefinition : public Statement {
//     std::string name;
//     std::vector<std::string> parameters;
//     Block block;

//     void run(Context & ctx) = 0;
// }

// class Operator : public Statement {
//     void run(Context & ctx) = 0;
// }

// class VariableDefinition {

// }

// class Lexer {
// public:
//     std::vector<Token> Lex(std::string code);
// }

// clsss

} // namespace mjs

#endif