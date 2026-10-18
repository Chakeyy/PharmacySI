#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define INVENTORY_FILE "inventory.txt"

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

int findMedicineById(const Medicine meds[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (meds[i].id == id) {
            return i;
        }
    }
    return -1;
}

void loadInventory(Medicine meds[], int *count) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    if (fp == NULL) {
        *count = 0;
        return;
    }
    *count = 0;
    while (*count < MAX && fscanf(fp, "%d|%49[^|]|%29[^|]|%f|%d|%10[^\r\n]\n",
           &meds[*count].id,
           meds[*count].name,
           meds[*count].category,
           &meds[*count].price,
           &meds[*count].quantity,
           meds[*count].expiry) == 6) {
        (*count)++;
    }
    fclose(fp);
}

void saveInventory(const Medicine meds[], int count) {
    FILE *fp = fopen(INVENTORY_FILE, "w");
    if (fp == NULL) {
        printf("Error opening %s for writing!\n", INVENTORY_FILE);
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

void addMedicine(Medicine meds[], int *count) {
    Medicine newMed;

    if (*count >= MAX) {
        printf("\n  [!] Error: Inventory is full!\n");
        return;
    }

    printf("\n+--------------------------------------------------+\n");
    printf("|                 [1] ADD MEDICINE                 |\n");
    printf("+--------------------------------------------------+\n");

    printf("  Enter Medicine ID             : ");
    if (scanf("%d", &newMed.id) != 1 || newMed.id <= 0) {
        clearInputBuffer();
        printf("\n  [!] Invalid ID! Must be a positive integer.\n");
        return;
    }
    clearInputBuffer();

    if (findMedicineById(meds, *count, newMed.id) != -1) {
        printf("\n  [!] Error: Medicine ID %d already exists!\n", newMed.id);
        return;
    }

    printf("  Enter Medicine Name           : ");
    if (scanf(" %49[^\n]", newMed.name) != 1) {
        clearInputBuffer();
        printf("\n  [!] Invalid medicine name!\n");
        return;
    }
    clearInputBuffer();

    printf("  Enter Category                : ");
    if (scanf(" %29[^\n]", newMed.category) != 1) {
        clearInputBuffer();
        printf("\n  [!] Invalid category!\n");
        return;
    }
    clearInputBuffer();

    printf("  Enter Unit Price              : ");
    if (scanf("%f", &newMed.price) != 1 ||
        newMed.price <= 0.0f) {
        clearInputBuffer();
        printf("\n  [!] Price must be greater than 0.00!\n");
        return;
    }
    clearInputBuffer();

    printf("  Enter Stock Quantity          : ");
    if (scanf("%d", &newMed.quantity) != 1 ||
        newMed.quantity < 0) {
        clearInputBuffer();
        printf("\n  [!] Quantity must be zero or positive!\n");
        return;
    }
    clearInputBuffer();

    printf("  Enter Expiry Date (YYYY-MM-DD): ");
    if (scanf("%10s", newMed.expiry) != 1) {
        clearInputBuffer();
        printf("\n  [!] Invalid expiry date!\n");
        return;
    }
    clearInputBuffer();

    if (strlen(newMed.expiry) != 10 ||
        newMed.expiry[4] != '-' ||
        newMed.expiry[7] != '-') {
        printf("\n  [!] Invalid date! Use YYYY-MM-DD format.\n");
        return;
    }

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7)
            continue;

        if (newMed.expiry[i] < '0' ||  newMed.expiry[i] > '9') {
            printf("\n  [!] Invalid date! Use YYYY-MM-DD format.\n");
            return;
        }
    }

    meds[*count] = newMed;
    (*count)++;

    saveInventory(meds, *count);

    printf("\n  [+] Medicine added successfully!\n");
}

