#!/bin/sh

EXE=./bin/project2

TRACE=./traces/perl.tra

SS_FACTOR=4
BTB_SIZE=64
RS_STATION_SIZE=8
FU_COUNT=2
RENAME_TABLE_SIZE=16
ROB_SIZE=10
IC_HR=98
L1_HR=95
L1_AT=1
L2_HR=70
L2_AT=6
MM_AT=60

$EXE --ss $SS_FACTOR --btb $BTB_SIZE --rs $RS_STATION_SIZE -fu $FU_COUNT \
--rnt $RENAME_TABLE_SIZE --rob $ROB_SIZE --l1hr $L1_HR --l1at $L1_AT \
--l2hr $L2_HR --l2at $L2_AT --trace $TRACE --inshr $IC_HR --mmat $MM_AT | tee run_output.log



