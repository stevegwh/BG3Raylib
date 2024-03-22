//
// Created by Steve Wheeler on 21/03/2024.
//

#include "Serializer.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

namespace sage
{

void Serializer::SerializeToFile(const SerializationData& serializeData)
{
    json j;

    for (const auto& type : serializeData)
    {
        std::string typeName = type.first;

        for (const auto& components : type.second)
        {
            json componentJson;

            for (const auto& field : components)
            {
                componentJson[field.first] = field.second;
            }
            
            j[typeName].push_back(componentJson);
        }
    }

    std::ofstream o("pretty.json");
    o << std::setw(4) << j << std::endl;
}

// std::vector<std::pair<std::string, std::vector<std::unordered_map<std::string, std::string>>>> SerializationData;
// { "TypeName": [ { "FieldName": "10004", "FieldName": "10.00, 0.00, 20.00" } ] }
// { "Transform": [ { "EntityId": "10004", "Position": "10.00, 0.00, 20.00" } ] }
SerializationData Serializer::DeserializeFile()
{
    std::ifstream i("pretty.json");
    json j = json::parse(i);
    i.close();

    SerializationData deserializeData;

    // Iterate over each type in the JSON object
    for (const auto& type : j.items())
    {
        std::string typeName = type.key();
        std::vector<std::unordered_map<std::string, std::string>> components;

        // Iterate over each component in the type
        for (const auto& component : type.value())
        {
            std::unordered_map<std::string, std::string> componentMap;

            // Deserialize each field in the component
            for (const auto& field : component.items())
            {
                componentMap[field.key()] = field.value();
            }

            // Add the deserialized component to the components vector
            components.push_back(componentMap);
        }

        // Add the deserialized type and its components to the deserializeData vector
        deserializeData.emplace_back(typeName, components);

        for (const auto& t : deserializeData)
        {
            std::cout << "Type: " << t.first << std::endl;
            for (const auto& component : t.second)
            {
                std::cout << "\tComponent:" << std::endl;
                for (const auto& field : component)
                {
                    std::cout << "\t\t" << field.first << ": " << field.second << std::endl;
                }
            }
        }
        return deserializeData;
    }

    // Now deserializeData contains the deserialized data in the same structure as it was serialized
    // You can further process this data as needed
    
}

} // sage