/* *
 * Teensy LED Strip Control Interface
 *
 * Copyright (C) 2021 Thomas G. Kenny Jr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>
#include <stdint.h>


/* *
 * Input Packet structure
 * [CMD:param_1:param_2:param_3:param_4]\r
 *
 * Parameters are optional and command dependant
 *
 * */

/* *
 * Response Packet structure
 * [CMD:param_1:param_2:param_3:param_4]\r
 *
 * Param 1 is the command status/error code
 * Parameters 2-4 are optional and command dependant
 *
 * */

#define MAX_PROTO_PACKET_LEN                256             // Max length of a packet
#define MAX_PROTO_CMD                       10              // Max length of a command
#define MAX_PROTO_PARAM_COUNT               4               // Max number of parameters for any one command packet
#define MAX_PROTO_PARAM_LEN                 50              // Max number of characters for any one parameter assuming there is only one param

#define ERR_PROTO_SUCCESS                   0               // Code for success no error.

#define ERR_PROTO_CMD_PARSING               -100            // Generic command processing error
#define ERR_PROTO_CP_MISSING_STX            -101            // Missing expected STX character
#define ERR_PROTO_CP_MISSING_ETX            -102            // Missing expected ETX character
#define ERR_PROTO_CP_MISSING_PSC            -103            // Missing expected PSC character
#define ERR_PROTO_CP_MISSING_EFC            -104            // Missing expected framing character
#define ERR_PROTO_CP_CMD_OVERFLOW           -105            // Command buffer overflow
#define ERR_PROTO_CP_CMD_NOT_IMP            -106            // Command not implemented
#define ERR_PROTO_CP_CMD_UNKNOWN            -107            // Unknown command
#define ERR_PROTO_CP_MISSING_PARAMS         -108            // Missing parameters
#define ERR_PROTO_CP_PARAM_OUT_RANGE        -109            // Parameter out of range
#define ERR_PROTO_CP_CRC16_MISMATCH         -110            // CRC16 mismatch
#define ERR_PROTO_CP_MISSING_CRC16          -111            // CRC16 missing

#define ERR_PROTO_RSP_BUILDING              -200            // Response packet error
#define ERR_PROTO_RB_TOO_MANY_PARAMS        -201            // Too many params attempted in response packet
#define ERR_PROTO_RB_PARAM_OVERFLOW         -202            // Param buffer overflow

#define ERR_ADC                              -300           // ADC Error
#define ERR_ADC_READFAIL                     -301           // Failed to read ADC
#define ERR_ADC_REGISTER_DEPTH               -302           // ADC Register Depth error. Occurs when attemtping to R/W ADC register with incorrect size value.

#define ERR_SMC                             -400            // Set Movetohall config
#define ERR_SMC_POLY_INDEX_OOR              -401            // Polynomial index out of range

#define  PROTO_STX                          '['             // Start Transmission Char
#define  PROTO_ETX                          ']'             // End Transmission Char
#define  PROTO_PSC                          ':'             // Param Separator Char
#define  PROTO_CR                           '\r'            // Cairrage Return Char
#define  PROTO_NL                           '\n'            // NewLine Char

#define DISABLE_CRC16                        1              // Disable checking for CRC16


const unsigned short CRC16_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};



typedef struct packet_struct
{
    char cmd[MAX_PROTO_CMD];                                    // Command parsed from packet buffer
    char params[MAX_PROTO_PARAM_COUNT][MAX_PROTO_PARAM_LEN];    // Array of params parsed from packet buffer
    uint8_t param_count;                                        // Number of params in proto_params
    uint16_t crc16;                                             // CRC16 calculated for protocol packet data
} proto_pkt_t;








/**
 * Calculates the CRC16 for a data buffer. Initialize a crc variable to 0
 * and save the result back and pass in for each byte in a bufffer.
 * */
