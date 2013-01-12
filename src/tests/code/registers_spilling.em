###############################################################################
# Emerald - code test
# Operations which need more registers than available
# Language's tested features: expressions, register spilling
###############################################################################

integer = 1 + 2 + 3 + 7 * 3 * 9 + 5 * (5 + 6 * 7) - 30 + 5 * (4 + 2 * 3)

puts( "integer (expected value: 450): #{integer}\n" ) 

float = 0.01 + 0.02 + 0.03 + 0.07 * 3.0 * 9.0 + 0.05 * (5.0 + 6.0 * 7.0) - .30 + 0.05 * (4.0 + 2.0 * 3.0)

puts( "float (expected value: 4.5): #{float}\n" ) 
