CC = gcc
CCFLAGS = -Wall -Wextra

SRCDIR = ./src/
OBJDIR = ./obj/
BINDIR = ./bin/

TEST_GRAPH = testGraphEditing
TEST_MAP = mapEditingTest
TESTS = $(TEST_MAP) $(TEST_GRAPH)
DRIVER = Driver_Eurodrift

SRC = $(wildcard $(SRCDIR)*.c)
OBJ = $(SRC:$(SRCDIR)%.c=$(OBJDIR)%.o)

all : $(OBJ) $(TESTS) $(DRIVER)

## Objects files

$(OBJDIR)%.o : $(SRCDIR)%.c
	$(CC) -o $@ -c $(CCFLAGS) $< -g

## Executables

$(TEST_MAP) : $(OBJDIR)mapEditing.o $(OBJDIR)mapEditingTest.o $(OBJDIR)mathObjects.o
	gcc $^ -o $(BINDIR)$@

$(TEST_GRAPH) : $(OBJDIR)mapEditing.o $(OBJDIR)testGraphEditing.o $(OBJDIR)graphEditing.o $(OBJDIR)tileQueue.o $(OBJDIR)mathObjects.o
	gcc $^ -o $(BINDIR)$@ -lm

$(DRIVER) : $(OBJDIR)graphEditing.o $(OBJDIR)tileQueue.o $(OBJDIR)mapEditing.o $(OBJDIR)main.o $(OBJDIR)mathObjects.o
	gcc $^ -o $(BINDIR)$@ -lm

## Cleaning 

clean : 
	-rm $(OBJDIR)*.o

distclean : clean
	-rm $(BINDIR)$(TEST_GRAPH)
	-rm $(BINDIR)$(TEST_MAP)
	-rm $(BINDIR)$(DRIVER)
