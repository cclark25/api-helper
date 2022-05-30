
print("API type: " .. type(API))
print("stringValue type: " .. type(API.stringValue))
print("stringValue: " .. (API.stringValue))
API.stringValue = "New String Value"
print("stringValue: " .. (API.stringValue))

-- do
--     while true do
--         -- print(API.stringValue)
--         API.stringValue = "String Value"
--     end
-- end

print("intValue type: " .. type(API.intValue))
print("intValue: " .. (API.intValue))
API.intValue = 1
print("intValue: " .. (API.intValue))

-- do
--     while true do
--         -- print(API.stringValue)
--         API.intValue = API.intValue + 1
--     end
-- end

print("objectValue type: " .. type(API.objectValue))
print("objectValue field1: " .. (API.objectValue.field1))
API.objectValue.field1 = "New field string"
print("objectValue field1: " .. (API.objectValue.field1))
API.objectValue = { field1 = "field1 value", field2 = "field2 value"}
print("objectValue field1: " .. (API.objectValue.field1))
print("objectValue field2: " .. (API.objectValue.field2))

-- local ____lualib = require('lualib_bundle');
-- for ____, ____value in ipairs(____lualib.__TS__ObjectEntries(API.objectValue)) do
--     local key = ____value[1]
--     local value = ____value[2]
--     print((("objectValue at field \"" .. key) .. "\": ") .. tostring(value))
-- end

-- print("Iterator: " .. tostring(____lualib.Symbol.iterator))

-- local ____lualib = require('lualib_bundle');
-- print("arrayValue type: " .. type(API.arrayValue))
