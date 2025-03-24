#ifndef CONTROLLER_HPP__
#define CONTROLLER_HPP__

#include <memory>

#include "pprint.hpp"
#include "repository.hpp"

namespace MySQL_Cpp
{

class MySQLController
{
public:
  MySQLController(std::shared_ptr<MySQLRepository> repository);
  ~MySQLController() {}

  bool show_all_tables();
  bool show_table(int index);

private:
  std::shared_ptr<MySQLRepository> repository;

  pprint::PrettyPrinter printer;
};

}  // namespace MySQL_Cpp

#endif  // CONTROLLER_HPP__
