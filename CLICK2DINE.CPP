#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <time.h>

#define MAX_ITEMS 5
#define MAX_TABLES 10
#define ADMIN_PASSWORD "admin123"

struct MenuItem {
    char name[30];
    float price;
};

struct Order {
    int itemIndex;
    int quantity;
};

void save_tables(int reservedTables[]) {
    FILE *fp;
    int i;
    fp = fopen("tables.txt", "w");
    if (fp != NULL) {
        for (i = 0; i < MAX_TABLES; i++)
            fprintf(fp, "%d ", reservedTables[i]);
        fclose(fp);
    }
}

void load_tables(int reservedTables[]) {
    FILE *fp;
    int i;
    fp = fopen("tables.txt", "r");
    if (fp != NULL) {
        for (i = 0; i < MAX_TABLES; i++)
            fscanf(fp, "%d", &reservedTables[i]);
        fclose(fp);
    } else {
        for (i = 0; i < MAX_TABLES; i++)
            reservedTables[i] = 0;
    }
}

void reset_tables(int reservedTables[]) {
    int i;
    for (i = 0; i < MAX_TABLES; i++)
        reservedTables[i] = 0;
    save_tables(reservedTables);
    printf("All table reservations have been reset.\n");
}

char get_yes_no(const char *prompt) {
    char answer[10];
    while (1) {
        printf("%s (Y/N): ", prompt);
        scanf("%s", answer);
        fflush(stdin);
        if (answer[0] == 'Y' || answer[0] == 'y') return 'Y';
        if (answer[0] == 'N' || answer[0] == 'n') return 'N';
        printf("Invalid input. Please enter Y or N.\n");
    }
}

void view_order_history() {
    FILE *fp;
    char line[256];
    printf("\n--- ORDER HISTORY ---\n");
    fp = fopen("history.txt", "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            printf("%s", line);
        }
        fclose(fp);
    } else {
        printf("No order history found.\n");
    }
    printf("---------------------\n");
}

void view_total_sales() {
    FILE *fp;
    char line[256];
    float totalSales = 0, sale;
    printf("\n--- TOTAL SALES FOR THE DAY ---\n");
    fp = fopen("sales.txt", "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "%f", &sale) == 1)
                totalSales += sale;
        }
        fclose(fp);
    }
    printf("Total Sales: %.2f\n", totalSales);
    printf("---------------------\n");
}

void reset_history() {
    FILE *fp;
    fp = fopen("history.txt", "w");
    if (fp != NULL) {
        fclose(fp);
        printf("Order history has been reset.\n");
    } else {
        printf("Could not reset order history.\n");
    }
    fp = fopen("sales.txt", "w");
    if (fp != NULL) {
        fclose(fp);
        printf("Sales log has been reset.\n");
    } else {
        printf("Could not reset sales log.\n");
    }
}

