SOURCES = src/test.cpp
INCLUDES = ../../include

nurbs_test: $(SOURCES)
	$(CXX) $(SOURCES) -I$(INCLUDES) -o $@

basis: nurbs_test
	./nurbs_test 2 > data.txt && gnuplot -p -e "plot \"data.txt\" using 1:2 with points"

curve: nurbs_test
	./nurbs_test 1 > data.txt && gnuplot -p -e "plot \"data.txt\" using 1:2 with points"

surface: nurbs_test
	./nurbs_test 0 > data.txt && gnuplot -p -e "set mouse; set dgrid3d 20, 20; set hidden3d; splot \"data.txt\" using 1:2:3 with lines"

clean:
	rm -f nurbs_test

