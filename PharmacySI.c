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
    (void)meds;
    (void)count;
    printf("\n[1] Add Medicine\n");
}

void addMedicine(Medicine meds[], int *count) {
    Medicine newMed;

    if (*count >= MAX) {
        printf("\nInventory is full!\n");
        return;
    }

    printf("\nEnter Medicine ID: ");
    if (scanf("%d", &newMed.id) != 1) {
        clearInputBuffer();
        printf("Invalid ID!\n");
        return;
    }
    clearInputBuffer();

    if (findMedicineById(meds, *count, newMed.id) != -1) {
        printf("Medicine ID already exists!\n");
        return;
    }

    printf("Enter Medicine Name: ");
    if (scanf(" %49[^\n]", newMed.name) != 1) {
        clearInputBuffer();
        printf("Invalid name!\n");
        return;
    }
    clearInputBuffer();

    printf("Enter Category: ");
    if (scanf(" %29[^\n]", newMed.category) != 1) {
        clearInputBuffer();
        printf("Invalid category!\n");
        return;
    }
    clearInputBuffer();

    printf("Enter Price: ");
    if (scanf("%f", &newMed.price) != 1 ||
        newMed.price <= 0.0f) {
        clearInputBuffer();
        printf("Price must be greater than 0.00!\n");
        return;
    }
    clearInputBuffer();

    printf("Enter Quantity: ");
    if (scanf("%d", &newMed.quantity) != 1 ||
        newMed.quantity < 0) {
        clearInputBuffer();
        printf("Quantity must be zero or positive!\n");
        return;
    }
    clearInputBuffer();

    printf("Enter Expiry Date (YYYY-MM-DD): ");
    if (scanf("%10s", newMed.expiry) != 1) {
        clearInputBuffer();
        printf("Invalid expiry date!\n");
        return;
    }
    clearInputBuffer();

    if (strlen(newMed.expiry) != 10 ||
        newMed.expiry[4] != '-' ||
        newMed.expiry[7] != '-') {
        printf("Use YYYY-MM-DD format!\n");
        return;
    }

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7)
            continue;

        if (newMed.expiry[i] < '0' ||  newMed.expiry[i] > '9') {
            printf("Use YYYY-MM-DD format!\n");
            return;
        }
    }

    meds[*count] = newMed;
    (*count)++;

    saveInventory(meds, *count);

    printf("Medicine added successfully!\n");
}

void viewMedicines(const Medicine meds[], int count) {
    if (count == 0) {
        printf("\nThe inventory is currently empty.\n");
        return;
    }

    printf("\n==================== MEDICINE INVENTORY ====================\n");

    printf("%-8s %-20s %-15s %10s %8s %-12s %s\n",
           "ID", "Name", "Category", "Price",
           "Quantity", "Expiry", "Status");

    printf("-----------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        printf("%-8d %-20.20s %-15.15s %10.2f %8d %-12s %s\n",
               meds[i].id,
               meds[i].name,
               meds[i].category,
               meds[i].price,
               meds[i].quantity,
               meds[i].expiry,
               meds[i].quantity <= 10 ? "[LOW STOCK]" : "OK");
    }

    printf("-----------------------------------------------------------------------------------------------\n");
    printf("Total medicines: %d\n", count);
}



void removeMedicine(Medicine meds[], int *count) {
    int id, index;

    if (*count == 0) {
        printf("\nInventory is empty!\n");
        return;
    }

    printf("\n[4] Remove Medicine\n");
    printf("Enter Medicine ID to remove: ");

    if (scanf("%d", &id) != 1) {
        clearInputBuffer();
        printf("Invalid ID!\n");
        return;
    }

    index = findMedicineById(meds, *count, id);

    if (index == -1) {
        printf("Medicine not found!\n");
        return;
    }

    for (int i = index; i < *count - 1; i++) {
        meds[i] = meds[i + 1];
    }

    (*count)--;

    saveInventory(meds, *count);

    printf("Medicine removed successfully!\n");
}


void sortAndViewMedicines(const Medicine meds[], int count) {
    (void)meds;
    (void)count;
    printf("\n[5] Listing (Sorted)\n");
}

void recordSale(Medicine meds[], int count) {
    (void)meds;
    (void)count;
    printf("\n[6] Sale\n");
}

int main(void) {
    Medicine meds[MAX];
    int count = 0;
    int choice;

    loadInventory(meds, &count);

    do {
        printf("\n====================================\n");
        printf("       PHARMACY MANAGEMENT SYSTEM\n");
        printf("====================================\n");
        printf("[1] Add Medicine\n");
        printf("[2] Update Medicine\n");
        printf("[3] View Medicine\n");
        printf("[4] Remove Medicine\n");
        printf("[5] Listing (Sorted)\n");
        printf("[6] Sale\n");
        printf("[0] Exit\n");
        printf("====================================\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("\nInvalid choice! Please try again.\n");
            continue;
        }

        switch (choice) {
            case 1:
                addMedicine(meds, &count);
                break;

            case 2:
                updateMedicine(meds, count);
                break;

            case 3:
                viewMedicines(meds, count);
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
                printf("\nExiting Pharmacy System...\n");
                break;

            default:
                printf("\nInvalid choice! Please try again.\n");
        }

    } while (choice != 0);

    return 0;
}
