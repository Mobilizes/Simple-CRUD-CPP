#include "../include/repository.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/sqlstring.h>
#include <cppconn/statement.h>

#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <stdexcept>

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
    std::cout << "There is no such index in table list when getting table data" << std::endl;
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
    std::cerr << "Error at getting all Mahasiswa associated with dosen : " << e.what() << std::endl;
    return std::nullopt;
  }
}

bool MySQLRepository::insert_table(int index)
{
  if (index <= 0 || index > table_list.size()) {
    std::cout << "There is no such index in table list when insertting data to table." << std::endl;
    return false;
  }

  std::string table_name = table_list[index - 1];

  try {
    MetadataWrapper metadata_wrapper = get_table_metadata(index);
    sql::ResultSetMetaData * metadata = metadata_wrapper.get_metadata();

    std::vector<std::string> column_names = get_column_names(metadata);

    std::string cancel_key = "\"" + generate_special_password(2);
    std::cout << "Type (" + cancel_key + ") to cancel the insertion." << std::endl;

    std::map<std::string, std::string> input_data;
    for (const std::string & column_name : column_names) {
      std::cout << "Enter " << column_name << ": ";
      std::cin >> input_data[column_name];

      if (input_data[column_name] == cancel_key) {
        std::cout << std::endl << "Insertion is cancelled!" << std::endl;
        return false;
      }
    }

    sql::SQLString question = "INSERT INTO " + table_name + " (";
    for (int i = 0; i < column_names.size(); ++i) {
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
        auto value = std::stoi(input_data[column_name]);
        prep_statement->setInt(i, value);
      } else if (column_type == "VARCHAR") {
        prep_statement->setString(i, input_data[column_name]);
      } else if (column_type == "BOOLEAN" or column_type == "TINYINT") {
        auto value = std::stoi(input_data[column_name]);
        if (value < 0 or value > 1) {
          std::cerr << "Boolean value must be 0 or 1!" << std::endl;
          return false;
        }

        prep_statement->setBoolean(i, value);
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
    std::cerr << "Error at insert a data to table : " << e.what() << std::endl;
    if (e.what() == std::string("stoi")) {
      std::cerr << "Column with number type is filled with non-number value!" << std::endl;
    }

    return false;
  }
}

bool MySQLRepository::update_table(int index)
{
  if (index <= 0 || index > table_list.size()) {
    std::cout << "There is no such index in table list when updating data in table." << std::endl;
    return false;
  }

  std::string table_name = table_list[index - 1];

  try {
    MetadataWrapper metadata_wrapper = get_table_metadata(index);
    sql::ResultSetMetaData * metadata = metadata_wrapper.get_metadata();

    std::vector<std::string> column_names = get_column_names(metadata);

    std::string cancel_key = "\"" + generate_special_password(2);
    std::cout << "Type (" + cancel_key + ") to cancel the insertion." << std::endl;

    std::string skip_key = "\"" + generate_special_password(1);
    std::cout << "Type (" + skip_key + ") to skip the column." << std::endl;

    std::map<std::string, std::string> input_data;
    for (int i = 0; i < column_names.size(); ++i) {
      std::string column_name = column_names[i];
      
      std::cout << "Enter " << column_name << ": ";
      std::cin >> input_data[column_name];

      if (input_data[column_name] == skip_key) {
        input_data.erase(column_name);

        if (column_name.size() >= 2 && column_name.substr(column_name.size() - 2, 2) == "ID") {
          std::cout << "You cannot skip ID!" << std::endl;
          --i;
        }

        continue;
      }

      if (input_data[column_name] == cancel_key) {
        std::cout << std::endl << "Update table is cancelled!" << std::endl;
        return false;
      }
    }

    sql::SQLString question = "UPDATE " + table_name + " SET ";

    std::map<std::string, std::string> primary_keys;
    bool first = true;
    for (const auto & [key, val] : input_data) {
      if (key.size() >= 2 && key.substr(key.size() - 2, 2) == "ID") {
        primary_keys[key] = val;
        continue;
      }

      if (first) {
        first = false;
      } else {
        question += ", ";
      }

      question += key + " = ?";
    }

    question += " WHERE ";

    for (const auto & [key, val] : primary_keys) {
      question += key + " = ?";

      if (key != primary_keys.rbegin()->first) {
        question += " AND ";
      }
    }

    std::unique_ptr<sql::PreparedStatement> prep_statement(conn->prepareStatement(question));

    int question_index = 1;
    for (int i = 1; i <= metadata->getColumnCount(); ++i) {
      auto column_type = metadata->getColumnTypeName(i);
      auto column_name = metadata->getColumnName(i);
      if (input_data[column_name].empty()) {
        continue;
      }

      if (column_name.length() >= 2 && column_name.substr(column_name.length() - 2, 2) == "ID") {
        continue;
      }

      if (column_type == "INT") {
        auto value = std::stoi(input_data[column_name]);
        prep_statement->setInt(question_index++, value);
      } else if (column_type == "VARCHAR") {
        prep_statement->setString(question_index++, input_data[column_name]);
      } else if (column_type == "BOOLEAN" or column_type == "TINYINT") {
        auto value = std::stoi(input_data[column_name]);
        if (value < 0 or value > 1) {
          std::cerr << "Boolean value must be 0 or 1!" << std::endl;
          return false;
        }

        prep_statement->setBoolean(question_index++, value);
      } else if (column_type == "DATE") {
        prep_statement->setDateTime(question_index++, input_data[column_name]);
      } else {
        std::cerr << "Column type not supported : " << column_type << std::endl;
        return false;
      }
    }

    for (const auto & [key, val] : primary_keys) {
      prep_statement->setInt(question_index++, std::stoi(val));
    }

    prep_statement->execute();

    return true;
  } catch (const std::exception & e) {
    std::cerr << "Error at update data in table : " << e.what() << std::endl;
    return false;
  }
}

bool MySQLRepository::delete_row_in_table(int index)
{
  if (index <= 0 || index > table_list.size()) {
    std::cout << "There is no such index in table list when updating data in table." << std::endl;
    return false;
  }

  std::string table_name = table_list[index - 1];

  try {
    MetadataWrapper metadata_wrapper = get_table_metadata(index);
    sql::ResultSetMetaData * metadata = metadata_wrapper.get_metadata();

    std::string cancel_key = "\"" + generate_special_password(2);
    std::cout << "Type (" + cancel_key + ") to cancel the deletion." << std::endl;

    std::map<std::string, std::string> primary_keys;
    for (int i = 1; i <= metadata->getColumnCount(); ++i) {
      auto column_name = metadata->getColumnName(i);

      if (column_name.length() >= 2 && column_name.substr(column_name.length() - 2, 2) == "ID") {
        std::cout << "Enter " << column_name << ": ";
        std::cin >> primary_keys[column_name];

        if (primary_keys[column_name] == cancel_key) {
          std::cout << std::endl << "Deletion is cancelled!" << std::endl;
          return false;
        }
      }
    }

    sql::SQLString question = "DELETE FROM " + table_name + " WHERE ";

    for (const auto & [key, val] : primary_keys) {
      question += key + " = ?";

      if (key != primary_keys.rbegin()->first) {
        question += " AND ";
      }
    }

    std::unique_ptr<sql::PreparedStatement> prep_statement(conn->prepareStatement(question));

    int question_index = 1;
    for (const auto & [key, val] : primary_keys) {
      prep_statement->setInt(question_index++, std::stoi(val));
    }

    prep_statement->execute();

  } catch (const std::exception & e) {
    std::cerr << "Error at delete a row in table : " << e.what() << std::endl;
    return false;
  }

  return true;
}

MySQLRepository::MetadataWrapper MySQLRepository::get_table_metadata(int index)
{
  if (index <= 0 || index > table_list.size()) {
    throw std::runtime_error("There is no such index in table list when getting table metadata.");
  }

  std::string table_name = table_list[index - 1];
  sql::SQLString query = "SELECT * FROM " + table_name + " LIMIT 1";

  std::unique_ptr<sql::Statement> statement(conn->createStatement());
  std::unique_ptr<sql::ResultSet> result(statement->executeQuery(query));

  return MetadataWrapper(std::move(statement), std::move(result));
}

std::vector<std::string> MySQLRepository::get_column_names(
  sql::ResultSetMetaData *& metadata)
{
  if (!metadata) {
    throw std::runtime_error("Metadata is null.");
  }

  std::vector<std::string> column_names;

  for (int i = 1; i <= metadata->getColumnCount(); ++i) {
    column_names.push_back(metadata->getColumnName(i));
  }

  return column_names;
}

std::vector<std::string> MySQLRepository::get_column_names(
  std::map<std::string, std::vector<std::string>> & table_data)
{
  std::vector<std::string> column_names;

  for (const auto & [key, val] : table_data) {
    column_names.push_back(key);
  }

  return column_names;
}

std::string MySQLRepository::generate_special_password(int len)
{
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

  std::string cancel_key;
  for (int i = 0; i < len; ++i) {
    cancel_key += char('a' + rand() % 26);
  }

  std::shuffle(cancel_key.begin(), cancel_key.end(), std::default_random_engine(seed));
  return cancel_key;
}

};  // namespace MySQL_Cpp
