CXX = g++
ROOTFLAGS += `root-config --cflags --glibs` -lSpectrum
LOADLIBES += -lm -lserial -DLINUX -l CAENVME -l ncurses -lc
#CXXFLAGS += -DLINUX -Wall -g -fPIC
CXXFLAGS += -Wall

all: CAENVME.o HV.o HV6519.o QDC965A.o ConfigFile.o ADC.o hvctl tiletester.o tiletester

#$(TARGET): $(TARGET).c
#	$(CXX) $(CFLAGS) -o $(TARGET) $(TARGET).c

CAENVME.o: CAENVME.h
	$(CXX) $(CXXFLAGS) $(LOADLIBES) -c CAENVME.C

HV.o: HV.h
	$(CXX) $(CXXFLAGS) $(LOADLIBES) -c HV.C

HV6519.o: HV6519.h
	$(CXX) $(CXXFLAGS) $(LOADLIBES) -c HV6519.C

QDC965A.o: QDC965A.h
	$(CXX) $(CXXFLAGS) $(LOADLIBES) $(ROOTFLAGS) -c QDC965A.C

ConfigFile.o: ConfigFile.h
	$(CXX) $(CXXFLAGS) $(LOADLIBES) -c ConfigFile.C

ADC.o: ADC.h
	$(CXX) $(CXXFLAGS) $(LOADLIBES) -c ADC.C

hvctl:  hvctl.o VME.o CAENVME.o HV.o HV6519.o
	$(CXX) $(CXXFLAGS) hvctl.o VME.o CAENVME.o HV.o HV6519.o  $(LOADLIBES) -o hvctl

tiletester.o: tiletester.C
	$(CXX) $(CXXFLAGS) $(LOADLIBES) $(ROOTFLAGS) -c tiletester.C
		
tiletester: tiletester.o
	$(CXX) $(CXXFLAGS) -lm CAENVME.o QDC965A.o HV.o HV6519.o ConfigFile.o ADC.o tiletester.o $(LOADLIBES) $(ROOTFLAGS) -o tiletester
	
clean:
	rm CAENVME.o HV6519.o QDC965A.o ConfigFile.o ADC.o hvctl hvctl.o tiletester.o tiletester
