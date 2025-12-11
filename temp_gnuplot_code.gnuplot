set terminal pngcairo size 600,400 enhanced font 'Times,10'
set output 'tex_graph.png'
set grid
set xlabel 'x'
set ylabel 'f(x)'
plot 'temp_gnuplot_data.dat'  using 1:2 with lines lc rgb '#ff0000' title 'Taylor', \
     'temp_gnuplot_data.dat'  using 1:3 with lines lc rgb '#001aff' title 'Function', \
     'temp_gnuplot_data.dat'  using 1:4 with lines lc rgb '#000000' title 'Tangent', \
     'temp_gnuplot_point.dat' using 1:2 with points pt 7 ps 1.5 lc rgb '#eeff00' title 'Touch point',
unset output
