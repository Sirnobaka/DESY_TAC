
#include "ADC.h"

ADC::ADC()
{
}

ADC::ADC(unsigned int address)
{
  this->addr = address;
}

ADC::~ADC()
{

}

unsigned short ADC::TestBit(unsigned short bit, unsigned short z) 
{
  unsigned short r=0;
  r = z & (1 << bit);
  if(r>0) return 1; else return 0;
}
