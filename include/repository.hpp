#ifndef REPOSITORY_HPP__
#define REPOSITORY_HPP__

#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/sqlstring.h>
#include <cppconn/statement.h>
#include <mysql_connection.h>
#include <mysql_driver.h>

#include <map>
#include <memory>
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
  bool insert_table(int index);
  bool update_table(int index);
  bool delete_row_in_table(int index);

private:
  struct MetadataWrapper
  {
    std::unique_ptr<sql::Statement> statement;
    std::unique_ptr<sql::ResultSet> result;
    sql::ResultSetMetaData * metadata;

    MetadataWrapper(std::unique_ptr<sql::Statement> stmt, std::unique_ptr<sql::ResultSet> res)
    : statement(std::move(stmt)), result(std::move(res)), metadata(result->getMetaData())
    {
    }

    sql::ResultSetMetaData * get_metadata() const { return metadata; }
  };

  MetadataWrapper get_table_metadata(int index);

  std::vector<std::string> get_column_names(sql::ResultSetMetaData *& metadata);
  std::vector<std::string> get_column_names(
    std::map<std::string, std::vector<std::string>> & table_data);

  std::string generate_special_password(int len);

  sql::ConnectOptionsMap connection_properties;

  sql::Driver * driver = nullptr;
  sql::Connection * conn = nullptr;

  std::vector<std::string> table_list;
};

}  // namespace MySQL_Cpp

#endif  // REPOSITORY_HPP__
