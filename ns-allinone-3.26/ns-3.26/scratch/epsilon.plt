set terminal png enhanced font "Times,14"
set output "CSDat/eps.png"
set xlabel "{/Symbol D} [dB]" font "Times,16"
set ylabel "SIR gain" font "Times,16"

set key left

plot "CSDat/epsilon.dat" using 1:2 title "Power control" with linespoints linetype 1, \
	"CSDat/epsilon.dat" using 1:3 title "CS control - X= 1.5" with linespoints linetype 2, \
	"CSDat/epsilon.dat" using 1:5 title "CS control - X= 3" with linespoints linetype 3, \
	"CSDat/epsilon.dat" using 1:6 title "CS control - X= 5" with linespoints linetype 4

	 
print "file collisions.png generated"

set key default
set output
set notitle

pause -1