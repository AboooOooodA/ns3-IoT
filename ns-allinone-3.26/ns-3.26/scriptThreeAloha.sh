#run the same simulation 10 times and changing the seed value

#run the first simulation with seed1 and sleep for 30 seconds
nbNode=100
traffic=0.1
seedInit=1
runSimInit="./waf --run \"scratch/myAloha/myAloha --nbrNodes="$nbNode" --seedNumber="$seedInit" --distance=100 --run=AlohaSCRIPTN"$nbNode"D100M3P64T"$traffic"seed"$seedInit"\"> scratch/myAloha/logs/AlohaSCRIPTN"$nbNode"D100M3P64T"$traffic"seed"$seedInit".out 2>&1"
screen -d -m -S "AlohaSCRIPTN"$nbNode"D100M3P64T"$traffic"seed"$seedInit
screen -S "AlohaSCRIPTN"$nbNode"D100M3P64T"$traffic"seed"$seedInit -X stuff "$runSimInit\n"

sleep 50

for ((i = 2 ; i < 11 ; i++ ));do
	seed=$(printf '%d' $i)

	#run the simulation	
	runSim="./waf --run \"scratch/myAloha/myAloha --nbrNodes="$nbNode" --seedNumber="$seed" --distance=100 --run=AlohaSCRIPTN"$nbNode"D100M3P64T"$traffic"seed"$seed"\"> scratch/myAloha/logs/AlohaSCRIPTN"$nbNode"D100M3P64T"$traffic"seed"$seed".out 2>&1"
	
	cool=0
	zero=0

	while (($cool <= 0))
	do
		if screen -d -m -S "AlohaSCRIPTN"$nbNode"D100M3P64T"$traffic"seed"$seed ; then
			echo "cool"
			cool=1
		else
			echo "NOT cool"
		fi
	done 	

	screenFound=0
	
	while (($screenFound <= 0))
	do
		if screen -S "AlohaSCRIPTN"$nbNode"D100M3P64T"$traffic"seed"$seed -X stuff "$runSim\n"; then
			echo "screen found"
			screenFound=1
		else
			echo "scrren NOT found"
		fi
	done
	
done

