#!/bin/bash

EXE=./bin/project2

TRACE_1=./traces/fpppp.tra
TRACE_3=./traces/perl.tra
TRACE_2=./traces/mpeg2d.tra
#TRACE=./traces/short.tra
#TRACE=./traces/applu.tra
#TRACE=./traces/compress.tra
#TRACE=./traces/epic.tra
#TRACE=./traces/ijpeg.tra
#TRACE=./traces/mpeg2e.tra
#TRACE=./traces/pegwitd.tra
#TRACE=./traces/toast.tra

SS_FACTOR=4
RS_STATION_SIZE=4
FU_COUNT=4
RENAME_TABLE_SIZE=17
ROB_SIZE=29
BTB_SIZE=64
IC_HR=98
L1_HR=95
L1_AT=1
L2_HR=70
L2_AT=6
MM_AT=60

$EXE -ss $SS_FACTOR -btb $BTB_SIZE -rs $RS_STATION_SIZE -fu $FU_COUNT \
-rnt $RENAME_TABLE_SIZE -rob $ROB_SIZE -l1hr $L1_HR -l1at $L1_AT \
-l2hr $L2_HR -l2at $L2_AT -trace $TRACE_1 -inshr $IC_HR -mmat $MM_AT | tee final_run_output.log 

$EXE -ss $SS_FACTOR -btb $BTB_SIZE -rs $RS_STATION_SIZE -fu $FU_COUNT \
-rnt $RENAME_TABLE_SIZE -rob $ROB_SIZE -l1hr $L1_HR -l1at $L1_AT \
-l2hr $L2_HR -l2at $L2_AT -trace $TRACE_2 -inshr $IC_HR -mmat $MM_AT | tee -a final_run_output.log 

$EXE -ss $SS_FACTOR -btb $BTB_SIZE -rs $RS_STATION_SIZE -fu $FU_COUNT \
-rnt $RENAME_TABLE_SIZE -rob $ROB_SIZE -l1hr $L1_HR -l1at $L1_AT \
-l2hr $L2_HR -l2at $L2_AT -trace $TRACE_3 -inshr $IC_HR -mmat $MM_AT | tee -a final_run_output.log 

echo "Ended after $SECONDS seconds"
echo "In minutes:"
echo "scale=3; $SECONDS/60" | bc

