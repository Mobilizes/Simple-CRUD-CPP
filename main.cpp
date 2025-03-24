#include <cctype>
#include <exception>
#include <stdexcept>

#include <cppconn/connection.h>
#include <cppconn/sqlstring.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

class MySQLDriver
{
public:
  MySQLDriver(std::string url, int port, std::string username, std::string password,
    std::string schema)
  {
    connection_properties["hostName"] = url;
    connection_properties["port"] = port;
    connection_properties["userName"] = username;
    connection_properties["password"] = password;
    connection_properties["schema"] = schema;

    driver = sql::mysql::get_driver_instance();
    conn = driver->connect(connection_properties);

    table_list = get_all_tables();
  }

  ~MySQLDriver() { delete conn; }

  void show_all_tables()
  {
    std::cout << "List of tables" << std::endl;

    for (int i = 0; i < table_list.size(); ++i) {
      std::cout << i + 1 << ": " << table_list[i] << std::endl;
    }
  }

  std::vector<std::string> get_all_tables()
  {
    std::vector<std::string> table_list;

    sql::Statement * statement;
    sql::ResultSet * result;
    sql::SQLString question = "SHOW TABLES";

    try {
      statement = conn->createStatement();
      result = statement->executeQuery("SHOW TABLES");

      while (result->next()) {
        table_list.push_back(result->getString(1));
      }

      delete result;
      delete statement;

      return table_list;
    } catch (const std::exception & e) {
      std::cout << e.what() << std::endl;

      return {};
    }
  }

  void show_table(int index)
  {
    if (index <= 0 || index > table_list.size()) {
      std::cout << "There is no such index in table list." << std::endl;

      return;
    }

    std::string table_name = table_list[index - 1];

    sql::Statement * statement;
    sql::ResultSet * result;
    sql::SQLString question = "SELECT * FROM " + table_name;

    try {
      statement = conn->createStatement();
      result = statement->executeQuery(question);

      while (result->next()) {
        for (int i = 1; i <= result->rowsCount(); ++i) {
          std::cout << result->getString(i) << std::endl;
        }
      }

      delete result;
      delete statement;
    } catch (const std::exception & e) {
      std::cout << e.what() << std::endl;
    }
  }

private:
  sql::ConnectOptionsMap connection_properties;

  sql::Driver * driver = nullptr;
  sql::Connection * conn = nullptr;

  std::vector<std::string> table_list;
};

void process(MySQLDriver & mysql)
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
    mysql.show_all_tables();
    std::cout << "Tables have been shown" << std::endl;

  } else if (prefix.front() == 'r' && isdigit(prefix.back())) {
    int index = std::stoi(prefix.substr(1));

    mysql.show_table(index);
    std::cout << "Table has been shown" << std::endl;

  } else if (input == "clear") {
    std::cout << "\033[2J\033[2H" << std::endl;

  } else if (prefix == "ex") {
    throw std::runtime_error("Exiting the program...");

  } else {
    std::cout << "Invalid input" << std::endl;

  }

  std::cout << std::endl;

  process(mysql);
}

int main()
{
  std::string url = "127.0.0.1";
  int port = 3306;
  std::string username = "admin_cpp";
  std::string password = "abc123";
  std::string schema = "departemen";

  try {
    MySQLDriver mysql(url, port, username, password, schema);

    std::cout << "\033[2J\033[2H" << std::endl;
    process(mysql);
  } catch (const std::exception & e) {
    std::cout << e.what() << std::endl;
  }
}
