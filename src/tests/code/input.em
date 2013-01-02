###############################################################################
# Emerald - code test
# Ask the user for different types of input and show them back to the user.
# Language's tested features: I/O, loops.
###############################################################################

# Ask the user for integers until a 0 is wrote.
integer = 1
while( integer != 0 ) do
	puts( "Input an integer (0 to exit): " )
	integer = geti
	puts( "Input integer: #{integer}\n" )
end

# FIXME: Finish character and float I/O.

# Ask the user for integers until a 'x' is wrote.
character = 'a'
#while( character != 'x' ) do
	#puts( "Input an char ('x' to exit): " )
	#character = getc
	#puts( "Input character: #{character}\n" )
#end

# Ask the user for floats until a 0 is wrote.
#float = 1.0
#while( float != 0.0 ) do
# 	puts( "Input an float: " )
#	float = getf
#	puts( "Input float: #{float}\n" )
#end
