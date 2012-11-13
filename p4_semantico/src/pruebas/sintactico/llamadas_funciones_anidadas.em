def multiplicar a, b
  res = a * b
  res
end

a = [1, 2, 3, 4]

a.each do |i|
	b = 4
	c = multiplicar i, b
	puts c
end
