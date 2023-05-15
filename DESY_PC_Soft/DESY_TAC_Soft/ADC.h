/*
 * ADC ABSTACT BASE CLASS
 *
 * Martin Goettlich DESY/FLC 2008
 */

#ifndef __ADC_H
#define __ADC_H

//#define OFFLINE

class ADC
{
 private:

 protected:

 unsigned int                   addr;           // hardware VME address
 int                            evno;           // event number
 unsigned int                   NumChannelsADC; // number of channels of the ADC

 virtual unsigned short         TestBit(unsigned short bit, unsigned short z);

 public:

  // constructor
  ADC();
  ADC(unsigned int address);
  // destructor
  virtual ~ADC();


  virtual void                   Print()   = 0;
  virtual int                    Config()  = 0;
  virtual int                    Reset()   = 0;
  virtual int                    Trigger() = 0;
  virtual int                    Readout() = 0; // -1: error; 0: no data; 1: data

  virtual float                  GetChannel(unsigned int nr) const = 0;

  virtual inline unsigned int    GetEventNumber() const { return this->evno; };
  virtual inline unsigned int    GetNumChannelsADC() const { return this->NumChannelsADC; };

};

#endif
