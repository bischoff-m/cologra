
build:
	mkdir -p out
	cd out && cmake .. && make

install:
	sudo apt-get install cmake openmpi-bin openmpi-common libopenmpi-dev

run:
	mpirun -np 1 out/cologra

build-run:
	make build && echo && make run

clean:
	rm -rf out
