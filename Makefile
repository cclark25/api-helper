SHELL=/bin/bash

CXX= g++
CXXFLAGS= -std=c++17 -Wall -pedantic -ggdb
CFLAGS=  -Wall -pedantic -ggdb -fPIC 

allegroFlags= -lallegro -lallegro_image -lallegro_primitives -lallegro_audio

OUT_DIR=./dist
lualibFile=./include/typescript-to-lua/lualib_bundle.h
luaLibSrc=./dependencies/TypeScriptToLua/dist/lualib/lualib_bundle.lua
# DEPENDENCIES=./Dependencies/AllegroCPPWrappers/BUILD/AllegroWrappers.o
test: a.out
	echo "Test Built";

a.out: ./src/**/* ./test/**
	make $(lualibFile)
	g++ ./test/main.cpp --std=c++17 -I"./include" -I"./include/lua/include" -I"./include/sol" -I"./include/typescript-to-lua" -L"./include/lua/lib" -llua -ldl

$(lualibFile): $(luaLibSrc)
	echo -e "#ifndef T2L_HEADERS \n#define T2L_HEADERS" > $(lualibFile); \
	echo "unsigned char lualib_bundle[] = {" >> $(lualibFile); \
	\
	cat ./dependencies/TypeScriptToLua/dist/lualib/lualib_bundle.lua | xxd -i >> $(lualibFile); \
	\
	echo -e "};\n" >> $(lualibFile); \
	echo "unsigned int lualib_bundle_length = `cat $(lualibFile) | grep 0x | sed 's/,/\n/g' | grep 0x | wc -l`;" >> $(lualibFile)
	echo "#endif" >> $(lualibFile); 

$(luaLibSrc): 
	cd dependencies/TypeScriptToLua;\
	npm install;\
	npm run build;