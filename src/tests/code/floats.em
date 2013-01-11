###############################################################################
# Emerald - code test
# Operations with floats
# Language's tested features: output, methods, floats.
###############################################################################

# Expression with floats (locals)
def floatExpression
	a = 1.25
	b = 7.75
	c = 3.36 * 15.3 + 5.12 * b / a + 32.5 + a
	puts( "Expression with local floats (expected result: 123,402): #{c}\n" )
end
res = floatExpression()
puts( "Returned float (expected result: 123,402): #{res}\n" )


# Expression with floats (globals)
$d = 1.25
$e = 7.75
$f = 3.36 * 15.3 + 5.12 * $e / $d + 32.5 + $d
puts( "Expression with global floats (expected result: 123,402): #{$f}\n" )

#a = 1.25
#b = 3

#c = 1.30 + a + 1.35

#puts( "#{c}\n" )

#c = a*5.32-12.4+b/1.5
#def foo(i,j,k,l,m,n,o,p,q,r,s,t)
#	puts("Los argumentos valen #{i} #{j} #{k} #{l} #{m} #{n}\n")
#	puts("Los argumentos valen #{i} #{j} #{k} #{l} #{m} #{n} #{p} #{r} #{t}\n")
#end

#foo(2,3,4,5,6,7,8,9,10,11.5,12,'a')

#def sumaF( arg1, arg2 )
#	suma = arg1 + arg2

#	puts( "#{arg1} + #{arg2} = #{suma}\n" )
#end

#def sumaI( arg1, arg2 )
#	suma = arg1 + arg2

#	puts( "#{arg1} + #{arg2} = #{suma}\n" )
#end

#a = 1.25
#b = 2.50
#c = a + b
#puts( "#{a} + #{b} = #{c}\n" )
#sumaF( 1.25, 2.5 )
#sumaI( 4, 5 )

fv = Array.new( 10, 1.25 )
i = 2
while( i < 10 ) do
	fv[i] = fv[i - 2] + fv[i - 1]
	i = i + 1
end

i = 0
while( i < 10 ) do
	puts( "#{fv[i]}\n" )
	i = i + 1
end

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


