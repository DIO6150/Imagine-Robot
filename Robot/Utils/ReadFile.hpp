#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstddef>
#include <sstream>
#include <optional>

#include <Robot/Errors/Error.hpp>
#include <vendor/nlohmann/json.hpp>

inline bool isValidUTF8(json data)
{
    std::string dataToString = data.dump();
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(dataToString.c_str());
    size_t length = dataToString.size();
    
    size_t i = 0;
    while (i < length)
    {
        unsigned char c = bytes[i];
        size_t numBytes = 0;
        
        // Détermination du nombre d'octets du caractère UTF-8
        if (c <= 0x7F) {
            numBytes = 1; // ASCII (0xxxxxxx)
        } else if ((c & 0xE0) == 0xC0) {
            numBytes = 2; // 110xxxxx
        } else if ((c & 0xF0) == 0xE0) {
            numBytes = 3; // 1110xxxx
        } else if ((c & 0xF8) == 0xF0) {
            numBytes = 4; // 11110xxx
        } else {
            return false; // Premier octet invalide
        }
        
        // Vérification que la chaîne ne se coupe pas au milieu d'un caractère
        if (i + numBytes > length) {
            return false;
        }
        
        // Vérification des octets de continuation (qui doivent tous commencer par 10xxxxxx, soit & 0xC0 == 0x80)
        for (size_t j = 1; j < numBytes; ++j) {
            unsigned char nextC = bytes[i + j];
            if ((nextC & 0xC0) != 0x80) {
                return false;
            }
        }
        
        i += numBytes;
    }
    
    return true; // Tout est valide !
}

inline JSONParserStatus readJsonFile(const std::string& path, json & data)
{
    // 1. Vérification de l'existence du fichier (std::filesystem accepte directement le string)
    if (!std::filesystem::exists(path))
    {
        return JSONParserStatus::IncorrectPath;
    }

    // 2. Ouverture du fichier en mode lecture
    std::ifstream file(path);
    if (!file.is_open())
    {
        return JSONParserStatus::FileNotFound;
    }

    // 3. Lecture et parsing du JSON directement depuis le flux du fichier ouvert
    data = json::parse(file, nullptr, false);

    // Le fichier se ferme automatiquement ici grâce au destructeur de std::ifstream

    // 4. Vérification si le JSON est mal formé
    if (data.is_discarded())
    {
        return JSONParserStatus::InvalidJson;
    }
    
    // 5. Validation UTF-8 avec la fonction précédente
    if (!isValidUTF8(data))
    {
        return JSONParserStatus::NotUTF8;
    }

    return JSONParserStatus::Ok;
}

