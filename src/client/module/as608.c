#include "as608.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <wiringPi.h>
#include <wiringSerial.h>


/*******************************BEGIN**********************************
 * Global variables (defined)
 */
AS608 g_as608;
int   g_fd;          // Global variable, file descriptor, i.e. the return value of the open() function to open the serial port
int   g_verbose;     // Global variable that outputs the level of detail of the information
char  g_error_desc[128]; // Global variables, meaning of error codes
uchar g_error_code;      // Global variable, confirmation code returned by the module, read this variable if the function does not return true
uchar g_order[64] = { 0 }; // Command packets sent to the module
uchar g_reply[64] = { 0 }; // Answer Packets for Modules 

/*
 **********************************END********************************/



/******************************************************************************
 *
 * The first part: auxiliary function area
 * The functions in this section are only valid in the scope of this file and are not declared in as608.h !!!
 *
 ******************************************************************************/

/*
 * Helper functions
 * Split an unsigned integer into multiple single-byte integers
 * such as num=0xa0b1c2d3, split into 0x0a, 0x1b, 0xc2, 0xd3
 */
void Split(uint num, uchar* buf, int count) {
    for (int i = 0; i < count; ++i) {
        *buf++ = (num & 0xff << 8*(count-i-1)) >> 8*(count-i-1);
    }
}

/*
 * Helper functions
 * Merge multiple single-byte integers (up to 4) into one unsigned int integer
 * such as 0xa0, 0xb1, 0xc2, 0xd3, merged into 0xa0b1c2d3
 * Parameters: num(pointer, output conversion result)
 * startAddr(pointer, pointer to an element of the array, you can use (array name + offset) when passing the parameter)
 * count(from startAddr, calculate the number of count into one number, assign to num)
 */
bool Merge(uint* num, const uchar* startAddr, int count) {
    *num = 0;
    for (int i = 0; i < count; ++i)
        *num += (int)(startAddr[i]) << (8*(count-i-1)); 

    return true;
}

/* 
 * Helper function for debug
 * Print hexadecimal data
 */
