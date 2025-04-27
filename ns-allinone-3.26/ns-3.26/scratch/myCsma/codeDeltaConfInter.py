import sys

nbNode=int(sys.argv[1])
model=sys.argv[5]+"EnergyDelay"
distance="D100"
pcktSize="P64"
nbRetransmission="M"+sys.argv[4]
traffic="T"+sys.argv[3]
seed="seed"
seedMax=int(sys.argv[2])

EnergyConfInt=sys.argv[5]+"Energy"+"N"+str(nbNode)+distance+nbRetransmission+pcktSize+traffic+"ConfInt.out"
DelayConfInt=sys.argv[5]+"AlohaDelay"+"N"+str(nbNode)+distance+nbRetransmission+pcktSize+traffic+"ConfInt.out"
#AlohaDelayConfInt="AlohaDelayNX"+distance+pcktSize+traffic+"ConfInt.out"
#AlohaEnergyConfInt="AlohaEnergyNX"+distance+pcktSize+traffic+"ConfInt.out"

print("\nAloha Nodes: "+str(nbNode)+" "+distance+" "+pcktSize+" "+nbRetransmission+" "+traffic+"\n")  

onTime=[0 for i in range(seedMax)]
delayTime=[0 for i in range(seedMax)]

for index in range(seedMax):
	fichier = open("results/EnergyDelay/N"+str(nbNode)+"/"+model+"N"+str(nbNode)+distance+nbRetransmission+pcktSize+traffic+seed+str(index+1)+".out", "r")
	for line in fichier:	
		if ("Total awake time avg Node[0]" in line):
			onTime[index]=float(line.split(" ")[5].replace("+","").replace("m","").replace("s",""))
		if ("Total EndToEnd Delay avg Node[0]" in line):		
			delayTime[index]=float(line.split(" ")[5].replace("+","").replace("m","").replace("s",""))
	print ("ON Time (seed = "+str(index+1)+"): "+str(onTime[index])+"\tEndToEnd Delay (seed = "+str(index+1)+"): "+str(delayTime[index]))
	fichier.close()			



def moyenne(tableau):
    return sum(tableau, 0.0) / len(tableau)



def variance(tableau):
    m=moyenne(tableau)
    return moyenne([(x-m)**2 for x in tableau])

#print(variance(probaFrames))

def ecartype(tableau):
    return variance(tableau)**0.5


energyStream=open("results/EnergyDelay/N"+str(nbNode)+"/"+EnergyConfInt,"w")
delayStream=open("results/EnergyDelay/N"+str(nbNode)+"/"+DelayConfInt,"w")
#delayConfInt=open("results/EnergyDelay/"+AlohaDelayConfInt,"a")
#energyConfInt=open("results/EnergyDelay/"+AlohaEnergyConfInt,"a")

print("\nON Time:")
energyStream.write("\nON Time:"+"\n")

#print("Sigma: "+str(ecartype(onTime)))
#energyStream.write("Sigma: "+str(ecartype(onTime))+"\n")

print("Mp: "+str(moyenne(onTime))+" (ms)")
energyStream.write("Mp: "+str(moyenne(onTime))+" (ms)\n")

print("Cinf: "+str(moyenne(onTime)-(1.96*(ecartype(onTime)/len(onTime)**0.5)))+" (ms)")
energyStream.write("Cinf: "+str(moyenne(onTime)-(1.96*(ecartype(onTime)/len(onTime)**0.5)))+" (ms)\n")

print("Csup: "+str(moyenne(onTime)+(1.96*(ecartype(onTime)/len(onTime)**0.5)))+" (ms)")
energyStream.write("Csup: "+str(moyenne(onTime)+(1.96*(ecartype(onTime)/len(onTime)**0.5)))+" (ms)\n")

#energyConfInt.write(str(nbNode)+"   "+str(moyenne(onTime)-(1.96*(ecartype(onTime)/len(onTime)**0.5)))+"   "+str(moyenne(onTime))+"   "+str(moyenne(onTime)+(1.96*(ecartype(onTime)/len(onTime)**0.5)))+"\n")
'''
print("\nDelay Time:")
delayStream.write("\nDelay Time:\n")

#print("Sigma: "+str(ecartype(delayTime)))
#delayStream.write("Sigma: "+str(ecartype(delayTime))+"\n")

print("Mp: "+str(moyenne(delayTime))+" (ms)")
delayStream.write("Mp: "+str(moyenne(delayTime))+" (ms)\n")

print("Cinf: "+str(moyenne(delayTime)-(1.96*(ecartype(delayTime)/len(delayTime)**0.5)))+" (ms)")
delayStream.write("Cinf: "+str(moyenne(delayTime)-(1.96*(ecartype(delayTime)/len(delayTime)**0.5)))+" (ms)\n")

print("Csup: "+str(moyenne(delayTime)+(1.96*(ecartype(delayTime)/len(delayTime)**0.5)))+" (ms)")
delayStream.write("Csup: "+str(moyenne(delayTime)+(1.96*(ecartype(delayTime)/len(delayTime)**0.5)))+" (ms)\n")

#delayConfInt.write(str(nbNode)+"   "+str(moyenne(delayTime)-(1.96*(ecartype(delayTime)/len(delayTime)**0.5)))+"   "+str(moyenne(delayTime))+"   "+str(moyenne(delayTime)+(1.96*(ecartype(delayTime)/len(delayTime)**0.5)))+"\n")'''

energyStream.close()
delayStream.close()
