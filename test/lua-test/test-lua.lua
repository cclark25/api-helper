firstRun = {
    testObjectType = "userdata";
    CustomObjectDataType = "table";
    testObjectO1Type = "userdata";

    i1 = 15;
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

function testField(runData, parentObject, secondaryRunData, key)
    print("runData[" .. key .. "] = " .. tostring(runData[key]) .. "\tparentObject[" .. key .. "] = " .. tostring(parentObject[key]) .. "\t new value = " .. tostring(secondaryRunData[key]));	
    assert(type(runData[key]) == type(parentObject[key]));
    assert(runData[key] == parentObject[key]);
    print("runData[" .. key .. "] = " .. tostring(runData[key]) .. "\tparentObject[" .. key .. "] = " .. tostring(parentObject[key]) .. "\t new value = " .. tostring(secondaryRunData[key]));	
    parentObject[key] = secondaryRunData[key];
    
end

function test()
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
    assert(runData.CustomObjectDataType == type(CustomObjectData));

    testField(runData, testObject, newRunData, "i1");
    testField(runData, testObject, newRunData, "s1");

    testField(runData, CustomObjectData, newRunData, "d1");

    assert(testObject:doStuff(0,"abc") == "abc");
    assert(testObject:doStuff(1,"abc") == "abc\nabc");


    assert(CustomObjectData.staticFunction(12) == "24");
    assert(CustomObjectData.staticFunction(25) == "50");


    assert(runData.testObjectO1Type == type(testObject.o1));

    testField(runData.o1Sub, testObject.o1, newRunData.o1Sub, "i2");
    testField(runData.o1Sub, testObject.o1, newRunData.o1Sub, "s2");

    testField(runData.o2Sub, testObject.o2, newRunData.o2Sub, "i2");
    testField(runData.o2Sub, testObject.o2, newRunData.o2Sub, "s2");

    testField(runData.o3Sub, testObject.o3, newRunData.o3Sub, "i2");
    testField(runData.o3Sub, testObject.o3, newRunData.o3Sub, "s2");

    print("Test completed successfully.");
end

test();