void PrintBuf(const uchar* buf, int size) {
    for (int i = 0; i < size; ++i) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

/*
 * Auxiliary functions
 * Calculate the sum of checks (from the 7th byte of buf to the third byte of the penultimate number, calculate the sum of the values of each byte)
 **/
int Calibrate(const uchar* buf, int size) {
    int count = 0;
    for (int i = 6; i < size - 2; ++i) {
        count += buf[i];
    }

    return count;
}

/*
 * Auxiliary functions
 * Determine the acknowledgement code && calculate the checksum
 * parameters: buf, the reply packet data of the module
 * size, the number of valid bytes of the reply packet
 */
bool Check(const uchar* buf, int size) {
    int count_ = 0;       // Module incoming check calibration and 
    Merge(&count_, buf+size-2, 2); 

    // Self-calculated check sums
    int count = Calibrate(buf, size);

    return (buf[9] == 0x00 && 
            count_ == count && 
            buf[0] != 0x00);   //  Prevent all as 0x00
}

/* 
 *  Helper functions
 * Send command packet
 * parameter, size (the actual number of valid characters to be sent, not including the ending '\0'.  !!!)
 */
int SendOrder(const uchar* order, int size) {
    // Output details
    if (g_verbose == 1) {
        printf("sent: ");
        PrintBuf(order, size);
    }
    int ret = write(g_fd, order, size);
    return ret;
}

/* 
 *  Helper functions
 * Receive answer packet
 * Parameters: size (actual data to be received, including command header, packet length, data area, check sum, etc.)
 */
bool RecvReply(uchar* hex, int size) {
    int availCount = 0;
    int timeCount  = 0;

    while (true) {
        if (serialDataAvail(g_fd)) {
            hex[availCount] = serialGetchar(g_fd);
            availCount++;
            if (availCount >= size) {
                break;
            }
        }
        usleep(10); // Wait 10 microseconds
        timeCount++;
        if (timeCount > 300000) {   // Maximum blockage of 3 seconds
            break;
        }
    }

    // Output details
    if (g_verbose == 1) {
        printf("recv: ");
        PrintBuf(hex, availCount);
    }

    // If the specified size of data is not received within the maximum blocking time, false is returned.
    if (availCount < size) {
        g_error_code = 0xff;
        return false;
    }

    g_error_code = hex[9];
    return true;
}

/*
 * Show progress bar
 * Parameters: done(amount completed) all(total)
 */
void PrintProcess(int done, int all) {
    // Progress bar, 0~100%, 50 characters
    char process[64] = { 0 };
    double stepSize = (double)all / 100;
    int doneStep = done / stepSize;

    // If the number of steps is even, update the progress bar
    // because 100 progress values are displayed in 50 characters
    for (int i = 0; i < doneStep / 2 - 1; ++i)
        process[i] = '=';
    process[doneStep/2 - 1] = '>';

    printf("\rProcess:[%-50s]%d%% ", process, doneStep);
    if (done < all)
        fflush(stdout);
    else
        printf("\n");
}

/* 
 *  Helper functions
 * Receive packet Acknowledgement code 0x02 means the packet and there is a follow-up packet, 0x08 means the last packet
 * Parameters.
 * validDataSize indicates the valid data size, excluding the data header, check and part
 */
bool RecvPacket(uchar* pData, int validDataSize) {
    if (g_as608.packet_size <= 0)
        return false;
    int realPacketSize = 11 + g_as608.packet_size; // Actual per packet size
    int realDataSize = validDataSize * realPacketSize / g_as608.packet_size;  // Total size of data to be accepted

    uchar readBufTmp[8] = { 0 };  // Read up to 8 bytes at a time and append them to the readBuf
    uchar* readBuf = (uchar*)malloc(realPacketSize); // Receive full realPacketSize bytes, indicating that a complete packet has been received and appended to pData

    int availSize      = 0;
    int readSize       = 0;
    int readCount      = 0;
    int readBufTmpSize = 0;
    int readBufSize    = 0;
    int offset         = 0;
    int timeCount      = 0;

    while (true) {
        if ((availSize = serialDataAvail(g_fd)) > 0) {
            timeCount = 0;
            if (availSize > 8) {
                availSize = 8;
            }
            if (readBufSize + availSize > realPacketSize) {
                availSize = realPacketSize - readBufSize;
            }

            memset(readBufTmp, 0, 8);
            readSize = read(g_fd, readBufTmp, availSize);
            memcpy(readBuf+readBufSize, readBufTmp, readSize);

            readBufSize += readSize;
            readCount   += readSize;

            // Whether to output detailed information
            if (g_verbose == 1) {
                printf("%2d%% RecvData: %d  count=%4d/%-4d  ", 
                        (int)((double)readCount/realDataSize*100), readSize, readCount, realDataSize);
                PrintBuf(readBufTmp, readSize);
            }
            else if (g_verbose == 0){ // Show progress bar by default
                PrintProcess(readCount, realDataSize);
            }
            else {
                // show nothing
            }

            // After receiving a complete packet (139 bytes)
            if (readBufSize >= realPacketSize) {
                int count_ = 0;
                Merge(&count_, readBuf+realPacketSize-2, 2);
                if (Calibrate(readBuf, realPacketSize) != count_) {
                    free(readBuf);
                    g_error_code = 0x01;
                    return false;
                }

                memcpy(pData+offset, readBuf+9, g_as608.packet_size);
                offset += g_as608.packet_size;
                readBufSize = 0;

                //Received End of Package
                if (readBuf[6] == 0x08) {
                    break;
                }
            }

            // validDataSize bytes of valid data were received, but the end packet was not received.
            if (readCount >= realDataSize) {
                free(readBuf);
                g_error_code = 0xC4;
                return false;
            }
        } // end outer if

        usleep(10); // Wait 10 microseconds
        timeCount++;
        if (timeCount > 300000) {   // Maximum blockage of 3 seconds
            break;
        }
    } // end while

    free(readBuf);

    // If the specified size of data is not received within the maximum blocking time, false is returned.
    if (readCount < realDataSize) {
        g_error_code = 0xC3;
        return false;
    }

    g_error_code = 0x00;
    return true; 
}


/* 
 *  Helper functions
 * Send packet Acknowledgement code 0x02 means packet and there is a follow-up packet, 0x08 means the last packet
 * Parameters.
 * validDataSize means the valid data size, excluding the data header, check calibration and part
 */
bool SendPacket(uchar* pData, int validDataSize) {
    if (g_as608.packet_size <= 0)
        return false;
    if (validDataSize % g_as608.packet_size != 0) {
        g_error_code = 0xC8;
        return false;
    }
    int realPacketSize = 11 + g_as608.packet_size; // Actual per packet size
    int realDataSize = validDataSize * realPacketSize / g_as608.packet_size;  // 总共需要发送的数据大小

    // Constructing packets
    uchar* writeBuf = (uchar*)malloc(realPacketSize);
    writeBuf[0] = 0xef; // packet header
    writeBuf[1] = 0x01; // packet header
    Split(g_as608.chip_addr, writeBuf+2, 4); // chip address
    Split(g_as608.packet_size+2, writeBuf+7, 2); // packet length

    int offset = 0; // valid data sent
    int writeCount = 0; // the actual data that has been sent

    while (true) {
        // fill the data area
        memcpy(writeBuf+9, pData+offset, g_as608.packet_size);

        // packet flag
        if (offset + g_as608.packet_size < validDataSize)
            writeBuf[6] = 0x02; // end packet (last packet)
        else
            writeBuf[6] = 0x08;  // General data package

        // Checking and proofreading
        Split(Calibrate(writeBuf, realPacketSize), writeBuf+realPacketSize-2, 2); 

        // Sending data packets
        write(g_fd, writeBuf, realPacketSize);

        offset     += g_as608.packet_size;
        writeCount += realPacketSize;

        // Whether to output details
        if (g_verbose == 1) {
            printf("%2d%% SentData: %d  count=%4d/%-4d  ", 
                    (int)((double)writeCount/realDataSize*100), realPacketSize, writeCount, realDataSize);
            PrintBuf(writeBuf, realPacketSize);
        }
        else if (g_verbose == 0) {
            // Show progress bar
            PrintProcess(writeCount, realDataSize);
        }
        else {
            // show nothing
        }

        if (offset >= validDataSize)
            break;
    } // end while

    free(writeBuf);
    g_error_code = 0x00;
    return true; 
}
/*
 * Helper functions
 * Constructs the instruction packet and assigns the result to the global variable g_order
 * Parameters.
 * orderCode, instruction code, e.g. 0x01, 0x02, 0x1a...
 * fmt, parameter description, e.g. an instruction packet with 2 parameters, one of type uchar, which takes up 1 byte, and another of type uchar, which takes up 2 bytes
 * Then fmt should be "%1d%2d".
 * If the argument is of type uchar, which takes up 1 byte, and type uchar*, which takes up 32 bytes
 * then fmt should be "%d%32s"
 * (the number represents the number of bytes the argument takes up, ignored if it is 1, the letter represents the type, only %d, %u and %s are supported)
 */
int GenOrder(uchar orderCode, const char* fmt, ...) {
    g_order[0] = 0xef;        // Baotou, 0xef
    g_order[1] = 0x01;
    Split(g_as608.chip_addr, g_order+2, 4);    // Chip address, needs to be set using PS_Setup() initialization
    g_order[6] = 0x01; // packet identifier, 0x01 means it is a command packet, 0x02 data packet, 0x08 end packet (last data packet)
    g_order[9] = orderCode; // instruction

    // Calculation Total number of parameters
    int count = 0;
    for (const char* p = fmt; *p; ++p) {
        if (*p == '%')
            count++;
    }

    // fmt==""
    if (count == 0) { 
        Split(0x03, g_order+7,  2);  // Packet length
        Split(Calibrate(g_order, 0x0c), g_order+10, 2); // check sum (if no arguments, instruction packet length is 12, i.e. 0x0c)
        return 0x0c;
    }
    else {
        va_list ap;
        va_start(ap, fmt);

        uint  uintVal;
        uchar ucharVal;
        uchar* strVal;

        int offset = 10;  // g_order pointer offset
        int width = 1; // width of modifier in fmt, e.g. %4d, %32s

        // Handling of indeterminate parameters
        for (; *fmt; ++fmt) {
            width = 1;
            if (*fmt == '%') {
                const char* tmp = fmt+1;

                // Get width, e.g. %4u, %32s
                if (*tmp >= '0' && *tmp <= '9') {
                    width = 0;
                    do {
                        width = (*tmp - '0') + width * 10;
                        tmp++;
                    } while(*tmp >= '0' && *tmp <= '9');
                }

                switch (*tmp) {
                    case 'u':
                    case 'd':
                        if (width > 4)
                            return 0;
                        uintVal = va_arg(ap, int);
                        Split(uintVal, g_order+offset, width);
                        break;
                    case 'c': // Equivalent to "%d"
                        if (width > 1)
                            return 0;
                        ucharVal = va_arg(ap, int);
                        g_order[offset] = ucharVal;
                        break;
                    case 's':
                        strVal = va_arg(ap, char*);
                        memcpy(g_order+offset, strVal, width);
                        break;
                    default:
                        return 0;
                } // end switch 

                offset += width;
            } // end if (*p == '%')
        } // end for 

        Split(offset+2-9, g_order+7, 2);  // Package length
        Split(Calibrate(g_order, offset+2), g_order+offset, 2); // Checksum

        va_end(ap);
        return offset + 2;
    } // end else (count != 0)
}

/***************************************************************************
 *
 * Part 2.
 * This section is the functions declared in as608.h
 *
 ***************************************************************************/

/*
 * Initialize the configuration
 * Set the chip address, default is 0xffffffff, and the communication password, default is 0x00000000, which means no password
 * Does not change the chip address
 */
bool PS_Setup(uint chipAddr, uint password) {
    g_as608.chip_addr = chipAddr;
    g_as608.password  = password;

    if (g_verbose == 1)
        printf("-------------------------Initializing-------------------------\n");
    //Verify password
    if (g_as608.has_password) {
        if (!PS_VfyPwd(password))
            return false;
    }

    // Get packet size, baud rate, etc.
    if (PS_ReadSysPara() && g_as608.packet_size > 0) {
        if (g_verbose == 1)
            printf("-----------------------------Done-----------------------------\n");
        return true;
    }

    if (g_verbose == 1)
        printf("-----------------------------Done-----------------------------\n");
    g_error_code = 0xC7;
    return false;
}

/*
 * Function name: PS_GetImage
 * Function: detect finger, record fingerprint image in ImgageBuffer after detection. return confirmation code: successful, no finger, etc.
 * Input parameter: none
 * Return value: true (success), false (error), the confirmation code is assigned to g_error_code
 * Confirmation code = 00H indicates successful entry.
 * Acknowledgement code = 01H indicates an error in packet receipt.
 * Acknowledgement code = 02H indicates no finger on the sensor.
 * Acknowledgement code = 03H indicates an unsuccessful entry.
 */
bool PS_GetImage() {
    int size = GenOrder(0x01, "");
    // Sending command packets
    SendOrder(g_order, size);

    // Receive the answer packet, check the acknowledgement code and check sum
    return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}


/*
 * Function name: PS_GenChar
 * Description: Generate a fingerprint feature file from the original image in ImageBuffer to CharBuffer1 or CharBuffer2
 * Parameters: bufferID Feature buffer number
 * Return value: true (success), false (error), acknowledgement code is assigned to g_error_code
 * Acknowledgement code = 00H means the feature was generated successfully.
 * Confirmation code = 01H indicates an error in receiving the packet.
 * Confirmation code = 06H means the fingerprint image is too messy to generate a feature.
 */
bool PS_GenChar(uchar bufferID) {
    int size = GenOrder(0x02, "%d", bufferID);
    SendOrder(g_order, size);

    // Receive the answer packet, check the acknowledgement code and check sum
    return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}


/*
 * Function name: PS_Match
 * Description: Exactly match the feature files in CharBuffer1 and CharBuffer2
 * Parameters: score(pointer, score of the comparison)
 * Return value: true (success), false (error), confirmation code is assigned to g_error_code
 * Confirmation code = 00H indicates a fingerprint match.
 * Confirmation code = 01H indicates an error in receiving the packet.
 * Confirmation code = 08H means fingerprint does not match.
 */
bool PS_Match(int* pScore) {
    int size = GenOrder(0x03, "");
    SendOrder(g_order, size);

    // Receive the answer packet, check the acknowledgement code and check sum
    return (RecvReply(g_reply, 14) && 
            Check(g_reply, 14) &&
            Merge(pScore, g_reply+10, 2));
}


/* 
 * Function name: PS_Search
 * Description: Search the whole or part of the fingerprint library with the feature file in CharBuffer1 or CharBuffer2
 * Parameters: bufferID (feature buffer number)
 * pageID(pointer to the search result returned)
 * score(pointer to the score corresponding to the search result)
 * startPageID(start pageID)
 * count(number of pages)
 * Return value: true (success), false (error), confirmation code assigned to g_error_code
 * Acknowledgement code = 00H means the search was completed.
 * Confirmation code = 01H indicates an error in receiving the packet.
 * Confirmation code=09H means no search; page number and score is 0
 */
bool PS_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore) {
    int size = GenOrder(0x04, "%d%2d%2d", bufferID, startPageID, count);
    SendOrder(g_order, size);

    // Receive the answer packet, check the acknowledgement code and check sum
    return ( RecvReply(g_reply, 16) && 
            Check(g_reply, 16) && 
            (Merge(pPageID, g_reply+10, 2)) &&  // assign a value to pageID and return true
            (Merge(pScore,  g_reply+12, 2)) // assign a value to score, return true
           );
}

