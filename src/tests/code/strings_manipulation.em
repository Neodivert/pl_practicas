###############################################################################
# Emerald - semantic test
# Take a string, change its elements and show the result.
# Language's tested features: string manipulation, output.
###############################################################################

# String definition.
string = "abcdefg"

# Print the original string.
string.each do |c1|
	puts( "#{c1}" )
end
puts( "\n" )

# String manipulation loop.
i = 0
while( i < 3 ) do
	var = string[i]
	puts( "I remove '#{var}' " )
	string[i] = 'a'
	var = string[i]
	puts( "and put '#{var}'\n" )
	i = i + 1
end

# Print the final string.
string.each do |c2|
	puts( "#{c2}" )
end
