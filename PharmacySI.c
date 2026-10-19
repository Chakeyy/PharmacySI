/*
 * ============================================================
 *  PharmacySI.c - Pharmacy Sales and Inventory System (CCS104)
 * ============================================================
 *  Console program written in standard C (C99).
 *  - Medicines are stored in a fixed-size array (no malloc).
 *  - Data is saved to plain-text, pipe-delimited files:
 *      inventory.txt : id|name|category|price|quantity|expiry
 *      sales.txt     : id|name|quantity|total
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ======================= CONSTANTS ======================= */

#define MAX             100              /* Maximum number of medicines        */
#define START_ID        101              /* First auto-generated medicine ID   */
#define LOW_STOCK_LIMIT 10               /* Stock at or below this is "low"    */
#define BOX_WIDTH       50               /* Inner width of header boxes        */
#define INVENTORY_FILE  "inventory.txt"
#define SALES_FILE      "sales.txt"

/* ===================== DATA STRUCTURE ==================== */

typedef struct {
    int id;
    char name[50];
    char category[30];
    float price;
    int quantity;
    char expiry[11];   /* YYYY-MM-DD + '\0' */
} Medicine;

/* ================== FUNCTION PROTOTYPES ================== */

/* Utility / input helpers */
void clearInputBuffer(void);
void clearScreen(void);
void pauseScreen(void);
void printBoxLine(void);
void printCentered(const char text[]);
void printHeader(const char title[], const char subtitle[]);
void sanitizePipes(char str[]);
int  findMedicineById(const Medicine meds[], int count, int id);
int  findMedicineByName(const Medicine meds[], int count, const char name[]);
int  promptMedicineIndex(const Medicine meds[], int count, const char prompt[]);
int  readAndFormatExpiry(char expiry[]);

/* File handling */
void initDataFiles(void);
int  loadInventory(Medicine meds[], int *count);
int  saveInventory(const Medicine meds[], int count);
int  logSale(int id, const char name[], int quantity, float total);
void getSalesSummary(int *salesCount, float *totalRevenue);

/* Inventory operations */
void addMedicine(Medicine meds[], int *count);
void updateMedicine(Medicine meds[], int count);
void viewMedicines(const Medicine meds[], int count);
void removeMedicine(Medicine meds[], int *count);

/* Sorting */
void sortAndViewMedicines(const Medicine meds[], int count);

/* Sales */
void recordSale(Medicine meds[], int count);

/* ========================= MAIN ========================== */

