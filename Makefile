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

$(OBJDIR)%.o : $(SRCDIR)%.c
	$(CC) -o $@ -c $(CCFLAGS) $< -g

all : $(OBJ) $(TESTS) $(DRIVER)

$(TEST_MAP) : $(OBJDIR)mapEditing.o $(OBJDIR)mapEditingTest.o
	gcc $^ -o $(BINDIR)$@

$(TEST_GRAPH) : $(OBJDIR)mapEditing.o $(OBJDIR)testGraphEditing.o $(OBJDIR)graphEditing.o $(OBJDIR)tileQueue.o
	gcc $^ -o $(BINDIR)$@ -lm

$(DRIVER) : $(OBJDIR)graphEditing.o $(OBJDIR)tileQueue.o $(OBJDIR)mapEditing.o $(OBJDIR)main.o
	gcc $^ -o $(BINDIR)$@ -lm
