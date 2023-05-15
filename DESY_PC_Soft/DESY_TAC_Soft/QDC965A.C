/*
 * CAEN DUAL RANGE QDC V965A
 *
 * Martin Goettlich 2008, DESY/FLC
 */

#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>

#include "QDC965A.h"
//#include <DAQ.h>
#include "TString.h"
using std::cerr;
using std::cout;
using std::endl;

QDC965A* QDC965A::fInstance=NULL;

QDC965A* QDC965A::Instance(unsigned int address,CAENVME *Vme)
{
  if( !QDC965A::fInstance ) {
    fInstance = new QDC965A(address,Vme);
  }
  return QDC965A::fInstance;
}

QDC965A::QDC965A(unsigned int address)
{
  VMEverbosity=false;
  fTypeA=true;
  if(QDC965A::fInstance) {  uint32 rec = mem->outb[this->pos];

    cout<<"QDC965A::QDC965A() : QDC965A is a singleton! rec: "<< rec<< endl;
    exit(0);
  }
  QDC965A::fInstance=this;

  this->addr = address;
  this->pos = 0;

  vme=0x0;

 if(fTypeA){
    this->NumChannelsADC = 16;
  }
  else{
     this->NumChannelsADC = 32;
  }

  // software reset
  mem->bset1 = QDC_V965A_B1_SOFT_RESET; // set permanent software reset
  mem->bclr1 = QDC_V965A_B1_SOFT_RESET; // release software reset

  if(vme) vme->writeVMEdata(this->addr>>16, 0x1006,(void*)&mem->bset1,cvA24_U_DATA,cvD16); //!!!!!!
  if(vme) vme->writeVMEdata(this->addr>>16, 0x1008,(void*)&mem->bclr1,cvA24_U_DATA,cvD16); //!!!!!!
 
  //this->Print();

  this->Reset();

  this->Config();

  //this->Print();

}

QDC965A::QDC965A(unsigned int address,CAENVME *Vme, bool typeA)
{
  VMEverbosity=false;
  fTypeA=typeA;
  mem = new qdc_v965a_mem;

  vme=Vme;
  if(NULL==vme){
    cout<<"QDC965A::QDC965A() : No valid VME  "<<endl;
    exit(0);
  }

  if(QDC965A::fInstance){
    uint32 rec = mem->outb[this->pos];

    if(vme) vme->readVMEdata32(this->addr>>16,this->pos,(unsigned int*)&mem->outb[this->pos]); //!!!!!!!!

    cout<<"QDC965A::QDC965A() : QDC965A is a singleton! rec: "<< rec<<endl;
    exit(0);
  }
  QDC965A::fInstance=this;

  this->addr = address;
  this->pos = 0;

  if(fTypeA){
    this->NumChannelsADC = 16;
  }
  else{
     this->NumChannelsADC = 32;
  }
  // software reset
  mem->bset1 = QDC_V965A_B1_SOFT_RESET|QDC_V965A_B1_BERR; // set permanent software reset

  mem->bclr1 = QDC_V965A_B1_SOFT_RESET; // release software reset

  if(vme) vme->writeVMEdata(this->addr>>16, 0x1006,(void*)&mem->bset1,cvA24_U_DATA,cvD16); //!!!!!!
  if(vme) vme->writeVMEdata(this->addr>>16, 0x1008,(void*)&mem->bclr1,cvA24_U_DATA,cvD16); //!!!!!!

  //this->Print();

  this->Reset();

  this->Config();

  //this->Print();

}

QDC965A::~QDC965A()
{

}

