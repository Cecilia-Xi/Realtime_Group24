#include "Executive.h"
#define SWITCH 7
Executive::Executive()
{

}
	
Executive::~Executive()
{
	
}
void Executive::lockerControl()
{
  pinMode (SWITCH,OUTPUT);
  printf("FBI open The Door!\n");
  digitalWrite (SWITCH,LOW);//set it initially as high, is closed

  delay(6000);
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
    //std::cout<<"serial "<<g_config.serial<<" baudrate "<<g_config.baudrate<<"\n";
		fprintf(stderr,"Unable to open serial device: %s\n", strerror(errno));
    exit(0);
	}

  
	// 6.Register the exit function (print some information, close the serial port, etc.)
	atexit(atExitFunc);

	// 7.initialize AS608 module 
	car1.PS_Setup(g_config.address, g_config.password) ||  PS_Exit();

	// 8.dispose main funtion and analysis general commands (argv[1])，
	analyseArgv(argc, argv);

}
	
//The program exited because the function in as608.h failed to execute
bool Executive::PS_Exit()
{
  printf("ERROR! code=%02X, desc=%s\n", g_error_code, car1.PS_GetErrorDesc());
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
    g_config.address = bike1.toUInt(argv[2]);
    writeConfig();
    exit(0);
  }

  // allocate communication password
  else if (match("cfgpwd")) {
    checkArgc(3);
    g_config.password = bike1.toUInt(argv[2]);
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
    g_config.baudrate = bike1.toInt(argv[2]);
    writeConfig();
    exit(0);
  }

  else if (match("cfgpin")) {
    checkArgc(3);
    g_config.detect_pin = bike1.toInt(argv[2]);
    writeConfig();
    exit(0);
  }
}

