#!/bin/bash

#directory in which the experiment files will be saved
OUTPUT_BASE=experiments/

PLAYERS=("PortfolioGreedySearch 0 NOKDPS 1 0" "PortfolioOnlineEvolution 25 NOKDPS 1 0" "AlphaBeta 25 20 ScriptFirst Playout NOKDPS NOKDPS Alternate NOKDPS" "StratifiedPolicySearch 0 NOKDPS 1 0" "UCT 20 1.6 5000 20 ScriptFirst Playout NOKDPS NOKDPS Alternate NOKDPS")
BASE_COMBAT="State SeparatedState 1000 128 128 400 360 840 360"
COMBAT_UNITS=("Protoss_Dragoon 2 Protoss_Zealot 2" "Protoss_Dragoon 4 Protoss_Zealot 4" "Protoss_Dragoon 8 Protoss_Zealot 8" "Protoss_Dragoon 16 Protoss_Zealot 16" "Protoss_Dragoon 32 Protoss_Zealot 32" "Protoss_Dragoon 2" "Protoss_Dragoon 4" "Protoss_Dragoon 8" "Protoss_Dragoon 16" "Protoss_Dragoon 32" "Protoss_Dragoon 64" "Protoss_Zealot 2" "Protoss_Zealot 4" "Protoss_Zealot 8" "Protoss_Zealot 16" "Protoss_Zealot 32" "Protoss_Zealot 64")

NUMBER_PLAYERS=${#PLAYERS[@]}

for (( i=0; i<${NUMBER_PLAYERS}; i++ ));
do
	for (( j=$i; j<${NUMBER_PLAYERS}; j++ ));
	do
		if [ $i == $j ]; then
			continue
		fi
	
		for COMBAT in "${COMBAT_UNITS[@]}" 
		do
			#######file where the details of the experiment are stored#######
			COMBAT_NO_SPACE="$(echo -e "${COMBAT}" | tr -d '[:space:]')"
			PLAYER_0_NO_SPACE="$(echo -e "${PLAYERS[$i]}" | tr -d '[:space:]')"
			PLAYER_1_NO_SPACE="$(echo -e "${PLAYERS[$j]}" | tr -d '[:space:]')"
			
			OUTPUT=${OUTPUT_BASE}${PLAYER_0_NO_SPACE}"_versus_"${PLAYER_1_NO_SPACE}"_"${COMBAT_NO_SPACE}".txt"						
			
			#######creating the file with experiment details#######
			echo "Player 0 "${PLAYERS[$i]} >> ${OUTPUT}
			echo "Player 1 "${PLAYERS[$j]} >> ${OUTPUT}
			
			echo ${BASE_COMBAT}" "${COMBAT} >> ${OUTPUT}
			
			SUMMARY_OUTPUT=${OUTPUT_BASE}"results/"${PLAYER_0_NO_SPACE}"_versus_"${PLAYER_1_NO_SPACE}"_"${COMBAT_NO_SPACE}
			echo "ResultsFile "${SUMMARY_OUTPUT}" true" >> ${OUTPUT}
			
			#######sending process to cluster's queue#######
			#(qsub -l nodes=1:STANDARD:ppn=1,mem=1gb -v FILENAME=$OUTPUT single_experiment.sh) &
		done
	done
done

