CXX = gcc
CXXFLAGS = -Wall -pthread -D_REENTRANT

SRC = 
OBJ = $(SRC:.c=.o)
EXEC = prog

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LBLIBS)

clean:
	rm -rf $(OBJ) $(EXEC)