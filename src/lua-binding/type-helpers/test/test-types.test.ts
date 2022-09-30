import { exec } from 'child_process';
import * as fs from 'fs';
import TypeGeneration from '../src/type-generation';

const classDefinition: TypeGeneration.TypeGenerationInterface = {
	dataPrimitive: 'classType',
	classDefinition: {
		className: 'TestClass',
		classInstanceType: {
			dataPrimitive: 'classInstance',
			objectFields: {
				abc: { dataPrimitive: 'int32' },
				def: { dataPrimitive: 'string' }
			}
		},
		constructor: {
			dataPrimitive: 'function',
			functionDefinition: {
				parameters: [
					{
						parameterName: 'p1',
						type: { dataPrimitive: 'int32' }
					},
					{
						parameterName: 'p2',
						type: { dataPrimitive: 'string' }
					}
				],
				returnType: { dataPrimitive: 'unknown' },
				isConstructor: true
			}
		}
	}
};

const api: TypeGeneration.TypeGenerationInterface = {
	dataPrimitive: 'object',
	objectFields: {
		f1: { dataPrimitive: 'string', objectFields: {} },
		f2: { dataPrimitive: 'int32', objectFields: {} },
		f3: {
			dataPrimitive: 'object',
			objectFields: {
				abc: {
					dataPrimitive: 'int32',
					objectFields: {}
				},
				def: {
					dataPrimitive: 'string',
					objectFields: {}
				}
			}
		},
		f4: {
			dataPrimitive: 'array',
			arrayOf: {
				dataPrimitive: 'object',
				objectFields: {
					a: { dataPrimitive: 'array' },
					b: {
						dataPrimitive: 'array',
						arrayOf: { dataPrimitive: 'string' }
					}
				}
			}
		},
		f5: {
			dataPrimitive: 'function'
		},
		f6: {
			dataPrimitive: 'function',
			functionDefinition: {
				parameters: [
					{
						parameterName: 'param1',
						type: { dataPrimitive: 'string' }
					},
					{
						parameterName: 'param2',
						type: {
							dataPrimitive: 'object',
							objectFields: {
								f1: { dataPrimitive: 'string' },
								f2: { dataPrimitive: 'int32' }
							}
						}
					}
				],
				returnType: {
					dataPrimitive: 'object',
					objectFields: { abc: { dataPrimitive: 'int32' } }
				}
			}
		},
		TestClass: classDefinition
	}
};

const newAPI = TypeGeneration.declareClass({
	dataPrimitive: 'classType',
	classDefinition: {
		className: 'APIState',
		classInstanceType: { ...api, dataPrimitive: 'classInstance' },
		constructor: {
			dataPrimitive: 'function',
			functionDefinition: {
				parameters: [],
				returnType: { dataPrimitive: 'object' },
				isConstructor: true
			}
		}
	}
});

const typeFileString = TypeGeneration.generateTypeFiles('TestModule', [
	newAPI!
]);

const path = fs.realpathSync(process.argv[1]).split('/').slice(0, -1);
const testFilePath = path.concat(['temp', 'testFile.ts']).join('/');

fs.writeFileSync(testFilePath, typeFileString);

exec(`ts-node ${testFilePath} --cwd ${path.join('/')}/..`, (e, out, err) => {
	console.log(out);
	console.error(err);
});
