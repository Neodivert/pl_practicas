###############################################################################
# Emerald - semantic test
# Take a string, change its elements and show the result.
# Language's tested features: string manipulation, output.
###############################################################################

# String definition.
string = "abcdefg"

# String manipulation loop.
i = 0
while( i < string.length ) do
	var = string[i]
	puts "I remove\t #{var}\n"
	string[i] = 48 + i
	var = string[i]
	puts "and put\t #{var}\n"
	i = i + 1
end
