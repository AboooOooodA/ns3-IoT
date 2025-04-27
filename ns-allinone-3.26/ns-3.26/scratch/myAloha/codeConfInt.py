import sys

nbNode=int(sys.argv[1])
model=sys.argv[5]+"ProbaRetry"
distance="D100"
pcktSize="P64"
nbRetransmission="M"+sys.argv[4]
traffic="T"+sys.argv[3]
seed="seed"
seedMax=int(sys.argv[2])

probaConfInt=model+"N"+str(nbNode)+distance+pcktSize+traffic+"ConfInt.out"

print("\nAloha Nodes: "+str(nbNode)+" "+distance+" "+pcktSize+" "+nbRetransmission+" "+traffic+"\n")  

probaFrames=[0 for i in range(seedMax)]
probaPckts=[0 for i in range(seedMax)]
avgRetry=[0 for i in range(seedMax)]

for index in range(seedMax):
	fichier = open("results/N/"+str(distance)+"/N"+str(nbNode)+"/"+model+"N"+str(nbNode)+distance+pcktSize+traffic+seed+str(index+1)+".out", "r")
	for line in fichier:	
		if ("Node[0]" in line and "Proba succes:" in line):
			probaFrames[index]=float(line.split(" ")[10].replace("]",""))
			probaPckts[index]=float(line.split(" ")[22].replace("]",""))
			avgRetry[index]=float(line.split(" ")[12])
			print ("Frames Succes Proba (seed = "+str(index+1)+"): "+str(probaFrames[index])+"\tPackets Succes Proba (seed = "+str(index+1)+"): "+str(probaPckts[index])+"\tAverage Retry: "+str(avgRetry[index]))
	fichier.close()			



def moyenne(tableau):
    return sum(tableau, 0.0) / len(tableau)



def variance(tableau):
    m=moyenne(tableau)
    return moyenne([(x-m)**2 for x in tableau])

#print(variance(probaFrames))

def ecartype(tableau):
    return variance(tableau)**0.5


probaStream=open("results/N/"+str(distance)+"/N"+str(nbNode)+"/"+probaConfInt,"w")
probaStream.write("Aloha Nodes: "+str(nbNode)+" "+distance+" "+pcktSize+" "+nbRetransmission+" "+traffic+"\n\n")

print("\nFrames Succes Proba:")

#print("Sigma: "+str(ecartype(probaFrames)))

print("Mp: "+str(moyenne(probaFrames)))
probaStream.write("Mp: "+str(moyenne(probaFrames))+"\n")

print("Cinf: "+str(moyenne(probaFrames)-(1.96*(ecartype(probaFrames)/len(probaFrames)**0.5))))
probaStream.write("Cinf: "+str(moyenne(probaFrames)-(1.96*(ecartype(probaFrames)/len(probaFrames)**0.5)))+"\n")

print("Csup: "+str(moyenne(probaFrames)+(1.96*(ecartype(probaFrames)/len(probaFrames)**0.5))))
probaStream.write("Csup: "+str(moyenne(probaFrames)+(1.96*(ecartype(probaFrames)/len(probaFrames)**0.5)))+"\n")

print("\n\tAverage Retry:")
probaStream.write("\n\tAverage Retry:\n")

print("\tMp: "+str(moyenne(avgRetry)))
probaStream.write("\tMp: "+str(moyenne(avgRetry))+"\n")

print("\tCinf: "+str(moyenne(avgRetry)-(1.96*(ecartype(avgRetry)/len(avgRetry)**0.5))))
probaStream.write("\tCinf: "+str(moyenne(avgRetry)-(1.96*(ecartype(avgRetry)/len(avgRetry)**0.5)))+"\n")

print("\tCsup: "+str(moyenne(avgRetry)+(1.96*(ecartype(avgRetry)/len(avgRetry)**0.5))))
probaStream.write("\tCsup: "+str(moyenne(avgRetry)+(1.96*(ecartype(avgRetry)/len(avgRetry)**0.5)))+"\n")

print("\nPackets Succes Proba:")
probaStream.write("\nPackets Succes Proba:\n")

#print("Sigma: "+str(ecartype(probaPckts)))

print("Mp: "+str(moyenne(probaPckts)))
probaStream.write("Mp: "+str(moyenne(probaPckts))+"\n")

print("Cinf: "+str(moyenne(probaPckts)-(1.96*(ecartype(probaPckts)/len(probaPckts)**0.5))))
probaStream.write("Cinf: "+str(moyenne(probaPckts)-(1.96*(ecartype(probaPckts)/len(probaPckts)**0.5)))+"\n")

print("Csup: "+str(moyenne(probaPckts)+(1.96*(ecartype(probaPckts)/len(probaPckts)**0.5))))
probaStream.write("Csup: "+str(moyenne(probaPckts)+(1.96*(ecartype(probaPckts)/len(probaPckts)**0.5)))+"\n")

probaStream.close()