//
/////////////// PRINT
//
void QDC965A::Print()
{
  if(vme) vme->readVMEdata(this->addr>>16,0x1000,(unsigned int*)&mem->firmware); //!!!!
  if(vme) vme->readVMEdata(this->addr>>16,0x1006,(unsigned int*)&mem->bset1); //!!!!  
  if(vme) vme->readVMEdata(this->addr>>16,0x1032,(unsigned int*)&mem->bset2); //!!!!  
  if(vme) vme->readVMEdata(this->addr>>16,0x100E,(unsigned int*)&mem->stat1); //!!!!
  if(vme) vme->readVMEdata(this->addr>>16,0x1010,(unsigned int*)&mem->ctl1); //!!!!
  if(vme) vme->readVMEdata(this->addr>>16,0x1022,(unsigned int*)&mem->stat2); //!!!!
  if(vme) vme->readVMEdata(this->addr>>16,0x1024,(unsigned int*)&mem->evcountl); //!!!!
  if(vme) vme->readVMEdata(this->addr>>16,0x1026,(unsigned int*)&mem->evcounth); //!!!!
  if(vme) vme->readVMEdata(this->addr>>16,0x103C,(unsigned int*)&mem->crsel); //!!!!
  if(vme) for(int i=0;i<32;i++) vme->readVMEdata(this->addr>>16,0x1080+2*i,(unsigned int*)&mem->thres[i]); //!!!!
  printf("\n\n<QDC965A::Print> : Start.\n");
  printf("Firmware Revision               : %hx \n",ntohs(mem->firmware));
  printf("Bit Set 1 Register              : %hx \n",mem->bset1);
  printf("Bit Set 1 Register BERR Flag    : %d \n",TestBit(3,mem->bset1));
  printf("Bit Set 1 Register Sel Addr     : %d \n",TestBit(4,mem->bset1));
  printf("Bit Set 1 Register Soft. Reset  : %d \n",TestBit(7,mem->bset1));
  printf("Status Register 1               : %hx \n",mem->stat1);
  printf("Status Register 1 DREADY        : %d \n",TestBit(0,mem->stat1));
  printf("Status Register 1 GLOBAL DREADY : %d \n",TestBit(1,mem->stat1));
  printf("Status Register 1 BUSY          : %d \n",TestBit(2,mem->stat1));
  printf("Status Register 1 GLOBAL BUSY   : %d \n",TestBit(3,mem->stat1));
  printf("Status Register 1 PURGED        : %d \n",TestBit(5,mem->stat1));
  printf("Status Register 1 EVRDY         : %d \n",TestBit(8,mem->stat1));
  printf("Control Register 1              : %hx \n",mem->ctl1);
  printf("Control Register 1 BLKEND       : %d \n",TestBit(2,mem->ctl1));
  printf("Control Register 1 PROG RESET   : %d \n",TestBit(4,mem->ctl1));
  printf("Control Register 1 BERR ENABLE  : %d \n",TestBit(5,mem->ctl1));
  printf("Control Register 1 ALIGN64      : %d \n",TestBit(6,mem->ctl1));
  printf("Status Register 2               : %hx \n",mem->stat2);
  printf("Status Register 2 BUFFER EMPTY  : %d \n",TestBit(1,mem->stat2));
  printf("Status Register 2 BUFFER FULL   : %d \n",TestBit(2,mem->stat2));
  printf("Status Register piggy-back type : %d%d%d%d",TestBit(7,mem->stat2),TestBit(6,mem->stat2)
                                                     ,TestBit(5,mem->stat2),TestBit(4,mem->stat2));
  printf("   (1110 for V965A)\n");
  printf("Event Counter High Register       : %u  \n",mem->evcounth);
  printf("Event Counter Low Register        : %u  \n",mem->evcountl);
  printf("Bit Set 2 Register                : %hx \n",mem->bset2);
  printf("Bit Set 2 Register TEST MEM       : %d  \n",TestBit(0,mem->bset2));
  printf("Bit Set 2 Register OFFLINE        : %d  \n",TestBit(1,mem->bset2));
  printf("Bit Set 2 Register CLEAR DATA     : %d  \n",TestBit(2,mem->bset2));
  printf("Bit Set 2 Register NO OVERFLOW SUP: %d  \n",TestBit(3,mem->bset2));
  printf("Bit Set 2 Register NO ZERO SUP    : %d  \n",TestBit(4,mem->bset2));
  printf("Bit Set 2 Register TEST AQ        : %d  \n",TestBit(6,mem->bset2));
  printf("Bit Set 2 Register SLIDE EN       : %d  \n",TestBit(7,mem->bset2));
  printf("Bit Set 2 Register STEP TH        : %d  \n",TestBit(8,mem->bset2));
  printf("Bit Set 2 Register AUTO INCR.     : %d  \n",TestBit(11,mem->bset2));
  printf("Bit Set 2 Register EMPTY ENABLE   : %d  \n",TestBit(12,mem->bset2));
  printf("Bit Set 2 Register SLIDE SUB EN.  : %d  \n",TestBit(13,mem->bset2));
  printf("Bit Set 2 Register ALL TRG        : %d  \n",TestBit(14,mem->bset2));
  printf("Crate                             : %d  \n",mem->crsel);
  //cout<<"Position in buffer: "<<this->pos<<endl;
  printf("Thresholds (ch.,KILL,SetValue)    :  \n");
  bool rflg = 0;
  if(fTypeA){
    for(int i=0;i<32;i+=2) {
      printf("Ch. %u%c %u  %hx  \n",i/4,(rflg?'L':'H'),TestBit(8,mem->thres[i]),mem->thres[i] & 0xFF);
      rflg = !rflg;
    }
  }
  else{
    for(int i=0;i<32;i++) {
      printf("Ch. %u%c %u  %hx  \n",i,(rflg?'L':'H'),TestBit(8,mem->thres[i]),mem->thres[i] & 0xFF);
      rflg = !rflg;
    }
  }

  printf("<QDC965A::Print> : End.\n\n\n");
 
 
}

