OBJ = main.o
INC = -I "./"

main: $(OBJ)
	g++ $(OBJ) -o main.exe
	rm -f $(OBJ)
	
main.o:
	g++ -c main.cpp $(INC)
	
clean:
	rm -f $(OBJ) main
