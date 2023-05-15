/*
 * CAEN VME Interface (singleton)
 *
 * Martin Goettlich 2010, DESY/FLC
 *
 * Alessandro Silenzi 08.2010 Added 32 bits and BLT support
 *
 */

#ifndef __CAENVME_H
#define __CAENVME_H

#include <iostream>
#include <string>
#include <sys/timeb.h>
#include <vector>
#include <map>
#include <signal.h>
#include <cmath>

#include <iostream>
#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include "VME.h"
#include "CAENVMElib.h"
#include "CAENVMEtypes.h"

#define VME_SEM_NAME "/CAENVMEBRIDGE"

#define DEBUG false

class CAENVME : public VME
{
 private:
  
  static CAENVME*  fInstance;
  sem_t *          sem;
  bool             inuse;
  int32_t          handle;
  
 public:
  
  CVAddressModifier cvAM;  
  CVDataWidth cvDW;
  
 public:
  
  // constructor
  CAENVME(){cvAM=cvA32_U_DATA;cvDW=cvD32;};
  CAENVME( CVAddressModifier CvAM, CVDataWidth CvDW){cvAM=CvAM;cvDW=CvDW;};
  // destructor
  ~CAENVME();
  
  void            Print();
  void            printError(CVErrorCodes error, std::string operation="",bool verbose = false);
  
  int32_t GetHandle(){return handle;};
  CVErrorCodes initVME();
  CVErrorCodes closeVME(); 
  bool isLockedByMe();
  // void unlockVME(); 
  CVErrorCodes readVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,CVAddressModifier CvAM, CVDataWidth CvDW);
  CVErrorCodes writeVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,CVAddressModifier CvAM, CVDataWidth CvDW);
  CVErrorCodes readVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data);
  CVErrorCodes writeVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data);
  CVErrorCodes readBLTVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,CVAddressModifier CvAM, CVDataWidth CvDW,int Size, int *count);
  CVErrorCodes writeBLTVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,CVAddressModifier CvAM, CVDataWidth CvDW,int Size, int *count);
  CVErrorCodes readFIFOBLTVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,int Size, int *count);
  CVErrorCodes writeFIFOBLTVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,int Size, int *count);
  CVErrorCodes writeMultiVMEdata(const int N, unsigned int *boardAdd, unsigned int *registerAdd, void* data, CVErrorCodes* EC);
  CVErrorCodes readMultiVMEdata(const int N, unsigned int *boardAdd, unsigned int *registerAdd, void* data, CVErrorCodes* EC);
  CVErrorCodes readVMEdata(unsigned int boardAdd, unsigned int registerAdd, unsigned short* data);
  CVErrorCodes writeVMEdata(unsigned int boardAdd, unsigned int registerAdd, unsigned short* data);
  CVErrorCodes readVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data);
  CVErrorCodes writeVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data);
  CVErrorCodes readBLTVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data,int Size, int *count);
  CVErrorCodes writeBLTVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data,int Size, int *count);
  CVErrorCodes readFIFOBLTVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data,int Size, int *count);
  CVErrorCodes writeFIFOBLTVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data,int Size, int *count);

  CVErrorCodes writeMultiVMEdata(const int N, unsigned int *boardAdd, unsigned int *registerAdd, unsigned int* data, CVErrorCodes* EC);
  CVErrorCodes readMultiVMEdata(const int N, unsigned int *boardAdd, unsigned int *registerAdd, unsigned int* data, CVErrorCodes* EC);
  std::vector<unsigned int32_t>* createEventWords(int nBytes, unsigned char* buffer);
  CVErrorCodes readVMEeventData(unsigned int boardAdd, std::vector<unsigned int32_t>* * data);


};

#endif
