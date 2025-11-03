#include <reg51.h>
#include <string.h>

sbit RS = P1^0;
sbit RW = P1^1;
sbit EN = P1^2;
sbit buzzer = P3^7;  // Buzzer connected to P3.7

sbit IN1 = P1^3;
sbit IN2 = P1^4;

unsigned char card[13], card1[] = "03300F65A292", card2[] = "123456789012";
int balance1 = 90, balance2 = 50;
int toll = 50;

sbit ROW0 = P3^3;
sbit ROW1 = P3^4;
sbit ROW2 = P3^5;
sbit ROW3 = P3^6;
sbit COL0 = P1^5;
sbit COL1 = P1^6;
sbit COL2 = P1^7;


void delay(unsigned int time) {
    unsigned int i, j;
    for(i = 0; i < time; i++)
        for(j = 0; j < 400; j++);
}

void lcd_cmd(unsigned char cmd) {
    RS = 0;
    RW = 0;
    P2= cmd;  
    EN = 1;
    delay(1);
    EN = 0;
    delay(1);
}

void lcd_data(unsigned char dat) {
    RS = 1;
    RW = 0;
    P2 = dat;
    EN = 1;
    delay(1);
    EN = 0;
    delay(1);
}

void lcd_print(char *str) {
    while(*str)
        lcd_data(*str++);
}

void lcd_init() {
    delay(15);
    lcd_cmd(0x38);
    lcd_cmd(0x01);
    lcd_cmd(0x0c);
    lcd_cmd(0x83);
    lcd_cmd(0x06);
    delay(2);
}

void uart_init() {
    TMOD = 0x20;  // Timer1 mode 2 (8-bit auto-reload)
    SCON = 0x50;  // 8-bit UART, REN enabled
    TH1 = 0xFD;   // 9600 baud rate with 11.0592MHz crystal
    TR1 = 1;      // Start Timer1
}

void uart_tx(char ch) {
    SBUF = ch;
    while (TI == 0);  // Wait until transmission complete
    TI = 0;           // Clear transmission flag
}

void uart_send_string(char *str) {
    while (*str) {
        uart_tx(*str++);
    }
}

// Simplified read_card function to avoid optimization issues
void read_card() {
    int i;
    
    // Clear previous card data
    for(i = 0; i < 13; i++) {
        card[i] = 0;
    }
    
    // Read 12 characters of card data
    for(i = 0; i < 12; i++) {
        // Wait for data to be received
        while(RI == 0);
        
        // Read the data into card array
        card[i] = SBUF;
        
        // Clear the receive flag
        RI = 0;
    }
    
    // Null-terminate the string
    card[12] = '\0';
}

void open_gate() {
    lcd_cmd(0x01);
    lcd_print("Gate Opening...");

    IN1 = 1;  // Motor forward
    IN2 = 0;
    delay(1000);
    
    IN1 = 0;  // Motor stop
    IN2 = 0;
    delay(3000);
    
    IN1 = 0;  // Motor reverse
    IN2 = 1;
    delay(1000);
    
    IN1 = 0;  // Motor stop
    IN2 = 0;

    lcd_cmd(0x01);
    lcd_print("Gate Closed");
}

// Completely revised buzzer function to ensure proper on/off behavior
void sound_buzzer(unsigned int duration) {
    // Turn buzzer ON - set to 1 for active-high or 0 for active-low
    buzzer = 0;  // Assuming active-high based on your connection description
    
    // Wait for the specified duration
    delay(duration);
    
    // Explicitly turn buzzer OFF - opposite of the ON state above
    buzzer = 1;  // For active-high buzzer
    
    // Ensure buzzer is completely off with a small additional delay
    delay(5);
}

char keypad_scan() {
    char key = '\0';
    char keymap[4][3] = {
        {'1','2','3'},
        {'4','5','6'},
        {'7','8','9'},
        {'*','0','#'}
    };

    int row, col;

    for(row = 0; row < 4; row++) {
        // Set all rows high
        ROW0 = ROW1 = ROW2 = ROW3 = 1;

        // Set current row low
        switch(row) {
            case 0: ROW0 = 0; break;
            case 1: ROW1 = 0; break;
            case 2: ROW2 = 0; break;
            case 3: ROW3 = 0; break;
        }

        // Check all columns
        if(COL0 == 0) { while(COL0 == 0); key = keymap[row][0]; break; }
        if(COL1 == 0) { while(COL1 == 0); key = keymap[row][1]; break; }
        if(COL2 == 0) { while(COL2 == 0); key = keymap[row][2]; break; }
    }

    return key;
}

void display_balance(int balance) {
    lcd_cmd(0x01);
    lcd_print("Balance: Rs.");
    
    if(balance >= 100) {
        lcd_data((balance / 100) + '0');
        lcd_data(((balance / 10) % 10) + '0');
        lcd_data((balance % 10) + '0');
    } else if(balance >= 10) {
        lcd_data((balance / 10) + '0');
        lcd_data((balance % 10) + '0');
    } else {
        lcd_data(balance + '0');
    }
}

