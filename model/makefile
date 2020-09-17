all : vp_main.cpp vp_coupled.hpp vp_res_cell.hpp 
	g++ -g -I ../../../cadmium/include -I ../../../cadmium/json/include -std=c++17 -o vp vp_main.cpp vp_coupled.hpp vp_res_cell.hpp
	
clean: 
	rm -f vp.exe