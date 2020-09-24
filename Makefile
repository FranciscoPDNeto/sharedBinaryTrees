CXX = gcc
CXXFLAGS = -Wall -Werror -Wextra -pedantic
LDFLAGS = -pthread

SRC = binaryTree.c main.c
OBJ = $(SRC:.c=.o)
EXEC = binaryTree

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LBLIBS)

clean:
	rm -rf $(OBJ) $(EXEC)