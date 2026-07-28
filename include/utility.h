#pragma once

namespace IniParser {

    void Load();
    void Save();

}

// ==================================================================================================================================================================================
//  GameSettings helper for writing to Skyrim's own runtime INI setting (It won't overwrite the user INI, it just applies to its virtual copy the game uses when started
// ==================================================================================================================================================================================

namespace GameSettings {

    void ApplyFloat(const std::string& a_settingName, float a_value);

}