/*
 * Function name: PS_RegModel
 * Description: Combine the feature files in CharBuffer1 and CharBuffer2 to generate a template.
 * The result is stored in CharBuffer1 and CharBuffer2.
 * Parameters: none
 * Return value: true (success), false (error), acknowledgement code is assigned to g_error_code
 * Acknowledgement code = 00H indicates a successful merge.
 * Acknowledgement code = 01H indicates an error in packet receipt.
 * confirmation code = 0aH means merge failed (two fingerprints do not belong to the same finger).
 */
bool PS_RegModel() {
    int size = GenOrder(0x05, "");
    SendOrder(g_order, size);

    // Receive the answer packet, check the acknowledgement code and check sum
    return (RecvReply(g_reply, 12) &&
            Check(g_reply, 12));
}


/*
 * Function name: PS_StoreChar
 * Description: Store the template file in CharBuffer1 or CharBuffer2 to the PageID number flash database location
 * Parameters: BufferID (buffer number), PageID (fingerprint database location number)
 * Return value: true (success), false (error), acknowledgement code assigned to g_error_code
 * Confirmation code = 00H indicates successful storage.
 * Acknowledgement code = 01H indicates an error in receiving the packet.
 * Acknowledgement code = 0bH indicates that the PageID is out of range of the fingerprint library.
 * Acknowledgement code = 18H indicates a FLASH write error.
 */
