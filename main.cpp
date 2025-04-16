#include <cstdio>        // Replaced <stdio.h>
#include <windows.h>     // Windows-specific header
#include <cstdlib>       // Replaced <stdlib.h>
#include <conio.h>       // Windows-specific (for _getch, etc.)
#include <ctime>         // Replaced <time.h>
#include <cstring>       // Replaced <string.h>
#include <cstdarg>       // Replaced <stdarg.h>
#include <regex>

using namespace std;

// Structures
struct adddata {
    char si[20];
    char train_number[20];
    char train_name[50];
    char start[30];
    char destination[30];
    struct {
        char price[20];
        int seat;
        int total_seats;
    } business, economical, sleeper;
    char departure_date[20];
    char departure_time[10];
} add[1000];

// Predefined train list with prices in EUR
struct trainlist {
    char si[20];
    char train_number[20];
    char train_name[50];
    char start[30];
    char destination[30];
    struct {
        char price[20];
        int total_seats;
    } business, economical, sleeper;
    char departure_date[20];
    char departure_time[10];
} train_list[] = {
    {"001", "T123", "Express", "CityA", "CityB", {"EUR100", 20}, {"EUR50", 30}, {"EUR30", 50}, "2025-04-18", "08:00"},
    {"002", "T456", "Superfast", "CityB", "CityC", {"EUR120", 15}, {"EUR60", 25}, {"EUR35", 40}, "2025-04-19", "10:30"},
    {"003", "T789", "Intercity", "CityA", "CityC", {"EUR140", 10}, {"EUR70", 20}, {"EUR40", 30}, "2025-04-20", "14:00"},
    {"004", "T101", "Night Express", "CityC", "CityA", {"EUR110", 25}, {"EUR55", 35}, {"EUR33", 60}, "2025-04-21", "22:00"},
    {"005", "T202", "Morning Star", "CityA", "CityD", {"EUR130", 18}, {"EUR65", 27}, {"EUR38", 45}, "2025-04-22", "06:45"},
    {"006", "T303", "Golden Arrow", "CityD", "CityB", {"EUR150", 12}, {"EUR75", 23}, {"EUR45", 35}, "2025-04-23", "12:15"},
    {"007", "T404", "Metro Link", "CityB", "CityE", {"EUR80", 10}, {"EUR40", 15}, {"EUR25", 25}, "2025-04-24", "09:00"},
    {"008", "T505", "Sunset Limited", "CityE", "CityA", {"EUR180", 8}, {"EUR90", 12}, {"EUR55", 20}, "2025-04-25", "18:30"},
    {"009", "T606", "Coastal Ride", "CityC", "CityD", {"EUR160", 10}, {"EUR80", 18}, {"EUR48", 28}, "2025-04-26", "13:45"},
    {"010", "T707", "Hill Runner", "CityD", "CityE", {"EUR110", 20}, {"EUR55", 30}, {"EUR33", 50}, "2025-04-27", "07:20"}
};
int TRAIN_LIST_SIZE = sizeof(train_list) / sizeof(train_list[0]);

struct bookticket {
    char train_number[20];
    char name[50];
    char phone[20];
    char date[20];
    int seat;
    char email[50];
    char class_type[20];
} book[1000];

// Global variables
int k = 0, u = 0;
HANDLE hConsole;

// Function declarations
void clearInputBuffer();
int getIntInput(const char* format, int minVal, int maxVal, ...);
void displayHeader(const char* title);
void displayMenuWithHighlight(const char* options[], int numOptions, int selected);
void viewinfo();
void bookticket();
void cancelticket();
void admin();
void password();
void viewpassenger();
void addtrain();
void dlttrain();
void modifyTrain();
void awrite();
void aread();
void bookticket_write();
void viewpassengers_read();
void searchTrain();
void printEmailTicket();
bool validatePhone(const char* phone);
bool validateDate(const char* date);
bool validateTrainNumber(const char* train_num);
bool validateEmail(const char* email);
bool isFutureDate(const char* date);
bool validateTime(const char* time);

// Validation Functions
bool validatePhone(const char* phone) {
    regex phoneRegex("^[0-9]{10}$");
    return regex_match(phone, phoneRegex);
}

bool validateDate(const char* date) {
    regex dateRegex("^[0-9]{4}-[0-1][0-9]-[0-3][0-9]$");
    if (!regex_match(date, dateRegex)) return false;

    int year, month, day;
    std::sscanf(date, "%d-%d-%d", &year, &month, &day);
    if (month < 1 || month > 12 || day < 1 || day > 31) return false;
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
    if (month == 2) {
        bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (day > (leap ? 29 : 28)) return false;
    }
    return true;
}

bool isFutureDate(const char* date) {
    std::time_t now = std::time(nullptr);
    struct tm today = *std::localtime(&now);
    char currentDate[20];
    std::strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", &today);

    return std::strcmp(date, currentDate) >= 0;
}

bool validateTrainNumber(const char* train_num) {
    regex trainRegex("^[T][0-9]{3}$");
    return regex_match(train_num, trainRegex);
}

bool validateEmail(const char* email) {
    if (std::strlen(email) == 0) return true; // Email is optional
    regex emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return regex_match(email, emailRegex);
}

bool validateTime(const char* time) {
    regex timeRegex("^[0-2][0-9]:[0-5][0-9]$");
    if (!regex_match(time, timeRegex)) return false;

    int hours, minutes;
    std::sscanf(time, "%d:%d", &hours, &minutes);
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) return false;
    
    return true;
}

void displayMenuWithHighlight(const char* options[], int numOptions, int selected) {
    std::printf("\n");
    for (int i = 0; i < numOptions; i++) {
        if (i + 1 == selected) {
            SetConsoleTextAttribute(hConsole, 0x0A); // Green text
            std::printf("  > %s\n", options[i]);
            SetConsoleTextAttribute(hConsole, 0x1F); // Reset to default
        } else {
            std::printf("    %s\n", options[i]);
        }
    }
    std::printf("\n");
    Sleep(500); // Brief pause to show highlight
}

