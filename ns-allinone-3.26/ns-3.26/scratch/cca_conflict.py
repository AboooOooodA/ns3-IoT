nbNode="250"
from csv import reader, writer
from matplotlib import pyplot as plt
y=[]
x=[]
y90=[]
x90=[]
y80=[]
x80=[]
monsters=[]
sol_file_Thre_99="myCsma/results/N/D100/N"+nbNode+"/CsmaSCRIPTProbaRetryN"+nbNode+"D100P64T0.1seed1Thre-99.out"
sol_file_Thre_90="myCsma/results/N/D100/N"+nbNode+"/CsmaSCRIPTProbaRetryN"+nbNode+"D100P64T0.1seed1Thre-90.out"
sol_file_Thre_80="myCsma/results/N/D100/N"+nbNode+"/CsmaSCRIPTProbaRetryN"+nbNode+"D100P64T0.1seed1.out"
node_table="../node_table.h"

'''with open(sol_file_Thre_99) as sol:
		for line in sol:
			if ("Node[0]" not in line):					
				y.append(float(line.split(" ")[9]))
				x.append(float(line.split(" ")[19].split("\n")[0]))

with open(sol_file_Thre_90) as sol:
		for line in sol:
			if ("Node[0]" not in line):					
				y90.append(float(line.split(" ")[9]))
				x90.append(float(line.split(" ")[19].split("\n")[0]))
'''
with open(sol_file_Thre_80) as sol:
		for line in sol:
			if ("Node[0]" not in line):					
				y80.append(float(line.split(" ")[21]))
				x80.append(float(line.split(" ")[25].split("\n")[0]))
				if (float(line.split(" ")[21].split("\n")[0])<0.9):
					monsters.append(int(line.split(" ")[0].split("[")[1].replace("]","")))

monsters_file=open(node_table,"w")
monsters_file.write("#define MAX "+str(len(monsters))+"\n"+"#define ELEMENTS "+str(str(monsters).strip('[')).strip(']'))
monsters_file.close()
'''
print(min(x))
print(max(x))
print(min(y))
print(max(y))'''
print(len(monsters))
print(monsters)
plt.figure(figsize=(10,10))
plt.xlabel('CCA Conflict')
plt.ylabel('Packet Success Probability')
#plt.xlim((0.75,1))
plt.ylim((0,1))


#plt.scatter(x, y, s=30, c="green", alpha=0.45, marker="o", label="CCA Thre= -99 dBm")
#plt.scatter(x90, y90, s=30, c="purple", alpha=0.45, marker="o", label="CCA Thre= -90 dBm")
plt.scatter(x80, y80, s=30, c="blue", alpha=0.45, marker="o", label="CCA Thre= -80 dBm")
#plt.legend(loc='lower left');
plt.show()
