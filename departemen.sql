CREATE DATABASE IF NOT EXISTS departemen;

CREATE USER IF NOT EXISTS 'admin_cpp'@'localhost' IDENTIFIED BY 'abc123';
GRANT SELECT, INSERT, UPDATE ON departemen.* TO 'admin_cpp'@'localhost';
FLUSH PRIVILEGES;

USE departemen;

CREATE TABLE IF NOT EXISTS Dosen (
  Dosen_ID INT PRIMARY KEY AUTO_INCREMENT,
  Nama VARCHAR(50),
  Umur INT,
  Jabatan VARCHAR(50)
);

CREATE TABLE IF NOT EXISTS Kelas (
  Kelas_ID INT PRIMARY KEY AUTO_INCREMENT,
  Nama VARCHAR(50),
  SKS INT DEFAULT 3,
  Genap BOOLEAN
);

CREATE TABLE IF NOT EXISTS Mahasiswa (
  Mahasiswa_ID INT PRIMARY KEY AUTO_INCREMENT,
  Nama VARCHAR(50),
  Umur INT,
  Semester INT DEFAULT 1
);

CREATE TABLE IF NOT EXISTS Mahasiswa_Kelas (
  Mahasiswa_ID INT,
  Kelas_ID INT,
  FOREIGN KEY (Mahasiswa_ID) REFERENCES Mahasiswa(Mahasiswa_ID),
  FOREIGN KEY (Kelas_ID) REFERENCES Kelas(Kelas_ID)
);

CREATE TABLE IF NOT EXISTS Dosen_Kelas (
  Dosen_ID INT,
  Kelas_ID INT,
  FOREIGN KEY (Dosen_ID) REFERENCES Dosen(Dosen_ID),
  FOREIGN KEY (Kelas_ID) REFERENCES Kelas(Kelas_ID)
);

INSERT IGNORE INTO Dosen (Dosen_ID, Nama, Umur, Jabatan) VALUES
('1', 'Budi', 35, 'Kaprodi'),
('2', 'Andi', 40, 'Dosen'),
('3', 'Azka', 20, 'Dosen');

INSERT IGNORE INTO Kelas (Kelas_ID, Nama, SKS, Genap) VALUES
('1', 'Matematika', 3, TRUE),
('2', 'Fisika', 3, FALSE),
('3', 'Kimia', 3, TRUE);

INSERT IGNORE INTO Mahasiswa (Mahasiswa_ID, Nama, Umur, Semester) VALUES
('1', 'Rina', 19, 1),
('2', 'Siti', 20, 2),
('3', 'Dewi', 21, 3);

INSERT IGNORE INTO Mahasiswa_Kelas (Mahasiswa_ID, Kelas_ID) VALUES
('1', '1'),
('2', '2'),
('3', '3');

INSERT IGNORE INTO Dosen_Kelas (Dosen_ID, Kelas_ID) VALUES
('1', '1'),
('2', '2'),
('3', '3');