uint16_t crc16(uint16_t crc, uint8_t data)
{
    return CRC16_table[((crc >> 8) ^ data) & 0xff] ^ (crc << 8);
}
/**
 * @brief crc16_buffer
 * @param crc
 * @param data
 * @param start
 * @param len
 * @return
 */
uint16_t crc16_buffer(uint16_t crc, char* data, uint16_t start, uint16_t len)
{
    for(int i=start; i<len; i++)
        crc = crc16(crc, data[i]);

    return crc;
}

/**
 * @brief crc16_buffer
 * @param crc
 * @param data
 * @param start
 * @param len
 * @return
 */
uint16_t crc16_buffer(uint16_t crc, uint8_t* data, uint16_t start, uint16_t len)
{
    return crc16_buffer(crc, (char*)data, start, len);
}

/**
 * @brief proto_clear_pkt - Clears protocol packet data structure
 * @param pkt
 */
void proto_clear_pkt(proto_pkt_t* pkt) {

    //clear cmd buffer
    memset(pkt->cmd, 0, MAX_PROTO_CMD);

    //clear param buffers
    for(int i=0; i<MAX_PROTO_PARAM_COUNT; i++) memset(pkt->params[i], 0, MAX_PROTO_PARAM_LEN);
    pkt->param_count = 0x00;
    pkt->crc16 = 0x0000;
}

/**
 * @brief proto_parse_pkt_buffer
 * @param buffer
 * @param len
 * @param pkt
 * @return
 */
int16_t proto_parse_pkt_buffer(char* buffer, uint16_t len, proto_pkt_t* pkt) {

    if(len <= 0) return 0;

    uint16_t pbi = 0, si = 0;
    pkt->crc16 = 0;

    /* Searching for STX */
    while(buffer[pbi++] != PROTO_STX && pbi < len) { si++; };

    if(pbi >= len) {
        return ERR_PROTO_CP_MISSING_STX;
    }

    int i=0;
    while(pbi < len
          && i < MAX_PROTO_CMD
          && buffer[pbi] != PROTO_PSC
          && buffer[pbi] != PROTO_ETX) {
        pkt->cmd[i++] = buffer[pbi++];
    }

    pkt->cmd[i] = 0x0;

    //check for errors
    if(pbi >= len) {
        return ERR_PROTO_CP_MISSING_EFC;
    } else if(i >= MAX_PROTO_CMD) {
        return ERR_PROTO_CP_CMD_OVERFLOW;
    } else if(buffer[pbi] == PROTO_ETX) {
        /* CMD with no Params */
    } else if(buffer[pbi] == PROTO_PSC) {

        do
        {
            pbi++;
            int pci = 0;
            while(pbi < len
                  && pci < MAX_PROTO_PARAM_LEN
                  && buffer[pbi] != PROTO_PSC
                  && buffer[pbi] != PROTO_ETX) {

                pkt->params[pkt->param_count][pci++] = buffer[pbi++];

            };

            pkt->params[pkt->param_count][pci++] = 0x00;
            pkt->param_count++;
            //print_line(PRINT_DEBUG_CHAR, pkt->params[pkt->param_count-1]);

        }while(pbi < len
               && buffer[pbi] != PROTO_ETX
               && pkt->param_count < MAX_PROTO_PARAM_COUNT);
    }

    //if on ETX char increment to next byte
    if(buffer[pbi] == PROTO_ETX) pbi++;

#ifndef DISABLE_CRC16
    // calculate buffer CRC16
    pkt->crc16 = crc16_buffer(pkt->crc16, buffer, si, pbi);

    // Read CRC16 from end of buffer
    char crc16Hex[4] = {0,0,0,0};
    for(int i=0; i<4 && pbi<len; i++) {
        crc16Hex[i] = buffer[pbi++];
    }

    // convert crc hex string to uint16
    char* endPtr = crc16Hex;
    uint16_t crc16In = (uint16_t)strtol(crc16Hex, &endPtr, 16);

    // check for strtol error
    // Added making sure calculated CRC16 is not 0 as a true 0 would cause this
    // to error out falsely
    if(crc16Hex == endPtr && crc16In == 0 && pkt->crc16 != 0) {
        return ERR_PROTO_CP_MISSING_CRC16;
    }

    // check for crc16 mismatch
    if(crc16In != pkt->crc16) {
        return ERR_PROTO_CP_CRC16_MISMATCH;
    }
#endif

    return pbi;
}

