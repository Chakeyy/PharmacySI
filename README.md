# Pharmacy Sales and Inventory System

[![C Standard](https://img.shields.io/badge/Language-C99-00599C?style=flat-square&logo=c)](https://en.wikipedia.org/wiki/C99)
[![Course](https://img.shields.io/badge/Course-CCS104-blue?style=flat-square)](#)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen?style=flat-square)](#)
[![License](https://img.shields.io/badge/License-Academic-lightgrey?style=flat-square)](#)

A console-based Pharmacy Sales and Inventory Management System developed in standard C (C99) for the CCS104 Data Structures and Algorithms course.

---

## Technical Constraints & Academic Guidelines

> [!IMPORTANT]
> **Strict CCS104 Compliance Rules**
> * **Standard**: Pure C99 using standard library headers only (`<stdio.h>`, `<stdlib.h>`, `<string.h>`).
> * **Single Source File**: All program logic resides in [`PharmacySI.c`](PharmacySI.c).
> * **Memory Model**: Fixed-size arrays only (`#define MAX 100`). Dynamic memory allocation (`malloc`, `calloc`, `realloc`, `free`) is prohibited.
> * **Data Structures**: Flat structures (`Medicine`). Linked lists and multi-level pointers (`**`) are prohibited.
> * **Algorithms**: Custom sorting algorithms only (e.g., Bubble Sort). Standard library `qsort()` is prohibited.
> * **File Persistence**: Plain-text, pipe-delimited flat files (`inventory.txt`, `sales.txt`).

---

## Data Schema & Storage Architecture

### 1. In-Memory Structure (`Medicine`)

```c
typedef struct {
    int id;              // Unique record identifier (> 0)
    char name[50];       // Medicine name (supports spaces)
    char category[30];   // Therapeutic category
    float price;         // Unit selling price (> 0.00)
    int quantity;        // Available stock units (>= 0)
    char expiry[11];     // Expiry date (Format: YYYY-MM-DD + '\0')
} Medicine;
```

| Field | Type | Constraints | Description |
|:---|:---|:---|:---|
| `id` | `int` | `> 0`, Unique | Primary identifier for search, updates, and removal |
| `name` | `char[50]` | Non-empty | Full commercial name |
| `category` | `char[30]` | Non-empty | Drug classification (e.g., Antibiotic, Pain Relief) |
| `price` | `float` | `> 0.00` | Unit price in local currency |
| `quantity` | `int` | `>= 0` | Current stock count; flags `[LOW STOCK]` when $\le 10$ |
| `expiry` | `char[11]` | `YYYY-MM-DD` | Expiration date formatted as 10 characters |

### 2. File Persistence

* **`inventory.txt`** — Primary stock ledger (`id|name|category|price|quantity|expiry`):
  ```text
  101|Paracetamol 500mg|Pain Relief|2.50|120|2027-03-15
  102|Amoxicillin 500mg|Antibiotic|8.50|8|2026-12-01
  ```
  *Loaded into memory on program launch via `loadInventory()` and auto-saved via `saveInventory()` after every state change.*

* **`sales.txt`** — Audit log for transactions (`id|name|quantity|total`):
  ```text
  101|Paracetamol 500mg|5|12.50
  ```
  *Appended automatically upon each completed purchase via `recordSale()` without overwriting past entries.*

---

## Feature Matrix & Implementation Status

| Option | Feature | Assigned Role | Primary Function | Status |
|:---:|:---|:---|:---|:---:|
| **`[1]`** | Add Medicine | Member 2 ([Issue #2](https://github.com/Chakeyy/PharmacySI/issues/2)) | `addMedicine(Medicine meds[], int *count)` | Completed |
| **`[2]`** | Update Medicine | Member 3 ([Issue #3](https://github.com/Chakeyy/PharmacySI/issues/3)) | `updateMedicine(Medicine meds[], int count)` | In Progress |
| **`[3]`** | View Inventory | Member 2 ([Issue #2](https://github.com/Chakeyy/PharmacySI/issues/2)) | `viewMedicines(const Medicine meds[], int count)` | Completed |
| **`[4]`** | Remove Medicine | Member 3 ([Issue #3](https://github.com/Chakeyy/PharmacySI/issues/3)) | `removeMedicine(Medicine meds[], int *count)` | Completed |
| **`[5]`** | Listing (Sorted) | Member 4 ([Issue #4](https://github.com/Chakeyy/PharmacySI/issues/4)) | `sortAndViewMedicines(const Medicine meds[], int count)` | Completed |
| **`[6]`** | Record Sale | Member 4 ([Issue #4](https://github.com/Chakeyy/PharmacySI/issues/4)) | `recordSale(Medicine meds[], int count)` | In Progress |
| **`[0]`** | Exit & UI Control | Lead (Core Architecture) | `main()`, `clearScreen()`, `pauseScreen()` | Completed |

---

## Defensive Programming Standards

* **Buffer Sanitation**: All integer and float inputs via `scanf` must immediately flush trailing newline characters (`\n`) using `clearInputBuffer()`.
* **String Preservation**: Multi-word string inputs (e.g. medicine names and categories) utilize scan formats like `scanf(" %49[^\n]", ...)` or `fgets()` to prevent truncation at spaces.
* **Array Bound Safety**: Guard every insertion with `*count < MAX` and every query/mutation with `count == 0` validation.
* **Date Validation**: Expiry inputs are verified to guarantee exact 10-character length, numerical digits, and hyphens at indices 4 and 7 (`YYYY-MM-DD`).
* **Non-Destructive Sorting**: `sortAndViewMedicines()` copies the working array into a local `tempMeds` buffer before executing Bubble Sort, ensuring database index integrity remains intact.

---

## Compilation & Execution

```bash
# General C99 Compilation Flag
gcc -Wall -Wextra -std=c99 PharmacySI.c -o PharmacySI
```

| Platform | Build & Run Command |
|:---|:---|
| **Linux / macOS** | `gcc -Wall -Wextra -std=c99 PharmacySI.c -o PharmacySI && ./PharmacySI` |
| **Windows (CMD / PowerShell)** | `gcc -Wall -Wextra -std=c99 PharmacySI.c -o PharmacySI.exe && PharmacySI.exe` |
| **GitHub Codespaces** | `gcc -Wall -Wextra -std=c99 PharmacySI.c -o PharmacySI && ./PharmacySI` |
| **Android (Termux Home `~`)** | `gcc -std=c99 PharmacySI.c -o PharmacySI && ./PharmacySI` |
| **Android (Shared Storage)** | `gcc PharmacySI.c -o ~/PharmacySI && ~/PharmacySI` *(Note: Shared storage has `noexec` flags)* |

---

## Development & Git Workflow

To maintain a conflict-free repository, team members must adhere to feature branches:

1. **Synchronize with latest `main`**:
   ```bash
   git checkout main
   git pull origin main
   ```
2. **Create a dedicated feature branch**:
   ```bash
   git checkout -b feature/issue-<number>-<feature-name>
   ```
3. **Verify compilation locally**:
   Ensure zero compiler warnings before staging:
   ```bash
   gcc -Wall -Wextra -std=c99 PharmacySI.c -o PharmacySI
   ```
4. **Commit and submit Pull Request**:
   ```bash
   git add PharmacySI.c
   git commit -m "Implement <Feature Name> (closes #<Issue Number>)"
   git push origin feature/issue-<number>-<feature-name>
   ```

---

## Sample Test Dataset (`inventory.txt`)

You can populate `inventory.txt` in the root directory to immediately test listing, sorting, and low-stock indicators without manual entry:

```text
101|Paracetamol 500mg|Pain Relief|2.50|120|2027-03-15
102|Amoxicillin 500mg|Antibiotic|8.50|8|2026-12-01
103|Cetirizine 10mg|Antihistamine|1.75|50|2028-01-20
104|Ibuprofen 400mg|Pain Relief|3.25|5|2026-11-10
105|Mefenamic Acid 500mg|Pain Relief|4.00|35|2027-09-05
```

> [!NOTE]
> Medicines `102` (Stock: 8) and `104` (Stock: 5) have quantities $\le 10$, which will automatically activate the `[LOW STOCK]` alert tag in the tabular view.
