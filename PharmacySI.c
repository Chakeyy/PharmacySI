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

void updateMedicine(Medicine meds[], int count) {
    (void)meds;
    (void)count;
    printf("\n[2] Update Medicine\n");
}

void viewMedicines(const Medicine meds[], int count) {
    (void)meds;
    (void)count;
    printf("\n[3] View Medicine\n");
}

void removeMedicine(Medicine meds[], int *count) {
    (void)meds;
    (void)count;
    printf("\n[4] Remove Medicine\n");
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
