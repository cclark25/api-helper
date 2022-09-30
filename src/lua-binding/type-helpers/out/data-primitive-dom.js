"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.objectFieldMappers = exports.constMappers = void 0;
const dom = require("dts-dom");
exports.constMappers = makeMapper(dom.create.const, dom.create.function);
exports.objectFieldMappers = makeMapper(dom.create.property, dom.create.method);
function makeMapper(creator, fCreator) {
    return {
        string: (typing) => creator(typing.name, 'string', typing.flags),
        int32: (typing) => creator(typing.name, 'number', typing.flags),
        null: (typing) => creator(typing.name, 'null', typing.flags),
        unknown: (typing) => creator(typing.name, 'any', typing.flags),
        object: (typing) => creator(typing.name, dom.create.objectType(Object.entries(typing.objectDefinition?.objectFields ?? []).map(([, f]) => exports.objectFieldMappers[f.dataPrimitive](f))), typing.flags),
        array: (typing) => creator(typing.name, typing.arrayOf
            ? dom.create.array(exports.constMappers[typing.arrayOf.dataPrimitive](typing.arrayOf))
            : 'any'),
        function: (typing) => fCreator(typing.name, (typing.functionDefinition?.parameters ?? []).map((p) => dom.create.parameter(p.parameterName, exports.constMappers[p.type.dataPrimitive](p.type))), typing.functionDefinition?.returnType
            ? exports.constMappers[typing.functionDefinition.returnType.dataPrimitive](typing.functionDefinition.returnType)
            : 'any', typing.flags),
        classType: (typing) => creator(typing.name, dom.create.class(typing.name, typing.flags), typing.flags)
    };
}
//# sourceMappingURL=data-primitive-dom.js.map