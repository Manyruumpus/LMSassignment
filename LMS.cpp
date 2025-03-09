#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

#ifdef _WIN32
    #include <conio.h>
    #define CLEAR_COMMAND "cls"
#endif

// Function to display a menu and let the user select an option using arrow keys
int menuSelect(const vector<string>& options) {
    int selected = 0;
    while (true) {
        system(CLEAR_COMMAND);
        cout << "Use UP/DOWN arrow keys to navigate and press ENTER to select:\n\n";
        // Display the options with a highlight on the selected one
        for (int i = 0; i < options.size(); i++) {
            if (i == selected)
                cout << ">> " << options[i] << "\n";
            else
                cout << "   " << options[i] << "\n";
        }
        int ch = getch();
#ifdef _WIN32
        if (ch == 224) { // arrow key indicator
            ch = getch();
            if (ch == 72) { // up arrow
                selected = (selected - 1 + options.size()) % options.size();
            } else if (ch == 80) { // down arrow
                selected = (selected + 1) % options.size();
            }
        } else if (ch == 13) { // Enter key
            return selected;
        }
#else
        // On Linux/Mac, arrow keys produce escape sequences
        if (ch == 27) { // Escape
            int ch2 = getch();
            if (ch2 == 91) {
                int ch3 = getch();
                if (ch3 == 65) { // up arrow
                    selected = (selected - 1 + options.size()) % options.size();
                } else if (ch3 == 66) { // down arrow
                    selected = (selected + 1) % options.size();
                }
            }
        } else if (ch == 10) { // Enter key (newline)
            return selected;
        }
#endif
    }
}

//-------------------- Book Class --------------------
enum BookStatus { AVAILABLE, BORROWED, RESERVED };

class Book {
private:
    string title;
    string author;
    string publisher;
    int year;
    string ISBN;
    BookStatus status;
public:
    Book() {}
    Book(string t, string a, string p, int y, string isbn, BookStatus s = AVAILABLE)
        : title(t), author(a), publisher(p), year(y), ISBN(isbn), status(s) {}

    // Getters
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getPublisher() const { return publisher; }
    int getYear() const { return year; }
    string getISBN() const { return ISBN; }
    BookStatus getStatus() const { return status; }

    // Setters
    void setStatus(BookStatus s) { status = s; }
    void setTitle(const string &t) { title = t; }
    void setAuthor(const string &a) { author = a; }
    void setPublisher(const string &p) { publisher = p; }
    void setYear(int y) { year = y; }

    // Display method
    void display() const {
        cout << "Title: " << title 
             << "\nAuthor: " << author 
             << "\nPublisher: " << publisher 
             << "\nYear: " << year 
             << "\nISBN: " << ISBN 
             << "\nStatus: " << (status == AVAILABLE ? "Available" : (status == BORROWED ? "Borrowed" : "Reserved"))
             << "\n";
    }

    // Serialization: comma separated values (title,author,publisher,year,ISBN,status)
    string serialize() const {
        return title + "," + author + "," + publisher + "," + to_string(year) + "," + ISBN + "," + to_string(status);
    }

    // Deserialization from a line of text
    static Book deserialize(const string &data) {
        stringstream ss(data);
        string t, a, p, yStr, isbn, statusStr;
        getline(ss, t, ',');
        getline(ss, a, ',');
        getline(ss, p, ',');
        getline(ss, yStr, ',');
        getline(ss, isbn, ',');
        getline(ss, statusStr, ',');
        int y = stoi(yStr);
        int s = stoi(statusStr);
        return Book(t, a, p, y, isbn, static_cast<BookStatus>(s));
    }
};

//-------------------- Account Class --------------------
class Account {
private:
    vector<string> borrowedBooks; // ISBNs of borrowed books
    double outstandingFine;
public:
    Account() : outstandingFine(0.0) {}

    void addBorrowedBook(const string &isbn) {
        borrowedBooks.push_back(isbn);
    }

    void removeBorrowedBook(const string &isbn) {
        for(auto it = borrowedBooks.begin(); it != borrowedBooks.end(); ++it) {
            if(*it == isbn) {
                borrowedBooks.erase(it);
                break;
            }
        }
    }

