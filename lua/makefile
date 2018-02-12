all: lua_test

lua_test: lua_test.cc
	#g++ luatest.cc -I /usr/include/lua5.3 -L /usr/lib/x86_64-linux-gnu -llua5.3 -o $@
	g++ lua_test.cc -I /usr/include/lua5.3 -llua5.3 -o $@
clean:
	rm -f *.o *.d lua_test

