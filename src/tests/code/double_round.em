###############################################################################
# Emerald - code test
# Force analyzer to analyze the code twice (because of calling to a method 
# not defined yet in the code)
# Language's tested features: dependences between methods, method return.
###############################################################################

# Call to f1 (not defined yet).
a = f1()

# f1 definition, with a call to f2 (not defined yet).
def f1()
	res1 = f2()
end

# f2 definition.
def f2()
   res2 = 3
end

# Show the result.
puts( "a = #{a}\n" )