bool PS_StoreChar(uchar bufferID, int pageID) {
    int size = GenOrder(0x06, "%d%2d", bufferID, pageID);
    SendOrder(g_order, size);

    // Receive the answer packet, check the acknowledgement code and check sum
    return (RecvReply(g_reply, 12) && 
            Check(g_reply, 12));
}


/*
 * Function Name: PS_LoadChar
 * Description: Read the fingerprint template with the specified ID number from the flash database into the template buffer CharBuffer1 or CharBuffer2
 * Parameters : BufferID (buffer number), PageID (fingerprint library template number)
 * Return value: true (success), false (error), acknowledgement code is assigned to g_error_code
 * Acknowledgement code = 00H indicates a successful readout.
 * Acknowledgement code = 01H indicates a packet received in error.
 * Acknowledgement code = 0cH indicates a readout error or an invalid template.
 * Acknowledgement code = 0BH indicates that the PageID is out of range of the fingerprint library；
 */
bool PS_LoadChar(uchar bufferID, int pageID) {
    int size = GenOrder(0x07, "%d%2d", bufferID, pageID);
    SendOrder(g_order, size);

    // Receive the answer packet, check the acknowledgement code and check sum
    return (RecvReply(g_reply, 12) &&
            Check(g_reply, 12));
}

/*
 * Function name: PS_UpChar
 * Description: Upload the feature file from the feature buffer to the host computer (downloaded from the fingerprint recognition module to the Raspberry Pi)
 * Parameters : bufferID (buffer number)
 * Return value : true (success), false (error), acknowledgement code assigned to g_error_code
 * Acknowledgement code = 00H indicates that the packet was subsequently sent.
 * Acknowledgement code = 01H indicates an error in receiving the packet.
 * Acknowledgement code = 0dH indicates that command execution failed.
 */
bool PS_UpChar(uchar bufferID, const char* filename) {
    int size = GenOrder(0x08, "%d", bufferID);
    SendOrder(g_order, size);

    // Receive the answer packet, check the acknowledgement code and check sum
    if (!(RecvReply(g_reply, 12) && Check(g_reply, 12))) {
        return false;
    }

    // Receive the packet and store the valid data in pData
    uchar pData[768] = { 0 };
    if (!RecvPacket(pData, 768)) {
        return false;
    }

    // Write to file
    FILE* fp = fopen(filename, "w+");
    if (!fp) { 
        g_error_code = 0xC2;
        return false;
    }

    fwrite(pData, 1, 768, fp);
    fclose(fp);

    return true;
}


/*
 * Function name: PS_DownChar
 * Description : The host computer downloads the feature file to one of the feature buffers of the module (uploaded from the Raspberry Pi to the fingerprint recognition module)
 * Return value : true (success), false (error), acknowledgement code assigned to g_error_code
 * Acknowledgement code = 00H indicates that subsequent packets can be received.
 * Acknowledgement code = 01H indicates an error in receiving the packet.
 * Acknowledgement code = 0eH indicates that subsequent packets cannot be received.
 */
bool PS_DownChar(uchar bufferID, const char* filename) {
    // Sending instructions
    int size = GenOrder(0x09, "%d", bufferID);
    SendOrder(g_order, size);

    // Receive an answer packet, if the acknowledgement code is 0x00, it means that subsequent packets can be sent
    if ( !(RecvReply(g_reply, 12) && Check(g_reply, 12)) )
        return false;

    // Open local files
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        g_error_code = 0xC2;
        return false;
    }

    // Get file size
    int fileSize = 0;
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    rewind(fp);
    if (fileSize != 768) {
        g_error_code = 0x09;
        fclose(fp);
        return false;
    }

    // Read the contents of a file
    uchar charBuf[768] = { 0 };
    fread(charBuf, 1, 768, fp);

    fclose(fp);

    // Sending data packets
    return SendPacket(charBuf, 768);
}


/*
 * Function name: PS_UpImage
 * Description: Upload the data in the image buffer to the Raspberry Pi (download image)
 * Parameters : Name of the saved file, format (.bmp)
 * Return value: true (success), false (error), acknowledgement code assigned to g_error_code
 * Acknowledgement code = 00H indicates that a subsequent packet was sent.
 * Acknowledgement code = 01H indicates an error in receiving the packet.
 * Acknowledgement code = 0fH indicates that subsequent packets cannot be sent.
 */
