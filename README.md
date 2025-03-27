# Simple CRUD CPP
Only in Linux, Debian distribution

Prerequisite:  
`sudo apt install cmake libboost-dev -y`

1. Install MySQL-Connector-CPP  
`sudo apt install libmysqlcppconn-dev -y`

2. Import predefined MySQL database  
`sudo mysql < departemen.sql`

3. Create build  
```
  mkdir build && cd build
  cmake ..
  make
```

4. Run program
`./simple_cli_app`

To rebuild, `make` again in build.

TODO:
- Make this app flexible enough to use any MySQL database imported
- Refactor codes
