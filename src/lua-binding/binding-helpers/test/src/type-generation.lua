local ____lualib = require("lualib_bundle")
local __TS__Unpack = ____lualib.__TS__Unpack
local __TS__ObjectValues = ____lualib.__TS__ObjectValues
local __TS__ArrayFilter = ____lualib.__TS__ArrayFilter
local __TS__ObjectEntries = ____lualib.__TS__ObjectEntries
local __TS__ArrayMap = ____lualib.__TS__ArrayMap
local __TS__ObjectFromEntries = ____lualib.__TS__ObjectFromEntries
local __TS__ObjectAssign = ____lualib.__TS__ObjectAssign
local ____exports = {}
local ____binding_2Dhelpers = require("src.binding-helpers")
local bindingHelpersModuleName = ____binding_2Dhelpers.bindingHelpersModuleName
local moduleResolutionPath = ____binding_2Dhelpers.moduleResolutionPath
local ____binding_2Dhelpers = require("src.binding-helpers")
local BindingHelpers = ____binding_2Dhelpers.default
local TypeGeneration = {}
do
    function TypeGeneration.declareClass(self, classDef)
        local classDefinition = classDef.classDefinition
        if not classDefinition then
            return nil
        end
        local instance = TypeGeneration.generateType(nil, classDefinition.classInstanceType)
        local result = {
            typeName = classDefinition.className,
            typeDef = classDef,
            typeString = function(____, name, typeString, dependencies) return ((((("class " .. name) .. " extends ") .. ((((("( " .. bindingHelpersModuleName) .. ".") .. BindingHelpers.APIClass.name) .. " as { new(): ( ") .. dependencies.classInstanceType.compiledType) .. " ) } )") .. " { ") .. dependencies.constructorType.compiledType) .. "; }" end,
            dependentTypes = {
                classInstanceType = instance,
                constructorType = TypeGeneration.generateType(nil, classDefinition.constructor)
            }
        }
        return result
    end
    function TypeGeneration.generateType(self, typingParams)
        local mappers = {
            string = function() return {
                typeName = typingParams.dataPrimitive,
                typeDef = typingParams,
                typeString = function() return "string" end,
                dependentTypes = {},
                isLiteralType = true
            } end,
            int32 = function() return {
                typeName = typingParams.dataPrimitive,
                typeDef = typingParams,
                typeString = function() return "number" end,
                dependentTypes = {},
                isLiteralType = true
            } end,
            null = function() return {
                typeName = typingParams.dataPrimitive,
                typeDef = typingParams,
                typeString = function() return "null" end,
                dependentTypes = {},
                isLiteralType = true
            } end,
            unknown = function() return {
                typeName = typingParams.dataPrimitive,
                typeDef = typingParams,
                typeString = function() return "unknown" end,
                dependentTypes = {},
                isLiteralType = true
            } end,
            object = function() return {
                typeName = typingParams.dataPrimitive,
                typeDef = typingParams,
                typeString = function(____, name, typeString, dependencies)
                    local fieldTypesString = ""
                    local objectFields = {}
                    if typingParams.objectFields then
                        objectFields = typingParams.objectFields
                    end
                    for ____, ____value in ipairs(__TS__ObjectEntries(objectFields)) do
                        local fieldName = ____value[1]
                        local field = ____value[2]
                        fieldTypesString = fieldTypesString .. ((("\"" .. fieldName) .. "\": ( ") .. dependencies[fieldName].compiledType) .. " ); "
                    end
                    return ("{ " .. fieldTypesString) .. " }"
                end,
                dependentTypes = __TS__ObjectFromEntries(__TS__ArrayMap(
                    __TS__ObjectEntries(typingParams.objectFields and typingParams.objectFields or ({})),
                    function(____, ____bindingPattern0)
                        local field
                        local fieldName
                        fieldName = ____bindingPattern0[1]
                        field = ____bindingPattern0[2]
                        return {
                            fieldName,
                            TypeGeneration.generateType(nil, field)
                        }
                    end
                )),
                isLiteralType = true
            } end,
            array = function() return {
                typeName = typingParams.dataPrimitive,
                typeDef = typingParams,
                typeString = function(____, name, typeString, dependencies) return ((((bindingHelpersModuleName .. ".") .. BindingHelpers.ArrayAccessor.name) .. "<") .. (typingParams.arrayOf and ("( " .. dependencies.arrayOf.compiledType) .. " )" or "unknown")) .. ">" end,
                dependentTypes = {arrayOf = typingParams.arrayOf and TypeGeneration.generateType(nil, typingParams.arrayOf) or nil},
                isLiteralType = true
            } end,
            ["function"] = function() return {
                typeName = typingParams.dataPrimitive,
                typeDef = typingParams,
                typeString = function(____, name, typeString, dependencies)
                    if not typingParams.functionDefinition then
                        return "( ()=>unknown )"
                    end
                    local parameterString = ("( " .. table.concat(
                        __TS__ArrayMap(
                            typingParams.functionDefinition.parameters,
                            function(____, p) return ((p.parameterName .. ": ( ") .. dependencies["parameter-" .. p.parameterName].compiledType) .. " )" end
                        ),
                        ", "
                    )) .. " )"
                    if not typingParams.functionDefinition.isConstructor then
                        return ((("( " .. parameterString) .. " => ( ") .. dependencies.returnType.compiledType) .. " ) )"
                    else
                        local ____ = 456
                        return "constructor" .. parameterString
                    end
                end,
                dependentTypes = __TS__ObjectAssign(
                    {},
                    typingParams.functionDefinition and __TS__ObjectFromEntries(__TS__ArrayMap(
                        typingParams.functionDefinition.parameters,
                        function(____, p) return {
                            "parameter-" .. p.parameterName,
                            TypeGeneration.generateType(nil, p.type)
                        } end
                    )) or ({}),
                    {returnType = typingParams.functionDefinition and TypeGeneration.generateType(nil, typingParams.functionDefinition.returnType) or nil}
                ),
                isLiteralType = true
            } end,
            classType = function() return TypeGeneration.declareClass(nil, typingParams) end,
            classInstance = function() return TypeGeneration.generateType(
                nil,
                __TS__ObjectAssign({}, typingParams, {dataPrimitive = "object"})
            ) end
        }
        local mapped = mappers[typingParams.dataPrimitive](mappers)
        return mapped
    end
    function TypeGeneration.declareType(self, typeName, typeDef, dependentTypes)
        local result = {
            typeName = typeName,
            typeDef = typeDef,
            typeString = function(____, name, typeString, dependencies) return ((("type " .. name) .. " = ") .. dependencies.typing.compiledType) .. "; " end,
            dependentTypes = dependentTypes
        }
        return result
    end
    function TypeGeneration.declareConst(self, def)
        local result = {
            typeName = "API",
            typeDef = def,
            typeString = function(____, name, typeString, dependencies) return ((("const " .. name) .. ": ") .. dependencies.typing.compiledType) .. " ;" end,
            dependentTypes = {typing = TypeGeneration.generateType(nil, def)}
        }
        return result
    end
    function TypeGeneration.extractAllTypes(self, types)
        local allTypes = {__TS__Unpack(types)}
        for ____, typing in ipairs(types) do
            allTypes[#allTypes + 1] = __TS__Unpack(TypeGeneration.extractAllTypes(
                nil,
                __TS__ArrayFilter(
                    __TS__ObjectValues(typing.dependentTypes),
                    function(____, v) return v end
                )
            ))
        end
        return allTypes
    end
    function TypeGeneration.generateTypeFiles(self, moduleName, types)
        local allTypes = {__TS__Unpack(TypeGeneration.extractAllTypes(nil, types))}
        local typeEntries = {}
        for ____, t in ipairs(allTypes) do
            local dependencies = __TS__ArrayFilter(
                __TS__ObjectEntries(t.dependentTypes),
                function(____, ____bindingPattern0)
                    local v
                    local k = ____bindingPattern0[1]
                    v = ____bindingPattern0[2]
                    return v
                end
            )
            local unresolvedDependencies = __TS__ArrayFilter(
                dependencies,
                function(____, ____bindingPattern0)
                    local v
                    local k = ____bindingPattern0[1]
                    v = ____bindingPattern0[2]
                    return v.__generatedType == nil
                end
            )
            local resolvedDependencies = __TS__ArrayFilter(
                dependencies,
                function(____, ____bindingPattern0)
                    local v
                    local k = ____bindingPattern0[1]
                    v = ____bindingPattern0[2]
                    return v.__generatedType ~= nil
                end
            )
            if #unresolvedDependencies == 0 then
                local dependencyMaps = __TS__ObjectFromEntries(__TS__ArrayMap(
                    resolvedDependencies,
                    function(____, ____bindingPattern0)
                        local v
                        local k
                        k = ____bindingPattern0[1]
                        v = ____bindingPattern0[2]
                        return {k, {compiledType = v.__generatedType, dependency = v}}
                    end
                ))
                local typeString = t:typeString(t.typeName, "aaaabbbbccccddddd", dependencyMaps)
                if t.typeDef.dataPrimitive == "classType" then
                    t.__generatedType = "typeof " .. t.typeName
                elseif t.isLiteralType then
                    t.__generatedType = typeString
                else
                    t.__generatedType = t.typeName
                end
                if not t.isLiteralType then
                    typeEntries[#typeEntries + 1] = typeString
                end
            else
                allTypes[#allTypes + 1] = t
            end
        end
        local typingFile = ((((((((("import type " .. bindingHelpersModuleName) .. " from '") .. moduleResolutionPath) .. "';\ndeclare module ") .. moduleName) .. " {\n") .. table.concat(typeEntries, "\n\n\texport ")) .. "\n};\nexport default ") .. moduleName) .. ";"
        return typingFile
    end
end
____exports.default = TypeGeneration
return ____exports
