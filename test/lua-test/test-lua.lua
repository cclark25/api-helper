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
API.objectValue = {
    field1 = "field1 value",
    field2 = "field2 value"
}
print("objectValue field1: " .. (API.objectValue.field1))
print("objectValue field2: " .. (API.objectValue.field2))

local ____lualib = require('lualib_bundle');
print("arrayValue type: " .. type(API.arrayValue))
print("arrayValue length type: " .. type(API.arrayValue.length))
print("arrayValue length: " .. (API.arrayValue.length))
print("arrayValue at 0: " .. (API.arrayValue.getIndex(0)))
print("arrayValue at 1: " .. (API.arrayValue.getIndex(1)))
print("arrayValue at 2: " .. (API.arrayValue.getIndex(2)))
do
    local function ____catch()
        print("Successfully got null when out of range.")
    end
    local ____try = pcall(function()
        print("arrayValue at 3: " .. (API.arrayValue.getIndex(3)))

    end)
    if not ____try then
        ____catch()
    end
end
print("arrayValue length after push: " .. tostring(API.arrayValue.push(6)));
print("arrayValue at 3: " .. (API.arrayValue.getIndex(3)))
API.arrayValue.setIndex(3, "New test string");
print("arrayValue length after setIndex: " .. tostring(API.arrayValue.length));
print("arrayValue at 3: " .. (API.arrayValue.getIndex(3)))
print("arrayValue popped value: " .. tostring(API.arrayValue.pop()));
print("arrayValue length after pop: " .. tostring(API.arrayValue.length));
print("arrayValue length after insert: " .. (API.arrayValue.insert(2, "New inserted string at index 2")))
print("arrayValue at 2: " .. (API.arrayValue.getIndex(2)))

do
    i = 0
    while i < API.arrayValue.length do
        print((tostring(i) .. ": ") .. tostring(API.arrayValue.getIndex(i)))
        i = i + 1
    end
end

print("functionValue type: " .. type(API.functionValue))
print("functionValue result: " .. (API.functionValue(1, 2, "abc")))
API.functionValue = function(a, b, c)
    return "Params: " .. tostring(a) .. ", " .. tostring(b) .. ", " .. tostring(c) .. "\tintValue" .. API.intValue;
end
print("functionValue result: " .. (API.functionValue(1, 2, "abc")))

i = 1
totalTime = 0;
while i <= 1000000 do
    start = os.time()
    API.functionValue(i, i + 1, "abc")
    API.functionValue = function(a, b, c)
        return "Params v" .. i .. ": " .. tostring(a) .. ", " .. tostring(b) .. ", " .. tostring(c) .. "\tintValue" .. API.intValue;
    end
    API.functionValue(i, i + 1, "abc");
    duration = os.time() - start
    totalTime = totalTime + duration;
    i = i + 1;

    if i % 100000 == 0 then
        print("Calls per second: " .. (i / totalTime));
    end
end
