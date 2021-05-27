#define RS485_BAUD 9600
#define RS232_BAUD 7200

#define SLAVE_A_ADDRESS 88
#define SLAVE_B_ADDRESS 52

#define AUTOMODE_ADDRESS 42

#define SLAVE_A_DATA "Pavliyk Vitaliy Petrovych" // len = 25 + '\0'
#define SLAVE_B_DATA "20.02.2002"

#define EOT '\4'


// [byte number][distortion mask]
uint8_t slave_a_data_distortion[5][2] = {
    {0, 0},
    {24, 1 << 4},
    {0, 0},
    {0, 0},
    {6, (1 << 0 | 1 << 4 | 1 << 5)}
};

uint8_t slave_b_data_distortion[5][2] = {
    {0, 0},
    {0, 0},
    {0, 1 << 4},
    {3, (1 << 0) | (1 << 2)},
    {0, 0},
};