#include "FingerPrinter.h"

int auto_page_id = 0;

AS_608 g_as608;
Config g_config;   // 配置文件 结构体，定义在"./utils.h"头文件中
int   g_fd;          // file char, return when serial opened
int   g_verbose;     // the details level of output
char  g_error_desc[128]; // error info
uchar g_error_code;      // the module return code when function return false

uchar g_order[64] = { 0 }; // the instruction package 
uchar g_reply[64] = { 0 }; // the reply package


bool Get_search_CallBack(void* lpvoid, cb_search_ptr callback_param, uchar bufferID, int startPageID, int count, int* pPageID, int* pScore)
{
	return callback_param(lpvoid, bufferID, startPageID, count, pPageID, pScore);
}
    

bool Get_getimage_CallBack(void* lpvoid, cb_getimage_ptr callback_param)
{
	return callback_param(lpvoid);
}
 

bool Get_genchar_CallBack(void* lpvoid, cb_genchar_ptr callback_param, uchar a)
{
	return callback_param(lpvoid, a);
}
    

bool Get_exit_CallBack(void* lpvoid, cb_exit_ptr callback_param)
{
	return callback_param(lpvoid);
}




///////////////////////
/*
void FingerPrinter::registerCallback(CallbackInterface* cb) {
	PS_Setupcallback = cb;
}*/
///////////////////////

void FingerPrinter::cb_add ()
{
  printf("Please put your finger on the module.\n");
  
  if (waitUntilDetectFinger(5000))
  {
    delay(500);
    if(Get_getimage_CallBack(this,getimage_CALLBACK)){
      Get_genchar_CallBack(this,genchar_CALLBACK,1)||Get_exit_CallBack(this,exit_CALLBACK);
      // Determine if the user has raised their finger，
      printf("Ok.\nPlease raise your finger !\n");
      if (waitUntilNotDetectFinger(5000)){
		delay(1000);
		printf("Ok.\nPlease put your finger again!\n");
		// input fingerprint for sencond time
		if (waitUntilDetectFinger(5000)){
		  delay(500);
		  if(Get_getimage_CallBack (this,getimage_CALLBACK)){
			Get_genchar_CallBack(this,genchar_CALLBACK,2)||Get_exit_CallBack(this,exit_CALLBACK);
			int score = 0;
			if ( PS_Match(&score)){
				printf("Matched! score=%d\n", score);
				if (g_error_code != 0x00)
					Get_exit_CallBack(this,exit_CALLBACK);
				else{
					// Merge feature files
					if(PS_RegModel()){
					  PS_StoreChar(2,  auto_page_id);
					  printf("OK! New fingerprint saved to pageID=%d\n",  auto_page_id);
					  auto_page_id++;
					}
					else
					  Get_exit_CallBack(this,exit_CALLBACK);
				}
			}
			else{
			  printf("Not matched, raise your finger and put it on again.\n");
			}	
		  }
		  else
			Get_exit_CallBack(this,exit_CALLBACK);
		}
		else{
		  printf("Error: Didn't detect finger!\n");
		}
      }
      else{
		printf("Error! Didn't raise your finger\n");
	 }
	}
	else
	  Get_exit_CallBack(this,exit_CALLBACK);
  }
  else{
    printf("Error: Didn't detect finger!\n");
  }  
}

    
bool FingerPrinter::CB_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore)
{   
	int size = GenOrder(0x04, "%d%2d%2d", bufferID, startPageID, count);
	SendOrder(g_order, size);

	// receive the response pack, check the comfirmation pack and verify sum 
	return ( RecvReply(g_reply, 16) && 
           Check(g_reply, 16) && 
           (Merge((uint*)pPageID, g_reply+10, 2)) &&  // assign value to pageID, return true
           (Merge((uint*)pScore,  g_reply+12, 2))     // assign value to score, return true
    );
    
}
    
