###############################################################################
# Emerald - semantic test
# Create and use a class.
# Language's tested features: classes, output, and method calling.
###############################################################################

# Class definition.
class Punto
	@x = 0
	@y = 0
end

# Class instances creation.
p = Punto.new
q = p

# Instance elements output.
puts("p.x = #{p.x} p.y = #{p.y}")

# Method definition and calling
def sum(var1, var2)
 a = var1 + var2
end 

foo( p.x, p.y )
