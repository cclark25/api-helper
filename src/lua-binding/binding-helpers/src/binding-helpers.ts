/**
 * A wrapper object passed into the runtime to bind APIs.
 */
export type DataWrapper<T> = {
	/**
	 * Getter function passed in from the API.
	 * @returns A value provided by the API.
	 */
	get?: () => T;
	/**
	 * Setter function passed in from the API.
	 * @param value A value to set in the API.
	 */
	set?: (value: T) => void;
};

export type DataWrapperReadOnly<T> = Required<Pick<DataWrapper<T>, 'get'>>;
export type DataWrapperSetOnly<T> = Required<Pick<DataWrapper<T>, 'set'>>;
export type DataWrapperReadSet<T> = Required<DataWrapper<T>>;
export type DataWrapperOptions<T> =
	| DataWrapperReadOnly<T>
	| DataWrapperSetOnly<T>
	| DataWrapperReadSet<T>;

export type BindWrapperTargetResult<
	TargetType extends Object,
	Key extends string,
	Wrapper extends DataWrapper<any>
> = TargetType & {
	[K in Key]: Wrapper extends DataWrapper<infer T> ? T : never;
};

/**
 *
 * @param key Key on destination to bind the wrapper to.
 * @param wrapper Data wrapper provided by the API to bind to the destination.
 * @param destination Destination to bind the wrapper on. Defaults to the module exports.
 */
export function bindWrapper<
	T,
	K extends string,
	TargetType extends Object,
	Wrapper extends DataWrapper<T>
>(key: K, wrapper: DataWrapper<T>, destination: TargetType = module.exports) {
	Object.defineProperty(destination, key, wrapper);
}

export interface ExternalArrayAccessor<T> {
	getIndex: (index: number) => T;
	setIndex: (index: number, value: T) => void;
	push: (value: T) => number;
	pop: () => T;
	insert: (index: number, value: T) => number;
	length: () => number;
}

export class ArrayAccessor<T> {
	public static *getArrayIterator<T>(array: ArrayAccessor<T>) {
		for (let i = 0; i < array.length; i++) {
			yield array.getIndex(i);
		}
	}

	public [Symbol.iterator] = () => ArrayAccessor.getArrayIterator(this);

	public constructor(public accessor: ExternalArrayAccessor<T>) {
		this.getIndex = this.accessor.getIndex;
		this.setIndex = this.accessor.setIndex;
		this.push = this.accessor.push;
		this.pop = this.accessor.pop;
		this.insert = this.accessor.insert;
	}

	public getIndex: (index: number) => T;
	public setIndex: (index: number, value: T) => void;
	public push: (value: T) => number;
	public pop: () => T;
	public insert: (index: number, value: T) => number;

	public get length() {
		return this.accessor.length();
	}

	public indexOf(
		callback: (value: T, index: number, array: ArrayAccessor<T>) => boolean
	) {
		let i = 0;
		for (const value of this) {
			if (callback(value, i, this)) {
				break;
			}
			i++;
		}

		return i;
	}

	public find(
		callback: (value: T, index: number, array: ArrayAccessor<T>) => boolean
	) {
		return this.accessor.getIndex(this.indexOf(callback));
	}
}

export function bindArrayWrapper<T>(source: ExternalArrayAccessor<T>) {
	return new ArrayAccessor<T>(source);
}

type KeysMatching<T, V> = {
	[K in keyof T]-?: T[K] extends V ? K : never;
}[keyof T];

type ReadOnlyMembers<Members extends Record<string, DataWrapper<any>>> =
	Exclude<
		KeysMatching<Members, DataWrapperReadOnly<any>>,
		KeysMatching<Members, DataWrapperSetOnly<any>>
	>;
type SetOnlyMembers<Members extends Record<string, DataWrapper<any>>> = Exclude<
	KeysMatching<Members, DataWrapperSetOnly<any>>,
	KeysMatching<Members, DataWrapperReadOnly<any>>
>;
type SetableMembers<Members extends Record<string, DataWrapper<any>>> =
	KeysMatching<Members, DataWrapperSetOnly<any>>;

export type ClassPrototype<Members extends Record<string, DataWrapper<any>>> = {
	readonly [K in ReadOnlyMembers<Members>]: Members[K] extends DataWrapperReadOnly<
		infer T
	>
		? T
		: never;
} & {
	[K in SetableMembers<Members>]: Members[K] extends DataWrapperSetOnly<
		infer T
	>
		? T
		: never;
};

