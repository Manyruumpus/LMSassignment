#include <iostream>
#include <string>
#include <stdexcept>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <conio.h>
#include <cstdlib>
#include <cstdio>   
#include <cmath>  
using namespace std;
enum usertype {
    Student, Faculty, Librarian
};
enum status {
    Available, Reserved, unavailable
};

// function to find the days difference between return and borrowed date 
int daysBetween(const string &startDate, const string &endDate) {
    tm tm_start = {};
    tm tm_end = {};
    
    istringstream issStart(startDate);
    issStart >> get_time(&tm_start, "%Y-%m-%d");
    if (issStart.fail()) {
        throw runtime_error("Failed to parse start date: " + startDate);
    }
    
    istringstream issEnd(endDate);
    issEnd >> get_time(&tm_end, "%Y-%m-%d");
    if (issEnd.fail()) {
        throw runtime_error("Failed to parse end date: " + endDate);
    }
    
    time_t t_start = mktime(&tm_start);
    time_t t_end   = mktime(&tm_end);
    if (t_start == -1 || t_end == -1) {
        throw runtime_error("mktime failed");
    }
    
    // Compute difference in seconds, then convert to days.
    int diffDays = static_cast<int>(difftime(t_end, t_start) / 86400);
    return diffDays;
}
// Function to calculate difference between two dates
int dateDifference(int year1, int month1, int day1, int year2, int month2, int day2) {
    // Create time structures
    tm date1 = {0}, date2 = {0};

    // Set date1 values
    date1.tm_year = year1 - 1900; // Years since 1900
    date1.tm_mon = month1 - 1;    // Months [0-11]
    date1.tm_mday = day1;

    // Set date2 values
    date2.tm_year = year2 - 1900;
    date2.tm_mon = month2 - 1;
    date2.tm_mday = day2;

    // Convert to time_t (seconds since epoch)
    time_t time1 = mktime(&date1);
    time_t time2 = mktime(&date2);

    // Calculate difference in seconds and convert to days
    double difference = difftime(time2, time1) / (60 * 60 * 24);

    return abs(difference); // Return absolute difference in days
}
//function to find the days between borrowed date and current date
class user {
    public:
        usertype user_type;
        string userid;
    
