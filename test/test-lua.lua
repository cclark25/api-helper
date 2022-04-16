print(API)
print(API.testString)
print(API.testInt)

print(API.objectExample.testString);
print(API.objectExample.testInt);


API.testFunction(111, "ABC 123", 123);

print(#API.arrayExample)

local ____lualib = require('lualib_bundle');
print("Class Name: " .. (API.TestClass.name))
testClassInstance = ____lualib.__TS__New(API.TestClass)
print("testClassInstance abc: " .. testClassInstance.abc)
print("testClassInstance def: " .. testClassInstance.def)