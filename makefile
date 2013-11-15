
SHELL = /bin/sh

# project name (generate executable with this name)
BinName = project2

#
# Directory name for obj-files
#
ObjDir = ./_obj

#
# Directory name for exe-files
#
BinDir = ./bin

CC = g++
# compiling flags here
CFLAGS = -c -g3 -ggdb3 -Wall

LINK = $(CC)
# linking flags here
LINKFLAGS = -Wall -lrt

#
# Directory names for sources files
#
SrcDirs = ./src/mainApp
SrcDirs += ./src/structDefs
SrcDirs += ./src/fetchStage
SrcDirs += ./src/decodeStage
SrcDirs += ./src/dispatchStage
SrcDirs += ./src/issueStage
SrcDirs += ./src/executeStage
SrcDirs += ./src/completeStage
SrcDirs += ./src/reservationStations
SrcDirs += ./src/functionalUnits
SrcDirs += ./src/utilities/branchPrediction
SrcDirs += ./src/utilities/cacheController
SrcDirs += ./src/utilities/commandLine
SrcDirs += ./src/utilities/globalVariables
SrcDirs += ./src/utilities/traceReader
SrcDirs += ./src/utilities/instruction



#
##############################################################################
#
# Makefile targets
#
##############################################################################
CINCLUDE += $(addprefix -I, $(SrcDirs)) -I/usr/include

# to search in implicit rules 
VPATH := $(SrcDirs)

search_wildcards := $(addsuffix /*.cpp, $(SrcDirs))

CppSrc  = $(wildcard $(search_wildcards))
Objs    = $(addprefix $(ObjDir)/, $(notdir $(patsubst %.cpp, %.o, $(CppSrc))))

##############################################################################

all : prepare build

build : prepare $(BinDir)/$(BinName)

	@echo "CFLAGS = $(CFLAGS)"
	@echo "============================================================"
	@echo "============================================================"
	@echo "==   ${BinName} Build complete."
	@echo "============================================================"
	@echo "============================================================"

	
$(BinDir)/$(BinName) : $(Objs)
	$(LINK) $^ $(LINKFLAGS) -o $@

$(ObjDir)/%.o: %.cpp 
	$(CC) $(CFLAGS) $(CINCLUDE) -o $@ $< 

-include $(Objs:%.o=%.d) 

.PHONY: prepare
prepare :

	mkdir -p $(ObjDir)
	mkdir -p $(BinDir)

.PHONEY: clean
clean:
	rm -r -f $(ObjDir)
	rm -r -f $(BinDir)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	rm -r -f $(BinDir)/$(BinName)
	@echo "Executable removed!"
