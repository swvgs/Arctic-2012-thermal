Arctic-2012-thermal
===================
This is Andrew Vaccaro's thermal processing script that was made to work with
raw data from Therma.

As of 5/2/13, these scripts are in a preliminary stage. They are only run from
the command line and require Python 2 to be installed. Later versions will
"hopefully" be stand-alone and have a GUI (assuming I ever get to them).

tim2.py takes original data files and splits them into multiple files, each one
containing the data along one survey line. tim2.py takes the data files and
processes them down to an equivalent number of data points between each survey
marker.

Currently, an unknown error prevents all of the code from being run in the same
file. Not entirely sure why, but splitting into two files works (thanks, CJ).
That being said, all file names and numbers are hard-coded into the scripts.
Annoying, but it's fairly simple to go in and edit the file name in tim2.py and
then the number of files produced in tim2.py.

tim3.py is in the works. It should be a single file that does the work of tim.py
and tim2.py. It is planned to account for error exceptions as well as gaps in or
issues with the data.
