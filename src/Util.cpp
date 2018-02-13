#include "Util.h"
#include "CCBot.h"
#include <iostream>

std::string Util::GetStringFromRace(const CCRace & race)
{
    if      (race == sc2::Race::Terran)  { return "Terran"; }
    else if (race == sc2::Race::Protoss) { return "Protoss"; }
    else if (race == sc2::Race::Zerg)    { return "Zerg"; }
    else if (race == sc2::Race::Random)  { return "Random"; }
    BOT_ASSERT(false, "Unknown Race");
    return "Error";
}

CCRace Util::GetRaceFromString(const std::string & raceIn)
{
    std::string race(raceIn);
    std::transform(race.begin(), race.end(), race.begin(), ::tolower);

    if      (race == "terran")  { return sc2::Race::Terran; }
    else if (race == "protoss") { return sc2::Race::Protoss; }
    else if (race == "zerg")    { return sc2::Race::Zerg; }
    else if (race == "random")  { return sc2::Race::Random; }
    
    BOT_ASSERT(false, "Unknown Race: ", race.c_str());
    return sc2::Race::Random;
}

CCPositionType Util::TileToPosition(float tile)
{
    return tile;
}

UnitType Util::GetSupplyProvider(const CCRace & race, CCBot & bot)
{
    switch (race) 
    {
        case sc2::Race::Terran: return UnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT, bot);
        case sc2::Race::Protoss: return UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, bot);
        case sc2::Race::Zerg: return UnitType(sc2::UNIT_TYPEID::ZERG_OVERLORD, bot);
        default: return UnitType();
    }
}

UnitType Util::GetTownHall(const CCRace & race, CCBot & bot)
{
    switch (race) 
    {
        case sc2::Race::Terran: return UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, bot);
        case sc2::Race::Protoss: return UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, bot);
        case sc2::Race::Zerg: return UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, bot);
        default: return UnitType();
    }
}

UnitType Util::GetRefinery(const CCRace & race, CCBot & bot)
{
    switch (race) 
    {
        case sc2::Race::Terran: return UnitType(sc2::UNIT_TYPEID::TERRAN_REFINERY, bot);
        case sc2::Race::Protoss: return UnitType(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, bot);
        case sc2::Race::Zerg: return UnitType(sc2::UNIT_TYPEID::ZERG_EXTRACTOR, bot);
        default: return UnitType();
    }
}

CCPosition Util::CalcCenter(const std::vector<Unit> & units)
{
    if (units.empty())
    {
        return CCPosition(0, 0);
    }

    CCPositionType cx = 0;
    CCPositionType cy = 0;

    for (auto & unit : units)
    {
        BOT_ASSERT(unit.isValid(), "Unit pointer was null");
        cx += unit.getPosition().x;
        cy += unit.getPosition().y;
    }

    return CCPosition(cx / units.size(), cy / units.size());
}

bool Util::IsZerg(const CCRace & race)
{
    return race == sc2::Race::Zerg;
}

bool Util::IsProtoss(const CCRace & race)
{
    return race == sc2::Race::Protoss;
}

bool Util::IsTerran(const CCRace & race)
{
    return race == sc2::Race::Terran;
}

CCTilePosition Util::GetTilePosition(const CCPosition & pos)
{
    return CCTilePosition((int)std::floor(pos.x), (int)std::floor(pos.y));
}

CCPosition Util::GetPosition(const CCTilePosition & tile)
{
    return CCPosition((float)tile.x, (float)tile.y);
}

float Util::Dist(const CCPosition & p1, const CCPosition & p2)
{
    return sqrtf((float)Util::DistSq(p1,p2));
}

float Util::Dist(const Unit & unit, const CCPosition & p2)
{
    return Dist(unit.getPosition(), p2);
}

float Util::Dist(const Unit & unit1, const Unit & unit2)
{
    return Dist(unit1.getPosition(), unit2.getPosition());
}

CCPositionType Util::DistSq(const CCPosition & p1, const CCPosition & p2)
{
    CCPositionType dx = p1.x - p2.x;
    CCPositionType dy = p1.y - p2.y;

    return dx*dx + dy*dy;
}

sc2::BuffID Util::GetBuffFromName(const std::string & name, CCBot & bot)
{
    for (const sc2::BuffData & data : bot.Observation()->GetBuffData())
    {
        if (name == data.name)
        {
            return data.buff_id;
        }
    }

    return 0;
}

sc2::AbilityID Util::GetAbilityFromName(const std::string & name, CCBot & bot)
{
    for (const sc2::AbilityData & data : bot.Observation()->GetAbilityData())
    {
        if (name == data.link_name)
        {
            return data.ability_id;
        }
    }

    return 0;
}

// checks where a given unit can make a given unit type now
// this is done by iterating its legal abilities for the build command to make the unit
bool Util::UnitCanMetaTypeNow(const Unit & unit, const UnitType & type, CCBot & m_bot)
{
    BOT_ASSERT(unit.isValid(), "Unit pointer was null");
    sc2::AvailableAbilities available_abilities = m_bot.Query()->GetAbilitiesForUnit(unit.getUnitPtr());
    
    // quick check if the unit can't do anything it certainly can't build the thing we want
    if (available_abilities.abilities.empty()) 
    {
        return false;
    }
    else 
    {
        // check to see if one of the unit's available abilities matches the build ability type
        sc2::AbilityID MetaTypeAbility = m_bot.Data(type).buildAbility;
        for (const sc2::AvailableAbility & available_ability : available_abilities.abilities) 
        {
            if (available_ability.ability_id == MetaTypeAbility)
            {
                return true;
            }
        }
    }
    return false;
}

std::vector<UnitType> Util::getEquivalentTypes(const UnitType unit, CCBot & m_bot)
{
	switch (unit.getAPIUnitType().ToType())
	{
		//Protoss
	case (sc2::UNIT_TYPEID::PROTOSS_GATEWAY):
		return { unit,UnitType(sc2::UNIT_TYPEID::PROTOSS_WARPGATE, m_bot) };
		//Terran
	case (sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER):
		return { unit, UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING, m_bot), UnitType(sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS, m_bot), UnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND, m_bot), UnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING, m_bot) };
	case (sc2::UNIT_TYPEID::TERRAN_BARRACKS):
		return  { unit, UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING, m_bot) };
	case (sc2::UNIT_TYPEID::TERRAN_FACTORY):
		return  { unit, UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORYFLYING, m_bot) };
	case (sc2::UNIT_TYPEID::TERRAN_STARPORT):
		return  { unit, UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORTFLYING, m_bot) };
	case (sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT):
		return  { unit, UnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED, m_bot) };
		//Zerg
	case (sc2::UNIT_TYPEID::ZERG_HATCHERY):
		return  { unit, UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) };
	case (sc2::UNIT_TYPEID::ZERG_LAIR):
		return  { unit,UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) };
	case (sc2::UNIT_TYPEID::ZERG_SPIRE):
		return  { unit,UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_bot) };
	}
	
	return { unit };
}

#ifndef LADDEREXE
std::string Util::ExePath()
{
	return ".";
}
#else
extern std::string fileName;
extern std::string filePath;
std::string Util::ExePath()
{
	return filePath + "\\Data\\" + fileName;
}
#endif
