#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
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

// Helper function to clear input buffer safely
void clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void save_tables(int reservedTables[]) {
    FILE *fp = fopen("tables.txt", "w");
    if (fp != NULL) {
        for (int i = 0; i < MAX_TABLES; i++)
            fprintf(fp, "%d ", reservedTables[i]);
        fclose(fp);
    }
}

void load_tables(int reservedTables[]) {
    FILE *fp = fopen("tables.txt", "r");
    if (fp != NULL) {
        for (int i = 0; i < MAX_TABLES; i++)
            fscanf(fp, "%d", &reservedTables[i]);
        fclose(fp);
    } else {
        for (int i = 0; i < MAX_TABLES; i++)
            reservedTables[i] = 0;
    }
}

void reset_tables(int reservedTables[]) {
    for (int i = 0; i < MAX_TABLES; i++)
        reservedTables[i] = 0;
    save_tables(reservedTables);
    printf("All table reservations have been reset.\n");
}

char get_yes_no(const char *prompt) {
    char answer[10];
    while (1) {
        printf("%s (Y/N): ", prompt);
        scanf("%9s", answer);
        clear_buffer();
        if (answer[0] == 'Y' || answer[0] == 'y') return 'Y';
        if (answer[0] == 'N' || answer[0] == 'n') return 'N';
        printf("Invalid input. Please enter Y or N.\n");
    }
}

void view_order_history() {
    FILE *fp = fopen("history.txt", "r");
    char line[256];
    printf("\n--- ORDER HISTORY ---\n");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) printf("%s", line);
        fclose(fp);
    } else {
        printf("No order history found.\n");
    }
    printf("---------------------\n");
}

void view_total_sales() {
    FILE *fp = fopen("sales.txt", "r");
    float totalSales = 0, sale;
    printf("\n--- TOTAL SALES ---\n");
    if (fp != NULL) {
        while (fscanf(fp, "%f", &sale) == 1) totalSales += sale;
        fclose(fp);
    }
    printf("Total Sales for the Day: %.2f\n", totalSales);
    printf("---------------------\n");
}

void reset_history() {
    fclose(fopen("history.txt", "w"));
    fclose(fopen("sales.txt", "w"));
    printf("History and Sales logs have been reset.\n");
}

void admin_panel(int reservedTables[]) {
    int admin_choice;
    while (1) {
        printf("\n--- ADMIN PANEL ---\n1. View History\n2. View Sales\n3. Reset History\n4. Reset Tables\n5. Exit\nChoice: ");
        if (scanf("%d", &admin_choice) != 1) {
            clear_buffer();
            continue;
        }
        if (admin_choice == 5) break;
        switch (admin_choice) {
            case 1: view_order_history(); break;
            case 2: view_total_sales(); break;
            case 3: reset_history(); break;
            case 4: reset_tables(reservedTables); break;
            default: printf("Invalid choice.\n");
        }
    }
}

void trim(char *str) {
    size_t len = strlen(str);
    while(len > 0 && (str[len-1] == ' ' || str[len-1] == '\n' || str[len-1] == '\r')) {
        str[--len] = '\0';
    }
}

int main() {
    char loginType[10], password[20], customerName[50], contactInfo[50];
    struct MenuItem menu[MAX_ITEMS] = {
        {"Spaghetti", 120.0}, {"Burger", 99.0}, {"Pizza", 150.0}, {"Salad", 80.0}, {"Juice", 40.0}
    };
    int reservedTables[MAX_TABLES];
    load_tables(reservedTables);

    while (1) {
        printf("\n--- RESTAURANT SYSTEM ---\n(admin / customer / end): ");
        scanf("%9s", loginType);
        clear_buffer();

        if (strcmp(loginType, "end") == 0) break;

        if (strcmp(loginType, "admin") == 0) {
            printf("Password: ");
            scanf("%19s", password);
            clear_buffer();
            if (strcmp(password, ADMIN_PASSWORD) == 0) admin_panel(reservedTables);
            else printf("Access Denied.\n");
            continue;
        }

        // CUSTOMER LOGIC
        struct Order orders[10];
        int orderCount = 0, choice, qty, diningOption, reservedTable = 0;
        float total = 0;

        printf("\nEnter Name: ");
        fgets(customerName, 50, stdin);
        trim(customerName);
        printf("Contact Info: ");
        fgets(contactInfo, 50, stdin);
        trim(contactInfo);

        // Ordering Loop
        while (1) {
            printf("\n--- MENU ---\n");
            for (int i = 0; i < MAX_ITEMS; i++) printf("%d. %s - %.2f\n", i+1, menu[i].name, menu[i].price);
            printf("0. Finish Order | 6. Remove Item\nChoice: ");
            
            if (scanf("%d", &choice) != 1) { clear_buffer(); continue; }
            if (choice == 0) break;
            
            if (choice == 6 && orderCount > 0) {
                orderCount--; // Simple removal of last item
                printf("Last item removed.\n");
                continue;
            }

            if (choice >= 1 && choice <= MAX_ITEMS) {
                printf("Quantity: ");
                scanf("%d", &qty);
                orders[orderCount].itemIndex = choice - 1;
                orders[orderCount].quantity = qty;
                orderCount++;
            }
        }

        if (orderCount == 0) continue;

        printf("\n1. Dine-in\n2. Reservation\n3. Take-out\nOption: ");
        scanf("%d", &diningOption);

        if (diningOption == 2) {
            printf("Available: ");
            for(int i=0; i<MAX_TABLES; i++) if(!reservedTables[i]) printf("%d ", i+1);
            printf("\nSelect Table: ");
            scanf("%d", &reservedTable);
            if (reservedTable > 0 && reservedTable <= MAX_TABLES && !reservedTables[reservedTable-1]) {
                reservedTables[reservedTable-1] = 1;
                save_tables(reservedTables);
            } else {
                printf("Invalid table. Switching to Dine-in.\n");
                diningOption = 1;
            }
        }

        // Summary and Receipt
        total = 0;
        printf("\n--- RECEIPT ---\nCustomer: %s\n", customerName);
        for(int i=0; i<orderCount; i++) {
            int idx = orders[i].itemIndex;
            float sub = menu[idx].price * orders[i].quantity;
            printf("%s x%d: %.2f\n", menu[idx].name, orders[i].quantity, sub);
            total += sub;
        }
        printf("TOTAL: %.2f\n", total);

        if (get_yes_no("Confirm Order?") == 'Y') {
            FILE *hfp = fopen("history.txt", "a");
            FILE *sfp = fopen("sales.txt", "a");
            if (hfp) {
                fprintf(hfp, "Order: %s | Total: %.2f\n", customerName, total);
                fclose(hfp);
            }
            if (sfp) {
                fprintf(sfp, "%.2f\n", total);
                fclose(sfp);
            }
            printf("Order Successful!\n");
        } else if (diningOption == 2) {
            reservedTables[reservedTable-1] = 0; // Cancel reservation
            save_tables(reservedTables);
        }
    }

    printf("Goodbye!\n");
    return 0;
}