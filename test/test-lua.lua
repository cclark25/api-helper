print(API)
print(API.testString)
print(API.testInt)

print(API.objectExample.testString);
print(API.objectExample.testInt);

function __TS__ObjectEntries(obj)
    local result = {}
    for key in pairs(obj) do
        result[#result + 1] = {key, obj[key]}
    end
    return result
end

-- do
--     local function ____catch(e)
--         print("Error Caught")
--         print((e))

--     end
--     local ____try, ____hasReturned = pcall(function()
--         API.testFunction(111, "ABC 123", 123);
--     end)
--     if not ____try then
--         ____catch(____hasReturned)
--     end
-- end

API.testFunction(111, "ABC 123", 123);

print(#API.arrayExample)
local function __TS__ObjectEntries(obj)
    local result = {}
    for key in pairs(obj) do
        result[#result + 1] = {key, obj[key]}
    end
    return result
end

for ____, ____value in ipairs(__TS__ObjectEntries(API.arrayExample)) do
    local key = ____value[1]
    local val = ____value[2]
    print((("Key: \"" .. key) .. "\"\tValue: ") .. tostring(val) .. "\tKey Type: " .. type(key) .. "\tType: " .. type(val))
end


for ____, ____value in ipairs(__TS__ObjectEntries(__TS__Class())) do
    local key = ____value[1]
    local val = ____value[2]
    print((("Key: \"" .. key) .. "\"\tValue: ") .. tostring(val) .. "\tKey Type: " .. type(key) .. "\tType: " .. type(val))
end


