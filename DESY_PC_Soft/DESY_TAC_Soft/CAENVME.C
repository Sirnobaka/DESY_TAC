/*
 * CAEN VME Interface
 *
 * Martin Goettlich   2010, DESY/FLC
 * Alessandro Silenzi 2010, DESY/FLC
 */

#include "CAENVME.h"

using std::cerr;
using std::cout;
using std::endl;

CAENVME::~CAENVME() {
  closeVME();
}

void CAENVME::Print() {
}

void CAENVME::printError(CVErrorCodes error, std::string operation,bool verbose) 
{
 
  switch (error) {
  case cvSuccess :
    if (verbose) std::cout << operation << ": " "Operation completed successfully" << std::endl;
    break;
  case cvBusError :
    std::cout << operation << ": " "VME bus error during the cycle               " << std::endl;
    break;
  case cvCommError:
    std::cout << operation << ": " "Communication error                          " << std::endl;
    break;
  case cvGenericError:
    std::cout << operation << ": " "Unspecified error                            " << std::endl;
    break;
  case cvInvalidParam:
    std::cout << operation << ": " "Invalid parameter                            " << std::endl;
    break;
  case cvTimeoutError:
    std::cout << operation << ": " "Timeout error                                " << std::endl;
    break;
  default :
    std::cout << operation << ": " "unsupported error code " << std::endl;
  };

}


CVErrorCodes CAENVME::initVME() {
  //  sem=sem_open(VME_SEM_NAME,O_CREAT,0666,1); //try to initialized a named semaphore
  // if(NULL==sem||SEM_FAILED==sem){
  //  std::cerr << "Cannot Open Semaphore   " <<VME_SEM_NAME << std::endl;
  //  exit(1);
  // }
  CVErrorCodes retval = CAENVME_Init(cvV2718, 0, 0, &handle);
  inuse=false;
  return retval;
}

CVErrorCodes CAENVME::closeVME() {
  //unlockVME();
  CVErrorCodes retval = CAENVME_End(handle);
  return retval;

}
/*
bool CAENVME::isLockedByMe(){
  int value;
  sem_getvalue(sem,&value);
  if(value==1&&inuse==false){
    return false;
  }
  if(0==value&&inuse==true){
    return true;
  }
  return false;
}

void CAENVME::unlockVME() {
  int value;

  if(this->isLockedByMe()){
    std::cout << "freeing the Semaphore   " <<VME_SEM_NAME << std::endl;
    sem_getvalue(sem,&value);
    if(value >0) return;
    sem_post(sem);
    inuse=false;
    sem_getvalue(sem,&value);
    std::cout << "this object has inuse = " << inuse << " and the semaphore value is "<<value <<std::endl;
  }
  return;
}
*/

/* Code using data width according to the parameter type */

CVErrorCodes CAENVME::readVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,CVAddressModifier CvAM, CVDataWidth CvDW){
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  //sem_wait(sem);  
  inuse=true;
  if(DEBUG)   printf("%s:: %s : read in %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,data,CvAM,CvDW);
  CVErrorCodes retval = CAENVME_ReadCycle(handle,address,data,CvAM,CvDW);
  //sem_post(sem);
  inuse=false;
  return retval;
}
CVErrorCodes CAENVME::writeVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,CVAddressModifier CvAM, CVDataWidth CvDW){
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  //sem_wait(sem);  
  inuse=true;
  if(DEBUG)   printf("%s:: %s : write in %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,data,CvAM,CvDW);
  CVErrorCodes retval = CAENVME_WriteCycle(handle,address,data,CvAM,CvDW);
  //sem_post(sem);
  inuse=false;
  return retval;
}

CVErrorCodes CAENVME::readVMEdata(unsigned int boardAdd, unsigned int registerAdd, unsigned short* data) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  //sem_wait(sem);  
  inuse=true;
  if(DEBUG) printf("read from %08x %04X am=%x dw=%x\n",address,*data,cvAM,cvD16);
  CVErrorCodes retval = CAENVME_ReadCycle(handle,address,data,cvAM,cvD16);
  //sem_post(sem);
  inuse=false;
  return retval;
}