bool FingerPrinter::CB_GetImage()
{
    int size = GenOrder(0x01, "");
    // send the command pack 
    SendOrder(g_order, size);
    // receive the response pack, check the comfirmation pack and verify sum 
    return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}


bool FingerPrinter::CB_GenChar(uchar bufferID)
{
    int size = GenOrder(0x02, "%d", bufferID);
    SendOrder(g_order, size);
    // receive the response pack, check the comfirmation pack and verify sum 
    return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}

bool FingerPrinter::CB_Exit()
{
    printf("ERROR! code=%02X, desc=%s\n", g_error_code,  PS_GetErrorDesc());
    return true;
}
/*************************************************************************************************/
/************************    FingerPrint_CallBack definition        ******************************/
/*************************************************************************************************/



bool FingerPrinter::search_CALLBACK(void* lpvoid, uchar bufferID, int startPageID, int count, int* pPageID, int* pScore)
{
    FingerPrinter* exe_ptr = (FingerPrinter*)(lpvoid);
    return exe_ptr->CB_Search(bufferID, startPageID, count, pPageID, pScore);
}	

bool FingerPrinter::getimage_CALLBACK(void* lpvoid)
{
    FingerPrinter* exe_ptr = (FingerPrinter*)(lpvoid);
    return exe_ptr->CB_GetImage();
}	

bool FingerPrinter::genchar_CALLBACK(void* lpvoid, uchar bufferID)
{
    FingerPrinter* exe_ptr = (FingerPrinter*)(lpvoid);
    return exe_ptr->CB_GenChar(bufferID);
}	

bool FingerPrinter::exit_CALLBACK(void* lpvoid)
{
    FingerPrinter* exe_ptr = (FingerPrinter*)(lpvoid);
    return exe_ptr->CB_Exit();
}

void FingerPrinter::Test1()
{
	printf("______________________IN____CB___SEARCH____________________\n");

	//mu.lock();
	
	if (Get_getimage_CallBack (this,getimage_CALLBACK)){
		if(Get_genchar_CallBack(this,genchar_CALLBACK,1)){
			int pageID = 0;
			int score = 0;
			if (! Get_search_CallBack(this,search_CALLBACK, 1, 0, 300, &pageID, &score))
				Get_exit_CallBack(this,exit_CALLBACK);
			else{
				//lockerControl();
				pinMode (SWITCH,OUTPUT);
				printf("FBI open the door!\n");
				digitalWrite (SWITCH,LOW);//set it initially as high, is closed

				delay(1000);
				digitalWrite (SWITCH,HIGH);
				printf("Your are No.%d pleace come in\n", pageID);
			}
		}
		else
			Get_exit_CallBack(this,exit_CALLBACK);

	//mu.unlock();	
  }	
}

void FingerPrinter::Test2()
{
	printf("______________________IN___ADDING___________________\n");
    if(!digitalRead(key_pin))
    {
      cb_add();
      delay(50);
      printf("666\n");
    }
 
}

void FingerPrinter::registerCallback(FingerPrint_CallBack* cb)
{
    fp_callback_ptr = cb;
}
  
void FingerPrinter::unRegisterCallback()
{
    fp_callback_ptr = nullptr;
}


void FingerPrinter::start()
{
  if (nullptr != thread_1 && nullptr != thread_2 ) {
	  // already running
	  return;
  }

  if (!readConfig())
    exit(1);

  if (g_verbose == 1)
    printConfig();

  if (-1 == wiringPiSetup()) {
    printf("wiringPi setup failed!\n");
    exit(0);
    }

  pinMode(g_config.detect_pin, INPUT);

  if((g_fd = serialOpen(g_config.serial, g_config.baudrate)) < 0)	
  {
    fprintf(stderr,"Unable to open serial device: %s\n", strerror(errno));
    exit(0);
  }
  
  atexit(atExitFunc);

  PS_Setup(g_config.address, g_config.password) ||  PS_Exit();
   
  // 7.dispose main funtion and analysis general commands (argv[1])，

  pinMode(key_pin,INPUT);
  pullUpDnControl(key_pin,PUD_UP);
  
  //wiringPiISR(key, INT_EDGE_FALLING, &);
  //int a1 = wiringPiISR(SWITCH, INT_EDGE_RISING, thread_1);
  //int b2 = wiringPiISR(key_pin, INT_EDGE_FALLING, thread_2);
  //a1 = b2;
  
  thread_1 = new thread(exec1,this);
  //thread_2 = new thread(exec2,this);

}

