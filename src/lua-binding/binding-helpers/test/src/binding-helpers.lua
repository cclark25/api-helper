local ____lualib = require("lualib_bundle")
local __TS__ObjectKeys = ____lualib.__TS__ObjectKeys
local __TS__ObjectDefineProperty = ____lualib.__TS__ObjectDefineProperty
local __TS__Class = ____lualib.__TS__Class
local __TS__Symbol = ____lualib.__TS__Symbol
local Symbol = ____lualib.Symbol
local __TS__SetDescriptor = ____lualib.__TS__SetDescriptor
local __TS__Generator = ____lualib.__TS__Generator
local __TS__Iterator = ____lualib.__TS__Iterator
local __TS__New = ____lualib.__TS__New
local __TS__ClassExtends = ____lualib.__TS__ClassExtends
local __TS__InstanceOf = ____lualib.__TS__InstanceOf
local __TS__Unpack = ____lualib.__TS__Unpack
local __TS__ObjectEntries = ____lualib.__TS__ObjectEntries
local ____exports = {}
local BindingHelpers
local moduleWrapper = {BindingHelpers = BindingHelpers}
____exports.bindingHelpersModuleName = __TS__ObjectKeys(moduleWrapper)[1]
____exports.moduleResolutionPath = "binding-helpers"
BindingHelpers = {}
do
    ---
    -- @param key Key on destination to bind the wrapper to.
    -- @param wrapper Data wrapper provided by the API to bind to the destination.
    -- @param destination Destination to bind the wrapper on. Defaults to the module exports.
    function BindingHelpers.bindWrapper(self, key, wrapper, destination)
        if destination == nil then
            destination = module.exports
        end
        __TS__ObjectDefineProperty(destination, key, wrapper)
    end
    BindingHelpers.ArrayAccessor = __TS__Class()
    local ArrayAccessor = BindingHelpers.ArrayAccessor
    ArrayAccessor.name = "ArrayAccessor"
    function ArrayAccessor.prototype.____constructor(self, accessor)
        self.accessor = accessor
        self[Symbol.iterator] = function() return BindingHelpers.ArrayAccessor:getArrayIterator(self) end
        self.getIndex = self.accessor.getIndex
        self.setIndex = self.accessor.setIndex
        self.push = self.accessor.push
        self.pop = self.accessor.pop
        self.insert = self.accessor.insert
    end
    __TS__SetDescriptor(
        ArrayAccessor.prototype,
        "length",
        {get = function(self)
            return self.accessor:length()
        end},
        true
    )
    ArrayAccessor.getArrayIterator = __TS__Generator(function(self, array)
        do
            local i = 0
            while i < array.length do
                coroutine.yield(array:getIndex(i))
                i = i + 1
            end
        end
    end)
    function ArrayAccessor.prototype.indexOf(self, callback)
        local i = 0
        for ____, value in __TS__Iterator(self) do
            if callback(nil, value, i, self) then
                break
            end
            i = i + 1
        end
        return i
    end
    function ArrayAccessor.prototype.find(self, callback)
        return self.accessor:getIndex(self:indexOf(callback))
    end
    function BindingHelpers.bindArrayWrapper(self, source)
        return __TS__New(BindingHelpers.ArrayAccessor, source)
    end
    local InitialEmptyConstructor = __TS__Class()
    InitialEmptyConstructor.name = "InitialEmptyConstructor"
    function InitialEmptyConstructor.prototype.____constructor(self)
    end
    local BindAsSrc = __TS__Class()
    BindAsSrc.name = "BindAsSrc"
    function BindAsSrc.prototype.____constructor(self, source)
        self.source = source
    end
    BindingHelpers.APIClass = __TS__Class()
    local APIClass = BindingHelpers.APIClass
    APIClass.name = "APIClass"
    function APIClass.prototype.____constructor(self)
    end
    ---
    -- @param className The name of the generated class
    -- @param constructor The function which generates the DataWrapper to bind as the class instance
    -- @param staticFields Static fields which are bound to the actual class object.
    -- @returns An object containing the generatedClass and a bindAs function which can be used by the
    -- C++ calling function to bind an object as an instance of the class.
    function BindingHelpers.makeClass(self, className, constructor, staticFields)
        local classContainer
        local ____className_1 = className
        local ____class_0 = __TS__Class()
        ____class_0.name = ____class_0.name
        __TS__ClassExtends(____class_0, BindingHelpers.APIClass)
        function ____class_0.prototype.____constructor(self, ...)
            local params = {...}
            ____class_0.____super.prototype.____constructor(self)
            if __TS__InstanceOf(params[1], InitialEmptyConstructor) then
                return
            end
            local constructorResult
            if __TS__InstanceOf(params[1], BindAsSrc) then
                constructorResult = params[1].source
            else
                constructorResult = constructor(
                    nil,
                    __TS__Unpack(params)
                )
            end
            local constructed = __TS__New(
                classContainer[className],
                __TS__New(InitialEmptyConstructor)
            )
            for ____, ____value in ipairs(__TS__ObjectEntries(constructorResult)) do
                local key = ____value[1]
                local value = ____value[2]
                BindingHelpers.bindWrapper(nil, key, value, constructed)
            end
            return constructed
        end
        classContainer = {[____className_1] = ____class_0}
        for ____, ____value in ipairs(__TS__ObjectEntries(staticFields)) do
            local key = ____value[1]
            local value = ____value[2]
            BindingHelpers.bindWrapper(nil, key, value, classContainer[className])
        end
        local generatedClass = classContainer[className]
        local generatedBindAsClass = generatedClass
        return {
            generatedClass = generatedClass,
            bindAs = function(____, sourceObject) return __TS__New(
                generatedBindAsClass,
                __TS__New(BindAsSrc, sourceObject)
            ) end
        }
    end
end
____exports.default = BindingHelpers
return ____exports
