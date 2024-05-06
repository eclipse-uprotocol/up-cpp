all : tut my_plugin_sum.so

my_plugin_sum.so : my_plugin_sum.cpp
	g++ --std=c++17 --shared -fPIC $< -o $@

tut : tutorial1.cpp
	g++ --std=c++17 $< -o $@ -ldl
