#ifndef ENV_H
#define ENV_H

#include <string>
#include <map>

class Env { // environment / symbol table
private:
  std::map<std::string,float> _table;
public:
  Env(): _table{} {}
  void put(const std::string& name, float val) {_table[name] = val;}
  float get(const std::string& name);
};

#endif
