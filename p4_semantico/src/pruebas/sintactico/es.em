
a = 1 + 5
puts "hola\n"
puts "\tque tal\n"
puts "\t\tque\ntal\n"
puts "\"adios\"\n"
puts "y estos es una barra! \\\n"

def foo
	i = 0
	while( i < 5 ) do
		j = 0
		while( j < 5 ) do
			puts "iterando\n"
			j = j + 1
		end
		i = i + 1
	end
end

foo()
