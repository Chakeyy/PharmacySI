# Pharmacy Sales and Inventory System (CCS104)

Console-based Pharmacy Sales and Inventory Management System written in C (C99 standard) for the CCS104 course.

---

## ⚠️ Important Academic & Technical Constraints

Please read these carefully before writing any code:

1. **Pure C (C99 Standard)**:
   - Use only standard C headers: `<stdio.h>`, `<stdlib.h>`, and `<string.h>`.
   - Single source file integration: All final code belongs in `PharmacySI.c`.

2. **Prohibited Concepts (DO NOT USE)**:
   - ❌ **NO dynamic memory allocation** (`malloc`, `calloc`, `realloc`, `free`).
   - ❌ **NO linked lists** or advanced data structures.
   - ❌ **NO pointers-to-pointers (`**`)**.
   - ❌ **NO `qsort()`** from `stdlib.h` (we must write our own sorting algorithm like Bubble Sort).

3. **Allowed & Expected Concepts**:
   - Fixed-size arrays: `#define MAX 100` and `Medicine meds[MAX];`.
   - Standard structures (`struct`).
   - Passing arrays and pointers to functions (e.g., `int *count` to update size).
   - Basic text file I/O (`fopen`, `fprintf`, `fscanf`, `fclose`).
   - Clean coding style with descriptive variable names.

---

## Data Structure & Storage Design

### Medicine Structure
Defined in `PharmacySI.c`:
```c
typedef struct {
    int id;
    char name[50];
    char category[30];
    float price;
    int quantity;
    char expiry[11]; // Format: YYYY-MM-DD
} Medicine;
```

### File Persistence
- **`inventory.txt`**: Pipe-delimited records:
  ```text
  101|Paracetamol 500mg|Pain Relief|2.50|120|2027-03-15
  ```
  Loaded on startup and auto-saved after any modifying operation (Add, Update, Remove, Sale).
- **`sales.txt`**: Appended log of completed transactions:
  ```text
  101|Paracetamol 500mg|5|12.50
  ```

---

## Member Task Breakdown

Check the **[GitHub Issues](https://github.com/Chakeyy/PharmacySI/issues)** tab for full implementation checklists:

- **Core & Architecture (Done)**:
  - Menu skeleton with buffer clearing.
  - Struct definition and file persistence (`loadInventory`, `saveInventory`).
  - Helper functions (`findMedicineById`, `clearInputBuffer`).

- **[Issue #2](https://github.com/Chakeyy/PharmacySI/issues/2) — Add & View**:
  - `addMedicine(Medicine meds[], int *count)`: Unique ID validation, prompt fields, append, and auto-save.
  - `viewMedicines(const Medicine meds[], int count)`: Tabular layout with column alignment and low-stock warning tag (`<= 10`).

- **[Issue #3](https://github.com/Chakeyy/PharmacySI/issues/3) — Update & Remove**:
  - `updateMedicine(Medicine meds[], int count)`: Search by ID, edit price/quantity/expiry, and auto-save.
  - `removeMedicine(Medicine meds[], int *count)`: Search by ID, shift array elements left, decrement count, and auto-save.

- **[Issue #4](https://github.com/Chakeyy/PharmacySI/issues/4) — Listing (Sorted) & Sale**:
  - `sortAndViewMedicines(const Medicine meds[], int count)`: Bubble Sort on a local copied array by Name, Price, Quantity, or Expiry (Ascending / Descending).
  - `recordSale(Medicine meds[], int count)`: Search by ID, validate stock, deduct quantity, compute total, log to `sales.txt`, and auto-save.

---

## Collaboration & Integration Guidelines

1. **Avoid Overwriting Each Other**:
   - Work on your assigned function in a separate file or draft snippet first.
   - Once tested, paste your function implementation into the corresponding placeholder in `PharmacySI.c`.
2. **Input Handling**:
   - Always clear the input buffer (`clearInputBuffer()`) when switching between numeric `scanf` and string `fgets` inputs to prevent newline skipping and infinite loops.
3. **Array Bounds**:
   - Always check bounds before adding elements (`*count < MAX`).
   - Always check if the inventory is empty (`count == 0`) before searching, viewing, or selling.

---

## How to Compile & Run

### Standard Linux / macOS / Windows (MinGW/WSL):
```bash
gcc -Wall -Wextra -std=c99 PharmacySI.c -o pharmacy
./pharmacy
```

### Android / Termux:
Because shared storage (`/storage/emulated/0/...`) cannot directly execute binaries, compile the executable to `/root/` or your internal home directory:
```bash
gcc PharmacySI.c -o /root/pharmacy && /root/pharmacy
```
