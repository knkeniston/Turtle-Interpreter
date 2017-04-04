#ifndef AST_H
#define AST_H

#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include "Env.h"

// Abstract base class for all expressions.
class Expr {
public:
  virtual ~Expr() {}
  virtual float eval(Env& env) const = 0;
};

// Abstract base class for all statements.
class Stmt {
public:
  virtual ~Stmt() {};
  virtual void execute(Env& env) = 0;
};


class AssignStmt : public Stmt {
protected:
  const std::string _name;  //l-value
  Expr *_expr; // r-value
public:
  AssignStmt(const std::string& n, Expr *e) : _name{n}, _expr{e} {}
  virtual void execute(Env& env) {
    env.put(_name, _expr->eval(env));
  }
   ~AssignStmt() {delete _expr;}
};

class HomeStmt : public Stmt {
public:
  virtual void execute(Env& env) {
    std::cout << "H" << std::endl;
  }
};

class PenUpStmt : public Stmt {
public:
  virtual void execute(Env& env) {
    std::cout << "U" << std::endl;
  }
};

class PenDownStmt : public Stmt {
public:
  virtual void execute(Env& env) {
    std::cout << "D" << std::endl;
  }
};

class PushStateStmt : public Stmt {
public:
  virtual void execute(Env& env) {
    std::cout << "[" << std::endl;
  }
};

class PopStateStmt : public Stmt {
public:
  virtual void execute(Env& env) {
    std::cout << "]" << std::endl;
  }
};

class ForwardStmt : public Stmt {
protected:
  Expr *_dist;
public:
  ForwardStmt(Expr *e) : _dist{e} {}
  virtual void execute(Env& env) {
    const float d = _dist->eval(env);
    std::cout << "M " << d << std::endl;
  }
};

class RightStmt : public Stmt {
protected:
  Expr *_angle;
public:
  RightStmt(Expr *e) : _angle{e} {}
  virtual void execute(Env& env) {
    const float a = _angle->eval(env);
    std::cout << "R " << -a << std::endl;
  }
};

class LeftStmt : public Stmt {
protected:
  Expr *_angle;
public:
  LeftStmt(Expr *e) : _angle{e} {}
  virtual void execute(Env& env) {
    const float a = _angle->eval(env);
    std::cout << "R " << a << std::endl;
  }
};

class WhileStmt : public Stmt {
protected:
   Expr* _cond;
   std::vector<Stmt*> _body;
public:
   WhileStmt(Expr* e, std::vector<Stmt*> s) : _cond{e}, _body{s} {}
   virtual void execute(Env& env) {
      while (_cond->eval(env) != 0) {
         for (int i = 0; i < (int)_body.size(); i++) {
            _body[i]->execute(env);
         }
      }
   }
};

class IfStmt: public Stmt {
protected:
   Expr* _cond;
   std::vector<Stmt*> _body;
   std::vector<Expr*> _elsif_cond;
   std::vector<std::vector<Stmt*>> _elsif_stmt;
   std::vector<Stmt*> _else;
public:
   IfStmt(Expr* e, std::vector<Stmt*> s, std::vector<Expr*> elcon, std::vector<std::vector<Stmt*>> elst, std::vector<Stmt*> el) : _cond{e}, _body{s}, _elsif_cond{elcon}, _elsif_stmt{elst}, _else{el} {}
   virtual void execute(Env& env) {
      bool exec = true;
      if (_cond->eval(env) != 0) {
         for (int i = 0; i < (int)_body.size(); i++) {
            _body[i]->execute(env);
         }
         exec=false;
      }
      for (int i = 0; i < (int)_elsif_cond.size(); i++) {
         if (_elsif_cond[i]->eval(env) != 0 && exec) {
            for (int j = 0; j < (int)_body.size(); j++) {
               _elsif_stmt[i][j]->execute(env);
            }
            exec=false;
         }
      }
      if (exec) {
         for (int i = 0; i < (int)_else.size(); i++) {
            _else[i]->execute(env);
         }
      }
   }
};

class VarExpr : public Expr {
protected:
  const std::string _name;
public:
  VarExpr(const std::string& n) : _name{n} {}
  virtual float eval(Env& env) const {
    return env.get(_name);
  }
};

class ConstExpr : public Expr {
protected:
  const float _val;
public:
  ConstExpr(float v) : _val{v} {}
  virtual float eval(Env& env) const {
    return _val;
  }
};

class UnaryExpr : public Expr {
protected:
  Expr *_expr;
public:
  UnaryExpr(Expr *e) : _expr{e} {}
};

class NegExpr : public UnaryExpr {
public:
  NegExpr(Expr *e) : UnaryExpr(e) {}
  virtual float eval(Env& env) const {
    return -_expr->eval(env);
  }
};

class BinaryExpr : public Expr {
protected:
  Expr *_left, *_right;
public:
  BinaryExpr(Expr *l, Expr *r) : _left{l}, _right{r} {}
};

class OrExpr : public BinaryExpr {
public:
  OrExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return (_left->eval(env) != 0) || (_right->eval(env) != 0);
  }
};

class AndExpr : public BinaryExpr {
public:
  AndExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return (_left->eval(env) != 0) && (_right->eval(env) != 0);
  }
};

class NotExpr : public Expr {
protected:
   Expr* _expr;
public:
  NotExpr(Expr *e) : _expr{e}{}
  virtual float eval(Env& env) const {
    return !_expr;
  }
};


class AddExpr : public BinaryExpr {
public:
  AddExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) + _right->eval(env);
  }
};

class SubExpr : public BinaryExpr {
public:
  SubExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) - _right->eval(env);
  }
};

class MulExpr : public BinaryExpr {
public:
  MulExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) * _right->eval(env);
  }
};

class DivExpr : public BinaryExpr {
public:
  DivExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) / _right->eval(env);
  }
};

class EqExpr : public BinaryExpr {
public:
  EqExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) == _right->eval(env);
  }
};

class NotEqExpr : public BinaryExpr {
public:
  NotEqExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) != _right->eval(env);
  }
};

class LessThanExpr : public BinaryExpr {
public:
  LessThanExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) < _right->eval(env);
  }
};

class LessEqExpr : public BinaryExpr {
public:
  LessEqExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) <= _right->eval(env);
  }
};

class GreaterThanExpr : public BinaryExpr {
public:
  GreaterThanExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) > _right->eval(env);
  }
};

class GreaterEqExpr : public BinaryExpr {
public:
  GreaterEqExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) >= _right->eval(env);
  }
};

#endif // AST_H
