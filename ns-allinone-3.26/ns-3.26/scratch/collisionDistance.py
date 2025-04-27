import sys

nbNode=int(sys.argv[1])
model=sys.argv[5]
distance="D100"
pcktSize="P64"
nbRetransmission="M"+sys.argv[4]
traffic="T"+sys.argv[3]
seed="seed"+sys.argv[2]
#ccaThre=float(sys.argv[6])

resultFile=model+"ProbaRetry"+"N"+str(nbNode)+distance+nbRetransmission+pcktSize+traffic+seed+".out"

distanceToGW=[]
distanceCollisonNd=[]
lastX=0.0
collisionX=0.0
lastY=0.0
collisionY=0.0
lastDistanceGW=0.0
collisionDistance=0.0

statsStream=open("results/N/"+str(distance)+"/N"+str(nbNode)+"/STATS"+resultFile,"r")


for line in statsStream:
	if ("STATS receiving in IDLE state" in line):
		lastX=float(line.split(" ")[10])
		lastY=float(line.split(" ")[12])
		lastDistanceGW=float(line.split(" ")[14])
	if(("STATS receiving in IDLE state") in line):
		collisionX=float(line.split(" ")[13])
		collisionY=float(line.split(" ")[15])
		collisionDistance=sqrt(Sqr(collisionX-lastX)+Sqr(collisionY-lastY))
		distanceCollisionNd.append(collisionDistance)
		distancetoGW.append(line.split(" ")[17])
		
print(distanceToGW)
print(distanceCollisonNd)