export type ClassConstructor<
	ConstructorParameters extends unknown[],
	MemberFields extends Record<string, DataWrapper<any>>
> = (...params: ConstructorParameters) => MemberFields;

export type Class<
	ConstructorParameters extends unknown[],
	MemberFields extends Record<string, any>,
	StaticFields extends Record<string, any>,
	StaticFieldNames extends string = StaticFields extends Record<infer N, any>
		? N
		: never
> = {
	new (...params: ConstructorParameters): ClassPrototype<MemberFields>;
} & {
	[K in StaticFieldNames]: BindWrapperTargetResult<{}, K, StaticFields[K]>[K];
};

class InitialEmptyConstructor {}

class BindAsSrc<MemberFields extends Record<string, DataWrapperOptions<any>>> {
	constructor(public source: MemberFields) {}
}

/**
 *
 * @param className The name of the generated class
 * @param constructor The function which generates the DataWrapper to bind as the class instance
 * @param staticFields Static fields which are bound to the actual class object.
 * @returns An object containing the generatedClass and a bindAs function which can be used by the
 * 	C++ calling function to bind an object as an instance of the class.
 */
export function makeClass<
	ConstructorParameters extends unknown[],
	MemberFields extends Record<string, DataWrapperOptions<any>>,
	StaticFields extends Record<string, DataWrapperOptions<any>>,
	ClassName extends string
>(
	className: ClassName,
	constructor: ClassConstructor<ConstructorParameters, MemberFields>,
	staticFields: StaticFields
): {
	generatedClass: Class<ConstructorParameters, MemberFields, StaticFields>;
	bindAs: (sourceObject: MemberFields) => ClassPrototype<MemberFields>;
} {
	/*
		Trick to get the class name to match what was passed into the function.
		This way, `class.name` will return that value.
	*/
	const classContainer = {
		[className]: class {
			constructor(
				...params:
					| ConstructorParameters
					| [InitialEmptyConstructor]
					| [BindAsSrc<MemberFields>]
			) {
				/*
					Trick to create an object that is an instance of the class,
					that way we can bind all of the member fields dynamically
					onto the object. Since the object was created with an actual
					constructor, it is an actual instance of the class, so the
					`instanceof` operator will still work.

					This is a workaround since TSTL does not support `Object.setPrototypeOf`.
				*/
				if (params[0] instanceof InitialEmptyConstructor) {
					return;
				}
				let constructorResult: MemberFields;
				if (params[0] instanceof BindAsSrc) {
					constructorResult = params[0].source;
				} else {
					constructorResult = constructor(
						...(params as ConstructorParameters)
					);
				}

				const constructed = new classContainer[className](
					new InitialEmptyConstructor()
				);

				for (const [key, value] of Object.entries(constructorResult)) {
					bindWrapper(key, value, constructed);
				}

				return constructed as ClassPrototype<MemberFields>;
			}
		}
	};

	for (const [key, value] of Object.entries(staticFields)) {
		bindWrapper(key, value, classContainer[className] as any);
	}

	const generatedClass = classContainer[className] as Class<
		ConstructorParameters,
		MemberFields,
		StaticFields
	>;

	const generatedBindAsClass: {
		new (source: BindAsSrc<MemberFields>): ClassPrototype<MemberFields>;
	} = generatedClass as unknown as any;

	return {
		generatedClass,
		bindAs: (sourceObject: MemberFields) =>
			new generatedBindAsClass(new BindAsSrc<MemberFields>(sourceObject))
	};
}

