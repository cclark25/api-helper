#ifndef API_CLASS_INTERFACE
#define API_CLASS_INTERFACE
#include <string>
#include <memory>

// TODO: remove this file as it is not used.

namespace APICore {
    typedef std::shared_ptr<void> Data;

    class ClassInterface {
        public:
            virtual Data get(const std::string &index);
            virtual Data get(const int &index);
            
            virtual Data set(const std::string &index);
            virtual Data set(const int &index);
    };
}

#endif