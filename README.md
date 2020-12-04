# Viral particle spread indoors

## Description 
Simulates viral particle movement and infection rates in enclosed environment.

## Table of Contents  
- [Installation](#installation)  
- [Models](#models)
- [Scripts](#scripts)

## Installation
1. Make sure you have cadmium installed before proceding, see the [User Manual](http://www.sce.carleton.ca/courses/sysc-5104/lib/exe/fetch.php?media=cadmium.pdf) for more details.
2. Navigate to **CADMIUM/** directory and run `git clone https://github.com/SimulationEverywhere/Cell-DEVS-Cadmium-Simulation-Environment`
3. Navigate to **CADMIUM/Cell-DEVS-Cadmium-Simulation-EnvironmentRun** and run `git submodule update --init --recursive`
4. Navigate to **CADMIUM/Cell-DEVS-Cadmium-Simulation-EnvironmentRun/Cadmium-Cell-DEVS-Models** and clone this repository using `git clone https://github.com/SimulationEverywhere-Models/indoor_virus_spread`

## Models
- [basic 2D model](./indoor_spread_basic_2D)
- [advanced 2D model](./indoor_spread_2D)
- [quanta 2D model](./indoor_quanta_2D)

## Scripts 
Scripts for converting scenarios to JSON and charting data
- [scripts](./scripts)
