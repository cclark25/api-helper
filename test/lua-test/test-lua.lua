firstRun = {
    testObjectType = "userdata";
    CustomObjectDataType = "table";
    testObjectO1Type = "userdata";

    i1 = 15;
    secondI = 12;
    s1 = "ABC123";
    d1 = 12.678;

    o1Sub = {
        i2 = 30;
        s2 = "DEF456"
    };
    o2Sub = {
        i2 = 30;
        s2 = "DEF456"
    };
    o3Sub = {
        i2 = 30;
        s2 = "DEF456"
    };
}
secondRun = {
    testObjectType = "userdata";
    CustomObjectDataType = "table";
    testObjectO1Type = "userdata";

    i1 = 30;
    secondI = 17;
    s1 = "AAABBB";
    d1 = 37890.3435;

    o1Sub = {
        i2 = 60;
        s2 = "XXYXXYZZ"
    };

    o2Sub = {
        i2 = 14;
        s2 = "o2s2v2"
    };

    o3Sub = {
        i2 = 869;
        s2 = "o3s2v2"
    };
}


function loopFunctionPointerTest(testObject)
    j = 1;
    while true do
        local oldFuncPtr = testObject.functionPointer;
        -- invokesPerSecond1 = testObject:testPassedFunction(1000);
        -- print('invokesPerSecond: ' .. invokesPerSecond1)
        testObject.functionPointer = function (i)
            -- print('Overwritten function called.')
            do
                i = 0
                while i < j do
                    i = i + 1;
                end
            end
            return 1;
            
        end
        invokesPerSecond2 = testObject:testPassedFunction(10);
        if j % 2 == 0 then
            print('invokesPerSecond (#' .. j .. '): ' .. invokesPerSecond2)
        end
        -- print('invokesPerSecond difference: ' .. invokesPerSecond2 / invokesPerSecond1)
        testObject.functionPointer = oldFuncPtr;
        if j < 524288 then
            j = j * 2;
        else
            break;
        end
    end
end

function testField(runData, parentObject, secondaryRunData, key)
    print("runData[" .. key .. "] = " .. tostring(runData[key]) .. "\tparentObject[" .. key .. "] = " .. tostring(parentObject[key]) .. "\t new value = " .. tostring(secondaryRunData[key]));	
    assert(type(runData[key]) == type(parentObject[key]));
    assert(runData[key] == parentObject[key]);
    print("runData[" .. key .. "] = " .. tostring(runData[key]) .. "\tparentObject[" .. key .. "] = " .. tostring(parentObject[key]) .. "\t new value = " .. tostring(secondaryRunData[key]));	
    if secondaryRunData[key] ~= nil then 
        parentObject[key] = secondaryRunData[key];
    end
end

function staticTest()
    runData = firstRun;
    newRunData = secondRun;
    if(runNum == 2) then
        tmp1 = runData;
        tmp2 = newRunData
        runData = tmp2;
        newRunData = tmp1;
    end
    assert(CustomObjectData.staticFunction(12) == "24");
    assert(CustomObjectData.staticFunction(25) == "50");
    testField(runData, CustomObjectData, newRunData, "d1");
    assert(runData.CustomObjectDataType == type(CustomObjectData));

    promise = CustomObjectData.staticAsync(1);
    
    promise:onResolve(
        function(result)
            print('Promise return value in callback: ' .. (result));
        end
    ):onResolve(
        function()
            print('Callback on the callback was called.');
        end
    ):await();
    promise = CustomObjectData.staticAsync(1);
    print('Promise return type: ' .. type(promise));
    print('Promise return value in await: ' .. (promise:await()));
    
    assert(CustomObjectData.staticFunctionPointer(1) == 12)
    oldFuncPtr = CustomObjectData.staticFunctionPointer;
    CustomObjectData.staticFunctionPointer = function (i)
        return i * 13;
    end
    assert(CustomObjectData.staticFunctionPointer(1) == 13)
    CustomObjectData.staticFunctionPointer = oldFuncPtr

    assert(CustomObjectData.staticOverload(1) == "1");
    assert(CustomObjectData.staticOverload("1") == "2");
    assert(CustomObjectData2.staticOverload(1) == "1");
    assert(CustomObjectData2.staticOverload("1") == "2");
    assert(CustomObjectData2.staticOverload("1", 1) == "3");
end

function test(testObject)
    runData = firstRun;
    newRunData = secondRun;

    print("runNum = " .. runNum);

    if(runNum == 2) then
        tmp1 = runData;
        tmp2 = newRunData
        runData = tmp2;
        newRunData = tmp1;
    end

    assert(runData.testObjectType == type(testObject));

    testField(runData, testObject, newRunData, "i1");

    if (testObject.getType() == CustomObjectData2) then
        testField(runData, testObject, newRunData, "secondI");
    end

    testField(runData, testObject, newRunData, "s1");


    assert(testObject:doStuff(0) == "NO STRING PROVIDED" );
    assert(testObject:doStuff(1) == "NO STRING PROVIDED\nNO STRING PROVIDED" );

    assert(testObject:doStuff(0,"abc") == "abc" );

    if(testObject:getType() == CustomObjectData) then
        assert(testObject:doStuff(1,"abc") == "abc\nabc");
    elseif(testObject:getType() == CustomObjectData2) then 
        assert(testObject:doStuff("abc") == "abc" );
        assert(testObject:doStuff(1,"abc") == "abc\tabc");
    else
        assert(false);
    end

    assert(testObject:doStuff(1,"abc") == "abc\nabc" or testObject:doStuff(1,"abc") == "abc\tabc");
    
    loopFunctionPointerTest(testObject);

    promise = testObject:memberAsync(1);
    print('Member promise return type: ' .. type(promise));
    print('Member promise return value: ' .. (promise:await()));

    assert(runData.testObjectO1Type == type(testObject.o1));

    testField(runData.o1Sub, testObject.o1, newRunData.o1Sub, "i2");
    testField(runData.o1Sub, testObject.o1, newRunData.o1Sub, "s2");

    testField(runData.o2Sub, testObject.o2, newRunData.o2Sub, "i2");
    testField(runData.o2Sub, testObject.o2, newRunData.o2Sub, "s2");

    testField(runData.o3Sub, testObject.o3, newRunData.o3Sub, "i2");
    testField(runData.o3Sub, testObject.o3, newRunData.o3Sub, "s2");

    print("Test completed successfully.");
end


staticTest();
test(testObject);

if(runNum == 1) then
    newObj = CustomObjectData.new(12);
    test(newObj);
    newObj2 = CustomObjectData.new("12");
    test(newObj2);

    newObj4 = CustomObjectData2.new(12.123);
    test(newObj4);

    -- TODO: Make the metatable callable to act as constructors.
    -- newObj3 = CustomObjectData.__call({}, 12)
    -- print(type(getmetatable(CustomObjectData)["test"] ));
end
