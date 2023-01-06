import {
	bindingHelpersModuleName,
	moduleResolutionPath
} from 'binding-helpers/src/binding-helpers';
import BindingHelpers from 'binding-helpers/src/binding-helpers';
import * as dom from 'dts-dom';
import { declaredTypes, createModuleMember } from './data-primitive-dom';
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
		description?: string;
		dataPrimitive: DataPrimitive;
		flags?: dom.DeclarationFlags;
		readonly?: boolean;
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
			classStaticType: TypeGenerationInterface & {
				dataPrimitive: 'object';
			};
			constructor: TypeGenerationInterface & {
				dataPrimitive: 'function';
			};
		};
	};

	export function generateTypeFiles(
		moduleName: string,
		types: TypeGenerationInterface[]
	) {
		const module = dom.create.module(moduleName);

		for(const t of types) {
			createModuleMember(t, true);
		};
		module.members.push(...declaredTypes);

		return dom.emit(module);
	}
}

export default TypeGeneration;

const types = TypeGeneration.generateTypeFiles(
	'API',
	JSON.parse(
		fs
			.readFileSync('/home/christopher/GIT/api-helper/test.json')
			.toString()
	)
);

console.log(types);