export type TypeGenerationInterface = {
	dataPrimitive:
		| 'string'
		| 'int32'
		| 'null'
		| 'object'
		| 'function'
		| 'array'
		| 'classType'
		| 'classInstance'
		| 'unknown';
	objectFields?: { [fieldName: string]: TypeGenerationInterface };
	arrayOf?: TypeGenerationInterface;
	functionDefinition?: {
		parameters: { parameterName: string; type: TypeGenerationInterface }[];
		returnType: TypeGenerationInterface;
		isConstructor?: boolean;
	};
	classDefinition?: {
		className: string;
		classInstanceType: TypeGenerationInterface & {
			dataPrimitive: 'classInstance';
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
			classCoreTyping: DeclaredTyping<{
				classInstanceType: DeclaredTyping<Dependencies<{}>>;
			}>;
			constructorType: DeclaredTyping<any>;
	  }>
	| undefined {
	let classDefinition = classDef.classDefinition;
	if (!classDefinition) {
		return undefined;
	}

	const classCoreTypeName = `${classDefinition.className}CoreClassType`;
	// console.log(JSON.stringify(classDefinition.classInstanceType));
	const instance = generateType(classDefinition.classInstanceType);

	let classCoreTyping: DeclaredTyping<{
		classInstanceType: DeclaredTyping<any>;
	}> = {
		typeName: classCoreTypeName,
		typeDef: classDefinition.classInstanceType,
		typeString: (name, typeString, dependencies) =>
			`declare const ${name}: { new(): ( ${dependencies.classInstanceType.compiledType} ) };`,
		dependentTypes: {
			classInstanceType: instance
		}
	};

	const result: DeclaredTyping<{
		classCoreTyping: typeof classCoreTyping;
		constructorType: DeclaredTyping<any>;
	}> = {
		typeName: classDefinition.className,
		typeDef: classDef,
		typeString: (name, typeString, dependencies) =>
			`declare class ${name} extends ${classCoreTyping.typeName} { ${dependencies.constructorType.compiledType}; }`,
		dependentTypes: {
			classCoreTyping,
			constructorType: generateType(classDefinition.constructor)
		}
	};

	return result;
}

export function extractAllTypes(
	types: DeclaredTyping<any>[]
): DeclaredTyping<any>[] {
	const allTypes: DeclaredTyping<any>[] = [...types];

	for (const typing of types) {
		allTypes.push(
			...(extractAllTypes(
				Object.values(typing.dependentTypes) as DeclaredTyping<any>[]
			) as DeclaredTyping<any>[])
		);
	}

	return allTypes;
}

export function generateTypeFiles(types: DeclaredTyping<any>[]) {
	const allTypes: (DeclaredTyping<any> & { __generatedType?: string })[] = [
		...extractAllTypes(types)
	];

	const typeEntries: string[] = [];

	for (const t of allTypes) {
		const dependencies: [string, typeof allTypes[number]][] =
			Object.entries(t.dependentTypes);
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

			t.__generatedType = t.isLiteralType ? typeString : t.typeName;

			if (!t.isLiteralType) {
				typeEntries.push(typeString);
			}
		} else {
			// console.log(
			// 	`Type ${
			// 		t.typeName
			// 	} is waiting for types ${unresolvedDependencies
			// 		.map(([k, v]) => v.typeName)
			// 		.join(', ')}`
			// );
			allTypes.push(t);
		}
	}

	console.log(typeEntries.join('\n\n'));
}

export function generateType(typingParams: TypeGenerationInterface) {
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
		object: () => ({
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

				for (const [fieldName, field] of Object.entries(objectFields)) {
					fieldTypesString += `"${fieldName}": ( ${dependencies[fieldName].compiledType} ); `;
				}
				return `{ ${fieldTypesString} }`;
			},
			dependentTypes: Object.fromEntries(
				Object.entries(
					typingParams.objectFields ? typingParams.objectFields : {}
				).map(([fieldName, field]) => [fieldName, generateType(field)])
			),
			isLiteralType: true
		}),

		array: () => ({
			typeName: typingParams.dataPrimitive,
			typeDef: typingParams,
			typeString: (name, typeString, dependencies) =>
				`${
					typingParams.arrayOf
						? `( ${dependencies.compiledType} )`
						: ''
				}[]`,
			dependentTypes: {
				arrayOf: typingParams.arrayOf
					? generateType(typingParams.arrayOf)
					: undefined
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
					? generateType(typingParams.functionDefinition.returnType)
					: undefined
			},
			isLiteralType: true
		}),
		classType: () =>
			declareClass(
				typingParams as TypeGenerationInterface & {
					dataPrimitive: 'classType';
				}
			)!,
		classInstance: () =>
			generateType({
				...typingParams,
				dataPrimitive: 'object'
			})
	};

	const mapped = mappers[typingParams.dataPrimitive]();

	return mapped;
}

const classDefinition = declareClass({
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
});
generateTypeFiles([classDefinition as DeclaredTyping<any>]);
