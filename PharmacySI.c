#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define INVENTORY_FILE "inventory.txt"
#define SALES_FILE "sales.txt"

typedef struct {
    int id;
    char name[50];
    char category[30];
    float price;
    int quantity;
    char expiry[11];
} Medicine;

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clearScreen(void) {
    system("cls");
}

void pauseScreen(void) {
    printf("\n  Press Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Replaces any '|' character with '-' so user input cannot break pipe-delimited .txt files
void sanitizePipes(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '|') {
            str[i] = '-';
        }
    }
}

int findMedicineById(const Medicine meds[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (meds[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Ensures both inventory.txt and sales.txt exist immediately on program startup
void initDataFiles(void) {
    FILE *fp;

    fp = fopen(INVENTORY_FILE, "r");
    if (fp == NULL) {
        fp = fopen(INVENTORY_FILE, "w");
    }
    if (fp != NULL) {
        fclose(fp);
    }

    fp = fopen(SALES_FILE, "r");
    if (fp == NULL) {
        fp = fopen(SALES_FILE, "w");
    }
    if (fp != NULL) {
        fclose(fp);
    }
}

void loadInventory(Medicine meds[], int *count) {
    char line[256];
    Medicine temp;

    initDataFiles();

    FILE *fp = fopen(INVENTORY_FILE, "r");
    if (fp == NULL) {
        *count = 0;
        return;
    }

    *count = 0;
    while (*count < MAX && fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "%d|%49[^|]|%29[^|]|%f|%d|%10[^\r\n]",
                   &temp.id,
                   temp.name,
                   temp.category,
                   &temp.price,
                   &temp.quantity,
                   temp.expiry) == 6) {
            // Skip invalid or duplicate IDs if file was edited manually
            if (temp.id > 0 && temp.price > 0.0f && temp.quantity >= 0 &&
                findMedicineById(meds, *count, temp.id) == -1) {
                meds[*count] = temp;
                (*count)++;
            }
        }
    }
    fclose(fp);
}

void saveInventory(const Medicine meds[], int count) {
    FILE *fp = fopen(INVENTORY_FILE, "w");
    if (fp == NULL) {
        printf("\n  [!] Error opening %s for writing!\n", INVENTORY_FILE);
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d|%s|%s|%.2f|%d|%s\n",
                meds[i].id,
                meds[i].name,
                meds[i].category,
                meds[i].price,
                meds[i].quantity,
                meds[i].expiry);
    }
    fclose(fp);
}

void viewMedicines(const Medicine meds[], int count) {
    if (count == 0) {
        printf("\n  [!] The inventory is currently empty.\n");
        return;
    }

    printf("\n+====================================================================================================+\n");
    printf("|                                         MEDICINE INVENTORY                                         |\n");
    printf("+====================================================================================================+\n");
    printf("| %-8s | %-20s | %-15s | %10s | %8s | %-10s | %-11s |\n",
           "ID", "Name", "Category", "Price",
           "Quantity", "Expiry", "Status");
    printf("+----------+----------------------+-----------------+------------+----------+------------+-------------+\n");

    for (int i = 0; i < count; i++) {
        printf("| %-8d | %-20.20s | %-15.15s | %10.2f | %8d | %-10s | %-11s |\n",
               meds[i].id,
               meds[i].name,
               meds[i].category,
               meds[i].price,
               meds[i].quantity,
               meds[i].expiry,
               meds[i].quantity <= 10 ? "[LOW STOCK]" : "OK");
    }

    printf("+----------+----------------------+-----------------+------------+----------+------------+-------------+\n");
    printf("  Total Medicines Listed: %d\n", count);
}

int readAndFormatExpiry(char expiry[]) {
    char rawDate[20];
    int len;

    if (scanf(" %19[^\n]", rawDate) != 1) {
        clearInputBuffer();
        return 0;
    }
    clearInputBuffer();

    // Instant cancel if user enters 0
    if (strcmp(rawDate, "0") == 0) {
        return -1;
    }

    len = (int)strlen(rawDate);

    // Format 1: 8 digits without separators (YYYYMMDD -> YYYY-MM-DD)
    if (len == 8) {
        expiry[0] = rawDate[0];
        expiry[1] = rawDate[1];
        expiry[2] = rawDate[2];
        expiry[3] = rawDate[3];
        expiry[4] = '-';
        expiry[5] = rawDate[4];
        expiry[6] = rawDate[5];
        expiry[7] = '-';
        expiry[8] = rawDate[6];
        expiry[9] = rawDate[7];
        expiry[10] = '\0';
    }
    // Format 2: 10 characters with spaces or dashes (YYYY MM DD or YYYY-MM-DD)
    else if (len == 10 &&
             (rawDate[4] == '-' || rawDate[4] == ' ') &&
             (rawDate[7] == '-' || rawDate[7] == ' ')) {
        strcpy(expiry, rawDate);
        expiry[4] = '-';
        expiry[7] = '-';
        expiry[10] = '\0';
    }
    else {
        return 0;
    }

    // Verify all date positions are digits (0-9)
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            continue;
        }
        if (expiry[i] < '0' || expiry[i] > '9') {
            return 0;
        }
    }

    // Validate month (01-12) and day (01-31)
    int month = (expiry[5] - '0') * 10 + (expiry[6] - '0');
    int day   = (expiry[8] - '0') * 10 + (expiry[9] - '0');
    if (month < 1 || month > 12 || day < 1 || day > 31) {
        return 0;
    }

    return 1;
}

