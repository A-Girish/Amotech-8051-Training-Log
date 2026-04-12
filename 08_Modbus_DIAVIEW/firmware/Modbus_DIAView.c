#include <reg51.h>

#define SLAVE_ID 1

#define MAX_COILS     1
#define MAX_DISCRETE  1
#define MAX_INPUT     1
#define MAX_HOLDING   2

unsigned char coils[1] = {1};
unsigned char discrete_inputs[1] = {1};
unsigned int input_reg[1]   = {111};
unsigned int holding_reg[2] = {123,456};


/* -------- UART -------- */
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

/* -------- CRC -------- */
unsigned int modbus_crc(unsigned char *buf, unsigned char len)
{
    unsigned int crc = 0xFFFF;
    unsigned char i, j;

    for(i=0;i<len;i++)
    {
        crc ^= buf[i];
        for(j=0;j<8;j++)
        {
            if(crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

/* -------- MAIN -------- */
void main()
{
    unsigned char req[8];
    unsigned char resp[32];
    unsigned int crc_rx, crc_calc;
    unsigned int addr, qty, value;
    unsigned char i, resp_len, byte_cnt;

    uart_init();

    while(1)
    {
        for(i=0;i<8;i++)
            req[i] = uart_rx();

        if(req[0] != SLAVE_ID)
            continue;

        crc_rx = (req[7] << 8) | req[6];
        crc_calc = modbus_crc(req, 6);
        if(crc_rx != crc_calc)
            continue;

        addr = (req[2] << 8) | req[3];
        qty  = (req[4] << 8) | req[5];

        /* -------- FC01: READ COILS -------- */
        if(req[1] == 0x01)
        {
            byte_cnt = (qty + 7)/8;

            resp[0] = SLAVE_ID;
            resp[1] = 0x01;
            resp[2] = byte_cnt;

            for(i=0;i<byte_cnt;i++)
                resp[3+i] = 0;

            for(i=0;i<qty;i++)
            {
                if((addr+i) < MAX_COILS)
                {
                    if(coils[addr+i])
                        resp[3 + (i/8)] |= (1 << (i%8));
                }
            }

            resp_len = 3 + byte_cnt;
        }

        /* -------- FC02: READ DISCRETE INPUTS -------- */
        else if(req[1] == 0x02)
        {
            byte_cnt = (qty + 7)/8;

            resp[0] = SLAVE_ID;
            resp[1] = 0x02;
            resp[2] = byte_cnt;

            for(i=0;i<byte_cnt;i++)
                resp[3+i] = 0;

            for(i=0;i<qty;i++)
            {
                if((addr+i) < MAX_DISCRETE)
                {
                    if(discrete_inputs[addr+i])
                        resp[3 + (i/8)] |= (1 << (i%8));
                }
            }

            resp_len = 3 + byte_cnt;
        }

        /* -------- FC03: READ HOLDING REGISTERS -------- */
        else if(req[1] == 0x03)
        {
            if(qty > MAX_HOLDING) qty = MAX_HOLDING;

            resp[0] = SLAVE_ID;
            resp[1] = 0x03;
            resp[2] = qty * 2;

            for(i=0;i<qty;i++)
            {
                if((addr+i) < MAX_HOLDING)
                    value = holding_reg[addr+i];
                else
                    value = 0;

                resp[3+2*i] = value >> 8;
                resp[4+2*i] = value & 0xFF;
            }

            resp_len = 3 + qty*2;
        }

        /* -------- FC04: READ INPUT REGISTERS -------- */
        else if(req[1] == 0x04)
        {
            if(qty > MAX_INPUT) qty = MAX_INPUT;

            resp[0] = SLAVE_ID;
            resp[1] = 0x04;
            resp[2] = qty * 2;

            for(i=0;i<qty;i++)
            {
                if((addr+i) < MAX_INPUT)
                    value = input_reg[addr+i];
                else
                    value = 0;

                resp[3+2*i] = value >> 8;
                resp[4+2*i] = value & 0xFF;
            }

            resp_len = 3 + qty*2;
        }

        /* -------- FC05: WRITE SINGLE COIL -------- */
        else if(req[1] == 0x05)
        {
            if(addr < MAX_COILS)
            {
                if(req[4] == 0xFF)
                    coils[addr] = 1;
                else
                    coils[addr] = 0;
            }

            for(i=0;i<6;i++)
                resp[i] = req[i];

            resp_len = 6;
        }

        /* -------- FC06: WRITE SINGLE REGISTER -------- */
        else if(req[1] == 0x06)
        {
            value = (req[4] << 8) | req[5];

            if(addr < MAX_HOLDING)
                holding_reg[addr] = value;

            for(i=0;i<6;i++)
                resp[i] = req[i];

            resp_len = 6;
        }

        else continue;

        /* -------- CRC + SEND -------- */
        crc_calc = modbus_crc(resp, resp_len);

        resp[resp_len]     = crc_calc & 0xFF;
        resp[resp_len + 1] = crc_calc >> 8;

        resp_len += 2;

        for(i=0;i<resp_len;i++)
            uart_tx(resp[i]);
    }
}