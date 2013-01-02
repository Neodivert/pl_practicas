###############################################################################
# Emerald - code test
# Call to a recursive function that prints messages.
# Language's tested features: I/O, recursion, method calling.
###############################################################################

# Recursive function.
def recursive ( i )
	puts( "Im the call #{i}\n" )
	if(i == 0)
		puts("Finish")
	else
		recursive(i - 1)	
	end	
end

recursive(10)	