void addMedicine(Medicine meds[], int *count) {
    Medicine newMed;
    int expiryStatus;

    if (*count >= MAX) {
        printf("\n  [!] Error: Inventory is full!\n");
        pauseScreen();
        return;
    }

    // Automatically generate the next available ID (starts at 101)
    newMed.id = 101;
    for (int i = 0; i < *count; i++) {
        if (meds[i].id >= newMed.id) {
            newMed.id = meds[i].id + 1;
        }
    }

    printf("\n+--------------------------------------------------+\n");
    printf("|                 [1] ADD MEDICINE                 |\n");
    printf("|        Auto-ID: %-4d |  Enter 0 to Cancel        |\n", newMed.id);
    printf("+--------------------------------------------------+\n\n");

    while (1) {
        printf("  Enter Medicine Name                  : ");
        if (scanf(" %49[^\n]", newMed.name) == 1) {
            clearInputBuffer();
            if (strcmp(newMed.name, "0") == 0) {
                return;
            }
            sanitizePipes(newMed.name);
            break;
        }
        clearInputBuffer();
        printf("  [!] Invalid medicine name! Please try again.\n");
    }

    while (1) {
        printf("  Enter Category                       : ");
        if (scanf(" %29[^\n]", newMed.category) == 1) {
            clearInputBuffer();
            if (strcmp(newMed.category, "0") == 0) {
                return;
            }
            sanitizePipes(newMed.category);
            break;
        }
        clearInputBuffer();
        printf("  [!] Invalid category! Please try again.\n");
    }

    while (1) {
        printf("  Enter Unit Price                     : ");
        if (scanf("%f", &newMed.price) == 1) {
            clearInputBuffer();
            if (newMed.price == 0.0f) {
                return;
            }
            if (newMed.price > 0.0f) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Price must be greater than 0.00! Please try again.\n");
    }

    while (1) {
        printf("  Enter Stock Quantity                 : ");
        if (scanf("%d", &newMed.quantity) == 1) {
            clearInputBuffer();
            if (newMed.quantity == 0) {
                return;
            }
            if (newMed.quantity > 0) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Quantity must be greater than 0! Please try again.\n");
    }

    while (1) {
        printf("  Enter Expiry (YYYYMMDD / YYYY MM DD) : ");
        expiryStatus = readAndFormatExpiry(newMed.expiry);
        if (expiryStatus == -1) {
            return;
        }
        if (expiryStatus == 1) {
            break;
        }
        printf("  [!] Invalid date! Use YYYYMMDD, YYYY MM DD, or YYYY-MM-DD.\n");
    }

    meds[*count] = newMed;
    (*count)++;

    saveInventory(meds, *count);

    printf("\n  [+] Medicine '%s' (ID: %d) added successfully!\n", newMed.name, newMed.id);
    pauseScreen();
}

void updateMedicine(Medicine meds[], int count) {
    int id, index, expiryStatus;
    float newPrice;
    int newQuantity;
    char newExpiry[11];

    if (count == 0) {
        printf("\n  [!] Inventory is currently empty!\n");
        pauseScreen();
        return;
    }

    viewMedicines(meds, count);

    printf("\n+--------------------------------------------------+\n");
    printf("|                [2] UPDATE MEDICINE               |\n");
    printf("|                Enter 0 to Cancel                 |\n");
    printf("+--------------------------------------------------+\n");

    while (1) {
        printf("  Enter Medicine ID                    : ");
        if (scanf("%d", &id) != 1) {
            clearInputBuffer();
            printf("  [!] Invalid ID! Please enter a numeric ID.\n");
            continue;
        }
        clearInputBuffer();

        if (id == 0) {
            return;
        }

        index = findMedicineById(meds, count, id);
        if (index != -1) {
            break;
        }
        printf("  [!] Medicine ID %d not found! Please try again.\n", id);
    }

    printf("\n  +--- Updating Record ---------------------------+\n");
    printf("  | Medicine : %-34.34s |\n", meds[index].name);
    printf("  | Category : %-34.34s |\n", meds[index].category);
    printf("  +-----------------------------------------------+\n\n");

    while (1) {
        printf("  Enter New Price (Current: %.2f)         : ", meds[index].price);
        if (scanf("%f", &newPrice) == 1) {
            clearInputBuffer();
            if (newPrice == 0.0f) {
                return;
            }
            if (newPrice > 0.0f) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Price must be greater than 0.00! Please try again.\n");
    }

    while (1) {
        printf("  Enter New Quantity (Current: %d)        : ", meds[index].quantity);
        if (scanf("%d", &newQuantity) == 1) {
            clearInputBuffer();
            if (newQuantity == 0) {
                return;
            }
            if (newQuantity > 0) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Quantity must be greater than 0! Please try again.\n");
    }

    while (1) {
        printf("  Enter New Expiry (Current: %s)  : ", meds[index].expiry);
        expiryStatus = readAndFormatExpiry(newExpiry);
        if (expiryStatus == -1) {
            return;
        }
        if (expiryStatus == 1) {
            break;
        }
        printf("  [!] Invalid date! Use YYYYMMDD, YYYY MM DD, or YYYY-MM-DD.\n");
    }

    meds[index].price = newPrice;
    meds[index].quantity = newQuantity;
    strcpy(meds[index].expiry, newExpiry);

    saveInventory(meds, count);

    printf("\n  [+] Medicine '%s' updated successfully!\n", meds[index].name);
    pauseScreen();
}

void removeMedicine(Medicine meds[], int *count) {
    int id, index, confirm;

    if (*count == 0) {
        printf("\n  [!] Inventory is currently empty!\n");
        pauseScreen();
        return;
    }

    viewMedicines(meds, *count);

    printf("\n+--------------------------------------------------+\n");
    printf("|                [4] REMOVE MEDICINE               |\n");
    printf("|                Enter 0 to Cancel                 |\n");
    printf("+--------------------------------------------------+\n");

    while (1) {
        printf("  Enter Medicine ID                    : ");
        if (scanf("%d", &id) != 1) {
            clearInputBuffer();
            printf("  [!] Invalid ID! Please enter a numeric ID.\n");
            continue;
        }
        clearInputBuffer();

        if (id == 0) {
            return;
        }

        index = findMedicineById(meds, *count, id);
        if (index != -1) {
            break;
        }
        printf("  [!] Medicine ID %d not found! Please try again.\n", id);
    }

    printf("\n  Selected: %s (ID: %d | Stock: %d)\n",
           meds[index].name, meds[index].id, meds[index].quantity);

    while (1) {
        printf("  Confirm removal? (1 = Yes, 0 = No)   : ");
        if (scanf("%d", &confirm) == 1) {
            clearInputBuffer();
            if (confirm == 0) {
                return;
            }
            if (confirm == 1) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Please enter 1 to confirm or 0 to cancel.\n");
    }

    for (int i = index; i < *count - 1; i++) {
        meds[i] = meds[i + 1];
    }

    (*count)--;

    saveInventory(meds, *count);

    printf("\n  [+] Medicine removed successfully!\n");
    pauseScreen();
}

void sortAndViewMedicines(const Medicine meds[], int count) {
    Medicine tempMeds[MAX];
    Medicine temp;
    int field, order;
    int i, j, swap;

    if (count == 0) {
        printf("\n  [!] Inventory is currently empty!\n");
        pauseScreen();
        return;
    }
    clearScreen();
    printf("+==================================================+\n");
    printf("|               [5] SORTED INVENTORY               |\n");
    printf("|                Enter 0 to Cancel                 |\n");
    printf("+==================================================+\n");
    printf("|  Sort By Field:                                  |\n");
    printf("|    [1] Name              [2] Price               |\n");
    printf("|    [3] Quantity          [4] Expiry Date         |\n");
    printf("+--------------------------------------------------+\n");

    while (1) {
        printf("  Select field (1-4): ");
        if (scanf("%d", &field) == 1) {
            clearInputBuffer();
            if (field == 0) {
                return;
            }
            if (field >= 1 && field <= 4) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Invalid choice! Please select 1 to 4 (or 0 to cancel).\n");
    }

    printf("\n+--------------------------------------------------+\n");
    printf("|  Sort Direction:                                 |\n");
    printf("|    [1] Ascending (A-Z / Low-High)                |\n");
    printf("|    [2] Descending (Z-A / High-Low)               |\n");
    printf("+--------------------------------------------------+\n");

    while (1) {
        printf("  Select order (1-2): ");
        if (scanf("%d", &order) == 1) {
            clearInputBuffer();
            if (order == 0) {
                return;
            }
            if (order == 1 || order == 2) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Invalid choice! Please select 1 or 2 (or 0 to cancel).\n");
    }

    // Copy the original inventory
    for (i = 0; i < count; i++) {
        tempMeds[i] = meds[i];
    }

    // Bubble Sort
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            swap = 0;

            if (field == 1) {
                // Sort by name
                int cmp = strcmp(tempMeds[j].name,
                                 tempMeds[j + 1].name);

                if ((order == 1 && cmp > 0) ||
                    (order == 2 && cmp < 0)) {
                    swap = 1;
                }
            }
            else if (field == 2) {
                // Sort by price
                if ((order == 1 &&
                     tempMeds[j].price > tempMeds[j + 1].price) ||
                    (order == 2 &&
                     tempMeds[j].price < tempMeds[j + 1].price)) {
                    swap = 1;
                }
            }
            else if (field == 3) {
                // Sort by quantity
                if ((order == 1 &&
                     tempMeds[j].quantity > tempMeds[j + 1].quantity) ||
                    (order == 2 &&
                     tempMeds[j].quantity < tempMeds[j + 1].quantity)) {
                    swap = 1;
                }
            }
            else if (field == 4) {
                // Sort by expiry date (YYYY-MM-DD)
                int cmp = strcmp(tempMeds[j].expiry,
                                 tempMeds[j + 1].expiry);

                if ((order == 1 && cmp > 0) ||
                    (order == 2 && cmp < 0)) {
                    swap = 1;
                }
            }

            if (swap) {
                temp = tempMeds[j];
                tempMeds[j] = tempMeds[j + 1];
                tempMeds[j + 1] = temp;
            }
        }
    }

    viewMedicines(tempMeds, count);
    pauseScreen();
}

void recordSale(Medicine meds[], int count) {
    int id, index, quantity;
    int availableItems = 0;
    float total;
    FILE *fp;

    if (count == 0) {
        printf("\n  [!] Inventory is currently empty!\n");
        pauseScreen();
        return;
    }

    for (int i = 0; i < count; i++) {
        if (meds[i].quantity > 0) {
            availableItems++;
        }
    }

    if (availableItems == 0) {
        viewMedicines(meds, count);
        printf("\n  [!] All medicines in inventory are currently out of stock!\n");
        pauseScreen();
        return;
    }

    viewMedicines(meds, count);

    printf("\n+--------------------------------------------------+\n");
    printf("|                [6] SELL MEDICINE                 |\n");
    printf("|                Enter 0 to Cancel                 |\n");
    printf("+--------------------------------------------------+\n");

    while (1) {
        printf("  Enter Medicine ID to Sell            : ");
        if (scanf("%d", &id) != 1) {
            clearInputBuffer();
            printf("  [!] Invalid ID! Please enter a numeric ID.\n");
            continue;
        }
        clearInputBuffer();

        if (id == 0) {
            return;
        }

        index = findMedicineById(meds, count, id);
        if (index == -1) {
            printf("  [!] Medicine ID %d not found! Please try again.\n", id);
            continue;
        }

        if (meds[index].quantity <= 0) {
            printf("  [!] '%s' is out of stock! Please select another ID.\n", meds[index].name);
            continue;
        }

        break;
    }

    printf("\n  +--- Medicine to Sell --------------------------+\n");
    printf("  | Medicine        : %-27.27s |\n", meds[index].name);
    printf("  | Category        : %-27.27s |\n", meds[index].category);
    printf("  | Unit Price      : %-27.2f |\n", meds[index].price);
    printf("  | Available Stock : %-27d |\n", meds[index].quantity);
    printf("  +-----------------------------------------------+\n\n");

    while (1) {
        printf("  Enter Quantity to Sell (Max: %-4d)   : ", meds[index].quantity);
        if (scanf("%d", &quantity) == 1) {
            clearInputBuffer();
            if (quantity == 0) {
                return;
            }
            if (quantity > 0 && quantity <= meds[index].quantity) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Invalid quantity! Enter 1 to %d (or 0 to cancel).\n", meds[index].quantity);
    }

    total = meds[index].price * quantity;

    // Open sales file before changing inventory
    fp = fopen(SALES_FILE, "a");

    if (fp == NULL) {
        printf("\n  [!] Error opening %s!\n", SALES_FILE);
        pauseScreen();
        return;
    }

    // Append transaction record
    if (fprintf(fp, "%d|%s|%d|%.2f\n",
                meds[index].id,
                meds[index].name,
                quantity,
                total) < 0) {
        printf("\n  [!] Error recording sale!\n");
        fclose(fp);
        pauseScreen();
        return;
    }

    if (fclose(fp) == EOF) {
        printf("\n  [!] Error saving sales record!\n");
        pauseScreen();
        return;
    }

    // Deduct sold quantity
    meds[index].quantity -= quantity;

    // Calculate cumulative sales summary from sales.txt
    int totalSalesCount = 0;
    float totalRevenue = 0.0f;
    fp = fopen(SALES_FILE, "r");
    if (fp != NULL) {
        char line[256];
        int sId, sQty;
        char sName[50];
        float sTotal;
        while (fgets(line, sizeof(line), fp) != NULL) {
            if (sscanf(line, "%d|%49[^|]|%d|%f", &sId, sName, &sQty, &sTotal) == 4) {
                totalSalesCount++;
                totalRevenue += sTotal;
            }
        }
        fclose(fp);
    }

    // Print receipt
    printf("\n  +================================================+\n");
    printf("  |                 SALES RECEIPT                  |\n");
    printf("  +================================================+\n");
    printf("  | Medicine ID   : %-30d |\n", meds[index].id);
    printf("  | Medicine      : %-30.30s |\n", meds[index].name);
    printf("  | Unit Price    : %-30.2f |\n", meds[index].price);
    printf("  | Quantity Sold : %-30d |\n", quantity);
    printf("  +------------------------------------------------+\n");
    printf("  | TOTAL SALE    : %-30.2f |\n", total);
    printf("  | Stock Left    : %-30d |\n", meds[index].quantity);
    printf("  +------------------------------------------------+\n");
    printf("  | Sales Logged  : %-4d   Total Revenue: %-8.2f |\n", totalSalesCount, totalRevenue);
    printf("  +================================================+\n");

    if (meds[index].quantity <= 10) {
        printf("\n  [!] LOW STOCK ALERT: Only %d unit(s) of '%s' left!\n",
               meds[index].quantity, meds[index].name);
    }

    // Save updated inventory
    saveInventory(meds, count);
    pauseScreen();
}

int main(void) {
    Medicine meds[MAX];
    int count = 0;
    int choice;

    loadInventory(meds, &count);

    do {
        int lowStockCount = 0;
        for (int i = 0; i < count; i++) {
            if (meds[i].quantity <= 10) {
                lowStockCount++;
            }
        }

        clearScreen();
        printf("+==================================================+\n");
        printf("|            PHARMACY MANAGEMENT SYSTEM            |\n");
        printf("|          Sales & Inventory Control (C99)         |\n");
        printf("+==================================================+\n");
        printf("|  Inventory: %-3d / %-3d      Low Stock Alerts: %-3d |\n",
               count, MAX, lowStockCount);
        printf("+--------------------------------------------------+\n");
        printf("|  INVENTORY MANAGEMENT                            |\n");
        printf("|    [1] Add Medicine                              |\n");
        printf("|    [2] Update Medicine                           |\n");
        printf("|    [3] View Medicine                             |\n");
        printf("|    [4] Remove Medicine                           |\n");
        printf("|                                                  |\n");
        printf("|  REPORTS & TRANSACTIONS                          |\n");
        printf("|    [5] Listing (Sorted)                          |\n");
        printf("|    [6] Sell Medicine                             |\n");
        printf("|                                                  |\n");
        printf("|  SYSTEM                                          |\n");
        printf("|    [0] Exit Program                              |\n");
        printf("+==================================================+\n");
        printf("  Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("\n  [!] Invalid choice! Please enter a number (0-6).\n");
            pauseScreen();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                addMedicine(meds, &count);
                break;

            case 2:
                updateMedicine(meds, count);
                break;

            case 3:
                viewMedicines(meds, count);
                pauseScreen();
                break;

            case 4:
                removeMedicine(meds, &count);
                break;

            case 5:
                sortAndViewMedicines(meds, count);
                break;

            case 6:
                recordSale(meds, count);
                break;

            case 0:
                printf("\n  Exiting Pharmacy Management System. Goodbye!\n");
                break;

            default:
                printf("\n  [!] Invalid choice! Please select 0 to 6.\n");
                pauseScreen();
                break;
        }

    } while (choice != 0);

    return 0;
}
