#pragma once

#include "Common.h"
#include "MicroManager.h"
#include <utility>

class CCBot;

class RangedManager: public MicroManager
{
public:

    RangedManager(CCBot & bot);
    void    executeMicro(const std::vector<Unit> & targets);
    void    assignTargets(const std::vector<Unit> & targets);
    int     getAttackPriority(const Unit & rangedUnit, const Unit & target);
    std::pair<Unit, float>    getTarget(const Unit & rangedUnit, const std::vector<Unit> & targets);
};