    int borrowedCount() const { return borrowedBooks.size(); }
    double getFine() const { return outstandingFine; }
    void addFine(double fine) { outstandingFine += fine; }
    void payFine() {
        outstandingFine = 0;
        cout << "Fine cleared.\n";
    }
    
    void displayAccount() const {
        cout << "Borrowed Books: ";
        if(borrowedBooks.empty())
            cout << "None";
        else {
            for(const auto &isbn : borrowedBooks)
                cout << isbn << " ";
        }
        cout << "\nOutstanding Fine: " << outstandingFine << "\n";
    }

    // Serialize account data: ISBNs separated by semicolons followed by |fine
    string serialize() const {
        string data = "";
        for(const auto &isbn : borrowedBooks) {
            data += isbn + ";";
        }
        data += "|" + to_string(outstandingFine);
        return data;
    }

    // Deserialize account data from string
    void deserialize(const string &data) {
        size_t pos = data.find("|");
        if(pos != string::npos) {
            string booksPart = data.substr(0, pos);
            string finePart = data.substr(pos+1);
            borrowedBooks.clear();
            stringstream ss(booksPart);
            string token;
            while(getline(ss, token, ';')) {
                if(!token.empty())
                    borrowedBooks.push_back(token);
            }
            outstandingFine = stod(finePart);
        }
    }
};

//-------------------- User Base Class --------------------
class User {
protected:
    int userID;
    string name;
    Account account;
public:
    User(int id, const string &n) : userID(id), name(n) {}
    virtual void borrowBook(Book &book) = 0;
    virtual void returnBook(Book &book, int daysBorrowed) = 0;
    virtual void display() = 0;
    int getUserID() { return userID; }
    string getName() { return name; }
    Account & getAccount() { return account; }
    virtual string getRole() = 0;
    virtual string serialize() = 0;
    virtual ~User() {}
};

//-------------------- Student Class --------------------
class Student : public User {
public:
    Student(int id, const string &n) : User(id, n) {}

    void borrowBook(Book &book) override {
        // Constraints: max 3 books, no outstanding fine, book must be available
        if(account.getFine() > 0) {
            cout << "Outstanding fine exists. Please clear fine before borrowing.\n";
            return;
        }
        if(account.borrowedCount() >= 3) {
            cout << "Reached maximum borrowing limit (3 books) for students.\n";
            return;
        }
        if(book.getStatus() != AVAILABLE) {
            cout << "Book is not available for borrowing.\n";
            return;
        }
        account.addBorrowedBook(book.getISBN());
        book.setStatus(BORROWED);
        cout << "Book borrowed successfully.\n";
    }

    void returnBook(Book &book, int daysBorrowed) override {
        book.setStatus(AVAILABLE);
        account.removeBorrowedBook(book.getISBN());
        if(daysBorrowed > 15) {
            int overdueDays = daysBorrowed - 15;
            double fine = overdueDays * 10;
            account.addFine(fine);
            cout << "Book returned. Overdue by " << overdueDays << " days. Fine added: " 
                 << fine << " rupees.\n";
        } else {
            cout << "Book returned on time.\n";
        }
    }

    void display() override {
        cout << "Student ID: " << userID << ", Name: " << name << "\n";
        account.displayAccount();
    }

    string getRole() override { return "Student"; }

    // Format: Role,userID,name,accountData
    string serialize() override {
        return "Student," + to_string(userID) + "," + name + "," + account.serialize();
    }
};

//-------------------- Faculty Class --------------------
class Faculty : public User {
public:
    Faculty(int id, const string &n) : User(id, n) {}

    void borrowBook(Book &book) override {
        if(account.borrowedCount() >= 5) {
            cout << "Reached maximum borrowing limit (5 books) for faculty.\n";
            return;
        }
        if(book.getStatus() != AVAILABLE) {
            cout << "Book is not available for borrowing.\n";
            return;
        }
        account.addBorrowedBook(book.getISBN());
        book.setStatus(BORROWED);
        cout << "Book borrowed successfully.\n";
    }

    void returnBook(Book &book, int daysBorrowed) override {
        book.setStatus(AVAILABLE);
        account.removeBorrowedBook(book.getISBN());
        if(daysBorrowed > 30) {
            if(daysBorrowed > 60)
                cout << "Book returned, but overdue by more than 60 days. Borrowing restrictions may apply.\n";
            else
                cout << "Book returned (overdue) but no fine for faculty.\n";
        } else {
            cout << "Book returned on time.\n";
        }
    }

