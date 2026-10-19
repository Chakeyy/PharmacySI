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
    int id;              // Auto-assigned unique batch identifier (starts at 101)
    char name[50];       // Medicine name (supports spaces and multiple batches)
    char category[30];   // Therapeutic category
    float price;         // Unit selling price (> 0.00)
    int quantity;        // Available stock units (>= 0)
    char expiry[11];     // Expiry date (Format: YYYY-MM-DD + '\0')
} Medicine;
```

| Field | Type | Constraints | Description |
|:---|:---|:---|:---|
| `id` | `int` | `>= 101`, Unique | Batch identifier auto-generated for search, updates, sales, and removal |
| `name` | `char[50]` | Non-empty | Commercial name; multiple batches of the same medicine with different expiry dates are supported |
| `category` | `char[30]` | Non-empty | Drug classification (e.g., Antibiotic, Pain Relief) |
| `price` | `float` | `> 0.00` | Unit selling price |
| `quantity` | `int` | `>= 0` | Current batch stock; flags `[OUT]` when $0$ and `[LOW STOCK]` when $1 \le \text{qty} \le 10$ |
| `expiry` | `char[11]` | `YYYY-MM-DD` | Validated calendar date; flags `[EXPIRED]` when earlier than `CURRENT_DATE` |

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
| **`[1]`** | Add Medicine (New Batch) | Member 2 ([Issue #2](https://github.com/Chakeyy/PharmacySI/issues/2)) | `addMedicine(Medicine meds[], int *count)` | Completed |
| **`[2]`** | Update Medicine | Member 3 ([Issue #3](https://github.com/Chakeyy/PharmacySI/issues/3)) | `updateMedicine(Medicine meds[], int count)` | Completed |
| **`[3]`** | View & Sort Inventory | Member 2 & Member 4 ([Issue #2](https://github.com/Chakeyy/PharmacySI/issues/2), [Issue #4](https://github.com/Chakeyy/PharmacySI/issues/4)) | `viewMedicines()`, `sortAndViewMedicines()` | Completed |
| **`[4]`** | Remove Medicine | Member 3 ([Issue #3](https://github.com/Chakeyy/PharmacySI/issues/3)) | `removeMedicine(Medicine meds[], int *count)` | Completed |
| **`[5]`** | Sell Medicine | Member 4 ([Issue #4](https://github.com/Chakeyy/PharmacySI/issues/4)) | `recordSale(Medicine meds[], int count)` | Completed |
| **`[0]`** | Exit & UI Control | Lead (Core Architecture) | `main()`, `clearScreen()`, `pauseScreen()` | Completed |

---

## Quality-of-Life (QoL) & Defensive Programming Standards

* **Batch-Based Auto-IDs**: Each added shipment receives a sequential Batch ID starting at `101` (`maxId + 1`), allowing multiple batches of the same medicine with different expiry dates to be tracked independently.
* **Expired Batch Detection & Sale Protection**: `isExpired()` compares `YYYY-MM-DD` strings against `CURRENT_DATE` using `strcmp()`. Expired batches are flagged as `[EXPIRED]` in the inventory view, counted on the main dashboard, and blocked from being sold in `recordSale()`.
* **Selective Field Updates**: In `Update Medicine`, users choose whether to update `[1] Unit Price`, `[2] Stock Quantity`, `[3] Expiry Date`, or `[4] All Fields` instead of re-entering unchanged values.
* **Integrated View & Non-Destructive Sorting**: Selecting `[3] View & Sort Inventory` displays the full inventory table and lets the user sort in-place by Name, Price, Stock, or Expiry (Ascending or Descending) using Bubble Sort on a temporary copy (`tempMeds`) without altering the saved array order.
* **Flexible Expiry Date Input**: `readAndFormatExpiry()` accepts `YYYYMMDD`, `YYYY MM DD`, or `YYYY-MM-DD`, validates calendar day bounds (including leap years for February), and normalizes storage to `YYYY-MM-DD`.
* **Field-Level Retry & Instant Cancellation**: Invalid inputs re-prompt only the active field instead of restarting the form. Entering `0` at designated prompts immediately cancels the current operation.
* **Delimiter Sanitization & Corruption Guard**: Any `|` character typed in string inputs is replaced with `-` via `sanitizePipes()`. `loadInventory()` skips malformed or duplicate-ID records and reports skipped lines on startup.

---

## Code Layout (`PharmacySI.c`)

1. **Constants (`#define`)**: `MAX`, `START_ID`, `LOW_STOCK_LIMIT`, `BOX_WIDTH`, `CURRENT_DATE`, `INVENTORY_FILE`, `SALES_FILE`.
2. **Data Structure**: `Medicine` struct definition.
3. **Function Prototypes**: Grouped by module.
4. **`main()` Loop**: Initializes data files, loads inventory, displays live stock/expired alerts, and routes menu options `0–5`.
5. **Modular Sections**:
   * `UTILITY / INPUT HELPERS`
   * `FILE HANDLING`
   * `INVENTORY OPERATIONS`
   * `SORTING`
   * `SALES`

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

You can populate `inventory.txt` in the root directory to immediately test batch listing, sorting, expired detection, out-of-stock, and low-stock indicators:

```text
101|Paracetamol 500mg|Pain Relief|2.50|120|2027-03-15
102|Paracetamol 500mg|Pain Relief|2.50|40|2025-08-10
103|Amoxicillin 500mg|Antibiotic|8.50|8|2026-12-01
104|Cetirizine 10mg|Antihistamine|1.75|50|2028-01-20
105|Ibuprofen 400mg|Pain Relief|3.25|0|2026-11-10
```

> [!NOTE]
> * Batch `102` (`2025-08-10`) is earlier than `CURRENT_DATE` (`2026-10-19`) and displays `[EXPIRED]` (blocked from sale).
> * Batch `103` (Stock: 8) displays `[LOW STOCK]`.
> * Batch `105` (Stock: 0) displays `[OUT]`.
