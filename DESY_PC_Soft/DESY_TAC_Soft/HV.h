	/*
 * HV
 * Wrapper class for all HV modules.
 * 2011 Alessandro Silenzi
 */

#ifndef __HV_H
#define __HV_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
//#include <SerialStream.h>
#include <stdlib.h>
#include <unistd.h>

//using namespace LibSerial;
using namespace std;

class HV
{
 public:

  // constructor
  HV(){};

  // destructor
  virtual ~HV(){};

  virtual void SetVoltage(float voltage);

};

#endif
