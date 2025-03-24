#include <cctype>
#include <exception>
#include <memory>
#include <stdexcept>

#include "controller.hpp"

using namespace MySQL_Cpp;

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