    void display() override {
        cout << "Faculty ID: " << userID << ", Name: " << name << "\n";
        account.displayAccount();
    }

    string getRole() override { return "Faculty"; }

    string serialize() override {
        return "Faculty," + to_string(userID) + "," + name + "," + account.serialize();
    }
};

//-------------------- Librarian Class --------------------
class Librarian : public User {
public:
    Librarian(int id, const string &n) : User(id, n) {}

    void borrowBook(Book &book) override {
        cout << "Librarians cannot borrow books.\n";
    }

    void returnBook(Book &book, int daysBorrowed) override {
        cout << "Librarians do not borrow books.\n";
    }

    void display() override {
        cout << "Librarian ID: " << userID << ", Name: " << name << "\n";
    }

    string getRole() override { return "Librarian"; }

    string serialize() override {
        return "Librarian," + to_string(userID) + "," + name + "," + account.serialize();
    }
};

//-------------------- Library Class --------------------
class Library {
private:
    vector<Book> books;
    vector<User*> users; // polymorphic pointers
public:
    Library() {}
    ~Library() {
        for(auto u : users)
            delete u;
    }

    // Book management
    void addBook(const Book &book) {
        books.push_back(book);
    }

    void removeBook(const string &isbn) {
        for(auto it = books.begin(); it != books.end(); ++it) {
            if(it->getISBN() == isbn) {
                books.erase(it);
                cout << "Book removed successfully.\n";
                return;
            }
        }
        cout << "Book not found.\n";
    }

    void updateBook(const string &isbn) {
        for(auto &book : books) {
            if(book.getISBN() == isbn) {
                cout << "Enter new title: ";
                string newTitle;
                getline(cin, newTitle);
                cout << "Enter new author: ";
                string newAuthor;
                getline(cin, newAuthor);
                cout << "Enter new publisher: ";
                string newPublisher;
                getline(cin, newPublisher);
                cout << "Enter new year: ";
                int newYear;
                cin >> newYear;
                cin.ignore();
                book.setTitle(newTitle);
                book.setAuthor(newAuthor);
                book.setPublisher(newPublisher);
                book.setYear(newYear);
                cout << "Book updated successfully.\n";
                return;
            }
        }
        cout << "Book not found.\n";
    }

    Book* findBook(const string &isbn) {
        for(auto &book : books) {
            if(book.getISBN() == isbn)
                return &book;
        }
        return nullptr;
    }

    void displayBooks() {
        cout << "\n--- Library Books ---\n";
        for(auto &book : books) {
            book.display();
            cout << "---------------------\n";
        }
        cout << "Press any key to continue...";
        getch();
    }

    // User management
    void addUser(User* user) {
        users.push_back(user);
    }

    void removeUser(int userID) {
        for(auto it = users.begin(); it != users.end(); ++it) {
            if((*it)->getUserID() == userID) {
                delete *it;
                users.erase(it);
                cout << "User removed successfully.\n";
                return;
            }
        }
        cout << "User not found.\n";
    }

    User* findUser(int userID) {
        for(auto u : users) {
            if(u->getUserID() == userID)
                return u;
        }
        return nullptr;
    }

    void displayUsers() {
        cout << "\n--- Library Users ---\n";
        for(auto u : users) {
            u->display();
            cout << "---------------------\n";
        }
        cout << "Press any key to continue...";
        getch();
    }

    // File handling for books
    void loadBooks(const string &filename) {
        ifstream fin(filename);
        if(!fin) {
            cout << "Books file not found, loading default books.\n";
            // Load default 10 books
            addBook(Book("C++ Primer", "Lippman", "Addison-Wesley", 2012, "ISBN001"));
            addBook(Book("Effective C++", "Meyers", "O'Reilly", 2005, "ISBN002"));
            addBook(Book("The C++ Programming Language", "Stroustrup", "Addison-Wesley", 2013, "ISBN003"));
            addBook(Book("Accelerated C++", "Koenig", "Addison-Wesley", 2000, "ISBN004"));
            addBook(Book("Clean Code", "Martin", "Prentice Hall", 2008, "ISBN005"));
            addBook(Book("Design Patterns", "Gamma", "Addison-Wesley", 1994, "ISBN006"));
            addBook(Book("Modern Operating Systems", "Tanenbaum", "Prentice Hall", 2014, "ISBN007"));
            addBook(Book("Introduction to Algorithms", "Cormen", "MIT Press", 2009, "ISBN008"));
            addBook(Book("Computer Networks", "Tanenbaum", "Prentice Hall", 2011, "ISBN009"));
            addBook(Book("Artificial Intelligence", "Russell", "Pearson", 2010, "ISBN010"));
            return;
        }
        string line;
        while(getline(fin, line)) {
            if(!line.empty()) {
                Book b = Book::deserialize(line);
                addBook(b);
            }
        }
        fin.close();
    }

