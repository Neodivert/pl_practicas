###############################################################################
# Emerald - code test
# Conditionally print one message or another depending on user input.
# Language's tested features: I/O, if.
###############################################################################

# Ask the user to input its grade.
puts( "Input your grade (an integer between 0 and 10): " )
grade = geti

# Print a message or another depending on user input.
puts( "Result: " )
if( grade >= 5)
	puts( "PASS\n" )
else
	puts( "FAIL\n" )
end

