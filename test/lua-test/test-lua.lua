print("testObject's type: " .. type(testObject));
print("CustomObjectData type: " .. type(CustomObjectData));	
print("testObject.i1 type: " .. type(testObject.i1));	
print("testObject.i1: " .. tostring(testObject.i1));	
print("testObject.s1 type: " .. type(testObject.s1));	
print("testObject.s1: " .. (testObject.s1));		
print("CustomObjectData.d1 type: " .. type(CustomObjectData.d1));	
print("CustomObjectData.d1: " .. tostring(CustomObjectData.d1));
CustomObjectData.d1 = 11.111;	
print("CustomObjectData.d1: " .. tostring(CustomObjectData.d1));
print("testObject.doStuff type: " .. type(testObject.doStuff));	
print("testObject.doStuff function result: " .. (testObject:doStuff(12, "stringParam")));	
print("CustomObjectData.staticFunction type: " .. type(CustomObjectData.staticFunction));	
print("CustomObjectData.staticFunction function result: " .. (CustomObjectData.staticFunction(12)));	

print("testObject.o1 type: " .. type(testObject.o1));	
print("testObject.o1.i2 type: " .. type(testObject.o1.i2));	
print("testObject.o1.i2: " .. tostring(testObject.o1.i2));	
print("testObject.o1.s2 type: " .. type(testObject.o1.s2));
print("testObject.o1.s2: " .. tostring(testObject.o1.s2));

testObject.o1.i2 = 76;
testObject.o1.s2 = "abcd";

print("testObject.o1 type: " .. type(testObject.o1));	
print("testObject.o1.i2 type: " .. type(testObject.o1.i2));	
print("testObject.o1.i2: " .. tostring(testObject.o1.i2));	
print("testObject.o1.s2 type: " .. type(testObject.o1.s2));
print("testObject.o1.s2: " .. tostring(testObject.o1.s2));