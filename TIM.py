"""
TIM.py
Author: Andrew Vaccaro
Created: 8/25/12

CSV file key:
Column 0 = markers
Column 1 = time in ms
Column 2 = IR temp
Column 3 = ambient temp
Data starts on row 2
"""


import csv


#initializes matrices, lists, and variables
thermal_data_matrix = []
flag = []
output_file_number = 1 
search_row = 2
start_line = 2
errors = []
new_files = 0


#determines file length
file_length = -1
file = open('030212-LOG00228-original-Line2-only.txt')
for line in file:
    file_length += 1
print "\nFile length is %d lines." %file_length


#converts csv file to matrix
thermal_data_rows = csv.reader(open("030212-LOG00228-original-Line2-only.txt")) #, 'U'), dialect='excel')
for row in thermal_data_rows:
    thermal_data_matrix.append(row)
print len(thermal_data_matrix)
print "Test print of row 2, column 3: %s" %thermal_data_matrix[2][3] #test print row 2, column 3
print "Test print of row 3: %s" %thermal_data_matrix[3] #test print row 3
print "Import finished."

#initial check for errors; returns any rows that do not have four elements
"""print "\nChecking for rows without four elements..."
for row in range(2, file_length):
    if len(thermal_data_matrix[row]) != 4:
        errors.append(row)
total_errors = []
if len(errors) == 0:
    print "No errors."
else:
    for element in errors:
        print "Removing errors at:"
        print "Line %d" %element, thermal_data_matrix[element]"""


#changes numerical values from strings to floats
errors = []
print "\nChecking for numerical values that will not resolve to floats..."
for row in range(2, file_length):
    for column in range(1, 4):

        print row
        try:
            float(thermal_data_matrix[row][column])
        except ValueError:
            #print "Error on row %d." %row
            errors.append(row)
        
if len(errors) == 0:
    print "No errors."
else:
    print "Could not resolve to floats on following rows: "
    print errors


#breaks up single data file into multiple files by survey lines
start_times = [thermal_data_matrix[2][1]]
print "File length: %d" %file_length
while search_row < (file_length - 1):

    #print search_row
    if float(thermal_data_matrix[search_row][1]) > float(thermal_data_matrix[search_row + 1][1]):
        print "\nError: Decreasing time at row %d" %search_row
        print "Time decreases from %s to %s." % (thermal_data_matrix[search_row][1], thermal_data_matrix[search_row + 1][1])
        search_row += 1
        error_row = thermal_data_matrix.pop(search_row)
        print error_row
        

    if float(thermal_data_matrix[search_row + 1][1]) - float(thermal_data_matrix[search_row][1]) > 5000:
        start_times.append(thermal_data_matrix[search_row][1])
        end_line = search_row
        write_file = open("thermal_survey_line_%d" %output_file_number, 'w')
        writer = csv.writer(write_file, delimiter = ',')
        for write_row in range(start_line, end_line + 1):
            writer.writerow(thermal_data_matrix[write_row])
        write_file.close()
        print output_file_number
        print thermal_data_matrix[search_row][1]

        output_file_number += 1
        start_line = end_line + 1
        new_files += 1

    search_row += 1

#writes last survey line
write_file = open("thermal_survey_line_%d" %output_file_number, 'w')
writer = csv.writer(write_file, delimiter = ',')
for write_row in range(start_line, file_length):
    writer.writerow(thermal_data_matrix[write_row])
new_files += 1

print "\nTimes of beginning row of each survey line:"
print start_times
print "New files created:", new_files
