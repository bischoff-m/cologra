default: suitesparse

suitesparse:
	- uv run src/suitesparse/main.py

bench:
	- uv run src/benchmark/main.py

profile:
	- uv run gprof2dot --format=callgrind --output=../shared/profile.dot -w -s ../shared/callgrind.out
	- dot -Tsvg ../shared/profile.dot -o ../shared/profile.svg
