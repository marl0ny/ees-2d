SHELL = /bin/bash
FLAGS = -O2 -g
C_COMPILE = clang
CPP_COMPILE = clang++ -std=c++17
LINKER = /usr/bin/ld

ifeq ($(shell uname),Darwin)
INCLUDE =  -I${PWD} -I${PWD}/gl_wrappers -I/opt/homebrew/include
LIBS = -ldl -L/opt/homebrew/lib -lglfw\
       -framework CoreVideo -framework OpenGL -framework IOKit\
       -framework Cocoa -framework Carbon
else
INCLUDE =  -I${PWD} -I${PWD}/gl_wrappers
LIBS = -lm -lGL -lGLEW -lglfw
endif

# Make sure to source <emcc_location>/emsdk/emsdk_env.sh first!
WEB_TARGET = main.js

TARGET = ${PWD}/program
DATA_DEPENDENCIES = parameters.json
GENERATION_SCRIPTS = make_parameter_files.py
GENERATED_DEPENDENCIES = parameters.hpp
C_SOURCES =
CPP_SOURCES = main.cpp simulation.cpp interactor.cpp gl_wrappers.cpp glfw_window.cpp eigenstates2d.cpp preset_potential.cpp circles_wireframe.cpp clock_hands_wireframe.cpp levels_wireframe.cpp parse.cpp surface.cpp # user_edit_glsl.cpp
SOURCES = ${C_SOURCES} ${CPP_SOURCES}
OBJECTS = main.o simulation.o interactor.o gl_wrappers.o glfw_window.o eigenstates2d.o preset_potential.o circles_wireframe.o clock_hands_wireframe.o levels_wireframe.o parse.o surface.o # user_edit_glsl.o
# SHADERS = ./shaders/*


all: ${TARGET}

${TARGET}: ${OBJECTS}
	${CPP_COMPILE} ${FLAGS} -o $@ ${OBJECTS} ${LIBS}

${WEB_TARGET}: ${SOURCES} ${GENERATED_DEPENDENCIES}
	emcc -lembind -o $@ ${SOURCES} ${INCLUDE} -std=c++17 -O3 -v -s WASM=2 -s USE_GLFW=3 -s FULL_ES3=1 \
	-s ALLOW_MEMORY_GROWTH=1 -s LLD_REPORT_UNDEFINED --embed-file shaders

${OBJECTS}: ${CPP_SOURCES} ${GENERATED_DEPENDENCIES}
	${CPP_COMPILE} ${FLAGS} -c ${CPP_SOURCES} ${INCLUDE}

${GENERATED_DEPENDENCIES}: ${DATA_DEPENDENCIES} ${GENERATION_SCRIPTS}
	python3 make_parameter_files.py

clean:
	rm -f *.o ${TARGET} *.wasm *.js
