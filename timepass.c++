// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <vector>
// #include <windows.h>
// #include <conio.h>
// #include <cstdlib>
// #include <ctime>
// #include <iomanip>
// #include <stdexcept>
// #include <cstdio>

// using namespace std;

// enum usertype {
//     Student, Faculty, Librarian
// };
// enum status {
//     Available, Reserved, unavailable
// };

// class user {
// public:
//     usertype user_type;
//     string userid;
    
//     virtual void show_option(string userid) {  // Virtual function
//         cout << "Default User Options" << endl;
//     }
// };

// // ----------------- Helper Functions for Date ----------------- //

// string currentDate() {
//     time_t now = time(0);
//     tm *ltm = localtime(&now);
//     char buffer[11]; // "YYYY-MM-DD"
//     sprintf(buffer, "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
//     return string(buffer);
// }

// string addDays(const string &date, int days) {
//     tm tm_date = {};
//     istringstream iss(date);
//     iss >> get_time(&tm_date, "%Y-%m-%d");
//     if (iss.fail()) {
//         throw runtime_error("Failed to parse date string: " + date);
//     }
//     time_t t = mktime(&tm_date);
//     if (t == -1) {
//         throw runtime_error("mktime failed");
//     }
//     t += days * 86400; // Add days in seconds
//     tm *new_tm = localtime(&t);
//     if (!new_tm) {
//         throw runtime_error("localtime failed");
//     }
//     ostringstream oss;
//     oss << put_time(new_tm, "%Y-%m-%d");
//     return oss.str();
// }

// // ----------------- Helper Functions for User Fine ----------------- //

// // Returns the current fine for a user by reading users.csv.
// int getUserFine(const string &userid) {
//     ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");
//     if (!fin) {
//          cout << "Error: Unable to open users.csv" << endl;
//          return -1;
//     }
//     string line;
//     while(getline(fin, line)) {
//          stringstream ss(line);
//          string uid, pass, userType, fineStr;
//          getline(ss, uid, ',');
//          getline(ss, pass, ',');
//          getline(ss, userType, ',');
//          getline(ss, fineStr, ',');
//          if (uid == userid) {
//              fin.close();
//              try {
//                 return stoi(fineStr);
//              } catch(...) {
//                 return -1;
//              }
//          }
//     }
//     fin.close();
//     return -1; // User not found
// }

// // Updates the user fine by adding additionalFine to the existing fine in users.csv.
// void updateUserFine(const string &userid, int additionalFine) {
//     vector<string> lines;
//     ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");
//     if (!fin) {
//         cout << "Error: Unable to open users.csv" << endl;
//         return;
//     }
//     string line;
//     while(getline(fin, line)) {
//         lines.push_back(line);
//     }
//     fin.close();
    
//     for(auto &l : lines) {
//          stringstream ss(l);
//          string uid, pass, userType, fineStr;
//          getline(ss, uid, ',');
//          getline(ss, pass, ',');
//          getline(ss, userType, ',');
//          getline(ss, fineStr, ',');
//          if (uid == userid) {
//              int currentFine = 0;
//              try {
//                 currentFine = stoi(fineStr);
//              } catch (...) {
//                 currentFine = 0;
//              }
//              currentFine += additionalFine;
//              ostringstream oss;
//              oss << uid << "," << pass << "," << userType << "," << currentFine;
//              l = oss.str();
//              break;
//          }
//     }
//     ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");
//     if (!fout) {
//          cout << "Error: Unable to write to users.csv" << endl;
//          return;
//     }
//     for (auto &l : lines) {
//          fout << l << "\n";
//     }
//     fout.close();
// }

// // ----------------- Students Class ----------------- //

// class Students : public user {
// public:
//     vector<string> borrowedBooks;
//     vector<string> reservedBooks;

//     // Student menu using arrow keys
//     void show_option(string userid) override {
//         const int numOptions = 4;
//         string options[numOptions] = {
//             "Borrow a Book",
//             "Return a Book",
//             "View Borrowed Book and Fine",
//             "Pay the Fine"
//         };
        
