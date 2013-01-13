###############################################################################
# Emerald - code test
# Obtain the fibonacci number for an input value.
# Language's tested features: recursive functions, input, output, method.
###############################################################################

def fibonacci( n )
	#puts("Arg1 vale #{n}\n")
	if ( n < 2 )
		n = n
		#puts("Simple N vale #{n}\n")
	else
		n = fibonacci( n - 1 ) + fibonacci( n - 2 )
		#puts("Complejo N vale #{n}\n")
	end
	n = n
end

puts ( "Input the value to calculate fibonacci's number: " )
n = geti
i = fibonacci( n )
puts ( "Fibonacci's number for #{n} is #{i}" )