void showWelcomeScreen() {
    system("cls");
    const char* title = "Welcome to Railway Reservation System";
    int consoleWidth = 50;
    int titleLength = std::strlen(title);
    int padding = (consoleWidth - titleLength) / 2;

    SetConsoleTextAttribute(hConsole, 0x0E);
    std::printf("\n\n\n");
    for (int i = 0; i < padding; i++) std::printf(" ");
    for (int i = 0; title[i]; i++) {
        putchar(title[i]);
        std::fflush(stdout);
        Sleep(50);
    }
    std::printf("\n\n");
    SetConsoleTextAttribute(hConsole, 0x1F);

    const char spinner[] = "|/-\\";
    std::printf("%*sInitializing system ", padding, "");
    for (int i = 0; i < 20; i++) {
        std::printf("\b%c", spinner[i % 4]);
        std::fflush(stdout);
        Sleep(100);
    }
    std::printf("\b \n");

    Sleep(500);
    system("cls");
}

int main() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleOutputCP(65001); // Set console to UTF-8 for potential future use
    system("COLOR 1F");

    showWelcomeScreen();
    aread();
    viewpassengers_read();

    std::time_t currentTime;
    std::time(&currentTime);
    char* timeStr = std::ctime(&currentTime);
    timeStr[std::strlen(timeStr)-1] = '\0';

    while (1) {
        system("cls");
        displayHeader("RAILWAY RESERVATION SYSTEM");
        std::printf("\n  System Started: %s\n", timeStr);
        std::printf("  ==================================================\n\n");

        const char* mainMenu[] = {
            "1. View All Trains",
            "2. Search Train",
            "3. Book Ticket",
            "4. Cancel Ticket",
            "5. Print & Email Ticket",
            "6. Admin Portal",
            "7. Exit"
        };
        std::printf("\n  ------------------ MAIN PORTAL ------------------\n");
        for (int i = 0; i < 7; i++) {
            std::printf("    %s\n", mainMenu[i]);
        }
        std::printf("  -----------------------------------------------\n");

        int choice = getIntInput("  Enter choice (1-7): ", 1, 7);
        displayMenuWithHighlight(mainMenu, 7, choice);

        switch (choice) {
            case 1: viewinfo(); break;
            case 2: searchTrain(); break;
            case 3: bookticket(); break;
            case 4: cancelticket(); break;
            case 5: printEmailTicket(); break;
            case 6: password(); break;
            case 7:
                SetConsoleTextAttribute(hConsole, 0x0A);
                std::printf("  Exiting system. Thank you!\n");
                SetConsoleTextAttribute(hConsole, 0x1F);
                Sleep(1000);
                exit(0);
        }
    }
    return 0;
}

void displayHeader(const char* title) {
    std::printf("\n");
    SetConsoleTextAttribute(hConsole, 0x1F);
    std::printf("  ==================================================\n");
    std::printf("  %s\n", title);
    std::printf("  ==================================================\n");
    SetConsoleTextAttribute(hConsole, 0x1F);
}

void clearInputBuffer() {
    while (getchar() != '\n');
}

int getIntInput(const char* format, int minVal, int maxVal, ...) {
    char prompt[100];
    va_list args;
    va_start(args, maxVal);
    std::vsnprintf(prompt, sizeof(prompt), format, args);
    va_end(args);

    int value;
    while (1) {
        std::printf("%s", prompt);
        if (std::scanf("%d", &value) != 1) {
            clearInputBuffer();
            SetConsoleTextAttribute(hConsole, 0x0C);
            std::printf("  Invalid input! Please enter a number.\n");
            SetConsoleTextAttribute(hConsole, 0x1F);
            Sleep(1000);
            continue;
        }
        clearInputBuffer();
        if (value >= minVal && value <= maxVal) {
            break;
        }
        SetConsoleTextAttribute(hConsole, 0x0C);
        std::printf("  Invalid choice! Please enter a number between %d and %d.\n", minVal, maxVal);
        SetConsoleTextAttribute(hConsole, 0x1F);
        Sleep(1000);
    }
    return value;
}

void viewinfo() {
    system("cls");
    aread();
    displayHeader("TRAIN INFORMATION");

    if (k == 0) {
        std::printf("  No trains available.\n");
    } else {
        std::printf("\n  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-10s %-10s %-6s\n",
               "SI", "Train No", "Train Name", "Start", "Dest", "Class", "Price", "Seats(Avail/Total)", "Date", "Time");
        std::printf("  ----------------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < k; i++) {
            std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-10d/%-6d %-10s %-6s\n",
                   add[i].si, add[i].train_number, add[i].train_name, add[i].start, add[i].destination,
                   "Business", add[i].business.price, add[i].business.seat, add[i].business.total_seats,
                   add[i].departure_date, add[i].departure_time);
            std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-10d/%-6d %-10s %-6s\n",
                   "", "", "", "", "", "Economical", add[i].economical.price, add[i].economical.seat,
                   add[i].economical.total_seats, "", "");
            std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-10d/%-6d %-10s %-6s\n",
                   "", "", "", "", "", "Sleeper", add[i].sleeper.price, add[i].sleeper.seat,
                   add[i].sleeper.total_seats, "", "");
        }
        std::printf("  ----------------------------------------------------------------------------------------------------\n");
    }

    std::printf("\n");
    getIntInput("  Press 1 to return to main portal: ", 1, 1);
}

