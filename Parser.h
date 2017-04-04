#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"
#include "AST.h"
#include <vector>
#include <list>

class Parser {
private:
  Scanner& _scanner;
  std::list<Stmt*> _AST;  // list of top-level AST's created
  Token _lookahead;
  Attribute _attribute;
  int _lineno;
public:
  Parser(Scanner& s) : _scanner{s}, _AST{} {}
  void parse(); // throws
  std::list<Stmt*>& syntaxTrees() {return _AST;}
private:
  void match(Token tok);
  void program();
  void stmt_list(); // executes (and deletes) each top-level statement
  Stmt *stmt();
  Stmt *assignment();
  std::vector<Stmt*> block();
  Stmt *while_stmt();
  Stmt *if_stmt();
  Stmt *else_part();
  Stmt *action();
  Expr *expr();
  Expr *term();
  Expr *factor();
  Expr *bool_();
  Expr *bool_term();
  Expr *bool_factor();
  Expr *cmp();
  Expr* cmp_op();
};  

#endif // PARSER_H
