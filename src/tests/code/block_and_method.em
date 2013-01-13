###############################################################################
# Emerald - semantic test
# Multiply a vector's elements by a constant.
# Language's tested features: block and method calling, arrays.
###############################################################################

# Multiplication's method.
def multiply( a, b )
  res = a * b
end

# Vector definition.
v = [1, 2, 3, 4]

# (Block) Iterate over the elements of vector and call Multiply.
v.each do |i|
	b = 4
	c = multiply( i, b )
	puts( "#{c}\n" )
end
