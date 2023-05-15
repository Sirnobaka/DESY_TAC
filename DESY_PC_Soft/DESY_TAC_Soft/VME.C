/*
 *  VME class prototype
 *
 *  Alessandro Silenzi 2011, DESY/FLC
 */

#include <stdio.h>
#include <stdlib.h>

#include "VME.h"

VME* VME::fInstance=NULL;

VME* VME::Instance()
{
  if( !VME::fInstance ) {
    std::cout<<"<VME::Instance> : Creating new instance of class VME."<<std::endl;
    fInstance = new VME();
  }
  return VME::fInstance;
}

