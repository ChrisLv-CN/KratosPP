#include "INI.h"

std::map<std::string, Dependency> INI::s_Dependency{};

const char *INI::SectionGeneral = "General";
const char *INI::SectionCombatDamage = "CombatDamage";
const char *INI::SectionAudioVisual = "AudioVisual";

GetDependency INI::Rules = INI::GetRules;
GetDependency INI::Art = INI::GetArt;
GetDependency INI::AI = INI::GetAI;

Dependency INI::s_Rules{};
Dependency INI::s_Art{};
Dependency INI::s_AI{};
