import {
	bindingHelpersModuleName,
	moduleResolutionPath
} from 'binding-helpers/src/binding-helpers';
import BindingHelpers from 'binding-helpers/src/binding-helpers';
import * as dom from 'dts-dom';
import {
	constMappers,
	declaredTypes,
	f,
	objectFieldMappers
} from './data-primitive-dom';
import * as fs from 'fs';

module TypeGeneration {
	export type DataPrimitive =
		| 'string'
		| 'int32'
		| 'null'
		| 'object'
		| 'function'
		| 'array'
		| 'classType'
		| 'unknown';

	export type TypeGenerationInterface = {
		name: string;
		dataPrimitive: DataPrimitive;
		flags?: dom.DeclarationFlags;
		objectDefinition?: {
			instanceOf?: TypeGenerationInterface & {
				dataPrimitive: 'classType';
			};
			objectFields: {
				[fieldName: string]: TypeGenerationInterface;
			};
		};
		arrayOf?: TypeGenerationInterface;
		functionDefinition?: {
			parameters: {
				parameterName: string;
				type: TypeGenerationInterface;
			}[];
			returnType: TypeGenerationInterface;
			isConstructor?: boolean;
		};
		classDefinition?: {
			className: string;
			classInstanceType: TypeGenerationInterface & {
				dataPrimitive: 'object';
			};
			classStaticType: TypeGenerationInterface & {
				dataPrimitive: 'object';
			};
			constructor: TypeGenerationInterface & {
				dataPrimitive: 'function';
			};

			__declared?: DeclaredTyping<any>;
		};
	};

	type Dependencies<S extends Dependencies<{}>> = Record<
		string,
		DeclaredTyping<S>
	>;
	interface CompiledDependency {
		dependency: DeclaredTyping<any>;
		compiledType: string;
		instanceOfType?: string;
	}

	interface DeclaredTyping<D extends Dependencies<any>> {
		typeName: string;
		typeDef: TypeGenerationInterface;
		typeString: (
			name: string,
			typeString: string,
			dependencies: {
				[K in keyof D]: CompiledDependency;
			}
		) => string;
		dependentTypes: D;
		isLiteralType?: boolean;
	}

	export function declareType<
		D extends Dependencies<{ typing: DeclaredTyping<any> }>
	>(
		typeName: string,
		typeDef: TypeGenerationInterface,
		dependentTypes: D
	): DeclaredTyping<D> {
		const result: DeclaredTyping<D> = {
			typeName,
			typeDef,
			typeString: (name, typeString, dependencies) =>
				`type ${name} = ${dependencies.typing.compiledType}; `,
			dependentTypes
		};

		return result;
	}

	export function declareClass(
		classDef: TypeGenerationInterface & { dataPrimitive: 'classType' }
	):
		| DeclaredTyping<{
				classInstanceType: DeclaredTyping<any>;
				classStaticType: DeclaredTyping<any>;
				constructorType: DeclaredTyping<any>;
		  }>
		| undefined {
		if (classDef.classDefinition?.__declared) {
			return classDef.classDefinition.__declared;
		}
		let classDefinition = classDef.classDefinition;
		if (!classDefinition) {
			return undefined;
		}

		const staticType = generateType(classDefinition.classStaticType);

		const result: DeclaredTyping<{
			classInstanceType: DeclaredTyping<any>;
			classStaticType: DeclaredTyping<any>;
			constructorType: DeclaredTyping<any>;
		}> = {
			typeName: classDefinition.className,
			typeDef: classDef,
			typeString: (name, typeString, dependencies) => {
				return `class ${name} extends ${`( ${bindingHelpersModuleName}.${BindingHelpers.APIClass.name} as { new(): ( ${dependencies.classInstanceType.compiledType} ) } & ${dependencies.classStaticType.compiledType} )`} { ${
					dependencies.constructorType.compiledType
				}; }`;
			},

			dependentTypes: {} as any
		};

		classDef.classDefinition!.__declared = result;
		const instance = generateType(classDefinition.classInstanceType);
		result.dependentTypes = {
			classInstanceType: instance,
			classStaticType: staticType,
			constructorType: generateType(classDefinition.constructor)
		};
		return result;
	}

