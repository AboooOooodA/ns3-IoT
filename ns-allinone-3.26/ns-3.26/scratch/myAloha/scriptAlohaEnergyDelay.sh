

nbNode=1000
traffic=0.1
retransmission=7
model="AlohaSCRIPT"


for ((i = 2 ; i < 11 ; i++ ));do

	#run the the python	
	runPython="python codeDelta.py "$nbNode" "$i" "$traffic" "$retransmission" "$model
	#screen -d -m -S "PythonAlohaN"$nbNode"D100M7P64T0v5seed"$i
	#screen -S "PythonAlohaN"$nbNode"D100M7P64T0v5seed"$i -X stuff "$runPython\n" 
	eval $runPython
	
done

	runPythonConfInt="python codeDeltaConfInter.py "$nbNode" "$((i-1))" "$traffic" "$retransmission" "$model
	eval $runPythonConfInt

