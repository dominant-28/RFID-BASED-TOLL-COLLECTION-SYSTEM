# RFID Based Toll Collection System

An automated toll collection system using RFID technology, built with the 8051 microcontroller (AT89C51). This system provides contactless payment processing with balance management and recharge capabilities.

## Features

- **RFID Card Authentication**: Automatic vehicle identification using RFID cards
- **Balance Management**: Real-time balance tracking for multiple users
- **Automated Gate Control**: Motor-driven barrier gate operation
- **Low Balance Alert**: Audio-visual warnings when balance is insufficient
- **Recharge Facility**: On-site balance recharge via keypad interface
- **LCD Display**: 16x2 LCD for user feedback and transaction details
- **UART Communication**: Serial debugging and monitoring capability
- **Audio Feedback**: Buzzer alerts for different system events

## Hardware Components

| Component | Specification | Purpose |
|-----------|--------------|---------|
| Microcontroller | AT89C51 (8051) | Main processing unit |
| LCD Display | 16x2 Character LCD | User interface display |
| RFID Reader | EM-18 or similar | Card reading module |
| Keypad | 4x3 Matrix Keypad | User input for recharge |
| Motor Driver | L293D or similar | Gate control mechanism |
| Buzzer | Active/Passive Buzzer | Audio alerts |
| Crystal Oscillator | 11.0592 MHz | Clock source for UART |

## Pin Configuration

### LCD Connections (16x2)
- **Data Pins**: P2.0 - P2.7 (8-bit mode)
- **RS**: P1.0 (Register Select)
- **RW**: P1.1 (Read/Write)
- **EN**: P1.2 (Enable)

### RFID Module
- **TX**: Connected to RX (P3.0) via UART
- **Baud Rate**: 9600 bps

### Keypad (4x3 Matrix)
- **Rows**: P3.3, P3.4, P3.5, P3.6
- **Columns**: P1.5, P1.6, P1.7

### Motor Driver (Gate Control)
- **IN1**: P1.3
- **IN2**: P1.4

### Buzzer
- **Control Pin**: P3.7

##  Circuit Diagram

![Circuit Diagram](https://drive.google.com/uc?export=view&id=1BIFTTxMIeuAYuFQxCPPQmAXVO0wWdZjr
)
*The circuit diagram shows the complete hardware setup and connections*

## Software Architecture

### Main Components

1. **Card Reading Module** (`read_card()`)
   - Reads 12-character RFID card data via UART
   - Validates card format and stores data

2. **Balance Management** (`display_balance()`, `handle_low_balance()`)
   - Tracks individual card balances
   - Handles insufficient balance scenarios
   - Provides recharge options

3. **Gate Control** (`open_gate()`)
   - Opens gate (forward motor rotation)
   - Maintains open state (3-second delay)
   - Closes gate (reverse motor rotation)

4. **Recharge System** (`recharge_card()`)
   - Accepts 3-digit amount input via keypad
   - Confirms transaction before processing
   - Updates balance and provides feedback

5. **Keypad Interface** (`keypad_scan()`)
   - Scans 4x3 matrix keypad
   - Debouncing implemented
   - Returns pressed key value

## System Flow

```
Start → Initialize LCD & UART → Display "Scan RFID Card"
  ↓
Card Scanned → Validate Card
  ↓
Valid Card? → Yes → Check Balance
  ↓              ↓
  No             Sufficient? → Yes → Deduct Toll → Open Gate
  ↓              ↓
Invalid Card     No → Low Balance Alert
  ↓              ↓
Buzzer Alert     Offer Recharge Option
  ↓              ↓
Return to Start  1: Recharge → Process Payment → Update Balance
                 2: Exit → Cancel Transaction
```

## Getting Started

### Prerequisites
- Keil µVision IDE or SDCC compiler
- 8051 programmer (USB ASP or similar)
- Proteus (for simulation)

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/rfid-toll-collection.git
   cd rfid-toll-collection
   ```

2. **Compile the code**
   ```bash
   # Using SDCC
   sdcc -mmcs51 main.c
   
   # Or open main.c in Keil µVision and build
   ```

3. **Flash to microcontroller**
   - Connect your 8051 programmer
   - Upload the generated HEX file
   - Reset the microcontroller

### Configuration

Edit these parameters in `main.c` to customize:

```c
int balance1 = 90;    // Initial balance for card 1 (Rs.)
int balance2 = 50;    // Initial balance for card 2 (Rs.)
int toll = 50;        // Toll amount (Rs.)

// Card IDs
char card1[] = "03300F65A292";
char card2[] = "123456789012";
```

## Usage

1. **Normal Transaction**:
   - Scan RFID card
   - System displays current balance
   - If balance ≥ toll: Gate opens automatically
   - Balance is deducted

2. **Low Balance Scenario**:
   - Buzzer sounds alert
   - LCD displays recharge options
   - Press '1' for recharge or '2' to exit

3. **Recharge Process**:
   - Enter 3-digit amount using keypad
   - Press '#' to confirm
   - Balance updated and displayed

##  System Parameters

- **Toll Amount**: Rs. 50 (configurable)
- **UART Baud Rate**: 9600 bps
- **Card Data Length**: 12 characters
- **Gate Open Duration**: 3 seconds
- **Supported Cards**: 2 (expandable)


---
## Contributor
 Soham Kale
