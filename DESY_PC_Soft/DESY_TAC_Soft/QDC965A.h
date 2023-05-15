/*
 * CAEN DUAL RANGE QDC V965A
 *
 * Martin Goettlich 2008, DESY/FLC
 */

#ifndef __QDC_V965A_H
#define __QDC_V965A_H


#include "CAENVME.h"
#include "assert.h"
#include "ADC.h"

//
// memory map
//

#define QDC_V965A_RAM_SIZE 0x10c0
#define QDC_V965A_MEM_SIZE 0x10000
#define QDC_V965A_CHANNELS 8
#define QDC_V965_CHANNELS  16
#define QDC_V965_RANGE     0    // high=0 or low=1 range

#ifndef uint32
typedef unsigned        uint32; // unsigned int          4 bytes     32 bits               0 -> +4,294,967,295   ( 4Gb)
typedef unsigned short  uint16; // unsigned short int    2 bytes     16 bits               0 -> +65,535          (64Kb)
typedef char            uint8;  // unsigned char         1 byte       8 bits               0 -> +255 
#endif

typedef struct {
  uint32 outb[511];        // Output buffer 0x0000-0x07FC (511 bytes)        (RO)
  uint32 nused1[513];
  uint16 firmware;         // Firmware Revision     (RO)
  uint16 geoaddr;          // GEO address           (RO*)
  uint16 cbltaddr;         // MCST/CBLT address
  uint16 bset1;            // Bit Set 1
  uint16 bclr1;            // Bit Clear 1
  uint16 irql;             // Interrupt Level
  uint16 irqv;             // Interrupt Vector
  uint16 stat1;            // Status Register 1     (RO)
  uint16 ctl1;             // Control Register 1
  uint16 aderh;            // ADER High
  uint16 aderl;            // ADER Low
  uint16 ssreset;          // Single Shot Reset     (WO)
  uint16 nused2;           // 0x1018-0x1019 (2 bytes)
  uint16 cbltctl;          // MCST/CBLT control
  uint16 nused3[2];        // 0x101C-0x101F (4 bytes)
  uint16 evtr;             // Event Trigger Register
  uint16 stat2;            // Status Register 2     (RO)
  uint16 evcountl;         // Event Counter Low     (RO)
  uint16 evcounth;         // Event counter High    (RO)
  uint16 incev;            // Increment Event       (WO)
  uint16 incoff;           // Increment Offset      (WO)
  uint16 loadtest;         // Load Test
  uint16 fclrwin;          // Fast CLeaR Window
  uint16 nused4;           // 0x1030-0x1031 (2 bytes)
  uint16 bset2;            // Bit Set 2
  uint16 bclr2;            // Bit Clear 2           (WO)
  uint16 wtestaddr;        // W Memory Test address (WO)
  uint16 mtestwh;          // Memory Test Word_High (WO)
  uint16 mtestwl;          // Memory Test Word_Low  (WO)
  uint16 crsel;            // Crate Select
  uint16 testev;           // Test Event Write      (WO)
  uint16 evcreset;         // Event Counter Reset   (WO)
  uint16 nused5a[15];      // 0x1042-0x105F (30 bytes)
  uint16 iped;             // Iped (0x1060)
  uint16 nused5b;          // 0x1062-0x1063 (2 bytes)
  uint16 rtestaddr;        // R Test address        (WO)
  uint16 nused6;           // 0x1066-0x1067 (2 bytes)
  uint16 swcomm;           // SW Comm               (WO)
  uint16 nused7[3];        // 0x106A-0x106F (6 bytes)
  uint16 aad;              // AAD                   (RO)
  uint16 bad;              // BAD                   (RO)
  uint16 nused8[6];        // 0x1074-0x107F (12 bytes)
  uint16 thres[32];        // Thresholds (0x1080 to 0x10BF -> 64 bytes)
} qdc_v965a_mem;

// precompiler macros to read output buffer 
#define QDC_V965A_WORD_TYPE(d32) (((d32)>>24)&0x7) // bits 24, 25 and 26 contain info if word (32 bit) is header, data or EOB word
#define QDC_V965A_HDR   0x2 // 010 => header
#define QDC_V965A_DATA  0x0 // 000 => valid datum
#define QDC_V965A_EOB   0x4 // 100 => EOB
#define QDC_V965A_INVAL 0x6 // 110 => invalid datum

#define QDC_V965A_THRESHOLD_KILLBIT           0x100       // KILL bit
#define QDC_V965A_THRESHOLD_THRESHOLDMASK     0xFF        // Set fill bit
#define QDC_V965A_THRESHOLD_THRE(d32)     ((d32)&0xFF)       // get GEO address
#define QDC_V965A_THRESHOLD_KILL(d32)     (((d32)>>8)&0x1)       // get GEO address