void updateMedicine(Medicine meds[], int count) {
    int id, index;
    float newPrice;
    int newQuantity;
    char newExpiry[11];

    if (count == 0) {
        printf("\n  [!] Inventory is currently empty!\n");
        return;
    }

    printf("\n+--------------------------------------------------+\n");
    printf("|                [2] UPDATE MEDICINE               |\n");
    printf("+--------------------------------------------------+\n");
    printf("  Enter Medicine ID to update   : ");

    if (scanf("%d", &id) != 1) {
        clearInputBuffer();
        printf("\n  [!] Invalid ID!\n");
        return;
    }
    clearInputBuffer();

    index = findMedicineById(meds, count, id);

    if (index == -1) {
        printf("\n  [!] Medicine ID %d not found!\n", id);
        return;
    }

    printf("\n  +--- Current Record ----------------------------+\n");
    printf("  | Medicine : %-34.34s |\n", meds[index].name);
    printf("  | Category : %-34.34s |\n", meds[index].category);
    printf("  | Price    : %-34.2f |\n", meds[index].price);
    printf("  | Quantity : %-34d |\n", meds[index].quantity);
    printf("  | Expiry   : %-34s |\n", meds[index].expiry);
    printf("  +-----------------------------------------------+\n\n");

    printf("  Enter New Price               : ");
    if (scanf("%f", &newPrice) != 1 || newPrice <= 0.0f) {
        clearInputBuffer();
        printf("\n  [!] Price must be greater than 0.00!\n");
        return;
    }
    clearInputBuffer();

    printf("  Enter New Quantity            : ");
    if (scanf("%d", &newQuantity) != 1 || newQuantity < 0) {
        clearInputBuffer();
        printf("\n  [!] Quantity must be zero or positive!\n");
        return;
    }
    clearInputBuffer();

    printf("  Enter New Expiry (YYYY-MM-DD) : ");
    if (scanf("%10s", newExpiry) != 1) {
        clearInputBuffer();
        printf("\n  [!] Invalid expiry date!\n");
        return;
    }
    clearInputBuffer();

    if (strlen(newExpiry) != 10 ||
        newExpiry[4] != '-' ||
        newExpiry[7] != '-') {
        printf("\n  [!] Invalid date! Use YYYY-MM-DD format.\n");
        return;
    }

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7)
            continue;

        if (newExpiry[i] < '0' || newExpiry[i] > '9') {
            printf("\n  [!] Invalid date! Use YYYY-MM-DD format.\n");
            return;
        }
    }

    meds[index].price = newPrice;
    meds[index].quantity = newQuantity;
    strcpy(meds[index].expiry, newExpiry);

    saveInventory(meds, count);

    printf("\n  [+] Medicine updated successfully!\n");
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

void removeMedicine(Medicine meds[], int *count) {
    int id, index;

    if (*count == 0) {
        printf("\n  [!] Inventory is currently empty!\n");
        return;
    }

    printf("\n+--------------------------------------------------+\n");
    printf("|                [4] REMOVE MEDICINE               |\n");
    printf("+--------------------------------------------------+\n");
    printf("  Enter Medicine ID to remove   : ");

    if (scanf("%d", &id) != 1) {
        clearInputBuffer();
        printf("\n  [!] Invalid ID!\n");
        return;
    }
    clearInputBuffer();

    index = findMedicineById(meds, *count, id);

    if (index == -1) {
        printf("\n  [!] Medicine ID %d not found!\n", id);
        return;
    }

    for (int i = index; i < *count - 1; i++) {
        meds[i] = meds[i + 1];
    }

    (*count)--;

    saveInventory(meds, *count);

    printf("\n  [+] Medicine removed successfully!\n");
}