    void saveBooks(const string &filename) {
        ofstream fout(filename);
        for(auto &book : books)
            fout << book.serialize() << "\n";
        fout.close();
    }

    // File handling for users (a simplified approach)
    void loadUsers(const string &filename) {
        ifstream fin(filename);
        if(!fin) {
            cout << "Users file not found, loading default users.\n";
            // Load default: 5 students, 3 faculty, 1 librarian
            addUser(new Student(1, "Alice"));
            addUser(new Student(2, "Bob"));
            addUser(new Student(3, "Charlie"));
            addUser(new Student(4, "Diana"));
            addUser(new Student(5, "Eve"));
            addUser(new Faculty(6, "Prof. Smith"));
            addUser(new Faculty(7, "Prof. Johnson"));
            addUser(new Faculty(8, "Prof. Williams"));
            addUser(new Librarian(9, "Mr. Librarian"));
            return;
        }
        string line;
        while(getline(fin, line)) {
            if(!line.empty()) {
                // Expected format: Role,userID,name,accountData
                stringstream ss(line);
                string role, idStr, name, accountData;
                getline(ss, role, ',');
                getline(ss, idStr, ',');
                getline(ss, name, ',');
                getline(ss, accountData);
                int id = stoi(idStr);
                User* user = nullptr;
                if(role == "Student")
                    user = new Student(id, name);
                else if(role == "Faculty")
                    user = new Faculty(id, name);
                else if(role == "Librarian")
                    user = new Librarian(id, name);
                if(user) {
                    user->getAccount().deserialize(accountData);
                    addUser(user);
                }
            }
        }
        fin.close();
    }

    void saveUsers(const string &filename) {
        ofstream fout(filename);
        for(auto u : users)
            fout << u->serialize() << "\n";
        fout.close();
    }
};

//-------------------- Menu Functions --------------------

// Student Menu using arrow keys
void studentMenu(Library &lib, User* user) {
    vector<string> options = {"Borrow Book", "Return Book", "View Account", "Pay Fine", "Exit"};
    while (true) {
        int choice = menuSelect(options);
        system(CLEAR_COMMAND);
        if (choice == 0) { // Borrow Book
            cout << "Enter ISBN of book to borrow: ";
            string isbn;
            getline(cin, isbn);
            Book* book = lib.findBook(isbn);
            if(book)
                user->borrowBook(*book);
            else
                cout << "Book not found.\n";
            cout << "\nPress any key to continue...";
            getch();
        } else if (choice == 1) { // Return Book
            cout << "Enter ISBN of book to return: ";
            string isbn;
            getline(cin, isbn);
            Book* book = lib.findBook(isbn);
            if(book) {
                cout << "Enter number of days book was borrowed: ";
                int days;
                cin >> days;
                cin.ignore();
                user->returnBook(*book, days);
            } else {
                cout << "Book not found.\n";
            }
            cout << "\nPress any key to continue...";
            getch();
        } else if (choice == 2) { // View Account
            user->display();
            cout << "\nPress any key to continue...";
            getch();
        } else if (choice == 3) { // Pay Fine
            user->getAccount().payFine();
            cout << "\nPress any key to continue...";
            getch();
        } else if (choice == 4) { // Exit
            break;
        }
    }
}

