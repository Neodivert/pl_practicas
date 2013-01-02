$a = 15
$b = 30

entrada = 1
while( entrada != 0 ) do
	puts( "Introduzca un entero: " )
	entrada = geti
	puts( "La entrada ha sido #{entrada}\n" )
end

a = 1 + 5
puts( "hola (#{a})\n" )

puts( "\tque tal\n" )
puts( "\t\tque\ntal\n" )
puts( "\"adios\"\n" )
puts( "y estos es una barra! \\\n" )

def foo
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
