class Punto
	@x = 0
	@y = 0
end

p = Punto.new
p.x = 2
p.y = p.x + 1
puts "p.x = #{p.x} p.y = #{p.y}"
