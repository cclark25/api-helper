using System.CodeDom;
using System;

class FunctionGenerator {
    // public static CodeTypeDeclaration Generate(TypeDefinition def){
    //     if(def is ClassTypeDefinition){
    //         return ClassGenerator.Generate(def as ClassTypeDefinition);
    //     }
    //     throw new Exception();
    // }
    public static CodeTypeMember GenerateMember(FunctionTypeDefinition def){
        CodeMemberMethod method = new CodeMemberMethod();
        method.ReturnType = GenericGenerator.GetTypeReference(def.ReturnType);
        foreach(var param in def.Parameters){
            CodeParameterDeclarationExpression paramExpr = new CodeParameterDeclarationExpression();
            paramExpr.Name = param.Name;
            paramExpr.Type = GenericGenerator.GetTypeReference(param.Type);
            
            method.Parameters.Add(paramExpr);
        }
        method.CustomAttributes = new CodeAttributeDeclarationCollection();
        method.CustomAttributes.Add(
            new CodeAttributeDeclaration(
                "MethodImplAttribute",  
                new CodeAttributeArgument(
                    new CodeSnippetExpression("MethodImplOptions.InternalCall")
                )
            )
        );
        method.Statements.Add(GenericGenerator.throwExceptionExpression);
        
        return method;        
    }
}