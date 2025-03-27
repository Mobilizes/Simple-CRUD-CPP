#include "../include/controller.hpp"

#include <memory>

namespace MySQL_Cpp
{

MySQLController::MySQLController(std::shared_ptr<MySQLRepository> repository)
{
  this->repository = repository;
}

bool MySQLController::read_all_tables()
{
  auto result = repository->get_all_tables();
  if (!result.has_value()) {
    std::cerr << "Show all tables' result is empty!" << std::endl;
    return false;
  }

  std::vector<std::string> table_list = result.value();

  std::cout << "List of tables" << std::endl;
  printer.print(table_list);
  std::cout << std::endl;

  return true;
}

bool MySQLController::insert_table(int index)
{
  if (!repository->insert_table(index)) {
    std::cerr << "Insert table failed!" << std::endl;
    return false;
  }
  return true;
}

bool MySQLController::read_table(int index)
{
  auto result = repository->get_table(index);
  if (!result.has_value()) {
    std::cerr << "Show table's result is empty!" << std::endl;
    return false;
  }

  std::map<std::string, std::vector<std::string>> table_data = result.value();

  printer.print(table_data);
  std::cout << std::endl;

  return true;
}

bool MySQLController::update_table(int index)
{
  if (!repository->update_table(index)) {
    std::cerr << "Update table failed!" << std::endl;
    return false;
  }

  return true;
}

bool MySQLController::read_all_mahasiswa_taught_by_dosen(int dosen_index)
{
  auto result = repository->get_all_mahasiswa_taught_by_dosen(dosen_index);
  if (!result.has_value()) {
    std::cout << "Failed to get all Mahasiswa taught by Dosen!" << std::endl;
    return false;
  }

  std::map<std::string, std::vector<std::string>> table_data = result.value();

  printer.print(table_data);
  std::cout << std::endl;

  return true;
}

}  // namespace MySQL_Cpp
