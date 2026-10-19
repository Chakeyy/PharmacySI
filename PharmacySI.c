#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ======================= CONSTANTS ======================= */

#define MAX             100
#define START_ID        101
#define LOW_STOCK_LIMIT 10
#define BOX_WIDTH       50
#define CURRENT_DATE    "2026-10-19"
#define INVENTORY_FILE  "inventory.txt"
#define SALES_FILE      "sales.txt"

/* ===================== DATA STRUCTURE ==================== */

typedef struct {
    int id;
    char name[50];
    char category[30];
    float price;
    int quantity;
    char expiry[11];
} Medicine;

/* ================== FUNCTION PROTOTYPES ================== */

void clearInputBuffer(void);
void clearScreen(void);
void pauseScreen(void);
void printBoxLine(void);
void printCentered(const char text[]);
void printHeader(const char title[], const char subtitle[]);
void sanitizePipes(char str[]);
int  isExpired(const char expiry[]);
int  findMedicineById(const Medicine meds[], int count, int id);
int  promptMedicineIndex(const Medicine meds[], int count, const char prompt[]);
int  readAndFormatExpiry(char expiry[]);

void initDataFiles(void);
int  loadInventory(Medicine meds[], int *count);
int  saveInventory(const Medicine meds[], int count);
int  logSale(int id, const char name[], int quantity, float total);
void getSalesSummary(int *salesCount, float *totalRevenue);

void addMedicine(Medicine meds[], int *count);
void updateMedicine(Medicine meds[], int count);
void viewMedicines(const Medicine meds[], int count);
void removeMedicine(Medicine meds[], int *count);

void sortAndViewMedicines(const Medicine meds[], int count);

void recordSale(Medicine meds[], int count);

/* ========================= MAIN ========================== */

int main(void) {
    Medicine meds[MAX];
    int count = 0;
    int choice;
    int skippedRows;

    skippedRows = loadInventory(meds, &count);
    if (skippedRows > 0) {
        printf("\n  [!] %d invalid row(s) in %s were skipped.\n", skippedRows, INVENTORY_FILE);
        printf("      They will be cleaned up the next time inventory is saved.\n");
        pauseScreen();
    }

    do {
        int lowStockCount = 0;
        int expiredCount = 0;

        for (int i = 0; i < count; i++) {
            if (isExpired(meds[i].expiry)) {
                expiredCount++;
            } else if (meds[i].quantity <= LOW_STOCK_LIMIT) {
                lowStockCount++;
            }
        }

        clearScreen();
        printf("+==================================================+\n");
        printf("|            PHARMACY MANAGEMENT SYSTEM            |\n");
        printf("|          Sales & Inventory Control (C99)         |\n");
        printf("+==================================================+\n");
        printf("|  Date: %-10s            Items: %-3d / %-3d    |\n",
               CURRENT_DATE, count, MAX);
        printf("|  Low/Out Stock: %-3d          Expired: %-3d        |\n",
               lowStockCount, expiredCount);
        printf("+--------------------------------------------------+\n");
        printf("|  INVENTORY                                       |\n");
        printf("|    [1] Add Medicine (New Batch)                  |\n");
        printf("|    [2] Update Medicine                           |\n");
        printf("|    [3] View & Sort Inventory                     |\n");
        printf("|    [4] Remove Medicine                           |\n");
        printf("|                                                  |\n");
        printf("|  TRANSACTIONS                                    |\n");
        printf("|    [5] Sell Medicine                             |\n");
        printf("|                                                  |\n");
        printf("|  SYSTEM                                          |\n");
        printf("|    [0] Exit Program                              |\n");
        printf("+==================================================+\n");
        printf("  Select an option (0-5): ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("\n  [!] Invalid input! Please enter a number from 0 to 5.\n");
            pauseScreen();
            choice = -1;
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
                sortAndViewMedicines(meds, count);
                break;

            case 4:
                removeMedicine(meds, &count);
                break;

            case 5:
                recordSale(meds, count);
                break;

            case 0:
                printf("\n  Exiting Pharmacy Management System. Goodbye!\n");
                break;

            default:
                printf("\n  [!] Invalid option! Please choose 0 to 5.\n");
                pauseScreen();
                break;
        }

    } while (choice != 0);

    return 0;
}

