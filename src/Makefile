#   TCP Chat Project
#   Author: Jimmy Nguyen
#   Email: Jimmy@JimmyWorks.net
#
#   Description:
#   Makefile for TCP Chat Project
#   Commands:
#   make		Make all executables.
#   make clean		Clean all intermediate files
#   make test		Test the program
#   make backup 	Make a backup of the current project

# Project name for make backup
PROJECTNAME = TCP_Chat

# Filenames
 # Source
SERVER_SRC = 	server.cc \
		server_driver.cc \
                json_parser.cc \
                user.cc
CLIENT_SRC = 	client.cc \
		client_driver.cc \
                json_parser.cc

 # Executables
SERVER_EXE = chat_service
CLIENT_EXE = chat_client

 # Input and Output
#INPUTFILE = input.txt
# Directories 
#INPUTDIR = ../input/
#OUTPUTDIR = ../output/
BIN_DIR = ../bin/

# Compilers and Flags
CXX = g++
CXXFLAGS = -pthread -Wall -I../include/ -std=c++11 

# Make Targets
CLIENT_OBJS=$(CLIENT_SRC:cc=o)
SERVER_OBJS=$(SERVER_SRC:cc=o)

 # make
all: $(SERVER_EXE) $(CLIENT_EXE)

$(SERVER_EXE): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)$@ 
$(CLIENT_EXE): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)$@

 # make clean
clean: 
	rm -f *.o *.d*

 # make test
test: clean_files
	# TESTING ...

Makefile: $(SRCS:.c=.d)

 # Pattern for .d files.
%.d:%.cc
	@echo Updating .d Dependency File
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

 # make backup (Backup Target)
backup: clean
	@mkdir -p ~/backups; chmod 700 ~/backups
	@$(eval CURDIRNAME := $(shell basename `pwd`))
	@$(eval MKBKUPNAME := ~/backups/$(PROJECTNAME)-$(shell date +'%Y.%m.%d-%H:%M:%S').tar.gz)
	@echo
	@echo Writing Backup file to: $(MKBKUPNAME)
	@echo
	@-tar zcfv $(MKBKUPNAME) ../$(CURDIRNAME)
	@chmod 600 $(MKBKUPNAME)
	@echo
	@echo Done!


 # Include the dependency files
-include $(SRCS:.cc=.d)

