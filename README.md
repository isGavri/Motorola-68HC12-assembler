# Motorola-68HC12-assembler

An attempt to a C assembler for the Motorola 68HC12 microcontroller.
This was inspired by a python assembler we had to write as a project on my low level programming course at my University but it didn't feel satifiying enough,
so I'm re-doing it on the best language to ever exist.

## Two-Phase 68HC12 Assembler

This implementation was achieved by making use of the !(Motorola 68HC12 Reference Manual 1996)[https://archive.org/details/bitsavers_motorola68eferenceManual1996_20968848]

### Phase 1 – Symbol & Size Calculation

On this phase we:
- Parse source lines

- Track location counter (LC)

- Build symbol table (labels and constants)

- Determine addressing mode

- Determine instruction size based on addressing mode and opcode

- Generate intermediate representation (*.lst file)

### Phase 2 – Machine Code Generation

On this phase we:

- Read intermediate lines

- Resolve symbol addresses

- Encode instructions into binary bytes

- Replace placeholders (ee, ff, rr, xb, etc.) with actual operand calculated values

- Produce new listing and object code

## Assembler Implementation Outline

### Phase 1: Lexical Analysis and Statement Structure (Updated Constraints)

The goal is to correctly segment the source code lines (`P1ASM.asm`) into `label`, `opcode`, and `operand` tokens.
At this point we will print the line instruction. Token with token value.

#### A. Input Format and Delimiters
1.  **Line Delimiter:** The carriage return serves as the definitive delimiter for every assembly statement.
2.  **Token Separation:** Words must be identified while ignoring spaces and tabulators.

#### B. Line Type Constraints (Size and Character Rules)

| Element | Rule/Constraint | Source |
| :--- | :--- | :--- |
| **Comment** | It's completely ignored source code as long as the scanner finds a (`;`) it stops analyzing that line | |
| **Label** | Must start with a letter (case-insensitive) located in the first position of the line. | |
| **Label** | **Length must be between 3 and 8 characters**. | |
| **Label** | Subsequent characters (after the first letter) can be letters, numbers, or underscores (`_`). Any other character results in an error. | |
| **Opcode** | Must begin with a letter (case-insensitive). | |
| **Opcode** | **Maximum length is 5 characters**. | |
| **Operand** | Can start with any character; subsequent practices review specific format exceptions. Can have any length. | |

#### C. Token Output and Initialization
1.  **Variable Naming:** Use string variables named `label`, `opcode`, and `operand`.
2.  **Null Assignment:** If a variable is not assigned a value for a given line, it must be assigned the string value “null” aka `(void*)0`.
3.  **Initialization:** After printing the results for each line, variables must be re-initialized with the null character to prevent "garbage" storage.
4.  **Mandatory CODOP:** The `opcode` field must always contain a word; otherwise, the line is considered an error.
5.  **Comment Handling:** For a comment line, the output must be the message “comentario”.

### Phase 2: Addressing Mode (ADDR) and Operand Validation

The system must validate operands based on numeric base rules and constraints specific to the Motorola HC12 architecture.

#### A. Numeric Base Validation
The assembler supports four numerical bases:
1.  **Hexadecimal:** Starts with `$`, followed by digits 0-9 and letters A-F.
2.  **Octal:** Starts with `@`, followed by digits 0-7.
3.  **Binary:** Starts with `%`, followed by digits 0 or 1.
4.  **Decimal:** Starts with digits 0-9.
5.  **Leading Zeros:** All bases, except decimal, allow leading zeros.

We parse each numerical base to decimal for operations, then back to hexadecimal on machine code.

#### B. Addressing Mode Constraints (Numeric Range/Format)
Operand values must be validated against specific bit sizes, ranges, and formats.

| ADDR Mode | Description/Format | Numeric Range Constraints (Decimal) | Opcode Size/Bytes | Source |
| :--- | :--- | :--- | :--- | :--- |
| **Inherente (INH)** | No operand. | N/A | Varies (e.g., 1 or 2 bytes). | |
| **Immediate (IMM)** | Starts with `#`. Uses all four bases. | 8-bit: 0 to 255. 16-bit: 256 to 65,535. | Varies (e.g., 2 or 3 bytes). | |
| **Direct (DIR)** | Uses all four bases. | 0 to 255. | Varies (e.g., 2 bytes). | |
| **Extended (EXT)** | Uses all four bases, or a label. | 256 to 65,535. | Varies (e.g., 3 bytes). | |
| **Indexed (IDX) 5-bit**| Format: `n,R` where R is X, Y, SP, or PC. | -16 to 15. | Varies (e.g., 2 bytes). | |
| **Indexed (IDX) 5-bit Exception** | `,X` must be interpreted as `0,X`. | 0 (implicitly). | 2 bytes. | |
| **Indexed (IDX1) 9-bit**| Format: `n,R` where R is X, Y, SP, or PC. | -256 to -17 AND 16 to 255. | Varies (e.g., 3 bytes). | |
| **Indexed (IDX2) 16-bit**| Format: `n,R` where R is X, Y, SP, or PC. | 256 to 65,535. | Varies (e.g., 4 bytes). | |
| **Indexed (IDX) Pre/Post**| Format: `n,+R`, `n,-R`, `R+`, `R-`. Registers R are X, Y, or SP (PC not allowed). Offset $n$ is 1 to 8. | $\pm$ 1 to 8. | Varies (e.g., 2 bytes). | |
| **Indexed (IDX) Acc Offset**| Format: `A/B/D,R`. Registers R are X, Y, SP, or PC. Accumulators A, B are 8-bit offset; D is 16-bit offset. | N/A (offset is register value) | Varies (e.g., 2 bytes). | |
| **Indexed Indirect \[IDX2\]**| Format: Must be enclosed in brackets `[n,R]`. R is X, Y, SP, or PC. | 0 to 65,535. | Varies (e.g., 4 bytes). | |
| **Indexed Indirect \[D,IDX\]**| Format: Must be enclosed in brackets `[D,R]`. Only D register before comma. R is X, Y, SP, or PC. | N/A (offset is D register value) | Varies (e.g., 2 bytes). | |
| **Relative (REL) 8/16-bit**| Operand must be a word (label) satisfying label rules. Instruction determines 8 or 16-bit jump. | N/A (calculated offset) | Varies (e.g., 2 or 4 bytes). | |
| **Relative (REL) 9-bit/Loop Primitive**| Format: `R, Label`. Register R (A, B, D, X, Y, SP). Used for instructions like `IBNE`, `DBEQ`. | N/A (calculated offset) | 3 bytes. | |

#### C. Error Handling
Specific errors must be reported, such as:
*   Operand out of range.
*   Addressing mode not accepted.
*   Label format error.

### Phase 3: Directives and Symbol Table Management

This phase focuses on memory addressing (`ALC - address locality counter`) and generating auxiliary files (`{filename}.lst` and `TABSIM.txt`).

#### A. Location Counter (CONTLOC)
1.  **Format:** Must be handled as a hexadecimal variable.
2.  **Range:** Values range from 0 to 65,535.
3.  **Output Format:** Must be padded with leading zeros to four hexadecimal digits in the listing file (`VALOR` column).
4.  **Increment:** CONTLOC is incremented by:
    *   The size (number of bytes) of each instruction.
    *   The total size generated by data accommodation directives.

#### B. Assembler Directives

| Directive | Function | Constraints | Effect on CONTLOC | Source |
| :--- | :--- | :--- | :--- | :--- |
| **ORG** | Sets the starting memory address (DIR\_INIC). | Must exist only once. The label field must be null. Operand range: 0 to 65,535. | Sets initial `CONTLOC`. | |
| **EQU** | Assigns an absolute value to a label. | Must always have a label (ETIQUETA) and an operand. Operand range: 0 to 65,535. | Does not generate code or advance CONTLOC (uses `VALOR` output type). | |
| **END** | Signals the end of assembly. | Must be the last instruction and exist only once. | N/A | |
| **DC (Define Constant)**| Defines constant values in memory. | Requires a size unit (`.B` or `.W`). `.B` range: 0 to 255. `.W` range: 0 to 65,535. | Increments CONTLOC by the total bytes defined. | |
| **DS (Define Space)**| Reserves memory space initialized to 0's. | Requires a size unit (`.B` or `.W`) and a quantity of contiguous blocks. | Increments CONTLOC by (Unit Size * Quantity). | |

#### C. Symbol Table (TABSIM.txt)
1.  **Output File:** `TABSIM.txt`.
2.  **Contents:** Stores the symbol (ETIQUETA, $S_i$) and its corresponding value ($T_i$).
3.  **Value Format:** Values ($T_i$) must be stored as two bytes (4 hexadecimal digits), always in hexadecimal format.
4.  **Label Type:** Labels assigned by `CONTLOC` are **Relative**; those assigned by `EQU` are **Absolute**.
5.  **Uniqueness:** Labels cannot be repeated.
6.  **Timing:** Labels are added as they appear in the source code.

### Phase 4: Machine Code Generation

The final phases require calculating the machine code (opcode + extension bytes/postbytes) using the Opcode Map (TABOP) and Indexed Addressing Tables (A-3, A-4, A-6).

#### A. General Encoding Rules
1.  The base Opcode is retrieved from TABOP.
2.  Numeric operands must be converted to hexadecimal.
3.  All machine code segments (immediate data, addresses, offsets, postbytes) must be padded with leading zeros to match the correct byte size (`ii`, `jj kk`, `dd`, `hh ll`, `rr`, `qq rr`).

#### B. Specific Encoding Requirements (Numeric Operands)

| ADDR Mode | Encoding Components | Calculation Details | Source |
| :--- | :--- | :--- | :--- |
| **Inherente (INH)** | Opcode only (1 or 2 bytes). | No additional calculation. | |
| **Immediate (IMM)** | Opcode + $ii$ (8-bit) or $jj kk$ (16-bit). | Operand converted to hexadecimal format corresponding to 1 or 2 bytes, depending on range. | |
| **Direct (DIR)** | Opcode + $dd$ (8-bit address). | Operand value is converted to 8-bit hexadecimal. | |
| **Extended (EXT)** | Opcode + $hh ll$ (16-bit address). | Operand value converted to 16-bit hexadecimal ($hh$ high byte, $ll$ low byte). | |

#### C. Indexed Addressing Encoding

Indexed addressing modes (IDX, IDX1, IDX2) require calculating the postbyte ($xb$) using lookup tables (A-3, A-4 on the Manual | [./resources/postbyte.csv] for the program).

| ADDR Mode | Encoding Components | Calculation Details | Source |
| :--- | :--- | :--- | :--- |
| **IDX (5-bit, Pre/Post, Acc)** | Opcode + $xb$ (postbyte). | $xb$ calculated based on offset (if 5-bit) or increment/decrement/accumulator type and register (X, Y, SP, PC) using Table A-3/A-4. | |
| **IDX1 (9-bit offset)** | Opcode + $xb$ + $ff$ (extension byte). | $xb$ encodes the register and the sign bit. $ff$ is the 8-bit lower portion of the offset. Positive values use absolute hex value for $ff$. Negative values use the 2's complement of the absolute value for $ff$. | |
| **IDX2 (16-bit offset)** | Opcode + $xb$ + $ee ff$ (two extension bytes). | $xb$ is calculated using Table A-3/A-4. $ee ff$ is the 16-bit hexadecimal representation of the offset value. | |
| **Indexed Indirect [IDX2]**| Opcode + $xb$ + $ee ff$ (two extension bytes). | $xb$ uses the specific encoding for 16-bit offset indexed-indirect (Table A-3/A-4). $ee ff$ is the 16-bit hexadecimal value of the numeric operand within the brackets. | |
| **Indexed Indirect [D,IDX]**| Opcode + $xb$. | $xb$ uses the specific encoding for D accumulator offset indexed-indirect (Table A-3/A-4). | |

#### D. Relative Addressing Encoding

Offset calculation determines the value of $rr$ (8-bit short offset) or $qq rr$ (16-bit long offset). The offset is calculated relative to the address of the byte immediately following the offset byte(s).

1.  **Relative Address Calculation:**
    *   **Positive Jump:** Offset = Destination Address - Origin Address.
    *   **Negative Jump:** Offset = $C2(\text{Origin Address} - \text{Destination Address})$ (Two's complement).
    *   **Neutro (Self-Loop):** Offset is $00$ (8-bit) or $00 00$ (16-bit).

2.  **Relative (REL) 8-bit/16-bit:**
    *   **Short Branches:** Opcode + $rr$ (8-bit signed offset).
    *   **Long Branches (L mnemonic):** Opcode (`$18 2x`) + $qq rr$ (16-bit signed offset).

3.  **Relative (REL) 9-bit (Loop Primitives):**
    *   Opcode (`$04`) + $lb$ (loop postbyte) + $rr$ (8-bit offset byte).
    *   $lb$ is determined by the specific instruction (`DBEQ`/`DBNE`/`IBEQ`/`IBNE`/`TBEQ`/`TBNE`), the counter register (A, B, D, X, Y, SP), and the sign of the displacement (positive or negative) using Table A-6.

## Relevant to the implementation
2️⃣ Data Structures
### a) ISA Table Entry

```c
typedef struct {
    char mnemonic[6];
    char operand_spec[16];  // e.g., "-", "#opr8i", "oprx16,xysp"
    char addr_mode[8];      // e.g., INH, IMM, DIR, EXT, IDX, REL
    uint8_t opcode[3];
    uint8_t delta;          // info for CCR flags
    uint8_t size;           // total instruction bytes
} ISAEntry;
```


Opcode array allows up to 3 bytes, covering multi-byte opcodes like extended/direct instructions.

### b) Symbol Table

Stores labels and constants.

```c
typedef struct Symbol {
    char label[32];
    uint16_t address;
    bool defined;
    struct Symbol *next; // linked list for simplicity
} Symbol;
```

Insert labels in phase 1

Lookup during phase 2

Optional: hash table for faster lookup

### c) Intermediate Line Representation

Each source line is parsed into a struct during phase 1.

```c
typedef struct {
    int line_num;             // source line number
    char label[10];           // optional label
    char mnemonic[6];         // instruction mnemonic
    char operand[64];         // raw operand text
    char addr_mode[8];        // detected addressing mode
    const ISAEntry *isa;      // pointer to the matching ISA row
    uint16_t address;         // calculated LC
    uint8_t size;             // number of bytes
    bool comment;             // is a comment line?
} InstructionLine;

```

Phase 2 will read these lines to generate machine code.

isa pointer gives direct access to opcode bytes and format.

### d) Location Counter

Tracks memory address:

```c
uint16_t LC = 0x0000; // start address
```


Incremented by size for each instruction in phase 1

Used to calculate relative offsets (rr) and symbol addresses

### e) Machine Code Encoding

The plan for byte generation:

Take opcode bytes from the ISA row

Determine how many bytes need calculation (ee, ff, rr, xb, ii)

Fill calculated bytes from right to left:

Example: LDAA EXT  ->  opcode: B6, format: ee ff
- B6 = fixed opcode
- ee ff = 16-bit address
- Place ee (MSB) and ff (LSB) after B6


Immediate, direct, extended, indexed, relative handled with helper functions

## Flow of program
Pseudocode of the main flow
### Phase 1
```

for each line in source:
    parse mnemonic, operands, label
    detect addressing mode
    lookup ISAEntry in CSV-based table
    set line.size = ISAEntry.size
    set line.address = LC
    if label exists:
        add to symbol table
    LC += line.size
```
### Phase 2
```
for each InstructionLine in intermediate list:
    resolve operand(s) to bytes
    if relative addressing:
        calculate rr from target label - (current address + size)
    if direct/extended/indexed/immediate:
        parse operand to ee/ff/xb/ii bytes
    write opcode bytes + calculated bytes to output
```
