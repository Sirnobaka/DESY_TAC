#!/bin/bash

add() { n="$@"; bc <<< "${n// /+}"; }
mult() { n="$@"; bc <<< "${n// /*}"; }
minus() { n="$@"; bc <<< "${n// /-}"; }

pathToTempDir="/home/tacuser/KAM/temp/"_
prefix="\/home\/tacuser\/KAM\/temp\/19112021_SPS_WrappedTile_pos270_TInBox23p7C_"
prefix1="/home/tacuser/KAM/temp/19112021_SPS_WrappedTile_pos270_TInBox23p7C_"
postfix="V_period37us_100k.root"
SiPMVoltage=-38.75
temperature=23.7
Nevents=100000
LEDOn=1
LEDVoltage=2.9
SourceOn=0
Nruns=29 # for 39 to 46 volt
Vstep=0.25

echo "Star test"

for (( n_run = 0; n_run < $Nruns; n_run++ ))
do
  SiPMVoltage=$(minus $SiPMVoltage $Vstep)
  SiPMVoltageTitle=$(mult -1 $SiPMVoltage)
  SiPMVoltageTitle=${SiPMVoltageTitle//./p}
  temperatureTitle=${temperature//./p}
  echo "$mod"
  #SiPMVoltage=(bc <<< '$SiPMVoltage+$Vstep') 
  sed -i "s/outfile $PARTITION_COLUMN.*/outfile      = $prefix$SiPMVoltageTitle$postfix/" sps.cfg
  sed -i "s/SetVoltage $PARTITION_COLUMN.*/SetVoltage   = $SiPMVoltage/" sps.cfg
  sed -i "s/Temperature $PARTITION_COLUMN.*/Temperature = $temperature/" sps.cfg
  sed -i "s/draw $PARTITION_COLUMN.*/draw         = 0              # draw data yes=1, no=0/" sps.cfg
  sed -i "s/interactive $PARTITION_COLUMN.*/interactive  = 0             # enter batch mode yes=1, no=0/" sps.cfg
  #sed -i "s/nevents $PARTITION_COLUMN.*/nevents      = $Nevents/" sps.cfg
  echo "Voltage on SiPM = $SiPMVoltage"
  echo "Name of file number ${n_run}"
  filename=$prefix1$SiPMVoltageTitle$postfix
  echo $filename
  ./tiletester sps.cfg 0 0
done

echo "End of test"

# sed -i 's/old-text/new-text/g' input.txt
