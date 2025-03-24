#include "include/controller.hpp"

#include <memory>

namespace MySQL_Cpp
{

MySQLController::MySQLController(std::shared_ptr<MySQLRepository> repository)
{
  this->repository = repository;
}

bool MySQLController::show_all_tables()
{
  auto result = repository->get_all_tables();
  if (!result.has_value()) {
    std::cout << "Show all tables' result is empty!";
    return false;
  }

  std::vector<std::string> table_list = result.value();

  std::cout << "List of tables" << std::endl;
  for (int i = 0; i < table_list.size(); ++i) {
    std::cout << i + 1 << ": " << table_list[i] << std::endl;
  }

  return true;
}

bool MySQLController::show_table(int index)
{
  auto result = repository->get_table(index);
  if (!result.has_value()) {
    std::cout << "Show table's result is empty!" << std::endl;
    return false;
  }

  std::map<std::string, std::vector<std::string>> table_data = result.value();

  printer.print(table_data);
  std::cout << '\n';

  return true;
}

}  // namespace MySQL_Cpp
