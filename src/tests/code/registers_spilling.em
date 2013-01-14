###############################################################################
# Emerald - code test
# Operations which need more registers than available
# Language's tested features: expressions, registers spilling
###############################################################################

# Integers
integer = 1 + 2 + 3 + 7 * 3 * 9 + 5 * (5 + 6 * 7) - 30 + 5 * (4 + 2 * 3)
puts( "integer (expected value: 450): #{integer}\n" ) 

# Floats
float = 0.01 + 0.02 + 0.03 + 0.07 * 3.0 * 9.0 + 0.05 * (5.0 + 6.0 * 7.0) - .30 + 0.05 * (4.0 + 2.0 * 3.0)
puts( "float (expected value: 4.5): #{float}\n" ) 

# Variables (integers)
a = 1
b = 2
c = 3
d = 7
e = 3
f = 9
g = 5
h = 5
i = 6
j = 7
k = 30
l = 5
m = 4
n = 2
o = 3
p = a + b + c + d * e * f + g * (h + i * j) - k + l * (m + n * o)
puts( "integer (expected value: 450): #{p}\n" )