int QDC965A::Reset()
{ 
  //printf("<QDC965A::Reset()> : Start.\n");

  //  uint32 rec = Record(); //!!!!!!
  // uint32 rec = mem->outb[this->pos];
  mem->bset2 = QDC_V965A_B2_CLEAR; // data reset
  mem->bclr2 = QDC_V965A_B2_CLEAR; // data reset

  if(vme) vme->writeVMEdata(this->addr>>16, 0x1032,(void*)&mem->bset2,cvA24_U_DATA,cvD16); //!!!!!!
  if(vme) vme->writeVMEdata(this->addr>>16, 0x1034,(void*)&mem->bclr2,cvA24_U_DATA,cvD16); //!!!!!!

  
  // software reset
  mem->bset1 = QDC_V965A_B1_SOFT_RESET; // set permanent software reset
  mem->bclr1 = QDC_V965A_B1_SOFT_RESET; // release software reset

  if(vme) vme->writeVMEdata(this->addr>>16, 0x1006,(void*)&mem->bset1,cvA24_U_DATA,cvD16); //!!!!!!
  if(vme) vme->writeVMEdata(this->addr>>16, 0x1008,(void*)&mem->bclr1,cvA24_U_DATA,cvD16); //!!!!!!!


  // set defaults 
  mem->crsel=1;                                    // we have the crate 1
  mem->evcreset=0;                                 // clear event counter

  //    if(vme) vme->writeVMEdata(this->addr>>16, 0x1032,(void*)&mem->bset2); //!!!!!!
  if(vme) vme->writeVMEdata(this->addr>>16, 0x103C,(void*)&mem->crsel,cvA24_U_DATA,cvD16); //!!!!!!!
  if(vme) vme->writeVMEdata(this->addr>>16, 0x1040,(void*)&mem->evcreset,cvA24_U_DATA,cvD16); //!!!!!!
 

  this->pos=0;

  //printf("<QDC965A::Reset()> :  End.\n");
  return 1;
}

int QDC965A::EnableChannel(int channel){
  int bytespace=2;
  int range = ((unsigned)channel>(this->NumChannelsADC/2))?1:0;
  int i = channel%(this->NumChannelsADC/2)+range;
  if(fTypeA){
    bytespace=4;
  }
  if(vme) vme->readVMEdata(this->addr>>16,0x1080+bytespace*i,(unsigned int*)&mem->thres[i]); //!!!!
  mem->thres[i] &= ~(QDC_V965A_THRESHOLD_KILLBIT);
  if(vme) vme->writeVMEdata(this->addr>>16,0x1080+bytespace*i,(unsigned int*)&mem->thres[i]); //!!!!
  return 1;
}

