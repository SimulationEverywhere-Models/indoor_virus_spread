all : co2_main.cpp co2_coupled.hpp co2_lab_cell.hpp 
	g++ -g -I ../../cadmium-json/include -I ../../cadmium-json/json/include -std=c++17 -o viral co2_main.cpp co2_coupled.hpp co2_lab_cell2.hpp
	
clean: 
	rm -f viral.exe