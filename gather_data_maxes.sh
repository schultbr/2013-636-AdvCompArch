#!/bin/bash

#fpppp.tra
#perl.tra
#mpeg2d.tra

echo "Starting at:"
date

rm -rf ./simulationResults

mkdir -pv ./simulationResults/fpppp/ssf
mkdir -pv ./simulationResults/fpppp/rs_entries
mkdir -pv ./simulationResults/fpppp/fu_count
mkdir -pv ./simulationResults/fpppp/rrf_size
mkdir -pv ./simulationResults/fpppp/rob_size

mkdir -pv ./simulationResults/perl/ssf
mkdir -pv ./simulationResults/perl/rs_entries
mkdir -pv ./simulationResults/perl/fu_count
mkdir -pv ./simulationResults/perl/rrf_size
mkdir -pv ./simulationResults/perl/rob_size

mkdir -pv ./simulationResults/mpeg/ssf
mkdir -pv ./simulationResults/mpeg/rs_entries
mkdir -pv ./simulationResults/mpeg/fu_count
mkdir -pv ./simulationResults/mpeg/rrf_size
mkdir -pv ./simulationResults/mpeg/rob_size

#MAXES (or defaults to use... change these when running test 2 with all optimal values)
SSF_TEST=8
RS_TEST=8
FU_TEST=8
RRF_TEST=64
ROB_TEST=64

# 2 4 6 8 0
#SWEEP RANGES
SSF_OPTS="1 2 3 4 5 6 7 8"
RS_OPTS="1 2 3 4 5 6 7 8"
FU_OPTS="1 2 3 4 5 6 7 8"
RRF_OPTS="4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50 52 54 56 58 60 62 64"
ROB_OPTS="4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50 52 54 56 58 60 62 64"

TRACE_FPPPP="./traces/fpppp.tra"
TRACE_PERL="./traces/perl.tra"
TRACE_MPEG="./traces/mpeg2d.tra"

# CONSTANTS 
EXE=./bin/project2
IC_HR=98
L1_HR=95
L1_AT=1
L2_HR=70
L2_AT=6
MM_AT=60
BTB_SIZE=64 

OUT_DIR_F=./simulationResults/fpppp/ssf
OUT_DIR_P=./simulationResults/perl/ssf
OUT_DIR_M=./simulationResults/mpeg/ssf

for ssf in `echo $SSF_OPTS`
do
	$EXE --ss $ssf --rs $RS_TEST --fu $FU_TEST --rnt $RRF_TEST --rob $ROB_TEST --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_FPPPP --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_F/ssf_$ssf.log 

	$EXE --ss $ssf --rs $RS_TEST --fu $FU_TEST --rnt $RRF_TEST --rob $ROB_TEST --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_PERL --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_P/ssf_$ssf.log 

	$EXE --ss $ssf --rs $RS_TEST --fu $FU_TEST --rnt $RRF_TEST --rob $ROB_TEST --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_MPEG --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_M/ssf_$ssf.log 
done


OUT_DIR_F=./simulationResults/fpppp/rs_entries
OUT_DIR_P=./simulationResults/perl/rs_entries
OUT_DIR_M=./simulationResults/mpeg/rs_entries

for rs in `echo $RS_OPTS`
do
	$EXE --ss $SSF_TEST --rs $rs --fu $FU_TEST --rnt $RRF_TEST --rob $ROB_TEST --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_FPPPP --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_F/rs_$rs.log

	$EXE --ss $SSF_TEST --rs $rs --fu $FU_TEST --rnt $RRF_TEST --rob $ROB_TEST --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_PERL --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_P/rs_$rs.log

	$EXE --ss $SSF_TEST --rs $rs --fu $FU_TEST --rnt $RRF_TEST --rob $ROB_TEST --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_MPEG --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_M/rs_$rs.log
done


OUT_DIR_F=./simulationResults/fpppp/fu_count
OUT_DIR_P=./simulationResults/perl/fu_count
OUT_DIR_M=./simulationResults/mpeg/fu_count

