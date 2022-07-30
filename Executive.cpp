#include "Executive.h"


Executive::Executive(){
    g_fp = new FingerPrint(this);
    initialize();
    g_fp->setUp(g_config.address, g_config.password);

    if (-1 == wiringPiSetup()) {
    printf("wiringPi setup failed!\n");
    exit(0);
    }

    pinMode(key_pin,OUTPUT);
}
	
Executive::~Executive(){
	g_fp->atExitFunc();
    delete g_fp;
	}
  
  
void Executive::EXE_run() {

  //digitalWrite (key_pin,HIGH);//set it initially as high, is closed
  //finger print sensor is ready now//
  
  //run_withQT();
  run_plain();
  
}

void Executive::search_withQT() {
    //printf("1");
    g_fp->search();
}

void Executive::add_withQT() {
    g_fp->add();
}

void Executive::run_plain() {
	while(1){
    delay(100);
    if(!digitalRead(g_fp->PS_DetectFinger())){
        printf("now pressed, pin value is :%d\n",digitalRead(g_fp->PS_DetectFinger()));
        if(g_fp->search()){
          printf("FBI open The Door!\n");
          digitalWrite (key_pin,LOW);//set it initially as high, is closed

          delay(1000);
          digitalWrite (key_pin,HIGH);

          //pinMode(g_fp->g_as608.detect_pin, INPUT);
        }
        
    }
    else{
      printf("NOT PRESS!!, pin value is :%d\n",digitalRead(g_fp->PS_DetectFinger()));
    }
  }
}


void Executive::initialize() {
  if (!readConfig())
    exit(1);

  if (g_fp->g_verbose == 1)
    printConfig();

  if (-1 == wiringPiSetup()) {
    printf("wiringPi setup failed!\n");
    exit(0);
  }

  pinMode(g_config.detect_pin, INPUT);

  if((g_fp->g_fd = serialOpen(g_config.serial, g_config.baudrate)) < 0)	{
    fprintf(stderr,"Unable to open serial device: %s\n", strerror(errno));
    exit(0);
  }

  
}

void Executive::checkADD(int finger, int score) const{
  cout << "The new  finger  No." << finger << " is added, Matched score: " << score << endl;
}

void Executive::checkSEARCH(int finger) const{
   cout << "The finger in the Library is No." << finger << endl;
}
  
void Executive::lockerControl() {
  pinMode (key_pin,OUTPUT);
  printf("FBI open The Door!\n");
  digitalWrite (key_pin,LOW);//set it initially as high, is closed

  delay(1000);
  digitalWrite (key_pin,LOW);

  pinMode(g_fp->g_as608.detect_pin, INPUT);
}


void Executive::asyncConfig() {
  g_fp->g_as608.detect_pin   = g_config.detect_pin;
  g_fp->g_as608.has_password = g_config.has_password;
  g_fp->g_as608.password     = g_config.password;
  g_fp->g_as608.chip_addr    = g_config.address;
  g_fp->g_as608.baud_rate    = g_config.baudrate;
}

bool Executive::readConfig() {
  FILE* fp;

  
  char filename[256] = { 0 };
  sprintf(filename, "%s/.fpconfig", getenv("HOME"));
  
  
  if (access(filename, F_OK) == 0) { 
    trimSpaceInFile(filename);
    fp = fopen(filename, "r");
  }
  else {
    
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

    int offset = 0;
    if (value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
      offset = 2;

    if (strcmp(key, "address") == 0) {
      g_config.address = toUInt(value+offset);
    }
    else if (strcmp(key, "password") == 0) {
      if (strcmp(value, "none") == 0 || strcmp(value, "false") == 0) {
        g_config.has_password = 0; 
      }
      else {
        g_config.has_password = 1; 
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


void Executive::writeConfig() {
  
  char filename[256] = { 0 };
  sprintf(filename, "%s/.fpconfig", getenv("HOME"));
  
  FILE* fp  = fopen(filename, "w+");
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
