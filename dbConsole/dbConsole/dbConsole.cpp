#include "sqlite3.h"
#include <io.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

#define  _O_U16TEXT 0x20000

using namespace std;

wstring string_to_wstring(const string& utf8);
bool isCreateDatabase(const char& enteredChar, bool& isCreateDb);
bool OpenDatabase(sqlite3*& database, string& path, const bool& isCreateDb);
void ProcessingSQLQueries(sqlite3*& database, string& SQLText);

int main()
{
	if (!setlocale(LC_ALL, "")) { setlocale(LC_ALL, "C.UTF-8"); } // The system localization is installed; if this fails, the universal localization is installed.

#ifdef _WIN32
	if (_setmode(_fileno(stdout), _O_U16TEXT) == -1) { wcerr << L"Error setting UTF-16 mode for stdout: " << L"Error code " << GetLastError(); } // Switches stdout output to Unicode UTF-16 mode so that std::wcout can print most characters correctly.

	if (_setmode(_fileno(stderr), _O_U16TEXT) == -1) { wcerr << L"Error setting UTF-16 mode for stderr: " << L"Error code " << GetLastError(); } // Switches stderr output to Unicode UTF-16 mode so that std::wcerr can print most characters correctly.
#endif

	bool isCreateDb = false;

	while (true)
	{
		char enteredChar = '\0';
		wcout << L"Enter 1 or 2:\n1. Open database\n2. Create and open database\n> ";
		cin >> enteredChar;
		cin.ignore((numeric_limits<streamsize>::max)(), '\n');

		if (isCreateDatabase(enteredChar, isCreateDb))
		{
			break;
		}
		else
		{
			wcout << L"Input error\n";
		}
	}

	string path = ""; // Absolute path to the database
	sqlite3* database = nullptr; // Pointer to a database object

	while (true)
	{
		isCreateDb ? wcout << L"Enter the absolute path where the database will be created and then opened: " : wcout << L"Enter the absolute path to the database: ";

		getline(cin, path);

		if (OpenDatabase(database, path, isCreateDb))
		{
			isCreateDb ? wcout << L"\nThe database has been created and opened\n" : wcout << L"\nThe database is open\n";
			break;
		}
		else
		{
			wcerr << L"Database error: " << sqlite3_errmsg(database) << "\n"; // We get an error from the database.
		}
	}

	string SQLText; // SQL query text

	while (true)
	{
		wcout << L"\nEnter SQL query (or QUIT/EXIT to exit):\n> ";

		getline(cin, SQLText);

		if (SQLText == "QUIT" || SQLText == "quit" || SQLText == "EXIT" || SQLText == "exit") { break; }

		if (SQLText.empty()) { continue; }

		ProcessingSQLQueries(database, SQLText);
	}

	sqlite3_close(database);
	return 0;
}

// Converts UTF-8 string to UTF-16 (wstring) to correctly display texts in different languages ??via wcout.
// utf8 - Input string in UTF-8 encoding.
wstring string_to_wstring(const string& utf8)
{
	if (utf8.empty())
	{
		return {};
	}

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);

	wstring wstr(size_needed, L'\0');

	MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), &wstr[0], size_needed);

	return wstr;
}

// Check user input and set the database mode.
// enteredChar - The character entered by the user. isCreateDb - Reference to a flag that is set to true when the database creation is selected.
bool isCreateDatabase(const char& enteredChar, bool& isCreateDb)
{
	if (enteredChar == '1' || enteredChar == '2')
	{
		isCreateDb = enteredChar == '2';
		return true;
	}
	else
	{
		return false;
	}
}

// Opens/creates and opens the database file at the specified path.
//database - Reference to the database pointer (initialized on success). path - Path to the database file. isCreateDb - Mode of operation (true = create and open, false = open).
bool OpenDatabase(sqlite3*& database, string& path, const bool& isCreateDb)
{
	if (sqlite3_open_v2(path.c_str(), &database, isCreateDb ? SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE : SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK && !path.empty()) // ѕытаемс€ открыть базу данных
	{
		sqlite3_close(database); // We close the database.
		return false;
	}
	else
	{
		return true;
	}
}

// Executes SQL queries. For SELECT, outputs the result as a table, for others, a success/error message.
// database - Link to the database pointer. SQLText - String with SQL query (can contain several commands).
void ProcessingSQLQueries(sqlite3*& database, string& SQLText)
{
	int returnCode = -1000;

	const char* SQLquery = SQLText.c_str();

	sqlite3_stmt* statement = nullptr; // Pointer to a prepared statement (bytecode)

	while (*SQLquery)
	{
		statement = nullptr;

		int returnCode = sqlite3_prepare_v2(database, SQLquery, -1, &statement, &SQLquery); // sqlite3_prepare_v2(our database, SQLquery, SQL query length (-1 - automatic calculation), sets "statement", works with only one SQL query at a time and if there is more than one SQL query, returns the next SQLquery.
		if (returnCode != SQLITE_OK)
		{
			wcerr << L"Error preparing SQL query: " << sqlite3_errmsg(database) << "\n";

			sqlite3_finalize(statement);  // Frees the resources of a prepared statement. Should always be called after finishing work with the prepared statement.
			continue;
		}

		bool readOnly = sqlite3_stmt_readonly(statement) != 0;

		if (readOnly)
		{
			wcout << L"\n";

			vector<vector<wstring>> rows;

			int cols = sqlite3_column_count(statement); // Number of columns in the table

			rows.emplace_back();
			for (int j = 0; j < cols; ++j)
			{
				rows[0].emplace_back(string_to_wstring(sqlite3_column_name(statement, j)));
			}

			while (sqlite3_step(statement) == SQLITE_ROW)
			{
				rows.emplace_back();
				for (int j = 0; j < cols; ++j)
				{
					const unsigned char* t = sqlite3_column_text(statement, j); // Data from the current result row
					rows.back().push_back(t ? string_to_wstring((const char*)t) : L"");
				}
			}

			vector<int> widths(cols, 0);
			for (auto& row : rows)
			{
				for (int j = 0; j < cols; ++j)
				{
					widths[j] = max<int>(widths[j], (int)row[j].size());
				}
			}

			for (auto& row : rows) {
				for (int j = 0; j < cols; ++j)
				{
					wcout << left << setw(widths[j] + 2) << row[j];
				}
				wcout << L"\n";
			}

			if (returnCode == SQLITE_DONE && returnCode != SQLITE_ROW)
			{
				wcerr << L"SQL query execution error: " << sqlite3_errmsg(database) << "\n";
			}
		}
		else
		{
			returnCode = sqlite3_step(statement);
			if (returnCode != SQLITE_DONE)
			{
				wcerr << L"SQL query execution error: " << sqlite3_errmsg(database) << "\n";
			}
			else {
				wcout << L"Request completed.\n";
			}
		}

	}
	sqlite3_finalize(statement);
}