void FingerPrinter::stop()
{
 
  if (nullptr != thread_1 && nullptr != thread_2)
  {
    thread_1->join();
    thread_2->join();
    delete thread_1;
    delete thread_2;
    thread_1 = nullptr;
    thread_2 = nullptr;
    
#ifdef DEBUG
    fprintf(stderr,"DAQ thread stopped.\n");
#endif	
  }
}

/******************************************************************************
* FingerOrinter main Functions
******************************************************************************/

void FingerPrinter::lockerControl()
{
  pinMode (SWITCH,OUTPUT);
  printf("FBI open The Door!\n");
  digitalWrite (SWITCH,LOW);//set it initially as high, is closed

  delay(1000);
  digitalWrite (SWITCH,HIGH);

  pinMode(g_config.detect_pin, INPUT);
}

bool FingerPrinter::PS_Setup(uint chipAddr, uint password) {

  g_as608.chip_addr = chipAddr;
  g_as608.password  = password;
  if (g_verbose == 1)
    printf("-------------------------Initializing-------------------------\n");
  //verify the password 
  if (g_as608.has_password) {
    if (!PS_VfyPwd(password))
     return false;
  }

  // obtain the size of datapack and the baud rate ec.
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


bool FingerPrinter::PS_GetImage() {
  int size = GenOrder(0x01, "");

  // send the command pack 
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) && Check(g_reply, 12));

}


bool FingerPrinter::PS_GenChar(uchar bufferID) {
  int size = GenOrder(0x02, "%d", bufferID);
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}

bool FingerPrinter::PS_Match(int* pScore) {
  int size = GenOrder(0x03, "");
  SendOrder(g_order, size);

  //  receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 14) && 
          Check(g_reply, 14) &&
          Merge((uint*)pScore, g_reply+10, 2));
}



bool FingerPrinter::PS_RegModel() {
  int size = GenOrder(0x05, "");
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) &&
      Check(g_reply, 12));
}


bool FingerPrinter::PS_StoreChar(uchar bufferID, int pageID) {
  int size = GenOrder(0x06, "%d%2d", bufferID, pageID);
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) && 
        Check(g_reply, 12));
}

bool FingerPrinter::PS_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore) {

  int size = GenOrder(0x04, "%d%2d%2d", bufferID, startPageID, count);
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return ( RecvReply(g_reply, 16) && 
           Check(g_reply, 16) && 
           (Merge((uint*)pPageID, g_reply+10, 2)) &&  // assign value to pageID, return true
           (Merge((uint*)pScore,  g_reply+12, 2))     // assign value to score, return true
        );
}

bool FingerPrinter::PS_DetectFinger() {
  return digitalRead(g_as608.detect_pin) == HIGH;
}

