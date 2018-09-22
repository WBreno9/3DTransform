CC = g++ -std=c++14

all:
	$(CC) main.cpp -msse4.1 -O0 -g3 -lassimp -lglut -lGLU -lGL -I. -o cgprog

readme: index.html

index.html: README.md
	pandoc -f markdown -s -t html $^ -o $@

clean:
	rm cgprog

