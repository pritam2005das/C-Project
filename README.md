# C-Based Database Management System (DBMS)

A simple command-line Database Management System developed using **C programming**. This project demonstrates fundamental DBMS operations, data structures, dynamic memory allocation, pointers, and file handling.

## Features

- **Create Table:** Create tables with automatically generated primary keys.
- **Show Tables:** Display all available tables.
- **Show Table:** View all records and columns in a selected table.
- **Table Details:** Display table structure, column information, and record count.
- **Edit Table:**
  - Add and delete columns.
  - Insert new records.
  - Find records using their ID.
  - Edit existing records.
  - Delete records.
  - Filter records using multiple conditions.
- **Data Types:** Supports `INT`, `FLOAT`, `DOUBLE`, and `STRING`.
- **Auto-Increment IDs:** Automatically generates unique IDs for tables and records.
- **Persistent Storage:** Saves database information to a file and loads it when the application starts.
- **Database Details:** Displays database name and total number of tables.

## Technologies Used

- C Programming Language
- Standard C Libraries:
  - `stdio.h`
  - `stdlib.h`
  - `string.h`
- Data Structures:
  - Doubly Circular Linked List
  - Dynamic Arrays
- File Handling
- Dynamic Memory Allocation (`malloc`, `calloc`, `realloc`, `free`)

## Project Structure

```text
C-DBMS/
│
├── main.c
├── database.db
└── README.md
```
`database.db` is generated automatically when the application saves database information.

## How to Run

### 1. Clone the Repository
```bash
git clone https://github.com/pritam2005das/C-Project.git
```

### 2. Navigate to the Project Directory
```bash
cd C-Project
```

### 3. Compile the Program

```bash
gcc database.c -o database
```


### 4. Run the Program

**Windows:**
```bash
database.exe
```

**Linux:**
```bash
./database
```

You can also specify a database filename through command-line arguments:

```bash
database.exe mydatabase.db
```

If no filename is provided, the program uses `database.db` by default.

## Concepts Implemented

This project applies the following C programming concepts:

- Structures and Pointers
- Functions and Modular Programming
- Doubly Circular Linked Lists
- Dynamic Memory Allocation
- Arrays and Strings
- Conditional Statements and Loops
- Command-Line Arguments
- File Handling and Data Persistence

## Learning Objectives

The main objective of this project is to understand how basic database operations work internally and how data can be managed dynamically using C.

It also provides practical experience with linked lists, memory management, and persistent file storage.

## Future Improvements

- Support for additional data types.
- SQL-like query interface.
- Sorting records.
- Import and export database functionality.
- Improved input validation and error handling.

## Author

**Pritam Das**

- GitHub: [https://github.com/pritam2005das](https://github.com/pritam2005das)

---

*This project is developed for educational purposes as a practical implementation of basic DBMS concepts using C.*
