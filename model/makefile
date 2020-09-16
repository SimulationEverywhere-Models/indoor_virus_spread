all : vp_main.cpp vp_coupled.hpp vp_lab_cell.hpp 
	g++ -g -I ../../cadmium-json/include -I ../../cadmium-json/json/include -std=c++17 -o vp vp_main.cpp vp_coupled.hpp vp_lab_cell.hpp
	
clean: 
	rm -f vp.exe