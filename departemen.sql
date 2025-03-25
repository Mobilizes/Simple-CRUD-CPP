CREATE DATABASE IF NOT EXISTS departemen;

CREATE USER IF NOT EXISTS 'admin_cpp'@'localhost' IDENTIFIED BY 'abc123';
GRANT SELECT, INSERT, UPDATE ON departemen.* TO 'admin_cpp'@'localhost';
FLUSH PRIVILEGES;

USE departemen;

CREATE TABLE IF NOT EXISTS Dosen (
  Dosen_ID VARCHAR(5) PRIMARY KEY,
  Nama VARCHAR(50),
  Umur INT,
  Jabatan VARCHAR(50)
);

CREATE TABLE IF NOT EXISTS Kelas (
  Kelas_ID VARCHAR(5) PRIMARY KEY,
  Nama VARCHAR(50),
  SKS INT DEFAULT 3,
  Genap BOOLEAN
);

CREATE TABLE IF NOT EXISTS Mahasiswa (
  Mahasiswa_ID VARCHAR(5) PRIMARY KEY,
  Nama VARCHAR(50),
  Umur INT,
  Semester INT DEFAULT 1
);

CREATE TABLE IF NOT EXISTS Mahasiswa_Kelas (
  Mahasiswa_ID VARCHAR(5),
  Kelas_ID VARCHAR(5),
  FOREIGN KEY (Mahasiswa_ID) REFERENCES Mahasiswa(Mahasiswa_ID) ON UPDATE CASCADE,
  FOREIGN KEY (Kelas_ID) REFERENCES Kelas(Kelas_ID) ON UPDATE CASCADE
);

CREATE TABLE IF NOT EXISTS Dosen_Kelas (
  Dosen_ID VARCHAR(5),
  Kelas_ID VARCHAR(5),
  FOREIGN KEY (Dosen_ID) REFERENCES Dosen(Dosen_ID) ON UPDATE CASCADE,
  FOREIGN KEY (Kelas_ID) REFERENCES Kelas(Kelas_ID) ON UPDATE CASCADE
);

INSERT IGNORE INTO Dosen (Dosen_ID, Nama, Umur, Jabatan) VALUES
('D001', 'Budi', 35, 'Kaprodi'),
('D002', 'Andi', 40, 'Dosen'),
('D003', 'Azka', 20, 'Dosen');

INSERT IGNORE INTO Kelas (Kelas_ID, Nama, SKS, Genap) VALUES
('K001', 'Matematika', 3, TRUE),
('K002', 'Fisika', 3, FALSE),
('K003', 'Kimia', 3, TRUE);

INSERT IGNORE INTO Mahasiswa (Mahasiswa_ID, Nama, Umur, Semester) VALUES
('M001', 'Rina', 19, 1),
('M002', 'Siti', 20, 2),
('M003', 'Dewi', 21, 3);

INSERT IGNORE INTO Mahasiswa_Kelas (Mahasiswa_ID, Kelas_ID) VALUES
('M001', 'K001'),
('M002', 'K002'),
('M003', 'K003');

INSERT IGNORE INTO Dosen_Kelas (Dosen_ID, Kelas_ID) VALUES
('D001', 'K001'),
('D002', 'K002'),
('D003', 'K003');
