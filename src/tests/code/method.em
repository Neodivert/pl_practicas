###############################################################################
# Emerald - code test
# Sum two values and print the result
# Language's tested features: output, methods.
###############################################################################

# Sum method definition.
def sum( a, b)
	while ( b > 0 ) do
		a = a + 1
		b = b - 1
	end
	#puts( "#{b}\n" );
	result = a
end 

# Sum method call.
a = 5
b = 7
result = sum( a, b )

c = 'a'

# Print the result.
puts( "[#{c}]: #{a} + #{b} = #{result}\n" )

