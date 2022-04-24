-- print(API)

-- print("testString: " .. API.testString)
-- API.testString = "Overriding the test string."
-- print("testString: " .. API.testString)

-- print("testInt:" .. API.testInt)

-- print("objectExample.testString: " .. API.objectExample.testString);

-- print("objectExample.testInt: " .. API.objectExample.testInt);

-- API.testFunction(111, "ABC 123", 123);

-- print(#API.arrayExample)

local ____lualib = require('lualib_bundle');
print("Class Name: " .. (API.TestClass.name))
testClassInstance = ____lualib.__TS__New(API.TestClass)
for ____, ____value in ipairs(____lualib.__TS__ObjectEntries(API.TestClass)) do
    local key = ____value[1]
    local value = ____value[2]
    print("Static field \"" .. (key .. "\" of TestClass: ") .. tostring(value))
end
for ____, ____value in ipairs(____lualib.__TS__ObjectEntries(testClassInstance)) do
    local key = ____value[1]
    local value = ____value[2]
    print("Instance field \"" .. (key .. "\" of TestClass instance: ") .. tostring(value))
end

print(testClassInstance.testInt)
print(API.TestClass.testString)
funcResult = API.testFunction(111, "ABC 123", 123)
print(funcResult)