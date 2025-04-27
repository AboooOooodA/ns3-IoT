nbNode=100
model="Aloha"
distance="D100"
pcktSize="P64"
nbRetransmission="M7"
traffic="T0v5"
retrySumList=[0 for i in range(nbNode)]
nbRetryList=[0 for i in range(nbNode)]
averageList=[0 for i in range (nbNode)]

resultFile="Retry"+model+"N"+str(nbNode)+distance+pcktSize+traffic+".out"





resStream=open("results/"+resultFile,"w")
fichier = open("logs/"+model+"N"+str(nbNode)+distance+nbRetransmission+pcktSize+traffic+".out", "r")

for line in fichier:

	if ("retry counter:" in line):
		resStream.write(line.split(" ")[2]+" "+line.split(" ")[5])
		print line.split(" ")[2]+" "+line.split(" ")[5]
		rowIndice=int(line.split(" ")[2].split("[")[1].replace("]",""))
		retries=int(line.split(" ")[5])
		retrySumList[rowIndice]=retrySumList[rowIndice]+retries
		nbRetryList[rowIndice]=nbRetryList[rowIndice]+1
print "============================================"
for j in range(nbNode):
	resStream.write("Node["+str(j)+"]: "+str(retrySumList[j])+"   "+str(nbRetryList[j])+" "+str(float(retrySumList[j])/float(nbRetryList[j])))
	print "Node["+str(j)+"]: "+str(retrySumList[j])+"   "+str(nbRetryList[j])+" "+str(float(retrySumList[j])/float(nbRetryList[j]))
resStream.close()
fichier.close()
