#include "../include/repository.hpp"
#include <optional>

namespace MySQL_Cpp
{

MySQLRepository::MySQLRepository(
  std::string url, int port, std::string username, std::string password, std::string schema)
{
  connection_properties["hostName"] = url;
  connection_properties["port"] = port;
  connection_properties["userName"] = username;
  connection_properties["password"] = password;
  connection_properties["schema"] = schema;
  connection_properties["OPT_RECONNECT"] = true;

  driver = sql::mysql::get_driver_instance();
  conn = driver->connect(connection_properties);

  table_list = get_all_tables().value();
}

MySQLRepository::~MySQLRepository() { delete conn; }

std::optional<std::vector<std::string>> MySQLRepository::get_all_tables()
{
  std::vector<std::string> table_list;

  sql::SQLString question = "SHOW TABLES";

  try {
    std::unique_ptr<sql::Statement> statement(conn->createStatement());
    std::unique_ptr<sql::ResultSet> result(statement->executeQuery("SHOW TABLES"));

    while (result->next()) {
      table_list.push_back(result->getString(1));
    }

    return table_list;
  } catch (const std::exception & e) {
    std::cout << e.what() << std::endl;

    return std::nullopt;
  }
}

std::optional<std::map<std::string, std::vector<std::string>>> MySQLRepository::get_table(int index)
{
  if (index <= 0 || index > table_list.size()) {
    std::cout << "There is no such index in table list." << std::endl;
    return std::nullopt;
  }

  std::string table_name = table_list[index - 1];

  sql::SQLString question = "SELECT * FROM " + table_name;

  try {
    std::unique_ptr<sql::Statement> statement(conn->createStatement());
    std::unique_ptr<sql::ResultSet> result(statement->executeQuery(question));

    auto metadata = result->getMetaData();

    std::map<std::string, std::vector<std::string>> table_data;

    for (int i = 1; i <= metadata->getColumnCount(); ++i) {
      table_data[metadata->getColumnName(i)] = {};
    }

    while (result->next()) {
      for (int i = 1; i <= metadata->getColumnCount(); ++i) {
        table_data[metadata->getColumnName(i)].push_back(result->getString(i));
      }
    }

    return table_data;
  } catch (const std::exception & e) {
    std::cout << e.what() << std::endl;
    return std::nullopt;
  }
}

std::optional<std::map<std::string, std::vector<std::string>>>
MySQLRepository::get_all_mahasiswa_taught_by_dosen(int dosen_index)
{
  auto dosen_result = get_table(1);
  if (!dosen_result.has_value()) {
    return std::nullopt;
  }

  auto dosen_map = dosen_result.value();
  if (dosen_index <= 0 || dosen_index > dosen_map["Dosen_ID"].size()) {
    std::cout << "Dosen index is not found!" << std::endl;

    return std::nullopt;
  }

  std::string dosen_id = dosen_map["Dosen_ID"][dosen_index - 1];

  sql::SQLString question =
    "SELECT M.* FROM Mahasiswa M LEFT JOIN Mahasiswa_Kelas MK ON M.Mahasiswa_ID = MK.Mahasiswa_ID "
    "WHERE MK.Kelas_ID IN (SELECT Kelas_ID FROM Dosen_Kelas WHERE Dosen_ID = '" +
    dosen_id + "')";

  try {
    std::unique_ptr<sql::Statement> statement(conn->createStatement());
    std::unique_ptr<sql::ResultSet> result(statement->executeQuery(question));

    auto metadata = result->getMetaData();

    std::map<std::string, std::vector<std::string>> table_data;

    for (int i = 1; i <= metadata->getColumnCount(); ++i) {
      table_data[metadata->getColumnName(i)] = {};
    }

    while (result->next()) {
      for (int i = 1; i <= metadata->getColumnCount(); ++i) {
        table_data[metadata->getColumnName(i)].push_back(result->getString(i));
      }
    }

    return table_data;
  } catch (const std::exception & e) {
    std::cout << e.what() << std::endl;
    return std::nullopt;
  }
}

};  // namespace MySQL_Cpp
