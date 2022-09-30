import TypeGeneration from './type-generation';
import * as dom from 'dts-dom';

type TypeGenerationInterface = TypeGeneration.TypeGenerationInterface;

export const constMappers = makeMapper(dom.create.const, dom.create.function);
export const variableMappers = makeMapper(
	dom.create.alias,
	dom.create.function
);
export const parameterMappers = makeMapper(dom.create.parameter);

export const objectFieldMappers = makeMapper(
	dom.create.property,
	dom.create.method
);

function makeMapper<R extends Object, F = never>(
	creator: (name: string, type: dom.Type, flags?: any | undefined) => R,
	fCreator?: (
		name: string,
		parameters: dom.Parameter[],
		returnType: dom.Type,
		flags?: any | undefined
	) => F
): Record<
	TypeGeneration.DataPrimitive,
	(typing: TypeGenerationInterface) => R | F | dom.ClassDeclaration
> {
	const mappers: Record<
		TypeGeneration.DataPrimitive,
		(typing: TypeGenerationInterface) => R | F | dom.ClassDeclaration
	> = {
		string: (typing) => creator(typing.name, 'string', typing.flags),
		int32: (typing) => creator(typing.name, 'number', typing.flags),
		null: (typing) => creator(typing.name, 'null', typing.flags),
		unknown: (typing) => creator(typing.name, 'any', typing.flags),
		object: (typing) =>
			creator(
				typing.name,
				dom.create.objectType(
					Object.entries(
						typing.objectDefinition?.objectFields ?? []
					).map(([, f]) =>
						objectFieldMappers[f.dataPrimitive]({
							...f,
							_parent: typing
						} as any)
					) as any
				),
				typing.flags
			),
		array: (typing) =>
			creator(
				typing.name,
				typing.arrayOf
					? dom.create.array(
							constMappers[typing.arrayOf.dataPrimitive]({
								...typing.arrayOf,
								_parent: typing
							} as any)
					  )
					: 'any'
			),
		function: (typing) => {
			if (!fCreator) {
				throw Error('Missing fCreator!');
			}
			return fCreator(
				typing.name,
				(typing.functionDefinition?.parameters ?? []).map((p) =>
					parameterMappers[p.type.dataPrimitive]({
						...p.type,
						_parent: typing
					} as any)
				) as any,
				typing.functionDefinition?.returnType
					? variableMappers[
							typing.functionDefinition.returnType.dataPrimitive
					  ]({
							...typing.functionDefinition.returnType,
							_parent: typing
					  } as any)
					: 'any',
				typing.flags
			);
		},
		classType: (typing) => {
			const classDef = dom.create.class(typing.name, typing.flags);
			classDef.members = Object.entries(
				typing.classDefinition?.classInstanceType.objectDefinition
					?.objectFields ?? {}
			).map(([key, value]) =>
				objectFieldMappers[value.dataPrimitive](value)
			) as any;

			classDef.members = classDef.members.concat(
				Object.entries(
					typing.classDefinition?.classStaticType.objectDefinition
						?.objectFields ?? {}
				).map(([key, value]) => {
					const result =
						objectFieldMappers[value.dataPrimitive](value);
					result.flags = dom.DeclarationFlags.Static;
					return result;
				}) as any
			);
			return classDef;
		}
	};

	return Object.fromEntries(
		Object.entries(mappers).map(([key, value]) => [
			key,
			(typing) => ({ ...value(typing), _typing: typing } as any)
		])
	) as Record<
		TypeGeneration.DataPrimitive,
		(typing: TypeGenerationInterface) => R | F
	>;
}

export const declaredTypes: dom.ModuleMember[] = [];
export function f(
	typing: TypeGenerationInterface,
	exported = false
): dom.ModuleMember | undefined {
	let type: dom.ModuleMember;
	switch (typing.dataPrimitive) {
		case 'function':
			const functionType = g(typing) as dom.FunctionType;
			type = dom.create.function(
				typing.name,
				functionType?.parameters,
				functionType?.returnType
			);
			break;
		case 'classType':
			const classDefinition = dom.create.class(
				typing.classDefinition?.className ?? typing.name
			);
			if (typing.classDefinition?.constructor) {
				const constructorType = g(
					typing.classDefinition?.constructor
				) as dom.FunctionType;
				const instanceFields = (
					constructorType.returnType as dom.ObjectType
				).members as Exclude<dom.ObjectTypeMember, dom.CallSignature>[];

				const staticFields = (
					g(typing.classDefinition.classStaticType) as dom.ObjectType
				).members as Exclude<dom.ObjectTypeMember, dom.CallSignature>[];

				classDefinition.members.push(
					...staticFields.map((s) => ({
						...s,
						flags: dom.DeclarationFlags.Static
					}))
				);
				classDefinition.members.push(...instanceFields);

				classDefinition.members.push(
					dom.create.constructor(constructorType.parameters)
				);
			}
			type = classDefinition;
			break;
		default:
			type = dom.create.const(typing.name, g(typing));
	}

	if (exported) {
		if (type.flags === undefined) {
			type.flags = dom.DeclarationFlags.None;
		}
		type.flags = type.flags | dom.DeclarationFlags.Export;
	}

	declaredTypes.push(type);
	return type;
}

function g(typing: TypeGenerationInterface): dom.Type {
	switch (typing.dataPrimitive) {
		case 'function':
			return dom.create.functionType(
				typing.functionDefinition?.parameters.map((p) =>
					dom.create.parameter(
						p.parameterName,
						g(p.type) ?? dom.type.void
					)
				) ?? [],
				(typing.functionDefinition?.returnType
					? g(typing.functionDefinition?.returnType)
					: dom.type.void) ?? dom.type.void
			);
		case 'object':
			return dom.create.objectType(
				Object.entries(
					typing.objectDefinition?.objectFields ?? {}
				).flatMap(([key, value]) => {
					const valueTyped = g(value);
					return dom.create.property(key, valueTyped);
				})
			);
		case 'string':
			return dom.type.string;
		case 'int32':
			return dom.type.number;
		case 'null':
			return dom.type.null;
		case 'unknown':
			return dom.type.any;
		case 'array':
			return dom.type.array(
				typing.arrayOf ? g(typing.arrayOf) : dom.type.any
			);
	}

	return dom.type.any;
}
