COMPILER=g++ -std=c++11
SOURCES=main.cpp
EXEC=program

all: clean build run

build:
	$(COMPILER) $(SOURCES) -o $(EXEC)

clean:
	rm -f $(EXEC)
run:
	./$(EXEC)

