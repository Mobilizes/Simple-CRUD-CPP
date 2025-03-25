#ifndef REPOSITORY_HPP__
#define REPOSITORY_HPP__

#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/sqlstring.h>
#include <cppconn/statement.h>
#include <mysql_connection.h>
#include <mysql_driver.h>

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace MySQL_Cpp
{

class MySQLRepository
{
public:
  MySQLRepository(
    std::string url, int port, std::string username, std::string password, std::string schema);
  ~MySQLRepository();

  std::optional<std::vector<std::string>> get_all_tables();
  std::optional<std::map<std::string, std::vector<std::string>>> get_table(int index);

  std::optional<std::map<std::string, std::vector<std::string>>> get_all_mahasiswa_taught_by_dosen(
    int dosen_index);

private:
  sql::ConnectOptionsMap connection_properties;

  sql::Driver * driver = nullptr;
  sql::Connection * conn = nullptr;

  std::vector<std::string> table_list;
};

}  // namespace MySQL_Cpp

#endif  // REPOSITORY_HPP__
