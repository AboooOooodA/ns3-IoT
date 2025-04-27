import datetime
import sys

nbNode=int(sys.argv[1])
model=sys.argv[5]
distance="D100"
pcktSize="P64"
nbRetransmission="M"+sys.argv[4]
traffic="T"+sys.argv[3]
seed="seed"+sys.argv[2]


resultFile=model+"EnergyDelay"+"N"+str(nbNode)+distance+nbRetransmission+pcktSize+traffic+seed+".out"


resStream=open("results/Trash/"+resultFile,"w")
fichier = open("logs/"+model+"N"+str(nbNode)+distance+nbRetransmission+pcktSize+traffic+seed+".out", "r")

for line in fichier:

	#if ("retry counter:" in line or "NeedDataRetransmission count: 7" in line): 
		#resStream.write(line)#.split(" ")[0]+" "+line.split(" ")[2]+"\n")
		#print line#.split(" ")[0]+" "+line.split(" ")[2]
	#if (("Transmitting DATA frame PHY layer" in line and "Retry: false" in line) or "Receive OK FOR ME from Node[-1]" in line or "Transmitting CTL_ACK frame PHY layer" in line):
		#resStream.write(line)#.split(" ")[0]+" "+line.split(" ")[1]+"\n")
		#print line#.split(" ")[0]+" "+line.split(" ")[1]
	#if ("Transmitting DATA frame PHY layer to Node[0]" in line and "arpEnd" in line):
		#resStream.write(line)#.split(" ")[0]+" "+line.split(" ")[1]+"\n")
		#print line#.split(" ")[0]+" "+line.split(" ")[1]
	'''if ("ON APP" in line):
		resStream.write(line.split(" ")[0]+" "+line.split(" ")[7]+" "+line.split(" ")[9]+" "+line.split(" ")[-1]+"\n")
		#print line.split(" ")[0]+" "+line.split(" ")[7]+" "+line.split(" ")[9]+" "+line.split(" ")[-1]'''

	if ("WakingUp" in line and " Transmitting DATA frame PHY layer to Node[0]" in line):
		resStream.write(line.split(" ")[0]+" "+line.split(" ")[1]+" "+line.split(" ")[-1]+"\n")
		#print line.split(" ")[0]+" "+line.split(" ")[1]+" "+line.split(" ")[-1]

	if ("GoBackToSleep" in line and "Node[0]" not in line):
		resStream.write(line.split(" ")[0]+" "+line.split(" ")[2]+" "+line.split(" ")[-1]+"\n")
		#print line.split(" ")[0]+" "+line.split(" ")[2]+" "+line.split(" ")[-1]



fichier.close()
resStream.close()

resStream2=open("results/EnergyDelay/N"+str(nbNode)+"/"+resultFile,"w")
deltaMeanTable=[0 for i in range(nbNode)]

resStream2.write("\nAloha Nodes: "+str(nbNode)+" "+distance+" "+pcktSize+" "+nbRetransmission+" "+traffic+" "+seed+"\n")
print("\n *** Aloha Nodes: "+str(nbNode)+" "+distance+" "+pcktSize+" "+nbRetransmission+" "+traffic+" "+seed+" ***") 

'''for i in range(nbNode):
	cpt=0
	cpt2=0
	wakeUpSum=0
	sleepSum=0
	readStream=open("results/Trash/"+resultFile,"r")
	for line in readStream:
		if "["+str(i)+"]" in line and "ON APP" in line:
			cpt=cpt+1
			timeChaineWakeUp = line.split(" ")[0].replace("+","").replace("n","").replace("s","")
			wakeUpSum=wakeUpSum+float(timeChaineWakeUp)
		elif "["+str(i)+"]" in line and ("GoBackToSleep" in line or "GoBackToSleepFrameDrop" in line):
			cpt2=cpt2+1
			timeChaineSleep = line.split(" ")[0].replace("+","").replace("n","").replace("s","")
			sleepSum=sleepSum+float(timeChaineSleep)
	#resStream2.write("Node["+str(i)+"]:     cpt:"+str(cpt)+"      cpt2:"+str(cpt2)+"\n")
	#print "Node["+str(i)+"]:     cpt:"+str(cpt)+"      cpt2:"+str(cpt2)	
	if cpt==cpt2:
		#print ("identical")
		if cpt!=0:
			deltaMeanTable[i]=(float(sleepSum)-float(wakeUpSum))#/float(cpt)
			#resStream2.write("awake time avg Node["+str(i)+"]: "+ str(deltaMeanTable[i])+"ns\n")
			#print "awake time Node["+str(i)+"]: "+ str(deltaMeanTable[i])+"ns"
	else:
		#print ("counters not identical")
		if cpt!=0:
			#sleepSum = sleepSum+float(1000000000000)
			wakeUpSum = wakeUpSum-float(timeChaineWakeUp)
			deltaMeanTable[i]=(float(sleepSum)-float(wakeUpSum))#/float(cpt)
			resStream2.write("awake time avg Node["+str(i)+"]: "+ str(deltaMeanTable[i])+"ns\n")
			print "awake time Node["+str(i)+"]: "+ str(deltaMeanTable[i])+"ns"
'''	

