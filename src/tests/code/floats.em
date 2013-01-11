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


fv = Array.new( 10, 1.25 )

#i = 2
#while( i < 10 ) do
#	fv[i] = fv[i - 2] + fv[i - 1]
#	i = i + 1
#end

fv.each do |element|
	puts( "#{element}\n" )
end


#i = 0
#while( i < 10 ) do
#	puts( "#{fv[i]}\n" )
#	i = i + 1
#end

#iv = [25, 50, 0]
#iv[2] = iv[0] + iv[1]
#puts( "#{iv[0]}, #{iv[1]}, #{iv[2]}\n" )

#fv = [1.5, 1.75, 0.5]
#fv[2] = fv[0] + fv[1]

#puts( "#{fv[0]}\n" )
#puts( "#{fv[1]}\n" )
#puts( "#{fv[2]}\n" )

#puts( "#{fv[3]}\n" )
#puts( "#{fv[1]}, #{fv[2]}\n" )

#puts( "#{fv[2]}, #{fv[1]}\n" )
#puts( "#{fv[0]}, #{fv[2]}\n" )
#puts( "#{fv[0]}, #{fv[1]}, #{fv[2]}\n" )