	export function declareConst(
		name: string,
		def: TypeGenerationInterface
	): DeclaredTyping<Record<string, DeclaredTyping<any>>> {
		const result: DeclaredTyping<Record<string, DeclaredTyping<any>>> = {
			typeName: name,
			typeDef: def,
			typeString: (name, typeString, dependencies) => {
				console.log('CONST: ' + dependencies.typing.instanceOfType);
				return `const ${name}: ${
					dependencies.typing.instanceOfType ??
					dependencies.typing.compiledType
				} ;`;
			},
			dependentTypes: {
				typing: generateType(def)
			}
		};

		return result;
	}

	export function declare(name: string, def: TypeGenerationInterface) {
		switch (def.dataPrimitive) {
			case 'classType':
				const createdClass = declareClass(
					def as TypeGenerationInterface & {
						dataPrimitive: 'classType';
					}
				);

				return createdClass;
			default:
				return declareConst(name, def);
		}
	}

	export function extractAllTypes(
		types: (DeclaredTyping<Record<string, DeclaredTyping<any>>> & {
			alreadyDone?: true;
		})[],
		skipTypes: DeclaredTyping<any>[] = []
	): DeclaredTyping<Record<string, DeclaredTyping<any>>>[] {
		const allTypes: DeclaredTyping<any>[] = [...types];

		for (const typing of types) {
			if (typing.alreadyDone) {
				continue;
			}

			allTypes.push(
				...(extractAllTypes(
					Object.values(typing.dependentTypes).filter(
						(v) =>
							v && !allTypes.includes(v) && !skipTypes.includes(v)
					),
					allTypes.concat(skipTypes)
				) as DeclaredTyping<any>[])
			);

			typing.alreadyDone = true;
		}

		return allTypes;
	}

	export function generateTypeFiles(
		moduleName: string,
		types: DeclaredTyping<any>[]
	) {
		const allTypes: (DeclaredTyping<Record<string, DeclaredTyping<any>>> & {
			__generatedType?: { literal: string; instanceOf?: string };
		})[] = [...extractAllTypes(types)];

		const typeEntries: string[] = [];

		for (const t of allTypes) {
			const dependencies: [string, typeof allTypes[number]][] =
				Object.entries(t.dependentTypes).filter(([k, v]) => v);

			const instanceOfDependency = t.typeDef.objectDefinition?.instanceOf
				? dependencies.find(
						(d) =>
							d[1].typeDef ===
							t.typeDef.objectDefinition?.instanceOf
				  )
				: undefined;

			const unresolvedDependencies = dependencies.filter(
				([k, v]) =>
					v.__generatedType === undefined &&
					(instanceOfDependency
						? v !== instanceOfDependency[1]
						: true)
			);
			const resolvedDependencies = dependencies.filter(
				([k, v]) => v.__generatedType !== undefined
			);
			if (unresolvedDependencies.length === 0) {
				const dependencyMaps = Object.fromEntries(
					resolvedDependencies.map(([k, v]) => [
						k,
						{
							compiledType: v.__generatedType!.literal,
							dependency: v
						} as CompiledDependency
					])
				);
				// if (t.typeDef.objectDefinition?.instanceOf) {
				// 	console.log(dump(dependencyMaps));
				// 	console.log(dump(instanceOfDependency));
				// }
				const typeString = t.typeString(
					t.typeName,
					'aaaabbbbccccddddd',
					dependencyMaps
				);

				if (t.typeDef.dataPrimitive === 'classType') {
					t.__generatedType = { literal: `typeof ${t.typeName}` };
				} else if (t.isLiteralType) {
					t.__generatedType = { literal: typeString };
				} else {
					t.__generatedType = { literal: t.typeName };
				}

				if (!t.isLiteralType) {
					typeEntries.push(typeString);
				}
			} else {
				console.log(
					`here: ${unresolvedDependencies
						.map((u) => u[0])
						.join(', ')}`
				);
				allTypes.push(t);
			}
		}

		const typingFile = `import type ${bindingHelpersModuleName} from '${moduleResolutionPath}';\ndeclare module ${moduleName} {\n${typeEntries
			.map((entry) => `\n\n\texport ${entry}`)
			.join('\n')}\n};\nexport default ${moduleName};`;

		return typingFile;
	}