CVErrorCodes CAENVME::writeVMEdata(unsigned int boardAdd, unsigned int registerAdd, unsigned short* data) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  //  sem_wait(sem);    
  inuse=true;
  if(DEBUG)   printf("%s:: %s : in %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,data,cvAM,cvD16);
  CVErrorCodes retval = CAENVME_WriteCycle(handle,address,data,cvAM,cvD16);
  // sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::readVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  //sem_wait(sem);  
  inuse=true;  
  if(DEBUG) printf("%s:: %s : from %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,*data,cvA32_U_DATA,cvD32);
  CVErrorCodes retval = CAENVME_ReadCycle(handle,address,data,cvA32_U_DATA,cvD32);
  //sem_post(sem);
  inuse=false;
  return retval;
  
}

CVErrorCodes CAENVME::writeVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  //sem_wait(sem);  
  inuse=true;  
  if(DEBUG)  printf("%s:: %s : write in %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,*data,cvA32_U_DATA,cvD32);
  CVErrorCodes retval = CAENVME_WriteCycle(handle,address,data,cvA32_U_DATA,cvD32);
  //sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::readBLTVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data,int Size,int *count) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  //sem_wait(sem);  
  inuse=true;
  if(DEBUG) printf("%s:: %s : from %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,*data,cvA32_U_DATA,cvD32);
  CVErrorCodes retval = CAENVME_BLTReadCycle(handle,address,data,Size, cvA32_U_DATA, cvD32,count);
  //sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::readFIFOBLTVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data,int Size, int *count) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  //sem_wait(sem);  
  inuse=true;
  CVErrorCodes retval = CAENVME_FIFOBLTReadCycle(handle,address,data,Size, cvA32_U_DATA, cvD32,count);
  //sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::writeBLTVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned  int* data,int Size, int *count) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  //sem_wait(sem);  
  inuse=true;
  if(DEBUG) printf("%s:: %s : from %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,*data,cvA32_U_DATA,cvD32);
  CVErrorCodes retval = CAENVME_BLTWriteCycle(handle,address,data,Size, cvA32_U_DATA, cvD32,count);
  //sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::writeFIFOBLTVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data,int Size, int *count) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  //sem_wait(sem);  
  inuse=true;
  if(DEBUG) printf("%s:: %s : from %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,*data,cvA32_U_DATA,cvD32);
  CVErrorCodes retval = CAENVME_FIFOBLTWriteCycle(handle,address,data,Size, cvA32_U_DATA, cvD32,count);
  //sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::writeMultiVMEdata(const int N, unsigned int *boardAdd, unsigned int *registerAdd, unsigned int* data, CVErrorCodes* EC) {
  if(N<1) return cvInvalidParam;
  unsigned int address[N];
  CVAddressModifier AM[N];  
  CVDataWidth DW[N];

  //sem_wait(sem);  
  inuse=true;
  for (int i=0;i<N;i++) { 
    address[i] = boardAdd[i]*0x10000 + registerAdd[i];
    AM[i]      = cvA32_U_DATA;
    DW[i]      = cvD16;
  }

  CVErrorCodes retval = CAENVME_MultiWrite(handle, address, data, N , AM, DW, EC);
  //sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::readMultiVMEdata(const int N, unsigned int *boardAdd, unsigned int *registerAdd, unsigned int* data, CVErrorCodes* EC) {
  if(N<1) return cvInvalidParam;
  unsigned int address[N];
  CVAddressModifier AM[N];  
  CVDataWidth DW[N];

  //sem_wait(sem);  
  inuse=true;
  for (int i=0;i<N;i++) { 
    address[i] = boardAdd[i]*0x10000 + registerAdd[i];
    AM[i]      = cvA32_U_DATA;
    DW[i]      = cvD16;
  }

  CVErrorCodes retval = CAENVME_MultiRead(handle, address, data, N , AM, DW, EC);
  //sem_post(sem);
  inuse=false;
  return retval;

}

/*

CVErrorCodes CAENVME::readVMEeventData(unsigned int boardAdd, std::vector<unsigned int32_t>* * data ) {
  unsigned long int address = boardAdd;
  address = (address << 16);
  
  unsigned char buffer[3000];
  int count=0;
  sem_wait(sem);  
inuse=true;
  CVErrorCodes error = CAENVME_BLTReadCycle(handle,address,buffer,3000,cvA32_U_BLT,cvD32,&count);

  *data = createEventWords(count,buffer);

  CVErrorCodes retval = error;
  // sem_post(sem);
  inuse=false;
  return retval;

}

*/
/* End of code using data width according to the parameter type */


/* Code using data width according to the constructor specifications (data pointer is pointer to void ) */


CVErrorCodes CAENVME::readVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data) {
  unsigned int address = boardAdd;
  address = (address << 16) + registerAdd;
  // sem_wait(sem);  
  inuse=true;
  if(DEBUG) printf("%s:: %s : from %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,*(int *)data,cvAM,cvDW);
  CVErrorCodes retval = CAENVME_ReadCycle(handle,address,data,cvAM,cvDW);
  // sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::writeVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  // sem_wait(sem);  
  inuse=true;
  if(DEBUG) printf("%s:: %s : from %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,*(int *)data,cvAM,cvDW);
  CVErrorCodes retval = CAENVME_WriteCycle(handle,address,data,cvAM,cvDW);
  // sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::readBLTVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,CVAddressModifier CvAM, CVDataWidth CvDW,int Size,int *count) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  // sem_wait(sem);  
  inuse=true;
  if(DEBUG) printf("%s:: %s : from %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,*(int *)data,cvAM,cvDW);
  CVErrorCodes retval = CAENVME_BLTReadCycle(handle,address,data,Size,CvAM ,CvDW ,count);
  // sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::writeBLTVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,CVAddressModifier CvAM, CVDataWidth CvDW,int Size, int *count) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  // sem_wait(sem);  
  inuse=true;
  if(DEBUG) printf("%s:: %s : from %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,*(int *)data,cvAM,cvDW);
  CVErrorCodes retval = CAENVME_BLTWriteCycle(handle,address,data,Size, CvAM, CvDW,count);
  // sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::readFIFOBLTVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,int Size, int *count) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  // sem_wait(sem);  
  inuse=true;
  if(DEBUG) printf("%s:: %s : from %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,*(int *)data,cvAM,cvDW);
  CVErrorCodes retval = CAENVME_FIFOBLTReadCycle(handle,address,data,Size, cvAM, cvDW,count);
  // sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::writeFIFOBLTVMEdata(unsigned int boardAdd, unsigned int registerAdd,  void* data,int Size, int *count) {
  unsigned long int address = boardAdd;
  address = (address << 16) + registerAdd;
  // sem_wait(sem);  
  inuse=true;
  if(DEBUG) printf("%s:: %s : from %08x %04X am=%x dw=%x\n",__FILE__,__FUNCTION__,address,*(int *)data,cvAM,cvDW);
  CVErrorCodes retval = CAENVME_FIFOBLTWriteCycle(handle,address,data,Size, cvAM, cvDW,count);
  // sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::writeMultiVMEdata(const int N, unsigned int *boardAdd, unsigned int *registerAdd, void* data, CVErrorCodes* EC) {
  if(N<1) return cvInvalidParam;
  unsigned int *address = new unsigned int[N];
  CVAddressModifier *AM = new CVAddressModifier[N];  
  CVDataWidth *DW = new CVDataWidth[N];
  // sem_wait(sem);  
  inuse=true;
  for (int i=0;i<N;i++) { 
    address[i] = boardAdd[i]*0x10000 + registerAdd[i];
    AM[i]      = cvAM;
    DW[i]      = cvDW;
  }
  CVErrorCodes retval = CAENVME_MultiWrite(handle, address, (uint32_t*)data, N , AM, DW, EC);
  // sem_post(sem);
  inuse=false;
  return retval;

}

CVErrorCodes CAENVME::readMultiVMEdata(const int N, unsigned int *boardAdd, unsigned int *registerAdd,  void* data, CVErrorCodes* EC) {
  if(N<1) return cvInvalidParam;
  unsigned int *address = new unsigned int[N];
  CVAddressModifier *AM = new CVAddressModifier[N];  
  CVDataWidth *DW = new CVDataWidth[N];

  // sem_wait(sem);  
  inuse=true;
  for (int i=0;i<N;i++) { 
    address[i] = boardAdd[i]*0x10000 + registerAdd[i];
    AM[i]      = cvAM;
    DW[i]      = cvDW;
  }

  CVErrorCodes retval = CAENVME_MultiRead(handle, address, (uint32_t*)data, N , AM, DW, EC);

  // sem_post(sem);
  inuse=false;
  return retval;

}

/* End of code using data width according to the constructor specifications (data pointer is pointer to void ) */


std::vector<unsigned int32_t>* CAENVME::createEventWords(int nBytes, unsigned char* buffer) {
  std::vector<unsigned int32_t>* v = new  std::vector<unsigned int32_t>;
  // sem_wait(sem);  
  inuse=true;
  for (int i=0;i<nBytes;i+=4) v->push_back(buffer[i+3]*0x1000000 + buffer[i+2]*0x10000 + buffer[i+1]*0x100 + buffer[i]);
  // sem_post(sem);
  inuse=false;
  return v;
}


CVErrorCodes CAENVME::readVMEeventData(unsigned int boardAdd, std::vector<unsigned int32_t>* * data ) {
  unsigned long int address = boardAdd;
  address = (address << 16);
  
  unsigned char buffer[3000];
  int count=0;

  // sem_wait(sem);  
  inuse=true;
  CVErrorCodes error = CAENVME_BLTReadCycle(handle,address,buffer,3000,cvA32_U_BLT,cvD32,&count);

  *data = createEventWords(count,buffer);

  // sem_post(sem);
  inuse=false;
  return error;
}

