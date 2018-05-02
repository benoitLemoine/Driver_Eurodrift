CC = gcc
CCFLAGS = -Wall -Wextra

SRCDIR = ./src/
OBJDIR = ./obj/
BINDIR = ./bin/

TEST_GRAPH = testGraphEditing
TEST_MAP = mapEditingTest
TESTS = $(TEST_MAP) $(TEST_GRAPH)

SRC = $(wildcard $(SRCDIR)*.c)
OBJ = $(SRC:$(SRCDIR)%.c=$(OBJDIR)%.o)

$(OBJDIR)%.o : $(SRCDIR)%.c
	$(CC) -o $@ -c $(CCFLAGS) $< -g

all : $(OBJ) $(TESTS)

$(TEST_MAP) : $(OBJDIR)mapEditing.o $(OBJDIR)mapEditingTest.o
	gcc $^ -o $(BINDIR)$@

$(TEST_GRAPH) : $(OBJDIR)mapEditing.o $(OBJDIR)testGraphEditing.o $(OBJDIR)graphEditing.o $(OBJDIR)tileQueue.o
	gcc $^ -o $(BINDIR)$@ -lm
