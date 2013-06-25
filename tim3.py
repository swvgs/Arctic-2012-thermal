"""
SV file key:
Column 0 = markers
Column 1 = time in ms
Column 2 = IR temp
Column 3 = ambient temp
Data starts on row 2
"""

import csv


#initializes matrices, lists, and variables
thermal_data_matrix = []
output_file_number = 1 
search_row = 2
start_line = 2
errors = [[]]
new_files = 0
file_length = 0

#opens data file and determines length
file_name = '030212-LOG00228-original-Line2-only.txt' #raw_input("Input data file name: ")
read_file = open(file_name)
for line in read_file:
    file_length += 1
    if file_length < 5:
        print line
        print file_length
print "\nFile length is %d lines." %file_length

#imports csv file to matrix
thermal_data_rows = csv.reader(open(file_name))
for row in thermal_data_rows:
    thermal_data_matrix.append(row)
print "%d data rows imported from %s" % (len(thermal_data_matrix), file_name)

#check for rows without exactly 4 elements
for row in thermal_data_matrix:
    if len(row) != 4:
        print "Error found at row %s. Removing and logging error." %row
        errors.append(thermal_data_matrix.pop(row)) #TODO: adding list to a list?