int QDC965A::DisableChannel(int channel){
  int bytespace=2;
  int range = ((unsigned)channel>(this->NumChannelsADC/2))?1:0;
  int i = channel%(this->NumChannelsADC/2)+range;
  if(fTypeA){
    bytespace=4;
  }
  if(vme) vme->readVMEdata(this->addr>>16,0x1080+bytespace*i,(unsigned int*)&mem->thres[i]); //!!!!
  mem->thres[i] |= QDC_V965A_THRESHOLD_KILLBIT;
  if(vme) vme->writeVMEdata(this->addr>>16,0x1080+bytespace*i,(unsigned int*)&mem->thres[i]); //!!!!
  return 1;
}

int QDC965A::SetThreshold(int channel, int threshold){
  int bytespace=2;
  int range = ((unsigned)channel>(this->NumChannelsADC/2))?1:0;
  int i = channel%(this->NumChannelsADC/2)+range;
  if(fTypeA){
    bytespace=4;
  }
  if(vme) vme->readVMEdata(this->addr>>16,0x1080+bytespace*i,(unsigned int*)&mem->thres[i]); //!!!!
  int sandbox= mem->thres[i];
  sandbox &= ~QDC_V965A_THRESHOLD_THRESHOLDMASK;
  sandbox |= threshold&QDC_V965A_THRESHOLD_THRESHOLDMASK;
  mem->thres[i] = sandbox;
  if(vme) vme->writeVMEdata(this->addr>>16,0x1080+bytespace*i,(unsigned int*)&mem->thres[i]); //!!!!
  return 1;
}


int QDC965A::Config()
{
  //printf("<QDC965A::Config> : Start.\n");

//   if(run->trigger != ADC_EXT_TRIGGER){
//     fprintf(stderr,"QDC V965A: unsupported triggertype (%u)\n",run->trigger);
//     return ADC_FAIL;
//   }

  // threshold/kill is not cleared by reset
//   for(i=0;i<QDC_V965A_CHANNELS;i++) {// disable unused channels
//     mem->thres[i<<1]=(info->chno[i]==ADC_NOT_USED)?QDC_V965A_THR_KILL:0;
//   }

//  mem->bclr2 = QDC_V965A_B2_ALLTRG; // event counter only incremented on accepted triggers 
  mem->bclr2 = 0x1FFF; // clear all bits
  mem->bset2 =
    QDC_V965A_B2_NOZS|
    QDC_V965A_B2_NOOVS|
    QDC_V965A_B2_ALLTRG|
    QDC_V965A_B2_EMPTY|
    QDC_V965A_B2_AUTOINC|
    QDC_V965A_B2_STEPTH;       //thres hold resolution
  if(vme) vme->writeVMEdata(this->addr>>16, 0x1034,(void*)&mem->bclr2,cvA24_U_DATA,cvD16); //!!!!!!!!!!!
  if(vme) vme->writeVMEdata(this->addr>>16,0x1032,(void*)&mem->bset2,cvA24_U_DATA,cvD16); //!!!!!!


  // set thresholds and activate channels
  // for(int i=0;i<32;i++) {
  //   mem->thres[i] = mem->thres[i] | 0x100;
  //   mem->thres[i] = mem->thres[i] & 0xFF;
  // }

  //printf("<QDC965A::Config> :  End.\n");
  return 1;
}

uint32 QDC965A::Record(int bytes)
{

  //  if(vme) vme->readVMEdata32(this->addr>>16,0x0,(unsigned int*)&mem->outb[0]); //!!!!
  if(bytes<0) exit(0);
  if(bytes==1){
    if(vme) vme->readVMEdata(this->addr>>16,0x0,(void *)&mem->outb[0],cvA24_U_DATA,cvD32); //!!!!
  }else{
      if(vme){
	CVErrorCodes EC[512];
	unsigned int boardAdd[512];
	unsigned int registerAdd[512];
	memset(boardAdd,this->addr>>16,512*sizeof(unsigned int));
	memset(registerAdd,0x0,512*sizeof(unsigned int));
	vme->readMultiVMEdata(bytes,boardAdd, registerAdd, (void *)&mem->outb[0],EC);
	this->pos=bytes;
      }
  }

      
  // if(vme) vme->readVMEdata32(this->addr>>16,this->pos,(unsigned int*)&mem->outb[this->pos]); //!!!!
  //uint32 rec = mem->outb[this->pos];
  uint32 rec = mem->outb[0];
  /*
  this->pos++;
  if(this->pos>=511) {
    this->pos=0;
    }*/
  return rec;
  

    /*  uint32 rec;
  
  for(int i=0;i<1;i++){
    for(int j=0;j<QDC_V965A_CHANNELS;j++){
      rec = mem->outb[this->addr>>16, 0x100+2*(QDC_V965A_CHANNELS*i+j),(void *)&(mem->evno[i].chnum[j])]

    }
  }

  return rec;*/
  

}

