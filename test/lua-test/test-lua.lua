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

print("nullValue type: " .. type(API.nullValue))
print("nullValue: " .. (API.nullValue))
API.nullValue = 1
print("nullValue: " .. (API.nullValue))