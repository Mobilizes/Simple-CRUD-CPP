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

  bool read_all_tables();

  bool insert_table(int index);
  bool read_table(int index);
  bool update_table(int index);
  bool delete_table(int index);

  bool read_all_mahasiswa_taught_by_dosen(int dosen_index);

private:
  std::shared_ptr<MySQLRepository> repository;

  pprint::PrettyPrinter printer;
};

}  // namespace MySQL_Cpp

#endif  // CONTROLLER_HPP__
