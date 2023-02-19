using System.CodeDom;
using System.Collections.Generic;

class ClassGenerator{
    private static Dictionary<
        ClassTypeDefinition,
        CodeTypeDeclaration
    > classMappings = new Dictionary<ClassTypeDefinition, CodeTypeDeclaration>();

    private static CodeTypeMember BindField(string key, Member overload, bool isStatic = false){
        TypeDefinition typing = overload.Definition.get();
        CodeTypeMember member = GenericGenerator.GenerateMember(typing);
        member.Name = key;

        CodeComment memberDescription = new CodeComment(
            overload.Description,
            true );
        CodeCommentStatement memberDescriptionStatement = new CodeCommentStatement( memberDescription );
        member.Comments.Add(memberDescriptionStatement);

        member.Attributes &= ~MemberAttributes.Private;
        member.Attributes |= MemberAttributes.Public;

        if(isStatic){
            member.Attributes |= MemberAttributes.Static;
        }

        return member;
    }

    public static CodeTypeDeclaration Generate(ClassTypeDefinition def){
        if(!ClassGenerator.classMappings.ContainsKey(def)){
            CodeTypeDeclaration newDef = new CodeTypeDeclaration(def.Name);

            newDef.IsClass = true;

            CodeComment classDescription = new CodeComment(
                def.Description,
                true );
            CodeCommentStatement classDescriptionStatement = new CodeCommentStatement( classDescription );

            newDef.Comments.Add(classDescriptionStatement);

            foreach(var m in def.MemberFields){
                foreach(var overload in m.Value){
                    newDef.Members.Add(ClassGenerator.BindField(m.Key, overload));
                }
            }
            foreach(var s in def.StaticFields){
                foreach(var overload in s.Value){
                    newDef.Members.Add(ClassGenerator.BindField(s.Key, overload, true));
                }
            }

            ClassGenerator.classMappings[def] = newDef;
        }
        return ClassGenerator.classMappings[def];
    }
}