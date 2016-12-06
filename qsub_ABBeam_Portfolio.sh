#!/bin/bash

#directory in which the experiment files will be saved
OUTPUT_BASE=experiments/

#######player to be tested#######
PLAYER_0="AdaptiveBeamAlphaBeta" 
PARAM_0_1="20 ScriptFirst Playout NOKDPS NOKDPS Alternate"
OP_MODEL=("None" "NOKDPS")
SEARCH_TIME=(10)
BEAMS=(1)

#######enemy to be played against#######
PLAYER_1="PortfolioGreedySearch" 
PARAM_1_1=" 0 NOKDPS 1 0"

BASE_COMBAT="State SeparatedState 2 128 128 400 360 840 360"
COMBAT_UNITS=("Protoss_Dragoon" "Protoss_Zealot")
NUMBER_UNITS=(2)

for op in "${OP_MODEL[@]}" 
do
	for beam in "${BEAMS[@]}" 
	do
		for t in "${SEARCH_TIME[@]}" 
		do
			for units in "${NUMBER_UNITS[@]}" 
			do
				#######file where the details of the experiment are stored#######
				OUTPUT=${OUTPUT_BASE}${PLAYER_0}"_beam"${beam}"_time"${t}"_opponent"${op}"_versus_"${PLAYER_1}"_"${COMBAT_UNITS[0]}"_"${units}"_"${COMBAT_UNITS[1]}"_"${units}".txt"	
				
				#######creating the file with experiment details#######
				POLICY_OUTPUT=${OUTPUT_BASE}"policies/"${PLAYER_0}"_beam"${beam}"_time"${t}"_opponent"${op}"_versus_"${PLAYER_1}"_"${COMBAT_UNITS[0]}"_"${units}"_"${COMBAT_UNITS[1]}"_"${units}".txt"
				echo "Player 0 "${PLAYER_0}" "${t}" "${PARAM_0_1}" "${op}" "${beam}" 1 "${POLICY_OUTPUT} >> ${OUTPUT}
				echo "Player 1 "${PLAYER_1}" "${PARAM_1_1} >> ${OUTPUT}
				
				echo ${BASE_COMBAT}" "${COMBAT_UNITS[0]}" "${units}" "${COMBAT_UNITS[1]}" "${units} >> ${OUTPUT}
				
				SUMMARY_OUTPUT=${OUTPUT_BASE}"results/"${PLAYER_0}"_beam"${beam}"_time"${t}"_opponent"${op}"_versus_"${PLAYER_1}"_"${COMBAT_UNITS[0]}"_"${units}"_"${COMBAT_UNITS[1]}"_"${units}".txt"
				RESULTS_FILE=${OUTPUT_BASE}"_"results"_"${PLAYER_0}"_"${beam}"_"${t}"_"${op}".txt"	
				echo "ResultsFile "${SUMMARY_OUTPUT}" true" >> ${OUTPUT}
				
				#######sending process to cluster's queue#######
				(qsub -l nodes=1:STANDARD:ppn=1,mem=1gb -v FILENAME=$OUTPUT single_experiment.sh) &
				#(qsub -l select=1:ncpus=1:mem=1GB -v FILENAME=$FILENAME single_experiment.sh) &
				#./bin/SparCraft ${OUTPUT}
			done
		done
	done
done

