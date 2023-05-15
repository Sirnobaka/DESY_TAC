#!/bin/bash

add() { n="$@"; bc <<< "${n// /+}"; }
mult() { n="$@"; bc <<< "${n// /*}"; }
minus() { n="$@"; bc <<< "${n// /-}"; }

pathToTempDir="/home/tacuser/KAM/temp/"_
prefix="\/home\/tacuser\/KAM\/temp\/18112021_SPS_WrappedTile_TInBox23p1C_"
prefix1="/home/tacuser/KAM/temp/18112021_SPS_WrappedTile_TInBox23p1C_"
postfix="V_period37us_100k.root"
SiPMVoltage=-38.50
temperature=22.3
Nevents=100000
LEDOn=1
LEDVoltage=3.7
SourceOn=0
Nruns=3
Vstep=0.5

for (( n_run = 0; n_run < $Nruns; n_run++ ))
do
  #VoltToadd=$(mult $n_run $Vstep)
  SiPMVoltage=$(minus $SiPMVoltage $Vstep)
  SiPMVoltageTitle=$(mult -1 $SiPMVoltage)
  SiPMVoltageTitle=${SiPMVoltageTitle//./p}
  temperatureTitle=${temperature//./p}
  echo "$mod"
  #SiPMVoltage=(bc <<< '$SiPMVoltage+$Vstep') 
  sed -i "s/outfile $PARTITION_COLUMN.*/outfile      = $prefix$SiPMVoltageTitle$postfix/" sps.cfg
  sed -i "s/SetVoltage $PARTITION_COLUMN.*/SetVoltage   = $SiPMVoltage/" sps.cfg
  sed -i "s/Temperature $PARTITION_COLUMN.*/Temperature = $temperature/" sps.cfg
  #sed -i "s/nevents $PARTITION_COLUMN.*/nevents      = $Nevents/" sps.cfg
  echo "Voltage on SiPM = $SiPMVoltage"
  echo "Name of file number ${n_run}"
  filename=$prefix1$SiPMVoltageTitle$postfix
  echo $filename
done