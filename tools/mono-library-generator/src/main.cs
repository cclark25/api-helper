using System.Runtime;
using System.IO;
using System;
using System.IO;
using System.Collections.Generic;
using System.CodeDom;
using System.CodeDom.Compiler;
using Microsoft.CSharp;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;



class APIDocJSON {
    JObject jsonO;
    public APIDocJSON(string jString){
        this.jsonO = JObject.Load(new JsonTextReader(new StringReader(jString)));
    }
    public List<TypeDefinition> typeDefinitions {
        get{ 
            List<TypeDefinition> defs = new List<TypeDefinition>();
            
            foreach (KeyValuePair<string, JToken> item in this.jsonO)
            {
                defs.Add(TypeDefinition.From(item.Key, item.Value));
            }

            return defs;
        }
    }
};

class MainClass {
    static void PrintTyping(string padding, TypeDefinition def){
        if(def.Description != null){
            Console.WriteLine($"{padding}name: {def.Name}");
            Console.WriteLine($"{padding}description: {def.Description}");
        }
        else {
            Console.WriteLine($"{padding}{def.Name}");
        }
        if(def is ClassTypeDefinition){
            PrintClassTyping(padding + " ", def as ClassTypeDefinition);
        }
        else if(def is FunctionTypeDefinition){
            PrintFunctionTyping(padding, def as FunctionTypeDefinition);
        }
    }
    static void PrintFunctionTyping(string padding, FunctionTypeDefinition def){
        var parameters = def.Parameters.ConvertAll<string>(
            (Parameter p)=>{ 
                return $"{p.Type.Name} {p.Name} ({p.Description})";
            }
        );
        string paramString = "";
        foreach(string p in parameters){
            if(paramString == ""){
                paramString += p;
            }
            else {
                paramString += $", {p}";
            }
        }
        if(paramString != ""){
            Console.WriteLine($"{padding}Parameters: {paramString}");
        }
        Console.WriteLine($"{padding}Returns: {def.ReturnType.Name}");
    }
    static void PrintClassTyping(string padding, ClassTypeDefinition classDef){
        if(classDef.InheritsFrom != null){
            Console.WriteLine($"{padding}Inherits from: {classDef.InheritsFrom.Name}");
        }
        if(classDef.MemberFields.Count > 0){
            Console.WriteLine($"{padding}MemberFields:");
            foreach(var p in classDef.MemberFields){
                Console.WriteLine($"{padding}  {p.Key}");
                foreach(var a in p.Value){
                    var memberTyping = a.Definition.get();
                    if(p.Value.Count > 1){
                        Console.WriteLine($"{padding}    (overload)");
                    }
                    PrintTyping($"{padding}      ", memberTyping);
                }
            }
        }
        if(classDef.StaticFields.Count > 0){
            Console.WriteLine($"{padding}StaticFields:");
            foreach(var p in classDef.StaticFields){
                Console.WriteLine($"{padding}  {p.Key}");
                foreach(var a in p.Value){
                    var memberTyping = a.Definition.get();
                    if(p.Value.Count > 1){
                        Console.WriteLine($"{padding}    (overload)");
                    }
                    PrintTyping($"{padding}      ", memberTyping);
                }
            }
        }
    }

    static void Main(string[] args){
        string filePath = args[0];
        string fileContents = System.IO.File.ReadAllText(filePath);

        APIDocJSON doc = new APIDocJSON(fileContents);
        var x = doc.typeDefinitions;
        CodeCompileUnit compileUnit = new CodeCompileUnit();
        CodeNamespace samples = new CodeNamespace("Samples");
        samples.Imports.Add(new CodeNamespaceImport("System.Runtime.CompilerServices") );
        compileUnit.Namespaces.Add( samples );

        foreach(var v in x){
            // if(v is ClassTypeDefinition){
            //     PrintTyping("", v);
            // }

            if(v is ClassTypeDefinition){
                CodeTypeDeclaration class1 = ClassGenerator.Generate(v as ClassTypeDefinition);
                samples.Types.Add(class1);
            }
        }

        CSharpCodeProvider provider = new CSharpCodeProvider();
        StringWriter sw = new StringWriter(); 
        provider.GenerateCodeFromCompileUnit(compileUnit, sw,
            new CodeGeneratorOptions());


        Console.WriteLine(sw);
    }
}