for i in range(nbNode):
	readStream=open("results/Trash/"+resultFile,"r")
	listOn=[]
	listSleep=[]
	typeSleep=[]	
	for line in readStream:
		if "["+str(i)+"]" in line and "WakingUp" in line:
			listOn.append(float(line.split(" ")[0].replace("+","").replace("n","").replace("s","")))
		elif "["+str(i)+"]" in line and "GoBackToSleep" in line:
			listSleep.append(float(line.split(" ")[0].replace("+","").replace("n","").replace("s","")))
			typeSleep.append(1)
	
	cpt=0
	somme=0
	for j in range(len(typeSleep)):
		if typeSleep[j]==1:
			cpt=cpt+1
			somme=somme+listSleep[j]-listOn[j]
	if cpt!=0:deltaMeanTable[i]=float(somme)
	#if cpt!=58: print "\n awake time avg Node["+str(i)+"]: "+str(deltaMeanTable[i])+"ns "+str(cpt)
	#if i==172: resStream2.write("\n hhh"+str(somme)+" cpt: "+str(cpt)+"\n")


	readStream.close()

for k in range(nbNode):
	if k==0: 
		#print "\nTotal awake time avg Node["+str(k)+"]: "+str(sum(deltaMeanTable)/float(nbNode))+"ns "+str(cpt)
		resStream2.write("\nTotal awake time avg Node["+str(k)+"]: "+str(sum(deltaMeanTable)/float(nbNode*1000000))+"ms\n")
	#else:
		#print "EndToEnd Delay avg Node["+str(k)+"]: "+str(endToEndDelay[k])+"ns"
		#resStream2.write("EndToEnd Delay avg Node["+str(k)+"]: "+str(endToEndDelay[k])+"ns"+"\n")




'''

endToEndDelay=[0 for i in range(nbNode)]
for i in range(nbNode):
	readStreamEnd=open("results/Trash/"+resultFile,"r")
	listOn=[]
	listSleep=[]
	typeSleep=[]	
	for line in readStreamEnd:
		if "["+str(i)+"]" in line and "WakingUp" in line:
			listOn.append(float(line.split(" ")[0].replace("+","").replace("n","").replace("s","")))
		elif "["+str(i)+"]" in line and "GoBackToSleep" in line and "GoBackToSleepFrameDrop" not in line:
			listSleep.append(float(line.split(" ")[0].replace("+","").replace("n","").replace("s","")))
			typeSleep.append(1)
		elif "["+str(i)+"]" in line and "GoBackToSleepFrameDrop" in line:
			listSleep.append(float(line.split(" ")[0].replace("+","").replace("n","").replace("s","")))
			typeSleep.append(0)
	
	cpt=0
	somme=0
	for j in range(len(typeSleep)):
		if typeSleep[j]==1:
			cpt=cpt+1
			somme=somme+listSleep[j]-listOn[j]
	if cpt!=0:endToEndDelay[i]=float(somme)/float(cpt)
	#if i==172: resStream2.write("\n hhh"+str(somme)+" cpt: "+str(cpt)+"\n")


	readStreamEnd.close()

for k in range(nbNode):
	if k==0: 
		#print "\nTotal EndToEnd Delay avg Node["+str(k)+"]: "+str(sum(endToEndDelay)/float(nbNode))+"ns\n"
		resStream2.write("\nTotal EndToEnd Delay avg Node["+str(k)+"]: "+str(sum(endToEndDelay)/float(nbNode*1000000))+"ms\n")#the 1000000 to get the value in ms not in ns ;)
	#else:
		#print "EndToEnd Delay avg Node["+str(k)+"]: "+str(endToEndDelay[k])+"ns"
		#resStream2.write("EndToEnd Delay avg Node["+str(k)+"]: "+str(endToEndDelay[k])+"ns"+"\n")'''

resStream2.close()



