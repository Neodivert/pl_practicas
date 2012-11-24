class Punto
	@x = 0
	@y = 0
end

a = [1]
p = Punto.new
p.x = 2.1
p.y = p.x + 1
puts("p.x = #{p.x} p.y = #{p.y}")