/* ================ UTILITY / INPUT HELPERS ================ */

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clearScreen(void) {
    system("cls");
}

void pauseScreen(void) {
    int c;
    printf("\n  Press Enter to continue...");
    while ((c = getchar()) != '\n' && c != EOF);
}

void printBoxLine(void) {
    printf("+");
    for (int i = 0; i < BOX_WIDTH; i++) {
        printf("-");
    }
    printf("+\n");
}

void printCentered(const char text[]) {
    int length = (int)strlen(text);
    int leftSpaces = (BOX_WIDTH - length) / 2;
    int rightSpaces = BOX_WIDTH - length - leftSpaces;

    printf("|");
    for (int i = 0; i < leftSpaces; i++) {
        printf(" ");
    }
    printf("%s", text);
    for (int i = 0; i < rightSpaces; i++) {
        printf(" ");
    }
    printf("|\n");
}

void printHeader(const char title[], const char subtitle[]) {
    printBoxLine();
    printCentered(title);
    if (subtitle[0] != '\0') {
        printCentered(subtitle);
    }
    printBoxLine();
}

void sanitizePipes(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '|') {
            str[i] = '-';
        }
    }
}

int isExpired(const char expiry[]) {
    return strcmp(expiry, CURRENT_DATE) < 0;
}

int findMedicineById(const Medicine meds[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (meds[i].id == id) {
            return i;
        }
    }
    return -1;
}

int promptMedicineIndex(const Medicine meds[], int count, const char prompt[]) {
    int id, index;

    while (1) {
        printf("  %-37s: ", prompt);
        if (scanf("%d", &id) != 1) {
            clearInputBuffer();
            printf("  [!] Please enter a valid numeric ID (or 0 to cancel).\n");
            continue;
        }
        clearInputBuffer();

        if (id == 0) {
            return -1;
        }

        index = findMedicineById(meds, count, id);
        if (index != -1) {
            return index;
        }
        printf("  [!] Medicine ID %d was not found. Please try again.\n", id);
    }
}

int readAndFormatExpiry(char expiry[]) {
    char rawDate[20];
    int length, year, month, day, maxDay;
    int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (scanf(" %19[^\n]", rawDate) != 1) {
        clearInputBuffer();
        return 0;
    }
    clearInputBuffer();

    if (strcmp(rawDate, "0") == 0) {
        return -1;
    }

    length = (int)strlen(rawDate);

    if (length == 8) {
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
    else if (length == 10 &&
             (rawDate[4] == '-' || rawDate[4] == ' ') &&
             (rawDate[7] == '-' || rawDate[7] == ' ')) {
        strcpy(expiry, rawDate);
        expiry[4] = '-';
        expiry[7] = '-';
    }
    else {
        return 0;
    }

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            continue;
        }
        if (expiry[i] < '0' || expiry[i] > '9') {
            return 0;
        }
    }

    year  = (expiry[0] - '0') * 1000 + (expiry[1] - '0') * 100 +
            (expiry[2] - '0') * 10   + (expiry[3] - '0');
    month = (expiry[5] - '0') * 10 + (expiry[6] - '0');
    day   = (expiry[8] - '0') * 10 + (expiry[9] - '0');

    if (year < 2000 || year > 2100) {
        return 0;
    }
    if (month < 1 || month > 12) {
        return 0;
    }

    maxDay = daysInMonth[month - 1];
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) {
        maxDay = 29;
    }
    if (day < 1 || day > maxDay) {
        return 0;
    }

    return 1;
}

/* ===================== FILE HANDLING ===================== */

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

int loadInventory(Medicine meds[], int *count) {
    char line[256];
    Medicine temp;
    int skippedRows = 0;
    FILE *fp;

    initDataFiles();

    *count = 0;
    fp = fopen(INVENTORY_FILE, "r");
    if (fp == NULL) {
        return 0;
    }

    while (*count < MAX && fgets(line, sizeof(line), fp) != NULL) {
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') {
            continue;
        }

        if (sscanf(line, "%d|%49[^|]|%29[^|]|%f|%d|%10[^\r\n]",
                   &temp.id, temp.name, temp.category,
                   &temp.price, &temp.quantity, temp.expiry) == 6 &&
            temp.id > 0 && temp.price > 0.0f && temp.quantity >= 0 &&
            findMedicineById(meds, *count, temp.id) == -1) {
            meds[*count] = temp;
            (*count)++;
        } else {
            skippedRows++;
        }
    }

    fclose(fp);
    return skippedRows;
}