void admin_panel(int reservedTables[]) {
    int admin_choice, valid;
    while (1) {
        printf("\n--- ADMIN PANEL ---\n");
        printf("1. View Order History\n");
        printf("2. View Total Sales for the Day\n");
        printf("3. Reset Order History and Sales\n");
        printf("4. Reset Table Reservations\n");
        printf("5. Exit Admin Panel\n");
        valid = 0;
        while (!valid) {
            printf("Enter your choice: ");
            if (scanf("%d", &admin_choice) != 1) {
                printf("Invalid input. Please enter a number.\n");
                fflush(stdin);
            } else {
                valid = 1;
            }
        }
        fflush(stdin);
        switch (admin_choice) {
            case 1:
                view_order_history();
                break;
            case 2:
                view_total_sales();
                break;
            case 3:
                reset_history();
                break;
            case 4:
                reset_tables(reservedTables);
                break;
            case 5:
                printf("Exiting admin panel.\n");
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}

void trim(char *str) {
    int i;
    // Remove leading spaces
    while (str[0] == ' ') memmove(str, str + 1, strlen(str));
    // Remove trailing spaces
    i = strlen(str) - 1;
    while (i >= 0 && str[i] == ' ') {
        str[i] = 0;
        i--;
    }
}

void main() {
    //clrscr();

    char loginType[10], password[20], customerName[50], contactInfo[50];
    struct MenuItem menu[MAX_ITEMS] = {
        {"Spaghetti", 120.0},
        {"Burger", 99.0},
        {"Pizza", 150.0},
        {"Salad", 80.0},
        {"Juice", 40.0}
    };
    struct Order orders[10];
    int reservedTables[MAX_TABLES];
    float totalSalesToday = 0;
    int ch, i, valid;

    load_tables(reservedTables);

    while (1) { // OUTER LOOP for main menu
        printf("Are you a customer, admin, or do you want to end the program?\n");
        printf("(Type 'admin' for admin login, 'end' to exit, anything else for customer): ");
        scanf("%s", loginType);
        fflush(stdin);

        if (strcmpi(loginType, "end") == 0) {
            printf("Program ended. Goodbye!\n");
            getch();
            return;
        }

        if (strcmp(loginType, "admin") == 0) {
            printf("Enter admin password: ");
            scanf("%s", password);
            fflush(stdin);
            if (strcmp(password, ADMIN_PASSWORD) == 0) {
                admin_panel(reservedTables);
                save_tables(reservedTables);
                continue; // Return to main menu after admin panel
            } else {
                printf("Incorrect password. Returning to main menu.\n");
                continue;
            }
        }

        // CUSTOMER LOOP
        while (1) {
            int orderCount = 0, choice, qty, diningOption, reservedTable = 0, scanResult;
            float total = 0;
            char reservationDateTime[64] = "";

            while ((ch = getchar()) != '\n' && ch != EOF);

            printf("\nENTER INFORMATION\n");
            // Name input with empty check
            do {
                printf("Enter name: ");
                fgets(customerName, sizeof(customerName), stdin);
                customerName[strcspn(customerName, "\n")] = 0;
                trim(customerName);
                if (strlen(customerName) == 0)
                    printf("Name cannot be empty. Please enter your name.\n");
            } while (strlen(customerName) == 0);

            // Contact info input with empty check
            do {
                printf("Contact info: ");
                fgets(contactInfo, sizeof(contactInfo), stdin);
                contactInfo[strcspn(contactInfo, "\n")] = 0;
                trim(contactInfo);
                if (strlen(contactInfo) == 0)
                    printf("Contact info cannot be empty. Please enter your contact info.\n");
            } while (strlen(contactInfo) == 0);

            printf("\nVIEW THE MENU\n");
            for (i = 0; i < MAX_ITEMS; i++) {
                printf("%d. %s - %.2f\n", i + 1, menu[i].name, menu[i].price);
            }
            printf("6. Remove item from order\n");

            printf("\nSELECT ITEMS TO ORDER\n");
            while (1) {
                valid = 0;
                while (!valid) {
                    printf("Type the number of the item you want (0 to finish): ");
                    if (scanf("%d", &choice) != 1) {
                        printf("Invalid input. Please enter a number between 0 and 6.\n");
                        fflush(stdin);
                    } else {
                        valid = 1;
                    }
                }
                fflush(stdin);

                if (choice == 0) break;
                if (choice == 6) {
                    if (orderCount == 0) {
                        printf("No items to remove.\n");
                        continue;
                    }
                    printf("\nCurrent Order:\n");
                    for (i = 0; i < orderCount; i++) {
                        int idx = orders[i].itemIndex;
                        printf("%d. %s x%d = %.2f\n", i + 1, menu[idx].name, orders[i].quantity, menu[idx].price * orders[i].quantity);
                    }
                    valid = 0;
                    while (!valid) {
                        printf("Enter the order number to remove (1-%d): ", orderCount);
                        if (scanf("%d", &qty) != 1) {
                            printf("Invalid input. Please enter a number.\n");
                            fflush(stdin);
                        } else if (qty < 1 || qty > orderCount) {
                            printf("Invalid order number.\n");
                        } else {
                            valid = 1;
                        }
                    }
                    fflush(stdin);
                    for (i = qty - 1; i < orderCount - 1; i++) {
                        orders[i] = orders[i + 1];
                    }
                    orderCount--;
                    printf("Item removed.\n");
                    continue;
                }
                if (choice < 1 || choice > MAX_ITEMS) {
                    printf("Invalid item number.\n");
                    continue;
                }
                valid = 0;
                while (!valid) {
                    printf("Enter the quantity you want: ");
                    if (scanf("%d", &qty) != 1) {
                        printf("Invalid input. Please enter a number greater than 0.\n");
                        fflush(stdin);
                    } else if (qty < 1) {
                        printf("Invalid quantity. Please enter a number greater than 0.\n");
                    } else {
                        valid = 1;
                    }
                }
                fflush(stdin);
                orders[orderCount].itemIndex = choice - 1;
                orders[orderCount].quantity = qty;
                orderCount++;
            }
            if (orderCount == 0) continue;

            printf("\nDINING OPTION\n");
            valid = 0;
            while (!valid) {
                printf("1. Dine in\n2. Reservation\n3. Take out\nChoose option: ");
                if (scanf("%d", &diningOption) != 1) {
                    printf("Invalid input. Please enter 1, 2, or 3.\n");
                    fflush(stdin);
                } else if (diningOption < 1 || diningOption > 3) {
                    printf("Invalid dining option. Please enter 1, 2, or 3.\n");
                } else {
                    valid = 1;
                }
            }
            fflush(stdin);

            if (diningOption == 2) {
                int available = 0;
                printf("Available tables: ");
                for (i = 0; i < MAX_TABLES; i++) {
                    if (!reservedTables[i]) {
                        printf("%d ", i + 1);
                        available = 1;
                    }
                }
                if (!available) {
                    printf("\nNo tables available for reservation.\n");
                    continue;
                }
                valid = 0;
                while (!valid) {
                    printf("\nChoose a table number to reserve: ");
                    if (scanf("%d", &reservedTable) != 1) {
                        printf("Invalid input. Please enter a valid table number.\n");
                        fflush(stdin);
                    } else if (reservedTable < 1 || reservedTable > MAX_TABLES || reservedTables[reservedTable - 1]) {
                        printf("Invalid or already reserved table number. Reservation cancelled.\n");
                    } else {
                        valid = 1;
                    }
                }
                fflush(stdin);
                reservedTables[reservedTable - 1] = 1;
                save_tables(reservedTables);

                /* Get current date and time for reservation */
                {
                    time_t now;
                    struct tm *t;
                    now = time(NULL);
                    t = localtime(&now);
                    sprintf(reservationDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
                            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                            t->tm_hour, t->tm_min, t->tm_sec);
                }
            }

            printf("\n--- Order Summary ---\n");
            printf("Name: %s\nContact: %s\n", customerName, contactInfo);
            printf("Items Ordered:\n");
            total = 0;
            for (i = 0; i < orderCount; i++) {
                int idx = orders[i].itemIndex;
                printf("%s x%d = %.2f\n", menu[idx].name, orders[i].quantity, menu[idx].price * orders[i].quantity);
                total += menu[idx].price * orders[i].quantity;
            }
            if (diningOption == 1)
                printf("Dining Option: Dine in\n");
            else if (diningOption == 2)
                printf("Dining Option: Reservation\n");
            else if (diningOption == 3)
                printf("Dining Option: Take out\n");
            printf("Total Bill: %.2f\n", total);
            if (diningOption == 2) {
                printf("Reserved Table: %d\n", reservedTable);
                printf("Reservation Date/Time: %s\n", reservationDateTime);
            }

            if (get_yes_no("Confirm Order?") == 'N') {
                printf("Order cancelled.\n");
                if (diningOption == 2 && reservedTable > 0) {
                    reservedTables[reservedTable - 1] = 0;
                    save_tables(reservedTables);
                }
            } else {
                FILE *fp, *fp2;
                printf("\n--- RECEIPT ---\n");
                printf("Name: %s\nContact: %s\n", customerName, contactInfo);
                printf("Items Ordered:\n");
                for (i = 0; i < orderCount; i++) {
                    int idx = orders[i].itemIndex;
                    printf("%s x%d = %.2f\n", menu[idx].name, orders[i].quantity, menu[idx].price * orders[i].quantity);
                }
                if (diningOption == 1)
                    printf("Dining Option: Dine in\n");
                else if (diningOption == 2)
                    printf("Dining Option: Reservation\n");
                else if (diningOption == 3)
                    printf("Dining Option: Take out\n");
                printf("Total Bill: %.2f\n", total);
                if (diningOption == 2) {
                    printf("Reserved Table: %d\n", reservedTable);
                    printf("Reservation Date/Time: %s\n", reservationDateTime);
                }
                printf("Thank you for your order!\n\n");

                fp = fopen("receipt.txt", "w");
                if (fp != NULL) {
                    fprintf(fp, "--- RECEIPT ---\n");
                    fprintf(fp, "Name: %s\nContact: %s\n", customerName, contactInfo);
                    fprintf(fp, "Items Ordered:\n");
                    for (i = 0; i < orderCount; i++) {
                        int idx = orders[i].itemIndex;
                        fprintf(fp, "%s x%d = %.2f\n", menu[idx].name, orders[i].quantity, menu[idx].price * orders[i].quantity);
                    }
                    if (diningOption == 1)
                        fprintf(fp, "Dining Option: Dine in\n");
                    else if (diningOption == 2)
                        fprintf(fp, "Dining Option: Reservation\n");
                    else if (diningOption == 3)
                        fprintf(fp, "Dining Option: Take out\n");
                    fprintf(fp, "Total Bill: %.2f\n", total);
                    if (diningOption == 2) {
                        fprintf(fp, "Reserved Table: %d\n", reservedTable);
                        fprintf(fp, "Reservation Date/Time: %s\n", reservationDateTime);
                    }
                    fprintf(fp, "Thank you for your order!\n");
                    fclose(fp);
                    printf("Receipt saved to receipt.txt\n");
                } else {
                    printf("Error: Could not save receipt to file.\n");
                }

                fp2 = fopen("history.txt", "a");
                if (fp2 != NULL) {
                    fprintf(fp2, "--- ORDER ---\n");
                    fprintf(fp2, "Name: %s\nContact: %s\n", customerName, contactInfo);
                    fprintf(fp2, "Items Ordered:\n");
                    for (i = 0; i < orderCount; i++) {
                        int idx = orders[i].itemIndex;
                        fprintf(fp2, "%s x%d = %.2f\n", menu[idx].name, orders[i].quantity, menu[idx].price * orders[i].quantity);
                    }
                    if (diningOption == 1)
                        fprintf(fp2, "Dining Option: Dine in\n");
                    else if (diningOption == 2)
                        fprintf(fp2, "Dining Option: Reservation\n");
                    else if (diningOption == 3)
                        fprintf(fp2, "Dining Option: Take out\n");
                    fprintf(fp2, "Total Bill: %.2f\n", total);
                    if (diningOption == 2) {
                        fprintf(fp2, "Reserved Table: %d\n", reservedTable);
                        fprintf(fp2, "Reservation Date/Time: %s\n", reservationDateTime);
                    }
                    fprintf(fp2, "---------------------\n");
                    fclose(fp2);
                }

                fp2 = fopen("sales.txt", "a");
                if (fp2 != NULL) {
                    fprintf(fp2, "%.2f\n", total);
                    fclose(fp2);
                }

                totalSalesToday += total;
            }

            // RESTART OR END PROMPT
            {
                char restartChoice[10];
                int validRestart = 0;
                while (!validRestart) {
                    printf("Would you like to go back to the start or end it? (Type 'start' or 'end'): ");
                    scanf("%s", restartChoice);
                    fflush(stdin);
                    if (strcmpi(restartChoice, "start") == 0) {
                        validRestart = 1; // Loop continues
                    } else if (strcmpi(restartChoice, "end") == 0) {
                        // Instead of exiting, break to main menu
                        goto main_menu_restart;
                    } else {
                        printf("Invalid input. Please type 'start' or 'end'.\n");
                    }
                }
            }
        }
main_menu_restart:
        ; // label for breaking to main menu
    }
    getch();
}