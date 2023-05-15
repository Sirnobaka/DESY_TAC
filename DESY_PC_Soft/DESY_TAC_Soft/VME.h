/*
 * CAEN VME Interface (singleton)
 *
 * Martin Goettlich 2010, DESY/FLC
 *
 * Alessandro Silenzi 08.2010 Added 32 bits and BLT support
 *
 */

#ifndef __VME_H
#define __VME_H
#include <stdio.h>

#include <iostream>
#include <string>
#include <sys/timeb.h>
#include <vector>
#include <map>
#include <signal.h>
#include <cmath>

#include "CAENVMElib.h"
#include "CAENVMEtypes.h"



class VME 
{
 private:

 static VME*  fInstance;
  
 public:

 CVAddressModifier cvAM;  
 CVDataWidth cvDW;
 
 public:

  // constructor
  VME(){cvAM=cvA32_U_DATA;cvDW=cvD32;};
  // destructor
  virtual ~VME(){};
  static VME* Instance();
  virtual void            Print(){};
  virtual void            printError(CVErrorCodes error, std::string operation="",bool verbose = false){};
  virtual CVErrorCodes initVME(){return cvGenericError;};
  virtual CVErrorCodes closeVME(){return cvGenericError;}; 
  virtual void unlockVME(){}; 
  virtual CVErrorCodes readVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,CVAddressModifier CvAM, CVDataWidth CvDW){ return cvGenericError;};
  virtual CVErrorCodes writeVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,CVAddressModifier CvAM, CVDataWidth CvDW){return cvGenericError;};
  virtual CVErrorCodes readVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data){ return cvGenericError;};
  virtual CVErrorCodes writeVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data){return cvGenericError;};
  virtual CVErrorCodes readBLTVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,int Size, int *count){return cvGenericError;};
  virtual CVErrorCodes writeBLTVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,int Size, int *count){return cvGenericError;};
  virtual CVErrorCodes readFIFOBLTVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,int Size, int *count){return cvGenericError;};
  virtual CVErrorCodes writeFIFOBLTVMEdata(unsigned int boardAdd, unsigned int registerAdd, void* data,int Size, int *count){return cvGenericError;};
  virtual CVErrorCodes writeMultiVMEdata(const int N, unsigned int *boardAdd, unsigned int *registerAdd, void* data, CVErrorCodes* EC){return cvGenericError;};
  virtual CVErrorCodes readMultiVMEdata(const int N, unsigned int *boardAdd, unsigned int *registerAdd, void* data, CVErrorCodes* EC){return cvGenericError;};
  virtual CVErrorCodes readVMEdata(unsigned int boardAdd, unsigned int registerAdd, unsigned short* data){return cvGenericError;};
  virtual CVErrorCodes writeVMEdata(unsigned int boardAdd, unsigned int registerAdd, unsigned short* data){return cvGenericError;};
  virtual CVErrorCodes readVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data){return cvGenericError;};
  virtual CVErrorCodes writeVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data){return cvGenericError;};
  virtual CVErrorCodes readBLTVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data,int Size, int *count){return cvGenericError;};
  virtual CVErrorCodes writeBLTVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data,int Size, int *count){return cvGenericError;};
  virtual CVErrorCodes readFIFOBLTVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data,int Size, int *count){return cvGenericError;};
  virtual CVErrorCodes writeFIFOBLTVMEdata32(unsigned int boardAdd, unsigned int registerAdd, unsigned int* data,int Size, int *count){return cvGenericError;};

  virtual CVErrorCodes writeMultiVMEdata(const int N, unsigned int *boardAdd, unsigned int *registerAdd, unsigned int* data, CVErrorCodes* EC){return cvGenericError;};
  virtual CVErrorCodes readMultiVMEdata(const int N, unsigned int *boardAdd, unsigned int *registerAdd, unsigned int* data, CVErrorCodes* EC){return cvGenericError;};
  virtual std::vector<unsigned int32_t>* createEventWords(int nBytes, unsigned char* buffer){return (0);};
  virtual CVErrorCodes readVMEeventData(unsigned int boardAdd, std::vector<unsigned int32_t>* * data){return cvGenericError;};

};

#endif