int saveInventory(const Medicine meds[], int count) {
    FILE *fp = fopen(INVENTORY_FILE, "w");
    if (fp == NULL) {
        return 0;
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

    if (fclose(fp) == EOF) {
        return 0;
    }
    return 1;
}

int logSale(int id, const char name[], int quantity, float total) {
    FILE *fp = fopen(SALES_FILE, "a");
    if (fp == NULL) {
        return 0;
    }

    if (fprintf(fp, "%d|%s|%d|%.2f\n", id, name, quantity, total) < 0) {
        fclose(fp);
        return 0;
    }

    if (fclose(fp) == EOF) {
        return 0;
    }
    return 1;
}

void getSalesSummary(int *salesCount, float *totalRevenue) {
    char line[256];
    char name[50];
    int id, quantity;
    float total;
    FILE *fp;

    *salesCount = 0;
    *totalRevenue = 0.0f;

    fp = fopen(SALES_FILE, "r");
    if (fp == NULL) {
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "%d|%49[^|]|%d|%f", &id, name, &quantity, &total) == 4) {
            (*salesCount)++;
            *totalRevenue += total;
        }
    }

    fclose(fp);
}

/* ================= INVENTORY OPERATIONS ================== */

void addMedicine(Medicine meds[], int *count) {
    Medicine newMed;
    char subtitle[BOX_WIDTH + 1];
    int expiryStatus;

    clearScreen();

    if (*count >= MAX) {
        printf("\n  [!] Inventory is full (%d / %d)!\n", *count, MAX);
        pauseScreen();
        return;
    }

    newMed.id = START_ID;
    for (int i = 0; i < *count; i++) {
        if (meds[i].id >= newMed.id) {
            newMed.id = meds[i].id + 1;
        }
    }

    sprintf(subtitle, "Batch ID: %d  |  Enter 0 to Cancel", newMed.id);
    printHeader("[1] ADD MEDICINE (NEW BATCH)", subtitle);
    printf("\n");

    while (1) {
        printf("  Medicine Name                        : ");
        if (scanf(" %49[^\n]", newMed.name) == 1) {
            clearInputBuffer();
            if (strcmp(newMed.name, "0") == 0) {
                return;
            }
            sanitizePipes(newMed.name);
            break;
        }
        clearInputBuffer();
        printf("  [!] Please enter a medicine name.\n");
    }

    while (1) {
        printf("  Category (e.g. Pain Relief)          : ");
        if (scanf(" %29[^\n]", newMed.category) == 1) {
            clearInputBuffer();
            if (strcmp(newMed.category, "0") == 0) {
                return;
            }
            sanitizePipes(newMed.category);
            break;
        }
        clearInputBuffer();
        printf("  [!] Please enter a category.\n");
    }

    while (1) {
        printf("  Unit Price                           : ");
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
        printf("  [!] Price must be greater than 0.00 (or 0 to cancel).\n");
    }

    while (1) {
        printf("  Stock Quantity                       : ");
        if (scanf("%d", &newMed.quantity) == 1) {
            clearInputBuffer();
            if (newMed.quantity >= 0) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Quantity must be 0 or greater.\n");
    }

    while (1) {
        printf("  Expiry Date (YYYYMMDD / YYYY-MM-DD)  : ");
        expiryStatus = readAndFormatExpiry(newMed.expiry);
        if (expiryStatus == -1) {
            return;
        }
        if (expiryStatus == 1) {
            break;
        }
        printf("  [!] Invalid date! Enter a real date between 2000 and 2100 (e.g. 20270315).\n");
    }

    meds[*count] = newMed;
    (*count)++;

    if (!saveInventory(meds, *count)) {
        (*count)--;
        printf("\n  [!] Error: Could not save to %s. Batch was NOT added.\n", INVENTORY_FILE);
        pauseScreen();
        return;
    }

    printf("\n  [+] '%s' (Batch ID: %d | Exp: %s) added to inventory!\n",
           newMed.name, newMed.id, newMed.expiry);
    if (isExpired(newMed.expiry)) {
        printf("  [!] Note: This batch's expiry date (%s) has already passed!\n", newMed.expiry);
    }
    pauseScreen();
}

void updateMedicine(Medicine meds[], int count) {
    Medicine backup;
    int index, action, expiryStatus;
    float newPrice;
    int newQuantity;
    char newExpiry[11];

    clearScreen();

    if (count == 0) {
        printf("\n  [!] Inventory is currently empty.\n");
        pauseScreen();
        return;
    }

    viewMedicines(meds, count);
    printf("\n");
    printHeader("[2] UPDATE MEDICINE", "Enter 0 to Cancel");

    index = promptMedicineIndex(meds, count, "Enter Medicine ID to Update");
    if (index == -1) {
        return;
    }

    printf("\n  +--- Selected Batch (ID: %-4d) -----------------+\n", meds[index].id);
    printf("  | Medicine       : %-28.28s |\n", meds[index].name);
    printf("  | Category       : %-28.28s |\n", meds[index].category);
    printf("  | [1] Unit Price : %-28.2f |\n", meds[index].price);
    printf("  | [2] Quantity   : %-28d |\n", meds[index].quantity);
    printf("  | [3] Expiry     : %-28s |\n", meds[index].expiry);
    printf("  | [4] Update All Fields                         |\n");
    printf("  | [0] Cancel                                    |\n");
    printf("  +-----------------------------------------------+\n\n");

    while (1) {
        printf("  What would you like to update? (0-4) : ");
        if (scanf("%d", &action) == 1) {
            clearInputBuffer();
            if (action == 0) {
                return;
            }
            if (action >= 1 && action <= 4) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Please select 1 to 4 (or 0 to cancel).\n");
    }

    newPrice = meds[index].price;
    newQuantity = meds[index].quantity;
    strcpy(newExpiry, meds[index].expiry);

    if (action == 1 || action == 4) {
        while (1) {
            printf("  New Unit Price (Current: %.2f)       : ", meds[index].price);
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
            printf("  [!] Price must be greater than 0.00 (or 0 to cancel).\n");
        }
    }

    if (action == 2 || action == 4) {
        while (1) {
            printf("  New Stock Quantity (Current: %d)     : ", meds[index].quantity);
            if (scanf("%d", &newQuantity) == 1) {
                clearInputBuffer();
                if (newQuantity >= 0) {
                    break;
                }
            } else {
                clearInputBuffer();
            }
            printf("  [!] Quantity must be 0 or greater.\n");
        }
    }

    if (action == 3 || action == 4) {
        while (1) {
            printf("  Corrected Expiry (Current: %s) : ", meds[index].expiry);
            expiryStatus = readAndFormatExpiry(newExpiry);
            if (expiryStatus == -1) {
                return;
            }
            if (expiryStatus == 1) {
                break;
            }
            printf("  [!] Invalid date! Enter YYYYMMDD or YYYY-MM-DD (or 0 to cancel).\n");
        }
    }

    backup = meds[index];
    meds[index].price = newPrice;
    meds[index].quantity = newQuantity;
    strcpy(meds[index].expiry, newExpiry);

    if (!saveInventory(meds, count)) {
        meds[index] = backup;
        printf("\n  [!] Error: Could not save to %s. Changes were reverted.\n", INVENTORY_FILE);
        pauseScreen();
        return;
    }

    printf("\n  [+] '%s' (ID: %d) updated successfully!\n", meds[index].name, meds[index].id);
    pauseScreen();
}

void viewMedicines(const Medicine meds[], int count) {
    const char *status;

    if (count == 0) {
        printf("\n  [!] Inventory is currently empty.\n");
        return;
    }

    printf("+====================================================================================================+\n");
    printf("|                                         MEDICINE INVENTORY                                         |\n");
    printf("+====================================================================================================+\n");
    printf("| %-8s | %-20s | %-15s | %10s | %8s | %-10s | %-11s |\n",
           "Batch ID", "Name", "Category", "Price", "Stock", "Expiry", "Status");
    printf("+----------+----------------------+-----------------+------------+----------+------------+-------------+\n");

    for (int i = 0; i < count; i++) {
        if (isExpired(meds[i].expiry)) {
            status = "[EXPIRED]";
        } else if (meds[i].quantity == 0) {
            status = "[OUT]";
        } else if (meds[i].quantity <= LOW_STOCK_LIMIT) {
            status = "[LOW STOCK]";
        } else {
            status = "OK";
        }

        printf("| %-8d | %-20.20s | %-15.15s | %10.2f | %8d | %-10s | %-11s |\n",
               meds[i].id,
               meds[i].name,
               meds[i].category,
               meds[i].price,
               meds[i].quantity,
               meds[i].expiry,
               status);
    }

    printf("+----------+----------------------+-----------------+------------+----------+------------+-------------+\n");
    printf("  Total Batches Listed: %d   (Current Date: %s)\n", count, CURRENT_DATE);
}

void removeMedicine(Medicine meds[], int *count) {
    int index, confirm;

    clearScreen();

    if (*count == 0) {
        printf("\n  [!] Inventory is currently empty.\n");
        pauseScreen();
        return;
    }

    viewMedicines(meds, *count);
    printf("\n");
    printHeader("[4] REMOVE MEDICINE", "Enter 0 to Cancel");

    index = promptMedicineIndex(meds, *count, "Enter Medicine ID to Remove");
    if (index == -1) {
        return;
    }

    printf("\n  Selected: %s (ID: %d | Stock: %d | Exp: %s)\n",
           meds[index].name, meds[index].id, meds[index].quantity, meds[index].expiry);

    while (1) {
        printf("  Remove this batch permanently? (1 = Yes, 0 = Cancel): ");
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
        printf("  [!] Please enter 1 to confirm removal or 0 to cancel.\n");
    }

    for (int i = index; i < *count - 1; i++) {
        meds[i] = meds[i + 1];
    }
    (*count)--;

    if (!saveInventory(meds, *count)) {
        printf("\n  [!] Warning: Removed from memory, but could not save to %s!\n", INVENTORY_FILE);
        pauseScreen();
        return;
    }

    printf("\n  [+] Medicine batch removed from inventory.\n");
    pauseScreen();
}

/* ======================== SORTING ======================== */

void sortAndViewMedicines(const Medicine meds[], int count) {
    Medicine tempMeds[MAX];
    Medicine temp;
    int field, order;
    int i, j, swap;

    clearScreen();

    if (count == 0) {
        printf("\n  [!] Inventory is currently empty.\n");
        pauseScreen();
        return;
    }

    for (i = 0; i < count; i++) {
        tempMeds[i] = meds[i];
    }

    while (1) {
        clearScreen();
        viewMedicines(tempMeds, count);

        printf("\n  +----------------------------------------------------------------------+\n");
        printf("  | Sort By: [1] Name  [2] Price  [3] Stock  [4] Expiry  |  [0] Main Menu |\n");
        printf("  +----------------------------------------------------------------------+\n");
        printf("  Select option (0-4): ");

        if (scanf("%d", &field) != 1) {
            clearInputBuffer();
            printf("  [!] Invalid input! Please enter 0 to 4.\n");
            pauseScreen();
            continue;
        }
        clearInputBuffer();

        if (field == 0) {
            return;
        }
        if (field < 1 || field > 4) {
            printf("  [!] Please select 1 to 4 to sort, or 0 to return.\n");
            pauseScreen();
            continue;
        }

        while (1) {
            printf("  Order: [1] Ascending (A-Z / Low-High)  [2] Descending  |  [0] Cancel: ");
            if (scanf("%d", &order) == 1) {
                clearInputBuffer();
                if (order >= 0 && order <= 2) {
                    break;
                }
            } else {
                clearInputBuffer();
            }
            printf("  [!] Please enter 1, 2, or 0.\n");
        }

        if (order == 0) {
            continue;
        }

        for (i = 0; i < count - 1; i++) {
            for (j = 0; j < count - i - 1; j++) {
                swap = 0;

                if (field == 1) {
                    int cmp = strcmp(tempMeds[j].name, tempMeds[j + 1].name);
                    if ((order == 1 && cmp > 0) || (order == 2 && cmp < 0)) {
                        swap = 1;
                    }
                }
                else if (field == 2) {
                    if ((order == 1 && tempMeds[j].price > tempMeds[j + 1].price) ||
                        (order == 2 && tempMeds[j].price < tempMeds[j + 1].price)) {
                        swap = 1;
                    }
                }
                else if (field == 3) {
                    if ((order == 1 && tempMeds[j].quantity > tempMeds[j + 1].quantity) ||
                        (order == 2 && tempMeds[j].quantity < tempMeds[j + 1].quantity)) {
                        swap = 1;
                    }
                }
                else if (field == 4) {
                    int cmp = strcmp(tempMeds[j].expiry, tempMeds[j + 1].expiry);
                    if ((order == 1 && cmp > 0) || (order == 2 && cmp < 0)) {
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
    }
}

/* ========================= SALES ========================= */

void recordSale(Medicine meds[], int count) {
    int index, quantity;
    int sellableItems = 0;
    int salesCount;
    float total, totalRevenue;

    clearScreen();

    if (count == 0) {
        printf("\n  [!] Inventory is currently empty.\n");
        pauseScreen();
        return;
    }

    for (int i = 0; i < count; i++) {
        if (meds[i].quantity > 0 && !isExpired(meds[i].expiry)) {
            sellableItems++;
        }
    }

    viewMedicines(meds, count);

    if (sellableItems == 0) {
        printf("\n  [!] No sellable stock available (all batches are out of stock or expired).\n");
        pauseScreen();
        return;
    }

    printf("\n");
    printHeader("[5] SELL MEDICINE", "Enter 0 to Cancel");

    while (1) {
        index = promptMedicineIndex(meds, count, "Enter Medicine ID to Sell");
        if (index == -1) {
            return;
        }
        if (isExpired(meds[index].expiry)) {
            printf("  [!] '%s' (ID: %d) expired on %s and cannot be sold!\n",
                   meds[index].name, meds[index].id, meds[index].expiry);
            continue;
        }
        if (meds[index].quantity == 0) {
            printf("  [!] '%s' (ID: %d) is out of stock! Please select another batch.\n",
                   meds[index].name, meds[index].id);
            continue;
        }
        break;
    }

    printf("\n  +--- Selected Batch for Sale -------------------+\n");
    printf("  | Medicine        : %-27.27s |\n", meds[index].name);
    printf("  | Category        : %-27.27s |\n", meds[index].category);
    printf("  | Unit Price      : %-27.2f |\n", meds[index].price);
    printf("  | Available Stock : %-27d |\n", meds[index].quantity);
    printf("  | Expiry Date     : %-27s |\n", meds[index].expiry);
    printf("  +-----------------------------------------------+\n\n");

    while (1) {
        printf("  Quantity to Sell (1 - %-4d, 0 = Cancel): ", meds[index].quantity);
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
        printf("  [!] Please enter a quantity from 1 to %d (or 0 to cancel).\n", meds[index].quantity);
    }

    total = meds[index].price * quantity;

    if (!logSale(meds[index].id, meds[index].name, quantity, total)) {
        printf("\n  [!] Error: Could not write to %s. Sale was NOT recorded.\n", SALES_FILE);
        pauseScreen();
        return;
    }

    meds[index].quantity -= quantity;
    getSalesSummary(&salesCount, &totalRevenue);

    printf("\n  +================================================+\n");
    printf("  |                 SALES RECEIPT                  |\n");
    printf("  +================================================+\n");
    printf("  | Batch ID      : %-30d |\n", meds[index].id);
    printf("  | Medicine      : %-30.30s |\n", meds[index].name);
    printf("  | Unit Price    : %-30.2f |\n", meds[index].price);
    printf("  | Quantity Sold : %-30d |\n", quantity);
    printf("  +------------------------------------------------+\n");
    printf("  | TOTAL AMOUNT  : %-30.2f |\n", total);
    printf("  | Remaining     : %-30d |\n", meds[index].quantity);
    printf("  +------------------------------------------------+\n");
    printf("  | Total Sales   : %-4d   Total Revenue: %-8.2f |\n", salesCount, totalRevenue);
    printf("  +================================================+\n");

    if (meds[index].quantity == 0) {
        printf("\n  [!] Batch ID %d ('%s') is now OUT OF STOCK.\n",
               meds[index].id, meds[index].name);
    } else if (meds[index].quantity <= LOW_STOCK_LIMIT) {
        printf("\n  [!] LOW STOCK: Only %d unit(s) left in Batch ID %d ('%s').\n",
               meds[index].quantity, meds[index].id, meds[index].name);
    }

    if (!saveInventory(meds, count)) {
        printf("\n  [!] Warning: Sale logged, but %s could not be updated!\n", INVENTORY_FILE);
    }
    pauseScreen();
}