// Block until a finger is detected, the longest block wait_time is milliseconds
bool Executive::waitUntilDetectFinger(int wait_time) {
  while (true) {
    if (car1.PS_DetectFinger()) {
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
    if (!car1.PS_DetectFinger()) {
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
  delay(2000);
  car1.PS_GetImage() || PS_Exit();
  car1.PS_GenChar(1) || PS_Exit();

  int pageID = 0, score = 0;
  if (!car1.PS_Search(1, 0, 300, &pageID, &score))
    PS_Exit();
  else
  {   
      //lockerControl();
      pinMode (SWITCH,OUTPUT);
      printf("FBI open the door!\n");
      digitalWrite (SWITCH,LOW);//set it initially as high, is closed
      
      delay(6000);
      digitalWrite (SWITCH,HIGH);

      pinMode(g_config.detect_pin, INPUT);
  }

  //printf("Matched! pageID=%d score=%d\n", pageID, score);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  }
  
  
  else if (match("add")) {
    checkArgc(3);
    printf("Please put your finger on the module.\n");
    if (waitUntilDetectFinger(5000)) {
      delay(500);
      car1.PS_GetImage() || PS_Exit();
      car1.PS_GenChar(1) || PS_Exit();
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
        car1.PS_GetImage() || PS_Exit();
        car1.PS_GenChar(2) || PS_Exit();
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
    if (car1.PS_Match(&score)) {
      printf("Matched! score=%d\n", score);
    }
    else {
      printf("Not matched, raise your finger and put it on again.\n");
      exit(1);
    }
    
    if (g_error_code != 0x00)
      PS_Exit();

    // Merge feature files
    car1.PS_RegModel() || PS_Exit();
    car1.PS_StoreChar(2, bike1.toInt(argv[2])) || PS_Exit();

    printf("OK! New fingerprint saved to pageID=%d\n", bike1.toInt(argv[2]));
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
    car1.PS_Enroll(&pageID) || PS_Exit();

    printf("OK! New fingerprint saved to pageID=%d\n", pageID);
  }
  
  else if (match("info")) {
    checkArgc(2);
    car1.PS_GetAllInfo() || PS_Exit();

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

    car1.PS_Empty() || PS_Exit();
    printf("OK!\n");
  }

  else if (match("delete")) {
    int startPageID = 0;
    int count = 0;

    // Determine the number of parameters
    if (g_argc == 3) {
      startPageID = bike1.toInt(argv[2]);
      count = 1;
      printf("Confirm to delete fingerprint %d: (Y/n)? ", startPageID);
    }
    else if (argc == 4) {
      startPageID = bike1.toInt(argv[2]);
      count = bike1.toInt(argv[3]);
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

    car1.PS_DeleteChar(startPageID, count) || PS_Exit();
    printf("OK!\n");
  }

  else if (match("count")) {
    checkArgc(2);
    int count = 0;
    car1.PS_ValidTempleteNum(&count) || PS_Exit();
    printf("%d\n", count);
  }

  else if (match("search")) {
    checkArgc(2);

    printf("Please put your finger on the module.\n");
    car1.PS_GetImage() || PS_Exit();
    car1.PS_GenChar(1) || PS_Exit();

    int pageID = 0, score = 0;
    if (!car1.PS_Search(1, 0, 300, &pageID, &score))
      PS_Exit();
    else
      printf("Matched! pageID=%d score=%d\n", pageID, score);
  }

  else if (match("hsearch")) {  // high speed search
    checkArgc(2);

    printf("Please put your finger on the module.\n");
    car1.PS_GetImage() || PS_Exit();
    car1.PS_GenChar(1) || PS_Exit();

    int pageID = 0, score = 0;
    if (!car1.PS_HighSpeedSearch(1, 0, 300, &pageID, &score))
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

    car1.PS_Identify(&pageID, &score) || PS_Exit();
    printf("Matched! pageID=%d score=%d\n", pageID, score);
  }

  // list fingerprints
  else if (match("list")) {
    checkArgc(2);
    int indexList[512] = { 0 };
    car1.PS_ReadIndexTable(indexList, 512) ||  PS_Exit();

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
    car1.PS_GetImage() || PS_Exit();
    printf("OK!\n");
  }

  else if (match("genchar")) {
    checkArgc(3);
    car1.PS_GenChar(bike1.toInt(argv[2])) || PS_Exit();
    printf("OK!\n");
  }

  else if (match("regmodel")) {
    checkArgc(2);
    car1.PS_RegModel() ||  PS_Exit();
    printf("OK!\n");
  }

  else if (match("storechar")) {
    checkArgc(4);
    car1.PS_StoreChar(bike1.toInt(argv[2]), bike1.toInt(argv[3])) ||  PS_Exit();
    printf("OK! Stored in pageID=%d\n", bike1.toInt(argv[3]));
  }

  else if (match("loadchar")) {
    checkArgc(4);
    car1.PS_LoadChar(bike1.toInt(argv[2]), bike1.toInt(argv[3])) || PS_Exit();
    printf("OK! Loaded to bufferID=%d\n", bike1.toInt(argv[2]));
  }

  else if (match("match")) {
    checkArgc(2);
    int score = 0;
    car1.PS_Match(&score) || PS_Exit();
    printf("Matched! Score=%d\n", score);
  }

  else if (match("random")) {
    checkArgc(2);
    uint randomNum = 0; // must be unsigned int，Otherwise it may overflow
    car1.PS_GetRandomCode(&randomNum) ||  PS_Exit();
    printf("%u\n", randomNum);
  }
  
  else if (match("upchar")) {
    checkArgc(4);
    car1.PS_UpChar(bike1.toInt(argv[2]), argv[3]) ||  PS_Exit();
    printf("OK!\n");
  }

  else if (match("downchar")) {
    checkArgc(4);
    car1.PS_DownChar(bike1.toInt(argv[2]), argv[3]) || PS_Exit();
    printf("OK!\n");
  }

  else if (match("upimage")) {
    checkArgc(3);
    car1.PS_UpImage(argv[2]) || PS_Exit();
    printf("OK!\n");
  }

  else if (match("downimage")) {
    checkArgc(3);
    car1.PS_DownImage(argv[2]) ||  PS_Exit();
    printf("OK!\n");
  }

  else if (match("flush")) {
    checkArgc(2);
    car1.PS_Flush();
    printf("OK!\n");
  }

  else if (match("writereg")) {
    checkArgc(4);
    car1.PS_WriteReg(bike1.toInt(argv[2]), bike1.toInt(argv[3])) ||  PS_Exit();
    printf("OK!\n");
  }

  else if (match("baudrate")) {
    if (g_argc == 2) {
      printf("%d\n", g_as608.baud_rate);
      exit(0);
    }
    else if (g_argc == 3) {
      car1.PS_SetBaudRate(bike1.toInt(argv[2])) ||  PS_Exit();
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
      car1.PS_SetSecureLevel(bike1.toInt(argv[2])) ||  PS_Exit();
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
      car1.PS_SetPacketSize(bike1.toInt(argv[2])) ||  PS_Exit();
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
      car1.PS_SetChipAddr(bike1.toUInt(argv[2])) || PS_Exit();
      g_config.address = bike1.toUInt(argv[2]);
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
    car1.PS_SetPwd(bike1.toUInt(argv[2])) ||  PS_Exit();
    g_config.has_password = 1;
    g_config.password = bike1.toUInt(argv[2]);
    if (writeConfig())
      printf("New password is 0x%08x\n", g_as608.password);
  }

  else if (match("vfypwd")) {
    checkArgc(3);
    car1.PS_VfyPwd(bike1.toUInt(argv[2])) ||  PS_Exit();
    printf("OK!\n");
  }

  else if (match("readinf")) {
    checkArgc(3);
    uchar buf[513] = { 0 };
    car1.PS_ReadINFpage(buf, 512) ||  PS_Exit();

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

    car1.PS_WriteNotepad(bike1.toInt(argv[2]), (uchar*)buf, 32) ||  PS_Exit();

    printf("OK\n");
  }

  else if (match("readnote")) {
    checkArgc(3);
    uchar buf[33] = { 0 }; 
    car1.PS_ReadNotepad(bike1.toInt(argv[2]), buf, 32) || PS_Exit();
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
    bike1.trimSpaceInFile(filename);
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
      bike1.trim(tmp, key);
    else
      continue;
    if (tmp = strtok(NULL, "="))
      bike1.trim(tmp, value);
    else
      continue;
    while (!tmp)
      tmp = strtok(NULL, "=");

    // If the value starts with 0x
    int offset = 0;
    if (value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
      offset = 2;

    if (strcmp(key, "address") == 0) {
      g_config.address = bike1.toUInt(value+offset);
    }
    else if (strcmp(key, "password") == 0) {
      if (strcmp(value, "none") == 0 || strcmp(value, "false") == 0) {
        g_config.has_password = 0; // no password
      }
      else {
        g_config.has_password = 1; // with password
        g_config.password = bike1.toUInt(value+offset);
      }
    }
    else if (strcmp(key, "serial") == 0) {
      int len = strlen(value);
      if (value[len-1] == '\n')
        value[len-1] = 0;
      strcpy(g_config.serial, value);
    }
    else if (strcmp(key, "baudrate") == 0) {
      g_config.baudrate = bike1.toInt(value);
    }
    else if (strcmp(key, "detect_pin") == 0) {
      g_config.detect_pin = bike1.toInt (value);
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

