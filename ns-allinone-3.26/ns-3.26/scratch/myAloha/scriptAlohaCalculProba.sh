

nbNode=250
traffic=0.1
retransmission=3
model="AlohaSCRIPT"

for ((i = 1 ; i < 11 ; i++ ));do

	#run the the python	
	runPython="python codeAloha.py "$nbNode" "$i" "$traffic" "$retransmission" "$model
	#screen -d -m -S "PythonAlohaN"$nbNode"D100M7P64T0v5seed"$i
	#screen -S "PythonAlohaN"$nbNode"D100M7P64T0v5seed"$i -X stuff "$runPython\n" 
	eval $runPython
	
done

	runPythonConfInt="python codeConfInt.py "$nbNode" "$((i-1))" "$traffic" "$retransmission" "$model
	eval $runPythonConfInt