// Block until a finger is detected, the longest block wait_time is milliseconds
bool FingerPrinter::waitUntilDetectFinger(int wait_time) {
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

bool FingerPrinter::waitUntilNotDetectFinger(int wait_time) {
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

bool FingerPrinter::PS_Empty() {
  int size = GenOrder(0x0d, "");
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum 
  return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}

//The program exited because the function in as608.h failed to execute
bool FingerPrinter::PS_Exit()
{
  printf("ERROR! code=%02X, desc=%s\n", g_error_code,  PS_GetErrorDesc());
  return true;
}

// obtain the defination of error code of g_error_code and assign value to g_error_desc
char* FingerPrinter::PS_GetErrorDesc()
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

/******************************************************************************
* Helper Functions
******************************************************************************/

//split unsigned int val into seperate int val
void FingerPrinter::Split(uint num, uchar* buf, int count) {
  for (int i = 0; i < count; ++i) {
    *buf++ = (num & 0xff << 8*(count-i-1)) >> 8*(count-i-1);
  }
}


bool FingerPrinter::Merge(uint* num, const uchar* startAddr, int count) {
  *num = 0;
  for (int i = 0; i < count; ++i)
    *num += (int)(startAddr[i]) << (8*(count-i-1)); 

  return true;
}


void FingerPrinter::PrintBuf(const uchar* buf, int size) {
  for (int i = 0; i < size; ++i) {
    printf("%02X ", buf[i]);
  }
  printf("\n");
}


int FingerPrinter::Calibrate(const uchar* buf, int size) {
  int count = 0;
  for (int i = 6; i < size - 2; ++i) {
    count += buf[i];
  }

  return count;
}


bool FingerPrinter::Check(const uchar* buf, int size) {
  //init check sum
  int count_ = 0; 
  Merge((uint*)&count_, buf+size-2, 2); 

  // sum of check
  int count = Calibrate(buf, size);
  
  //avoid 0x00
  return (buf[9] == 0x00 && count_ == count && buf[0] != 0x00);
}


int FingerPrinter::SendOrder(const uchar* order, int size) {
  // print detials info
  if (g_verbose == 1) {
    printf("sent: ");
    PrintBuf(order, size);
  }
  int ret = write(g_fd, order, size);
  return ret;
}


bool FingerPrinter::RecvReply(uchar* hex, int size) {
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
    usleep(10); 
    timeCount++;
    if (timeCount > 300000) { //if delay >3s
      break;
    }
  }

  if (g_verbose == 1) {
    printf("recv: ");
    PrintBuf(hex, availCount);
  }

  // if no data return within required time
  if (availCount < size) {
    g_error_code = 0xff;
    return false;
  }

  g_error_code = hex[9];
  return true;
}


void FingerPrinter::PrintProcess(int done, int all) {
  // progress bar showing as 0~100%，50 chars
  char process[64] = { 0 };
  double stepSize = (double)all / 100;
  int doneStep = done / stepSize;
  
  // update the progress bar if the step is even 
  // 50 chars used to display 100 the value of prgress 
  for (int i = 0; i < doneStep / 2 - 1; ++i)
    process[i] = '=';
  process[doneStep/2 - 1] = '>';

  printf("\rProcess:[%-50s]%d%% ", process, doneStep);
  if (done < all)
    fflush(stdout);
  else
    printf("\n");
}


bool FingerPrinter::RecvPacket(uchar* pData, int validDataSize) {
  if (g_as608.packet_size <= 0)
    return false;
  int realPacketSize = 11 + g_as608.packet_size; // how big every datapacks are in reality
  int realDataSize = validDataSize * realPacketSize / g_as608.packet_size;  // how big datapacks received totally

  uchar readBufTmp[8] = { 0 };  // read 8 chars every time at most, implus to readBuf
  uchar* readBuf = (uchar*)malloc(realPacketSize); // receiving full chars of realPacketSize means receiving a whole datapack, superadd into pData

  int availSize      = 0;
  int readSize       = 0;
  int readCount      = 0;
  //unused variable
  //int readBufTmpSize = 0;
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

      // export detailed message or not 
      if (g_verbose == 1) {
        printf("%2d%% RecvData: %d  count=%4d/%-4d  ", 
            (int)((double)readCount/realDataSize*100), readSize, readCount, realDataSize);
        PrintBuf(readBufTmp, readSize);
      }
      else if (g_verbose == 0){ // dispaly progress bar acquiescently
        PrintProcess(readCount, realDataSize);
      }
      else {
        // show nothing
      }

      // receive a whole datapack(139 bytes)
      if (readBufSize >= realPacketSize) {
        int count_ = 0;
        Merge((uint*)&count_, readBuf+realPacketSize-2, 2);
        if (Calibrate(readBuf, realPacketSize) != count_) {
          free(readBuf);
          g_error_code = 0x01;
          return false;
        }

        memcpy(pData+offset, readBuf+9, g_as608.packet_size);
        offset += g_as608.packet_size;
        readBufSize = 0;

        // received and terminate the pack 
        if (readBuf[6] == 0x08) {
          break;
        }
      }

      // received the effective chars of validDataSize, but haven't receive the ending datapack 
      if (readCount >= realDataSize) {
        free(readBuf);
        g_error_code = 0xC4;
        return false;
      }
    } // end outer if

    usleep(10); // wait for 10 um 
    timeCount++;
    if (timeCount > 300000) {   // block up to 3 seconds 
      break;
    }
  } // end while

  free(readBuf);

  // haven't recive the appointed size fa data within the maximum blocking time, return false
  if (readCount < realDataSize) {
    g_error_code = 0xC3;
    return false;
  }
  
  g_error_code = 0x00;
  return true; 
}



