#set terminal png enhanced font "Times,14"
set term wxt enhanced dashed "Times,14"
#set output "packetsR.png"

set xlabel "CCA Conflict" font "Times,16"
set ylabel "Packet Success Probability" font "Times,16"


set key left bottom
set key box
#set key horizontal
set grid 

#set xtics(10,40,70,150,250,500)
set yrange [0:1]

plot "myCsma/results/N/D100/N100/CsmaSCRIPTProbaRetryN100D100P64T0.1seed1.out" using 10:20 with dots

print "file packetsR.png generated"

set key default

#set output
set notitle

pause -1
