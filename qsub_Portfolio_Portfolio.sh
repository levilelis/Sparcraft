#!/bin/bash

#directory in which the experiment files will be saved
OUTPUT_BASE=experiments/

#######player to be tested#######
PLAYER_0="PortfolioGreedySearch" 
PARAM_0_1=" 0 NOKDPS "
ITERATIONS_0=(1 2)
RESPONSES_0=(0 1 2)

#######enemy to be played against#######
PLAYER_1="PortfolioGreedySearch" 
PARAM_1_1=" 0 NOKDPS "
ITERATIONS_1=(1 2)
RESPONSES_1=(0 1 2)

BASE_COMBAT="State SeparatedState 1000 128 128 400 360 840 360"
COMBAT_UNITS=("Protoss_Dragoon 1 Protoss_Zealot 1" "Protoss_Dragoon 2 Protoss_Zealot 2" "Protoss_Dragoon 4 Protoss_Zealot 4" "Protoss_Dragoon 8 Protoss_Zealot 8" "Protoss_Dragoon 16 Protoss_Zealot 16")
#COMBAT_UNITS2=("Protoss_Dragoon 2" "Protoss_Dragoon 4" "Protoss_Dragoon 8" "Protoss_Dragoon 16" "Protoss_Dragoon 32")
#COMBAT_UNITS2=("Protoss_Zealot 2" "Protoss_Zealot 4" "Protoss_Zealot 8" "Protoss_Zealot 16" "Protoss_Zealot 32")


for it_0 in "${ITERATIONS_0[@]}" 
do
	for resp_0 in "${RESPONSES_0[@]}" 
	do
		for it_1 in "${ITERATIONS_1[@]}" 
		do
			for resp_1 in "${RESPONSES_1[@]}" 
			do
				for combat in "${COMBAT_UNITS[@]}" 
				do
					#######file where the details of the experiment are stored#######
					COMBAT_NO_SPACE="$(echo -e "${combat}" | tr -d '[:space:]')"
					OUTPUT=${OUTPUT_BASE}${PLAYER_0}"_"${it_0}"_"${resp_0}"_versus_"${PLAYER_1}"_"${it_1}"_"${resp_1}"_"${COMBAT_NO_SPACE}".txt"						
					
					#######creating the file with experiment details#######
					echo "Player 0 "${PLAYER_0}" "${PARAM_0_1}" "${it_0}" "${resp_0} >> ${OUTPUT}
					echo "Player 1 "${PLAYER_1}" "${PARAM_1_1}" "${it_1}" "${resp_1} >> ${OUTPUT}
					
					echo ${BASE_COMBAT}" "${combat} >> ${OUTPUT}
					
					SUMMARY_OUTPUT=${OUTPUT_BASE}"results/"${PLAYER_0}"_"${it_0}"_"${resp_0}"_versus_"${PLAYER_1}"_"${it_1}"_"${resp_1}"_"${COMBAT_NO_SPACE}
					echo "ResultsFile "${SUMMARY_OUTPUT}" true" >> ${OUTPUT}
					
					#######sending process to cluster's queue#######
					(qsub -l nodes=1:STANDARD:ppn=1,mem=1gb -v FILENAME=$OUTPUT single_experiment.sh) &
				done
			done
		done
	done
done

