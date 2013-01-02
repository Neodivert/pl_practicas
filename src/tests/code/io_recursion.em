def recursive(i)
	puts("Soy la llamada #{i}\n")
	if(i == 0)
		puts("Termine")
	else
		recursive(i - 1)	
	end	
end
recursive(10)	
