#include "Executive.h"
#include <iostream>


#define SWITCH 7


int   g_fd;          // file char, return when serial opened
int   g_verbose;     // the details level of output
char  g_error_desc[128]; // error info
uchar g_error_code;      // the module return code when function return false

uchar g_order[64] = { 0 }; // the instruction package 
uchar g_reply[64] = { 0 }; // the reply package
//split unsigned int val into seperate int val

void Executive::trimSpaceInFile(const char* filename) {
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


void Executive::trim(const char* strIn, char* strOut) {
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


int Executive::toInt(const char* str) {
  int ret = 0;
  sscanf(str, "%d", &ret);
  return ret;
}

unsigned int Executive::toUInt(const char* str) {
  unsigned int ret = 0;
  if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    sscanf(str+2, "%x", &ret);
  else
    sscanf(str, "%x", &ret);

  return ret;
}


Executive::Executive()
{
  /**< Getter function pointer of type int void (*summer)(int,int). */
  //a1->register_callbacks();
  //a1->callback_sum(1,4);
  //call_cpp_sum_function();
  //register_handler( &Executive::call_cpp_sum_function );

}
	
Executive::~Executive()
{
  //a1 = nullptr;
  //delete a1;
	
}
void Executive::lockerControl()
{
  pinMode (SWITCH,OUTPUT);
  printf("FBI open The Door!\n");
  digitalWrite (SWITCH,LOW);//set it initially as high, is closed

  delay(1000);
  digitalWrite (SWITCH,HIGH);

  pinMode(g_config.detect_pin, INPUT);
}

void Executive::run(int argc, char *argv[])
{
	// 1.read configuration files, obtain the chip address and the signal password 
	if (!readConfig())
		exit(1);

	// 2.priorify analysis content such as parameters options and allocate local files 
	priorAnalyseArgv(argc, argv);

	if (g_verbose == 1)
		printConfig();

	// 3.initialize wiringPi library
	if (-1 == wiringPiSetup()) {
		printf("wiringPi setup failed!\n");
    exit(0);
	}

	// 4.Detect if there is a GPIO port on which a finger is placed, and set it to input mode
	pinMode(g_config.detect_pin, INPUT);

  //test the configeration
  //printConfig();

	// 5.Open serial port
	if((g_fd = serialOpen(g_config.serial, g_config.baudrate)) < 0)	{
    std::cout<<"serial "<<g_config.serial<<" baudrate "<<g_config.baudrate<<"\n";
		fprintf(stderr,"Unable to open serial device: %s\n", strerror(errno));
    exit(0);
	}

  
	// 6.Register the exit function (print some information, close the serial port, etc.)
	atexit(atExitFunc);


	// 7.initialize AS608 module 
	 PS_Setup(g_config.address, g_config.password) ||  PS_Exit();

	// 8.dispose main funtion and analysis general commands (argv[1])，
	analyseArgv(argc, argv);
  /*
   *
  AD7705settings s;
  s.channel = AD7705settings::AIN1;
  s.samplingRate = AD7705settings::FS50HZ;
  
  Executive m_car();
  FINGERPRINT_SampleCallback fp_SAMPLE_cb;
  m_car.registerCallback(&fp_SAMPLE_cb);
  m_car.start();
  getchar();
  m_car.stop();
  return 0;
  * 
  * */
}
	
//The program exited because the function in as608.h failed to execute
bool Executive::PS_Exit()
{
  printf("ERROR! code=%02X, desc=%s\n", g_error_code,  PS_GetErrorDesc());
  exit(2);
  return true;
}

// The work performed when the program exits, closing the serial port, etc.
void Executive::atExitFunc()
{
  if (g_verbose == 1)
	printf("Exit\n");
  if (g_fd > 0)
	serialClose(g_fd); 
}



// Check if the number of parameters is correct
bool Executive::checkArgc(int argcNum) {
  if (argcNum == g_argc)
    return true;
  else if (argcNum == 2)
    printf("ERROR! \"%s\" accept no parameter\n", g_command);
  else if (argcNum == 3)
    printf("ERROR! \"%s\" accept 1 parameter\n", g_command);
  else if (argcNum > 3)
    printf("ERROR! \"%s\" accept %d parameters\n", g_command, argcNum);

  exit(1);
}

// matching argv[1], that is g_command
bool Executive::match(const char* str) {
  return strcmp(str, g_command) == 0;
}

// Commands that need to be parsed first, such as configuration file modification, option parsing, etc.
// No need to communicate with the module
void Executive::priorAnalyseArgv(int argc, char* argv[]) {
  if (argc < 2) {
    printUsage();
    exit(1);
  }

  // check options  -v -h
  for (int i = 0; i < argc; ++i) {
    if (strcmp(argv[i], "-h") == 0) {
      printUsage();
      g_option_count++;
      exit(0);
    }
    else if (strcmp(argv[i], "-v") == 0) {
      g_verbose = 1;
      g_option_count++;
    }
  }
  
  g_argc = argc - g_option_count;
  strcpy(g_command, argv[1]);

  if (match("cfg")) {
    printConfig();
    exit(0);
  }

  // allocate communication address
  else if (match("cfgaddr")) {
    checkArgc(3);
    g_config.address =  toUInt(argv[2]);
    writeConfig();
    exit(0);
  }

  // allocate communication password
  else if (match("cfgpwd")) {
    checkArgc(3);
    g_config.password =  toUInt(argv[2]);
    g_config.has_password = 1;
    writeConfig();
    exit(0);
  }

  // allocate serial port number
  else if (match("cfgserial")) {
    checkArgc(3);
    strcpy(g_config.serial, argv[2]);
    writeConfig();
    exit(0);
  }

  else if (match("cfgbaud")) {
    checkArgc(3);
    g_config.baudrate =  toInt(argv[2]);
    writeConfig();
    exit(0);
  }

  else if (match("cfgpin")) {
    checkArgc(3);
    g_config.detect_pin =  toInt(argv[2]);
    writeConfig();
    exit(0);
  }
}

// Block until a finger is detected, the longest block wait_time is milliseconds
bool Executive::waitUntilDetectFinger(int wait_time) {
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

bool Executive::waitUntilNotDetectFinger(int wait_time) {
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


// The main processing function, parsing the command
void Executive::analyseArgv(int argc, char* argv[]) {
  if (match("door")) {
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    checkArgc(2);

    printf("Please put your finger on the module.\n");
    delay(1000);
     PS_GetImage() || PS_Exit();
     PS_GenChar(1) || PS_Exit();

    int pageID = 0, score = 0;
    if (! PS_Search(1, 0, 300, &pageID, &score))
      PS_Exit();
    else
    {   
        //lockerControl();
        pinMode (SWITCH,OUTPUT);
        printf("FBI open the door!\n");
        digitalWrite (SWITCH,LOW);//set it initially as high, is closed
        
        delay(1000);
        digitalWrite (SWITCH,HIGH);

        pinMode(g_config.detect_pin, INPUT);
    }

    //printf("Matched! pageID=%d score=%d\n", pageID, score);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  }
  else if (match("doortest")) {
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  checkArgc(2);
 
  //lockerControl();
  pinMode (SWITCH,OUTPUT);
  printf("FBI open the door!\n");
  digitalWrite (SWITCH,LOW);//set it initially as high, is closed
  
  delay(3000);
  digitalWrite (SWITCH,HIGH);

  pinMode(g_config.detect_pin, INPUT);

  //printf("Matched! pageID=%d score=%d\n", pageID, score);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  }
  
  else if (match("add")) {
    checkArgc(3);
    printf("Please put your finger on the module.\n");
    if (waitUntilDetectFinger(5000)) {
      delay(500);
       PS_GetImage() || PS_Exit();
       PS_GenChar(1) || PS_Exit();
    }
    else {
      printf("Error: Didn't detect finger!\n");
      exit(1);
    }

    // Determine if the user has raised their finger，
    printf("Ok.\nPlease raise your finger!\n");
    if (waitUntilNotDetectFinger(5000)) {
      delay(100);
      printf("Ok.\nPlease put your finger again!\n");
      // input fingerprint for sencond time
      if (waitUntilDetectFinger(5000)) {
        delay(500);
         PS_GetImage() || PS_Exit();
         PS_GenChar(2) || PS_Exit();
      }
      else {
        printf("Error: Didn't detect finger!\n");
        exit(1);
      }
    }
    else {
      printf("Error! Didn't raise your finger\n");
      exit(1);
    }

    int score = 0;
    if ( PS_Match(&score)) {
      printf("Matched! score=%d\n", score);
    }
    else {
      printf("Not matched, raise your finger and put it on again.\n");
      exit(1);
    }
    
    if (g_error_code != 0x00)
      PS_Exit();

    // Merge feature files
     PS_RegModel() || PS_Exit();
     PS_StoreChar(2,  toInt(argv[2])) || PS_Exit();

    printf("OK! New fingerprint saved to pageID=%d\n",  toInt(argv[2]));
  }

  else if (match("enroll")) {
    checkArgc(2);

    int count = 0;
    printf("Please put your finger on the moudle\n");
    while (digitalRead(g_as608.detect_pin) == LOW) {
      delay(1);
      if ((count++) > 5000) {
        printf("Not detected the finger!\n");
        exit(2);
      }
    }
        
    int pageID = 0;
     PS_Enroll(&pageID) || PS_Exit();

    printf("OK! New fingerprint saved to pageID=%d\n", pageID);
  }
  
  else if (match("info")) {
    checkArgc(2);
     PS_GetAllInfo() || PS_Exit();

    printf("Product SN:        %s\n", g_as608.product_sn);
    printf("Software version:  %s\n", g_as608.software_version);
    printf("Manufacture:       %s\n", g_as608.manufacture);
    printf("Sensor model:      %d\n", g_as608.model);
    printf("Sensor name:       %s\n", g_as608.sensor_name);
    printf("Status register:   %d\n", g_as608.status);
    printf("Database capacity: %d\n", g_as608.capacity);
    printf("Secure level(1-5): %d\n", g_as608.secure_level);
    printf("Device address:    0x%08x\n", g_as608.chip_addr);
    printf("Packet size:       %u bytes\n", g_as608.packet_size);
    printf("Baud rate:         %d\n", g_as608.baud_rate);
  }

  else if (match("empty")) {
    checkArgc(2);
    printf("Confirm to empty database: (Y/n)? ");
    fflush(stdout);
    char cmd;
    scanf("%c", &cmd);
    if (cmd == 'n' || cmd ==  'N') {
      printf("Canceled!\n");
      exit(3);
    }

     PS_Empty() || PS_Exit();
    printf("OK!\n");
  }

  else if (match("delete")) {
    int startPageID = 0;
    int count = 0;

    // Determine the number of parameters
    if (g_argc == 3) {
      startPageID =  toInt(argv[2]);
      count = 1;
      printf("Confirm to delete fingerprint %d: (Y/n)? ", startPageID);
    }
    else if (argc == 4) {
      startPageID =  toInt(argv[2]);
      count =  toInt(argv[3]);
      printf("Confirm to delete fingerprint %d-%d: (Y/n)? ", startPageID, startPageID+count-1);
    }
    else {
      printf("Command \"delete\" accept 1 or 2 parameter\n");
      printf("  Usage: fp delete startPageID [count]\n");
      exit(1);
    }

    // ask for continuing or not 
    fflush(stdout);
    char cmd = getchar();
    if (cmd == 'n' || cmd ==  'N') {
      printf("Canceled!\n");
      exit(0);
    }

     PS_DeleteChar(startPageID, count) || PS_Exit();
    printf("OK!\n");
  }

  else if (match("count")) {
    checkArgc(2);
    int count = 0;
     PS_ValidTempleteNum(&count) || PS_Exit();
    printf("%d\n", count);
  }

  else if (match("search")) {
    checkArgc(2);

    printf("Please put your finger on the module.\n");
     PS_GetImage() || PS_Exit();
     PS_GenChar(1) || PS_Exit();

    int pageID = 0, score = 0;
    if (! PS_Search(1, 0, 300, &pageID, &score))
      PS_Exit();
    else
      printf("Matched! pageID=%d score=%d\n", pageID, score);
  }

  else if (match("hsearch")) {  // high speed search
    checkArgc(2);

    printf("Please put your finger on the module.\n");
     PS_GetImage() || PS_Exit();
     PS_GenChar(1) || PS_Exit();

    int pageID = 0, score = 0;
    if (! PS_HighSpeedSearch(1, 0, 300, &pageID, &score))
      PS_Exit();
    else
      printf("Matched! pageID=%d score=%d\n", pageID, score);
  }

  else if (match("identify")) {
    checkArgc(2);
    int pageID = 0;
    int score = 0;
    
    // check the finger exist or not 
    int count = 0;
    printf("Please put your finger on the moudle\n");
    while (digitalRead(g_as608.detect_pin) == LOW) {
      delay(1);
      if ((count++) > 5000) {
        printf("Not detected the finger!\n");
        exit(2);
      }
    }

     PS_Identify(&pageID, &score) || PS_Exit();
    printf("Matched! pageID=%d score=%d\n", pageID, score);
  }

  // list fingerprints
  else if (match("list")) {
    checkArgc(2);
    int indexList[512] = { 0 };
     PS_ReadIndexTable(indexList, 512) ||  PS_Exit();

    int i = 0;
    for (i = 0; i < 300; ++i) {
      if (indexList[i] == -1)
        break;
      printf("%d\n", indexList[i]);
    }
    if (i == 0) {
      printf("The database is empty!\n");
    }
  }

  else if (match("getimage")) {
    checkArgc(2);
    printf("Please put your finger on the module.\n");
     PS_GetImage() || PS_Exit();
    printf("OK!\n");
  }

  else if (match("genchar")) {
    checkArgc(3);
     PS_GenChar( toInt(argv[2])) || PS_Exit();
    printf("OK!\n");
  }

  else if (match("regmodel")) {
    checkArgc(2);
     PS_RegModel() ||  PS_Exit();
    printf("OK!\n");
  }

  else if (match("storechar")) {
    checkArgc(4);
     PS_StoreChar( toInt(argv[2]),  toInt(argv[3])) ||  PS_Exit();
    printf("OK! Stored in pageID=%d\n",  toInt(argv[3]));
  }

  else if (match("loadchar")) {
    checkArgc(4);
     PS_LoadChar( toInt(argv[2]),  toInt(argv[3])) || PS_Exit();
    printf("OK! Loaded to bufferID=%d\n",  toInt(argv[2]));
  }

  else if (match("match")) {
    checkArgc(2);
    int score = 0;
     PS_Match(&score) || PS_Exit();
    printf("Matched! Score=%d\n", score);
  }

  else if (match("random")) {
    checkArgc(2);
    uint randomNum = 0; // must be unsigned int，Otherwise it may overflow
     PS_GetRandomCode(&randomNum) ||  PS_Exit();
    printf("%u\n", randomNum);
  }
  
  else if (match("upchar")) {
    checkArgc(4);
     PS_UpChar( toInt(argv[2]), argv[3]) ||  PS_Exit();
    printf("OK!\n");
  }

  else if (match("downchar")) {
    checkArgc(4);
     PS_DownChar( toInt(argv[2]), argv[3]) || PS_Exit();
    printf("OK!\n");
  }

  else if (match("upimage")) {
    checkArgc(3);
     PS_UpImage(argv[2]) || PS_Exit();
    printf("OK!\n");
  }

  else if (match("downimage")) {
    checkArgc(3);
     PS_DownImage(argv[2]) ||  PS_Exit();
    printf("OK!\n");
  }

  else if (match("flush")) {
    checkArgc(2);
     PS_Flush();
    printf("OK!\n");
  }

  else if (match("writereg")) {
    checkArgc(4);
     PS_WriteReg( toInt(argv[2]),  toInt(argv[3])) ||  PS_Exit();
    printf("OK!\n");
  }

  else if (match("baudrate")) {
    if (g_argc == 2) {
      printf("%d\n", g_as608.baud_rate);
      exit(0);
    }
    else if (g_argc == 3) {
       PS_SetBaudRate( toInt(argv[2])) ||  PS_Exit();
    }
    else {
      printf("Command \"baudrate\" accept 1 parameter at most\n");
      exit(1);
    }

    printf("OK!\n");
  }

  else if (match("level")) {
    if (g_argc == 2) {
      printf("%d\n", g_as608.secure_level);
      exit(0);
    }
    else if (g_argc == 3) {
       PS_SetSecureLevel( toInt(argv[2])) ||  PS_Exit();
    }
    else {
      printf("Command \"level\" accept 1 parameter at most\n");
      exit(1);
    }

    printf("OK!\n");
  }

  else if (match("packetsize")) {
    if (g_argc == 2) {
      printf("%d\n", g_as608.packet_size);
      exit(0);
    }
    else if (g_argc == 3) {
       PS_SetPacketSize( toInt(argv[2])) ||  PS_Exit();
    }
    else {
      printf("Command \"packetsize\" accept 1 parameter at most\n");
      exit(1);
    }

    printf("OK!\n");
  }
  
  else if (match("address")) {
    if (g_argc == 2) {
      printf("0x%08x\n", g_as608.chip_addr);
      exit(0);
    }
    else if (g_argc == 3) {
       PS_SetChipAddr( toUInt(argv[2])) || PS_Exit();
      g_config.address =  toUInt(argv[2]);
      if (writeConfig())
        printf("New chip address is 0x%08x\n", g_as608.chip_addr);
    }
    else {
      printf("Command \"address\" accept 1 parameter at most\n");
      exit(1);
    }

    printf("OK!\n");
  }

  else if (match("setpwd")) {
    checkArgc(3);
     PS_SetPwd( toUInt(argv[2])) ||  PS_Exit();
    g_config.has_password = 1;
    g_config.password =  toUInt(argv[2]);
    if (writeConfig())
      printf("New password is 0x%08x\n", g_as608.password);
  }

  else if (match("vfypwd")) {
    checkArgc(3);
     PS_VfyPwd( toUInt(argv[2])) ||  PS_Exit();
    printf("OK!\n");
  }

  else if (match("readinf")) {
    checkArgc(3);
    uchar buf[513] = { 0 };
     PS_ReadINFpage(buf, 512) ||  PS_Exit();

    // input files
    FILE* fp = fopen(argv[2], "w+");
    if (!fp) {
      printf("Open file error\n");
      exit(1);
    }
    fwrite(buf, 1, 512, fp);
    fclose(fp);

    printf("%s\n", buf);
  }

  else if (match("writenote")) {
    char buf[128] = { 0 };
    if (g_argc == 3) {
      printf("Please write note: (max lenght is 31 !)\n");
      fgets(buf, 128, stdin);
    }
    else if (g_argc == 4) {
      memcpy(buf, argv[3], 32);
    }
    else {
      printf("Command \"writenote\" accept 1 or 2 parameter\n");
      exit(1);
    }

    if (strlen(buf) > 31) {   // If more than 31 chars are entered
      printf("Too long, continute to save the first 31 characters? (Y/n): ");
      char c = 0;
      scanf("%c", &c);
      if (c != 'y' && c != 'Y') {
        printf("Canceled!\n");
        exit(0);
      }
    }

     PS_WriteNotepad( toInt(argv[2]), (uchar*)buf, 32) ||  PS_Exit();

    printf("OK\n");
  }

  else if (match("readnote")) {
    checkArgc(3);
    uchar buf[33] = { 0 }; 
     PS_ReadNotepad( toInt(argv[2]), buf, 32) || PS_Exit();
    printf("%s\n", buf);
  }

  else {
    printf("Unknown parameter \"%s\"\n", argv[1]);
    exit(1);
  }
} // end analyseArgv


// print the contents of the configuration file to the screen
void Executive::printConfig() {
  printf("address=%08x\n", g_config.address);
  if (g_config.has_password)
    printf("password=%08x\n", g_config.password);
  else
    printf("password=none(no password)\n");
  printf("serial_file=%s\n",   g_config.serial);
  printf("baudrate=%d\n",   g_config.baudrate);
  printf("detect_pin=%d\n",   g_config.detect_pin);
}

// Synchronize g_config variable content with other variable content
void Executive::asyncConfig() {
  g_as608.detect_pin   = g_config.detect_pin;
  g_as608.has_password = g_config.has_password;
  g_as608.password     = g_config.password;
  g_as608.chip_addr    = g_config.address;
  g_as608.baud_rate    = g_config.baudrate;
}

// read configuration files
bool Executive::readConfig() {
  FILE* fp;

  // obtain main catalogue of users
  char filename[256] = { 0 };
  sprintf(filename, "%s/.fpconfig", getenv("HOME"));
  
  // configurate files under home directory
  if (access(filename, F_OK) == 0) { 
     trimSpaceInFile(filename);
    fp = fopen(filename, "r");
  }
  else {
    // if configuration files are not exist, establish configuration files under main and write default disposition
    // set up default values
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
    
    // Split the string to get the key and value
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

    // If the value starts with 0x
    int offset = 0;
    if (value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
      offset = 2;

    if (strcmp(key, "address") == 0) {
      g_config.address =  toUInt(value+offset);
    }
    else if (strcmp(key, "password") == 0) {
      if (strcmp(value, "none") == 0 || strcmp(value, "false") == 0) {
        g_config.has_password = 0; // no password
      }
      else {
        g_config.has_password = 1; // with password
        g_config.password =  toUInt(value+offset);
      }
    }
    else if (strcmp(key, "serial") == 0) {
      int len = strlen(value);
      if (value[len-1] == '\n')
        value[len-1] = 0;
      strcpy(g_config.serial, value);
    }
    else if (strcmp(key, "baudrate") == 0) {
      g_config.baudrate =  toInt(value);
    }
    else if (strcmp(key, "detect_pin") == 0) {
      g_config.detect_pin =  toInt (value);
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

//write configuration file

bool Executive::writeConfig() {
  // obtain mian catalogue of users
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


// print description of this procedure
void Executive::printUsage() {
  printf("A command line program to interact with AS608 module.\n\n");
  printf("Usage:\n  ./fp [command] [param] [option]\n");
  printf("\nAvailable Commands:\n");

  printf("-------------------------------------------------------------------------\n");
  printf("  command  | param     | description\n");
  printf("-------------------------------------------------------------------------\n");
  printf("  cfgaddr   [addr]     Config address in local config file\n");
  printf("  cfgpwd    [pwd]      Config password in local config file\n");
  printf("  cfgserial [serialFile] Config serial port in local config file. Default:/dev/ttyAMA0\n");
  printf("  cfgbaud   [rate]     Config baud rate in local config file\n");
  printf("  cfgpin    [GPIO_pin] Config GPIO pin to detect finger in local confilg file\n\n");

  printf("  add       [pID]      Add a new fingerprint to database. (Read twice) \n");
  printf("  enroll    []         Add a new fingerprint to database. (Read only once)\n");
  printf("  delete    [pID {count}]  Delete one or contiguous fingerprints.\n");
  printf("  empty     []         Empty the database.\n");
  printf("  search    []         Collect fingerprint and search in database.\n");
  printf("  identify  []         Search\n");
  printf("  count     []         Get the count of registered fingerprints.\n");
  printf("  list      []         Show the registered fingerprints list.\n");
  printf("  info      []         Show the basic parameters of the module.\n");
  printf("  random    []         Generate a random number.(0~2^32)\n\n");

  printf("  getimage  []         Collect a fingerprint and store to ImageBuffer.\n");
  printf("  upimage   [filename] Download finger image to ras-pi in ImageBuffer of the module\n");
  printf("  downimage [filename] Upload finger image to module\n");
  printf("  genchar   [cID]      Generate fingerprint feature from ImageBuffer.\n");
  printf("  match     []         Accurate comparison of CharBuffer1 and CharBuffer2\n");
  printf("                         feature files.\n");
  printf("  regmodel  []         Merge the characteristic file in CharBuffer1 and\n");
  printf("                         CharBuffer2 and then generate the template, the\n");
  printf("                         results are stored in CharBuffer1 and CharBuffer2.\n");
  printf("  storechar [cID pID]  Save the template file in CharBuffer1 or CharBuffer2\n");
  printf("                         to the flash database location with the PageID number\n");
  printf("  loadchar  [cID pID]  Reads the fingerprint template with the ID specified\n");
  printf("                         in the flash database into the template buffer,\n");
  printf("                         CharBuffer1 or CharBuffer2\n");
  printf("  readinf   [filename] Read the FLASH Info Page (512bytes), and save to file\n");
  printf("  writenote     [page {note}]   Write note loacted in pageID=page\n");
  printf("  readnote      [page]          Read note loacted in pageID=page\n");
  printf("  upchar        [cID filename]  Download feature file in CharBufferID to ras-pi\n");
  printf("  downchar      [cID filename]  Upload feature file in loacl disk to module\n");
  printf("  setpwd        [pwd]           Set password\n");
  printf("  vfypwd        [pwd]           Verify password\n");
  printf("  packetsize    [{size}]        Show or Set data packet size\n");
  printf("  baudrate      [{rate}]        Show or Set baud rate\n");
  printf("  level         [{level}]       Show or Set secure level(1~5)\n");
  printf("  address       [{addr}]        Show or Set secure level(1~5)\n");
  
  printf("\nAvaiable options:\n");
  printf("  -h    Show help\n");
  printf("  -v    Shwo details while excute the order\n");

  printf("\nUsage:\n  ./fp [command] [param] [option]\n\n");
}	






































/*************************************************************************************************/
/****************************     Call Back declairation        **********************************/
/*************************************************************************************************/
/*
int Executive::fd_Poll(int gpio_fd, int timeout)
{
  struct pollfd fdset[1];
  int nfds = 1;
  int rc;
  char buf[MAX_BUF];

  memset((void*)fdset, 0, sizeof(fdset));

  fdset[0].fd = gpio_fd;
  fdset[0].events = POLLPRI;//

  rc = poll(fdset, nfds, timeout);

  if (fdset[0].revents & POLLPRI) {//
    // dummy read
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    printf("Please put your finger on the module.\n");
    if (waitUntilDetectFinger(500)) {
      delay(500);
       PS_GetImage() || PS_Exit();
       PS_GenChar(1) || PS_Exit();
    }
    else {
      printf("Error: Didn't detect finger!\n");
      exit(1);
    }

    // Determine if the user has raised their finger，
    printf("Ok.\nPlease raise your finger!\n");
    if (waitUntilNotDetectFinger(500)) {
      delay(100);
      printf("Ok.\nPlease put your finger again!\n");
      // input fingerprint for sencond time
      if (waitUntilDetectFinger(500)) {
        delay(500);
         PS_GetImage() || PS_Exit();
         PS_GenChar(2) || PS_Exit();
      }
      else {
        printf("Error: Didn't detect finger!\n");
        exit(1);
      }
    }
    else {
      printf("Error! Didn't raise your finger\n");
      exit(1);
    }

    int score = 0;
    if ( PS_Match(&score)) {
      printf("Matched! score=%d\n", score);
    }
    else {
      printf("Not matched, raise your finger and put it on again.\n");
      exit(1);
    }
    
    if (g_error_code != 0x00)
      PS_Exit();

    // Merge feature files
     PS_RegModel() || PS_Exit();
     PS_StoreChar(2,  toInt(argv[2])) || PS_Exit();

    printf("OK! New fingerprint saved to pageID=%d\n",  toInt(argv[2]));
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //add func
    //
    int r = read(fdset[0].fd, buf, MAX_BUF);
    if (r < 0) {
#ifdef DEBUG
    perror("gpio/export");
#endif
    return r;
    }
  }

  return rc;
}
	
void Executive::registerCallback(FingerPrint_CallBack* cb)
{
  fp_callback_ptr = cb;
}
  
void Executive::unRegisterCallback()
{
  fp_callback_ptr = nullptr;
}



void Executive::run()
{
  // get a file descriptor for the IRQ GPIO pin
  // int sys_fs_fd = getSysfsIRQfd(drdy_GPIO); sys_fs_fd == g_fd
  int sys_fs_fd = g_fd;
  running = 1;
  while (running)
  {
    // let's wait for data for max one second
    // goes to sleep until an interrupt happens
    int ret = fd_Poll(sys_fs_fd,1000);
    if (ret < 1)
    {
#ifdef DEBUG
      fprintf(stderr,"Poll error %d\n",ret);
#endif
      throw "Interrupt timeout";
    }
    PS_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore) 
    // tell the AD7705 to read the data register (16 bits)
    writeReg(fd, commReg() | 0x38);
    
    // read the data register by performing two 8 bit reads
    const float norm = 0x8000;
    const float value = (readData(fd))/norm * 
      ADC_REF / pgaGain();
    
    if (nullptr != fp_callback_ptr)
    {
      fp_callback_ptr->demo_func(value);
      //search
      //fp_callback_ptr->demo_search(value); 
      //
    }
  }
  close(sys_fs_fd);
  gpio_unexport(drdy_GPIO);

}

void Executive::start()
{
  if (fp_callback_ptr != nullptr)
  {
    // already running
    return;
  }

#ifdef DEBUG
  fprintf(stderr,"Sending reset.\n");
#endif


  writeReset(fd);

  // tell the AD7705 that the next write will be to the clock register
  writeReg(fd,commReg() | 0x20);
  
  // write 00000100 : CLOCKDIV=0,CLK=1,expects 4.9152MHz input clock, sampling rate
  writeReg(fd,0x04 | ad7705settings.samplingRate);

  // tell the AD7705 that the next write will be the setup register
  writeReg(fd,commReg() | 0x10);
  
  // intiates a self calibration and then converting starts
  writeReg(fd,0x40 | (ad7705settings.mode << 2) | ( ad7705settings.pgaGain << 3) );


#ifdef DEBUG
  fprintf(stderr,"Starting DAQ thread.\n");
#endif

  car_Thread = new std::thread(exec,this);
}

void Executive::stop()
{
  running = 0;
  if (nullptr != car_Thread)
  {
    car_Thread->join();
    delete car_Thread;
    car_Thread = nullptr;
    
#ifdef DEBUG
    fprintf(stderr,"DAQ thread stopped.\n");
#endif	
  }
}
*/
/*************************************************************************************************/
/****************************          Call Back END            **********************************/
/*************************************************************************************************/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void Executive::Sum(int a, int b)
{
  int c =a+b;
  std::cout<<"here is sum\n";
  std::cout<<c;
}

void Executive::cb_sum(int fuck_a, int fuck_b, void *arg1)
{
  std::cout<<"here is cb_sum\n";
  Executive* Car_demo = static_cast<Executive*>(arg1);
  
  Car_demo->Sum(fuck_a,fuck_b);
}

//! \param summer Setter function pointer of type void (*summer)(int,int).
void Executive::register_handler( void (*summer)(int, int, void *) , void* p_instance)
{
    std::cout<<"here is call back handler\n";
    sum_handler_ = summer;
    foo_object_instance = p_instance;
}

void Executive::register_callbacks() {
    std::cout<<"here is call back func\n";
    register_handler(Executive::cb_sum, static_cast<void *>(this));
}	
void Executive::callback_sum(int value,int value2)
{
    sum_handler_(value, value2, foo_object_instance);
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
* Helper Functions
******************************************************************************/
///////////////////////
/*
void Executive::registerCallback(CallbackInterface* cb) {
	PS_Setupcallback = cb;
}*/
///////////////////////
void Executive::Split(uint num, uchar* buf, int count) {
  for (int i = 0; i < count; ++i) {
    *buf++ = (num & 0xff << 8*(count-i-1)) >> 8*(count-i-1);
  }
}


bool Executive::Merge(uint* num, const uchar* startAddr, int count) {
  *num = 0;
  for (int i = 0; i < count; ++i)
    *num += (int)(startAddr[i]) << (8*(count-i-1)); 

  return true;
}


void Executive::PrintBuf(const uchar* buf, int size) {
  for (int i = 0; i < size; ++i) {
    printf("%02X ", buf[i]);
  }
  printf("\n");
}


int Executive::Calibrate(const uchar* buf, int size) {
  int count = 0;
  for (int i = 6; i < size - 2; ++i) {
    count += buf[i];
  }

  return count;
}


bool Executive::Check(const uchar* buf, int size) {
  //init check sum
  int count_ = 0; 
  Merge((uint*)&count_, buf+size-2, 2); 

  // sum of check
  int count = Calibrate(buf, size);
  
  //avoid 0x00
  return (buf[9] == 0x00 && count_ == count && buf[0] != 0x00);
}


int Executive::SendOrder(const uchar* order, int size) {
  // print detials info
  if (g_verbose == 1) {
    printf("sent: ");
    PrintBuf(order, size);
  }
  int ret = write(g_fd, order, size);
  return ret;
}


bool Executive::RecvReply(uchar* hex, int size) {
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


void Executive::PrintProcess(int done, int all) {
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


bool Executive::RecvPacket(uchar* pData, int validDataSize) {
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



bool Executive::SendPacket(uchar* pData, int validDataSize) {
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

int Executive::GenOrder(uchar orderCode, const char* fmt, ...) {
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

/******************************************************************************
* AS608 main Functions
******************************************************************************/


bool Executive::PS_Setup(uint chipAddr, uint password) {

  g_as608.chip_addr = chipAddr;
  g_as608.password  = password;
  ////////////////////////////////////////////////////////
  //PS_Setupcallback->PS_Setup_cb(chipAddr, password);
  ////////////////////////////////////////////////////////
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


bool Executive::PS_GetImage() {
  int size = GenOrder(0x01, "");

  // send the command pack 
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) && Check(g_reply, 12));

}


bool Executive::PS_GenChar(uchar bufferID) {
  int size = GenOrder(0x02, "%d", bufferID);
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}

bool Executive::PS_Match(int* pScore) {
  int size = GenOrder(0x03, "");
  SendOrder(g_order, size);

  //  receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 14) && 
          Check(g_reply, 14) &&
          Merge((uint*)pScore, g_reply+10, 2));
}

bool Executive::PS_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore) {
  int size = GenOrder(0x04, "%d%2d%2d", bufferID, startPageID, count);
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return ( RecvReply(g_reply, 16) && 
           Check(g_reply, 16) && 
           (Merge((uint*)pPageID, g_reply+10, 2)) &&  // assign value to pageID, return true
           (Merge((uint*)pScore,  g_reply+12, 2))     // assign value to score, return true
        );
}

bool Executive::PS_RegModel() {
  int size = GenOrder(0x05, "");
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) &&
      Check(g_reply, 12));
}


bool Executive::PS_StoreChar(uchar bufferID, int pageID) {
  int size = GenOrder(0x06, "%d%2d", bufferID, pageID);
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) && 
        Check(g_reply, 12));
}


bool Executive::PS_LoadChar(uchar bufferID, int pageID) {
  int size = GenOrder(0x07, "%d%2d", bufferID, pageID);
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  return (RecvReply(g_reply, 12) &&
         Check(g_reply, 12));
}


bool Executive::PS_UpChar(uchar bufferID, const char* filename) {
  int size = GenOrder(0x08, "%d", bufferID);
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum 
  if (!(RecvReply(g_reply, 12) && Check(g_reply, 12))) {
    return false;
  }

  // receive datapack and restore the effective data into pData 
  uchar pData[768] = { 0 };
  if (!RecvPacket(pData, 768)) {
    return false;
  }

  // wirte into file 
  FILE* fp = fopen(filename, "w+");
  if (!fp) { 
    g_error_code = 0xC2;
    return false;
  }

  fwrite(pData, 1, 768, fp);
  fclose(fp);

  return true;
}


bool Executive::PS_DownChar(uchar bufferID, const char* filename) {
  // send command 
  int size = GenOrder(0x09, "%d", bufferID);
  SendOrder(g_order, size);

  // receive response pack, the subsequent datapack can be sent if the confirmation is 0x00
  if ( !(RecvReply(g_reply, 12) && Check(g_reply, 12)) )
    return false;

  // open local file 
  FILE* fp = fopen(filename, "rb");
  if (!fp) { 
    g_error_code = 0xC2;
    return false;
  }

  // obtain the file size 
  int fileSize = 0;
  fseek(fp, 0, SEEK_END);
  fileSize = ftell(fp);
  rewind(fp);
  if (fileSize != 768) {
    g_error_code = 0x09;
    fclose(fp);
    return false;
  }

  // read the content of file 
  uchar charBuf[768] = { 0 };
  fread(charBuf, 1, 768, fp);

  fclose(fp);

  // send datapack 
  return SendPacket(charBuf, 768);
}


bool Executive::PS_UpImage(const char* filename) {
  int size = GenOrder(0x0a, "");
  SendOrder(g_order, size);

  // receive the response pack, check the comfirmation pack and verify sum
  if (!(RecvReply(g_reply, 12) && Check(g_reply, 12))) {
    return false;
  }

  // receive datapack and restore the effective data into pData 
  // the size of picture 128*288 = 36864
  uchar* pData = (uchar*)malloc(36864);
  if (!RecvPacket(pData, 36864)) {
    return false;
  }

  // wirte pData into file 
  FILE* fp = fopen(filename, "w+");
  if (!fp) {
    g_error_code = 0xC2;
    return false;
  }

  // construct the begining of bmp (it's fixed for this module)
  uchar header[54] = "\x42\x4d\x00\x00\x00\x00\x00\x00\x00\x00\x36\x04\x00\x00\x28\x00\x00\x00\x00\x01\x00\x00\x20\x01\x00\x00\x01\x00\x08";
  for (int i = 29; i < 54; ++i)
    header[i] = 0x00;
  fwrite(header, 1, 54, fp);

  // pallet
  uchar palette[1024] = { 0 };
  for (int i = 0; i < 256; ++i) {
    palette[4*i]   = i;
    palette[4*i+1] = i;
    palette[4*i+2] = i;
    palette[4*i+3] = 0;
  }
  fwrite(palette, 1, 1024, fp);

  // the pixel data of bmp
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


bool Executive::PS_DownImage(const char* filename) {
  int size = GenOrder(0x0b, "");
  SendOrder(g_order, size);

  if (!RecvReply(g_reply, 12) && Check(g_reply, 12))
    return false;

  // the file size of fingerprint picture is 748069 bytes
  uchar imageBuf[74806] = { 0 };

  FILE* fp = fopen(filename, "rb");
  if (!fp) {
    g_error_code = 0xC2;
    return false;
  }

  // obtain the size of picture file
  int imageSize = 0;
  fseek(fp, 0, SEEK_END);
  imageSize = ftell(fp);
  rewind(fp);
  if (imageSize != 74806) { // the size of fingerprint should be 74806kb
    g_error_code = 0xC9;
    fclose(fp);
    return false;
  }

  // read file
  if (fread(imageBuf, 1, 74806, fp) != 74806) {
    g_error_code = 0xCA;
    fclose(fp);
    return false;
  }
  fclose(fp);

  return SendPacket(imageBuf+1078, 73728);
}



bool Executive::PS_DeleteChar(int startPageID, int count) {
  int size = GenOrder(0x0c, "%2d%2d", startPageID, count);
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum 
  return (RecvReply(g_reply, 12) &&
         Check(g_reply, 12));
}


bool Executive::PS_Empty() {
  int size = GenOrder(0x0d, "");
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum 
  return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}


bool Executive::PS_WriteReg(int regID, int value) {
  if (regID != 4 && regID != 5 && regID != 6) {
    g_error_code = 0x1a;
    return false;
  }

  int size = GenOrder(0x0e, "%d%d", regID, value);
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum 
  return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}

/*
 * funtion name: PS_ReadSysPara
 * description：reads the basic parameters of as608(baud rate，pack size ec.）
 * parameter：none(saved at the system variable of g_as608.chip_addr, g_as608.packet_size, PS_BPS ec.)
 * return value ：true(sucessful)，false(error accurred)，assign the confirmation code to g_error_code
 *   confirmation code =00H means OK；
 *   confirmation code =01H means error accurred in receiving pack；
*/
bool Executive::PS_ReadSysPara() {
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


bool Executive::PS_Enroll(int* pPageID) {
  int size = GenOrder(0x10, "");
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum
  return (RecvReply(g_reply, 14) &&
          Check(g_reply, 14) &&
          Merge((uint*)pPageID, g_reply+10, 2)
         );
}


bool Executive::PS_Identify(int* pPageID, int* pScore) { 
  int size = GenOrder(0x11, "");
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum
  return (RecvReply(g_reply, 16) &&
          Check(g_reply, 16) &&
          Merge((uint*)pPageID, g_reply+10, 2) &&
          Merge((uint*)pScore,  g_reply+12, 2)
         );
}


bool Executive::PS_SetPwd(uint pwd) {   // 0x00 ~ 0xffffffff
  int size  = GenOrder(0x12, "%4d", pwd);
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum
  return (RecvReply(g_reply, 12) && 
          Check(g_reply, 12) &&
          (g_as608.has_password = 1) &&
          ((g_as608.password = pwd) || true)); // 防止pwd=0x00
}


bool Executive::PS_VfyPwd(uint pwd) { 
  int size = GenOrder(0x13, "%4d", pwd); 
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum
  return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}


bool Executive::PS_GetRandomCode(uint* pRandom) {
  int size = GenOrder(0x14, "");
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum
  return (RecvReply(g_reply, 16) &&
          Check(g_reply, 16) &&
          Merge((uint*)pRandom, g_reply+10, 4)
         );
}


bool Executive::PS_SetChipAddr(uint addr) {
  int size = GenOrder(0x15, "%4d", addr);
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum
  return (RecvReply(g_reply, 12) && 
          Check(g_reply, 12) && 
          ((g_as608.chip_addr = addr) || true)); // avoid addr=0x00
}


bool Executive::PS_ReadINFpage(uchar* pInfo, int pInfoSize/*>=512*/) {
  if (pInfoSize < 512) {
    g_error_code = 0xC1;
    return false;
  }

  int size = GenOrder(0x16, "");
  SendOrder(g_order, size);

  // receive response pack 
  if (!(RecvReply(g_reply, 12) && Check(g_reply, 12))) 
    return false;

  // receive datapack
  if (!RecvPacket(pInfo, 512))
    return false;
  
  memcpy(g_as608.product_sn,       pInfo+28, 8);
  memcpy(g_as608.software_version, pInfo+36, 8);
  memcpy(g_as608.manufacture,      pInfo+44, 8);
  memcpy(g_as608.sensor_name,      pInfo+52, 8);

  return true;
}


bool Executive::PS_WriteNotepad(int notePageID, uchar* pContent, int contentSize) {
  if (contentSize > 32) {
    g_error_code = 0xC6;
    return false;
  }

  pContent[32] = 0; // ending of the string
  int size = GenOrder(0x18, "%d%32s", notePageID, pContent);
  SendOrder(g_order, size);

  return (RecvReply(g_reply, 12) && Check(g_reply, 12));
}


bool Executive::PS_ReadNotepad(int notePageID, uchar* pContent, int contentSize) {
  if (contentSize < 32) {
    g_error_code = 0xC1;
    return false;
  }

  int size = GenOrder(0x19, "%d", notePageID);
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum
  if (!(RecvReply(g_reply, 44) && Check(g_reply, 44)))
    return false;

  memcpy(pContent, g_reply+10, 32);
  return true;
}


bool Executive::PS_HighSpeedSearch(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore) {
  int size = GenOrder(0x1b, "%d%2d%2d", bufferID, startPageID, count);
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum
  return ( RecvReply(g_reply, 16) && 
           Check(g_reply, 16) && 
           (Merge((uint*)pPageID, g_reply+10, 2)) &&  // assign value to pageID，return true
           (Merge((uint*)pScore,  g_reply+12, 2))     // assign value to score，return true
        );
}


bool Executive::PS_ValidTempleteNum(int* pValidN) {
  int size = GenOrder(0x1d, "");
  SendOrder(g_order, size);

  // receive data, verify the confirmation code and the sum
  return (RecvReply(g_reply, 14) &&
          Check(g_reply, 14) &&
          Merge((uint*)pValidN, g_reply+10, 2)
         );
}


bool Executive::PS_ReadIndexTable(int* indexList, int size) {
  // Initialize all elements of indexList to -1
  for (int i = 0; i < size; ++i)
    indexList[i] = -1;

  int nIndex = 0;

  for (int page = 0; page < 2; ++page) {
    // send data（twice，256 fingerprint modules at every page which needs 2 page's request），
    int size = GenOrder(0x1f, "%d", page);
    SendOrder(g_order, size);

    // receive data, verify the confirmation code and the sum
    if (!(RecvReply(g_reply, 44) && Check(g_reply, 44)))
      return false;

    for (int i = 0; i < 32; ++i) {
      for (int j = 0; j < 8; ++j) {
        if ( ( (g_reply[10+i] & (0x01 << j) ) >> j) == 1 ) {
          if (nIndex > size) {
            g_error_code = 0xC1;    // array is too samll 
            return false;
          }
          indexList[nIndex++] = page*256 + 8 * i + j;
        } // end if

      } // end internel for

    } // end middle for

  }// end outer for

  return true;
}

// wrapper function

bool Executive::PS_DetectFinger() {
  return digitalRead(g_as608.detect_pin) == HIGH;
}

bool Executive::PS_SetBaudRate(int value) {
  return PS_WriteReg(4, value / 9600);
}

bool Executive::PS_SetSecureLevel(int level) {
  return PS_WriteReg(5, level);
}

bool Executive::PS_SetPacketSize(int size) {
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

bool Executive::PS_GetAllInfo()
{
	uchar buf[512] = { 0 };
	if (PS_ReadSysPara() && g_as608.packet_size > 0 && PS_ReadINFpage(buf, 512)) {
		return true;
	}
	else {
		g_error_code = 0xC7;
		return false;
	}
}

bool Executive::PS_Flush()
{
	int num = 0;
	for (int i = 0; i < 3; ++i) {
		if (PS_ValidTempleteNum(&num)) {
			return true;
		}
		sleep(1);
	}
  return false;
}

// obtain the defination of error code of g_error_code and assign value to g_error_desc
char* Executive::PS_GetErrorDesc()
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


