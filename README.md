2013-ECE636 -- Readme
========================================

Authors: Jason Cain && Brian Schultek

========================================

Okay, so this hopefully should be pretty easy. Our code requires normal g++, so please run the following command if you don't already have it. (We cleared this in class, so hopefully it's still kosher.)

sudo apt-get install g++  (if this doesn't work for some reason, say, a 404 error due to old Ubuntu repositories, see the last entry here or Appendix A in our report for a workaround).


After ensuring g++ is installed (our base gcc version was 4.7.3), you can just call "make" from this directory (or, make -f makefile, if you wish). This should get everything built up for you. Once the makefile finishes up, there should be an executable in ./bin/ called project2. That's the simulator.

To actually run stuff, we found it was easiest to use our test.sh script  (provided) to make sure everything lines up alright, but basically the program will prompt you for all values (unless you ran the script... see below) and once they're all inputted, off it runs. It ends up spitting out the IPC and various other statistics and end-of-simulation snapshots based on what happened, and also dumps all standard output to run_output.log. 


==========================
Using the test.sh script
==========================
The test.sh script contains everything needed to run the program. Simply uncomment (remove the #) the trace file desired and re-comment (add a # to the beginning of the line) to pick your input trace file. Also of import are the actual parameters: SS_FACTOR, BTB_SIZE, etc. These should be self explanatory and the code should correct for any out-of-bounds entries. 

Once the options are set and the script has been saved and closed... just execute it:

./test.sh

and everything should play out nicely.

=============================
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

There's some batch-style scripts that we used to gather our data and parse it, called debug.sh, gather_data_maxes.sh, gather_data_optimals.sh and final_values_test.sh. The gather data scripts usually take around 15 minutes if you're feeling adventurous, while the finals script executes the three target traces with the optimal simulator specs that we derived. Lastly, the debug script sets everything up similarly to test.sh, leaving the user at the start of a gdb session. To start debugging from there, simply type "run" or "r" and hit return. Useful if something is acting up. All scripts should be dumping their output to some log file (*.log), in case there is a need to check or diff anything, but be careful as the same target log can be overwritten by subsequent runs.

===================================
Installing g++ on Ubuntu
===================================

Sometimes, depending on the version of Ubunutu, getting g++ can be... finicky. If that's the case, here's a guide to getting around any difficulties and getting g++ installed.

1) Open a terminal window

2) Type:     sudo apt-get update
If using a newer version of Ubuntu that is still supported this will find and download the packages needed for installing g++.  Continue to step 3.
If using an older version of Ubuntu like 10.10 this will return several “Error 404 Not Found” and you will need to update your source list to point to a different repository.  Continue to step 2b.

2b) If using the previous command results in “Error 404 Not Found” or the install command in step 3 or 4 results in “Unable to locate package ….” then you will need to update your source list.
In the terminal navigate to: cd /etc/apt
If you would like to make a backup of the file you will be editing type: sudo cp sources.list sources.backup
Type: sudo gedit sources.list
The uncommented lines will have addresses similar to - http://us.archive.ubuntu.com/ubuntu/ 
These will need replaced with -     http://old-releases.ubuntu.com/ubuntu/
There should be 8 instances that need this change.  Save the file and close.
Now type:      sudo apt-get update

3) Optional - if you want to install ALL of the updates to current software on your system type: sudo apt-get upgrade
This may take a while and should NOT be necessary to install g++

4) Type: sudo apt-get install build-essential
This should install g++ and the c/c++ libraries needed to compile c++ code




+++++++++++++++++++++++++++++++++++++
        Questions? Comments? 

           Contact info:
+++++++++++++++++++++++++++++++++++++
Jason Cain       -    cainj3@udayton.edu
Brian Schultek   -    bschultek1@udayton.edu
