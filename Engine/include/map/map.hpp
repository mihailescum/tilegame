#ifndef __ENGINE_MAP_H__
#define __ENGINE_MAP_H__

#include <string>

namespace engine
{
    class Map
    {
        private:
            
        public:
            Map();
            ~Map();
            void loadFromFile(const std::string &filename);
    };
} // namespace engine

#endif