/**
 * @brief proto_append_response_pkt_param - Appends a parameter to a packet if packet params has space.
 * @param pkt_rsp - Packet to append parameter to.
 * @param param - Parameter to add.
 * @return Packet parameter count.
 */
int16_t proto_append_response_pkt_param(proto_pkt_t* pkt_rsp, const char *param) {

    if(pkt_rsp->param_count+1 > MAX_PROTO_PARAM_COUNT) return ERR_PROTO_RB_TOO_MANY_PARAMS;
    if(strlen(param) > MAX_PROTO_PARAM_LEN) return ERR_PROTO_RB_PARAM_OVERFLOW;

    strlcpy(pkt_rsp->params[pkt_rsp->param_count++], param, MAX_PROTO_PARAM_LEN);

    return pkt_rsp->param_count;
}

/**
 * @brief proto_set_response_pkt_error_code - Set the error code parameter for the given packet.
 * @param pkt_rsp - Packet to set error code on.
 * @param error_code - Error code to set.
 */
void proto_set_response_pkt_error_code(proto_pkt_t* pkt_rsp, int16_t error_code) {
    if(pkt_rsp->param_count <= 0) pkt_rsp->param_count = 1;
    sprintf(pkt_rsp->params[0], "%d", error_code);
}

/**
 * @brief proto_init_response_pkt - Initialize response packet given command packet. The default
 * error code is Success. Use proto_set_response_pkt_error_code() to set a failure error code.
 * @param pkt_rsp - Response packet.
 * @param pkt_cmd - Command packet a response is being built for.
 */
void proto_init_response_pkt(proto_pkt_t* pkt_rsp, proto_pkt_t* pkt_cmd) {
    proto_clear_pkt(pkt_rsp);
    strlcpy(pkt_rsp->cmd, pkt_cmd->cmd, MAX_PROTO_CMD);
    proto_set_response_pkt_error_code(pkt_rsp, ERR_PROTO_SUCCESS);
}

/**
 * @brief proto_print_response_pkt - Prints a packet to the serial port.
 * @param pkt_rsp - Packet to print
 */
void proto_print_response_pkt(proto_pkt_t* pkt_rsp) {

    pkt_rsp->crc16 = 0;

    Serial.write(PROTO_STX);
    pkt_rsp->crc16 = crc16(pkt_rsp->crc16, PROTO_STX);

    Serial.write(pkt_rsp->cmd, strlen(pkt_rsp->cmd));
    pkt_rsp->crc16 = crc16_buffer(pkt_rsp->crc16, pkt_rsp->cmd, 0, strlen(pkt_rsp->cmd));

    if(pkt_rsp->param_count > 0)
    {
        int i=0;
        do{
            Serial.write(PROTO_PSC);
            pkt_rsp->crc16 = crc16(pkt_rsp->crc16, PROTO_PSC);

            Serial.write(pkt_rsp->params[i], strlen(pkt_rsp->params[i]));
            pkt_rsp->crc16 = crc16_buffer(pkt_rsp->crc16, pkt_rsp->params[i], 0, strlen(pkt_rsp->params[i]));
            i++;
        } while(i<pkt_rsp->param_count);
    }
    Serial.write(PROTO_ETX);
    pkt_rsp->crc16 = crc16(pkt_rsp->crc16, PROTO_ETX);
    Serial.print(pkt_rsp->crc16, HEX);
    Serial.write(PROTO_CR);
}

#endif
