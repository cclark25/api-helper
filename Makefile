SHELL=/bin/bash

CXX= "g++-10"
CFLAGS=  -Wall -pedantic -ggdb -fPIC -lpthread

OUT_DIR=./dist

testString:="abc"
includeParameters=-I"./include" -I"./include/lua/include" -I"./include/sol" -I"./include/typescript-to-lua"
cppVersionParameter=--std=c++2a
gppAdditionalParameter="-g"

out=dist/engine

test: $(out)
	echo "Test Built";

$(out): ./src/**/* ./src/* ./test/lua-test/*
	# make $(lualibFile)
	$(CXX) $(gppAdditionalParameter) $(cppVersionParameter) ./test/lua-test/main.cpp $(includeParameters) -L"./include/lua/lib" -I"./include/json" -llua -ldl -lpthread -o ${out}

# $(lualibFile): $(luaLibSrc)
# 	echo -e "#ifndef T2L_HEADERS \n#define T2L_HEADERS" > $(lualibFile); \
# 	echo "unsigned char lualib_bundle[] = {" >> $(lualibFile); \
# 	\
# 	cat ./dependencies/TypeScriptToLua/dist/lualib/lualib_bundle.lua | xxd -i >> $(lualibFile); \
# 	\
# 	echo -e "};\n" >> $(lualibFile); \
# 	echo "unsigned int lualib_bundle_length = `cat $(lualibFile) | grep 0x | sed 's/,/\n/g' | grep 0x | wc -l`;" >> $(lualibFile)
# 	echo "#endif" >> $(lualibFile); 

# $(luaLibSrc): 
# 	cd dependencies/TypeScriptToLua;\
# 	npm install;\
# 	npm run build;