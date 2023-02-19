using System.Runtime;
using System.IO;
using System;
using System.IO;
using System.Collections.Generic;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;


using Overloads = System.Collections.Generic.List<Member>;

class Member {
    public string Description;
    public TypeDefinitionRef Definition;
    public Member(string Description, TypeDefinitionRef Definition){
        this.Description = Description;
        this.Definition = Definition;
    }
}


class ClassTypeDefinition : TypeDefinition {
    public ClassTypeDefinition(string typeID, JToken token): base(typeID,token) {
    }

    private static bool isNull(JToken input){
        return input == null || input.Type == JTokenType.Null;
    }

    public static bool IsClass(JToken token){
        if(token == null){
            return false;
        }

        var t = token.Value<JObject>();
        var name = t["name"];
        var e1 = ClassTypeDefinition.isNull(t["memberFields"]);
        var e2 = ClassTypeDefinition.isNull(t["staticFields"]);
        var e3 = ClassTypeDefinition.isNull(t["staticFields"]);
        var e4 = ClassTypeDefinition.isNull(t["inheritsFrom"]);

        if(t != null && (
            !ClassTypeDefinition.isNull(t["memberFields"]) ||
            !ClassTypeDefinition.isNull(t["staticFields"]) ||
            !ClassTypeDefinition.isNull(t["staticFields"]) ||
            !ClassTypeDefinition.isNull(t["inheritsFrom"])
        )){
            return true;
        }
        return false;
    }


    public Dictionary<string, Overloads> MemberFields {
        get{ 
            var field = this.data["memberFields"];
            JObject memberJson;
            memberJson = field.Value<JObject>();
            Dictionary<string, Overloads> mappings = new Dictionary<string, Overloads>();
            if(memberJson != null){
                foreach(KeyValuePair<string, JToken> item in memberJson){
                    string key = item.Key;
                    Overloads ol = new Overloads();
                    if(item.Value == null){
                        continue;
                    }
                    JObject member = item.Value.Value<JObject>();
                    if(
                        member["isMemberOverload"] != null
                        && member["isMemberOverload"].Value<bool>()
                    ){
                        JToken overloadToken = member["overloads"];
                        if(overloadToken != null){
                            JArray values = overloadToken.Value<JArray>();
                            foreach(JToken t in values){
                                if(t == null){
                                    continue;
                                }
                                JObject to = t.Value<JObject>();
                                ol.Add(
                                    new Member(
                                        to["description"].Value<string>(),
                                        new TypeDefinitionRef(to["typeId"].Value<string>())
                                    )
                                );
                            }
                        }
                    }
                    else {
                        ol.Add(
                            new Member(
                                member["description"].Value<string>(),
                                new TypeDefinitionRef(member["typeId"].Value<string>())
                            )
                        );
                    }

                    mappings[key] = ol;
                }
            }
            return mappings;
        }
    }

    public Dictionary<string, Overloads> StaticFields {
        get{ 
            var field = this.data["staticFields"];
            JObject memberJson;
            memberJson = field.Value<JObject>();
            Dictionary<string, Overloads> mappings = new Dictionary<string, Overloads>();
            if(memberJson != null){
                foreach(KeyValuePair<string, JToken> item in memberJson){
                    string key = item.Key;
                    Overloads ol = new Overloads();
                    if(item.Value == null){
                        continue;
                    }
                    JObject member = item.Value.Value<JObject>();
                    if(
                        member["isStaticOverload"] != null
                        && member["isStaticOverload"].Value<bool>()
                    ){
                        JToken overloadToken = member["overloads"];
                        if(overloadToken != null){
                            JArray values = overloadToken.Value<JArray>();
                            foreach(JToken t in values){
                                if(t == null){
                                    continue;
                                }
                                JObject to = t.Value<JObject>();
                                ol.Add(
                                    new Member(
                                        to["description"].Value<string>(),
                                        new TypeDefinitionRef(to["typeId"].Value<string>())
                                    )
                                );
                            }
                        }
                    }
                    else {
                        ol.Add(
                            new Member(
                                member["description"].Value<string>(),
                                new TypeDefinitionRef(member["typeId"].Value<string>())
                            )
                        );
                    }

                    mappings[key] = ol;
                }
            }
            return mappings;
        }
    }

    public TypeDefinition InheritsFrom {
        get{
            if(this.data["inheritsFrom"] == null){
                return null;
            }

            string typeID = this.data["inheritsFrom"].Value<string>();
            
            return TypeDefinition.DeclaredTypes[typeID];
        }
    }
}