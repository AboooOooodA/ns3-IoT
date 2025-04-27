piii=0.00125

def alohaNoCap(pi,N):
    return (1-pi)**(N-1)


def alohaKNoCap(pi,N,K):
    return (1-(1-(1-(K*pi))**(N-1))**K)  

def alohaCap(pi,N,Pr):
    return ((1-pi)**(N-1)+(N-1)*pi*(1-pi)**(N-2)*Pr)

def alohaKCap(pi,N,K,Pr):
    return 1-(1-((1-K*pi)**(N-1)+(N-1)*K*pi*(1-K*pi)**(N-2)*Pr))**K


print("Without Capture\n")

print("K	|	N	|	pi*N	|	Ps	")

print("1	|	10	| "+str(float(piii*float(10)))+"	|	"+str(alohaNoCap(piii,10)))
print("1	|	40	| "+str(float(piii*int(40)))+"		|	"+str(alohaNoCap(piii,40)))
print("1	|	70	| "+str(float(piii*int(70)))+"	|	"+str(alohaNoCap(piii,70)))
print("1	|	100	| "+str(float(piii*int(100)))+"		|	"+str(alohaNoCap(piii,100)))
print("1	|	150	| "+str(float(piii*int(150)))+"	|	"+str(alohaNoCap(piii,150)))
print("1	|	250	| "+str(float(piii*int(250)))+"	|	"+str(alohaNoCap(piii,250))+"\n")


print("2	|	10	| "+str(float(piii*float(10)))+"	|	"+str(alohaKNoCap(piii,10,2)))
print("2	|	40	| "+str(float(piii*int(40)))+"		|	"+str(alohaKNoCap(piii,40,2)))
print("2	|	70	| "+str(float(piii*int(70)))+"	|	"+str(alohaKNoCap(piii,70,2)))
print("2	|	100	| "+str(float(piii*int(100)))+"		|	"+str(alohaKNoCap(piii,100,2)))
print("2	|	150	| "+str(float(piii*int(150)))+"	|	"+str(alohaKNoCap(piii,150,2)))
print("2	|	250	| "+str(float(piii*int(250)))+"	|	"+str(alohaKNoCap(piii,250,2))+"\n")

print("3	|	10	| "+str(float(piii*float(10)))+"	|	"+str(alohaKNoCap(piii,10,3)))
print("3	|	40	| "+str(float(piii*int(40)))+"		|	"+str(alohaKNoCap(piii,40,3)))
print("3	|	70	| "+str(float(piii*int(70)))+"	|	"+str(alohaKNoCap(piii,70,3)))
print("3	|	100	| "+str(float(piii*int(100)))+"		|	"+str(alohaKNoCap(piii,100,3)))
print("3	|	150	| "+str(float(piii*int(150)))+"	|	"+str(alohaKNoCap(piii,150,3)))
print("3	|	250	| "+str(float(piii*int(250)))+"	|	"+str(alohaKNoCap(piii,250,3))+"\n")

print("With Capture and Pr=0.2\n")

print("K	|	N	|	pi*N	|	Ps	")

print("1	|	10	| "+str(float(piii*float(10)))+"	|	"+str(alohaCap(piii,10,0.2)))
print("1	|	40	| "+str(float(piii*int(40)))+"		|	"+str(alohaCap(piii,40,0.2)))
print("1	|	70	| "+str(float(piii*int(70)))+"	|	"+str(alohaCap(piii,70,0.2)))
print("1	|	100	| "+str(float(piii*int(100)))+"		|	"+str(alohaCap(piii,100,0.2)))
print("1	|	150	| "+str(float(piii*int(150)))+"	|	"+str(alohaCap(piii,150,0.2)))
print("1	|	250	| "+str(float(piii*int(250)))+"	|	"+str(alohaCap(piii,250,0.2))+"\n")

print("2	|	10	| "+str(float(piii*float(10)))+"	|	"+str(alohaKCap(piii,10,2,0.2)))
print("2	|	40	| "+str(float(piii*int(40)))+"		|	"+str(alohaKCap(piii,40,2,0.2)))
print("2	|	70	| "+str(float(piii*int(70)))+"	|	"+str(alohaKCap(piii,70,2,0.2)))
print("2	|	100	| "+str(float(piii*int(100)))+"		|	"+str(alohaKCap(piii,100,2,0.2)))
print("2	|	150	| "+str(float(piii*int(150)))+"	|	"+str(alohaKCap(piii,150,2,0.2)))
print("2	|	250	| "+str(float(piii*int(250)))+"	|	"+str(alohaKCap(piii,250,2,0.2))+"\n")

print("3	|	10	| "+str(float(piii*float(10)))+"	|	"+str(alohaKCap(piii,10,3,0.2)))
print("3	|	40	| "+str(float(piii*int(40)))+"		|	"+str(alohaKCap(piii,40,3,0.2)))
print("3	|	70	| "+str(float(piii*int(70)))+"	|	"+str(alohaKCap(piii,70,3,0.2)))
print("3	|	100	| "+str(float(piii*int(100)))+"		|	"+str(alohaKCap(piii,100,3,0.2)))
print("3	|	150	| "+str(float(piii*int(150)))+"	|	"+str(alohaKCap(piii,150,3,0.2)))
print("3	|	250	| "+str(float(piii*int(250)))+"	|	"+str(alohaKCap(piii,250,3,0.2))+"\n")

