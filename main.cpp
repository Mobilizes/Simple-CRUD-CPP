#include <cctype>
#include <exception>
#include <stdexcept>

#include <cppconn/connection.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

bool show_all_tables(sql::Connection * conn)
{
  sql::Statement * stmt;
  sql::ResultSet * res;

  try {
    stmt = conn->createStatement();
    res = stmt->executeQuery("SHOW TABLES");

    for (int i = 1; res->next(); ++i) {
      std::cout << i << ": " << res->getString(1) << std::endl;
    }

    delete res;
    delete stmt;
  } catch (std::exception & e) {
    std::cout << e.what() << std::endl;
    return false;
  }

  return true;
}

bool show_table(sql::Connection * conn, int index)
{
  return true;
}

void process(sql::Driver * driver, sql::Connection * conn)
{
  std::string input;

  std::cout << "What would you like to do?\n";
  std::cout << "sa. Show all tables\n";
  std::cout << "si. Show the content of table of index i\n";
  std::cout << "ex. Exit the program\n";

  std::cout << std::endl;
  std::cin >> input;
  std::cout << std::endl;

  std::string prefix = input.substr(0, 2);

  if (prefix == "sa") {
    if (show_all_tables(conn)) {
      std::cout << "Tables have been shown\n";
    }
  } else if (prefix == "ex") {
    throw std::runtime_error("Exiting the program...");
  } else if (prefix.front() == 's' && isdigit(prefix.back())) {
    int index = std::stoi(prefix.substr(1));

    if (show_table(conn, index)) {
      std::cout << "Table has been shown\n";
    }
  } else {
    std::cout << "Invalid input\n";
  }

  process(driver, conn);
}

int main()
{
  sql::Driver * driver = nullptr;
  sql::Connection * conn = nullptr;

  try {
    driver = sql::mysql::get_driver_instance();
    conn = driver->connect("tcp://127.0.0.1:3306", "admin_cpp", "abc123");
    conn->setSchema("departemen");

    process(driver, conn);
  } catch (std::exception & e) {
    std::cout << e.what() << std::endl;

    delete conn;
  }
}
