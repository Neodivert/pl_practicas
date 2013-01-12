###############################################################################
# Emerald - code test
# Obtain the fibonacci number for an input value.
# Language's tested features: recursive functions, input, output, method.
###############################################################################

# FIXME Da error en el argumento para el caso base
#Input the value to calculate fibonacci's number: 0
#Q.h: Acceso a memoria fuera de l�mites (error 2)
#16�		R0 = I(R0 + 8); //Loading value of var arg1
#
def fibonacci( arg1 )
	if ( ( arg1 < 1 ) and ( arg1 >= 0 ) )
		n = arg1
	else
		n = fibonacci( n - 1 ) + fibonacci( n - 2 )
	end
	n = n
end

puts ( "Input the value to calculate fibonacci's number: " )
n = geti
i = fibonacci( n )
puts ( "Fibonacci's number for #{i} is #{n}" )

