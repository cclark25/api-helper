import TypeGeneration from './type-generation';
import * as dom from 'dts-dom';
declare type TypeGenerationInterface = TypeGeneration.TypeGenerationInterface;
export declare const constMappers: Record<TypeGeneration.DataPrimitive, (typing: TypeGenerationInterface) => dom.ConstDeclaration | dom.FunctionDeclaration>;
export declare const objectFieldMappers: Record<TypeGeneration.DataPrimitive, (typing: TypeGenerationInterface) => dom.PropertyDeclaration | dom.MethodDeclaration>;
export {};
