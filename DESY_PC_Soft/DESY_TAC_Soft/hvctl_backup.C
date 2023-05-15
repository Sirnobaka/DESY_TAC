#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "CAENVME.h"
//#include "HV.h"
#include "HV6519.h"
#include <sys/types.h>
CAENVME * vme= NULL;
HV6519 *hv = NULL;

using namespace std;

void do_exit(){
  if(NULL!=hv) delete hv;
  if(NULL!=vme)  vme->closeVME();
  signal(SIGTERM,SIG_DFL);
  signal(SIGINT ,SIG_DFL);
  exit(0);    
}

void signalHandler(int signal) {
  std::cout << std::endl << "Process " << getpid() << " received signal "  << signal << std::endl;
  if(signal==SIGTERM||signal==SIGKILL||signal==SIGINT)
    do_exit();
}

void do_usage(){
  cout<<"#### HV control with DDC cmd via GPIB ####"<<endl;
  cout<<"usage:./hvctl [arguments]"<<endl;
  cout<<"  -aVME <address HEX>    VME Base address"<<endl;
  cout<<"  -v <volt>              voltage value"<<endl;
  cout<<"  -ch <channel>           channel (conflict with adress!! default0)"<<endl;
  cout<<"  -off                   turn device off"<<endl;
  cout<<"  -read                  read "<<endl;
  

}

int main(int argc, char* argv[])
{
  int VMEADD  = -1;  // VME address
  int CHANNEL = 0;
  if(argc<2){
    do_usage();
    exit(0);
  }
  signal(SIGTERM,signalHandler);
  signal(SIGINT ,signalHandler);

  //****************** argument test********************//
  int index =0;
  float volt = 0.0;
  bool off=0;
  bool read=0;
  
  // setting for scanner loop
  //
  while(index<argc){
    if(strcmp(argv[index],"-aVME")==0){
      if((index+1)<argc)
	sscanf(argv[++index],"%X",&VMEADD);
      else{
	do_usage();
	exit(0);
      }
    }    
    
    if(strcmp(argv[index],"-v")==0){
      if((index+1)<argc) {
	volt = atof(argv[++index]);
	cout<<"Voltage "<<volt<<endl;
      }
    
      else{
	do_usage();
	exit(0);
      }
    }
    
    if(strcmp(argv[index],"-ch")==0){
      CHANNEL = atoi(argv[++index]);
      cout<<"Channel: "<<CHANNEL<<endl;
    }
    if(strcmp(argv[index],"-off")==0){
      off=1;
    }
    if(strcmp(argv[index],"-read")==0){
      read=1;
    }
    index++;
  }
  if(!off && volt==0 && !read){
    do_usage();
    exit(0);
  }

  else if(-1!=VMEADD){
    vme = new CAENVME(cvA32_U_DATA,cvD16);
    vme->initVME();
    printf("VME address: %X \n",VMEADD);
    hv = new HV6519(VMEADD,CHANNEL,vme);
  }else{
    do_usage(); 
    exit(0);
  }

  if(volt!=0.0){
    cout<<"Setting Voltage to: "<<volt<<endl;
    if (abs(volt) < 50)
      hv->SetVoltage(volt);
    else
      cout << ".......0-0......" << endl;
    float curr=hv->ReadCurrent();
    cout << endl << "Current: " << curr << endl;
  }
    
  if(off){
    cout<<"Turn HV off"<<endl;
    hv->SetVoltage(-0.0);
    delete hv;
  }
  if(read) {
    cout<<"Reading"<<endl;
    float curr=hv->ReadCurrent();
    float vvmon=hv->ReadVoltage();
    cout << endl << " Voltage: " << vvmon <<"\tCurrent: " << curr << endl;
  }
  //   do_exit();  //   do_exit();
    
  if(NULL!=vme)  vme->closeVME();
  return EXIT_SUCCESS;
}
