
set xlabel "Number of Nodes"
set ylabel "Frame Success Probability"
set font 'Helvetica, 32'

plot "framesPST1.data" using 1:3:2:4 with yerrorlines t "Aloha", "framesPST1.data" using 1:6:5:7 with yerrorlines t "CSMA"
