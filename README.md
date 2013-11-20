2013-ECE636 -- Readme
========================================

Authors: Jason Cain && Brian Schultek

========================================
Okay, so this is pretty easy. Our code requires normal g++, so please run 

sudo apt-get install g++

if you don't already have it. We cleared this in class, so hopefully it's still kosher.

Once the makefile finishes up, there should be an executable in ./bin/ called project2. 

It's easiest to use our test.sh script  (provided) to make sure everything lines up alright, but basically
the program will prompt you for all values (unless you ran the script... see below) and once they're all inputted, 
off it runs. It ends up spitting out the IPC and various other statistics based on what happened. 


Using the test.sh script
========================
The test.sh script contains everything needed to run the program. Simply uncomment (remove the #) the trace file 
desired and re-comment (add a # to the beginning of the line) to pick your input trace file. Also of import are 
the actual parameters: SS_FACTOR, BTB_SIZE, etc. These should be self explanatory and the code should correct for 
any out-of-bounds entries. 

Once the options are set and the script has been saved and closed... just execute it:

./test.sh

and everything should play out nicely.


Included in turn-in tarball:
=============================
README.md   this file
makefile    our makefile
src/        contains all code
trace/      contains all traces
opcodes/    contains opcodes in traces
bin/        executable directory (included for reference.. makefile blows it away)

Eclipse Project Info:
.cproject
.project
.settings/

Also:
So you'll notice there's some extra schwag in the directory... pretty much purely provided for reference.

There's some batch-style scripts that we used to gather our data and parse it. The sweeps usually take around 
15 minutes if you're feeling adventurous. 



+++++++++++++++++++++++++++++++++++++
        Questions? Comments? 

           Contact info:
+++++++++++++++++++++++++++++++++++++
Jason Cain       -    cainj3@udayton.edu
Brian Schultek   -    bschultek1@udayton.edu
