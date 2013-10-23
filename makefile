
# project name (generate executable with this name)
TARGET   = project2

CC       = gcc
# compiling flags here
CFLAGS   = -std=c99 -Wall -I.

LINKER   = gcc -o
# linking flags here
LFLAGS   = -Wall -I. -lm

# change these to set the proper directories where each files shoould be
SRCPREFIX = ./src
OBJDIR    = obj
BINDIR    = bin

SRCDIRS=mainApp
SRCDIRS+= structDefs
SRCDIRS+= fetchStage
SRCDIRS+= decodeStage
SRCDIRS+= dispatchStage
SRCDIRS+= issueStage
SRCDIRS+= executeStage
SRCDIRS+= reservationStations
SRCDIRS+= functionalUnits
SRCDIRS+= utilities/branchPrediction
SRCDIRS+= utilities/cacheController
SRCDIRS+= utilities/commandLine


SOURCES  := $(wildcard $(SRCPREFIX)/$(SRCDIRS)/*.c)
INCLUDES := $(wildcard $(SRCPREFIX)/$(SRCDIRS)/*.h)
OBJECTS  := $(SOURCES:$(SRCPREFIX)/$(SRCDIRS)/%.c=$(OBJDIR)/%.o)
rm       = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS)
	@echo "$(SOURCES)"
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCPREFIX)/$(SRCDIRS)/%.c
	@echo $(SOURCES)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
