set terminal png enhanced font "Times,14"
set output "CSDat/csavg.png"

set xlabel "Carrier Sense Threshold" font "Times,16"
set ylabel "Percentage of Vehicles" font "Times,16"

set key left
set key box

set style data histogram
set style histogram cluster gap 1
set style fill pattern border -1
set boxwidth 0.9
set yrange [0:0.7]

#set xtics ("> (-65.00)" 0, "(-70.00) - (-65.01)" 1, "(-75.00) - (-70.00)" 2, "< (-75.00)" 3)
set label 1 "I: CS_t > -70 dBm" at 1, 0.65
set label 2 "II: -70 dBm > CS_t > -75 dBm" at 1, 0.61
set label 3 "III: -75 dBm > CS_t > -80 dBm" at 1, 0.57
set label 4 "IV: CS_t < -85 dBm" at 1, 0.53

plot "CSDat/csavg.dat" using ($4/150):xtic(1) title "25 veh/km", \
	"CSDat/csavg.dat" using ($2/200) title "35 veh/km", \
	"CSDat/csavg.dat" using ($3/250) title "45 veh/km"


print "file collisions.png generated"

set key default

set output
set notitle

pause -1