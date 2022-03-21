#ifndef __AS608_H__
#define __AS608_H__

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct AS608_Module_Info {
    uint status;      // Status register 0
    uint model; // Sensor type 0-15
    uint capacity; // Fingerprint capacity, 300
    uint secure_level; // security level 1/2/3/4/5, default is 3
    uint packet_size; // packet size 32/64/128/256 bytes, default is 128
    uint baud_rate; // baud rate factor 
    uint chip_addr; // device (chip) address                  
    uint password; // communication password
    char product_sn[12]; // model number
    char software_version[12]; // software version number
    char manufacture[12]; // Manufacturer's name
    char sensor_name[12]; // Sensor name

    uint detect_pin; // the Raspberry Pi GPIO pin number connected to the WAK pin of the AS608
    uint has_password; // whether there is a password
} AS608;


/*******************************BEGIN**********************************
 * Global variables
 */
extern AS608 g_as608;
extern int g_fd; // file descriptor, i.e. the return value of the open() function to open the serial port
extern int g_verbose; // The level of detail of the output message
extern char g_error_desc[128]; // the meaning of the error code
extern uchar g_error_code; // the acknowledgement code returned by the module, read this variable if the function returns a value other than true
/*
 **********************************END********************************/


#ifdef __cplusplus
extern "C" {
#endif

    extern bool PS_Setup(uint chipAddr, uint password);       // 0x00000000 ~ 0xffffffff

    extern bool PS_GetImage();
    extern bool PS_GenChar(uchar bufferID);
    extern bool PS_Match(int* pScore);
    extern bool PS_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);
    extern bool PS_RegModel();
    extern bool PS_StoreChar(uchar bufferID, int pageID);
    extern bool PS_LoadChar(uchar bufferID, int pageID);
    extern bool PS_UpChar(uchar bufferID, const char* filename);
    extern bool PS_DownChar(uchar bufferID, const char* filename);
    extern bool PS_UpImage(const char* filename);
    extern bool PS_DownImage(const char* filename);
    extern bool PS_DeleteChar(int startpageID, int count);
    extern bool PS_Empty();
    extern bool PS_WriteReg(int regID, int value);
    extern bool PS_ReadSysPara();
    extern bool PS_Enroll(int* pPageID);
    extern bool PS_Identify(int* pPageID, int* pScore);
    extern bool PS_SetPwd(uint passwd);   // 4-byte unsigned integer
    extern bool PS_VfyPwd(uint passwd); // 4-byte unsigned integer
    extern bool PS_GetRandomCode(uint* pRandom);
    extern bool PS_SetChipAddr(uint newAddr);
    extern bool PS_ReadINFpage(uchar* pInfo, int size/*>=512*/);
    extern bool PS_WriteNotepad(int notePageID, uchar* pContent, int contentSize);
    extern bool PS_ReadNotepad(int notePageID, uchar* pContent, int contentSize);
    extern bool PS_HighSpeedSearch(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);
    extern bool PS_ValidTempleteNum(int* pValidN);
    extern bool PS_ReadIndexTable(int* indexList, int size);

    // Encapsulated functions
    extern bool PS_DetectFinger(int status);    // Detecting the presence of fingerprints
    extern bool PS_SetBaudRate(int value);
    extern bool PS_SetSecureLevel(int level);
    extern bool PS_SetPacketSize(int size);
    extern bool PS_GetAllInfo();
    extern bool PS_Flush();

    // Get the meaning of the error code g_error_code and assign it to g_error_desc
    extern char* PS_GetErrorDesc(); 

#ifdef __cplusplus
}
#endif

#endif // __AS608_H__
