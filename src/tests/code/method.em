###############################################################################
# Emerald - code test
# Sum two values and print the result
# Language's tested features: output, methods.
###############################################################################


def f1( arg1, arg2, arg3 )
	puts( "vector (MET_ARG): #{arg1} + #{arg2} = #{arg3}\n" )
end

f1( 1, 2, 3 )

def f2()
	a = 1
	b = 2
	c = a + b
	puts( "suma (MET_LOCAL): #{a} + #{b} = #{c}\n" )
end

f2()

a1 = 25
a2 = 50
a3 = a1 + a2

puts( "sum (MAIN): #{a1} + #{a2} = #{a3}\n" )

b1 = 1
b2 = 'a'
b3 = 'b'
b4 = 5.25
b5 = 2

puts( "vector (MAIN): #{b1}, #{b2}, #{b3}, #{b4}, #{b5}\n" )

