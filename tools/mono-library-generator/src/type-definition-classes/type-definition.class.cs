using System.Runtime;
using System.IO;
using System;
using System.IO;
using System.Collections.Generic;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

class TypeDefinition {
    public readonly string typeID;
    public readonly JObject data;
    protected TypeDefinition(string typeID, JToken token){
        this.typeID = typeID;
        this.data = token.Value<JObject>();
        TypeDefinition.DeclaredTypes[typeID] = this;
    }

    public static Dictionary<string, TypeDefinition> DeclaredTypes = new Dictionary<string, TypeDefinition>();

    static public TypeDefinition From(string typeID, JToken token){
        if(ClassTypeDefinition.IsClass(token)){
            return new ClassTypeDefinition(typeID, token);
        }
        else if(FunctionTypeDefinition.IsFunction(token)){
            return new FunctionTypeDefinition(typeID, token);   
        }
        else if(ReferenceTypeDefinition.IsReference(token)){
            return new ReferenceTypeDefinition(typeID, token);   
        }
        return new TypeDefinition(typeID, token);
    }

    public string Name {
        get{ return this.data["name"].Value<string>(); }
    }
    public string Description {
        get{ 
            var descField = this.data["description"];
            if(descField != null){
                return descField.Value<string>(); 
            }
            return null;
        }
    }
    public bool IsBasicType {
        get{ 
            var field = this.data["isBasicType"];
            if(field != null){
                return field.Value<bool>(); 
            }
            return false;
        }
    }
}

class TypeDefinitionRef {
    private string typeID;
    public TypeDefinitionRef(string typeID){
        this.typeID = typeID;
    }
    public TypeDefinition get(){
        return TypeDefinition.DeclaredTypes[this.typeID];
    }
}