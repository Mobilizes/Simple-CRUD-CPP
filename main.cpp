#include <cctype>
#include <exception>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>

#include <cppconn/connection.h>
#include <cppconn/sqlstring.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "pprint.hpp"

class MySQLRepository
{
public:
  MySQLRepository(
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

  ~MySQLRepository() { delete conn; }

  std::optional<std::vector<std::string>> get_all_tables()
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

  std::optional<std::map<std::string, std::vector<std::string>>> get_table(int index)
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

private:
  sql::ConnectOptionsMap connection_properties;

  sql::Driver * driver = nullptr;
  sql::Connection * conn = nullptr;

  std::vector<std::string> table_list;
};

class MySQLController
{
public:
  MySQLController(std::shared_ptr<MySQLRepository> repository) { this->repository = repository; }

  bool show_all_tables()
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

  bool show_table(int index)
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

private:
  std::shared_ptr<MySQLRepository> repository;

  pprint::PrettyPrinter printer;
};

void process(std::shared_ptr<MySQLController> controller)
{
  std::string input;

  std::cout << "What would you like to do?" << std::endl;
  std::cout << "ra. Show all tables" << std::endl;
  std::cout << "ri. Show the content of table of index i" << std::endl;
  std::cout << "ex. Exit the program" << std::endl;

  std::cout << std::endl;
  std::cin >> input;
  std::cout << std::endl;

  std::string prefix = input.substr(0, 2);

  if (prefix == "ra") {
    if (controller->show_all_tables()) {
      std::cout << "All tables been shown" << std::endl;
    }
  } else if (prefix.front() == 'r' && isdigit(prefix.back())) {
    int index = std::stoi(prefix.substr(1));

    if (controller->show_table(index)) {
      std::cout << "Table has been shown" << std::endl;
    }
  } else if (input == "clear") {
    std::cout << "\033[2J\033[2H" << std::endl;

  } else if (prefix == "ex") {
    throw std::runtime_error("Exiting the program...");

  } else {
    std::cout << "Invalid input" << std::endl;
  }

  std::cout << std::endl;

  process(controller);
}

int main()
{
  std::string url = "127.0.0.1";
  int port = 3306;
  std::string username = "admin_cpp";
  std::string password = "abc123";
  std::string schema = "departemen";

  try {
    std::shared_ptr<MySQLRepository> repository =
      std::make_shared<MySQLRepository>(url, port, username, password, schema);
    std::shared_ptr<MySQLController> controller = std::make_shared<MySQLController>(repository);

    std::cout << "\033[2J\033[2H" << std::endl;
    process(controller);
  } catch (const std::exception & e) {
    std::cout << e.what() << std::endl;
  }
}
