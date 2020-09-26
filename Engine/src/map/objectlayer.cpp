#include "map/objectlayer.hpp"

#include <string>

namespace engine
{
    void ObjectLayer::loadFromXMLElement(const tinyxml2::XMLElement *element)
    {

        const tinyxml2::XMLElement *object = element->FirstChildElement();
        while (object)
        {
            std::string type = object->Attribute("type");
            int x = object->IntAttribute("x");
            int y = object->IntAttribute("y");
            int width = object->IntAttribute("width");
            int height = object->IntAttribute("height");

            if (type == "NPC")
            {
                this->parseNPC(object, x, y, width, height);
            }

            object = object->NextSiblingElement();
        }
    }

    void ObjectLayer::parseNPC(const tinyxml2::XMLElement *object, const int x, const int y, const int width, const int height)
    {
        const tinyxml2::XMLElement *properties = object->FirstChildElement("properties");
        int id, spriteId;
        std::string spriteSheet;

        const tinyxml2::XMLElement *property = properties->FirstChildElement();
        while (property)
        {
            std::string name = property->Attribute("name");
            if (name == "id")
                id = property->IntAttribute("value");
            else if (name == "sprite_id")
                spriteId = property->IntAttribute("value");
            else if (name == "sprite_sheet")
                spriteSheet = property->Attribute("value");

            property = property->NextSiblingElement();
        }

        
    }
} // namespace engine