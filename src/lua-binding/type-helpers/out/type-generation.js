"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const binding_helpers_1 = require("binding-helpers/src/binding-helpers");
const binding_helpers_2 = require("binding-helpers/src/binding-helpers");
const dom = require("dts-dom");
const data_primitive_dom_1 = require("./data-primitive-dom");
const fs = require("fs");
var TypeGeneration;
(function (TypeGeneration) {
    function declareType(typeName, typeDef, dependentTypes) {
        const result = {
            typeName,
            typeDef,
            typeString: (name, typeString, dependencies) => `type ${name} = ${dependencies.typing.compiledType}; `,
            dependentTypes
        };
        return result;
    }
    TypeGeneration.declareType = declareType;
    function declareClass(classDef) {
        if (classDef.classDefinition?.__declared) {
            return classDef.classDefinition.__declared;
        }
        let classDefinition = classDef.classDefinition;
        if (!classDefinition) {
            return undefined;
        }
        const staticType = generateType(classDefinition.classStaticType);
        const result = {
            typeName: classDefinition.className,
            typeDef: classDef,
            typeString: (name, typeString, dependencies) => {
                return `class ${name} extends ${`( ${binding_helpers_1.bindingHelpersModuleName}.${binding_helpers_2.default.APIClass.name} as { new(): ( ${dependencies.classInstanceType.compiledType} ) } & ${dependencies.classStaticType.compiledType} )`} { ${dependencies.constructorType.compiledType}; }`;
            },
            dependentTypes: {}
        };
        classDef.classDefinition.__declared = result;
        const instance = generateType(classDefinition.classInstanceType);
        result.dependentTypes = {
            classInstanceType: instance,
            classStaticType: staticType,
            constructorType: generateType(classDefinition.constructor)
        };
        return result;
    }
    TypeGeneration.declareClass = declareClass;
    function declareConst(name, def) {
        const result = {
            typeName: name,
            typeDef: def,
            typeString: (name, typeString, dependencies) => {
                console.log('CONST: ' + dependencies.typing.instanceOfType);
                return `const ${name}: ${dependencies.typing.instanceOfType ??
                    dependencies.typing.compiledType} ;`;
            },
            dependentTypes: {
                typing: generateType(def)
            }
        };
        return result;
    }
    TypeGeneration.declareConst = declareConst;
    function declare(name, def) {
        switch (def.dataPrimitive) {
            case 'classType':
                const createdClass = declareClass(def);
                return createdClass;
            default:
                return declareConst(name, def);
        }
    }
    TypeGeneration.declare = declare;
    function extractAllTypes(types, skipTypes = []) {
        const allTypes = [...types];
        for (const typing of types) {
            if (typing.alreadyDone) {
                continue;
            }
            allTypes.push(...extractAllTypes(Object.values(typing.dependentTypes).filter((v) => v && !allTypes.includes(v) && !skipTypes.includes(v)), allTypes.concat(skipTypes)));
            typing.alreadyDone = true;
        }
        return allTypes;
    }
    TypeGeneration.extractAllTypes = extractAllTypes;
    function generateTypeFiles(moduleName, types) {
        const allTypes = [...extractAllTypes(types)];
        const typeEntries = [];
        for (const t of allTypes) {
            const dependencies = Object.entries(t.dependentTypes).filter(([k, v]) => v);
            const instanceOfDependency = t.typeDef.objectDefinition?.instanceOf
                ? dependencies.find((d) => d[1].typeDef ===
                    t.typeDef.objectDefinition?.instanceOf)
                : undefined;
            const unresolvedDependencies = dependencies.filter(([k, v]) => v.__generatedType === undefined &&
                (instanceOfDependency
                    ? v !== instanceOfDependency[1]
                    : true));
            const resolvedDependencies = dependencies.filter(([k, v]) => v.__generatedType !== undefined);
            if (unresolvedDependencies.length === 0) {
                const dependencyMaps = Object.fromEntries(resolvedDependencies.map(([k, v]) => [
                    k,
                    {
                        compiledType: v.__generatedType.literal,
                        dependency: v
                    }
                ]));
                // if (t.typeDef.objectDefinition?.instanceOf) {
                // 	console.log(dump(dependencyMaps));
                // 	console.log(dump(instanceOfDependency));
                // }
                const typeString = t.typeString(t.typeName, 'aaaabbbbccccddddd', dependencyMaps);
                if (t.typeDef.dataPrimitive === 'classType') {
                    t.__generatedType = { literal: `typeof ${t.typeName}` };
                }
                else if (t.isLiteralType) {
                    t.__generatedType = { literal: typeString };
                }
                else {
                    t.__generatedType = { literal: t.typeName };
                }
                if (!t.isLiteralType) {
                    typeEntries.push(typeString);
                }
            }
            else {
                console.log(`here: ${unresolvedDependencies
                    .map((u) => u[0])
                    .join(', ')}`);
                allTypes.push(t);
            }
        }
        const typingFile = `import type ${binding_helpers_1.bindingHelpersModuleName} from '${binding_helpers_1.moduleResolutionPath}';\ndeclare module ${moduleName} {\n${typeEntries
            .map((entry) => `\n\n\texport ${entry}`)
            .join('\n')}\n};\nexport default ${moduleName};`;
        return typingFile;
    }
    TypeGeneration.generateTypeFiles = generateTypeFiles;
    function generateTypeFilesV2(moduleName, types) {
        const module = dom.create.module(moduleName);
        module.members.push(...types.map((t) => data_primitive_dom_1.constMappers[t.dataPrimitive](t)));
        return dom.emit(module);
    }
    TypeGeneration.generateTypeFilesV2 = generateTypeFilesV2;
    function generateType(typingParams) {
        if (!typingParams) {
            typingParams = { name: '', dataPrimitive: 'unknown' };
        }
        const mappers = {
            string: () => ({
                typeName: typingParams.dataPrimitive,
                typeDef: typingParams,
                typeString: () => 'string',
                dependentTypes: {},
                isLiteralType: true
            }),
            int32: () => ({
                typeName: typingParams.dataPrimitive,
                typeDef: typingParams,
                typeString: () => 'number',
                dependentTypes: {},
                isLiteralType: true
            }),
            null: () => ({
                typeName: typingParams.dataPrimitive,
                typeDef: typingParams,
                typeString: () => 'null',
                dependentTypes: {},
                isLiteralType: true
            }),
            unknown: () => ({
                typeName: typingParams.dataPrimitive,
                typeDef: typingParams,
                typeString: () => 'unknown',
                dependentTypes: {},
                isLiteralType: true
            }),
            object: () => {
                return {
                    typeName: typingParams.dataPrimitive,
                    typeDef: typingParams,
                    typeString: (name, typeString, dependencies) => {
                        if (dependencies.instanceof) {
                            return dependencies.instanceof.compiledType;
                        }
                        let fieldTypesString = '';
                        let objectFields = {};
                        if (typingParams.objectDefinition?.objectFields) {
                            objectFields =
                                typingParams.objectDefinition.objectFields;
                        }
                        for (const [fieldName, field] of Object.entries(objectFields)) {
                            fieldTypesString += `"${fieldName}": ( ${dependencies[fieldName].compiledType} ); `;
                        }
                        return `{ ${fieldTypesString} }`;
                    },
                    dependentTypes: {
                        ...(typingParams.objectDefinition?.instanceOf
                            ? {
                                instanceof: typingParams.objectDefinition.instanceOf
                                    .classDefinition?.__declared ??
                                    generateType(typingParams.objectDefinition
                                        .instanceOf)
                            }
                            : {}),
                        ...Object.fromEntries(Object.entries(typingParams.objectDefinition?.objectFields
                            ? typingParams.objectDefinition.objectFields
                            : {}).map(([fieldName, field]) => [
                            fieldName,
                            generateType(field)
                        ]))
                    },
                    isLiteralType: true
                };
            },
            array: () => ({
                typeName: typingParams.dataPrimitive,
                typeDef: typingParams,
                typeString: (name, typeString, dependencies) => `${binding_helpers_1.bindingHelpersModuleName}.${binding_helpers_2.default.ArrayAccessor.name}<${typingParams.arrayOf
                    ? `( ${dependencies.arrayOf.compiledType} )`
                    : 'unknown'}>`,
                dependentTypes: {
                    arrayOf: typingParams.arrayOf
                        ? generateType(typingParams.arrayOf)
                        : generateType({ name: '', dataPrimitive: 'unknown' })
                },
                isLiteralType: true
            }),
            function: () => ({
                typeName: typingParams.dataPrimitive,
                typeDef: typingParams,
                typeString: (name, typeString, dependencies) => {
                    if (!typingParams.functionDefinition) {
                        return '( ()=>unknown )';
                    }
                    const parameterString = `( ${typingParams.functionDefinition.parameters
                        .map((p) => `${p.parameterName}: ( ${dependencies[`parameter-${p.parameterName}`]
                        .compiledType} )`)
                        .join(', ')} )`;
                    if (!typingParams.functionDefinition.isConstructor) {
                        return `( ${parameterString} => ( ${dependencies.returnType.compiledType} ) )`;
                    }
                    else {
                        456;
                        return `constructor${parameterString}`;
                    }
                },
                dependentTypes: {
                    ...(typingParams.functionDefinition
                        ? Object.fromEntries(typingParams.functionDefinition.parameters.map((p) => [
                            `parameter-${p.parameterName}`,
                            generateType(p.type)
                        ]))
                        : {}),
                    returnType: typingParams.functionDefinition
                        ? generateType(typingParams.functionDefinition.returnType)
                        : undefined
                },
                isLiteralType: true
            }),
            classType: () => declareClass(typingParams) ?? generateType({ ...typingParams, dataPrimitive: 'unknown' })
        };
        const mapped = mappers[typingParams.dataPrimitive]();
        return mapped;
    }
    TypeGeneration.generateType = generateType;
})(TypeGeneration || (TypeGeneration = {}));
exports.default = TypeGeneration;
const types = TypeGeneration.generateTypeFilesV2('API', JSON.parse(fs
    .readFileSync('/home/christopher/GIT/api-helper/dist/test.json')
    .toString()));
console.log(types);
//# sourceMappingURL=type-generation.js.map