local ____lualib = require("lualib_bundle")
local __TS__ObjectAssign = ____lualib.__TS__ObjectAssign
local __TS__StringSplit = ____lualib.__TS__StringSplit
local __TS__ArraySlice = ____lualib.__TS__ArraySlice
local __TS__ArrayConcat = ____lualib.__TS__ArrayConcat
local ____exports = {}
local ____child_process = require("test.child_process")
local exec = ____child_process.exec
local fs = require("test.fs")
local ____type_2Dgeneration = require("src.type-generation")
local TypeGeneration = ____type_2Dgeneration.default
local classDefinition = {dataPrimitive = "classType", classDefinition = {className = "TestClass", classInstanceType = {dataPrimitive = "classInstance", objectFields = {abc = {dataPrimitive = "int32"}, def = {dataPrimitive = "string"}}}, constructor = {dataPrimitive = "function", functionDefinition = {parameters = {{parameterName = "p1", type = {dataPrimitive = "int32"}}, {parameterName = "p2", type = {dataPrimitive = "string"}}}, returnType = {dataPrimitive = "unknown"}, isConstructor = true}}}}
local api = {dataPrimitive = "object", objectFields = {
    f1 = {dataPrimitive = "string", objectFields = {}},
    f2 = {dataPrimitive = "int32", objectFields = {}},
    f3 = {dataPrimitive = "object", objectFields = {abc = {dataPrimitive = "int32", objectFields = {}}, def = {dataPrimitive = "string", objectFields = {}}}},
    f4 = {dataPrimitive = "array", arrayOf = {dataPrimitive = "object", objectFields = {a = {dataPrimitive = "array"}, b = {dataPrimitive = "array", arrayOf = {dataPrimitive = "string"}}}}},
    f5 = {dataPrimitive = "function"},
    f6 = {dataPrimitive = "function", functionDefinition = {parameters = {{parameterName = "param1", type = {dataPrimitive = "string"}}, {parameterName = "param2", type = {dataPrimitive = "object", objectFields = {f1 = {dataPrimitive = "string"}, f2 = {dataPrimitive = "int32"}}}}}, returnType = {dataPrimitive = "object", objectFields = {abc = {dataPrimitive = "int32"}}}}},
    TestClass = classDefinition
}}
local newAPI = TypeGeneration:declareClass({
    dataPrimitive = "classType",
    classDefinition = {
        className = "APIState",
        classInstanceType = __TS__ObjectAssign({}, api, {dataPrimitive = "classInstance"}),
        constructor = {dataPrimitive = "function", functionDefinition = {parameters = {}, returnType = {dataPrimitive = "object"}, isConstructor = true}}
    }
})
local typeFileString = TypeGeneration:generateTypeFiles("TestModule", {newAPI})
local path = __TS__ArraySlice(
    __TS__StringSplit(
        fs:realpathSync(process.argv[2]),
        "/"
    ),
    0,
    -1
)
local testFilePath = table.concat(
    __TS__ArrayConcat(path, {"temp", "testFile.ts"}),
    "/"
)
fs:writeFileSync(testFilePath, typeFileString)
exec(
    nil,
    (("ts-node " .. testFilePath) .. " --cwd ") .. table.concat(path, "/"),
    function(____, e, out, err)
        print(out)
        print(err)
    end
)
return ____exports
