using System.Runtime;
using System.IO;
using System;
using System.IO;
using System.Collections.Generic;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

class Parameter {
    public string Description;
    public string Name;
    public string TypeID;

    public TypeDefinition Type {
        get{
            return TypeDefinition.DeclaredTypes[this.TypeID];
        }
    }
    public Parameter(string Name, string Description, string TypeID){
        this.Description = Description;
        this.TypeID = TypeID;
        this.Name = Name;
    }
}

class FunctionTypeDefinition : TypeDefinition {
    public FunctionTypeDefinition(string typeID, JToken token): base(typeID,token) {
    } 

    public static bool IsFunction(JToken token){
        if(token == null){
            return false;
        }

        var t = token.Value<JObject>();
        if(t != null && ( t["functionDescription"] != null )){
            return true;
        }
        return false;
    }

    public TypeDefinition ReturnType {
        get{
            string typeID = this.data["functionDescription"].Value<JObject>()["returnTypeId"].Value<string>();
            return TypeDefinition.DeclaredTypes[typeID];
        }
    }

    public List<Parameter> Parameters {
        get {
            List<Parameter> results = new List<Parameter>();
            JArray a = this.data["functionDescription"].Value<JObject>()["parameters"].Value<JArray>();
            if(a == null){
                return results;
            }
            foreach(JObject o in a){
                results.Add(new Parameter(
                    o["name"].Value<string>(),
                    o["description"].Value<string>(),
                    o["typeId"].Value<string>()
                ));
            }
            return results;
        }
    }
}