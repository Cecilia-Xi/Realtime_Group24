#include "Executive.h"


void Executive::checkADD(int finger, int score) const{
  qDebug("Callback ++ADD++ print: The new  finger  No. %d is added, Matched score: %d\n",finger,score);
}

void Executive::checkSEARCH(int finger) const{
   qDebug("Callback ==SEARCH= print: The finger in the Library is No. %d\n",finger);
   lockerControl(1);
}

Executive::Executive(QObject *parent): QObject(parent){
    m_fingerprint = new FingerPrint(this);
    initialize();
    if (-1 == wiringPiSetup()) {
        qDebug("wiringPi setup failed!\n");
    exit(0);
    }
}

void Executive::initialize() {
  if (!readConfig())
    exit(1);
  if (m_fingerprint->g_verbose == 1)
    printConfig();
  if (-1 == wiringPiSetup()) {
    qDebug("wiringPi setup failed!\n");
    exit(0);
  }
  pinMode(g_config.detect_pin, INPUT);
  pinMode(m_fingerprint->g_as608.detect_pin, INPUT);
  pinMode(key_pin,OUTPUT);
  if((m_fingerprint->g_fd = serialOpen(g_config.serial, g_config.baudrate)) < 0)	{
    fprintf(stderr,"Unable to open serial device: %s\n", strerror(errno));
    exit(0);
  }
  m_fingerprint->setUp(g_config.address, g_config.password);

}

Executive::~Executive(){
	m_fingerprint->atExitFunc();
    delete m_fingerprint;
	}

void Executive::lockerControl(int second) const {
  qDebug("FBI open The Door!\n");
  digitalWrite (key_pin,LOW);//set it initially as high, is closed
  //delay for locker open with customized seconds
  delay(second*1000);
  digitalWrite (key_pin,HIGH);
}


void Executive::search_withQT() {
    qDebug()<<"detect_thread id"<<QThread::currentThreadId();
    m_fingerprint->search();
}

void Executive::add_withQT() {
    qDebug()<<"add_thread id"<<QThread::currentThreadId();
    m_fingerprint->add();

}


void Executive::asyncConfig() {
  m_fingerprint->g_as608.detect_pin   = g_config.detect_pin;
  m_fingerprint->g_as608.has_password = g_config.has_password;
  m_fingerprint->g_as608.password     = g_config.password;
  m_fingerprint->g_as608.chip_addr    = g_config.address;
  m_fingerprint->g_as608.baud_rate    = g_config.baudrate;
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
    qDebug("Please config the address and password in \"~/.fpconfig\"\n");
    qDebug("  fp cfgaddr   0x[address]\n");
    qDebug("  fp cfgpwd    0x[password]\n");
    qDebug("  fp cfgserial [serialFile]\n");
    qDebug("  fp cfgbaud   [rate]\n");
    qDebug("  fp cfgpin    [GPIO_pin]\n");
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
      qDebug("Unknown key:%s\n", key);
      fclose(fp);
      return false;
    }

  } // end while(!feof(fp))

  asyncConfig();

  fclose(fp);
  return true;
}

void Executive::printConfig() {
  qDebug("address=%08x\n", g_config.address);
  if (g_config.has_password)
    qDebug("password=%08x\n", g_config.password);
  else
    qDebug("password=none(no password)\n");
  qDebug("serial_file=%s\n",   g_config.serial);
  qDebug("baudrate=%d\n",   g_config.baudrate);
  qDebug("detect_pin=%d\n",   g_config.detect_pin);
}


void Executive::writeConfig() {
  
  char filename[256] = { 0 };
  sprintf(filename, "%s/.fpconfig", getenv("HOME"));
  
  FILE* fp  = fopen(filename, "w+");
  if (!fp) {
    qDebug("Write config file error!\n");
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
