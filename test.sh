#!/bin/sh

EXE=./bin/project2

TRACE=./traces/compress.tra

SS_FACTOR=1
BTB_SIZE=1
RS_STATION_SIZE=1
FU_COUNT=1
RENAME_TABLE_SIZE=1
ROB_SIZE=10
IC_HR=97
IC_AT=5
L1_HR=98
L1_AT=15
L2_HR=99
L2_AT=15

$EXE --ss $SS_FACTOR --btb $BTB_SIZE --rs $RS_STATION_SIZE -fu $FU_COUNT \
--rnt $RENAME_TABLE_SIZE --rob $ROB_SIZE --l1hr $L1_HR --l1at $L1_AT \
--l2hr $L2_HR --l2at $L2_AT --trace $TRACE --inshr $IC_HR --insat $IC_AT