void * QDC965A::GetOutputBuffer(){
  return (void*)mem->outb;
}

int QDC965A::ReadoutBuffer(){
  if(vme==NULL)return 511;//if using memory mapping you can skip this step.
  int count;
  if(vme) vme->readVMEdata(this->addr>>16, 0x100E,(void*)&mem->stat1,cvA24_U_DATA,cvD16); //!!!!!!!!!!!
  if(!(mem->stat1&QDC_V965A_S1_DRDY)) {
    cout<<"<QDC965A::ReadoutBuffer> : DRDY ==0."<<"\r"<<std::flush;
    return 0;
  }
  
  if(vme) vme->readVMEdata(this->addr>>16, 0x1022,(void*)&mem->stat2,cvA24_U_DATA,cvD16); //!!!!!!!!!!!
  if(mem->stat2&QDC_V965A_S2_BFULL) {
    //    if(DAQ::Instance()->GetNumberOfADCModules()>1) {
    //      cout<<"<QDC965A::ReadoutBuffer> : BUFFER FULL."<<endl;
    //      return -1;
    //    }
  }
  
  if(vme) vme->readVMEdata(this->addr>>16, 0x100E,(void*)&mem->stat1,cvA24_U_DATA,cvD16); //!!!!!!!!!!!
  if(mem->stat1&QDC_V965A_S1_BUSY) {
    //    if(DAQ::Instance()->GetNumberOfADCModules()>1) {
    //      cout<<"<QDC965A::ReadoutBuffer> : MODULE BUSY."<<endl;
    //      return 0;
    //     }
  }
  int blts=511*4;
  if(vme) memset(mem->outb,0,blts);
  
  if(vme) vme->readBLTVMEdata(this->addr>>16, 0x0,(void *)mem->outb,cvA24_U_BLT,cvD32,blts, &count);
  //  if(vme) vme->readBLTVMEdata(this->addr>>16, 0x0,(void *)&mem->outb[0],cvA24_U_BLT,cvD32,0x07FE, &count);
  this->pos=count/4;
  // for(unsigned short i=0;i<this->pos;i++){
  //   cerr<<"<QDC965A::ReadoutBuffer> : word "<< i <<" is 0X"<< Form("%08X  Type: %d -- Count %d -- channel %d range %d Charge %d ",mem->outb[i],QDC_V965A_WORD_TYPE(  mem->outb[i]), QDC_V965A_HDR_COUNT(  mem->outb[i]), QDC_V965_DATA_CH(mem->outb[i]),(QDC_V965_DATA_RANGE(  mem->outb[i])), QDC_V965A_DATA_VALUE(  mem->outb[i])) << " "<<endl;
  // }
  return count/4;
}