bool FingerPrinter::SendPacket(uchar* pData, int validDataSize) {
  if (g_as608.packet_size <= 0)
    return false;
  if (validDataSize % g_as608.packet_size != 0) {
    g_error_code = 0xC8;
    return false;
  }
  int realPacketSize = 11 + g_as608.packet_size; // the size of every datapack in reality 
  int realDataSize = validDataSize * realPacketSize / g_as608.packet_size;  // the total size of data to be sent 

  // construct datapack 
  uchar* writeBuf = (uchar*)malloc(realPacketSize);
  writeBuf[0] = 0xef;  // the begining of datapack 
  writeBuf[1] = 0x01;  // the begining of datapack
  Split(g_as608.chip_addr, writeBuf+2, 4);  // the address of chip 
  Split(g_as608.packet_size+2, writeBuf+7, 2);  // the length of datapack

  int offset     = 0;  // valid data sent
  int writeCount = 0;  // factual data sent

  while (true) {
    // fill the data zone 
    memcpy(writeBuf+9, pData+offset, g_as608.packet_size);

    // datapack symbol 
    if (offset + g_as608.packet_size < (uint)validDataSize)
      writeBuf[6] = 0x02;  // ending pack(the final datapack)
    else
      writeBuf[6] = 0x08;  // general datapack 

    // verify the sum
    Split(Calibrate(writeBuf, realPacketSize), writeBuf+realPacketSize-2, 2); 

    // send datapack
    write(g_fd, writeBuf, realPacketSize);

    offset     += g_as608.packet_size;
    writeCount += realPacketSize;

    // export detailed message or not 
    if (g_verbose == 1) {
      printf("%2d%% SentData: %d  count=%4d/%-4d  ", 
          (int)((double)writeCount/realDataSize*100), realPacketSize, writeCount, realDataSize);
      PrintBuf(writeBuf, realPacketSize);
    }
    else if (g_verbose == 0) {
      // dispaly the prpgress bar 
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

int FingerPrinter::GenOrder(uchar orderCode, const char* fmt, ...) {
  g_order[0] = 0xef;        // the begining of pack，0xef
  g_order[1] = 0x01;
  Split(g_as608.chip_addr, g_order+2, 4);    // the address of chip, PS_Setup() is needed to initial setup 
  g_order[6] = 0x01;        // the symbol of pack，0x01 stands for the command pack，0x02 datapack ，0x08 is ending(final) pack 
  g_order[9] = orderCode;   // command

  // calculate the total number of parameters 
  int count = 0;
  for (const char* p = fmt; *p; ++p) {
    if (*p == '%')
      count++;
  }

  // fmt==""
  if (count == 0) { 
    Split(0x03, g_order+7,  2);  // the length of pack 
    Split(Calibrate(g_order, 0x0c), g_order+10, 2);  // verify the sum(the length pf pack should be 12(0x0c) if without parameters
    return 0x0c;
  }
  else {
    va_list ap;
    va_start(ap, fmt);

    uint  uintVal;
    uchar ucharVal;
    uchar* strVal;

    int offset = 10;  // the offset of pin g_order
    int width = 1;    // the width of modifier in fmt，such as:%4d, %32s

    // dispose the uncertain parameters 
    for (; *fmt; ++fmt) {
      width = 1;
      if (*fmt == '%') {
        const char* tmp = fmt+1;

        // obtain the width, such as: %4u, %32s
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
          case 'c': // equal to "%d"
            if (width > 1)
              return 0;
            ucharVal = va_arg(ap, int);
            g_order[offset] = ucharVal;
            break;
          case 's':
            strVal = va_arg(ap, uchar*);
            memcpy(g_order+offset, strVal, width);
            break;
          default:
            return 0;
        } // end switch 

        offset += width;
      } // end if (*p == '%')
    } // end for 

    Split(offset+2-9, g_order+7, 2);  // the length of pack
    Split(Calibrate(g_order, offset+2), g_order+offset, 2); // verify the sum
    
    va_end(ap);
    return offset + 2;
  } // end else (count != 0)
}

bool FingerPrinter::PS_VfyPwd(uint pwd) { 
  int size = GenOrder(0x13, "%4d", pwd); 
  SendOrder(g_order, size);

  // 接收数据，核对确认码和检校和
  return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}

bool FingerPrinter::PS_ReadSysPara() {
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

bool FingerPrinter::readConfig() {
  FILE* fp;

  // 获取用户主目录
  char filename[256] = { 0 };
  sprintf(filename, "%s/.fpconfig", getenv("HOME"));
  
  // 主目录下的配置文件
  if (access(filename, F_OK) == 0) { 
    trimSpaceInFile(filename);
    fp = fopen(filename, "r");
  }
  else {
    // 如果配置文件不存在，就在主目录下创建配置文件，并写入默认配置
    // 设置默认值
    g_config.address = 0xffffffff;
    g_config.password= 0x00000000;
    g_config.has_password = 0;
    g_config.baudrate = 9600;
    g_config.detect_pin = 1; 
    strcpy(g_config.serial, "/dev/ttyAMA0");

    writeConfig();

    printf("Please config the address and password in \"~/.fpconfig\"\n");
    printf("  fp cfgaddr   0x[address]\n");
    printf("  fp cfgpwd    0x[password]\n");
    printf("  fp cfgserial [serialFile]\n");
    printf("  fp cfgbaud   [rate]\n");
    printf("  fp cfgpin    [GPIO_pin]\n");
    return false;
  }

  char key[16] = { 0 };
  char value[16] = { 0 };
  char line[32] = { 0 };

  char *tmp;
  while (!feof(fp)) {
    fgets(line, 32, fp);
    
    // 分割字符串，得到key和value
    if (tmp = strtok(line, "="))
      trim(tmp, key);
    else
      continue;
    if (tmp = strtok(NULL, "="))
      trim(tmp, value);
    else
      continue;
    while (!tmp)
      tmp = strtok(NULL, "=");

    // 如果数值以 0x 开头
    int offset = 0;
    if (value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
      offset = 2;

    if (strcmp(key, "address") == 0) {
      g_config.address = toUInt(value+offset);
    }
    else if (strcmp(key, "password") == 0) {
      if (strcmp(value, "none") == 0 || strcmp(value, "false") == 0) {
        g_config.has_password = 0; // 无密码
      }
      else {
        g_config.has_password = 1; // 有密码
        g_config.password = toUInt(value+offset);
      }
    }
    else if (strcmp(key, "serial") == 0) {
      int len = strlen(value);
      if (value[len-1] == '\n')
        value[len-1] = 0;
      strcpy(g_config.serial, value);
    }
    else if (strcmp(key, "baudrate") == 0) {
      g_config.baudrate = toInt(value);
    }
    else if (strcmp(key, "detect_pin") == 0) {
      g_config.detect_pin = toInt(value);
    }
    else {
      printf("Unknown key:%s\n", key);
      fclose(fp);
      return false;
    }

  } // end while(!feof(fp))

  asyncConfig();

  fclose(fp);
  return true;
}

// 打印配置文件内容到屏幕上
void FingerPrinter::printConfig() {
  printf("address=%08x\n", g_config.address);
  if (g_config.has_password)
    printf("password=%08x\n", g_config.password);
  else
    printf("password=none(no password)\n");
  printf("serial_file=%s\n",   g_config.serial);
  printf("baudrate=%d\n",   g_config.baudrate);
  printf("detect_pin=%d\n",   g_config.detect_pin);
}

// 同步g_config变量内容和其他变量内容
void FingerPrinter::asyncConfig() {
  g_as608.detect_pin   = g_config.detect_pin;
  g_as608.has_password = g_config.has_password;
  g_as608.password     = g_config.password;
  g_as608.chip_addr    = g_config.address;
  g_as608.baud_rate    = g_config.baudrate;
}

// 程序退出时执行的工作，关闭串口等
void FingerPrinter::atExitFunc() {
  if (g_verbose == 1)
    printf("Exit\n");
  if (g_fd > 0)
    serialClose(g_fd); 
}

/*
 * 写配置文件
*/
bool FingerPrinter::writeConfig() {
  // 获取用户主目录
  char filename[256] = { 0 };
  sprintf(filename, "%s/.fpconfig", getenv("HOME"));
  
  FILE* fp = fp = fopen(filename, "w+");
  if (!fp) {
    printf("Write config file error!\n");
    exit(0);
  }

  fprintf(fp, "address=0x%08x\n", g_config.address);
  if (g_config.has_password)
    fprintf(fp, "password=0x%08x\n", g_config.password);
  else
    fprintf(fp, "password=none\n");
  fprintf(fp, "baudrate=%d\n", g_config.baudrate);
  fprintf(fp, "detect_pin=%d\n", g_config.detect_pin);
  fprintf(fp, "serial=%s\n", g_config.serial);

  fclose(fp);
}
void FingerPrinter::trimSpaceInFile(const char* filename) {
  FILE* fp = fopen(filename, "r");
  if (!fp)
    return;
  //buffer for input except white space and '\n'
  char lineBuf[64] = { 0 };
  char writeBuf[1024] = { 0 };
  
  int offset = 0;
  
  while (!feof(fp)) {
    fgets(lineBuf, 64, fp);
    if (feof(fp))
      break;
      
    for (int i = 0, len = strlen(lineBuf); i < len; ++i) {
      if (lineBuf[i] != ' ' && lineBuf[i] != '\t') {
        if (lineBuf[i] == '\n' && offset > 0 && writeBuf[offset-1] == '\n')
          continue;
        writeBuf[offset++] = lineBuf[i];
      }
    }
    
  }

  fclose(fp);
  //overwrtie file
  fp = fopen(filename, "w+");
  if (!fp)
    return;
  fwrite(writeBuf, 1, strlen(writeBuf), fp);
  fclose(fp);
}


void FingerPrinter::trim(const char* strIn, char* strOut) {
  int i = 0;
  int j = strlen(strIn) - 1;
  //detect space
  while (strIn[i] == ' ')
    ++i;
  //detect '\n'
  while (strIn[j] == ' ' || strIn[j] == '\n')
    --j;
  strncpy(strOut, strIn+i, j-i+1);
  strOut[j-i+1] = 0;
}


int FingerPrinter::toInt(const char* str) {
  int ret = 0;
  sscanf(str, "%d", &ret);
  return ret;
}

unsigned int FingerPrinter::toUInt(const char* str) {
  unsigned int ret = 0;
  if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    sscanf(str+2, "%x", &ret);
  else
    sscanf(str, "%x", &ret);

  return ret;
}
