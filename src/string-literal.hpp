#ifndef STRING_LITERAL
#define STRING_LITERAL

namespace APICore
{
    template <size_t Length>
    struct StringLiteral
    {
        char value[Length]{};
        static constexpr std::size_t size = Length - 1;
        std::size_t length = Length - 1;

        consteval StringLiteral() {}
        consteval StringLiteral(const char (&str)[Length])
        {
            std::copy_n(str, Length, value);
        }

        template <size_t L>
        bool operator==(const StringLiteral<L> &other)
        {
            return std::string(this->value) == std::string(other.value);
        }

        constexpr operator std::string()
        {
            return std::string(this->value);
        }
    };

};

#endif