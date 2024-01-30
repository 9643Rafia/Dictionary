#include<iostream>
#include<string.h>
#include<string>
#include<fstream>
#include<stdio.h>
#include <algorithm>
#include<iomanip>
using namespace std;

//node structure for a bst
struct Entries
{
	string word;
	string meaning;

	Entries() {}
	Entries(const string w, const string m)
	{
		word = w;
		meaning = m;
	}
};

//using rehashing for dynamic resizing
class HashTable
{
	//initial size of hash table
	static const int size = 48544;
	static const double loadFactorThreshhold;
	//array of entries
	Entries* table;
	int* tableSize;
	int occupied;
	int capacity;

public:
	//constructor
	HashTable()
	{
		table = new Entries[size];
		tableSize = new int[size];
		occupied = 0;
		capacity = size;

		for (int i = 0; i < size; ++i)
		{
			tableSize[i] = 0;
		}
	}

	~HashTable()
	{
		delete[] table;
		delete[] tableSize;

	}
	//simple hash function
	//converts key to hash value which indicates index
	//where string is located in table
	size_t hashFunction(const string& key) const
	{
		if (key == " ")
		{
			return 0;
		}
		// A simple hash combining the length of the key and ASCII values of characters
		size_t hashValue = 17; // Initial value
		for (char ch : key)
		{
			hashValue = hashValue * 31 + static_cast<size_t>(ch);
		}
		return hashValue % capacity;
	}
	//rehash function
	void rehash()
	{
		// Save the current table and size
		Entries* oldTable = table;
		int* oldTableSize = tableSize;
		int oldCapacity = capacity;

		// Create a new table with double the size
		int newCapacity = capacity * 2;
		table = new Entries[newCapacity];
		tableSize = new int[newCapacity];
		capacity = newCapacity;

		// Initialize the new table size array
		for (int i = 0; i < newCapacity; ++i)
		{
			tableSize[i] = 0;
		}

		// Reinsert all elements from the old table into the new table
		for (int i = 0; i < oldCapacity; ++i)
		{
			for (int j = 0; j < oldTableSize[i]; ++j)
			{
				if (tableSize[i] != 0)
				{
					insert(table[i].word, table[i].meaning);
				}
			}
		}
		// Deallocate memory for the old table and size array
		delete[] oldTable;
		delete[] oldTableSize;
	}
	//to check user input
	bool isAlpha(const string& str)
	{
		{
			for (char ch : str)
			{
				if (!isalpha(ch))
				{
					return false;  // Found a non-alphabetic character
				}
			}
			return true;  // All characters are alphabetic
		}
	}
	//insert in hash table
	void insert(const string& word, const string& meaning)
	{
		//if the occupied size greater than threshold defined then 
		//resize the table dynamically

		if (word.empty() || meaning.empty() || !isAlpha(word) || !isAlpha(meaning))
		{
			// You may choose to handle this case differently, e.g., print a message
			cerr << "Warning: Attempted to insert an entry with an empty or null key." << endl;
			return; // Skip insertion
		}

		if ((occupied / capacity) > loadFactorThreshhold)
		{
			rehash();
		}

		int index = hashFunction(word);
		// Linear probing to handle collisions
		while (!table[index].word.empty())
		{
			index = (index + 1) % capacity;
		}

		table[index] = Entries(word, meaning);
		tableSize[index]++;
		occupied++;
	}
	//same as insertion
	Entries* retrieve(const string& word)
	{
		if (word.empty() || !isAlpha(word))
		{
			// You may choose to handle this case differently, e.g., print a message
			cerr << "Warning: Attempted to insert an entry with an empty or null key." << endl;
			return nullptr; // Skip insertion
		}
		int index = hashFunction(word);

		//linear probing to find the word
		if (!table[index].word.empty() && table[index].word == word)
		{
			// Return a pointer to the entry if found
			return &table[index];
		}
		return nullptr;
	}
	////same as insert except it gives the
	////index of the word after searching for it
	int retrieve_index(const string& word)
	{
		if (word.empty() || !isAlpha(word))
		{
			// You may choose to handle this case differently, e.g., print a message
			cerr << "Warning: Attempted to insert an entry with an empty or null key." << endl;
			return -1; // Skip insertion
		}
		int index = hashFunction(word);
		//if found
		if (!table[index].word.empty() && table[index].word == word)
		{
			return index;
		}
		else
		{
			return -1;
		}
	}
	//remove word from dictionary txt file
	bool remove(const string& word)
	{
		if (word.empty() || !isAlpha(word))
		{
			// You may choose to handle this case differently, e.g., print a message
			cerr << "Warning: Attempted to insert an entry with an empty or null key." << endl;
			return false; // Skip insertion
		}
		int index = hashFunction(word);
		// Linear probing to handle collisions
		while (!table[index].word.empty())
		{
			// Check if the current entry's word matches the search key
			if (table[index].word == word)
			{
				// Remove the entry by marking it as empty
				table[index].word.clear();
				table[index].meaning.clear();
				tableSize[index]--;
				occupied--;
				break;
			}
			index = (index + 1) % capacity;
		}
		return true;
	}
	//sorts the text file in alphabetical order
	void sortTextFile(const string& filename)
	{
		Entries* table = new Entries[capacity];
		int count = 0;
		ifstream file(filename);
		// Check if the input file is open
		if (!file.is_open()) {
			std::cerr << "Error opening file: " << filename << std::endl;
			return;
		}
		// Read entries into the array
		while (count < capacity && file >> table[count].meaning >> table[count].word)
		{
			++count;
		}
		file.close();
		sort(table, table + count, [](const auto& a, const auto& b)
		{
				return a.word < b.word;
		});
	}
	bool prefix(const string& pre, const string& str)
	{
		return str.compare(0, pre.length(), pre) == 0;
	}
	//word suggestion
	string* wordSuggestion(string& subword)
	{
		//atleast 10 suggestions
		string* sugg = new string[10];
		int count = 0;
		for (int i = 0; i < capacity; ++i)
		{
			if (!table[i].word.empty() && prefix(subword, table[i].word) && table[i].word != subword)
			{
				if (count < 10)
				{
					sugg[count] = table[i].word;
					count++;
				}
			}
		}
		//fill remaining with null strings
		for (int i = count; i < 10; ++i)
		{
			sugg[i] = "";
		}
		return sugg;
	}

};
const double HashTable::loadFactorThreshhold=0.7;