int QDC965A::DecodeBuffer(int Pos){ //returns end Position // this function is  a bit risky
  unsigned short data,ch,range;
  assert(Pos>=0);
  assert(Pos<this->pos);

  //exit upon first word not header
  if(QDC_V965A_WORD_TYPE(  mem->outb[Pos])!=QDC_V965A_HDR){
    //    cerr<<"<QDC965A::DecodeBuffer> : First 32 bit word in output buffer is not a header! Type: "<<QDC_V965A_WORD_TYPE(mem->outb[Pos])<<endl;
    //    cerr<<"<QDC965A::DecodeBuffer> : word is 0X"<< std::hex  << mem->outb[Pos] <<std::dec << " "<<endl;
    if(QDC_V965A_WORD_TYPE(  mem->outb[Pos])==QDC_V965A_INVAL) {
      //      cerr<<"<QDC965A::DecodeBuffer> : Type 6: Invalid datum!"<<endl; 
      return this->pos;
    }
    return Pos+1;
  }
  
  int count = QDC_V965A_HDR_COUNT(  mem->outb[Pos]); // get number of memorised channels from 32 bit header word
  this->chnum = count;
  if(VMEverbosity) cout << "QDC965A::DecodeBuffer(): count = " << count << endl;
  
  for(unsigned short i=0;i<NumChannelsADC;i++) this->chval[i] = 0.0;
  
  for(unsigned int i=0;i<(unsigned)count;i++){
    Pos++;
    if(QDC_V965A_WORD_TYPE(  mem->outb[Pos])!=QDC_V965A_DATA){
      cerr<<"QDC965A::DecodeBuffer> : Expected data not found: 0X"<<  Form("%X",mem->outb[Pos]) <<endl;
      this->Reset();
      this->Config();
      //      this->Print();
      return -1;
      //exit(0);
      
    }
    if(fTypeA){
      ch = QDC_V965A_DATA_CH(  mem->outb[Pos]);
    }else{
      ch = QDC_V965_DATA_CH(  mem->outb[Pos]);
    }

    data = QDC_V965A_DATA_VALUE(  mem->outb[Pos]);

    if(fTypeA){
      range = QDC_V965A_DATA_RANGE(  mem->outb[Pos]);
      if(VMEverbosity) cout << "QDC965A::DecodeBuffer():   mem->outb[Pos] = " << Form("0x%X",  mem->outb[Pos]) << endl;
    }
    else{
      range = QDC_V965_DATA_RANGE(  mem->outb[Pos]);
      if(VMEverbosity) cout << "QDC965::DecodeBuffer():   mem->outb[Pos] = " << Form("0x%X",  mem->outb[Pos]) << endl;
    }
    
    //if(range==0) ch=ch+0x8;// high range to ch nr 8-15
    if(range==0) ch += ((fTypeA)?8:16);

     if(QDC_V965A_DATA_OV(  mem->outb[Pos])) {// to make OV/UN visible
       if(VMEverbosity)std::cout<<"<QDC965A::DecodeBuffer>  --------------------- Over Flow at CH: "<<ch<<"\r"<<std::flush;
       data=4096;
     }
     if(QDC_V965A_DATA_UN(  mem->outb[Pos])) {
       if(VMEverbosity)std::cout<<"<QDC965A::DecodeBuffer> --------------------------Under threshold at CH: "<<ch<<"\r"<<std::flush;
       data=0;
     }
     //cout<<"channel:          "<<ch<<"        "<<data<<endl;
     this->chval[ch] = (float)data;
  }
  Pos++;
  if(QDC_V965A_WORD_TYPE(  mem->outb[Pos])!=QDC_V965A_EOB){
    printf("QDC965A::DecodeBuffer> : Expected EOB not found: 0X%X \r",  mem->outb[Pos]);
    if(QDC_V965A_WORD_TYPE(  mem->outb[Pos])==QDC_V965A_INVAL) {
      cerr<<"<QDC965A::DecodeBuffer> : Type 6: Invalid datum!"<<endl; 
    }
    //this->Print();
    //     exit(0);
    return -1;
  }
  //printf("QDC965A::Readout> : Internal event %u is processed.\n",QDC_V965A_EOB_EVNO(  mem->outb[Pos]));
  this->evno = QDC_V965A_EOB_EVNO(  mem->outb[Pos]);
  return Pos+1;
}


