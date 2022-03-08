SHELL = /bin/bash
FLAGS = -std=c++17 -O0 -Wall -g
COMPILER = c++
LIBS = -L/opt/homebrew/lib -L${HOME}/lib -lSDL2 -lm
LIBS2 = -L/opt/homebrew/lib -lGL -lglew -lglfw
INCLUDE = -I/opt/homebrew/include -I${HOME}/include -I${PWD}
INCLUDE2 = -I/opt/homebrew/include -I${PWD}/gl_wrappers2d -I${PWD}
SOURCES = eigenstates_1d.cpp eigenstates_2d.cpp \
${PWD}/gl_wrappers2d/gl_wrappers.cpp
OBJECTS = eigenstates_1d.o eigenstates_2d.o

SOURCES2 = ${PWD}/gl_wrappers2d/gl_wrappers.cpp draw_view.cpp \
init_energy_states.cpp init_potential.cpp \
input_handling.cpp to_rgba_array.cpp main.cpp
OBJECTS2 = gl_wrappers.o draw_view.o init_energy_states.o \
init_potential.o input_handling.o to_rgba_array.o main.o
HEADERS2 = complex_float_rgba.hpp draw_view.hpp init_energy_states.hpp \
init_potential.hpp input_handling.hpp params.hpp shader_programs.hpp \
to_rgba_array.hpp shaders.hpp

TARGET1 = ${PWD}/program
TARGET2 = ${PWD}/program2
TARGET3 = program3
TARGET4 = main.js

all: ${TARGET1} ${TARGET2}

${TARGET1}: eigenstates_1d.o 
	${COMPILER} ${FLAGS} -o $@ $^ ${LIBS}

${TARGET2}: eigenstates_2d.o 
	${COMPILER} ${FLAGS} -o $@ $^ ${LIBS}

${OBJECTS2}: ${SOURCES2} ${HEADERS2}
	${COMPILER} -c ${SOURCES2} ${FLAGS} ${INCLUDE2}

${TARGET3}: ${OBJECTS2}
	${COMPILER} ${FLAGS} -o $@ $^ ${LIBS2}

${OBJECTS}: ${SOURCES}
	${COMPILER} -c $^ ${FLAGS} ${INCLUDE}

${OBJECTS2}: ${SOURCES2} ${HEADERS2}
	${COMPILER} -c ${SOURCES2} ${FLAGS} ${INCLUDE2}

${TARGET4}: ${SOURCES2} ${HEADERS2} shaders.hpp
	emcc -o $@ -I${PWD}/gl_wrappers2d -I${PWD} ${SOURCES2} \
	-std=c++17 -O3 -s WASM=1 -s USE_GLFW=3 -s FULL_ES2=1 \
	-s ALLOW_MEMORY_GROWTH=1

shaders.hpp:
	python3 make_shaders_h.py

clean:
	rm -rf ${OBJECTS} ${TARGET1} ${TARGET2} ${OBJECTS2}