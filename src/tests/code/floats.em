###############################################################################
# Emerald - code test
# Operations with floats
# Language's tested features: output, methods, floats.
###############################################################################

# Expression with floats (locals)
def floatExpression
	a = 1.25
	b = 7.75
	c = 3.36 * 15.3 + 5.12  * b / a + 32.5 + a
	puts( "Expression with local floats (expected result: 116,902): #{c}\n" )
end
res = floatExpression() * 2.0
puts( "Returned float * 2.0 (expected result: 233,804): #{res}\n" )


# Expression with floats (globals)
$d = 1.25
$e = 7.75
$f = 3.36 * 15.3 + 5.12 * $e / $d + 32.5 + $d
puts( "Expression with global floats (expected result: 116,902): #{$f}\n" )


# Float array - Iteration and modification
fv = Array.new( 10, 1.25 )

i = 2

while( i < 10 ) do
	fv[i] = fv[i - 2] + fv[i - 1]
	i = i + 1
end

# FIXME: se muestra siempre 1.25
fv.each do |e|
	if( e > 5.9 ) then
		puts( "A: #{e}\n" )
	else
		puts( "B: #{e}\n" )
	end
end