for fu in `echo $FU_OPTS`
do
	$EXE --ss $SSF_TEST --rs $RS_TEST --fu $fu --rnt $RRF_TEST --rob $ROB_TEST --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_FPPPP --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_F/fu_$fu.log

	$EXE --ss $SSF_TEST --rs $RS_TEST --fu $fu --rnt $RRF_TEST --rob $ROB_TEST --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_PERL --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_P/fu_$fu.log

	$EXE --ss $SSF_TEST --rs $RS_TEST --fu $fu --rnt $RRF_TEST --rob $ROB_TEST --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_MPEG --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_M/fu_$fu.log
done


#mkdir -pv ./simulationResults/mpeg/rrf_size

OUT_DIR_F=./simulationResults/fpppp/rrf_size
OUT_DIR_P=./simulationResults/perl/rrf_size
OUT_DIR_M=./simulationResults/mpeg/rrf_size

for rrf in `echo $RRF_OPTS`
do
	$EXE --ss $SSF_TEST --rs $RS_TEST --fu $FU_TEST --rnt $rrf --rob $ROB_TEST --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_FPPPP --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_F/rrf_$rrf.log

	$EXE --ss $SSF_TEST --rs $RS_TEST --fu $FU_TEST --rnt $rrf --rob $ROB_TEST --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_PERL --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_P/rrf_$rrf.log

	$EXE --ss $SSF_TEST --rs $RS_TEST --fu $FU_TEST --rnt $rrf --rob $ROB_TEST --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_MPEG --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_M/rrf_$rrf.log
done



#mkdir -pv ./simulationResults/mpeg/rob_size

OUT_DIR_F=./simulationResults/fpppp/rob_size
OUT_DIR_P=./simulationResults/perl/rob_size
OUT_DIR_M=./simulationResults/mpeg/rob_size

for rob in `echo $ROB_OPTS`
do
	$EXE --ss $SSF_TEST --rs $RS_TEST --fu $FU_TEST --rnt $RRF_TEST --rob $rob --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_FPPPP --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_F/rob_$rob.log

	$EXE --ss $SSF_TEST --rs $RS_TEST --fu $FU_TEST --rnt $RRF_TEST --rob $rob --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_PERL --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_P/rob_$rob.log

	$EXE --ss $SSF_TEST --rs $RS_TEST --fu $FU_TEST --rnt $RRF_TEST --rob $rob --btb $BTB_SIZE --l1hr $L1_HR --l1at $L1_AT --l2hr $L2_HR --l2at $L2_AT --trace $TRACE_MPEG --inshr $IC_HR --mmat $MM_AT | tee $OUT_DIR_M/rob_$rob.log
done


grep -R IPC simulationResults/ | tee ./simulationResults/IPC_results.log

#grep -R ROB simulationResults/ | tee ./simulationResults/ROB_results.log
#grep -R RRF simulationResults/ | tee ./simulationResults/RRF_results.log
#grep -R "RS INT" simulationResults/ | tee ./simulationResults/RS_INT_results.log
#grep -R "RS FP" simulationResults/ | tee ./simulationResults/RS_FP_results.log
#grep -R "RS MEM" simulationResults/ | tee ./simulationResults/RS_MEM_results.log
#grep -R "RS BR" simulationResults/ | tee ./simulationResults/RS_BR_results.log
#grep -R "FU ADD" simulationResults/ | tee ./simulationResults/FU_ADD_results.log
#grep -R "FU MULT" simulationResults/ | tee ./simulationResults/FU_MULT_results.log
#grep -R "FU FP" simulationResults/ | tee ./simulationResults/FU_FP_results.log
#grep -R "FU BR" simulationResults/ | tee ./simulationResults/FU_BR_results.log









echo "Ended at:"
date
#let MIN = echo "$SECONDS/60" | bc
echo "Test took $SECONDS seconds"
echo "In minutes:"
echo "scale=3; $SECONDS/60" | bc