bool PS_UpImage(const char* filename) {
    int size = GenOrder(0x0a, "");
    SendOrder(g_order, size);

    // Receive the answer packet, check the acknowledgement code and check sum
    if (!(RecvReply(g_reply, 12) && Check(g_reply, 12))) {
        return false;
    }

    // Receive the packet and store the valid data in pData
    // Image size 128*288 = 36864
    uchar* pData = (uchar*)malloc(36864);
    if (!RecvPacket(pData, 36864)) {
        return false;
    }

    // Writing pData to a file
    FILE* fp = fopen(filename, "w+");
    if (!fp) {
        g_error_code = 0xC2;
        return false;
    }

    // Constructing the bmp header (fixed for this module)
    uchar header[54] = "\x42\x4d\x00\x00\x00\x00\x00\x00\x00\x00\x36\x04\x00\x00\x28\x00\x00\x00\x00\x01\x00\x00\x20\x01\x00\x00\x01\x00\x08";
    for (int i = 29; i < 54; ++i)
        header[i] = 0x00;
    fwrite(header, 1, 54, fp);

    // Palette
    uchar palette[1024] = { 0 };
    for (int i = 0; i < 256; ++i) {
        palette[4*i]   = i;
        palette[4*i+1] = i;
        palette[4*i+2] = i;
        palette[4*i+3] = 0;
    }
    fwrite(palette, 1, 1024, fp);

    // bmp pixel data
    uchar* pBody = (uchar*)malloc(73728);
    for (int i = 0; i < 73728; i += 2) {
        pBody[i] = pData[i/2] & 0xf0;  
    }
    for (int i = 1; i < 73728; i += 2) {
        pBody[i] = (pData[i/2] & 0x0f) << 4;
    }

    fwrite(pBody, 1, 73728, fp);

    free(pBody);
    free(pData);
    fclose(fp);

    return true; 
}


/*
 * Function name: PS_DownImage
 * Description: The upper computer downloads the image data to the module (uploads the image to the AS608 module)
 * Parameters : Local fingerprint image file name
 * Return value: true (success), false (error), acknowledgement code assigned to g_error_code
 * Acknowledgement code = 00H indicates that subsequent packets can be received.
 * Acknowledgement code = 01H indicates an error in receiving the packet.
 * Acknowledgement code = 0eH indicates that subsequent packets cannot be received.
 */
bool PS_DownImage(const char* filename) {
    int size = GenOrder(0x0b, "");
    SendOrder(g_order, size);

    if (!RecvReply(g_reply, 12) && Check(g_reply, 12))
        return false;

    // Fingerprint image file size 748069 bytes
    uchar imageBuf[74806] = { 0 };

    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        g_error_code = 0xC2;
        return false;
    }

    // Get image file size
    int imageSize = 0;
    fseek(fp, 0, SEEK_END);
    imageSize = ftell(fp);
    rewind(fp);
    if (imageSize != 74806) { // Fingerprint image size must be 74806kb
        g_error_code = 0xC9;
        fclose(fp);
        return false;
    }

    // Read documents
    if (fread(imageBuf, 1, 74806, fp) != 74806) {
        g_error_code = 0xCA;
        fclose(fp);
        return false;
    }
    fclose(fp);

    //FILE* fpx = fopen("temp.bmp", "wb");
    //if (!fpx) {
    //  printf("Error\n");
    //  return false;
    //}
    //fwrite(imageBuf, 1, 74806, fpx);
    //fclose(fpx);
    //return true;

    //uchar dataBuf[128*288] = { 0 };
    //for (uint i = 0, size=128*288; i < size; ++i) {
    //  dataBuf[i] = imageBuf[1078 + i*2] + (imageBuf[1078 + i*2 + 1] >> 4);
    //}

    // send the pixel data of the image, offset 54+1024=1078, size 128*256*2=73728
    //return SendPacket(dataBuf, 128*288);
    return SendPacket(imageBuf+1078, 73728);
}


/*
 * Function name: PS_DeleteChar
 * Description: Delete the N fingerprint templates starting from the specified ID number in the flash database
 * Parameters: startPageID(the starting value of the fingerprint database template number) count(the number of templates deleted)
 * Return value: true (success), false (error), confirmation code is assigned to g_error_code
 * Acknowledgement code = 00H means the template was deleted successfully.
 * Confirmation code = 01H indicates an error in receiving the packet.
 * acknowledgement code = 10H indicates that the deletion of the template failed.
 */
bool PS_DeleteChar(int startPageID, int count) {
    int size = GenOrder(0x0c, "%2d%2d", startPageID, count);
    SendOrder(g_order, size);

    // Receive data, check confirmation codes and checksums
    return (RecvReply(g_reply, 12) &&
            Check(g_reply, 12));
}

/*
 * Function name: PS_Empty
 * Description: Delete all fingerprint templates from the flash database.
 * Parameters: none
 * Return value: true (success), false (error), confirmation code assigned to g_error_code
 * Confirmation code = 00H means emptying was successful.
 * Acknowledgement code = 01H indicates an error in receiving the packet.
 * Acknowledgement code = 11H means emptying failed.
 */
bool PS_Empty() {
    int size = GenOrder(0x0d, "");
    SendOrder(g_order, size);

    // Receive data, check confirmation codes and checksums
    return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}

/*
 * Function Name: PS_WriteReg
 * Description: Write module register
 * Parameters : none(parameters are stored in g_as608.chip_addr, g_as608.packet_size, PS_BPS and other system variables)
 * Return value: true (success), false (error), acknowledgement code is assigned to g_error_code
 * Acknowledgement code = 00H means OK.
 * Acknowledgement code = 01H indicates a packet received in error.
 * Acknowledgement code = 1aH indicates an error in the register serial number.
 */
bool PS_WriteReg(int regID, int value) {
    if (regID != 4 && regID != 5 && regID != 6) {
        g_error_code = 0x1a;
        return false;
    }

    int size = GenOrder(0x0e, "%d%d", regID, value);
    SendOrder(g_order, size);

    // Receive data, check confirmation codes and checksums
    return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}

/*
 * Function name: PS_ReadSysPara
 * Description: Read the basic parameters of the module (baud rate, packet size, etc.).
 * Parameters : none (parameters are saved to g_as608.chip_addr, g_as608.packet_size, PS_BPS and other system variables)
 * Return value: true (success), false (error), acknowledgement code is assigned to g_error_code
 * Acknowledgement code = 00H means OK.
 * Acknowledgement code = 01H indicates an error in receiving the packet.
 */
bool PS_ReadSysPara() {
    int size = GenOrder(0x0f, "");
    SendOrder(g_order, size);

    return (RecvReply(g_reply, 28) &&
            Check(g_reply, 28) &&
            Merge(&g_as608.status,       g_reply+10, 2) &&
            Merge(&g_as608.model,        g_reply+12, 2) && 
            Merge(&g_as608.capacity,     g_reply+14, 2) &&
            Merge(&g_as608.secure_level, g_reply+16, 2) &&
            Merge(&g_as608.chip_addr,    g_reply+18, 4) &&
            Merge(&g_as608.packet_size,  g_reply+22, 2) &&
            Merge(&g_as608.baud_rate,    g_reply+24, 2) &&
            (g_as608.packet_size = 32 * (int)pow(2, g_as608.packet_size)) &&
            (g_as608.baud_rate *= 9600)
           );
}