//         int selected = 0;
//         while (true) {
//             system("cls");
//             cout << "Student's Option Menu" << endl << endl;
//             for (int i = 0; i < numOptions; i++) {
//                 cout << (i == selected ? " -> " : "    ") << options[i] << endl;
//             }
//             int key = _getch();
//             if (key == 224) {
//                 key = _getch();
//                 if (key == 72 && selected > 0)
//                     selected--;
//                 else if (key == 80 && selected < numOptions - 1)
//                     selected++;
//             } else if (key == 13) {
//                 break;
//             }
//         }
        
//         switch (selected) {
//             case 0: borrowBook(userid); break;
//             case 1: returnBook(); break;
//             case 2: viewBorrowedBookAndFine(); break;
//             case 3: payFine(); break;
//             default: cout << "Invalid selection" << endl; break;
//         }
//     }
    
//     // Borrow book function with fine check.
//     void borrowBook(string userid) {
//         system("cls");
//         // Check if user has outstanding fines.
//         int userFine = getUserFine(userid);
//         if (userFine < 0) {
//             cout << "User record not found." << endl;
//             system("pause");
//             return;
//         }
//         if (userFine != 0) {
//             cout << "You have an outstanding fine of Rs. " << userFine 
//                  << ". Please pay your fine before borrowing a new book." << endl;
//             system("pause");
//             return;
//         }
        
//         // Check active borrowed count from borrowedbook.csv.
//         int activeBorrows = 0;
//         ifstream finBorrowed("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv");
//         if (finBorrowed) {
//             string line;
//             while (getline(finBorrowed, line)) {
//                 stringstream ss(line);
//                 string recordUser, bookISBN, borrowDate, dueDate, returnDate, fine;
//                 getline(ss, recordUser, ',');
//                 getline(ss, bookISBN, ',');
//                 getline(ss, borrowDate, ',');
//                 getline(ss, dueDate, ',');
//                 getline(ss, returnDate, ',');
//                 getline(ss, fine, ',');
//                 if (recordUser == userid && returnDate == "") {
//                     activeBorrows++;
//                 }
//             }
//             finBorrowed.close();
//         }
//         if (activeBorrows >= 3) {
//             cout << "You have already borrowed " << activeBorrows 
//                  << " books. You cannot borrow more than 3 books at a time." << endl;
//             system("pause");
//             return;
//         }
        
//         // Continue with borrowing: prompt for book ISBN.
//         string bookISBN;
//         cout << "Enter the ISBN of the book you want to borrow: ";
//         cin >> bookISBN;
        
//         // Open books.csv.
//         ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\books.csv");
//         if (!fin) {
//             cout << "Error: Unable to open books.csv" << endl;
//             system("pause");
//             return;
//         }
//         vector<string> fileLines;
//         string line;
//         bool found = false;
//         bool available = false;
//         int targetLineIndex = -1;
//         while (getline(fin, line)) {
//             fileLines.push_back(line);
//         }
//         fin.close();
        
//         // Search for the book.
//         for (size_t i = 0; i < fileLines.size(); i++) {
//             stringstream ss(fileLines[i]);
//             string title, author, publisher, yearStr, ISBNStr, statusStr;
//             getline(ss, title, ',');
//             getline(ss, author, ',');
//             getline(ss, publisher, ',');
//             getline(ss, yearStr, ',');
//             getline(ss, ISBNStr, ',');
//             getline(ss, statusStr, ',');
            
//             if (ISBNStr == bookISBN) {
//                 found = true;
//                 targetLineIndex = i;
//                 if (statusStr == "Available") {
//                     available = true;
//                 } else if (statusStr == "Unavailable") {
//                     string reserve;
//                     cout << "Sorry, the book is currently UNAVAILABLE.\n"
//                          << "But you can reserve it. Do you want to reserve the book? (Yes/No): ";
//                     cin >> reserve;
//                     if (reserve == "YES" || reserve == "yes" || reserve == "Yes") {
//                         string updatedLine = title + "," + author + "," + publisher + "," +
//                                              yearStr + "," + ISBNStr + ",Reserved";
//                         fileLines[i] = updatedLine;
//                         ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\books.csv");
//                         if (!fout) {
//                             cout << "Error: Unable to write to books.csv" << endl;
//                             system("pause");
//                             return;
//                         }
//                         for (const auto &l : fileLines) {
//                             fout << l << "\n";
//                         }
//                         fout.close();
//                         cout << "Book reserved successfully!" << endl;
//                         system("pause");
//                         return;
//                     } else {
//                         cout << "Reservation cancelled. Book remains unavailable." << endl;
//                         system("pause");
//                         return;
//                     }
//                 } else {
//                     cout << "Sorry, the book is currently UNAVAILABLE" << endl;
//                 }
//                 break;
//             }
//         }
//         if (!found) {
//             cout << "Book with ISBN " << bookISBN << " not found!" << endl;
//             system("pause");
//             return;
//         }
        
