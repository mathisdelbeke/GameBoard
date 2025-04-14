#include "uart.h"

#define F_CPU 16000000UL                            
#define TX_BUFFER_SIZE 64
#define RX_BUFFER_SIZE 64

volatile char tx_buffer[TX_BUFFER_SIZE];
volatile uint8_t tx_head = 0;
volatile uint8_t tx_tail = 0;

volatile char rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_head = 0;
volatile uint8_t rx_tail = 0;

void uart_init(uint32_t baud) {
    uint16_t ubbr_val = (F_CPU / (16UL * baud)) - 1;

    UBRR0H = (ubbr_val >> 8);               // Upper 8 bits
    UBRR0L = ubbr_val;                      // Lower 8 bits
    UCSR0B = (1 << TXEN0) | (1 << UDRIE0);  // Enable TX and TX Empty Interrupt
    UCSR0B |= (1 << RXEN0) | (1 << RXCIE0); // Enable RX and RX Complete Interrupt
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit, 1 stop, no parity

    sei();
}

void uart_transmit(char c) {
    uint8_t next_head = (tx_head + 1) % TX_BUFFER_SIZE;
    while (next_head == tx_tail);

    tx_buffer[tx_head] = c;
    tx_head = next_head;

    UCSR0B |= (1 << UDRIE0);    // Enable TX Empty Interrupt
}

void uart_print_string(const char *str) {          
    while (*str) {                         
        uart_transmit(*str++);    
    }
}

void uart_print_signed(int32_t num) {
    char buffer[12];                   // Max chars needed
    itoa(num, buffer, 10);             // Convert signed number into chars
    uart_print_string(buffer);
}

void uart_print_unsigned(uint32_t num) {
    char buffer[12];                   // Max chars needed
    utoa(num, buffer, 10);             // Convert unsigned number into chars
    uart_print_string(buffer);
}

ISR(USART_UDRE_vect) {
    if (tx_head != tx_tail) { 
        UDR0 = tx_buffer[tx_tail];
        tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
    } else {
        UCSR0B &= ~(1 << UDRIE0); 
    }
}

char uart_read() {
    while (rx_head == rx_tail); // Wait if buffer is empty

    char c = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
    return c;
}

uint8_t uart_available() {
    return (rx_head != rx_tail);
}

ISR(USART_RX_vect) {   // Triggered when data received
    uint8_t next_head = (rx_head + 1) % RX_BUFFER_SIZE;
    
    // UDR0 holds the received byte
    if (next_head != rx_tail) {
        rx_buffer[rx_head] = UDR0;
        rx_head = next_head;
    }
}