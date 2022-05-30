#ifndef LUA_LOADERS
#define LUA_LOADERS
#include <string>
#include <sol/sol.hpp>

namespace APILua
{
    int LoadFile(lua_State *L)
    {
        std::string path = "binding-helpers/" + sol::stack::get<std::string>(L) + ".lua";

        auto i = luaL_loadfilex(L, path.c_str(), "text");
        if (i == 0)
        {
            return 1;
        }
        else
        {
            return i;
        }
    }
}

#endif