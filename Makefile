CXX  = g++
OBJ  = main.o one.o prs.o one60.o
BIN  = ONE_Unpk
RM   = rm -f

all: $(BIN)

clean:
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(OBJ) -o $(BIN) $(LIBS)