	export function generateTypeFilesV2(
		moduleName: string,
		types: TypeGenerationInterface[]
	) {
		const module = dom.create.module(moduleName);

		types.forEach((t) => f(t, true));
		module.members.push(...declaredTypes);

		return dom.emit(module);
	}

	export function generateType(typingParams: TypeGenerationInterface) {
		if (!typingParams) {
			typingParams = { name: '', dataPrimitive: 'unknown' };
		}
		const mappers: Record<
			typeof typingParams['dataPrimitive'],
			() => DeclaredTyping<any>
		> = {
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
						let objectFields: Exclude<
							TypeGenerationInterface['objectDefinition'],
							undefined
						>['objectFields'] = {};
						if (typingParams.objectDefinition?.objectFields) {
							objectFields =
								typingParams.objectDefinition.objectFields;
						}

						for (const [fieldName, field] of Object.entries(
							objectFields
						)) {
							fieldTypesString += `"${fieldName}": ( ${dependencies[fieldName].compiledType} ); `;
						}
						return `{ ${fieldTypesString} }`;
					},
					dependentTypes: {
						...(typingParams.objectDefinition?.instanceOf
							? {
									instanceof:
										typingParams.objectDefinition.instanceOf
											.classDefinition?.__declared ??
										generateType(
											typingParams.objectDefinition
												.instanceOf
										)
							  }
							: {}),
						...Object.fromEntries(
							Object.entries(
								typingParams.objectDefinition?.objectFields
									? typingParams.objectDefinition.objectFields
									: {}
							).map(([fieldName, field]) => [
								fieldName,
								generateType(field)
							])
						)
					},
					isLiteralType: true
				};
			},

			array: () => ({
				typeName: typingParams.dataPrimitive,
				typeDef: typingParams,
				typeString: (name, typeString, dependencies) =>
					`${bindingHelpersModuleName}.${
						BindingHelpers.ArrayAccessor.name
					}<${
						typingParams.arrayOf
							? `( ${dependencies.arrayOf.compiledType} )`
							: 'unknown'
					}>`,
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
						.map(
							(p) =>
								`${p.parameterName}: ( ${
									dependencies[`parameter-${p.parameterName}`]
										.compiledType
								} )`
						)
						.join(', ')} )`;
					if (!typingParams.functionDefinition.isConstructor) {
						return `( ${parameterString} => ( ${dependencies.returnType.compiledType} ) )`;
					} else {
						456;
						return `constructor${parameterString}`;
					}
				},
				dependentTypes: {
					...(typingParams.functionDefinition
						? Object.fromEntries(
								typingParams.functionDefinition.parameters.map(
									(p) => [
										`parameter-${p.parameterName}`,
										generateType(p.type)
									]
								)
						  )
						: {}),
					returnType: typingParams.functionDefinition
						? generateType(
								typingParams.functionDefinition.returnType
						  )
						: undefined
				},
				isLiteralType: true
			}),
			classType: () =>
				declareClass(
					typingParams as TypeGenerationInterface & {
						dataPrimitive: 'classType';
					}
				) ?? generateType({ ...typingParams, dataPrimitive: 'unknown' })
		};

		const mapped = mappers[typingParams.dataPrimitive]();

		return mapped;
	}
}

export default TypeGeneration;

const types = TypeGeneration.generateTypeFilesV2(
	'API',
	JSON.parse(
		fs
			.readFileSync('/home/christopher/GIT/api-helper/dist/test.json')
			.toString()
	)
);

console.log(types);
