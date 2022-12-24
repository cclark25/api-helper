#ifndef BASE_LUA_BINDER
#define BASE_LUA_BINDER

#include <memory>
#include <string>
#include <sol.hpp>

namespace APICore
{
    template <class ParentClassType, class T, StringLiteral BinderTag, class... ExtraData>
    struct LuaBinder
    {
        using type_definition = TypeLookup<T>;
        static void bind(sol::state &state, void *userType = nullptr, T *memberPtr = nullptr)
        {
            return;
        }
    };

    template <class ParentClassType, class T, class... ExtraData>
        requires requires {
                     std::is_class<ParentClassType>::value;
                     {
                         TypeLookup<T>::registeredType::ptr
                     };
                 }
    struct LuaBinder<ParentClassType, T, "MEMBER", ExtraData...>
    {
        using type_definition = TypeLookup<T>;
        static void bind(sol::state &state, sol::usertype<ParentClassType> *userType, T ParentClassType::*memberPtr, std::string key)
        {
            sol::usertype<ParentClassType> &parentType = *userType;
            parentType[key] = memberPtr;

            return;
        }
        static void bind(sol::state &state, sol::usertype<ParentClassType> *userType, T *ptr, std::string key)
        {
            sol::usertype<ParentClassType> &parentType = *userType;
            T &value = *ptr;
            // parentType[key] = sol::var(std::ref(value));
            // parentType[key] = sol::property([ptr](){
            //     std::cout << "Here 1\n";
            //     return *ptr;
            //     }, 
            //     [ptr](double newVal){
            //     std::cout << "Here 2\n";
            //     (*ptr) = newVal;
            // });            

            return;
        }
    };

    // template<class... ExtraData>
    // struct LuaBinder<std::string, "ANY", ExtraData...> {
    //     static void bind(sol::state & state, std::shared_ptr<sol::usertype<ClassType>> userType){
    //     }
    // };

    // template<class... ExtraData>
    // struct LuaBinder<int, "ANY", ExtraData...> {
    //     static void bind(sol::state & state){
    //     }
    // };

    // template<class... ExtraData>
    // struct LuaBinder<void, "ANY", ExtraData...> {
    //     static void bind(sol::state & state){
    //     }
    // };

    // template<class... ExtraData>
    // struct LuaBinder<double, "ANY", ExtraData...> {
    //     static void bind(sol::state & state){
    //     }
    // };
}

#include "./class-binder.hpp"
#include "./function-binder.hpp"

#endif