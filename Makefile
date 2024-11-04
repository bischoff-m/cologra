
build:
	mkdir -p build
	cd build && cmake .. && make

install:
	sudo apt-get install cmake openmpi-bin openmpi-common libopenmpi-dev

run:
	mpirun -np 4 build/cologra

clean:
	rm -rf build