// Faculty Menu using arrow keys
void facultyMenu(Library &lib, User* user) {
    vector<string> options = {"Borrow Book", "Return Book", "View Account", "Exit"};
    while (true) {
        int choice = menuSelect(options);
        system(CLEAR_COMMAND);
        if (choice == 0) { // Borrow Book
            cout << "Enter ISBN of book to borrow: ";
            string isbn;
            getline(cin, isbn);
            Book* book = lib.findBook(isbn);
            if(book)
                user->borrowBook(*book);
            else
                cout << "Book not found.\n";
            cout << "\nPress any key to continue...";
            getch();
        } else if (choice == 1) { // Return Book
            cout << "Enter ISBN of book to return: ";
            string isbn;
            getline(cin, isbn);
            Book* book = lib.findBook(isbn);
            if(book) {
                cout << "Enter number of days book was borrowed: ";
                int days;
                cin >> days;
                cin.ignore();
                user->returnBook(*book, days);
            } else {
                cout << "Book not found.\n";
            }
            cout << "\nPress any key to continue...";
            getch();
        } else if (choice == 2) { // View Account
            user->display();
            cout << "\nPress any key to continue...";
            getch();
        } else if (choice == 3) { // Exit
            break;
        }
    }
}

// Librarian Menu using arrow keys
void librarianMenu(Library &lib, User* user) {
    vector<string> options = {"Add Book", "Remove Book", "Update Book", "Add User", "Remove User", "View Books", "View Users", "Exit"};
    while (true) {
        int choice = menuSelect(options);
        system(CLEAR_COMMAND);
        if(choice == 0) { // Add Book
            cout << "Enter title: ";
            string title;
            getline(cin, title);
            cout << "Enter author: ";
            string author;
            getline(cin, author);
            cout << "Enter publisher: ";
            string publisher;
            getline(cin, publisher);
            cout << "Enter year: ";
            int year;
            cin >> year;
            cin.ignore();
            cout << "Enter ISBN: ";
            string isbn;
            getline(cin, isbn);
            lib.addBook(Book(title, author, publisher, year, isbn));
            cout << "Book added successfully.\n";
        } else if(choice == 1) { // Remove Book
            cout << "Enter ISBN of book to remove: ";
            string isbn;
            getline(cin, isbn);
            lib.removeBook(isbn);
        } else if(choice == 2) { // Update Book
            cout << "Enter ISBN of book to update: ";
            string isbn;
            getline(cin, isbn);
            lib.updateBook(isbn);
        } else if(choice == 3) { // Add User
            cout << "Enter role for new user (Student/Faculty): ";
            string role;
            getline(cin, role);
            cout << "Enter user ID: ";
            int id;
            cin >> id;
            cin.ignore();
            cout << "Enter name: ";
            string name;
            getline(cin, name);
            if(role == "Student")
                lib.addUser(new Student(id, name));
            else if(role == "Faculty")
                lib.addUser(new Faculty(id, name));
            else
                cout << "Invalid role.\n";
            cout << "User added successfully.\n";
        } else if(choice == 4) { // Remove User
            cout << "Enter user ID to remove: ";
            int id;
            cin >> id;
            cin.ignore();
            lib.removeUser(id);
        } else if(choice == 5) { // View Books
            lib.displayBooks();
        } else if(choice == 6) { // View Users
            lib.displayUsers();
        } else if(choice == 7) { // Exit
            break;
        }
        cout << "\nPress any key to continue...";
        getch();
    }
}

//-------------------- Main Function --------------------
int main() {
    Library lib;
    // Load data from files
    lib.loadBooks("books.txt");
    lib.loadUsers("users.txt");

    int userID;
    cout << "Enter your user ID to login: ";
    cin >> userID;
    cin.ignore();
    User* currentUser = lib.findUser(userID);
    if(!currentUser) {
        cout << "User not found. Exiting.\n";
        return 0;
    }
    cout << "Welcome " << currentUser->getName() << " (" << currentUser->getRole() << ")\n";
    cout << "Press any key to continue...";
    getch();

    // Display appropriate menu based on user role using arrow key navigation
    if(currentUser->getRole() == "Student")
        studentMenu(lib, currentUser);
    else if(currentUser->getRole() == "Faculty")
        facultyMenu(lib, currentUser);
    else if(currentUser->getRole() == "Librarian")
        librarianMenu(lib, currentUser);

    // Save data to files before exiting
    lib.saveBooks("books.txt");
    lib.saveUsers("users.txt");

    cout << "Exiting Library Management System.\n";
    return 0;
}
