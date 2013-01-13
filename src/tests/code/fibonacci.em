###############################################################################
# Emerald - code test
# Obtain the fibonacci number for an input value.
# Language's tested features: recursive functions, input, output, method.
###############################################################################

def fibonacci( n )
	if ( n >= 2 )
		n = fibonacci( n - 1 ) + fibonacci( n - 2 )
	end
	n = n
end

puts ( "Input the value to calculate fibonacci's number: " )
n = geti
i = fibonacci( n )
puts ( "Fibonacci's number for #{n} is #{i}" )