// New function to handle recharge
int recharge_card() {
    int amount = 0;
    char key;
    
    lcd_cmd(0x01);
    lcd_print("Recharge Amount:");
    lcd_cmd(0xC0);  // Move to second line
    
    // Get first digit
    while(1) {
        key = keypad_scan();
        if(key >= '0' && key <= '9') {
            lcd_data(key);
            amount = (key - '0') * 100;  // Hundreds place
            break;
        }
    }
    
    // Get second digit
    while(1) {
        key = keypad_scan();
        if(key >= '0' && key <= '9') {
            lcd_data(key);
            amount += (key - '0') * 10;  // Tens place
            break;
        }
    }
    
    // Get third digit
    while(1) {
        key = keypad_scan();
        if(key >= '0' && key <= '9') {
            lcd_data(key);
            amount += (key - '0');       // Ones place
            break;
        }
    }
    
    // Confirm recharge with # key
    lcd_cmd(0x01);
    lcd_print("Confirm: Rs.");
    if(amount >= 100) {
        lcd_data((amount / 100) + '0');
        lcd_data(((amount / 10) % 10) + '0');
        lcd_data((amount % 10) + '0');
    } else if(amount >= 10) {
        lcd_data((amount / 10) + '0');
        lcd_data((amount % 10) + '0');
    } else {
        lcd_data(amount + '0');
    }
    
    lcd_cmd(0xC0);  // Move to second line
    lcd_print("# to confirm");
    
    while(1) {
        key = keypad_scan();
        if(key == '#') {
            lcd_cmd(0x01);
            lcd_print("Recharge Success!");
            sound_buzzer(200);  // Success beep
            delay(2000);
            return amount;
        } else if(key == '*') {
            lcd_cmd(0x01);
            lcd_print("Recharge Canceled");
            delay(2000);
            return 0;
        }
    }
}

void handle_low_balance(int* balance) {
    char key;
    
    lcd_cmd(0x01);
    lcd_print("Low Balance!");
    sound_buzzer(500);  // Alert sound
    
    lcd_cmd(0xC0);  // Move to second line
    lcd_print("1-Recharge 2-Exit");
    
    while(1) {
        key = keypad_scan();
        if(key == '1') {
            int recharge_amount = recharge_card();
            *balance += recharge_amount;
            
            if(*balance >= toll) {
                lcd_cmd(0x01);
                lcd_print("New Balance:");
                lcd_cmd(0xC0);
                display_balance(*balance);
                delay(2000);
                
                // Deduct toll and open gate
                *balance -= toll;
                open_gate();
            } else {
                lcd_cmd(0x01);
                lcd_print("Still Low Balance");
                lcd_cmd(0xC0);
                lcd_print("Try Again Later");
                sound_buzzer(300);  // Alert sound
                delay(2000);
            }
            break;
        } else if(key == '2') {
            lcd_cmd(0x01);
            lcd_print("Transaction");
            lcd_cmd(0xC0);
            lcd_print("Canceled");
            delay(2000);
            break;
        }
    }
}

void main() {
    // Initialize LCD and UART
    lcd_init();
    uart_init();
    
    // Initialize buzzer as output and ensure it's off at startup
    buzzer = 1;  // Assuming active-high configuration
    
    // Send startup message via UART
    uart_send_string("RFID Toll System Started\r\n");
    
    lcd_cmd(0x01);
    lcd_print("RFID Toll System");
    delay(2000);
    
    while(1) {
        lcd_cmd(0x01);
        lcd_print("Scan RFID Card");
        uart_send_string("Waiting for card...\r\n");
        
        read_card();
        uart_send_string("Card Read: ");
        uart_send_string(card);
        uart_send_string("\r\n");
        
        lcd_cmd(0x01);
        lcd_print("RFID Scanned");
        delay(1000);
        
        if(strcmp(card, card1) == 0) {
            if(balance1 >= toll) {
                balance1 -= toll;
                display_balance(balance1);
                delay(2000);
                open_gate();
            } else {
                handle_low_balance(&balance1);
            }
        }
        else if(strcmp(card, card2) == 0) {
            if(balance2 >= toll) {
                balance2 -= toll;
                display_balance(balance2);
                delay(2000);
                open_gate();
            } else {
                handle_low_balance(&balance2);
            }
        }
        else {
            lcd_cmd(0x01);
            lcd_print("Invalid Card");
            sound_buzzer(500);  // Alert sound for 0.5 seconds
            delay(2000);
        }
        
        lcd_cmd(0x01);
        lcd_print("Press any key");
        while(keypad_scan() == '\0');
    }
}