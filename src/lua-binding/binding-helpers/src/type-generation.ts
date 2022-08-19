import {
	bindingHelpersModuleName,
	moduleResolutionPath
} from 'binding-helpers';
import BindingHelpers from 'binding-helpers';

module TypeGeneration {
	type DataPrimitive =
		| 'string'
		| 'int32'
		| 'null'
		| 'object'
		| 'function'
		| 'array'
		| 'classType'
		| 'classInstance'
		| 'unknown';

	export type TypeGenerationInterface = {
		dataPrimitive: DataPrimitive;
		objectFields?: { [fieldName: string]: TypeGenerationInterface };
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
				dataPrimitive: 'classInstance';
			};
			classStaticType: TypeGenerationInterface & {
				dataPrimitive: 'object';
			};
			constructor: TypeGenerationInterface & {
				dataPrimitive: 'function';
			};
		};
	};

	type Dependencies<S extends Dependencies<{}>> = Record<
		string,
		DeclaredTyping<S>
	>;
	interface DeclaredTyping<D extends Dependencies<any>> {
		typeName: string;
		typeDef: TypeGenerationInterface;
		typeString: (
			name: string,
			typeString: string,
			dependencies: {
				[K in keyof D]: { dependency: D[K]; compiledType: string };
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
		let classDefinition = classDef.classDefinition;
		if (!classDefinition) {
			return undefined;
		}

		const instance = generateType(classDefinition.classInstanceType);
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
			dependentTypes: {
				classInstanceType: instance,
				classStaticType: staticType,
				constructorType: generateType(classDefinition.constructor)
			}
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
			typeString: (name, typeString, dependencies) =>
				`const ${name}: ${dependencies.typing.compiledType} ;`,
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
		types: DeclaredTyping<Record<string, DeclaredTyping<any>>>[]
	): DeclaredTyping<Record<string, DeclaredTyping<any>>>[] {
		const allTypes: DeclaredTyping<any>[] = [...types];

		for (const typing of types) {
			allTypes.push(
				...(extractAllTypes(
					Object.values(typing.dependentTypes).filter((v) => v)
				) as DeclaredTyping<any>[])
			);
		}

		return allTypes;
	}

	export function generateTypeFiles(
		moduleName: string,
		types: DeclaredTyping<any>[]
	) {
		const allTypes: (DeclaredTyping<Record<string, DeclaredTyping<any>>> & {
			__generatedType?: string;
		})[] = [...extractAllTypes(types)];

		const typeEntries: string[] = [];

		for (const t of allTypes) {
			const dependencies: [string, typeof allTypes[number]][] =
				Object.entries(t.dependentTypes).filter(([k, v]) => v);
			const unresolvedDependencies = dependencies.filter(
				([k, v]) => v.__generatedType === undefined
			);
			const resolvedDependencies = dependencies.filter(
				([k, v]) => v.__generatedType !== undefined
			);
			if (unresolvedDependencies.length === 0) {
				const dependencyMaps = Object.fromEntries(
					resolvedDependencies.map(([k, v]) => [
						k,
						{
							compiledType: v.__generatedType!,
							dependency: v
						}
					])
				);
				const typeString = t.typeString(
					t.typeName,
					'aaaabbbbccccddddd',
					dependencyMaps
				);
				if (t.typeDef.dataPrimitive === 'classType') {
					t.__generatedType = `typeof ${t.typeName}`;
				} else if (t.isLiteralType) {
					t.__generatedType = typeString;
				} else {
					t.__generatedType = t.typeName;
				}

				if (!t.isLiteralType) {
					typeEntries.push(typeString);
				}
			} else {
				allTypes.push(t);
			}
		}

		const typingFile = `import type ${bindingHelpersModuleName} from '${moduleResolutionPath}';\ndeclare module ${moduleName} {\n${typeEntries
			.map((entry) => `\n\n\texport ${entry}`)
			.join('\n')}\n};\nexport default ${moduleName};`;

		return typingFile;
	}

	export function generateType(typingParams: TypeGenerationInterface) {
		if (!typingParams) {
			typingParams = { dataPrimitive: 'unknown' };
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
				console.log('object: ' + typeof typingParams.objectFields);
				return {
					typeName: typingParams.dataPrimitive,
					typeDef: typingParams,
					typeString: (name, typeString, dependencies) => {
						let fieldTypesString = '';
						let objectFields: Exclude<
							TypeGenerationInterface['objectFields'],
							undefined
						> = {};
						if (typingParams.objectFields) {
							objectFields = typingParams.objectFields;
						}

						for (const [fieldName, field] of Object.entries(
							objectFields
						)) {
							fieldTypesString += `"${fieldName}": ( ${dependencies[fieldName].compiledType} ); `;
						}
						return `{ ${fieldTypesString} }`;
					},
					dependentTypes: Object.fromEntries(
						Object.entries(
							typingParams.objectFields
								? typingParams.objectFields
								: {}
						).map(([fieldName, field]) => [
							fieldName,
							generateType(field)
						])
					),
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
						: generateType({ dataPrimitive: 'unknown' })
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
				) ??
				generateType({ ...typingParams, dataPrimitive: 'unknown' }),
			classInstance: () => {
				console.log('classInstance: ' + typingParams.dataPrimitive);
				return generateType({
					...typingParams,
					dataPrimitive: 'object'
				});
			}
		};

		const mapped = mappers[typingParams.dataPrimitive]();

		return mapped;
	}
}

export default TypeGeneration;