void bookticket() {
    int j, n, i, found, seatChoice;
    char bookingDate[20], trainNum[20], email[50], classChoice[20];
    system("cls");
    displayHeader("BOOK TICKET");

    const char* ticketMenu[] = {
        "1. Single Ticket",
        "2. Group Ticket"
    };
    std::printf("\n  Select ticket type:\n");
    for (int i = 0; i < 2; i++) {
        std::printf("    %s\n", ticketMenu[i]);
    }
    int ticketType = getIntInput("  Enter choice (1-2): ", 1, 2);
    displayMenuWithHighlight(ticketMenu, 2, ticketType);

    if (ticketType == 1) {
        n = 1;
    } else {
        n = getIntInput("  How many tickets for the group? ", 1, 1000 - u);
    }

    if (n <= 0 || n > 1000 - u) {
        SetConsoleTextAttribute(hConsole, 0x0C);
        std::printf("  Invalid number of tickets! Must be positive and within capacity.\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
        Sleep(1000);
        return;
    }

    for (j = 0; j < n; j++) {
        found = -1;
        std::printf("\n  Ticket %d\n", j + 1);
        std::printf("  ---------\n");
        while (1) {
            std::printf("  Enter train number (e.g., T123): ");
            std::scanf("%19s", trainNum);
            if (validateTrainNumber(trainNum)) break;
            SetConsoleTextAttribute(hConsole, 0x0C);
            std::printf("  Invalid train number! Must be format T followed by 3 digits.\n");
            SetConsoleTextAttribute(hConsole, 0x1F);
        }
        std::strcpy(book[u + j].train_number, trainNum);

        while (1) {
            std::printf("  Enter travel date (YYYY-MM-DD): ");
            std::scanf("%19s", bookingDate);
            if (validateDate(bookingDate) && isFutureDate(bookingDate)) break;
            SetConsoleTextAttribute(hConsole, 0x0C);
            std::printf("  Invalid date or date in the past! Use YYYY-MM-DD format.\n");
            SetConsoleTextAttribute(hConsole, 0x1F);
        }

        const char* classMenu[] = {
            "1. BusinessCouch",
            "2. EconomicalCouch",
            "3. SleeperCouch"
        };
        std::printf("  Select class:\n");
        for (int i = 0; i < 3; i++) {
            std::printf("    %s\n", classMenu[i]);
        }
        int classIndex = getIntInput("  Enter choice (1-3): ", 1, 3);
        displayMenuWithHighlight(classMenu, 3, classIndex);

        if (classIndex == 1) std::strcpy(classChoice, "BusinessCouch");
        else if (classIndex == 2) std::strcpy(classChoice, "EconomicalCouch");
        else std::strcpy(classChoice, "SleeperCouch");
        std::strcpy(book[u + j].class_type, classChoice);

        for (i = 0; i < k; i++) {
            if (std::strcmp(book[u + j].train_number, add[i].train_number) == 0 && std::strcmp(bookingDate, add[i].departure_date) == 0) {
                int availableSeats = (classIndex == 1) ? add[i].business.seat :
                                    (classIndex == 2) ? add[i].economical.seat : add[i].sleeper.seat;
                int totalSeats = (classIndex == 1) ? add[i].business.total_seats :
                                (classIndex == 2) ? add[i].economical.total_seats : add[i].sleeper.total_seats;
                if (availableSeats == 0) {
                    SetConsoleTextAttribute(hConsole, 0x0C);
                    std::printf("  No available seats for %s class on this date!\n", classChoice);
                    SetConsoleTextAttribute(hConsole, 0x1F);
                    Sleep(1000);
                    return;
                }
                found = i;
                std::printf("  Enter details for ticket %d:\n", j + 1);
                std::strcpy(book[u + j].date, bookingDate);
                std::printf("  Enter your name: ");
                std::scanf(" %49[^\n]s", book[u + j].name);
                while (1) {
                    std::printf("  Enter your phone number (10 digits): ");
                    std::scanf("%19s", book[u + j].phone);
                    if (validatePhone(book[u + j].phone)) break;
                    SetConsoleTextAttribute(hConsole, 0x0C);
                    std::printf("  Invalid phone number! Must be 10 digits.\n");
                    SetConsoleTextAttribute(hConsole, 0x1F);
                }
                while (1) {
                    std::printf("  Enter your email: ");
                    std::scanf(" %49[^\n]s", email);
                    if (validateEmail(email)) break;
                    SetConsoleTextAttribute(hConsole, 0x0C);
                    std::printf("  Invalid email format!\n");
                    SetConsoleTextAttribute(hConsole, 0x1F);
                }
                std::strcpy(book[u + j].email, email);
                std::printf("  Available %s seats: %d out of %d\n", classChoice, availableSeats, totalSeats);

                bool seatValid = false;
                while (!seatValid) {
                    seatChoice = getIntInput("  Enter seat number (1 to %d): ", 1, totalSeats, totalSeats);
                    if (seatChoice < 1 || seatChoice > totalSeats) {
                        SetConsoleTextAttribute(hConsole, 0x0C);
                        std::printf("  Invalid seat number! Must be between 1 and %d.\n", totalSeats);
                        SetConsoleTextAttribute(hConsole, 0x1F);
                        Sleep(1000);
                        continue;
                    }

                    bool seatTaken = false;
                    for (int b = 0; b < u; b++) {
                        if (std::strcmp(book[b].train_number, book[u + j].train_number) == 0 &&
                            std::strcmp(book[b].date, book[u + j].date) == 0 &&
                            std::strcmp(book[b].class_type, book[u + j].class_type) == 0 &&
                            book[b].seat == seatChoice) {
                            seatTaken = true;
                            break;
                        }
                    }

                    if (seatTaken) {
                        SetConsoleTextAttribute(hConsole, 0x0C);
                        std::printf("  Seat %d is already booked for %s class on this date! Please choose another seat.\n", seatChoice, classChoice);
                        SetConsoleTextAttribute(hConsole, 0x1F);
                        Sleep(1000);
                    } else {
                        seatValid = true;
                        book[u + j].seat = seatChoice;
                        if (classIndex == 1) add[i].business.seat--;
                        else if (classIndex == 2) add[i].economical.seat--;
                        else add[i].sleeper.seat--;
                        awrite();
                        bookticket_write();
                        SetConsoleTextAttribute(hConsole, 0x0A);
                        std::printf("  Ticket booked successfully for %s seat %d on %s!\n", classChoice, seatChoice, bookingDate);
                        SetConsoleTextAttribute(hConsole, 0x1F);
                    }
                }
            }
        }
        if (found == -1) {
            SetConsoleTextAttribute(hConsole, 0x0C);
            std::printf("  Train not found or not scheduled for %s!\n", bookingDate);
            SetConsoleTextAttribute(hConsole, 0x1F);
            Sleep(1000);
            return;
        }
    }
    u += n;
    bookticket_write();
    std::printf("\n");
    getIntInput("  Press 1 to return to main portal: ", 1, 1);
}

void cancelticket() {
    viewpassengers_read();
    char pnnmbr[20];
    int location = -1, e;
    system("cls");
    displayHeader("CANCEL TICKET");

    while (1) {
        std::printf("  Enter phone number (10 digits): ");
        std::scanf("%19s", pnnmbr);
        if (validatePhone(pnnmbr)) break;
        SetConsoleTextAttribute(hConsole, 0x0C);
        std::printf("  Invalid phone number! Must be 10 digits.\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
    }

    for (e = 0; e < u; e++) {
        if (std::strcmp(pnnmbr, book[e].phone) == 0) {
            location = e;
            break;
        }
    }
    if (location == -1) {
        SetConsoleTextAttribute(hConsole, 0x0C);
        std::printf("  Booking not found!\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
        Sleep(1000);
    } else {
        for (int i = 0; i < k; i++) {
            if (std::strcmp(book[location].train_number, add[i].train_number) == 0 &&
                std::strcmp(book[location].date, add[i].departure_date) == 0) {
                if (std::strcmp(book[location].class_type, "BusinessCouch") == 0)
                    add[i].business.seat++;
                else if (std::strcmp(book[location].class_type, "EconomicalCouch") == 0)
                    add[i].economical.seat++;
                else if (std::strcmp(book[location].class_type, "SleeperCouch") == 0)
                    add[i].sleeper.seat++;
                break;
            }
        }
        for (e = location; e < u - 1; e++) {
            std::strcpy(book[e].date, book[e + 1].date);
            std::strcpy(book[e].train_number, book[e + 1].train_number);
            std::strcpy(book[e].name, book[e + 1].name);
            std::strcpy(book[e].phone, book[e + 1].phone);
            book[e].seat = book[e + 1].seat;
            std::strcpy(book[e].email, book[e + 1].email);
            std::strcpy(book[e].class_type, book[e + 1].class_type);
        }
        u--;
        bookticket_write();
        awrite();
        SetConsoleTextAttribute(hConsole, 0x0A);
        std::printf("  Ticket cancelled successfully!\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
        Sleep(1000);
    }
    std::printf("\n");
    getIntInput("  Press 1 to return to main portal: ", 1, 1);
}

void admin() {
    while (1) {
        system("cls");
        displayHeader("ADMIN PORTAL");

        const char* adminMenu[] = {
            "1. View Passengers",
            "2. Add Train",
            "3. Delete Train",
            "4. Modify Train",
            "5. Exit from Admin Account"
        };
        std::printf("\n  ------------------ ADMIN PORTAL ------------------\n");
        for (int i = 0; i < 5; i++) {
            std::printf("    %s\n", adminMenu[i]);
        }
        std::printf("  -----------------------------------------------\n");

        int chhh = getIntInput("  Enter choice (1-5): ", 1, 5);
        displayMenuWithHighlight(adminMenu, 5, chhh);

        switch (chhh) {
            case 1: viewpassenger(); break;
            case 2: addtrain(); break;
            case 3: dlttrain(); break;
            case 4: modifyTrain(); break;
            case 5:
                SetConsoleTextAttribute(hConsole, 0x0A);
                std::printf("  Exiting admin account...\n");
                SetConsoleTextAttribute(hConsole, 0x1F);
                Sleep(1000);
                return;
        }
    }
}

void password() {
    int number = 3456;
    system("cls");
    displayHeader("ADMIN LOGIN");

    int pass = getIntInput("  Enter password: ", 0, 9999);
    if (pass == number) {
        SetConsoleTextAttribute(hConsole, 0x0A);
        std::printf("  Login successful!\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
        Sleep(1000);
        admin();
    } else {
        SetConsoleTextAttribute(hConsole, 0x0C);
        std::printf("  Wrong password!\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
        Sleep(1000);
    }
}

void addtrain() {
    while (1) {
        system("cls");
        displayHeader("ADD TRAIN FROM LIST");

        std::printf("\n  Available Trains:\n");
        if (TRAIN_LIST_SIZE == 0) {
            std::printf("  No trains available to add.\n");
            std::printf("\n  What would you like to do next?\n");
            const char* nextMenu[] = {
                "1. Add another train",
                "2. Return to admin portal"
            };
            for (int i = 0; i < 2; i++) {
                std::printf("    %s\n", nextMenu[i]);
            }
            int choice = getIntInput("  Enter choice (1-2): ", 1, 2);
            displayMenuWithHighlight(nextMenu, 2, choice);
            if (choice == 1) continue;
            return;
        }

        std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-5s %-10s %-6s\n",
               "SI", "Train No", "Train Name", "Start", "Dest", "Class", "Price", "Seats", "Date", "Time");
        std::printf("  ----------------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < TRAIN_LIST_SIZE; i++) {
            std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-5d %-10s %-6s\n",
                   train_list[i].si, train_list[i].train_number, train_list[i].train_name,
                   train_list[i].start, train_list[i].destination, "Business",
                   train_list[i].business.price, train_list[i].business.total_seats,
                   train_list[i].departure_date, train_list[i].departure_time);
            std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-5d %-10s %-6s\n",
                   "", "", "", "", "", "Economical",
                   train_list[i].economical.price, train_list[i].economical.total_seats, "", "");
            std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-5d %-10s %-6s\n",
                   "", "", "", "", "", "Sleeper",
                   train_list[i].sleeper.price, train_list[i].sleeper.total_seats, "", "");
        }
        std::printf("  ----------------------------------------------------------------------------------------------------\n");

        char train_num[20];
        while (1) {
            std::printf("\n  Enter Train Number to add (or '0' to cancel): ");
            std::scanf("%19s", train_num);
            if (std::strcmp(train_num, "0") == 0 || validateTrainNumber(train_num)) break;
            SetConsoleTextAttribute(hConsole, 0x0C);
            std::printf("  Invalid train number! Must be format T followed by 3 digits.\n");
            SetConsoleTextAttribute(hConsole, 0x1F);
        }

        if (std::strcmp(train_num, "0") == 0) {
            SetConsoleTextAttribute(hConsole, 0x0C);
            std::printf("  Operation cancelled.\n");
            SetConsoleTextAttribute(hConsole, 0x1F);
            Sleep(1000);
        } else {
            int found = -1;
            for (int i = 0; i < TRAIN_LIST_SIZE; i++) {
                if (std::strcmp(train_num, train_list[i].train_number) == 0) {
                    found = i;
                    break;
                }
            }

            if (found == -1) {
                SetConsoleTextAttribute(hConsole, 0x0C);
                std::printf("  Train number not found in the list!\n");
                SetConsoleTextAttribute(hConsole, 0x1F);
                Sleep(1000);
            } else {
                for (int i = 0; i < k; i++) {
                    if (std::strcmp(train_num, add[i].train_number) == 0 &&
                        std::strcmp(train_list[found].departure_date, add[i].departure_date) == 0) {
                        SetConsoleTextAttribute(hConsole, 0x0C);
                        std::printf("  This train is already available for booking on %s!\n", add[i].departure_date);
                        SetConsoleTextAttribute(hConsole, 0x1F);
                        Sleep(1000);
                        goto after_action;
                    }
                }

                std::strcpy(add[k].si, train_list[found].si);
                std::strcpy(add[k].train_number, train_list[found].train_number);
                std::strcpy(add[k].train_name, train_list[found].train_name);
                std::strcpy(add[k].start, train_list[found].start);
                std::strcpy(add[k].destination, train_list[found].destination);
                std::strcpy(add[k].business.price, train_list[found].business.price);
                add[k].business.total_seats = train_list[found].business.total_seats;
                add[k].business.seat = train_list[found].business.total_seats;
                std::strcpy(add[k].economical.price, train_list[found].economical.price);
                add[k].economical.total_seats = train_list[found].economical.total_seats;
                add[k].economical.seat = train_list[found].economical.total_seats;
                std::strcpy(add[k].sleeper.price, train_list[found].sleeper.price);
                add[k].sleeper.total_seats = train_list[found].sleeper.total_seats;
                add[k].sleeper.seat = train_list[found].sleeper.total_seats;
                std::strcpy(add[k].departure_date, train_list[found].departure_date);
                std::strcpy(add[k].departure_time, train_list[found].departure_time);

                k++;
                awrite();

                for (int i = found; i < TRAIN_LIST_SIZE - 1; i++) {
                    train_list[i] = train_list[i + 1];
                }
                TRAIN_LIST_SIZE--;

                SetConsoleTextAttribute(hConsole, 0x0A);
                std::printf("  Train %s added successfully for booking!\n", train_num);
                SetConsoleTextAttribute(hConsole, 0x1F);
                Sleep(1000);
            }
        }

    after_action:
        std::printf("\n  What would you like to do next?\n");
        const char* nextMenu[] = {
            "1. Add another train",
            "2. Return to admin portal"
        };
        for (int i = 0; i < 2; i++) {
            std::printf("    %s\n", nextMenu[i]);
        }
        int choice = getIntInput("  Enter choice (1-2): ", 1, 2);
        displayMenuWithHighlight(nextMenu, 2, choice);
        if (choice == 1) continue;
        return;
    }
}

void dlttrain() {
    aread();
    char train[20];
    int location = -1, f;
    system("cls");
    displayHeader("DELETE TRAIN");

    while (1) {
        std::printf("  Enter train number (e.g., T123): ");
        std::scanf("%19s", train);
        if (validateTrainNumber(train)) break;
        SetConsoleTextAttribute(hConsole, 0x0C);
        std::printf("  Invalid train number! Must be format T followed by 3 digits.\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
    }

    for (f = 0; f < k; f++) {
        if (std::strcmp(train, add[f].train_number) == 0) {
            location = f;
            break;
        }
    }
    if (location == -1) {
        SetConsoleTextAttribute(hConsole, 0x0C);
        std::printf("  Train not found!\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
        Sleep(1000);
    } else {
        for (int b = 0; b < u; b++) {
            if (std::strcmp(book[b].train_number, train) == 0 &&
                std::strcmp(book[b].date, add[location].departure_date) == 0) {
                SetConsoleTextAttribute(hConsole, 0x0C);
                std::printf("  Cannot delete train with active bookings!\n");
                SetConsoleTextAttribute(hConsole, 0x1F);
                Sleep(1000);
                std::printf("\n");
                getIntInput("  Press 1 to return to admin portal: ", 1, 1);
                return;
            }
        }

        std::strcpy(train_list[TRAIN_LIST_SIZE].si, add[location].si);
        std::strcpy(train_list[TRAIN_LIST_SIZE].train_number, add[location].train_number);
        std::strcpy(train_list[TRAIN_LIST_SIZE].train_name, add[location].train_name);
        std::strcpy(train_list[TRAIN_LIST_SIZE].start, add[location].start);
        std::strcpy(train_list[TRAIN_LIST_SIZE].destination, add[location].destination);
        std::strcpy(train_list[TRAIN_LIST_SIZE].business.price, add[location].business.price);
        train_list[TRAIN_LIST_SIZE].business.total_seats = add[location].business.total_seats;
        std::strcpy(train_list[TRAIN_LIST_SIZE].economical.price, add[location].economical.price);
        train_list[TRAIN_LIST_SIZE].economical.total_seats = add[location].economical.total_seats;
        std::strcpy(train_list[TRAIN_LIST_SIZE].sleeper.price, add[location].sleeper.price);
        train_list[TRAIN_LIST_SIZE].sleeper.total_seats = add[location].sleeper.total_seats;
        std::strcpy(train_list[TRAIN_LIST_SIZE].departure_date, add[location].departure_date);
        std::strcpy(train_list[TRAIN_LIST_SIZE].departure_time, add[location].departure_time);
        TRAIN_LIST_SIZE++;

        for (f = location; f < k - 1; f++) {
            std::strcpy(add[f].si, add[f + 1].si);
            std::strcpy(add[f].train_number, add[f + 1].train_number);
            std::strcpy(add[f].train_name, add[f + 1].train_name);
            std::strcpy(add[f].start, add[f + 1].start);
            std::strcpy(add[f].destination, add[f + 1].destination);
            std::strcpy(add[f].business.price, add[f + 1].business.price);
            add[f].business.seat = add[f + 1].business.seat;
            add[f].business.total_seats = add[f + 1].business.total_seats;
            std::strcpy(add[f].economical.price, add[f + 1].economical.price);
            add[f].economical.seat = add[f + 1].economical.seat;
            add[f].economical.total_seats = add[f + 1].economical.total_seats;
            std::strcpy(add[f].sleeper.price, add[f + 1].sleeper.price);
            add[f].sleeper.seat = add[f + 1].sleeper.seat;
            add[f].sleeper.total_seats = add[f + 1].sleeper.total_seats;
            std::strcpy(add[f].departure_date, add[f + 1].departure_date);
            std::strcpy(add[f].departure_time, add[f + 1].departure_time);
        }
        k--;
        awrite();
        SetConsoleTextAttribute(hConsole, 0x0A);
        std::printf("  Train deleted successfully and added back to the train list!\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
        Sleep(1000);
    }
    std::printf("\n");
    getIntInput("  Press 1 to return to admin portal: ", 1, 1);
}

void viewpassenger() {
    system("cls");
    viewpassengers_read();
    displayHeader("PASSENGER LIST");

    if (u == 0) {
        std::printf("  No passengers booked yet.\n");
    } else {
        std::printf("\n  %-10s %-15s %-10s %-10s %-10s %-5s %-15s\n",
               "Train No", "Name", "Phone", "Date", "Class", "Seat", "Email");
        std::printf("  ---------------------------------------------------------------------------\n");
        for (int j = 0; j < u; j++) {
            std::printf("  %-10s %-15s %-10s %-10s %-10s %-5d %-15s\n",
                   book[j].train_number, book[j].name, book[j].phone, book[j].date,
                   book[j].class_type, book[j].seat, book[j].email);
        }
        std::printf("  ---------------------------------------------------------------------------\n");
    }

    std::printf("\n");
    getIntInput("  Press 1 to go back to the admin portal: ", 1, 1);
}

void modifyTrain() {
    char trainNum[20];
    int location = -1, i;
    system("cls");
    displayHeader("MODIFY TRAIN");

    aread();
    while (1) {
        std::printf("  Enter Train Number to modify (e.g., T123): ");
        std::scanf("%19s", trainNum);
        if (validateTrainNumber(trainNum)) break;
        SetConsoleTextAttribute(hConsole, 0x0C);
        std::printf("  Invalid train number! Must be format T followed by 3 digits.\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
    }

    for (i = 0; i < k; i++) {
        if (std::strcmp(add[i].train_number, trainNum) == 0) {
            location = i;
            break;
        }
    }

    if (location == -1) {
        SetConsoleTextAttribute(hConsole, 0x0C);
        std::printf("  Train not found!\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
        Sleep(1000);
        std::printf("\n");
        getIntInput("  Press 1 to return to admin portal: ", 1, 1);
        return;
    }

    bool continueModifying = true;
    while (continueModifying) {
        system("cls");
        displayHeader("MODIFY TRAIN");
        std::printf("\n  Current Train Details:\n");
        std::printf("  -----------------------\n");
        std::printf("  Serial No:    %s\n", add[location].si);
        std::printf("  Train Number: %s\n", add[location].train_number);
        std::printf("  Train Name:   %s\n", add[location].train_name);
        std::printf("  Start Place:  %s\n", add[location].start);
        std::printf("  Destination:  %s\n", add[location].destination);
        std::printf("  Business Class - Price: %s, Seats: %d, Total Seats: %d\n",
               add[location].business.price, add[location].business.seat, add[location].business.total_seats);
        std::printf("  Economical Class - Price: %s, Seats: %d, Total Seats: %d\n",
               add[location].economical.price, add[location].economical.seat, add[location].economical.total_seats);
        std::printf("  Sleeper Class - Price: %s, Seats: %d, Total Seats: %d\n",
               add[location].sleeper.price, add[location].sleeper.seat, add[location].sleeper.total_seats);
        std::printf("  Date:         %s\n", add[location].departure_date);
        std::printf("  Time:         %s\n", add[location].departure_time);
        std::printf("  -----------------------\n");

        const char* modifyMenu[] = {
            "1. Train Name",
            "2. Start Place",
            "3. Destination",
            "4. Business Class Price",
            "5. Business Class Total Seats",
            "6. Economical Class Price",
            "7. Economical Class Total Seats",
            "8. Sleeper Class Price",
            "9. Sleeper Class Total Seats",
            "10. Departure Date",
            "11. Departure Time"
        };
        std::printf("\n  Select what to modify:\n");
        for (int i = 0; i < 11; i++) {
            std::printf("    %s\n", modifyMenu[i]);
        }
        std::printf("  -----------------------\n");

        int choice = getIntInput("  Enter choice (1-11): ", 1, 11);
        displayMenuWithHighlight(modifyMenu, 11, choice);

        switch (choice) {
            case 1:
                std::printf("  Enter new Train Name: ");
                std::scanf(" %49[^\n]s", add[location].train_name);
                break;
            case 2:
                std::printf("  Enter new Start Place: ");
                std::scanf("%29s", add[location].start);
                break;
            case 3:
                std::printf("  Enter new Destination: ");
                std::scanf("%29s", add[location].destination);
                break;
            case 4: {
                int price = getIntInput("  Enter new Business Class Price (numeric value, e.g., 100): ", 1, 10000);
                std::snprintf(add[location].business.price, sizeof(add[location].business.price), "EUR%d", price);
                break;
            }
            case 5:
                while (1) {
                    int newTotalSeats = getIntInput("  Enter new Business Class Total Seats: ", 1, 1000);
                    if (newTotalSeats >= (add[location].business.total_seats - add[location].business.seat)) {
                        add[location].business.total_seats = newTotalSeats;
                        break;
                    }
                    SetConsoleTextAttribute(hConsole, 0x0C);
                    std::printf("  New total cannot be less than booked seats (%d)!\n",
                           add[location].business.total_seats - add[location].business.seat);
                    SetConsoleTextAttribute(hConsole, 0x1F);
                }
                break;
            case 6: {
                int price = getIntInput("  Enter new Economical Class Price (numeric value, e.g., 50): ", 1, 10000);
                std::snprintf(add[location].economical.price, sizeof(add[location].economical.price), "EUR%d", price);
                break;
            }
            case 7:
                while (1) {
                    int newTotalSeats = getIntInput("  Enter new Economical Class Total Seats: ", 1, 1000);
                    if (newTotalSeats >= (add[location].economical.total_seats - add[location].economical.seat)) {
                        add[location].economical.total_seats = newTotalSeats;
                        break;
                    }
                    SetConsoleTextAttribute(hConsole, 0x0C);
                    std::printf("  New total cannot be less than booked seats (%d)!\n",
                           add[location].economical.total_seats - add[location].economical.seat);
                    SetConsoleTextAttribute(hConsole, 0x1F);
                }
                break;
            case 8: {
                int price = getIntInput("  Enter new Sleeper Class Price (numeric value, e.g., 30): ", 1, 10000);
                std::snprintf(add[location].sleeper.price, sizeof(add[location].sleeper.price), "EUR%d", price);
                break;
            }
            case 9:
                while (1) {
                    int newTotalSeats = getIntInput("  Enter new Sleeper Class Total Seats: ", 1, 1000);
                    if (newTotalSeats >= (add[location].sleeper.total_seats - add[location].sleeper.seat)) {
                        add[location].sleeper.total_seats = newTotalSeats;
                        break;
                    }
                    SetConsoleTextAttribute(hConsole, 0x0C);
                    std::printf("  New total cannot be less than booked seats (%d)!\n",
                           add[location].sleeper.total_seats - add[location].sleeper.seat);
                    SetConsoleTextAttribute(hConsole, 0x1F);
                }
                break;
            case 10:
                while (1) {
                    std::printf("  Enter new Departure Date (YYYY-MM-DD): ");
                    std::scanf("%19s", add[location].departure_date);
                    if (validateDate(add[location].departure_date) && isFutureDate(add[location].departure_date)) break;
                    SetConsoleTextAttribute(hConsole, 0x0C);
                    std::printf("  Invalid date or date in the past! Use YYYY-MM-DD format.\n");
                    SetConsoleTextAttribute(hConsole, 0x1F);
                }
                break;
            case 11:
                while (1) {
                    std::printf("  Enter new Departure Time (HH:MM): ");
                    std::scanf("%9s", add[location].departure_time);
                    if (validateTime(add[location].departure_time)) break;
                    SetConsoleTextAttribute(hConsole, 0x0C);
                    std::printf("  Invalid time format! Use HH:MM (e.g., 14:30).\n");
                    SetConsoleTextAttribute(hConsole, 0x1F);
                }
                break;
        }
        awrite();
        SetConsoleTextAttribute(hConsole, 0x0A);
        std::printf("  Train details modified successfully!\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
        Sleep(1000);

        std::printf("\n  What would you like to do next?\n");
        const char* nextMenu[] = {
            "1. Modify more details",
            "2. Return to admin portal"
        };
        for (int i = 0; i < 2; i++) {
            std::printf("    %s\n", nextMenu[i]);
        }
        int nextAction = getIntInput("  Enter choice (1-2): ", 1, 2);
        displayMenuWithHighlight(nextMenu, 2, nextAction);
        if (nextAction == 2) {
            continueModifying = false;
        }
    }
}

void searchTrain() {
    int i, found = 0;
    char searchTerm[50];
    system("cls");
    aread();
    displayHeader("SEARCH TRAIN");

    const char* searchMenu[] = {
        "1. Train Name",
        "2. Destination"
    };
    std::printf("\n  Search by:\n");
    for (int i = 0; i < 2; i++) {
        std::printf("    %s\n", searchMenu[i]);
    }
    int choice = getIntInput("  Enter choice (1-2): ", 1, 2);
    displayMenuWithHighlight(searchMenu, 2, choice);

    if (choice == 1) {
        std::printf("  Enter Train Name: ");
        std::scanf(" %49[^\n]s", searchTerm);
        std::printf("\n  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-5s %-10s %-6s\n",
               "SI", "Train No", "Train Name", "Start", "Dest", "Class", "Price", "Seats", "Date", "Time");
        std::printf("  ----------------------------------------------------------------------------------------------------\n");
        for (i = 0; i < k; i++) {
            if (std::strcmp(add[i].train_name, searchTerm) == 0) {
                found = 1;
                std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-5d %-10s %-6s\n",
                       add[i].si, add[i].train_number, add[i].train_name, add[i].start, add[i].destination,
                       "Business", add[i].business.price, add[i].business.seat, add[i].departure_date, add[i].departure_time);
                std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-5d %-10s %-6s\n",
                       "", "", "", "", "", "Economical", add[i].economical.price, add[i].economical.seat, "", "");
                std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-5d %-10s %-6s\n",
                       "", "", "", "", "", "Sleeper", add[i].sleeper.price, add[i].sleeper.seat, "", "");
            }
        }
        std::printf("  ----------------------------------------------------------------------------------------------------\n");
    } else if (choice == 2) {
        std::printf("  Enter Destination: ");
        std::scanf("%29s", searchTerm);
        std::printf("\n  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-5s %-10s %-6s\n",
               "SI", "Train No", "Train Name", "Start", "Dest", "Class", "Price", "Seats", "Date", "Time");
        std::printf("  ----------------------------------------------------------------------------------------------------\n");
        for (i = 0; i < k; i++) {
            if (std::strcmp(add[i].destination, searchTerm) == 0) {
                found = 1;
                std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-5d %-10s %-6s\n",
                       add[i].si, add[i].train_number, add[i].train_name, add[i].start, add[i].destination,
                       "Business", add[i].business.price, add[i].business.seat, add[i].departure_date, add[i].departure_time);
                std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-5d %-10s %-6s\n",
                       "", "", "", "", "", "Economical", add[i].economical.price, add[i].economical.seat, "", "");
                std::printf("  %-4s %-10s %-15s %-10s %-10s %-10s %-6s %-5d %-10s %-6s\n",
                       "", "", "", "", "", "Sleeper", add[i].sleeper.price, add[i].sleeper.seat, "", "");
            }
        }
        std::printf("  ----------------------------------------------------------------------------------------------------\n");
    }

    if (!found) {
        SetConsoleTextAttribute(hConsole, 0x0C);
        std::printf("  No trains found matching '%s'!\n", searchTerm);
        SetConsoleTextAttribute(hConsole, 0x1F);
    }

    std::printf("\n");
    getIntInput("  Press 1 to return to main portal: ", 1, 1);
}

void printEmailTicket() {
    system("cls");
    viewpassengers_read();
    char pnnmbr[20];
    int location = -1, e;

    displayHeader("PRINT & EMAIL TICKET");

    while (1) {
        std::printf("  Enter phone number (10 digits): ");
        std::scanf("%19s", pnnmbr);
        if (validatePhone(pnnmbr)) break;
        SetConsoleTextAttribute(hConsole, 0x0C);
        std::printf("  Invalid phone number! Must be 10 digits.\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
    }

    for (e = 0; e < u; e++) {
        if (std::strcmp(pnnmbr, book[e].phone) == 0) {
            location = e;
            break;
        }
    }

    if (location == -1) {
        SetConsoleTextAttribute(hConsole, 0x0C);
        std::printf("  Booking not found!\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
        Sleep(1000);
        std::printf("\n");
        getIntInput("  Press 1 to return to main portal: ", 1, 1);
        return;
    }

    int trainIndex = -1;
    char price[20] = "N/A";
    for (int i = 0; i < k; i++) {
        if (std::strcmp(book[location].train_number, add[i].train_number) == 0 &&
            std::strcmp(book[location].date, add[i].departure_date) == 0) {
            trainIndex = i;
            if (std::strcmp(book[location].class_type, "BusinessCouch") == 0)
                std::strcpy(price, add[i].business.price);
            else if (std::strcmp(book[location].class_type, "EconomicalCouch") == 0)
                std::strcpy(price, add[i].economical.price);
            else if (std::strcmp(book[location].class_type, "SleeperCouch") == 0)
                std::strcpy(price, add[i].sleeper.price);
            break;
        }
    }

    std::printf("\n  ========== TRAIN TICKET ==========\n");
    std::printf("  Passenger Name: %s\n", book[location].name);
    std::printf("  Phone: %s\n", book[location].phone);
    if (std::strlen(book[location].email) > 0) {
        std::printf("  Email: %s\n", book[location].email);
    }
    std::printf("  Train Number: %s\n", book[location].train_number);
    if (trainIndex != -1) {
        std::printf("  Train Name: %s\n", add[trainIndex].train_name);
        std::printf("  From: %s\n", add[trainIndex].start);
        std::printf("  To: %s\n", add[trainIndex].destination);
        std::printf("  Class: %s\n", book[location].class_type);
        std::printf("  Price: %s\n", price);
    }
    std::printf("  Travel Date: %s\n", book[location].date);
    std::printf("  Seat Number: %d\n", book[location].seat);
    std::printf("  ==================================\n");

    const char* emailMenu[] = {
        "1. Yes",
        "2. No"
    };
    std::printf("  Email this ticket?\n");
    for (int i = 0; i < 2; i++) {
        std::printf("    %s\n", emailMenu[i]);
    }
    int choice = getIntInput("  Enter choice (1-2): ", 1, 2);
    displayMenuWithHighlight(emailMenu, 2, choice);

    if (choice == 1) {
        if (std::strlen(book[location].email) == 0) {
            while (1) {
                std::printf("  Enter your email: ");
                std::scanf("%49s", book[location].email);
                if (validateEmail(book[location].email)) break;
                SetConsoleTextAttribute(hConsole, 0x0C);
                std::printf("  Invalid email format!\n");
                SetConsoleTextAttribute(hConsole, 0x1F);
            }
            bookticket_write();
        }
        SetConsoleTextAttribute(hConsole, 0x0A);
        std::printf("  Simulating email send to %s...\n", book[location].email);
        std::printf("  Email sent successfully! (Simulation)\n");
        SetConsoleTextAttribute(hConsole, 0x1F);
        Sleep(1000);
    }

    std::printf("\n");
    getIntInput("  Press 1 to return to main portal: ", 1, 1);
}

void awrite() {
    std::printf("  Writing train data...\n");
}

void aread() {
    std::printf("  Reading train data...\n");
}

void bookticket_write() {
    std::printf("  Writing booking data...\n");
}

void viewpassengers_read() {
    std::printf("  Reading passenger data...\n");
}
