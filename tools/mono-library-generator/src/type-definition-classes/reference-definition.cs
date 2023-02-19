using System.Runtime;
using System.IO;
using System;
using System.IO;
using System.Collections.Generic;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

class ReferenceTypeDefinition : TypeDefinition {
    public ReferenceTypeDefinition(string typeID, JToken token): base(typeID,token) {
    } 

    public static bool IsReference(JToken token){
        if(token == null){
            return false;
        }

        var t = token.Value<JObject>();
        string name = t["name"].Value<string>();
        if(t != null && !( t["referenceTypeId"] == null || t["referenceTypeId"].Type == JTokenType.Null )){
            return true;
        }
        return false;
    }

    public TypeDefinition ReferenceType {
        get{
            string typeID = this.data["referenceTypeId"].Value<string>();
            return TypeDefinition.DeclaredTypes[typeID];
        }
    }
}