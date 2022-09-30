import * as dom from 'dts-dom';
declare module TypeGeneration {
    export type DataPrimitive = 'string' | 'int32' | 'null' | 'object' | 'function' | 'array' | 'classType' | 'unknown';
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
    type Dependencies<S extends Dependencies<{}>> = Record<string, DeclaredTyping<S>>;
    interface CompiledDependency {
        dependency: DeclaredTyping<any>;
        compiledType: string;
        instanceOfType?: string;
    }
    interface DeclaredTyping<D extends Dependencies<any>> {
        typeName: string;
        typeDef: TypeGenerationInterface;
        typeString: (name: string, typeString: string, dependencies: {
            [K in keyof D]: CompiledDependency;
        }) => string;
        dependentTypes: D;
        isLiteralType?: boolean;
    }
    export function declareType<D extends Dependencies<{
        typing: DeclaredTyping<any>;
    }>>(typeName: string, typeDef: TypeGenerationInterface, dependentTypes: D): DeclaredTyping<D>;
    export function declareClass(classDef: TypeGenerationInterface & {
        dataPrimitive: 'classType';
    }): DeclaredTyping<{
        classInstanceType: DeclaredTyping<any>;
        classStaticType: DeclaredTyping<any>;
        constructorType: DeclaredTyping<any>;
    }> | undefined;
    export function declareConst(name: string, def: TypeGenerationInterface): DeclaredTyping<Record<string, DeclaredTyping<any>>>;
    export function declare(name: string, def: TypeGenerationInterface): DeclaredTyping<{
        classInstanceType: DeclaredTyping<any>;
        classStaticType: DeclaredTyping<any>;
        constructorType: DeclaredTyping<any>;
    }> | DeclaredTyping<Record<string, DeclaredTyping<any>>> | undefined;
    export function extractAllTypes(types: (DeclaredTyping<Record<string, DeclaredTyping<any>>> & {
        alreadyDone?: true;
    })[], skipTypes?: DeclaredTyping<any>[]): DeclaredTyping<Record<string, DeclaredTyping<any>>>[];
    export function generateTypeFiles(moduleName: string, types: DeclaredTyping<any>[]): string;
    export function generateTypeFilesV2(moduleName: string, types: TypeGenerationInterface[]): string;
    export function generateType(typingParams: TypeGenerationInterface): DeclaredTyping<any>;
    export {};
}
export default TypeGeneration;
