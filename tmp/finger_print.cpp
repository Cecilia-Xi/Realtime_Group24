#include "finger_print.h"

FingerPrint::FingerPrint(){
	//g_order[64] = { 0 }; // 发送给模块的指令包
	//g_reply[64] = { 0 }; // 模块的应答包 
	cout<<"auto\n";
}
	
FingerPrint::~FingerPrint(){

}

void FingerPrint::run(){
	cout<<"in running";
}

void FingerPrint::lockerControl()
{
  pinMode (SWITCH,OUTPUT);
  printf("FBI open The Door!\n");
  digitalWrite (SWITCH,LOW);//set it initially as high, is closed

  delay(1000);
  digitalWrite (SWITCH,HIGH);

  pinMode(g_config.detect_pin, INPUT);
}





bool FingerPrint::PS_GetImage() {
  int size = GenOrder(0x01, "");

  // send the command pack 
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) && Check(g_reply, 12));

}


bool FingerPrint::PS_GenChar(uchar bufferID) {
  int size = GenOrder(0x02, "%d", bufferID);
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}

bool FingerPrint::PS_Match(int* pScore) {
  int size = GenOrder(0x03, "");
  SendOrder(g_order, size);

  //  receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 14) && 
          Check(g_reply, 14) &&
          Merge((uint*)pScore, g_reply+10, 2));
}



bool FingerPrint::PS_RegModel() {
  int size = GenOrder(0x05, "");
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) &&
      Check(g_reply, 12));
}


bool FingerPrint::PS_StoreChar(uchar bufferID, int pageID) {
  int size = GenOrder(0x06, "%d%2d", bufferID, pageID);
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) && 
        Check(g_reply, 12));
}

bool FingerPrint::PS_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore) {

  int size = GenOrder(0x04, "%d%2d%2d", bufferID, startPageID, count);
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return ( RecvReply(g_reply, 16) && 
           Check(g_reply, 16) && 
           (Merge((uint*)pPageID, g_reply+10, 2)) &&  // assign value to pageID, return true
           (Merge((uint*)pScore,  g_reply+12, 2))     // assign value to score, return true
        );
}

bool FingerPrint::PS_DetectFinger() {
  return digitalRead(g_as608.detect_pin) == HIGH;
}

// Block until a finger is detected, the longest block wait_time is milliseconds
bool FingerPrint::waitUntilDetectFinger(int wait_time) {
  while (true) {
    if ( PS_DetectFinger()) {
      return true;
    }
    else {
      delay(100);
      wait_time -= 100;
      if (wait_time < 0) {
        return false;
      }
    }
  }
}

bool FingerPrint::waitUntilNotDetectFinger(int wait_time) {
  while (true) {
    if (! PS_DetectFinger()) {
      return true;
    }
    else {
      delay(100);
      wait_time -= 100;
      if (wait_time < 0) {
        return false;
      }
    }
  }
}

bool FingerPrint::PS_Empty() {
  int size = GenOrder(0x0d, "");
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum 
  return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}

//The program exited because the function in as608.h failed to execute
bool FingerPrint::PS_Exit()
{
  printf("ERROR! code=%02X, desc=%s\n", g_error_code,  PS_GetErrorDesc());
  return true;
}

// obtain the defination of error code of g_error_code and assign value to g_error_desc
char* FingerPrint::PS_GetErrorDesc()
{
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