/*
 * Function Name: PS_Enroll
 * Description: Capture a fingerprint registration template once, search for empty space in the fingerprint library and store it, return the storage ID
 * Parameters : pageID(pointer, pass out the storage ID)
 * Return value : true (success), false (error), confirmation code assigned to g_error_code
 * Acknowledgement code = 00H indicates successful registration.
 * Acknowledgement code = 01H indicates an error in receiving the packet.
 * Confirmation code = 1eH means registration failed.
 */
bool PS_Enroll(int* pPageID) {
    int size = GenOrder(0x10, "");
    SendOrder(g_order, size);

    // Receive data, check confirmation codes and checksums
    return (RecvReply(g_reply, 14) &&
            Check(g_reply, 14) &&
            Merge(pPageID, g_reply+10, 2)
           );
}

/*
 * Function Name: PS_Identify
 * Description: 1. Automatically collect fingerprints, search for the target template in the fingerprint database and return the search result.
 * If the target template has a score greater than the maximum threshold for comparison with the currently collected fingerprint and the target template
 * If the target template is incomplete, the blank area of the target template is updated with the captured features.
 * Parameters: none
 * Return value: true (success), false (error), confirmation code is assigned to g_error_code
 * Acknowledgement code = 00H indicates that the search was performed.
 * Confirmation code = 01H indicates an error in receiving the packet.
 * Confirmation code=09H means no search; page number and score is 0
 */
bool PS_Identify(int* pPageID, int* pScore) { 
    int size = GenOrder(0x11, "");
    SendOrder(g_order, size);

    // Receive data, check confirmation codes and checksums
    return (RecvReply(g_reply, 16) &&
            Check(g_reply, 16) &&
            Merge(pPageID, g_reply+10, 2) &&
            Merge(pScore,  g_reply+12, 2)
           );
}

/*
 * Function name: PS_SetPwd
 * Description: Set the module handshake password
 * Parameters: passwd(password)
 * Return value: true (success), false (error), acknowledgement code is assigned to g_error_code
 * Confirmation code = 00H means OK.
 * Acknowledgement code = 01H means there was an error in receiving the packet.
 */
bool PS_SetPwd(uint pwd) {   // 0x00 ~ 0xffffffff
    int size  = GenOrder(0x12, "%4d", pwd);
    SendOrder(g_order, size);

    // Receive data, check confirmation codes and checksums
    return (RecvReply(g_reply, 12) && 
            Check(g_reply, 12) &&
            (g_as608.has_password = 1) &&
            ((g_as608.password = pwd) || true)); //Prevent pwd=0x00
}


/*
 * Function name: PS_VfyPwd
 * Description: Verify the module handshake password
 * Parameters: passwd(password) 0x00 ~ 0xffffffff
 * Return value: true (success), false (error), confirmation code is assigned to g_error_code
 * Confirmation code = 00H means the password was verified correctly.
 * Confirmation code = 01H indicates an error in packet receipt.
 * Confirmation code = 13H indicates incorrect password.
 */
bool PS_VfyPwd(uint pwd) { 
    int size = GenOrder(0x13, "%4d", pwd); 
    SendOrder(g_order, size);

    // Receive data, check confirmation codes and checksums
    return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}

/*
 * Function Name: PS_GetRandomCode
 * Description: Make the chip generate a random number
 * Parameters: none
 * Return value: true (success), false (error), confirmation code is assigned to g_error_code
 * Acknowledgement code = 00H means that the generation was successful.
 * Acknowledgement code = 01H indicates an error in receiving the packet.
 */
bool PS_GetRandomCode(uint* pRandom) {
    int size = GenOrder(0x14, "");
    SendOrder(g_order, size);

    // Receive data, check confirmation codes and checksums
    return (RecvReply(g_reply, 16) &&
            Check(g_reply, 16) &&
            Merge(pRandom, g_reply+10, 4)
           );
}

/*
 * Function name: PS_SetChipAddr
 * Description: Set the chip address
 * Parameters: addr(new address of the chip)
 * Return value: true (success), false (error), acknowledgement code assigned to g_error_code
 * Acknowledgement code = 00H means that the address generation was successful.
 * Acknowledgement code = 01H indicates an error in packet receipt.
 * Remarks.
 * The default address of the chip is 0xffffffff when the upper computer transmits the command packet, and the newly generated address is used in the address field of the reply packet
 * After this instruction is executed, the chip address is then fixed and remains unchanged. Only by clearing the FLASH can the chip address be changed
 * After this instruction is executed, all packets must use the generated address.
 */
bool PS_SetChipAddr(uint addr) {
    int size = GenOrder(0x15, "%4d", addr);
    SendOrder(g_order, size);

    // Receive data, check confirmation codes and checksums
    return (RecvReply(g_reply, 12) && 
            Check(g_reply, 12) && 
            ((g_as608.chip_addr = addr) || true)); // Prevent addr=0x00
}

/*
 * Function name: PS_ReadINFpage
 * Description: Read the information page where the FLASH Information Page is located (512bytes)
 * Parameters: pInfo, pInfoSize(array size)
 * Return value: true (success), false (error), acknowledgement code assigned to g_error_code
 * Acknowledgement code = 00H indicates that the packet was subsequently sent.
 * Acknowledgement code = 01H indicates an error in receiving the packet.
 * Acknowledgement code = 0dH indicates that command execution failed.
 */
bool PS_ReadINFpage(uchar* pInfo, int pInfoSize/*>=512*/) {
    if (pInfoSize < 512) {
        g_error_code = 0xC1;
        return false;
    }

    int size = GenOrder(0x16, "");
    SendOrder(g_order, size);

    // Receiving an answer packet
    if (!(RecvReply(g_reply, 12) && Check(g_reply, 12))) 
        return false;

    // Receiving packets
    if (!RecvPacket(pInfo, 512))
        return false;

    memcpy(g_as608.product_sn,       pInfo+28, 8);
    memcpy(g_as608.software_version, pInfo+36, 8);
    memcpy(g_as608.manufacture,      pInfo+44, 8);
    memcpy(g_as608.sensor_name,      pInfo+52, 8);

    return true;
}

