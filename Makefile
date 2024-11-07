
build:
	mkdir -p build
	cd build && cmake .. && make

install:
	sudo apt-get install cmake openmpi-bin openmpi-common libopenmpi-dev

run:
	mpirun -np 1 build/cologra

build-run:
	make build && echo && make run

clean:
	rm -rf build
