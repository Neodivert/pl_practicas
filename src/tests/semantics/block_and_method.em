###############################################################################
# Emerald - semantic test
# Multiply a vector's elements by a constant.
# Language's tested features: block and method calling, arrays.
###############################################################################

# Multiplication's method.
def Multiply a, b
  res = a * b
  res
end

# Vector definition.
a = [1, 2, 3, 4]

# (Block) Iterate over the elements of vector and call Multiply.
a.each do |i|
	b = 4
	c = Multiply i, b
	puts c
end