#define QDC_V965A_GEO_ADD(d32)     ((d32)>>27)       // get GEO address
#define QDC_V965A_HDR_COUNT(d32)  (((d32)>>8)&0x3f) // get number of memorised channels
#define QDC_V965A_EOB_EVNO(d32)   ((d32)&0xffffff)  // event counter in EOB
#define QDC_V965A_DATA_VALUE(d32) ((d32)&0xfff)
#define QDC_V965A_DATA_OV(d32)    (((d32)>>12)&0x1)
#define QDC_V965A_DATA_UN(d32)    (((d32)>>13)&0x1)
#define QDC_V965A_DATA_RANGE(d32) (((d32)>>17)&0x1) // get range: (0=high, 1=low)
//#define QDC_V965_DATA_RANGE(d32)  (((d32)>>16)&0x1)
// use with V965A
#define QDC_V965A_DATA_CH(d32)    (((d32)>>18)&0x7) // get channel
#define QDC_V965A_DATA_RANGE(d32) (((d32)>>17)&0x1)
// use with V965
#define QDC_V965_DATA_CH(d32)     (((d32)>>17)&0xf)
#define QDC_V965_DATA_RANGE(d32)  (((d32)>>16)&0x1)


// convert sequence no to channel no in buffer (0,8,1,9...)
//#define QDC_V965AN_S2C(no) ((((no)&1)<<3)+((no)>>1))
//#define QDC_V965A_S2C(no)  ((((no)&1)<<4)+((no)>>1))

//
/////////// r/w registers 
//

// Bit Set 1 / Bit Clear 1
#define QDC_V965A_B1_BERR       0x8
#define QDC_V965A_B1_USE_ADER   0x10
#define QDC_V965A_B1_SOFT_RESET 0x80

// Status 1
#define QDC_V965A_S1_DRDY     0x1
#define QDC_V965A_S1_GDRDY    0x2
#define QDC_V965A_S1_BUSY     0x4
#define QDC_V965A_S1_GBUSY    0x8
#define QDC_V965A_S1_PURGED   0x20
#define QDC_V965A_S1_EVRDY    0x100

// Control 1
#define QDC_V965A_C1_BLKEND     0x4
#define QDC_V965A_C1_RESET_MODE 0x10
#define QDC_V965A_C1_BERR_ENAB  0x20
#define QDC_V965A_C1_ALIGN_64   0x40

// Status 2
#define QDC_V965A_S2_BEMPTY      0x2
#define QDC_V965A_S2_BFULL       0x4
#define QDC_V965A_S2_PIGGY(d16)  (((d16)>>4)&0xf) // check me!


// Bit Set 2 / Bit Clear 2
#define QDC_V965A_B2_MTEST     0x1
#define QDC_V965A_B2_OFFLINE   0x2
#define QDC_V965A_B2_CLEAR     0x4
#define QDC_V965A_B2_NOOVS     0x8  // no overflow suppression 
#define QDC_V965A_B2_NOZS      0x10 // no zero suppression
#define QDC_V965A_B2_TEST      0x40
#define QDC_V965A_B2_SLIDE     0x80
#define QDC_V965A_B2_STEPTH    0x100 //threshold resoultion (value x2)
#define QDC_V965A_B2_AUTOINC   0x800
#define QDC_V965A_B2_EMPTY     0x1000
#define QDC_V965A_B2_SLIDESUB  0x2000
#define QDC_V965A_B2_ALLTRG    0x4000

// Thresholds
#define QDC_V965A_THR_KILL     0x100

class QDC965A : public ADC
{
 private:

 static QDC965A*         fInstance;

 qdc_v965a_mem* mem;       // pointer to mapped memory
 unsigned short          pos;       // position in output buffer
 unsigned int            addr;      // hardware VME address

 uint32                  Record(int bytes=1);

 //float                   chval[16]; // 0-7 low and 8-15 high range
 float                   chval[32]; // 0-7 low and 8-15 high range
 unsigned int            chnum;     // number of channels above th
 unsigned int            evno;      // event number
 CAENVME * vme;
 bool fTypeA;                 // fTypeA == False is 16ch
 bool VMEverbosity;

 public:

 // constructor
 QDC965A(unsigned int address);
 QDC965A(unsigned int address=0,CAENVME *Vme=NULL,bool typeA=true); //default for 8CH
 // destructor
 virtual ~QDC965A();

  static QDC965A* Instance(unsigned int address,CAENVME *Vme);
  void            Print();
  int             Config();
  int             Reset();
  int             Trigger();
  int             Readout(); // -1: error; 0: no data; 1: data
  unsigned short  SetCurrentPed(unsigned short cuped);
  unsigned short  SetZSThreshold(unsigned short ch, unsigned short th);  //not working with 16CH
  int             ResetZSThreshold();                                    //not working with 16CH
  int EnableChannel(int channel);
  int DisableChannel(int channel);
  int SetThreshold(int channel, int threshold);
  void * GetOutputBuffer();
  int ReadoutBuffer();
  int DecodeBuffer(int pos);

  unsigned int           GetNumberOfChannels() const { return this->chnum; };
  virtual float          GetChannel(unsigned int nr) const { return this->chval[nr]; };
  unsigned int           GetEventNumber() const { return this->evno; };
  qdc_v965a_mem*         GetMemory() const {return (qdc_v965a_mem*)&mem;}          //return a snap of current QDC memory

};

#endif
