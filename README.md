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
   - Clean coding style with descriptive variable names and comments.

---

## Data Structure & Storage Design

### Medicine Structure
Defined in `PharmacySI.c`:
```c
typedef struct {
    int id;
    char name[50];       // Medicine name (supports spaces)
    char category[30];   // Category / Classification
    float price;         // Unit price (> 0.00)
    int quantity;        // Available stock (>= 0)
    char expiry[11];     // Expiry date (Format: YYYY-MM-DD + '\0')
} Medicine;
```

### File Persistence
- **`inventory.txt`**: Pipe-delimited records (`id|name|category|price|quantity|expiry`):
  ```text
  101|Paracetamol 500mg|Pain Relief|2.50|120|2027-03-15
  102|Amoxicillin 500mg|Antibiotic|8.50|8|2026-12-01
  ```
  Loaded on startup via `loadInventory()` and auto-saved via `saveInventory()` after any modifying operation (Add, Update, Remove, Sale).

- **`sales.txt`**: Appended transaction log (`id|name|quantity|total`):
  ```text
  101|Paracetamol 500mg|5|12.50
  ```
  Appended automatically whenever a sale is completed via `recordSale()`.

---

## Menu & Task Breakdown

### Menu Navigation Map
| Option | Feature | Assigned Issue | Function | Status |
|:---:|:---|:---|:---|:---:|
| **[1]** | Add Medicine | [Issue #2](https://github.com/Chakeyy/PharmacySI/issues/2) | `addMedicine(Medicine meds[], int *count)` | In Progress |
| **[2]** | Update Medicine | [Issue #3](https://github.com/Chakeyy/PharmacySI/issues/3) | `updateMedicine(Medicine meds[], int count)` | In Progress |
| **[3]** | View Medicine | [Issue #2](https://github.com/Chakeyy/PharmacySI/issues/2) | `viewMedicines(const Medicine meds[], int count)` | In Progress |
| **[4]** | Remove Medicine | [Issue #3](https://github.com/Chakeyy/PharmacySI/issues/3) | `removeMedicine(Medicine meds[], int *count)` | In Progress |
| **[5]** | Listing (Sorted) | [Issue #4](https://github.com/Chakeyy/PharmacySI/issues/4) | `sortAndViewMedicines(const Medicine meds[], int count)` | In Progress |
| **[6]** | Sale | [Issue #4](https://github.com/Chakeyy/PharmacySI/issues/4) | `recordSale(Medicine meds[], int count)` | In Progress |
| **[0]** | Exit | Core Architecture | `main()` | Done |

### Detailed Scope Per Issue
Check the **[GitHub Issues](https://github.com/Chakeyy/PharmacySI/issues)** tab for full implementation checklists:

- **Core & Architecture (Done)**:
  - Menu skeleton with buffer clearing.
  - Struct definition and file persistence (`loadInventory`, `saveInventory`).
  - Helper functions (`findMedicineById`, `clearInputBuffer`).

- **[Issue #2](https://github.com/Chakeyy/PharmacySI/issues/2) — Add & View (Member 2)**:
  - `addMedicine(Medicine meds[], int *count)`: Unique ID validation, prompt fields (name, category, price, quantity, expiry), append, and auto-save.
  - `viewMedicines(const Medicine meds[], int count)`: Formatted tabular display with aligned columns and a `[LOW STOCK]` warning tag for items with `quantity <= 10`.

- **[Issue #3](https://github.com/Chakeyy/PharmacySI/issues/3) — Update & Remove (Member 3)**:
  - `updateMedicine(Medicine meds[], int count)`: Search by ID, edit price/quantity/expiry, and auto-save.
  - `removeMedicine(Medicine meds[], int *count)`: Search by ID, shift subsequent array elements left to fill the gap, decrement `*count`, and auto-save.

- **[Issue #4](https://github.com/Chakeyy/PharmacySI/issues/4) — Listing (Sorted) & Sale (Member 4)**:
  - `sortAndViewMedicines(const Medicine meds[], int count)`: Copy array to a local buffer, Bubble Sort by user choice (Name, Price, Quantity, Expiry in Ascending / Descending order), and display with `viewMedicines()`.
  - `recordSale(Medicine meds[], int count)`: Search by ID, validate stock (`qty > 0 && qty <= stock`), deduct quantity, compute total, append receipt to `sales.txt`, and auto-save.

---

## Collaboration & Git Workflow

To prevent overwriting each other's code and avoid merge conflicts, please follow this workflow:

### 1. Step-by-Step Contribution Workflow
1. **Pull the latest `main` branch before starting**:
   ```bash
   git checkout main
   git pull origin main
   ```
2. **Create a new branch for your issue**:
   ```bash
   git checkout -b feature/issue-2-add-view   # Replace with your issue number
   ```
3. **Make and test your changes in `PharmacySI.c`**:
   - Only implement your assigned functions.
   - Do not change existing struct definitions or function signatures.
   - Test locally to ensure it compiles with zero warnings:
     ```bash
     gcc -Wall -Wextra -std=c99 PharmacySI.c -o pharmacy
     ```
4. **Commit and push your branch to GitHub**:
   ```bash
   git add PharmacySI.c
   git commit -m "Implement Add Medicine and View Medicine (fixes #2)"
   git push origin feature/issue-2-add-view
   ```
5. **Open a Pull Request (PR)**:
   - Go to the repository on GitHub.
   - Click **Compare & pull request**.
   - Reference your issue in the description (e.g., `Closes #2`).
   - Request a review before merging into `main`.

### 2. Coding & Defensive Programming Standards
- **Input Buffer Clearing**: When switching from numeric `scanf("%d", ...)` to string reading, leftover newlines in `stdin` can cause inputs to be skipped. Always flush the buffer using `clearInputBuffer()`.
- **String Input with Spaces**: Do **not** use `scanf("%s", ...)` for medicine names or categories (it truncates at the first space). Use `fgets()` and strip trailing newlines (`\n`).
- **Array Bounds Checking**: Always verify `*count < MAX` before adding, and check `count == 0` before viewing, updating, sorting, or selling.
- **Data Validation**: Reject IDs `<= 0`, prices `<= 0.00`, negative quantities, and ensure dates follow `YYYY-MM-DD`.

---

## How to Compile & Run

### 🪟 Windows (MinGW / GCC / Command Prompt / PowerShell)
```cmd
gcc -Wall -Wextra -std=c99 PharmacySI.c -o pharmacy.exe
pharmacy.exe
```
*If using an IDE like Code::Blocks or Dev-C++, open `PharmacySI.c`, ensure C99 mode is enabled in compiler settings, and press **Build and Run**.*

### 🐧 Linux / 🍎 macOS (Terminal)
```bash
gcc -Wall -Wextra -std=c99 PharmacySI.c -o pharmacy
./pharmacy
```

### 📱 Android (Termux)
- **Option A (Recommended — Running inside internal storage `~`)**:
  If you clone the repository directly into your Termux home directory, you can compile and run normally:
  ```bash
  cd ~
  git clone https://github.com/Chakeyy/PharmacySI.git
  cd PharmacySI
  gcc -std=c99 PharmacySI.c -o pharmacy
  ./pharmacy
  ```
- **Option B (Working in shared storage `/storage/emulated/0/...`)**:
  Android shared storage is mounted with the `noexec` permission (binaries cannot be executed directly from shared folders). Compile the binary into your Termux home folder:
  ```bash
  gcc PharmacySI.c -o ~/pharmacy && ~/pharmacy
  ```
  *(Note: Do not use `/root/` unless your Termux environment is explicitly rooted/PRoot).*

---

## Sample Test Data (`inventory.txt`)

To quickly test viewing, updating, sorting, and sales without manual data entry, create an `inventory.txt` file in the same directory:

```text
101|Paracetamol 500mg|Pain Relief|2.50|120|2027-03-15
102|Amoxicillin 500mg|Antibiotic|8.50|8|2026-12-01
103|Cetirizine 10mg|Antihistamine|1.75|50|2028-01-20
104|Ibuprofen 400mg|Pain Relief|3.25|5|2026-11-10
105|Mefenamic Acid 500mg|Pain Relief|4.00|35|2027-09-05
```

> **Testing Tip**: Items `102` (Qty: 8) and `104` (Qty: 5) have quantities $\le 10$, which will trigger the `[LOW STOCK]` warning tag in **[3] View Medicine** and alert during sales.