        virtual void show_option(string userid,usertype user_type) {  // ✅ Virtual function
            cout << "Default User Options" << endl;
        }
    };

    int see_user_fine(const string userid){
        ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");
                if (!fin) {
                    cout << "Error: Unable to open users.csv" << endl;
                    system("pause");
                    return NULL;
                }
                
                string line;
                bool userFound = false;
                int userFine = 0;
                while (getline(fin, line)) {
                    if (line.empty()) continue;
                    stringstream ss(line);
                    string uid, pass, userType, fineStr;
                    getline(ss, uid, ',');
                    getline(ss, pass, ',');
                    getline(ss, userType, ',');
                    getline(ss, fineStr, ',');
                    if (uid == userid) {
                        try {
                            userFine = stoi(fineStr);
                        } catch (...) {
                            userFine = 0;
                        }
                        userFound = true;
                        break;
                    }
                }
                fin.close();
                
                if (!userFound) {
                    cout << "User " << userid << " not found in users.csv" << endl;
                } 
                return userFine;
    }
    
    void see_borrowed_book(const string userid){
            string line;
            ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv");
                if (!fin) {
                    cout << "Error: Unable to open borrowedbook.csv" << endl;
                    system("pause");
                    return;
                }
                
                int activeCount = 0;
                
                cout << "\nActive borrowed books for user " << userid << ":\n";
                while (getline(fin, line)) {
                    if (line.empty()) continue;
                    stringstream ss(line);
                    string recordUser, bookISBN, borrowed_date, due_date, return_date, fine;
                    getline(ss, recordUser, ',');
                    getline(ss, bookISBN, ',');
                    getline(ss, borrowed_date, ',');
                    getline(ss, due_date, ',');
                    getline(ss, return_date, ',');
                    getline(ss, fine, ',');
                    
                    // Check if this record belongs to the given user and return_date is empty.
                    if (recordUser == userid && return_date == "") {
                        cout << "Book ISBN: " << bookISBN 
                             << ", Borrowed Date: " << borrowed_date 
                             << ", Due Date: " << due_date << "\n";
                        activeCount++;
                    }
                }
                fin.close();
                
                if (activeCount == 0) {
                    cout << "\nNo active borrowed books found for user " << userid << ".\n";
                } else {
                    cout << "\nTotal active borrowed books: " << activeCount << "\n";
                }
                
                system("pause");
    }

    string currentDate() {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        char buffer[11]; // "YYYY-MM-DD"
        sprintf(buffer, "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
        return string(buffer);
    }
         
    string addDays(const string &date, int days) {
        tm tm_date = {};
        istringstream iss(date);
        iss >> get_time(&tm_date, "%Y-%m-%d");
        if (iss.fail()) {
            throw runtime_error("Failed to parse date string: " + date);
        }
        time_t t = mktime(&tm_date);
        if (t == -1) {
            throw runtime_error("mktime failed");
        }
        t += days * 86400; // Add days in seconds
        tm *new_tm = localtime(&t);
        if (!new_tm) {
            throw runtime_error("localtime failed");
        }
        ostringstream oss;
        oss << put_time(new_tm, "%Y-%m-%d");
        return oss.str();
    }
    
    void updateUserFine(const string &userid, int additionalFine) {
        vector<string> lines;
        ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");
        if (!fin) {
            cout << "Error: Unable to open users.csv" << endl;
            return;
        }
        string line;
        while(getline(fin, line)) {
            lines.push_back(line);
        }
        fin.close();
        
        for(auto &l : lines) {
             stringstream ss(l);
             string uid, pass, userType, fineStr;
             getline(ss, uid, ',');
             getline(ss, pass, ',');
             getline(ss, userType, ',');
             getline(ss, fineStr, ',');
             if (uid == userid) {
                 int currentFine = 0;
                 try {
                    currentFine = stoi(fineStr);
                 } catch (...) {
                    currentFine = 0;
                 }
                 currentFine += additionalFine;
                 ostringstream oss;
                 oss << uid << "," << pass << "," << userType << "," << currentFine;
                 l = oss.str();
                 break;
             }
        }
        ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");
        if (!fout) {
             cout << "Error: Unable to write to users.csv" << endl;
             return;
        }
        for (auto &l : lines) {
             fout << l << "\n";
        }
        fout.close();
    }
    
    bool findoverdue(const string &userid) {
        string current_date = currentDate();
        
        ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv");
        if (!fin) {
            cout << "Error: Unable to open borrowedbook.csv" << endl;
            return false;
        }
        
        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string recordUser, bookISBN, borrowed_date, due_date, return_date, fine;
            getline(ss, recordUser, ',');
            getline(ss, bookISBN, ',');
            getline(ss, borrowed_date, ',');
            getline(ss, due_date, ',');
            getline(ss, return_date, ',');
            getline(ss, fine, ',');
            
            // Check if the book is borrowed by the given user and has no return date
            if (recordUser == userid && return_date=="") {
                try {
                    int diff = daysBetween(borrowed_date, current_date);
                    if (diff > 30) {
                        fin.close();
                        return true; // The user has an overdue book
                    }
                } catch (const exception &ex) {
                    cout << "Error calculating date difference: " << ex.what() << endl;
                    fin.close();
                    return true;
                }
            }
        }
        fin.close();
        return false; // The user does not have any overdue books
    }
        
    void availabiltychange(const string &userid, const string &bookISBN) {
        // Read all lines from books.csv
        vector<string> bookLines;
        ifstream finBooks("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\books.csv");
        if (!finBooks) {
            cout << "Error: Unable to open books.csv" << endl;
            return;
        }
        string line;
        while(getline(finBooks, line)) {
            bookLines.push_back(line);
        }
        finBooks.close();
        
        bool found = false;
        // Iterate over the book records
        for (size_t i = 0; i < bookLines.size(); i++) {
            stringstream bs(bookLines[i]);
            string title, author, publisher, yearStr, ISBNStr, statusStr;
            getline(bs, title, ',');
            getline(bs, author, ',');
            getline(bs, publisher, ',');
            getline(bs, yearStr, ',');
            getline(bs, ISBNStr, ',');
            getline(bs, statusStr, ',');
            
            if (ISBNStr == bookISBN) {
                found = true;
                // If the book is reserved, create a borrowing entry and update status
                if (statusStr == "Reserved") {
                    string current_date = currentDate();
                    string due_date = addDays(current_date, 15);
                    
                    // Append a new borrowing record in borrowedbook.csv.
                    ofstream foutBorrowed("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv", ios::app);
                    if (!foutBorrowed) {
                        cout << "Error: Unable to open borrowedbook.csv" << endl;
                        return;
                    }
                    // Format: userID,bookISBN,borrowed_date,due_date,return_date,fine
                    foutBorrowed << userid << "," << bookISBN << "," 
                                << current_date << "," << due_date 
                                << ",0" << "\n";
                    foutBorrowed.close();
                    
                    // Clear the reservation by updating the book status to "Available".
                    statusStr = "Available";
                }
                // If the book is "Unavailable", simply mark it as "Available" (e.g., on return).
                else if (statusStr == "Unavailable") {
                    statusStr = "Available";
                }
                
                // Rebuild the book record line with the updated status.
                ostringstream bos;
                bos << title << "," << author << "," << publisher << "," 
                    << yearStr << "," << ISBNStr << "," << statusStr;
                bookLines[i] = bos.str();
                break;
            }
        }
        
        // Write the updated book records back to books.csv.
        ofstream foutBooks("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\books.csv");
        if (!foutBooks) {
            cout << "Error: Unable to write to books.csv" << endl;
            return;
        }
        for (const auto &l : bookLines) {
            foutBooks << l << "\n";
        }
        foutBooks.close();
        
        cout << "Book status processed successfully." << endl;
    }

            class Students  : public user {
                public:
                    void show_option(string userid,usertype user_type) override {
                        const int numOptions = 5;
                        string options[numOptions] = {
                            "Borrow a Book",
                            "Return a Book",
                            "View Borrowed Book and Fine",
                            "Pay the Fine",
                            "Exit"
                        };
                        
                        int selected = 0;
                        while (true) {
                            system("cls");
                            cout << "Student's Option Menu" << endl << endl;
                            for (int i = 0; i < numOptions; i++) {
                                cout << (i == selected ? " -> " : "    ") << options[i] << endl;
                            }
                            int key = _getch();
                            if (key == 224) {
                                key = _getch();
                                if (key == 72 && selected > 0)
                                    selected--;
                                else if (key == 80 && selected < numOptions - 1)
                                    selected++;
                            } else if (key == 13) {
                                break;
                            }
                        }
                        
                        switch (selected) {
                            case 0: borrowBook(userid); break;
                            case 1: returnBook(userid); break;
                            case 2: viewBorrowedBookAndFine(userid); break;
                            case 3: payFine(); break;
                            case 4:exit();
                            default: cout << "Invalid selection" << endl; break;
                        }
                    }
        // Borrow book function with fine check.
                    void borrowBook(string userid) {
                        system("cls");
                        // Check if user has outstanding fines.
                        int userFine = see_user_fine(userid);
                        if (userFine < 0) {
                            cout << "User record not found." << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                        }
                        if (userFine != 0) {
                            cout << "You have an outstanding fine of Rs. " << userFine 
                                << ". Please pay your fine before borrowing a new book." << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                        }
                        // Check active borrowed count from borrowedbook.csv.
                        int activeBorrows = 0;
                        ifstream finBorrowed("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv");
                        if (finBorrowed) {
                            string line;
                            while (getline(finBorrowed, line)) {
                                stringstream ss(line);
                                string recordUser, bookISBN, borrowDate, dueDate, returnDate, fine;
                                getline(ss, recordUser, ',');
                                getline(ss, bookISBN, ',');
                                getline(ss, borrowDate, ',');
                                getline(ss, dueDate, ',');
                                getline(ss, returnDate, ',');
                                getline(ss, fine, ',');
                                if (recordUser == userid && returnDate == "") {
                                    activeBorrows++;
                                }
                            }
                            finBorrowed.close();
                        }
                        if (activeBorrows >= 3) {
                            cout << "You have already borrowed " << activeBorrows 
                                << " books. You cannot borrow more than 3 books at a time." << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                        }
                        // Continue with borrowing: prompt for book ISBN.
                        string bookISBN;
                        cout << "Enter the ISBN of the book you want to borrow: ";
                        cin >> bookISBN;
        // Check if book is available.
                        ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\books.csv");
                        if (!fin) {
                            cout << "Error: Unable to open books.csv" << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                        }
                        vector<string> fileLines;
                        string line;
                        bool found = false;
                        bool available = false;
                        int targetLineIndex = -1;
                        while (getline(fin, line)) {
                            fileLines.push_back(line);
                        }
                        fin.close();
                        
                        // Search for the book.
                        for (size_t i = 0; i < fileLines.size(); i++) {
                            stringstream ss(fileLines[i]);
                            string title, author, publisher, yearStr, ISBNStr, statusStr;
                            getline(ss, title, ',');
                            getline(ss, author, ',');
                            getline(ss, publisher, ',');
                            getline(ss, yearStr, ',');
                            getline(ss, ISBNStr, ',');
                            getline(ss, statusStr, ',');
                            
                            if (ISBNStr == bookISBN) {
                                found = true;
                                targetLineIndex = i;
                                if (statusStr == "Available") {
                                    available = true;
                                } else if (statusStr == "Unavailable") {
                                    string reserve;
                                    cout << "Sorry, the book is currently UNAVAILABLE.\n"
                                        << "But you can reserve it. Do you want to reserve the book? (Yes/No): ";
                                    cin >> reserve;
                                    //update 
                                    if (reserve == "YES" || reserve == "yes" || reserve == "Yes") {
                                        string updatedLine = title + "," + author + "," + publisher + "," +
                                                            yearStr + "," + ISBNStr + ",Reserved";
                                        fileLines[i] = updatedLine;
                                        ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\books.csv");
                                        if (!fout) {
                                            cout << "Error: Unable to write to books.csv" << endl;
                                            system("pause");
                                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                                            return;
                                        }
                                        for (const auto &l : fileLines) {
                                            fout << l << "\n";
                                        }
                                        fout.close();
                                        cout << "Book reserved successfully!" << endl;
                                        system("pause");
                                        string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                                        return;
                                    } else {
                                        cout << "Reservation cancelled. Book remains unavailable." << endl;
                                        system("pause");
                                        string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                                        return;
                                    }
                                } else {
                                    cout << "Sorry, the book is currently UNAVAILABLE" << endl;
                                }
                                break;
                            }
                        }
                        if (!found) {
                            // cout << "Book with ISBN " << bookISBN << " not found!" << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        
                        // Update the book record to "Unavailable" if it was available.
                        {
                            stringstream ss(fileLines[targetLineIndex]);
                            string title, author, publisher, yearStr, ISBNStr, statusStr;
                            getline(ss, title, ',');
                            getline(ss, author, ',');
                            getline(ss, publisher, ',');
                            getline(ss, yearStr, ',');
                            getline(ss, ISBNStr, ',');
                            getline(ss, statusStr, ',');
                            string updatedLine = title + "," + author + "," + publisher + "," + yearStr + "," + ISBNStr + ",Unavailable";
                            fileLines[targetLineIndex] = updatedLine;
                        }
                        {
                            ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\books.csv");
                            if (!fout) {
                                cout << "Error: Unable to write to books.csv" << endl;
                                system("pause");
                                string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                                return;
                            }
                            for (const auto &l : fileLines) {
                                fout << l << "\n";
                            }
                            fout.close();
                        }
                        // Record the borrowing transaction in borrowedbook.csv.
                        // Format: userID,bookISBN,borrowed_date,due_date,return_date,fine
                        string current_date = currentDate();
                        string due_date = addDays(current_date, 15);
                        string return_date = "";  // blank for now
                        string fine = "0";
                        ofstream foutBorrowed("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv", ios::app);
                        if (!foutBorrowed) {
                            cout << "Error: Unable to open borrowedbook.csv" << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        foutBorrowed << userid << "," << bookISBN << "," << current_date << "," << due_date 
                                    << "," << return_date << "," << fine << "\n";
                        foutBorrowed.close();
                        
                        cout << "Book borrowed successfully!" << endl;
                        system("pause");
                        string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                        return;
                    }

                    void returnBook(string userid) {
                        system("cls");
                        string bookISBN;
                        cout << "Enter the ISBN of the book you are returning: ";
                        cin >> bookISBN;
                        
                        // Read all records from borrowedbook.csv
                        vector<string> records;
                        ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv");
                        if (!fin) {
                            cout << "Error: Unable to open borrowedbook.csv" << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        string line;
                        while (getline(fin, line)) {
                            records.push_back(line);
                        }
                        fin.close();
                        
                        bool recordFound = false;
                        int targetIndex = -1;
                        int computedFine = 0; // Fine for this transaction
                        string current_date = currentDate();  // Helper function already defined in your code
                        
                        // Look for the record for this user and the provided ISBN where return_date is empty.
                        for (size_t i = 0; i < records.size(); i++) {
                            stringstream ss(records[i]);
                            string recordUser, r_bookISBN, borrowDate, dueDate, returnDate, fineStr;
                            getline(ss, recordUser, ',');
                            getline(ss, r_bookISBN, ',');
                            getline(ss, borrowDate, ',');
                            getline(ss, dueDate, ',');
                            getline(ss, returnDate, ',');
                            getline(ss, fineStr, ',');
                            
                            if (recordUser == userid && r_bookISBN == bookISBN && returnDate == "") {
                                recordFound = true;
                                targetIndex = static_cast<int>(i);
                                
                                // Compute overdue days using the helper function.
                                int overdueDays = daysBetween(dueDate, current_date);
                                if (overdueDays > 0) {
                                    computedFine = overdueDays * 10;  // Rs.10 fine per overdue day
                                }
                                
                                // Build updated record with return_date and computed fine.
                                ostringstream oss;
                                oss << recordUser << "," << r_bookISBN << "," << borrowDate << "," 
                                    << dueDate << "," << current_date << "," << computedFine;
                                records[i] = oss.str();
                                break;
                            }
                        }
                        
                        if (!recordFound) {
                            cout << "No active borrow record found for ISBN " << bookISBN << " for user " << userid << "." << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        
                        // Write updated records back to borrowedbook.csv
                        ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv");
                        if (!fout) {
                            cout << "Error: Unable to write to borrowedbook.csv" << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        for (const auto &rec : records) {
                            fout << rec << "\n";
                        }
                        fout.close();
                        
                        // Update the user's overall fine in users.csv using the helper function.
                        // Here we add the computedFine (if any) to the user's current fine.
                        updateUserFine(userid, computedFine);
                        availabiltychange(bookISBN,userid);
                        
                        cout << "Book returned successfully!" << endl;
                        if (computedFine > 0) {
                            cout << "You have incurred a fine of Rs. " << computedFine << "." << endl;
                        } else {
                            cout << "No fine incurred." << endl;
                        }
                        system("pause");
                        string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            string check2;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check2;
                            if (check2 == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;

                    }
                    
                    void viewBorrowedBookAndFine(string userid) {
                        system("cls");
                    
                        // ----- Step 1: Get the user's fine from users.csv -----
                        int userFine = see_user_fine(userid);
                        cout << "You have a fabulous fine of Rs. " << userFine << "." <<endl;

                        // ----- Step 2: Print active borrowed books from borrowedbook.csv -----
                        see_borrowed_book(userid);
                        
                    }
                    
                    void payFine() {
                        system("cls");
                        string inputUser;
                        cout << "Enter your User ID to confirm fine payment: ";
                        cin >> inputUser;
                        
                        // Check if the entered ID matches the current user's ID.
                        while(inputUser != userid) {
                            cout << "User ID mismatch! Payment cancelled." << endl;
                            cout << "Enter your User ID again to confirm fine payment: ";
                            cin >> inputUser;
                        }
                        
                        // Read all user records from users.csv
                        vector<string> lines;
                        ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");
                        if (!fin) {
                            cout << "Error: Unable to open users.csv" << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        string line;
                        while (getline(fin, line)) {
                            if (!line.empty())
                                lines.push_back(line);
                        }
                        fin.close();
                        
                        bool found = false;
                        // Update the record for the matching user: set fine to 0
                        for (auto &l : lines) {
                            stringstream ss(l);
                            string uid, pass, userType, fineStr;
                            getline(ss, uid, ',');
                            getline(ss, pass, ',');
                            getline(ss, userType, ',');
                            getline(ss, fineStr, ',');
                            if (uid == inputUser) {
                                ostringstream oss;
                                oss << uid << "," << pass << "," << userType << "," << 0;
                                l = oss.str();
                                found = true;
                                break;
                            }
                        }
                        
                        if (!found) {
                            cout << "User not found in users.csv. Payment failed." << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        
                        // Write the updated records back to users.csv
                        ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");
                        if (!fout) {
                            cout << "Error: Unable to write to users.csv" << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        for (const auto &l : lines) {
                            fout << l << "\n";
                        }
                        fout.close();
                        
                        cout << "Fine payment successful. Your fine has been cleared." << endl;
                        system("pause");
                        string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                    }
                   
                    void exit(){
                        cout << "Exiting the system..." << endl;
                        system("Exit");
                    }
                };
                    
            class Faculties  : public user {
                public:
                    void show_option(string userid,usertype user_type) override {
                        const int numOptions = 4;
                        string options[numOptions] = {
                            "Borrow a Book",
                            "Return a Book",
                            "View Borrowed Book ",
                            "Exit"
                        };
                        
                        int selected = 0;
                        while (true) {
                            system("cls");
                            cout << "Student's Option Menu" << endl << endl;
                            for (int i = 0; i < numOptions; i++) {
                                cout << (i == selected ? " -> " : "    ") << options[i] << endl;
                            }
                            int key = _getch();
                            if (key == 224) {
                                key = _getch();
                                if (key == 72 && selected > 0)
                                    selected--;
                                else if (key == 80 && selected < numOptions - 1)
                                    selected++;
                            } else if (key == 13) {
                                break;
                            }
                        }
                        
                        switch (selected) {
                            case 0: borrowBook(userid); break;
                            case 1: returnBook(userid); break;
                            case 2: viewBorrowedBookAndFine(userid); break;
                            
                            default: cout << "Invalid selection" << endl; break;
                        }
                    }
        // Borrow book function with fine check.
                    void borrowBook(string userid) {
                        system("cls");
                        // Check if user has outstanding fines.
                        bool overdue = findoverdue(userid);
                        if (overdue) {
                            cout << "You have a book which is in overdue preiod. "  
                                << ". Please Return the book before borrowing a new book." << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        // Check active borrowed count from borrowedbook.csv.
                        int activeBorrows = 0;
                        ifstream finBorrowed("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv");
                        if (finBorrowed) {
                            string line;
                            while (getline(finBorrowed, line)) {
                                stringstream ss(line);
                                string recordUser, bookISBN, borrowDate, dueDate, returnDate, fine;
                                getline(ss, recordUser, ',');
                                getline(ss, bookISBN, ',');
                                getline(ss, borrowDate, ',');
                                getline(ss, dueDate, ',');
                                getline(ss, returnDate, ',');
                                getline(ss, fine, ',');
                                if (recordUser == userid && returnDate == "") {
                                    activeBorrows++;
                                }
                            }
                            finBorrowed.close();
                        }
                        if (activeBorrows >= 5) {
                            cout << "You have already borrowed " << activeBorrows 
                                << " books. You cannot borrow more than 5 books at a time." << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        string bookISBN;
                        cout << "Enter the ISBN of the book you want to borrow: ";
                        cin >> bookISBN;
// Check if book is available.
                        ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\books.csv");
                        if (!fin) {
                            cout << "Error: Unable to open books.csv" << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        vector<string> fileLines;
                        string line;
                        bool found = false;
                        bool available = false;
                        int targetLineIndex = -1;
                        while (getline(fin, line)) {
                            fileLines.push_back(line);
                        }
                        fin.close();
                        
                        // Search for the book.
                        for (size_t i = 0; i < fileLines.size(); i++) {
                            stringstream ss(fileLines[i]);
                            string title, author, publisher, yearStr, ISBNStr, statusStr;
                            getline(ss, title, ',');
                            getline(ss, author, ',');
                            getline(ss, publisher, ',');
                            getline(ss, yearStr, ',');
                            getline(ss, ISBNStr, ',');
                            getline(ss, statusStr, ',');
                            
                            if (ISBNStr == bookISBN) {
                                found = true;
                                targetLineIndex = i;
                                if (statusStr == "Available") {
                                    available = true;
                                } else if (statusStr == "Unavailable") {
                                    string reserve;
                                    cout << "Sorry, the book is currently UNAVAILABLE.\n"
                                        << "But you can reserve it. Do you want to reserve the book? (Yes/No): ";
                                    cin >> reserve;
                                    //update 
                                    if (reserve == "YES" || reserve == "yes" || reserve == "Yes") {
                                        string updatedLine = title + "," + author + "," + publisher + "," +
                                                            yearStr + "," + ISBNStr + ",Reserved";
                                        fileLines[i] = updatedLine;
                                        ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\books.csv");
                                        if (!fout) {
                                            cout << "Error: Unable to write to books.csv" << endl;
                                            system("pause");
                                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                                            return;
                                        }
                                        for (const auto &l : fileLines) {
                                            fout << l << "\n";
                                        }
                                        fout.close();
                                        cout << "Book reserved successfully!" << endl;
                                        system("pause");
                                        string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                                        return;
                                    } else {
                                        cout << "Reservation cancelled. Book remains unavailable." << endl;
                                        system("pause");
                                        string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                                        return;
                                    }
                                } else {
                                    cout << "Sorry, the book is currently UNAVAILABLE" << endl;
                                }
                                break;
                            }
                        }
                        if (!found) {
                            // cout << "Book with ISBN " << bookISBN << " not found!" << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        
                        // Update the book record to "Unavailable" if it was available.
                        {
                            stringstream ss(fileLines[targetLineIndex]);
                            string title, author, publisher, yearStr, ISBNStr, statusStr;
                            getline(ss, title, ',');
                            getline(ss, author, ',');
                            getline(ss, publisher, ',');
                            getline(ss, yearStr, ',');
                            getline(ss, ISBNStr, ',');
                            getline(ss, statusStr, ',');
                            string updatedLine = title + "," + author + "," + publisher + "," + yearStr + "," + ISBNStr + ",Unavailable";
                            fileLines[targetLineIndex] = updatedLine;
                        }
                        {
                            ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\books.csv");
                            if (!fout) {
                                cout << "Error: Unable to write to books.csv" << endl;
                                system("pause");
                                string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                                return;
                            }
                            for (const auto &l : fileLines) {
                                fout << l << "\n";
                            }
                            fout.close();
                        }
            // Record the borrowing transaction 
                        string current_date = currentDate();
                        string due_date = addDays(current_date, 15);
                        string return_date = "";  // blank for now
                        string fine = "0";
                        ofstream foutBorrowed("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv", ios::app);
                        if (!foutBorrowed) {
                            cout << "Error: Unable to open borrowedbook.csv" << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        foutBorrowed << userid << "," << bookISBN << "," << current_date << "," << due_date 
                                    << "," << return_date << "," << fine << "\n";
                        foutBorrowed.close();
                        
                        cout << "Book borrowed successfully!" << endl;
                        system("pause");
                        string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                        string check2;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check2;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                        return;
                    }
        //return the book 
                    void returnBook(string userid) {
                        system("cls");
                        string bookISBN;
                        cout << "Enter the ISBN of the book you are returning: ";
                        cin >> bookISBN;
                        
                        // Read all records from borrowedbook.csv
                        vector<string> records;
                        ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv");
                        if (!fin) {
                            cout << "Error: Unable to open borrowedbook.csv" << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        string line;
                        while (getline(fin, line)) {
                            records.push_back(line);
                        }
                        fin.close();
                        
                        bool recordFound = false;
                        int targetIndex = -1;
                        int computedFine = 0;
                        string current_date = currentDate(); 
                        
                        // Look for the record for this user and the provided ISBN where return_date is empty.
                        for (size_t i = 0; i < records.size(); i++) {
                            stringstream ss(records[i]);
                            string recordUser, r_bookISBN, borrowDate, dueDate, returnDate, fineStr;
                            getline(ss, recordUser, ',');
                            getline(ss, r_bookISBN, ',');
                            getline(ss, borrowDate, ',');
                            getline(ss, dueDate, ',');
                            getline(ss, returnDate, ',');
                            getline(ss, fineStr, ',');
                            
                            if (recordUser == userid && r_bookISBN == bookISBN && returnDate == "") {
                                recordFound = true;
                                targetIndex = static_cast<int>(i);

                                // Build updated record with return_date and computed fine.
                                ostringstream oss;
                                oss << recordUser << "," << r_bookISBN << "," << borrowDate << "," 
                                    << dueDate << "," << current_date << "," << "0";
                                records[i] = oss.str();
                                break;
                            }
                        }
                        
                        if (!recordFound) {
                            cout << "No active borrow record found for ISBN " << bookISBN << " for user " << userid << "." << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        
                        // Write updated records back to borrowedbook.csv
                        ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\borrowedbook.csv");
                        if (!fout) {
                            cout << "Error: Unable to write to borrowedbook.csv" << endl;
                            system("pause");
                            string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                            return;
                        }
                        for (const auto &rec : records) {
                            fout << rec << "\n";
                        }
                        fout.close();
                        
                        // Update the user's overall fine in users.csv using the helper function.
                        availabiltychange(bookISBN,userid);

                        cout << "Book returned successfully!" << endl;
                        system("pause");
                        string check;
                            cout << "Do you want to back to menu option:(Yes/No)";
                            cin >> check;
                            if (check == "Yes" || "yes" || "YES") show_option(userid,user_type);
                            else exit();
                            return;
                    }
                    
                    void viewBorrowedBookAndFine(string userid) {
                        system("cls");
                        see_borrowed_book(userid);   
                    }                   
                    
                    void exit(){
                        cout << "Exiting the System ..."
                        << endl;
                        system("exit");
                    }
                };
                            
            class Librarians : public user {
                public:
                    void show_option(string userid,usertype user_type) override {
                        const int numOptions = 5;
                        string options[numOptions] = {
                            "Add a User",
                            "Remove a User",
                            "Add a Book",
                            "Remove a Book"
                            "exit"
                        };                     
                        int selected = 0;
                        while (true) {
                            system("cls");
                            cout << "Librarian's Option Menu" << endl << endl;
                            for (int i = 0; i < numOptions; i++) {
                                cout << (i == selected ? " -> " : "    ") << options[i] << endl;
                            }
                            int key = _getch();
                            if (key == 224) {
                                key = _getch();
                                if (key == 72 && selected > 0)
                                    selected--;
                                else if (key == 80 && selected < numOptions - 1)
                                    selected++;
                            } else if (key == 13) {
                                break;
                            }
                        }
                        
                    }
        // Borrow book function with fine check.
                    void addUser(){
                        cout<< "Type the details of the user you want to add"<<endl;
                        string userid,password;
                        string user_type_int;
                        cout << "Userid:";
                        cin >> userid;
                        cout << "\nPassword:";
                        cin >> password;
                        cout << "User Type(Student / Faculty / Librarian):";
                        cin >> user_type_int;
                        usertype user_type;
                        if(user_type_int == "Student")user_type = Student;
                        else if(user_type_int == "Faculty")user_type = Faculty;
                        else if(user_type_int == "Librarian"){
                            cout<<"YOU CAN NOT ADD A LIBRARIAN , LIBRARY CAN HAVE ONLY ONE LIBRARIAN AND THAT IS ME YOU SILLY FELLOW";
                            return;
                        }
                        
                        ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv", ios::app);
                        if (!fout) {
                            cout << "Error: Unable to open users.csv" << endl;
                            return;
                        }
                        fout << userid << "," << password << "," << user_type << ",0" << "\n";
                        fout.close();
                        cout << "User " << userid << " added successfully." << endl;
                    }
                    
                    void removeUser(){

                        cout << "Type the details of the user you want to remove" << endl;
                        string userid, password, user_type_str;
                        cout << "Userid: ";
                        cin >> userid;
                        cout << "Password: ";
                        cin >> password;
                        cout << "User Type (Student / Faculty / Librarian): ";
                        cin >> user_type_str;
                        
                        // Prevent removal of a librarian
                        if(user_type_str == "Librarian") {
                            cout << "YOU CAN NOT REMOVE A LIBRARIAN. LIBRARY CAN HAVE ONLY ONE LIBRARIAN AND THAT IS ME, YOU SILLY FELLOW." << endl;
                            return;
                        }
                        ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");
                        if (!fin) {
                            cout << "Error: Unable to open users.csv" << endl;
                            return;
                        }

                        vector<string> lines;
                        string line;
                        bool userFound = false;
                        
                        while(getline(fin, line)) {
                            if(line.empty()) continue;
                            
                            // Parse the record assuming format: userid,password,usertype,fine
                            stringstream ss(line);
                            string uid, pass, utype, fine;
                            getline(ss, uid, ',');
                            getline(ss, pass, ',');
                            getline(ss, utype, ',');
                            getline(ss, fine, ',');
                            
                            // If this record matches the details of the user to remove, skip it.
                            if(uid == userid && pass == password && utype == user_type_str) {
                                userFound = true;
                            } else {
                                lines.push_back(line);
                            }
                        }
                        fin.close();
                        
                        if(!userFound) {
                            cout << "User " << userid << " not found in users.csv." << endl;
                            return;
                        }
                        // Write the updated list back to users.csv
                        ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");
                        if (!fout) {
                            cout << "Error: Unable to open users.csv for writing." << endl;
                            return;
                        }                        
                        for(const auto &record : lines) {
                            fout << record << "\n";
                        }
                        fout.close();
                        cout << "User " << userid << " removed successfully." << endl;
                    }
                    
                    void addBook(){
                        cout<< "Type the ISBN number of the BOOK you want to add"<<endl;
                        string book_name,author,publisher,year,ISBN;
                        cout << "Book Name";
                        cin >> book_name;
                        cout << "\nAuthor Name:";
                        cin >> author;
                        cout << "\nPusblisher Name:";
                        cin >> publisher;
                        cout << "\nEdition Year";
                        cin >> year;
                        cout << "\nISBN:";
                        cin >> ISBN;

                        
                        ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv", ios::app);
                        if (!fout) {
                            cout << "Error: Unable to open users.csv" << endl;
                            return;
                        }
                        fout << book_name << "," << author << "," << publisher << "," <<year<<","<< ISBN <<",Available\n";
                        fout.close();
                        cout << "User " << userid << " added successfully." << endl;
                    }   
                    
                    void removeBook(){
                        cout << "Type the details of the BOOK you want to remove" << endl;
                        string ISBN;
                        cout << "ISBN of the book: ";
                        cin >> ISBN;
                        
                        ifstream fin("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");
                        if (!fin) {
                            cout << "Error: Unable to open users.csv" << endl;
                            return;
                        }

                        vector<string> lines;
                        string line;
                        bool userFound = false;
                        string book_name,author,publisher,year,isbn,status;
                        while(getline(fin, line)) {
                            if(line.empty()) continue;
                            
                            // Parse the record assuming format: userid,password,usertype,fine
                            stringstream ss(line);
                            getline(ss, book_name, ',');
                            getline(ss, author, ',');
                            getline(ss, publisher, ',');
                            getline(ss, year, ',');
                            getline(ss, isbn, ',');
                            getline(ss, status);
                            
                            // If this record matches the details of the user to remove, skip it.
                            if(isbn == ISBN) {
                                userFound = true;
                            } else {
                                lines.push_back(line);
                            }
                        }
                        fin.close();
                        
                        if(!userFound) {
                            cout << "Book " << book_name << " with ISBN "<<ISBN<<" not found." << endl;
                            return;
                        }
                        // Write the updated list back to users.csv
                        ofstream fout("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");
                        if (!fout) {
                            cout << "Error: Unable to open users.csv for writing." << endl;
                            return;
                        }                        
                        for(const auto &record : lines) {
                            fout << record << "\n";
                        }
                        fout.close();
                        cout << "Book " << book_name << " with ISBN "<<ISBN<<" is removed." << endl;
                    } 
                    
                    void exit(){
                        cout << "Exiting the System ..."<<endl;
                        system("exit");
                    }
                };
               
class book{
    private :
    string title, author, publisher; 
    int year;
    int ISBN;
    protected:
    status st ;
    public:
    book() : title(""), author(""), publisher(""),year(2000),st(Available) {}

    book(string t, string a, string p, int y,status s)
    : title(t), author(a), year(y),publisher(p),st(s) {}


};

class Account {
    private:
        string userid;
        string password;
        
    protected:
        usertype type;
        
    public:
        Account() : userid(""), password(""), type(Student) {}
        Account(string uid, string pass, usertype utype)
            : userid(uid), password(pass), type(utype) {}    
        
        // Pass the stored userid from the Account to the respective object.
        void showoption(usertype user_type, bool check) {
            if (!check) {
                cout << "No user found";
            } else {
                if (user_type == Student) {
                    Students s;
                    s.userid = this->userid;
                    s.show_option(s.userid, s.user_type);
                } else if (user_type == Faculty) {
                    Faculties f;
                    f.userid = this->userid;
                    f.show_option(f.userid, f.user_type);
                } else if (user_type == Librarian) {
                    Librarians l;
                    l.userid = this->userid;
                    l.show_option(l.userid, l.user_type);
                }
            }
        }
    };
    // Function to move the cursor
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
// Function to select the user type
usertype knowuser_type() {
    const int numOptions = 3;
    string user_types[numOptions] = {"Student", "Faculty", "Librarian"};
    int selected = 0;
    
    while (true) {
        system("cls");
        cout << "Welcome to PK Kelkar Library" << endl;
        cout << "Please fill in the following details" << endl;
        cout << "You are a:" ;
        cout << "\n\n";
        
        for (int i = 0; i < numOptions; i++) {
            if (i == selected)
                cout << " -> ";  // Highlight selected option
            else
                cout << "    ";
            
            cout << user_types[i] << endl;
        }

        int key = _getch();
        if (key == 224) { // Arrow keys return two key codes
            key = _getch();
            if (key == 72 && selected > 0) selected--; // Up arrow
            if (key == 80 && selected < numOptions - 1) selected++; // Down arrow
        } else if (key == 13) { // Enter key
            return static_cast<usertype>(selected); // Correctly return the usertype
        }
    }
}
// function to get the login details
pair<string,string> get_details(usertype user_type) {
    string user_type_str[] = {"Student", "Faculty", "Librarian"};
    cout << "So you are a " << user_type_str[user_type]<<endl;

    string userid;
    string password;
    cout << "Enter your User ID: ";
    cin >> userid;
    cout << "Enter your Password: ";
    cin >> password;

    return make_pair(userid,password);
}

bool auth(pair<string, string> user_login,usertype user_type) {
    ifstream file("C:\\Users\\mohit\\OneDrive\\Desktop\\LMS\\users.csv");


    if (!file) {
        cout << "Error: Unable to open users.csv. File may not exist!" << endl;
        return false;
    }

    string line;  
    string stored_userid, stored_password;
    string stored_user_type;
    string fine;

    string user_type_st;

    switch (user_type){
        case 0:
        user_type_st = "Student" ;
        break;
        case 1:
        user_type_st = "Faculty" ;
        break;
        case 2:
        user_type_st = "Librarian" ;
        break;
    }

    while (getline(file, line)) {  
        stringstream ss(line);
        getline(ss, stored_userid, ',');
        getline(ss, stored_password, ',');
        getline(ss, stored_user_type, ',');
        getline(ss, fine);


        if (user_login.first == stored_userid && user_login.second == stored_password && user_type_st == stored_user_type && stoi(fine)>=0 ) {
            return true;  
        }
    }
    cout << "Sorry , UserId or password are incorrect"<<endl;
    return false;  
}

int main() {
    usertype user_type = knowuser_type();
    // Print the selected user type as a string
    string user_type_str[] = {"Student", "Faculty", "Librarian"};
    // call for getting the login details 
    pair<string,string> user_login = get_details(user_type);
    // now a function to check the login details are correct or not 
    bool check = auth(user_login,user_type);
    // showing option for the user and selection of the option 
    Account acc(user_login.first,user_login.second,user_type);
    acc.showoption(user_type,check);
    return 0;
}
