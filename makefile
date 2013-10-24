
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

CC       = gcc
# compiling flags here
CFLAGS   = -std=c99 -g3 -ggdb3 -Wall

LINK   = $(CC) -o
# linking flags here
LFLAGS   = -Wall -I. -lm


SrcDirs = ./src/mainApp
SrcDirs += ./src/structDefs
SrcDirs += ./src/fetchStage
SrcDirs += ./src/decodeStage
SrcDirs += ./src/dispatchStage
SrcDirs += ./src/issueStage
SrcDirs += ./src/executeStage
SrcDirs += ./src/reservationStations
SrcDirs += ./src/functionalUnits
SrcDirs += ./src/utilities/branchPrediction
SrcDirs += ./src/utilities/cacheController
SrcDirs += ./src/utilities/commandLine

CINCLUDE += $(addprefix -I, $(SrcDirs))

#
##############################################################################
#
# Makefile targets
#
##############################################################################
# to search in implicit rules 
VPATH := $(SrcDirs)

search_wildcards := $(addsuffix /*.c, $(SrcDirs))

CSrc  = $(wildcard $(search_wildcards))
Objs    = $(addprefix $(ObjDir)/, $(notdir $(patsubst %.c, %.o, $(CSrc))))
rm       = rm -f

all : prepare build

build : prepare $(BinDir)/$(BinName)

	@echo "CFLAGS = $(CFLAGS)"
	@echo "============================================================"
	@echo "============================================================"
	@echo "==   ${BinName} Build complete."
	@echo "============================================================"
	@echo "============================================================"

	
$(BinDir)/$(BinName) : $(Objs)
	$(LINKER) $^ $(LINKFLAGS) $(LIBS) -o $@

$(ObjDir)/%.o: %.c 
	$(CC) $(CFLAGS) $(CINCLUDE) -o $@ $< 

-include $(Objs:%.o=%.d) 

.PHONY: prepare
prepare :

	mkdir -p $(ObjDir)
	mkdir -p $(BinDir)

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
