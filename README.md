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
> * **Algorithms**: Custom sorting algorithms only (Bubble Sort). Standard library `qsort()` is prohibited.
> * **File Persistence**: Plain-text, pipe-delimited flat files (`inventory.txt`, `sales.txt`).

---

## Data Schema & Storage Architecture

### 1. In-Memory Structure (`Medicine`)

```c
typedef struct {
    int id;              // Auto-assigned unique record identifier (starts at 101)
    char name[50];       // Medicine name (supports spaces)
    char category[30];   // Therapeutic category
    float price;         // Unit selling price (> 0.00)
    int quantity;        // Available stock units (>= 0)
    char expiry[11];     // Expiry date (Format: YYYY-MM-DD + '\0')
} Medicine;
```

| Field | Type | Constraints | Description |
|:---|:---|:---|:---|
| `id` | `int` | `>= 101`, Unique | Primary identifier auto-generated via `getNextId()` for search, updates, and removal |
| `name` | `char[50]` | Non-empty | Full commercial name (pipes automatically sanitized) |
| `category` | `char[30]` | Non-empty | Drug classification (e.g., Antibiotic, Pain Relief) |
| `price` | `float` | `> 0.00` | Unit price in local currency |
| `quantity` | `int` | `>= 0` | Current stock count; flags `[OUT]` when $0$ and `[LOW STOCK]` when $1 \le \text{qty} \le 10$ |
| `expiry` | `char[11]` | `YYYY-MM-DD` | Expiration date validated against real calendar month lengths and leap years |

### 2. File Persistence

* **`inventory.txt`** — Primary stock ledger (`id|name|category|price|quantity|expiry`):
  ```text
  101|Paracetamol 500mg|Pain Relief|2.50|120|2027-03-15
  102|Amoxicillin 500mg|Antibiotic|8.50|8|2026-12-01
  ```
  *Automatically created if missing on startup via `initDataFiles()`, loaded into memory via `loadInventory()`, and auto-saved via `saveInventory()` after every state change.*

* **`sales.txt`** — Audit log for completed sales (`id|name|quantity|total`):
  ```text
  101|Paracetamol 500mg|5|12.50
  ```
  *Automatically created if missing on startup and appended upon each confirmed transaction via `recordSale()` and `logSale()`.*

---

## Feature Matrix & Implementation Status

| Option | Feature | Assigned Role | Primary Function | Status |
|:---:|:---|:---|:---|:---:|
| **`[1]`** | Add Medicine | Member 2 ([Issue #2](https://github.com/Chakeyy/PharmacySI/issues/2)) | `addMedicine(Medicine meds[], int *count)` | Completed |
| **`[2]`** | Update Medicine | Member 3 ([Issue #3](https://github.com/Chakeyy/PharmacySI/issues/3)) | `updateMedicine(Medicine meds[], int count)` | Completed |
| **`[3]`** | View Inventory | Member 2 ([Issue #2](https://github.com/Chakeyy/PharmacySI/issues/2)) | `viewMedicines(const Medicine meds[], int count)` | Completed |
| **`[4]`** | Remove Medicine | Member 3 ([Issue #3](https://github.com/Chakeyy/PharmacySI/issues/3)) | `removeMedicine(Medicine meds[], int *count)` | Completed |
| **`[5]`** | Listing (Sorted) | Member 4 ([Issue #4](https://github.com/Chakeyy/PharmacySI/issues/4)) | `sortAndViewMedicines(const Medicine meds[], int count)` | Completed |
| **`[6]`** | Sell Medicine | Member 4 ([Issue #4](https://github.com/Chakeyy/PharmacySI/issues/4)) | `recordSale(Medicine meds[], int count)` | Completed |
| **`[0]`** | Exit & UI Control | Lead (Core Architecture) | `main()`, `clearScreen()`, `pauseScreen()` | Completed |

---

## Quality-of-Life (QoL) & Defensive Programming Standards

* **Auto-Generated IDs**: New medicines automatically receive the next sequential ID starting at `101` (`maxId + 1`), eliminating manual ID collisions.
* **Flexible Expiry Date Input**: `readAndFormatExpiry()` accepts `YYYY-MM-DD`, `YYYY MM DD`, or `YYYYMMDD`, validates calendar day bounds (including leap years for February), and normalizes storage to `YYYY-MM-DD`.
* **Field-Level Retry & Instant Cancellation**: Invalid inputs re-prompt only the active field instead of restarting the entire form. Entering `0` at designated prompts immediately cancels the current operation and returns to the main menu.
* **Selective Field Updates**: During `Update Medicine`, entering `-1` for Price, Quantity, or Expiry retains the existing value without requiring re-entry.
* **Duplicate Name Warning**: Adding a medicine whose name already exists (case-insensitive check via `findMedicineByName()`) alerts the user with the existing ID and asks for confirmation before creating a duplicate entry.
* **Delimiter Sanitization & Corruption Guard**: Any `|` character typed in string inputs is automatically replaced with `/` via `sanitizePipes()` so `inventory.txt` and `sales.txt` columns cannot be corrupted. `loadInventory()` skips malformed or duplicate-ID records and reports skipped lines on startup.
* **Verified File Saves**: `saveInventory()` and `logSale()` return status codes (`1` on success, `0` on failure). If saving fails during an operation, in-memory changes are rolled back or reported immediately.
* **Non-Destructive Sorting**: `sortAndViewMedicines()` copies the working array into a local `tempMeds` buffer before executing Bubble Sort, ensuring the primary array order remains intact.

---

## Code Layout (`PharmacySI.c`)

1. **`#includes` and `#defines`**: Standard headers (`<stdio.h>`, `<stdlib.h>`, `<string.h>`) and named constants (`MAX`, `START_ID`, `LOW_STOCK_LIMIT`, `BOX_WIDTH`, `INVENTORY_FILE`, `SALES_FILE`).
2. **Struct Definition**: `Medicine` structure definition.
3. **Function Prototypes**: Grouped by functional module for top-down readability.
4. **`main()` Loop**: Initializes data files, loads existing inventory, and runs the interactive `do-while` and `switch` menu loop.
5. **Modular Sections**:
   * Utility & Input Helpers (`clearScreen`, `clearInputBuffer`, `pauseScreen`, `findMedicineById`, `findMedicineByName`, `getNextId`, `sanitizePipes`, `readAndFormatExpiry`, `promptMedicineIndex`)
   * File Handling (`initDataFiles`, `loadInventory`, `saveInventory`, `logSale`)
   * Inventory Operations (`addMedicine`, `viewMedicines`, `updateMedicine`, `removeMedicine`)
   * Sorting (`sortAndViewMedicines`)
   * Sales Operations (`recordSale`)

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

## Sample Test Dataset (`inventory.txt`)

You can populate `inventory.txt` in the root directory to immediately test listing, sorting, out-of-stock, and low-stock indicators without manual entry:

```text
101|Paracetamol 500mg|Pain Relief|2.50|120|2027-03-15
102|Amoxicillin 500mg|Antibiotic|8.50|8|2026-12-01
103|Cetirizine 10mg|Antihistamine|1.75|50|2028-01-20
104|Ibuprofen 400mg|Pain Relief|3.25|0|2026-11-10
105|Mefenamic Acid 500mg|Pain Relief|4.00|35|2027-09-05
```

> [!NOTE]
> Medicine `102` (Stock: 8) activates the `[LOW STOCK]` alert tag ($\le 10$), while Medicine `104` (Stock: 0) displays the `[OUT]` tag in the tabular view.
