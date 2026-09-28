# MP3 Tag Reader

## Project Overview

MP3 Tag Reader is a C-based application designed to read and modify metadata stored inside MP3 files using ID3 tags.

The application works with metadata such as title, artist, album, year, comment, and genre. It provides a command-line interface for viewing and editing MP3 metadata while demonstrating practical implementation of binary file handling and modular C programming.

## Features

- View MP3 metadata
- Display title, artist, album, year, comment, and genre
- Edit MP3 metadata
- Read and process ID3 tag information
- Validate MP3 files before processing
- Command-line based operation
- Error handling for invalid operations

## Technologies Used

- C
- GCC
- Linux
- File Handling
- Binary File Processing

## Concepts Used

- Structures
- Pointers
- Dynamic Memory Allocation
- String Handling
- File Handling
- Binary File Processing
- Command-Line Arguments
- Modular Programming
- Error Handling

## Compilation

Compile the project using GCC:

```bash
gcc *.c -o mp3tag
```

## Usage

Display the available commands:

```bash
./mp3tag -h
```

View MP3 metadata:

```bash
./mp3tag -v filename.mp3
```

The help command displays the available options for viewing and editing MP3 metadata.

## Project Objective

The objective of this project is to implement an application that can process MP3 metadata using ID3 tags while applying file handling, memory management, data structures, and modular programming techniques in C.

## Author

Arjun K
