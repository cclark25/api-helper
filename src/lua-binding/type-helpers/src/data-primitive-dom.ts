import TypeGeneration from './type-generation';
import * as dom from 'dts-dom';

type TypeGenerationInterface = TypeGeneration.TypeGenerationInterface;

export const declaredTypes: dom.ModuleMember[] = [];
export function createModuleMember(
	typing: TypeGenerationInterface,
	exported = false
): dom.ModuleMember | undefined {
	let type: dom.ModuleMember;
	switch (typing.dataPrimitive) {
		case 'function':
			const functionType = createDomType(typing) as dom.FunctionType;
			type = dom.create.function(
				typing.name,
				functionType?.parameters,
				functionType?.returnType
			);
			type.jsDocComment = createDescription(typing);
			break;
		case 'classType':
			const classDefinition = dom.create.class(
				typing.classDefinition?.className ?? typing.name
			);
			if (typing.classDefinition?.constructor) {
				const constructorType = createDomType(
					typing.classDefinition?.constructor
				) as dom.FunctionType;
				const instanceFields = (
					constructorType.returnType as dom.ObjectType
				).members as Exclude<dom.ObjectTypeMember, dom.CallSignature>[];

				const staticFields = (
					createDomType(
						typing.classDefinition.classStaticType
					) as dom.ObjectType
				).members as Exclude<dom.ObjectTypeMember, dom.CallSignature>[];

				classDefinition.members.push(
					...staticFields.map((s) => ({
						...s,
						flags:
							(s.flags ?? dom.DeclarationFlags.None) |
							dom.DeclarationFlags.Static
					}))
				);
				classDefinition.members.push(...instanceFields);

				classDefinition.members.push({
					...dom.create.constructor(constructorType.parameters),
					jsDocComment: createDescription(
						typing.classDefinition.constructor
					)
				});
			}
			type = classDefinition;
			break;
		default:
			if (typing.readonly) {
				type = dom.create.const(typing.name, createDomType(typing));
			} else {
				type = dom.create.variable(typing.name, createDomType(typing));
			}
	}

	if (exported) {
		if (type.flags === undefined) {
			type.flags = dom.DeclarationFlags.None;
		}
		type.flags = type.flags | dom.DeclarationFlags.Export;
	}
	type.jsDocComment ??= typing.description;

	declaredTypes.push(type);
	return type;
}

function createDomType(typing: TypeGenerationInterface): dom.Type {
	let resultType: dom.Type = dom.type.any;
	switch (typing.dataPrimitive) {
		case 'function':
			resultType = dom.create.functionType(
				typing.functionDefinition?.parameters.map((p) => {
					const param = dom.create.parameter(
						p.parameterName,
						createDomType(p.type) ?? dom.type.void
					);

					return param;
				}) ?? [],
				(typing.functionDefinition?.returnType
					? createDomType(typing.functionDefinition?.returnType)
					: dom.type.void) ?? dom.type.void
			);

			break;
		case 'object':
			resultType = dom.create.objectType(
				Object.entries(
					typing.objectDefinition?.objectFields ?? {}
				).flatMap(([key, value]) => {
					const valueTyped = createDomType(value);

					const definedProperty = dom.create.property(
						key,
						valueTyped
					);

					if (value.readonly) {
						definedProperty.flags ??= dom.DeclarationFlags.None;
						definedProperty.flags |= dom.DeclarationFlags.ReadOnly;
					}

					definedProperty.jsDocComment = createDescription(value);
					return definedProperty;
				})
			);
			break;
		case 'string':
			resultType = dom.type.string;
			break;
		case 'int32':
			resultType = dom.type.number;
			break;
		case 'null':
			resultType = dom.type.null;
			break;
		case 'unknown':
			resultType = dom.type.any;
			break;
		case 'array':
			resultType = dom.type.array(
				typing.arrayOf ? createDomType(typing.arrayOf) : dom.type.any
			);
			break;
	}

	return resultType;
}

function createDescription(
	typing: TypeGenerationInterface
): string | undefined {
	switch (typing.dataPrimitive) {
		case 'function':
			let result: string | undefined = typing.description ?? '';
			result +=
				'\n' +
				(typing.functionDefinition?.parameters ?? [])
					.map(
						(p) =>
							`@param ${p.parameterName}` +
							(p.type.description ? ` ${p.type.description}` : '')
					)
					.join('\n');

			result = result.trim();
			if (!result) {
				result = undefined;
			}
			return result;
		default:
			return typing.description;
	}
}
