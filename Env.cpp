#include "Env.h"

float Env::get(const std::string& name) {
  auto iter = _table.find(name);
  if (iter != _table.end())
    return iter->second;
  put(name, 0.0); // insert into table if it doesn't exist
  return 0.0;
}