void sortAndViewMedicines(const Medicine meds[], int count) {
    Medicine tempMeds[MAX];
    Medicine temp;
    int field, order;
    int i, j, swap;

    if (count == 0) {
        printf("\n  [!] Inventory is currently empty!\n");
        return;
    }
    clearScreen();
    printf("+==================================================+\n");
    printf("|               [5] SORTED INVENTORY               |\n");
    printf("+==================================================+\n");
    printf("|  Sort By Field:                                  |\n");
    printf("|    [1] Name              [2] Price               |\n");
    printf("|    [3] Quantity          [4] Expiry Date         |\n");
    printf("+--------------------------------------------------+\n");
    printf("  Select field (1-4): ");

    if (scanf("%d", &field) != 1) {
        clearInputBuffer();
        printf("\n  [!] Invalid choice!\n");
        return;
    }
    clearInputBuffer();

    if (field < 1 || field > 4) {
        printf("\n  [!] Invalid choice!\n");
        return;
    }

    printf("\n+--------------------------------------------------+\n");
    printf("|  Sort Direction:                                 |\n");
    printf("|    [1] Ascending (A-Z / Low-High)                |\n");
    printf("|    [2] Descending (Z-A / High-Low)               |\n");
    printf("+--------------------------------------------------+\n");
    printf("  Select order (1-2): ");

    if (scanf("%d", &order) != 1) {
        clearInputBuffer();
        printf("\n  [!] Invalid choice!\n");
        return;
    }
    clearInputBuffer();

    if (order < 1 || order > 2) {
        printf("\n  [!] Invalid choice!\n");
        return;
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
}

void recordSale(Medicine meds[], int count) {
    int id, index, quantity;
    float total;
    FILE *fp;

    if (count == 0) {
        printf("\n  [!] Inventory is currently empty!\n");
        return;
    }

    printf("\n+--------------------------------------------------+\n");
    printf("|                  [6] RECORD SALE                 |\n");
    printf("+--------------------------------------------------+\n");
    printf("  Enter Medicine ID             : ");

    if (scanf("%d", &id) != 1) {
        clearInputBuffer();
        printf("\n  [!] Invalid ID!\n");
        return;
    }
    clearInputBuffer();

    index = findMedicineById(meds, count, id);

    if (index == -1) {
        printf("\n  [!] Medicine ID %d not found!\n", id);
        return;
    }

    if (meds[index].quantity <= 0) {
        printf("\n  [!] Medicine '%s' is out of stock!\n", meds[index].name);
        return;
    }

    printf("\n  +--- Selected Item -----------------------------+\n");
    printf("  | Medicine        : %-27.27s |\n", meds[index].name);
    printf("  | Unit Price      : %-27.2f |\n", meds[index].price);
    printf("  | Available Stock : %-27d |\n", meds[index].quantity);
    printf("  +-----------------------------------------------+\n\n");
    printf("  Enter Quantity to Buy         : ");

    if (scanf("%d", &quantity) != 1) {
        clearInputBuffer();
        printf("\n  [!] Invalid quantity!\n");
        return;
    }
    clearInputBuffer();

    if (quantity <= 0 || quantity > meds[index].quantity) {
        printf("\n  [!] Invalid quantity! Please check available stock.\n");
        return;
    }

    total = meds[index].price * quantity;

    // Open sales file before changing inventory
    fp = fopen("sales.txt", "a");

    if (fp == NULL) {
        printf("\n  [!] Error opening sales.txt!\n");
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
        return;
    }

    if (fclose(fp) == EOF) {
        printf("\n  [!] Error saving sales record!\n");
        return;
    }

    // Deduct sold quantity
    meds[index].quantity -= quantity;

    // Print receipt
    printf("\n  +================================================+\n");
    printf("  |                 SALES RECEIPT                  |\n");
    printf("  +================================================+\n");
    printf("  | Medicine ID : %-32d |\n", meds[index].id);
    printf("  | Medicine    : %-32.32s |\n", meds[index].name);
    printf("  | Unit Price  : %-32.2f |\n", meds[index].price);
    printf("  | Quantity    : %-32d |\n", quantity);
    printf("  +------------------------------------------------+\n");
    printf("  | TOTAL DUE   : %-32.2f |\n", total);
    printf("  | Remaining   : %-32d |\n", meds[index].quantity);
    printf("  +================================================+\n");

    if (meds[index].quantity <= 10) {
        printf("\n  [!] LOW STOCK ALERT: Only %d unit(s) left!\n",
               meds[index].quantity);
    }

    // Save updated inventory
    saveInventory(meds, count);
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
        printf("|    [6] Sale                                      |\n");
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
                pauseScreen();
                break;

            case 2:
                updateMedicine(meds, count);
                pauseScreen();
                break;

            case 3:
                viewMedicines(meds, count);
                pauseScreen();
                break;

            case 4:
                removeMedicine(meds, &count);
                pauseScreen();
                break;

            case 5:
                sortAndViewMedicines(meds, count);
                pauseScreen();
                break;

            case 6:
                recordSale(meds, count);
                pauseScreen();
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
