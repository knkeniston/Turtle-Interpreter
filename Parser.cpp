#include "Parser.h"
#include <sstream>
#include <stdexcept>
#include <vector>
#include <set>

void Parser::match(Token tok) {
  if (tok != _lookahead)
    throw std::runtime_error("Unexpected Token!");
  _lookahead = _scanner.nextToken(_attribute, _lineno);
}

void Parser::parse() {
   _lookahead = _scanner.nextToken(_attribute, _lineno);
   try {
      program();  // start symbol method
   } catch (const std::exception& error) {
      throw std::runtime_error("Unexpected token!");
   }
}

void Parser::program() {
  stmt_list();
  match(Token::EOT);
}

void Parser::stmt_list() {
   do {
      Stmt *s = stmt();
      _AST.push_back(s);
   } while (_lookahead != Token::EOT);
}

Stmt *Parser::stmt() {
   if (_lookahead == Token::IDENT)
      return assignment();
   else if (_lookahead == Token::WHILE) 
      return while_stmt();
   else if (_lookahead == Token::IF)
      return if_stmt();
   else
      return action();
}

Stmt *Parser::assignment() {
   std::string name = _attribute.s;
   match(Token::IDENT);
   match(Token::ASSIGN);
   Expr *e = expr();
   return new AssignStmt(name, e);
}

std::vector<Stmt*> Parser::block() {
   std::vector<Stmt*> stList;
   stList.push_back((Stmt*)stmt());
   while (_lookahead != Token::ELSE &&
       _lookahead != Token::ELSIF &&
       _lookahead != Token::FI &&
       _lookahead != Token::OD) {
      stList.push_back((Stmt*)stmt());
   }
   return stList;
}

Stmt *Parser::while_stmt() {
   match(Token::WHILE);
   Expr *cond = bool_();
   match(Token::DO);
   std::vector<Stmt*> body = block();
   match(Token::OD);
   return new WhileStmt(cond, body);
}

Stmt *Parser::if_stmt() {
   match(Token::IF);
   Expr *cond = bool_();
   match(Token::THEN);
   std::vector<Stmt*> then_body = block();
   std::vector<std::vector<Stmt*>> elsif_stmt;
   std::vector<Expr*> elsif_cond;
   while(_lookahead == Token::ELSIF) {
      match(Token::ELSIF);
      Expr* cond = bool_();
      match(Token::THEN);
      std::vector<Stmt*> body = block();
      elsif_cond.push_back(cond);
      elsif_stmt.push_back(body);
   }
   std::vector<Stmt*> else_body;
   if (_lookahead == Token::ELSE) {
      match(Token::ELSE);
      else_body = block();
   }
   match(Token::FI);
   return new IfStmt(cond, then_body, elsif_cond, elsif_stmt, else_body);
}

Stmt *Parser::action() {
   switch(_lookahead) {
   case Token::HOME:    match(Token::HOME); return new HomeStmt();
   case Token::PENUP:   match(Token::PENUP); return new PenUpStmt();
   case Token::PENDOWN: match(Token::PENDOWN); return new PenDownStmt();
   case Token::FORWARD: match(Token::FORWARD); return new ForwardStmt(expr());
   case Token::LEFT:    match(Token::LEFT); return new LeftStmt(expr());
   case Token::RIGHT:   match(Token::RIGHT); return new RightStmt(expr());
   case Token::PUSHSTATE: 
      match(Token::PUSHSTATE); return new PushStateStmt();
   case Token::POPSTATE:
      match(Token::POPSTATE); return new PopStateStmt();
   default:
      throw std::runtime_error("Expecting turtle action statement!");
   }
}

Expr *Parser::factor() {
   switch(_lookahead) {
   case Token::PLUS:   match(Token::PLUS); return factor();
   case Token::MINUS:  match(Token::MINUS); return new NegExpr(factor());
   case Token::LPAREN: {
      match(Token::LPAREN);
      Expr *e = expr();
      match(Token::RPAREN);
      return e;
      }
   case Token::IDENT: {
      const std::string name = _attribute.s;
      match(Token::IDENT);
      return new VarExpr(name);
      }
   case Token::REAL: {
      const float val = _attribute.f;
      match(Token::REAL);
      return new ConstExpr(val);
      }
   default:
      throw std::runtime_error("Expecting factor!");
  }
}

Expr *Parser::expr() {
  Expr *e = term();
  while (_lookahead == Token::PLUS ||
	 _lookahead == Token::MINUS) {
    Token op = _lookahead;
    match(_lookahead);
    Expr *t = term();
    if (op == Token::PLUS){
      e = new AddExpr(e, t);
    }else{
      e = new SubExpr(e, t);
   }
  }
  return e;
}

Expr *Parser::term() {
  Expr *e = factor();
  while (_lookahead == Token::MULT ||
	 _lookahead == Token::DIV) {
    Token op = _lookahead;
    match(_lookahead);
    Expr *t = factor();
    if (op == Token::MULT) {
      e = new MulExpr(e, t);
    } else {
      e = new DivExpr(e, t);
   }
  }
  return e;
}

Expr *Parser::bool_() {
   Expr* e = bool_term();
   while(_lookahead == Token::OR) {
      match(Token::OR);
      e = new OrExpr(e, bool_term());
   }
   return e;
}

Expr *Parser::bool_term() {
   Expr* e = bool_factor();
   while (_lookahead == Token::AND) {
      match(Token::AND);
      e = new AndExpr(e, bool_factor());
   }
   return e;
}

Expr *Parser::bool_factor() {
   switch(_lookahead) {
   case (Token::NOT): {
      match(Token::NOT);
      return new NotExpr(bool_factor());
      }
   case (Token::RPAREN): {
      match(Token::RPAREN);
      return bool_();
      match(Token::LPAREN);
      }
   default:
      return cmp();
   }
}

Expr *Parser::cmp() {
   Expr* e1 = expr();
   Token next = _lookahead;
   match(_lookahead);
   Expr* e2 = expr();
   switch(next) {
   case (Token::EQ): return new EqExpr(e1,e2);
   case (Token::NE): return new NotEqExpr(e1,e2);
   case (Token::LT): return new LessThanExpr(e1,e2);
   case (Token::LE): return new LessEqExpr(e1,e2);
   case (Token::GT): return new GreaterThanExpr(e1,e2);
   case (Token::GE): return new GreaterEqExpr(e1,e2);
   default: throw std::runtime_error("Need comparison.");
   }
}