//class dictionary
//loading the dictionary using hashing
class Dictionary
{
	HashTable table;
public:
	//loading the dictionary using hashing
	void loadDictionary(const string& filename)
	{
		ifstream file(filename, ios::app);
		if (!file.is_open())
		{
			cerr << "Error opening file: " << filename << endl;
			return;
		}
		string word, meaning;
		int count = 0;
		while (file >> meaning >> word)
		{
			table.insert(word, meaning);
			cout << count << "-";
			count++;
		}
		file.close();
	}
	//conert word into small letters
	string small(string str)
	{
		string temp = "\0";
		for (size_t i = 0; i < str.length(); i++)
		{
			char ch = str[i];
			if (ch < 97)//97 is the ascii of a
			{
				ch += 32;//converting capital letters to small ,therefore, compiler can understand easily
			}
			temp += ch;
		}
		return temp;
	}
	//add a word in the dictionary
	void addWord(string& word, string& meaning)
	{
		//change word into small letters
		word = small(word);
		//insert in hash table
		table.insert(word, meaning);
		//insert in dictionary
		ifstream file("Dictionary.txt", ios::app);
		ofstream file_o("temp.txt", ios::app);

		if (file.is_open() && file_o.is_open())
		{
			bool check = false;
			//iterate through dictionary to find correct position
			//to add the word
			string current;
			string currentMeaning;
			while (file >> currentMeaning >> current)
			{
				//if the word is not the same
				if (word.compare(current) < 0 && !check)
				{
					file_o << meaning << "\t\t\t" << word << endl;
					check = true;
				}
				file_o << currentMeaning << "\t\t\t" << current << endl;
			}
			if (!check)
			{
				file_o << meaning << "\t\t\t" << word << endl;
			}
			file.close();
			file_o.close();

			//replace the temp with original
			remove("Dictionary.txt");
			rename("temp.txt", "Dictionary.txt");
		}
		else
		{
			cerr << "Error opening file: Dictionary.txt" << endl;
			return;
		}
	}
	//search for a word in the dictionary
	bool searchWord(string word)
	{
		//change word into small letters
		word = small(word);

		Entries* result = table.retrieve(word);
		if (result)
		{
			cout << "Word: " << result->word << endl;
			cout << "Meaning: " << result->meaning << endl;
			return true;
		}
		else
		{
			cout << "Word not found" << endl;
			return false;
		}
	}
	//deleting a word from dictionary
	void deleteWord(string& word)
	{
		//change word into small letters
		word = small(word);
		char op;
		cout << "-> Do you want to delete " << word << " from dictionary? (y/n): ";
		cin >> op;
		cout << endl;
		if (op == 'y' || op == 'Y')
		{
			if (!table.remove(word))
			{
				cout << "-> Found: Word did not get deleted " << word << endl;
				return;
			}
			else
			{
				//delete from the dictionary as well
				//insert in dictionary
				ifstream file("Dictionary.txt", ios::app);
				ofstream file_o("temp.txt", ios::app);

				if (file.is_open() && file_o.is_open())
				{
					string current;
					string currentMeaning;
					while (file >> currentMeaning >> current)
					{
						//if the word is not the same
						if (current != word)
						{
							file_o << currentMeaning << "\t\t\t" << current << endl;
						}
					}
					file.close();
					file_o.close();

					//replace the temp with original
					remove("Dictionary.txt");
					rename("temp.txt", "Dictionary.txt");
				}
				else
				{
					cerr << "Error opening file: Dictionary.txt" << endl;
				}
				cout << "-> Not found: Word has been deleted " << endl;
			}
		}
		else
		{
			cout << "------------->Back to Main Menu" << endl;
			return;
		}
	}
	//update word is same as delete
	void updateWord(string& word)
	{
		if (word.empty() || !table.isAlpha(word))
		{
			// You may choose to handle this case differently, e.g., print a message
			cerr << "Warning: Attempted to insert an entry with an empty or null key." << endl;
			return; // Skip insertion
		}
		//change word into small letters
		word = small(word);
		int index = table.retrieve_index(word);
		Entries* node = table.retrieve(word);
		char op;
		cout << "Word: " << word << endl;
		if (node != nullptr && index >= 0)
		{
			cout << "Meaning: " << node->meaning << endl;
			cout << "Do you want to update " << word << "(y/n)? ";
			cin >> op;
			if (op == 'y' || op == 'Y')
			{
				// update meaning
				string newMeaning;
				cout << "New Meaning: ";
				/*cin >> newMeaning;*/
				cin.ignore();
				getline(cin, newMeaning);
				node->meaning = newMeaning;

				if (newMeaning.empty() || !table.isAlpha(word))
				{
					// You may choose to handle this case differently, e.g., print a message
					cerr << "Warning: Attempted to insert an entry with an empty or null key." << endl;
					return; // Skip insertion
				}

				//update in dictionary text file as well
				ifstream file("Dictionary.txt", ios::app);
				ofstream file_o("temp.txt", ios::app);

				if (file.is_open() && file_o.is_open())
				{
					string current, currentMeaning;
					// Iterate through the dictionary to find and update the word
					while (file >> currentMeaning >> current)
					{
						if (current != word)
						{
							file_o << currentMeaning << "\t\t\t" << current << endl;
						}
						else
						{
							// If it's the word to be updated, write the new entry
							file_o << newMeaning << "\t\t\t" << word << endl;
						}
					}

					file.close();
					file_o.close();

					//replace the temp with original
					remove("Dictionary.txt");
					rename("temp.txt", "Dictionary.txt");
				}
				else
				{
					cerr << "Error opening file: Dictionary.txt" << endl;
				}
			}
			else
			{
				cout << "------------->Back to Main Menu" << endl;
			}
		}
		else
		{
			cerr << "Warning: Attempted to insert an entry with an empty or null key." << endl;
		}
	}
	//suggest words based on hashing
	void wordSuggestion(string& word)
	{
		string* sugg = table.wordSuggestion(word);
		cout << "   -> Suggestions    " << endl;
		for (int i = 0; i < 10 && !sugg[i].empty(); ++i)
		{
			cout << i + 1 << " - " << sugg[i] << endl;
		}
		delete[]sugg;
		cout << endl;
	}
};
int main()
{

	Dictionary obj;
	HashTable obj2;
	//sort the dictionay text file alphabetically before
	//loading in hash table
	obj2.sortTextFile("Dictionary.txt");
	obj.loadDictionary("Dictionary.txt");
	while (1)
	{
		system("cls");
		//system("F6");
		cout << endl << endl << endl << endl;
		cout << setw(80) << "---------------------------------------------" << endl;
		cout << setw(80) << "|     ->  MAIN MENU FOR DICTIONARY          |" << endl;
		cout << setw(80) << "---------------------------------------------" << endl;
		cout << setw(80) << "|     ->   1-ADD NEW WORD                   |" << endl;
		cout << setw(80) << "|     ->   2-SEARCH FOR WORD                |" << endl;
		cout << setw(80) << "|     ->   3-DELETE WORD                    |" << endl;
		cout << setw(80) << "|     ->   4-UPDATE WORD                    |" << endl;
		cout << setw(80) << "|     ->   5-SUGGESTION                     |" << endl;
		cout << setw(80) << "|     ->   6-Exit                           |" << endl;
		cout << setw(80) << "---------------------------------------------" << endl;
		cout << endl;
		cout << "-------------------------" << endl;
		cout << "->>Enter Your Choice   : ";  string option;
		cout << endl; cout << "-------------------------" << endl;
		cin >> option;
		cin.ignore();
		if (option == "1")
		{
			string word;
			string meaning;
			//obj.loading();
			system("cls");
			cout << setw(60) << "----------------------------" << endl;
			cout << setw(60) << "     ->    INSERTION        " << endl;
			cout << setw(60) << "----------------------------" << endl; cout << endl;
			//cin.ignore();
			cout << "Insert word  : ";
			cin >> word;
			cout << endl;
			cin.ignore();
			cout << "Insert meaning :  ";
			getline(cin, meaning);
			obj.addWord(word, meaning);
			system("pause");
		}
		else if (option == "2")
		{
			//obj.loading();
			system("cls");
			cout << setw(60) << "----------------------------" << endl;
			cout << setw(60) << "      ->   SEARCHING        " << endl;
			cout << setw(60) << "----------------------------" << endl;
			cout << endl;
			string sword;
			//cin.ignore();
			cout << "Enter Word you want to Search-> ";
			getline(cin, sword);
			obj.searchWord(sword);
			system("pause");
		}
		else if (option == "3")
		{
			//obj.loading();
			system("cls");
			cout << setw(60) << "----------------------------" << endl;
			cout << setw(60) << "       ->   DELETION        " << endl;
			cout << setw(60) << "----------------------------" << endl; cout << endl;
			string dword;
			cout << "Enter Word You want to Delete-> ";
			cin >> dword;
			obj.deleteWord(dword);
			system("pause");
		}
		else if (option == "4")
		{
			//obj.loading();
			system("cls");
			cout << setw(60) << "----------------------------" << endl;
			cout << setw(60) << "       ->   UPDATION        " << endl;
			cout << setw(60) << "----------------------------" << endl; cout << endl;
			string upword;
			cout << "Enter Word You want to Update-> ";
			cin >> upword;
			obj.updateWord(upword);
			system("pause");
		}
		else if (option == "5")
		{
			system("cls");
			cout << setw(60) << "----------------------------" << endl;
			cout << setw(60) << "       ->   SUGGESTION      " << endl;
			cout << setw(60) << "----------------------------" << endl; cout << endl;
			string word;
			cout << "Tell us the word whose suggestion you want-> ";
			//cin.ignore();
			getline(cin, word);
			obj.wordSuggestion(word);
			system("pause");
		}
		else if (option == "6")
		{
			cout << endl;
			return 0;
		}
		else
		{
			cout << "Invalid Input " << endl;
		}
	}
	//system("pause");
	return 0;
}
