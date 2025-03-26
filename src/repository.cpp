#include "../include/repository.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/sqlstring.h>
#include <cppconn/statement.h>

#include <memory>
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
    std::cerr << e.what() << std::endl;

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
    std::cerr << e.what() << std::endl;
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

    sql::ResultSetMetaData * metadata(result->getMetaData());

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
    std::cerr << e.what() << std::endl;
    return std::nullopt;
  }
}

bool MySQLRepository::insert_table(int index)
{
  if (index <= 0 || index > table_list.size()) {
    std::cout << "There is no such index in table list." << std::endl;
    return false;
  }

  std::string table_name = table_list[index - 1];

  try {
    auto metadata_result = get_table_metadata(index);
    if (!metadata_result.has_value()) {
      throw std::runtime_error("Failed to get column names.");
    }

    sql::ResultSetMetaData * metadata = metadata_result.value().getMetadata();

    auto column_result = get_column_names(metadata);
    if (!column_result.has_value()) {
      throw std::runtime_error("Failed to get column names.");
    }

    std::vector<std::string> column_names = column_result.value();
    std::map<std::string, std::string> input_data;
    for (const std::string & column_name : column_names) {
      std::cout << "Enter " << column_name << ": ";
      std::cin >> input_data[column_name];
    }

    sql::SQLString question = "INSERT INTO " + table_name + " (";
    for (int i = 0; i < column_names.size(); ++i) {
      std::cout << input_data[column_names[i]] << std::endl;
      question += column_names[i];
      if (i < column_names.size() - 1) {
        question += ", ";
      }
    }

    question += ") VALUES (";

    for (int i = 0; i < column_names.size(); ++i) {
      question += "?";
      if (i < column_names.size() - 1) {
        question += ", ";
      }
    }

    question += ")";

    std::unique_ptr<sql::PreparedStatement> prep_statement(conn->prepareStatement(question));

    for (int i = 1; i <= metadata->getColumnCount(); ++i) {
      auto column_type = metadata->getColumnTypeName(i);
      auto column_name = metadata->getColumnName(i);
      if (column_type == "INT") {
        prep_statement->setInt(i, std::stoi(input_data[column_name]));
      } else if (column_type == "VARCHAR") {
        prep_statement->setString(i, input_data[column_name]);
      } else if (column_type == "BOOLEAN" or column_type == "TINYINT") {
        prep_statement->setBoolean(i, std::stoi(input_data[column_name]));
      } else if (column_type == "DATE") {
        prep_statement->setDateTime(i, input_data[column_name]);
      } else {
        std::cerr << "Column type not supported : " << column_type << std::endl;
        return false;
      }
    }

    prep_statement->execute();

    return true;
  } catch (const std::exception & e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

std::optional<MySQLRepository::MetadataWrapper> MySQLRepository::get_table_metadata(int index)
{
  if (index <= 0 || index > table_list.size()) {
    std::cerr << "There is no such index in table list." << std::endl;
    return std::nullopt;
  }

  std::string table_name = table_list[index - 1];
  sql::SQLString query = "SELECT * FROM " + table_name + " LIMIT 1";

  try {
    std::unique_ptr<sql::Statement> statement(conn->createStatement());
    std::unique_ptr<sql::ResultSet> result(statement->executeQuery(query));

    return MetadataWrapper(std::move(statement), std::move(result));
  } catch (const std::exception & e) {
    std::cerr << e.what() << std::endl;
    return std::nullopt;
  }
}

std::optional<std::vector<std::string>> MySQLRepository::get_column_names(
  sql::ResultSetMetaData *& metadata)
{
  if (!metadata) {
    std::cerr << "Error: metadata is null." << std::endl;
    return std::nullopt;
  }

  std::vector<std::string> column_names;

  try {
    for (int i = 1; i <= metadata->getColumnCount(); ++i) {
      column_names.push_back(metadata->getColumnName(i));
    }
  } catch (const std::exception & e) {
    std::cerr << e.what() << std::endl;
    return std::nullopt;
  }

  if (column_names.empty()) {
    std::cerr << "Failed to get column names." << std::endl;
    return std::nullopt;
  }

  return column_names;
}

};  // namespace MySQL_Cpp
