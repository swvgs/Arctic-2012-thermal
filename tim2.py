"""
TIM_2.py
For split files BEFORE processing
Row 0: Markers
Row 1: Time 
Row 2: IR
Row 3: Ambient
"""

import csv
input_file_number = 1
output_file_number = 1
shortest_length = 1000000
marker_ratio = 0.0
shortest_10m = 0


#find shortest survey line
for file_number in range(1, input_file_number + 1):
    read_file = "thermal_survey_line_%d" % file_number
    file_length = -1
    for line in open(read_file):
        file_length += 1
    #print "%s is %d lines long." % (read_file, file_length)
    if file_length < shortest_length:
        shortest_length = float(file_length)
print "\n"
print "Shortest file: %d" % shortest_length
        
for file_number in range(1, input_file_number + 1):
    velocities = []
    thermal_data_matrix_strings = []
    marker_slot = 0
    middle_markers = []
    length_10m = []
    velocities = []
    dividers = []


    #determines new file length
    read_file = "thermal_survey_line_%d" % file_number
    file_length = 0
    for line in open(read_file):
        file_length += 1
    print "%s is %d lines long." % (read_file, file_length)

    #converts csv file into matrix
    thermal_data_rows = csv.reader(open(read_file))
    for row in thermal_data_rows:
        thermal_data_matrix_strings.append(row)
    print "Test print of row 2, column 1: ", thermal_data_matrix_strings[2][1]

    #convert strings in matrix to floats
    thermal_data_matrix = thermal_data_matrix_strings
    for row in range(len(thermal_data_matrix_strings)):
        #print row
        thermal_data_matrix[row][0] = thermal_data_matrix_strings[row][0]
        thermal_data_matrix[row][1] = float(thermal_data_matrix_strings[row][1])
        thermal_data_matrix[row][2] = float(thermal_data_matrix_strings[row][2])
        thermal_data_matrix[row][3] = float(thermal_data_matrix_strings[row][3])
    
    

    #searches for markers
    while marker_slot < (file_length - 1):
    
        if thermal_data_matrix[marker_slot][0] == "*":
            start_marker = marker_slot
            marker_slot += 1

            
            for counter in range(70):
                if marker_slot == file_length:
                    break
                if thermal_data_matrix[marker_slot][0] == "*":
                    end_marker = marker_slot #set or overwrite end marker
                    marker_slot += 1
            middle_markers.append(int((end_marker + start_marker) / 2))
            print "Start marker: ", start_marker, "End marker: ", end_marker
            

        marker_slot += 1

    print "Middle markers for %s: " %read_file, middle_markers
    print "Total markers: ", len(middle_markers)

    #find velocities
    for marker in range(len(middle_markers) - 1):
        velocities.append(10000 / (thermal_data_matrix[middle_markers[marker + 1]][1] - 
            thermal_data_matrix[middle_markers[marker]][1]))
        

    #find length of 10m segments
    for segment in range(len(middle_markers) - 1):
        length_10m.append(middle_markers[segment + 1] - middle_markers[segment])

    print "10m length ", length_10m
    print "Middle markers", middle_markers

    #find shortest segment
    shortest_segment = 10000
    for segment in range(len(length_10m)):
        if shortest_segment > length_10m[segment]:
            if length_10m[segment] > 100:
                shortest_segment = length_10m[segment]

    #set up variables to average points
    write_file = open('thermal_output_%d' %output_file_number, 'w')
    output_file_number += 1
    expanding_factor = 100
    multiplier_1 = 0
    multiplier_2 = 0
    new_IR_points = []
    new_ambient_points = []
    expanded_matrix = []
    counter = 0

    #created expanded matrix
    for row in range(len(thermal_data_matrix)):
        for row2 in range(expanding_factor):
            expanded_matrix.append(thermal_data_matrix[row])
    #print "Expanded matrix is %d lines long." % len(expanded_matrix)
    
    #runs through segments
    for segment in range(len(length_10m)):
        print "Segment: %d" %segment
        divider = int(float(length_10m[segment]) / shortest_segment * expanding_factor)
	print "Divider: ", divider
        dividers.append(divider)
        '''multiplier_2 = 0
        multiplier_1 = expanding_factor
        multiplier_2 = divider - multiplier_1'''

        #add together groups of points for IR
        row = expanding_factor * middle_markers[segment]
        while row < expanding_factor * middle_markers[segment + 1]:
            new_point = 0
            for point in range(divider):
                new_point += expanded_matrix[row][2]
                row += 1
	    new_point /= divider
            new_IR_points.append(new_point)

        #add together groups of points for ambient
        row = expanding_factor * middle_markers[segment]
        while row < expanding_factor * middle_markers[segment + 1]:
        
            new_point = 0
            for point in range(divider):
                new_point += expanded_matrix[row][3]
                row += 1
            new_point /= divider
            new_ambient_points.append(new_point)

            
    #print new_points
    print len(new_IR_points), shortest_segment, row#, expanding_factor * middle_markers[segment + 1]

    #write new points to output file

    write_file.write("# markers: %d\n" %len(middle_markers))
    write_file.write("Expanding factor: %d\n" %expanding_factor)
    write_file.write("Segment #: velocity, divider, line start, points\n")
    for counter in range(len(length_10m)):
        write_file.write("Segment %d: %s, %s, %s, %s\n" %(counter + 1, velocities[counter], dividers[counter], middle_markers[counter], middle_markers[counter + 1] - middle_markers[counter]))
    write_file.write("\n")
    for row in range(len(new_IR_points)):
        write_file.write(str(new_IR_points[row]) + ',' + str(new_ambient_points[row]) + '\n')