int main(void) {
    Medicine meds[MAX];
    int count = 0;
    int choice;
    int skippedRows;

    /* Load saved data (also creates the .txt files if missing) */
    skippedRows = loadInventory(meds, &count);
    if (skippedRows > 0) {
        printf("\n  [!] %d invalid row(s) in %s were skipped.\n", skippedRows, INVENTORY_FILE);
        printf("      They will be removed the next time the inventory is saved.\n");
        pauseScreen();
    }

    do {
        /* Count medicines that are low or out of stock for the dashboard */
        int lowStockCount = 0;
        for (int i = 0; i < count; i++) {
            if (meds[i].quantity <= LOW_STOCK_LIMIT) {
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
            choice = -1;   /* keep the loop running */
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
                clearScreen();
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

/* ================ UTILITY / INPUT HELPERS ================ */

/* Removes leftover characters (including '\n') from the input buffer
 * so the next scanf/prompt does not get skipped. */
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Clears the console window (Windows "cls"). */
void clearScreen(void) {
    system("cls");
}

/* Waits for the user to press Enter before continuing. */
void pauseScreen(void) {
    int c;
    printf("\n  Press Enter to continue...");
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Prints a horizontal border: +-----...-----+ */
void printBoxLine(void) {
    printf("+");
    for (int i = 0; i < BOX_WIDTH; i++) {
        printf("-");
    }
    printf("+\n");
}

/* Prints text centered inside a box row: |   text   | */
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

/* Prints a boxed screen header with a title and optional subtitle. */
void printHeader(const char title[], const char subtitle[]) {
    printBoxLine();
    printCentered(title);
    if (subtitle[0] != '\0') {
        printCentered(subtitle);
    }
    printBoxLine();
}

/* Replaces any '|' with '-' so user input cannot break the
 * pipe-delimited format of the .txt files. */
void sanitizePipes(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '|') {
            str[i] = '-';
        }
    }
}

/* Linear search by ID. Returns the array index, or -1 if not found. */
int findMedicineById(const Medicine meds[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (meds[i].id == id) {
            return i;
        }
    }
    return -1;
}

/* Linear search by exact name. Returns the array index, or -1 if not found. */
int findMedicineByName(const Medicine meds[], int count, const char name[]) {
    for (int i = 0; i < count; i++) {
        if (strcmp(meds[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/* Keeps asking for a Medicine ID until a valid one is entered.
 * Returns the array index of the medicine, or -1 if the user entered 0 (cancel). */
int promptMedicineIndex(const Medicine meds[], int count, const char prompt[]) {
    int id, index;

    while (1) {
        printf("  %-37s: ", prompt);
        if (scanf("%d", &id) != 1) {
            clearInputBuffer();
            printf("  [!] Invalid ID! Please enter a number.\n");
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
        printf("  [!] Medicine ID %d not found! Please try again.\n", id);
    }
}

/* Reads an expiry date and stores it as YYYY-MM-DD.
 * Accepts: YYYYMMDD, YYYY MM DD, or YYYY-MM-DD.
 * Returns:  1 = valid date
 *           0 = invalid date
 *          -1 = user entered "0"  (cancel)
 *           2 = user entered "-1" (keep current value)  */
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
    if (strcmp(rawDate, "-1") == 0) {
        return 2;
    }

    length = (int)strlen(rawDate);

    if (length == 8) {
        /* YYYYMMDD -> insert the dashes */
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
        /* YYYY MM DD or YYYY-MM-DD -> force dashes */
        strcpy(expiry, rawDate);
        expiry[4] = '-';
        expiry[7] = '-';
    }
    else {
        return 0;
    }

    /* Every position except the dashes must be a digit */
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            continue;
        }
        if (expiry[i] < '0' || expiry[i] > '9') {
            return 0;
        }
    }

    /* Convert the digit characters into numbers */
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

    /* February has 29 days in a leap year */
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

/* Creates inventory.txt and sales.txt if they do not exist yet. */
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

/* Loads medicines from inventory.txt into the array, one line at a time.
 * Blank lines are ignored. Invalid or duplicate rows are skipped.
 * Returns the number of skipped rows. */
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
        /* Ignore empty lines */
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

/* Writes the whole array to inventory.txt.
 * Returns 1 if saved successfully, 0 if the file could not be written. */
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

/* Appends one sale record to sales.txt.
 * Returns 1 if saved successfully, 0 on error. */
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

/* Reads sales.txt and computes the number of sales and total revenue. */
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

/* [1] Adds a new medicine with an auto-generated ID. */
void addMedicine(Medicine meds[], int *count) {
    Medicine newMed;
    char subtitle[BOX_WIDTH + 1];
    int expiryStatus, existingIndex, confirm;

    clearScreen();

    if (*count >= MAX) {
        printf("\n  [!] Error: Inventory is full!\n");
        pauseScreen();
        return;
    }

    /* Next ID = highest existing ID + 1 (starts at START_ID) */
    newMed.id = START_ID;
    for (int i = 0; i < *count; i++) {
        if (meds[i].id >= newMed.id) {
            newMed.id = meds[i].id + 1;
        }
    }

    sprintf(subtitle, "Auto-ID: %d  |  Enter 0 to Cancel", newMed.id);
    printHeader("[1] ADD MEDICINE", subtitle);
    printf("\n");

    /* Name (warns if the same name already exists) */
    while (1) {
        printf("  Enter Medicine Name                  : ");
        if (scanf(" %49[^\n]", newMed.name) != 1) {
            clearInputBuffer();
            printf("  [!] Invalid medicine name! Please try again.\n");
            continue;
        }
        clearInputBuffer();

        if (strcmp(newMed.name, "0") == 0) {
            return;
        }
        sanitizePipes(newMed.name);

        existingIndex = findMedicineByName(meds, *count, newMed.name);
        if (existingIndex == -1) {
            break;
        }

        printf("  [!] '%s' already exists (ID: %d).\n",
               meds[existingIndex].name, meds[existingIndex].id);
        printf("  Add anyway? (1 = Yes, 0 = Re-enter)  : ");
        if (scanf("%d", &confirm) == 1 && confirm == 1) {
            clearInputBuffer();
            break;
        }
        clearInputBuffer();
    }

    /* Category */
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

    /* Price (0 cancels because a real price must be > 0) */
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

    /* Quantity (0 is a valid stock amount here, so it does NOT cancel) */
    while (1) {
        printf("  Enter Stock Quantity                 : ");
        if (scanf("%d", &newMed.quantity) == 1) {
            clearInputBuffer();
            if (newMed.quantity >= 0) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Quantity cannot be negative! Please try again.\n");
    }

    /* Expiry date */
    while (1) {
        printf("  Enter Expiry (YYYYMMDD / YYYY MM DD) : ");
        expiryStatus = readAndFormatExpiry(newMed.expiry);
        if (expiryStatus == -1) {
            return;
        }
        if (expiryStatus == 1) {
            break;
        }
        printf("  [!] Invalid date! Use a real date from 2000 to 2100 (e.g. 20270315).\n");
    }

    meds[*count] = newMed;
    (*count)++;

    if (!saveInventory(meds, *count)) {
        (*count)--;   /* undo so memory matches the file */
        printf("\n  [!] Error: Could not save to %s. Medicine was NOT added.\n", INVENTORY_FILE);
        pauseScreen();
        return;
    }

    printf("\n  [+] Medicine '%s' (ID: %d) added successfully!\n", newMed.name, newMed.id);
    pauseScreen();
}

/* [2] Updates price, quantity, and expiry of an existing medicine.
 *     -1 keeps the current value, 0 cancels (except quantity, where 0 is valid). */
void updateMedicine(Medicine meds[], int count) {
    Medicine backup;
    int index, expiryStatus;
    float newPrice;
    int newQuantity;
    char newExpiry[11];

    clearScreen();

    if (count == 0) {
        printf("\n  [!] Inventory is currently empty!\n");
        pauseScreen();
        return;
    }

    viewMedicines(meds, count);
    printf("\n");
    printHeader("[2] UPDATE MEDICINE", "0 = Cancel  |  -1 = Keep Current Value");

    index = promptMedicineIndex(meds, count, "Enter Medicine ID");
    if (index == -1) {
        return;
    }

    printf("\n  +--- Updating Record ---------------------------+\n");
    printf("  | Medicine : %-34.34s |\n", meds[index].name);
    printf("  | Category : %-34.34s |\n", meds[index].category);
    printf("  +-----------------------------------------------+\n\n");

    /* Price */
    while (1) {
        printf("  New Price    (Current: %10.2f)     : ", meds[index].price);
        if (scanf("%f", &newPrice) == 1) {
            clearInputBuffer();
            if (newPrice == 0.0f) {
                return;
            }
            if (newPrice == -1.0f) {
                newPrice = meds[index].price;
                break;
            }
            if (newPrice > 0.0f) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Price must be greater than 0.00 (-1 = keep, 0 = cancel).\n");
    }

    /* Quantity (0 is valid, e.g. to mark an item as out of stock) */
    while (1) {
        printf("  New Quantity (Current: %10d)     : ", meds[index].quantity);
        if (scanf("%d", &newQuantity) == 1) {
            clearInputBuffer();
            if (newQuantity == -1) {
                newQuantity = meds[index].quantity;
                break;
            }
            if (newQuantity >= 0) {
                break;
            }
        } else {
            clearInputBuffer();
        }
        printf("  [!] Quantity cannot be negative (-1 = keep current).\n");
    }

    /* Expiry date */
    while (1) {
        printf("  New Expiry   (Current: %10s)     : ", meds[index].expiry);
        expiryStatus = readAndFormatExpiry(newExpiry);
        if (expiryStatus == -1) {
            return;
        }
        if (expiryStatus == 2) {
            strcpy(newExpiry, meds[index].expiry);
            break;
        }
        if (expiryStatus == 1) {
            break;
        }
        printf("  [!] Invalid date! Use a real date from 2000 to 2100 (-1 = keep, 0 = cancel).\n");
    }

    /* Apply changes, but keep a backup in case saving fails */
    backup = meds[index];
    meds[index].price = newPrice;
    meds[index].quantity = newQuantity;
    strcpy(meds[index].expiry, newExpiry);

    if (!saveInventory(meds, count)) {
        meds[index] = backup;
        printf("\n  [!] Error: Could not save to %s. No changes were made.\n", INVENTORY_FILE);
        pauseScreen();
        return;
    }

    printf("\n  [+] Medicine '%s' updated successfully!\n", meds[index].name);
    pauseScreen();
}

/* [3] Displays all medicines in a formatted table. */
void viewMedicines(const Medicine meds[], int count) {
    const char *status;

    if (count == 0) {
        printf("\n  [!] The inventory is currently empty.\n");
        return;
    }

    printf("+====================================================================================================+\n");
    printf("|                                         MEDICINE INVENTORY                                         |\n");
    printf("+====================================================================================================+\n");
    printf("| %-8s | %-20s | %-15s | %10s | %8s | %-10s | %-11s |\n",
           "ID", "Name", "Category", "Price", "Quantity", "Expiry", "Status");
    printf("+----------+----------------------+-----------------+------------+----------+------------+-------------+\n");

    for (int i = 0; i < count; i++) {
        if (meds[i].quantity == 0) {
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
    printf("  Total Medicines Listed: %d\n", count);
}

/* [4] Removes a medicine by ID and shifts the remaining elements left. */
void removeMedicine(Medicine meds[], int *count) {
    int index, confirm;

    clearScreen();

    if (*count == 0) {
        printf("\n  [!] Inventory is currently empty!\n");
        pauseScreen();
        return;
    }

    viewMedicines(meds, *count);
    printf("\n");
    printHeader("[4] REMOVE MEDICINE", "Enter 0 to Cancel");

    index = promptMedicineIndex(meds, *count, "Enter Medicine ID");
    if (index == -1) {
        return;
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

    /* Shift every element after 'index' one position to the left */
    for (int i = index; i < *count - 1; i++) {
        meds[i] = meds[i + 1];
    }
    (*count)--;

    if (!saveInventory(meds, *count)) {
        printf("\n  [!] Warning: Removed from memory, but could not save to %s!\n", INVENTORY_FILE);
        pauseScreen();
        return;
    }

    printf("\n  [+] Medicine removed successfully!\n");
    pauseScreen();
}

/* ======================== SORTING ======================== */

/* [5] Copies the inventory, sorts the copy with Bubble Sort,
 *     and displays it. The original array order is not changed. */
void sortAndViewMedicines(const Medicine meds[], int count) {
    Medicine tempMeds[MAX];
    Medicine temp;
    int field, order;
    int i, j, swap;

    clearScreen();

    if (count == 0) {
        printf("\n  [!] Inventory is currently empty!\n");
        pauseScreen();
        return;
    }

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

    /* Copy the original inventory so it stays unchanged */
    for (i = 0; i < count; i++) {
        tempMeds[i] = meds[i];
    }

    /* Bubble Sort: compare neighbours and swap if they are in the wrong order */
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            swap = 0;

            if (field == 1) {
                /* Sort by name */
                int cmp = strcmp(tempMeds[j].name, tempMeds[j + 1].name);
                if ((order == 1 && cmp > 0) || (order == 2 && cmp < 0)) {
                    swap = 1;
                }
            }
            else if (field == 2) {
                /* Sort by price */
                if ((order == 1 && tempMeds[j].price > tempMeds[j + 1].price) ||
                    (order == 2 && tempMeds[j].price < tempMeds[j + 1].price)) {
                    swap = 1;
                }
            }
            else if (field == 3) {
                /* Sort by quantity */
                if ((order == 1 && tempMeds[j].quantity > tempMeds[j + 1].quantity) ||
                    (order == 2 && tempMeds[j].quantity < tempMeds[j + 1].quantity)) {
                    swap = 1;
                }
            }
            else if (field == 4) {
                /* Sort by expiry (YYYY-MM-DD strings compare correctly as text) */
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

    printf("\n");
    viewMedicines(tempMeds, count);
    pauseScreen();
}

/* ========================= SALES ========================= */

/* [6] Sells a medicine: checks stock, logs the sale to sales.txt,
 *     deducts the quantity, prints a receipt, and saves the inventory. */
void recordSale(Medicine meds[], int count) {
    int index, quantity;
    int availableItems = 0;
    int salesCount;
    float total, totalRevenue;

    clearScreen();

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

    viewMedicines(meds, count);

    if (availableItems == 0) {
        printf("\n  [!] All medicines in inventory are currently out of stock!\n");
        pauseScreen();
        return;
    }

    printf("\n");
    printHeader("[6] SELL MEDICINE", "Enter 0 to Cancel");

    /* Pick a medicine that exists AND has stock */
    while (1) {
        index = promptMedicineIndex(meds, count, "Enter Medicine ID to Sell");
        if (index == -1) {
            return;
        }
        if (meds[index].quantity > 0) {
            break;
        }
        printf("  [!] '%s' is out of stock! Please select another ID.\n", meds[index].name);
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

    /* Log the sale first; only change stock if logging worked */
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
    printf("  | Medicine ID   : %-30d |\n", meds[index].id);
    printf("  | Medicine      : %-30.30s |\n", meds[index].name);
    printf("  | Unit Price    : %-30.2f |\n", meds[index].price);
    printf("  | Quantity Sold : %-30d |\n", quantity);
    printf("  +------------------------------------------------+\n");
    printf("  | TOTAL SALE    : %-30.2f |\n", total);
    printf("  | Stock Left    : %-30d |\n", meds[index].quantity);
    printf("  +------------------------------------------------+\n");
    printf("  | Sales Logged  : %-4d   Total Revenue: %-8.2f |\n", salesCount, totalRevenue);
    printf("  +================================================+\n");

    if (meds[index].quantity == 0) {
        printf("\n  [!] '%s' is now OUT OF STOCK!\n", meds[index].name);
    } else if (meds[index].quantity <= LOW_STOCK_LIMIT) {
        printf("\n  [!] LOW STOCK ALERT: Only %d unit(s) of '%s' left!\n",
               meds[index].quantity, meds[index].name);
    }

    if (!saveInventory(meds, count)) {
        printf("\n  [!] Warning: Sale was logged, but %s could not be updated!\n", INVENTORY_FILE);
    }
    pauseScreen();
}
