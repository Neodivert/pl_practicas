###############################################################################
# Emerald - code test
# Play with de output.
# Language's tested features: I/O, loops.
###############################################################################

# Print an expression's result.
a = 1 + 5
puts( "hola (#{a})\n" )

# Print some messages with escape sequences.
puts( "\tque tal\n" )
puts( "\t\tque\ntal\n" )
puts( "\"adios\"\n" )
puts( "y esto es una barra! \\\n" )

# Method with a triple nested loop that prints the iteration variables.
def foo()
	i = 0
	while( i < 3 ) do
		j = 0
		while( j < 3 ) do
			k = 0
			while( k < 3 ) do
				puts ( "iterando #{i}, #{j}, #{k}\n" )
				k = k + 1
			end
			j = j + 1
		end
		i = i + 1
	end
end

foo()
