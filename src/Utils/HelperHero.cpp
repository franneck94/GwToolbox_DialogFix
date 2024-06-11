#include <cstdint>
#include <functional>

#include <GWCA/Constants/Constants.h>
#include <GWCA/Constants/Maps.h>
#include <GWCA/Context/GameContext.h>
#include <GWCA/Context/WorldContext.h>
#include <GWCA/GWCA.h>
#include <GWCA/GameEntities/Agent.h>
#include <GWCA/GameEntities/Hero.h>
#include <GWCA/GameEntities/Map.h>
#include <GWCA/GameEntities/Party.h>
#include <GWCA/GameEntities/Player.h>
#include <GWCA/Managers/AgentMgr.h>
#include <GWCA/Managers/PartyMgr.h>
#include <GWCA/Managers/UIMgr.h>
#include <GWCA/Utilities/Scanner.h>

#include "DataHero.h"
#include "DataPlayer.h"
#include "HelperHero.h"

bool HeroUseSkill(const uint32_t target_agent_id, const uint32_t skill_idx, const uint32_t hero_idx_zero_based)
{
    auto hero_action = GW::UI::ControlAction_Hero1Skill1;
    if (hero_idx_zero_based == 0)
        hero_action = GW::UI::ControlAction_Hero1Skill1;
    else if (hero_idx_zero_based == 1)
        hero_action = GW::UI::ControlAction_Hero2Skill1;
    else if (hero_idx_zero_based == 2)
        hero_action = GW::UI::ControlAction_Hero3Skill1;
    else if (hero_idx_zero_based == 3)
        hero_action = GW::UI::ControlAction_Hero4Skill1;
    else if (hero_idx_zero_based == 4)
        hero_action = GW::UI::ControlAction_Hero5Skill1;
    else if (hero_idx_zero_based == 5)
        hero_action = GW::UI::ControlAction_Hero6Skill1;
    else if (hero_idx_zero_based == 6)
        hero_action = GW::UI::ControlAction_Hero7Skill1;
    else
        return false;

    const auto curr_target_id = GW::Agents::GetTargetId();
    auto success = true;

    GW::GameThread::Enqueue([=, &success] {
        if (target_agent_id && target_agent_id != GW::Agents::GetTargetId())
            success &= GW::Agents::ChangeTarget(target_agent_id);
        const auto keypress_id = (GW::UI::ControlAction)(static_cast<uint32_t>(hero_action) + skill_idx);
        success &= GW::UI::Keypress(keypress_id);
        if (curr_target_id && target_agent_id != curr_target_id)
            success &= GW::Agents::ChangeTarget(curr_target_id);
    });

    return success;
}

bool HeroCastSkillIfAvailable(const Hero &hero,
                              const DataPlayer &player_data,
                              const GW::Constants::SkillID skill_id,
                              std::function<bool(const DataPlayer &, const Hero &)> cb_fn,
                              const bool use_player_target)
{
    if (!hero.hero_living || !hero.hero_living->agent_id)
        return false;

    if (!cb_fn(player_data, hero))
        return false;

    const auto [skill_idx, can_cast_skill] = SkillIdxOfHero(hero, skill_id);
    const auto has_skill_in_skillbar = skill_idx != static_cast<uint32_t>(-1);

    if (has_skill_in_skillbar && can_cast_skill)
    {
        const auto valid_target = use_player_target && player_data.target && player_data.target->agent_id;
        const auto skill_target = valid_target ? player_data.target->agent_id : player_data.id;
        const auto success = HeroUseSkill(skill_target, skill_idx, hero.hero_idx_zero_based);
        return success;
    }

    return false;
}


std::tuple<uint32_t, bool> SkillIdxOfHero(const Hero &hero, const GW::Constants::SkillID skill_id)
{
    const auto hero_energy =
        static_cast<uint32_t>(hero.hero_living->energy * static_cast<float>(hero.hero_living->max_energy));

    auto skill_idx = 0U;
    for (const auto &skill : hero.skills)
    {
        const auto has_skill_in_skillbar = skill.skill_id == skill_id;
        if (!has_skill_in_skillbar)
        {
            ++skill_idx;
            continue;
        }

        const auto *skill_data = GW::SkillbarMgr::GetSkillConstantData(skill_id);
        if (!skill_data)
        {
            ++skill_idx;
            continue;
        }

        const auto can_cast_skill = skill.GetRecharge() == 0 && hero_energy >= skill_data->GetEnergyCost();
        return std::make_tuple(skill_idx, can_cast_skill);
    }

    return std::make_tuple(static_cast<uint32_t>(-1), false);
}