/*
 * Function name: PS_WriteNotepad
 * Description: The module internally creates 256bytes of FLASH space for the user to store user data.
 * The storage space is called the user's notepad, which is logically divided into 16 pages.
 * This command writes 32bytes of user data to the specified notepad page.
 * Parameters: notePageID (number of pages), buf (content of the page), bufSize (<=32)
 * Return value: true (success), false (error), acknowledgement code assigned to g_error_code
 * Acknowledgement code = 00H means OK.
 * Acknowledgement code = 01H means that there was an error in receiving the packet.
 */
bool PS_WriteNotepad(int notePageID, uchar* pContent, int contentSize) {
    if (contentSize > 32) {
        g_error_code = 0xC6;
        return false;
    }

    pContent[32] = 0; // String terminator
    int size = GenOrder(0x18, "%d%32s", notePageID, pContent);
    SendOrder(g_order, size);

    return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}

/*
 * Function Name: PS_ReadNotepad
 * Description: Read 128bytes of data from the FLASH user area
 * Parameters : notePageID(number of pages), buf(store the content read), bufSize(>=32)
 * Return value: true (success), false (error), acknowledgement code is assigned to g_error_code
 * Acknowledgement code = 00H means OK.
 * Acknowledgement code = 01H means that there was an error in receiving the packet.
 */
bool PS_ReadNotepad(int notePageID, uchar* pContent, int contentSize) {
    if (contentSize < 32) {
        g_error_code = 0xC1;
        return false;
    }

    int size = GenOrder(0x19, "%d", notePageID);
    SendOrder(g_order, size);

    // Receive the answer packet, check the acknowledgement code and check sum
    if (!(RecvReply(g_reply, 44) && Check(g_reply, 44)))
        return false;

    memcpy(pContent, g_reply+10, 32);
    return true;
}

/*
 * Function name: PS_HighSpeedSearch
 * Description: Searches the whole or part of the fingerprint library at high speed using the feature file in CharBuffer1 or CharBuffer2. If the search is found, the page number is returned.
 * This command will give quick search results for fingerprints that do exist in the fingerprint library and are of good quality at login.
 * Parameters: bufferID(feature buffer number)
 * startPageID(start page code)
 * count(number of pages)
 * pageID(pointer to the search result returned)
 * score(pointer to the score corresponding to the search result)
 * Return value: true (success), false (error), confirmation code assigned to g_error_code
 * Acknowledgement code = 00H means the search was performed.
 * Confirmation code = 01H indicates an error in receiving the packet.
 * Confirmation code=09H means no search; page number and score is 0
 */
bool PS_HighSpeedSearch(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore) {
    int size = GenOrder(0x1b, "%d%2d%2d", bufferID, startPageID, count);
    SendOrder(g_order, size);

    // Receive data, check confirmation codes and checksums
    return ( RecvReply(g_reply, 16) && 
            Check(g_reply, 16) && 
            (Merge(pPageID, g_reply+10, 2)) &&  // assign a value to pageID and return true
            (Merge(pScore, g_reply+12, 2)) // assign a value to score, return true
           );
}

/*
 * Function name: PS_ValidTempleteNum
 * Description: Read the number of valid templates
 * Parameters: num (address parameter, save the number of valid templates)
 * Return value: true (success), false (error), acknowledgement code assigned to g_error_code
 * Acknowledgement code = 00H indicates a successful read.
 * Acknowledgement code = 01H indicates an error in receiving the packet.
 */
bool PS_ValidTempleteNum(int* pValidN) {
    int size = GenOrder(0x1d, "");
    SendOrder(g_order, size);

    // Receive data, check confirmation codes and checksums
    return (RecvReply(g_reply, 14) &&
            Check(g_reply, 14) &&
            Merge(pValidN, g_reply+10, 2)
           );
}

/*
 * Function name: PS_ReadIndexTable
 * Description: Reads the index table of the entry template.
 * Parameters.
 * Return value : true (success), false (error), acknowledgement code assigned to g_error_code
 * Confirmation code = 00H means OK.
 * Acknowledgement code = 01H indicates an error in receiving the packet.
 */
bool PS_ReadIndexTable(int* indexList, int size) {
    // Initialize all elements of indexList to -1
    for (int i = 0; i < size; ++i)
        indexList[i] = -1;

    int nIndex = 0;

    for (int page = 0; page < 2; ++page) {
        // sending data (twice, 256 fingerprint templates per page, two pages need to be requested)
        int size = GenOrder(0x1f, "%d", page);
        SendOrder(g_order, size);

        // Receive data, check confirmation codes and checksums
        if (!(RecvReply(g_reply, 44) && Check(g_reply, 44)))
            return false;

        for (int i = 0; i < 32; ++i) {
            for (int j = 0; j < 8; ++j) {
                if ( ( (g_reply[10+i] & (0x01 << j) ) >> j) == 1 ) {
                    if (nIndex > size) {
                        g_error_code = 0xC1;    // Arrays too small
                        return false;
                    }
                    indexList[nIndex++] = page*256 + 8 * i + j;
                } // end if

            } // end internel for

        } // end middle for

    }// end outer for

    return true;
}

/******************************************************************
 *
 * Part 3.
 * Encapsulated functions, declared in as608.h
 *
 ******************************************************************/

// detects if the fingerprint is present
// If status is HEGH, then the module returns true if there is a fingerprint on it, false if there is no fingerprint
// If status is LOW, return false if the module has a fingerprint, true if it doesn't
bool PS_DetectFinger(int status) {
    if (digitalRead(g_as608.detect_pin) == status)
        return true;
    else
        return false;
}

bool PS_SetBaudRate(int value) {
    return PS_WriteReg(4, value / 9600);
}

bool PS_SetSecureLevel(int level) {
    return PS_WriteReg(5, level);
}

