OBJ = main.o
INC = -I "./"

Raytracing: $(OBJ)
	g++ $(OBJ) -o Raytracing.exe
	rm -f $(OBJ)

main.o:
	g++ -c main.cpp $(INC)

clean:
	rm -f $(OBJ) Raytracing
