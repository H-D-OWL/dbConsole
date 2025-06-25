# dbConsole
## Description
A console application that allows you to create and open databases and send any SQL queries to them.

Ideal for those who need a simple, no-installation SQL shell.

## Requirement

Target platform: Windows x64

## Features

* Working with sqlite3
  
* Processing multiple SQL queries in one input

* UTF‑16 support (for correct display of text in any language)

## Before use

0. Open "dbConsole.sln".

1. Make sure that "sqlite3.h" is in "Header Files". If the file is missing: right-click on "Header Files" -> Add -> Existing item -> select "sqlite3.h" from the "SQL" folder.

2. Right-click on "dbConsole" in Solution Explorer -> Properties -> C/C++ -> General -> Additional Include Directories -> Edit -> New Line -> ... -> select the "SQL" folder -> Ok -> Click "Apply".

3. Right click on “dbConsole” in Solution Explorer -> Properties -> Linker -> Input -> Additional Dependencies -> Edit -> write: “sqlite3.lib” -> Ok -> Click "Apply".

4. Right-click on "dbConsole" in Solution Explorer -> Properties -> Linker -> General -> Additional Library Directories -> Edit -> New Line -> ... -> select the "SQL" folder -> Ok -> Click "Apply".
  
## Usage

0. Navigate to the folder with dbConsole.exe (e.g. dbConsole\x64\Release).

1. Select the action. Open (1) or create and open (2) the database.
   
![Sprite-0001](https://github.com/user-attachments/assets/4e4be799-e2fa-4eeb-b3af-6eab4207fa4b)

2. Enter the path by which the database will be opened or created and opened.

![Sprite-0002](https://github.com/user-attachments/assets/d995ab24-0271-4a4e-a577-001474faf5fe)

3. We enter any SQL queries.

![Sprite-0003](https://github.com/user-attachments/assets/6b9865e2-9386-433b-a126-f63f07301ead)

## Warning
 1. If you move or copy dbConsole.exe, be sure to include sqlite3.dll in the same folder, otherwise the program may not start.

 2. If after entering the path you get the error: "Database error: bad parameter or other API misuse"
    - 1. Run the program with administrator rights.
    - 2. Check that the path contains only Latin letters (A-Z, a-z) and numbers (0-9) and does not contain non-Latin characters. (spaces are allowed)


## Notes

* The program is autonomous - it does not require other tools or the Internet.

* The program works with .db files.
