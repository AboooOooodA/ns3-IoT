import sys

nbNode=int(sys.argv[1])
model=sys.argv[5]
distance="D100"
pcktSize="P64"
nbRetransmission="M"+sys.argv[4]
traffic="T"+sys.argv[3]
seed="seed"+sys.argv[2]

resultFile=model+"ProbaRetry"+"N"+str(nbNode)+distance+pcktSize+traffic+seed+".out"

nbrRcvdFrames=[0 for i in range(nbNode)]
nbrSentFrames=[0 for i in range(nbNode)]

nbrRcvdPckts=[0 for i in range(nbNode)]
nbrSentPckts=[0 for i in range(nbNode)]

retrySumList=[0 for i in range(nbNode)]
nbRetryList=[0 for i in range(nbNode)]
averageList=[0 for i in range (nbNode)]
retryFailSumList=[0 for i in range(nbNode)]




resStream=open("results/N/"+str(distance)+"/N"+str(nbNode)+"/"+resultFile,"w")
fichier = open("logs/"+model+"N"+str(nbNode)+distance+nbRetransmission+pcktSize+traffic+seed+".out", "r")

for line in fichier:
	#code for frames
	if ("Node[0] [00:00:00:00:00:01] Receive OK FOR ME" in line) and ("DATA" in line) and ("arpEnd" in line):
		rcvdFrameSource=int(line.split(" ")[8].split("[")[1].replace("]",""))
		nbrRcvdFrames[rcvdFrameSource]=nbrRcvdFrames[rcvdFrameSource]+1

	if ("Transmitting DATA frame PHY layer to Node[0] [00:00:00:00:00:01]" in line) and ("arpEnd" in line):
		senderNodeID=int(line.split(" ")[1].split("[")[1].replace("]",""))
		nbrSentFrames[senderNodeID]=nbrSentFrames[senderNodeID]+1

	if ("retry counter:" in line) and ("arpEnd" in line):
		#resStream.write(line.split(" ")[2]+" "+line.split(" ")[5]+"\n")
		#print line.split(" ")[2]+" "+line.split(" ")[5]+" test*************************************************"
		rowIndice=int(line.split(" ")[2].split("[")[1].replace("]",""))
		#print rowIndice
		retries=int(line.split(" ")[5])
		#print retries
		retrySumList[rowIndice]=retrySumList[rowIndice]+retries
		nbRetryList[rowIndice]=nbRetryList[rowIndice]+1

	if("GoBackToSleepFrameDrop" in line):
		rowIndice=int(line.split(" ")[2].split("[")[1].replace("]",""))
		retries=int(sys.argv[4])
		retrySumList[rowIndice]=retrySumList[rowIndice]+retries
		nbRetryList[rowIndice]=nbRetryList[rowIndice]+1

	#code for packets
	#if ("APPLICATION LEVEL SENDER *****" in line) and ("ON APP" in line):
	if ("Transmitting DATA frame PHY layer to Node[0] [00:00:00:00:00:01]" in line) and ("arpEnd" in line)and ("WakingUp" in line):
		pcktSource=int(line.split(" ")[1].split("[")[1].replace("]",""))
		nbrSentPckts[pcktSource]=nbrSentPckts[pcktSource]+1

	if ("APPLICATION LEVEL Received" in line) and ("OFF APP" in line):
		rcvdPcktSource=int(line.split(" ")[8].split("[")[1].replace("]",""))
		nbrRcvdPckts[rcvdPcktSource]=nbrRcvdPckts[rcvdPcktSource]+1



for index in range(len(nbrRcvdFrames)):

	if nbRetryList[index]!=0: averageList[index] = float(retrySumList[index])/float(nbRetryList[index])

for ind in range(len(nbrRcvdFrames)):

	if(ind == 0):
		print "\nNode["+str(ind)+"] Tx frames: "+str(sum(nbrSentFrames))+" Rx frames: "+str(sum(nbrRcvdFrames))+" [Frame Proba succes: "+str(float(sum(nbrRcvdFrames))/float(sum(nbrSentFrames)))+"] AvgRetry: "+str(sum(averageList)/float(nbNode))+" Tx packets: "+str(sum(nbrSentPckts))+" Rx packets: "+str(sum(nbrRcvdPckts))+" [Packet Proba succes: "+str(float(sum(nbrRcvdPckts))/float(sum(nbrSentPckts)))+"] "+seed
		resStream.write("Node["+str(ind)+"] Tx frames: "+str(sum(nbrSentFrames))+" Rx frames: "+str(sum(nbrRcvdFrames))+" [Frame Proba succes: "+str(float(sum(nbrRcvdFrames))/float(sum(nbrSentFrames)))+"] AvgRetry: "+str(sum(averageList)/float(nbNode))+" Tx packets: "+str(sum(nbrSentPckts))+" Rx packets: "+str(sum(nbrRcvdPckts))+" [Packet Proba succes: "+str(float(sum(nbrRcvdPckts))/float(sum(nbrSentPckts)))+"] "+seed+"\n")
	else: 
		'''print "Node["+str(ind)+"] Tx frames: "+str(nbrSentFrames[ind])+" Rx frames: "+str(nbrRcvdFrames[ind])+" Proba succes: "+str(float(nbrRcvdFrames[ind])/float(nbrSentFrames[ind]))+" AvgRetry: "+str(averageList[ind])+" Tx packets: "+str(nbrSentPckts[ind])+" Rx packets: "+str(nbrRcvdPckts[ind])'''
		resStream.write("Node["+str(ind)+"] Tx frames: "+str(nbrSentFrames[ind])+" Rx frames: "+str(nbrRcvdFrames[ind])+" Proba succes: "+str(float(nbrRcvdFrames[ind])/float(nbrSentFrames[ind]))+" AvgRetry: "+str(averageList[ind])+" Tx packets: "+str(nbrSentPckts[ind])+" Rx packets: "+str(nbrRcvdPckts[ind])+"\n")

resStream.close()
fichier.close()