bool PS_SetPacketSize(int size) {
    int value = 0;
    printf("size=%d\n", size);
    switch (size) {
        default: 
            g_error_code = 0xC5; 
            return false;
        case 32:  value = 0; break;
        case 64:  value = 1; break;
        case 128: value = 2; break;
        case 256: value = 3; break;
    }

    return PS_WriteReg(6, value);
}

/*
 * Get the module details and assign them to the corresponding global variables, g_as608.packet_size, PS_LEVEL, etc.
 */
bool PS_GetAllInfo() {
    uchar buf[512] = { 0 };
    if (PS_ReadSysPara() && g_as608.packet_size > 0 && PS_ReadINFpage(buf, 512)) {
        return true;
    }
    else {
        g_error_code = 0xC7;
        return false;
    }
}

/*
 * refreshing the buffer.
 * This function can be executed when the program unexpectedly exits during the reception of data, such as when the data is not received or happens to be finished
 **/
bool PS_Flush() {
    int num = 0;
    for (int i = 0; i < 3; ++i) {
        if (PS_ValidTempleteNum(&num)) {
            return true;
        }
        sleep(1);
    }
    return false;
}

/*
 * Get the description of the error code
 * assign to the global variable g_error_desc, and return g_error_desc
 */
char* PS_GetErrorDesc() {
    switch (g_error_code) {
        default:   strcpy(g_error_desc, "Undefined error"); break;
        case 0x00: strcpy(g_error_desc, "OK"); break;
        case 0x01: strcpy(g_error_desc, "Recive packer error"); break;
        case 0x02: strcpy(g_error_desc, "No finger on the sensor"); break;
        case 0x03: strcpy(g_error_desc, "Failed to input fingerprint image"); break;
        case 0x04: strcpy(g_error_desc, "Fingerprint images are too dry and bland to be characteristic"); break;
        case 0x05: strcpy(g_error_desc, "Fingerprint images are too wet and mushy to produce features"); break;
        case 0x06: strcpy(g_error_desc, "Fingerprint images are too messy to be characteristic"); break;
        case 0x07: strcpy(g_error_desc, "The fingerprint image is normal, but there are too few feature points (or too small area) to produce a feature"); break;
        case 0x08: strcpy(g_error_desc, "Fingerprint mismatch"); break;
        case 0x09: strcpy(g_error_desc, "Not found in fingerprint libary"); break;
        case 0x0A: strcpy(g_error_desc, "Feature merge failed"); break;
        case 0x0B: strcpy(g_error_desc, "The address serial number is out of the range of fingerprint database when accessing fingerprint database"); break;
        case 0x0C: strcpy(g_error_desc, "Error or invalid reading template from fingerprint database"); break;
        case 0x0D: strcpy(g_error_desc, "Upload feature failed"); break;
        case 0x0E: strcpy(g_error_desc, "The module cannot accept subsequent packets"); break;
        case 0x0F: strcpy(g_error_desc, "Failed to upload image"); break;
        case 0x10: strcpy(g_error_desc, "Failed to delete template"); break;
        case 0x11: strcpy(g_error_desc, "Failed to clear the fingerprint database"); break;
        case 0x12: strcpy(g_error_desc, "Cannot enter low power consumption state"); break;
        case 0x13: strcpy(g_error_desc, "Incorrect password"); break;
        case 0x14: strcpy(g_error_desc, "System reset failure"); break;
        case 0x15: strcpy(g_error_desc, "An image cannot be generated without a valid original image in the buffer"); break;
        case 0x16: strcpy(g_error_desc, "Online upgrade failed"); break;
        case 0x17: strcpy(g_error_desc, "There was no movement of the finger between the two collections"); break;
        case 0x18: strcpy(g_error_desc, "FLASH reading or writing error"); break;
        case 0x19: strcpy(g_error_desc, "Undefined error"); break;
        case 0x1A: strcpy(g_error_desc, "Invalid register number"); break;
        case 0x1B: strcpy(g_error_desc, "Register setting error"); break;
        case 0x1C: strcpy(g_error_desc, "Notepad page number specified incorrectly"); break;
        case 0x1D: strcpy(g_error_desc, "Port operation failed"); break;
        case 0x1E: strcpy(g_error_desc, "Automatic enrollment failed"); break;
        case 0xFF: strcpy(g_error_desc, "Fingerprint is full"); break;
        case 0x20: strcpy(g_error_desc, "Reserved. Wrong address or wrong password"); break;
        case 0xF0: strcpy(g_error_desc, "There are instructions for subsequent packets, and reply with 0xf0 after correct reception"); break;
        case 0xF1: strcpy(g_error_desc, "There are instructions for subsequent packets, and the command packet replies with 0xf1"); break;
        case 0xF2: strcpy(g_error_desc, "Checksum error while burning internal FLASH"); break;
        case 0xF3: strcpy(g_error_desc, "Package identification error while burning internal FLASH"); break;
        case 0xF4: strcpy(g_error_desc, "Packet length error while burning internal FLASH"); break;
        case 0xF5: strcpy(g_error_desc, "Code length is too long to burn internal FLASH"); break;
        case 0xF6: strcpy(g_error_desc, "Burning internal FLASH failed"); break;
        case 0xC1: strcpy(g_error_desc, "Array is too smalll to store all the data"); break;
        case 0xC2: strcpy(g_error_desc, "Open local file failed!"); break;
        case 0xC3: strcpy(g_error_desc, "Packet loss"); break;
        case 0xC4: strcpy(g_error_desc, "No end packet received, please flush the buffer(PS_Flush)"); break;
        case 0xC5: strcpy(g_error_desc, "Packet size not in 32, 64, 128 or 256"); break;
        case 0xC6: strcpy(g_error_desc, "Array size is to big");break;
        case 0xC7: strcpy(g_error_desc, "Setup failed! Please retry again later"); break;
        case 0xC8: strcpy(g_error_desc, "The size of the data to send must be an integral multiple of the g_as608.packet_size"); break;
        case 0xC9: strcpy(g_error_desc, "The size of the fingerprint image is not 74806bytes(about73.1kb)");break;
        case 0xCA: strcpy(g_error_desc, "Error while reading local fingerprint imgae"); break;

    }

    return g_error_desc;
}