int QDC965A::Readout()
{
   unsigned count;
   uint32 rec;
   unsigned short data,ch,range;

#ifdef OFFLINE
  static unsigned int no = 0;
  no++;
  for(unsigned short i=0;i<QDC_V965A_CHANNELS;i++) this->chval[i] = 3000+100.0*(i+1);
  this->chnum = 8;
  this->evno = no;


  return 1;
#endif

  if(vme) vme->readVMEdata(this->addr>>16, 0x100E,(void*)&mem->stat1,cvA24_U_DATA,cvD16); //!!!!!!!!!!!
  if(!(mem->stat1&QDC_V965A_S1_DRDY)) {
    cout<<"<QDC965A::Readout> : DRDY ==0."<<"\r"<<std::flush;
    return 0;
  }
  
  if(vme) vme->readVMEdata(this->addr>>16, 0x1022,(void*)&mem->stat2,cvA24_U_DATA,cvD16); //!!!!!!!!!!!
  if(mem->stat2&QDC_V965A_S2_BFULL) {
    //    if(DAQ::Instance()->GetNumberOfADCModules()>1) {
    //      cout<<"<QDC965A::Readout> : BUFFER FULL."<<endl;
    //      return -1;
    //    }
  }
  
  if(vme) vme->readVMEdata(this->addr>>16, 0x100E,(void*)&mem->stat1,cvA24_U_DATA,cvD16); //!!!!!!!!!!!
  if(mem->stat1&QDC_V965A_S1_BUSY) {
    //    if(DAQ::Instance()->GetNumberOfADCModules()>1) {
    //      cout<<"<QDC965A::Readout> : MODULE BUSY."<<endl;
    //      return 0;
    //     }
  }
  
  //printf("<QDC965A::Readout> : DRDY.\n");
  
  rec = this->Record();
  // while(QDC_V965A_WORD_TYPE(rec)!=QDC_V965A_HDR){
  //   printf("QDC965A::Readout> :  0X%X \r",rec);
  //   rec = this->Record();
  // }
  if(QDC_V965A_WORD_TYPE(rec)!=QDC_V965A_HDR){
    cerr<<"<QDC965A::Readout> : First 32 bit word in output buffer is not a header! Type: "<<QDC_V965A_WORD_TYPE(rec)<<endl;
    if(QDC_V965A_WORD_TYPE(rec)==QDC_V965A_INVAL) {
      cerr<<"<QDC965A::Readout> : Type 6: Invalid datum!"<<endl; 
    }
    //    this->Print();
    cerr<<"<QDC965A::Readout> : Trying to reset"<<endl;
    this->Reset();
    return -1;
  }

  count = QDC_V965A_HDR_COUNT(rec); // get number of memorised channels from 32 bit header word
  //cout<<"<QDC965A::Readout> : "<<count<<" memorised channels."<<endl;
  this->chnum = count;
  if(VMEverbosity) cout << "QDC965A::Readout(): count = " << count << endl;
  
  for(unsigned short i=0;i<NumChannelsADC;i++) this->chval[i] = 0.0;
  
  for(unsigned int i=0;i<count;i++){
    rec = this->Record();
    if(QDC_V965A_WORD_TYPE(rec)!=QDC_V965A_DATA){
      cerr<<"QDC965A::Readout> : Expected data not found: "<<rec<<endl;
      this->Print();
      exit(0);
    }
    // ch = QDC_V965A_DATA_CH(rec);
    if(fTypeA){
      ch = QDC_V965A_DATA_CH(rec);
    }else{
      ch = QDC_V965_DATA_CH(rec);
    }
     //if(info->chno[ch]==ADC_NOT_USED){
     //  fprintf(stderr,"QDC V965A: BUG: unused channel was not killed (%u)...\n",ch);
     //  continue;
     // }
   
    data = QDC_V965A_DATA_VALUE(rec);

    if(fTypeA){
      range = QDC_V965A_DATA_RANGE(rec);
      if(VMEverbosity) cout << "QDC965A::Readout(): rec = " << Form("0x%X",rec) << endl;
    }
    else{
      range = QDC_V965_DATA_RANGE(rec);
      if(VMEverbosity) cout << "QDC965::Readout(): rec = " << Form("0x%X",rec) << endl;
    }


    //if(range==0) ch=ch+0x8;// high range to ch nr 8-15
    if(range==0) ch += ((fTypeA)?8:16);

     if(QDC_V965A_DATA_OV(rec)) {// to make OV/UN visible
       if(VMEverbosity)std::cout<<"<QDC965A::Readout>  --------------------- Over Flow at CH: "<<ch<<"\r"<<std::flush;
       data=4096;    
     }
     if(QDC_V965A_DATA_UN(rec)) {
       if(VMEverbosity)std::cout<<"<QDC965A::Readout> --------------------------Under threshold at CH: "<<ch<<"\r"<<std::flush;
       data=0;
     }

     //cout<<"channel:          "<<ch<<"        "<<data<<endl;

     this->chval[ch] = (float)data;
   }
   rec = this->Record();
   if(QDC_V965A_WORD_TYPE(rec)!=QDC_V965A_EOB){
     printf("QDC965A::Readout> : Expected EOB not found: 0X%X \r",rec);
    if(QDC_V965A_WORD_TYPE(rec)==QDC_V965A_INVAL) {
      cerr<<"<QDC965A::Readout> : Type 6: Invalid datum!"<<endl; 
    }
    //this->Print();
    //     exit(0);
    return 0;
   }
   //printf("QDC965A::Readout> : Internal event %u is processed.\n",QDC_V965A_EOB_EVNO(rec));
   this->evno = QDC_V965A_EOB_EVNO(rec);
   return 1;
}
 