//         // Update the book record to "Unavailable" if it was available.
//         {
//             stringstream ss(fileLines[targetLineIndex]);
//             string title, author, publisher, yearStr, ISBNStr, statusStr;
//             getline(ss, title, ',');
//             getline(ss, author, ',');
//             getline(ss, publisher, ',');
//             getline(ss, yearStr, ',');
//             getline(ss, ISBNStr, ',');
//             getline(ss, statusStr, ',');
//             string updatedLine = title + "," + author + "," + publisher + "," + yearStr + "," + ISBNStr + ",Unavailable";
//             fileLines[targetLineIndex] = updatedLine;
//         }
//         {
//             ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\books.csv");
//             if (!fout) {
//                 cout << "Error: Unable to write to books.csv" << endl;
//                 system("pause");
//                 return;
//             }
//             for (const auto &l : fileLines) {
//                 fout << l << "\n";
//             }
//             fout.close();
//         }
        
//         // Record the borrowing transaction in borrowedbook.csv.
//         // Format: userID,bookISBN,borrowed_date,due_date,return_date,fine
//         string current_date = currentDate();
//         string due_date = addDays(current_date, 15);
//         string return_date = "";  // blank for now
//         string fine = "0";
//         ofstream foutBorrowed("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv", ios::app);
//         if (!foutBorrowed) {
//             cout << "Error: Unable to open borrowedbook.csv" << endl;
//             system("pause");
//             return;
//         }
//         foutBorrowed << userid << "," << bookISBN << "," << current_date << "," << due_date 
//                      << "," << return_date << "," << fine << "\n";
//         foutBorrowed.close();
//         borrowedBooks.push_back(bookISBN);
        
//         cout << "Book borrowed successfully!" << endl;
//         system("pause");
//         return;
//     }
    
//     // Dummy returnBook() implementation. When fine is calculated here,
//     // update the user's record in users.csv.
//     void returnBook() {
//         system("cls");
//         // [Your implementation of returning a book goes here...]
//         // For example, after computing a fine (say, int computedFine), call:
//         // updateUserFine(userid, computedFine);
//         cout << "Return book functionality is not yet implemented." << endl;
//         system("pause");
//     }
    
//     void viewBorrowedBookAndFine() {
//         system("cls");
//         // [Your implementation for viewing borrowed books and fine...]
//         system("pause");
//     }
    
//     void payFine() {
//         system("cls");
//         // [Your implementation for paying fine...]
//         system("pause");
//     }
// };
  
// // (Faculties and Librarians classes remain similar, omitted for brevity)

// class Account {
// private:
//     string userid;
//     string password;
    
// protected:
//     usertype type;
    
// public:
//     Account() : userid(""), password(""), type(Student) {}
//     Account(string uid, string pass, usertype utype)
//         : userid(uid), password(pass), type(utype) {}    
    
//     // Pass the stored userid from the Account to the respective object.
//     void showoption(usertype user_type, bool check) {
//         if (!check) {
//             cout << "No user found";
//         } else {
//             if (user_type == Student) {
//                 Students s;
//                 s.userid = this->userid;
//                 s.show_option(s.userid);
//             } else if (user_type == Faculty) {
//                 // Similar for Faculties...
//             } else if (user_type == Librarian) {
//                 // Similar for Librarians...
//             }
//         }
//     }
// };

// // (Other functions like gotoxy, knowuser_type, get_details, and auth remain the same)

// int main() {
//     usertype user_type = /* ... obtain user type, e.g., via knowuser_type() ... */;
//     pair<string, string> user_login = /* ... get_details(...) ... */;
//     bool check = /* ... auth(...) ... */;
//     Account acc(user_login.first, user_login.second, user_type);
//     acc.showoption(user_type, check);
//     return 0;
// }
