using System.CodeDom;
using System;

class GenericGenerator {
    public static CodeTypeDeclaration Generate(TypeDefinition def){
        if(def is ClassTypeDefinition){
            return ClassGenerator.Generate(def as ClassTypeDefinition);
        }
        throw new Exception();
    }

    public static CodeThrowExceptionStatement throwExceptionExpression = new CodeThrowExceptionStatement(
        new CodeObjectCreateExpression(
            new CodeTypeReference(typeof(System.Exception)),
            new CodePrimitiveExpression("This function was not properly bounded by an internal call.") 
        )
    );

    public static CodeTypeMember GenerateMember(TypeDefinition def){
        // if(def is ClassTypeDefinition){
        //     return ClassGenerator.GenerateMember(def as ClassTypeDefinition);
        // }
        if(def is FunctionTypeDefinition){
            return FunctionGenerator.GenerateMember(def as FunctionTypeDefinition);
        }
        if(def is ReferenceTypeDefinition){
            return GenericGenerator.GenerateMember((def as ReferenceTypeDefinition).ReferenceType);
        }

        var result = new CodeMemberProperty();
        string name = def.Name;
        result.Type = GenericGenerator.GetTypeReference(def);
        result.HasGet = true;
        result.HasSet = true;
        result.GetStatements.Add(
            GenericGenerator.throwExceptionExpression
        );
        result.SetStatements.Add(
            GenericGenerator.throwExceptionExpression
        );
        return result;
    }
    public static CodeTypeReference GetTypeReference(TypeDefinition def){
        if(def is ReferenceTypeDefinition){
            return GenericGenerator.GetTypeReference((def as ReferenceTypeDefinition).ReferenceType);
        }

        string name = def.Name;
        if(def.IsBasicType){
            switch(def.Name){
                case "string":
                    name = "System.String";
                    break;
                
                case "int":
                    name = "System.Int32";
                    break;
                
                case "double":
                    name = "System.Double";
                    break;
                
                case "void":
                    name = "System.Void";
                    break;

                
                case "bool":
                    name = "System.Bool";
                    break;

                case "size_t":
                    name = "System.UInt32";
                    break;
            }
        }

        return new CodeTypeReference(name);
    }
}