int QDC965A::Trigger()
{
  printf("<Trigger> : Not yet implemented.\n");
  return 1;
}

unsigned short QDC965A::SetCurrentPed(unsigned short cuped)
{
  if(cuped>0xFF){
    printf("<SetCurrentPed> : current pedestal between 0 to FF. \n");
    return 0;
  }
  // uint16 ped = (unsigned short int)cuped;
  mem->iped = cuped;
  if(vme) vme->writeVMEdata(this->addr>>16, 0x1060,(void*)&mem->iped,cvA24_U_DATA,cvD16); //!!!!!!!
  if(vme) vme->readVMEdata(this->addr>>16, 0x1060,(void*)&mem->iped,cvA24_U_DATA,cvD16);
  return (unsigned short)mem->iped;
}

 unsigned short  QDC965A::SetZSThreshold(unsigned short ch, unsigned short th)
{
  if(fTypeA){
    // Set zero suppression threshold for V965A module
    // 0x1080 ch1 High, 0x1084 ch1 Low, ... 0x10B8 ch7 High, 0x10BC ch7 Low
    if(th>0xFF){
      cout<<"Threshold range 0~255"<<endl;
      return 0;
    }
    unsigned short cha;
    if(ch<8)  cha = (unsigned short)(4*ch+2);   //Low range
    else cha = (unsigned short)((ch-8)*4); //High range
    
    mem->thres[cha] = th;
    if(vme) vme->writeVMEdata(this->addr>>16, 0x1080+2*cha,(void*)&mem->thres[cha],cvA24_U_DATA,cvD16); //!!!!!!
    if(vme) vme->readVMEdata(this->addr>>16, 0x1080+2*cha, (void*)&mem->thres[cha],cvA24_U_DATA,cvD16); //!!!!!!
    
    return (unsigned short)mem->thres[cha];
  }
  else{
    // Set zero suppression threshold for V965 module
    // 0x1080 ch1 High, 0x1082 ch1 Low, ... 0x10BC ch15 High, 0x10BE ch15 Low
     if(th>0xFF){
      cout<<"Threshold range 0~255"<<endl;
      return 0;
     }
     unsigned short cha;
     if(ch<15)  cha = (unsigned short)(4*ch+2);   //Low range
     else cha = (unsigned short)((ch-16)*4); //High range

     mem->thres[cha] = th;
     if(vme) vme->writeVMEdata(this->addr>>16, 0x1080+cha,(void*)&mem->thres[cha],cvA24_U_DATA,cvD16); //!!!!!!
     if(vme) vme->readVMEdata(this->addr>>16, 0x1080+cha, (void*)&mem->thres[cha],cvA24_U_DATA,cvD16); //!!!!!!
    
    return (unsigned short)mem->thres[cha];
  }
}

int QDC965A::ResetZSThreshold()
{
  if(fTypeA){
    for(int i=0;i<16;i++){
      SetZSThreshold(i,0); //!!!!!!
    }
    return 0;
  }
  else{
    for(int i=0;i<32;i++){
      SetZSThreshold(i,0); //!!!!!!
    }
    return 0;
  }
}
