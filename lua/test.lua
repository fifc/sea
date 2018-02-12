print("Hello from Lua")
print("Lua code is capable of reading the value set from C++", cppvar)
luavar = cppvar * 3

function entries(arg) -- iterator
	return function()
		return arg:pop();
	end
end

--function myluafunction(times, the_list)
function myluafunction(times)
	for i in entries(the_list) do
		io.write("From LUA:  ", i, "\n")
	end

	for i=1,10 do
		the_list:push(50+i*100);
	end
	return string.rep("(-)", times)
end

function myfunction(arg)
	return cppfunction(arg)
end
