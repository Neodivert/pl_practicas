###############################################################################
# Emerald - code test
# Conditionally print one message or another depending on user input.
# Language's tested features: I/O, nested ifs.
###############################################################################

# Ask the user to input its grade.
puts( "Input your grade (0-10): " )
grade = geti

# Print a message or another depending on user input.
puts("Result: ")
if( grade >= 5) 
	puts( "PASS with grade: ")
	
	if (grade < 7) then 
		puts("   C\n")
	else
		if (grade < 9) then 
			puts( "   B\n")
		else
			if (grade < 10) then
				puts( "   A\n")
			else
				puts( "   A+\n")
			end
		end
	end
else
	puts("FAIL\n")
end

