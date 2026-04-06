#include <reg51.h>

#define TOTAL_REGS 10

/* ---- Dummy memory ---- */
unsigned int holding_reg[10] = {1234,4567,6789,9876,6543,4321,3210,2221,2580,9156};
unsigned int input_reg[10]   = {1234,4567,6789,9876,6543,4321,3210,2221,2580,9156};
unsigned char output_coils[2] = {0x15, 0x03};
unsigned char digital_input[2] = {0x15, 0x03};

unsigned char resp[25];

/* ---- UART ---- */
void uart_init() {
    TMOD = 0x20;
    TH1  = 0xFD;
    SCON = 0x50;
    TR1  = 1;
}

void uart_tx(unsigned char c) {
    SBUF = c;
    while(!TI);
    TI = 0;
}

unsigned char uart_rx() {
    while(!RI);
    RI = 0;
    return SBUF;
}

unsigned int modbus_crc(unsigned char *buf, unsigned char len) {
    unsigned int crc = 0xFFFF;
    unsigned char i, j;

    for(i = 0; i < len; i++) {
        crc ^= buf[i];
        for(j = 0; j < 8; j++) {
            if(crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

/* ---- FC01 ---- */
void handle_fc01(unsigned char *req) 
{
    unsigned int crc;
    unsigned int addr;
    unsigned int num_of_regs;
    unsigned int byte_count;
    int i;
    
    for(i = 0; i < 25; i++)
       resp[i] = 0;

    addr = (req[2] << 8) | req[3];
    num_of_regs = (req[4] << 8) | req[5];

    byte_count = (num_of_regs + 7) / 8;

    resp[0] = 1;
    resp[1] = 1;
    resp[2] = byte_count;

    if((addr + num_of_regs) <= TOTAL_REGS) {
        for(i = 0; i < num_of_regs; i++) {
            if(output_coils[(addr + i)/8] & (1 << ((addr + i) % 8)))
            {
               resp[3 + (i/8)] |= (1 << (i % 8));
            }
        }

        crc = modbus_crc(resp, 3 + byte_count);

        resp[3 + byte_count] = crc & 0xFF;
        resp[4 + byte_count] = (crc >> 8) & 0xFF;

        for(i = 0; i < (5 + byte_count); i++) {
            uart_tx(resp[i]);
        }
    }
}

/* ---- FC02 ---- */
void handle_fc02(unsigned char *req) 
{
    unsigned int crc;
    unsigned int addr;
    unsigned int num_of_regs;
    unsigned int byte_count;
    int i;
    
    for(i = 0; i < 25; i++)
       resp[i] = 0;

    addr = (req[2] << 8) | req[3];
    num_of_regs = (req[4] << 8) | req[5];

    byte_count = (num_of_regs + 7) / 8;

    resp[0] = 1;
    resp[1] = 2;
    resp[2] = byte_count;

    if((addr + num_of_regs) <= TOTAL_REGS) {
        for(i = 0; i < num_of_regs; i++) {
            if(digital_input[(addr + i)/8] & (1 << ((addr + i) % 8)))
            {
               resp[3 + (i/8)] |= (1 << (i % 8));
            }
        }

        crc = modbus_crc(resp, 3 + byte_count);

        resp[3 + byte_count] = crc & 0xFF;
        resp[4 + byte_count] = (crc >> 8) & 0xFF;

        for(i = 0; i < (5 + byte_count); i++) {
            uart_tx(resp[i]);
        }
    }
}

/* ---- FC03 ---- */
void handle_fc03(unsigned char *req)
{
    unsigned int crc;
    unsigned int addr;
    unsigned int value;
    unsigned int quantity;
    int i;
    
    for(i = 0; i < 25; i++)
       resp[i] = 0;

    addr = (req[2] << 8) | req[3];
    quantity = (req[4] << 8) | req[5];

    resp[0] = 1;
    resp[1] = 3;
    resp[2] = quantity * 2;

    if((addr + quantity) <= TOTAL_REGS) {
        for(i = 0; i < quantity; i++) {
            value = holding_reg[addr + i];

            resp[3 + 2*i] = value >> 8;
            resp[4 + 2*i] = value & 0xFF;
        }

        crc = modbus_crc(resp, 3 + (2 * quantity));

        resp[3 + (2 * quantity)] = crc & 0xFF;
        resp[4 + (2 * quantity)] = (crc >> 8) & 0xFF;

        for(i = 0; i < (5 + (2 * quantity)); i++) {
            uart_tx(resp[i]);
        }
    }
}

/* ---- FC04 ---- */
void handle_fc04(unsigned char *req)
{
    unsigned int crc;
    unsigned int addr;
    unsigned int value;
    unsigned int quantity;
    int i;

    for(i = 0; i < 25; i++)
       resp[i] = 0;

    addr = (req[2] << 8) | req[3];
    quantity = (req[4] << 8) | req[5];

    resp[0] = 1;
    resp[1] = 4;
    resp[2] = quantity * 2;

    if((addr + quantity) <= TOTAL_REGS) {
        for(i = 0; i < quantity; i++) {
            value = input_reg[addr + i];

            resp[3 + 2*i] = value >> 8;
            resp[4 + 2*i] = value & 0xFF;
        }

        crc = modbus_crc(resp, 3 + (2 * quantity));

        resp[3 + (2 * quantity)] = crc & 0xFF;
        resp[4 + (2 * quantity)] = (crc >> 8) & 0xFF;

        for(i = 0; i < (5 + (2 * quantity)); i++) {
            uart_tx(resp[i]);
        }
    }
}

/* ---- FC05 ---- */
void handle_fc05(unsigned char *req) 
{
    unsigned int addr;
    unsigned int value;
    unsigned int crc;
    unsigned char resp[8];
    int i;
  
    addr = (req[2] << 8) | req[3];
    value = (req[4] << 8) | req[5];
  
    if(addr < TOTAL_REGS) {
        output_coils[addr] = value;
    }
    
    for(i = 0; i < 6; i++) {
        resp[i] = req[i];
    }

    crc = modbus_crc(resp, 6);

    resp[6] = crc & 0xFF;
    resp[7] = crc >> 8;

    for(i = 0; i < 8; i++) 
    {
        uart_tx(resp[i]);
    }
    
}  

/* ---- FC06 ---- */
void handle_fc06(unsigned char *req) 
{
    unsigned int addr;
    unsigned int value;
    unsigned int crc;
    unsigned char resp[8];
    int i;

    addr = (req[2] << 8) | req[3];
    value = (req[4] << 8) | req[5];

    if(addr < TOTAL_REGS) {
        holding_reg[addr] = value;
    }

    for(i = 0; i < 6; i++) {
        resp[i] = req[i];
    }

    crc = modbus_crc(resp, 6);

    resp[6] = crc & 0xFF;
    resp[7] = crc >> 8;

    for(i = 0; i < 8; i++) 
    {
        uart_tx(resp[i]);
    }
}

/* ---- MAIN ---- */
void main() {
    unsigned char req[8];
    int i;
    //unsigned int crc_calc, crc_recv;

    uart_init();

    while(1) {
        req[0] = uart_rx();

        for(i = 1; i < 8; i++) {
            req[i] = uart_rx();
        }

        if(req[0] != 1)
            continue;

        /*crc_calc = modbus_crc(req, 6);
        crc_recv = req[6] | (req[7] << 8);

        if(crc_calc != crc_recv)
            continue; */

        if(req[1] == 0x01) {
            handle_fc01(req);
        }
        else if(req[1] == 0x02) {
            handle_fc02(req);
        }
        else if(req[1] == 0x03) {
            handle_fc03(req);
        }
        else if(req[1] == 0x04) {
            handle_fc04(req);
        }
        else if(req[1] == 0x06) {
            handle_fc06(req);
        }
    }
}