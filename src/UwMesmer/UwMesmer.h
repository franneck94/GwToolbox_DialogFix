#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include <ToolboxUIPlugin.h>

#include <GWCA/GameEntities/Agent.h>

#include "ActionsBase.h"
#include "ActionsUw.h"
#include "DataLivings.h"
#include "DataPlayer.h"
#include "UwMetadata.h"

#include <imgui.h>

class LtRoutine : public MesmerActionABC
{
public:
    LtRoutine(DataPlayer *p, MesmerSkillbarData *s, const AgentLivingData *a)
        : MesmerActionABC(p, "LtRoutine", s), livings_data(a){};

    RoutineState Routine() override;
    void Update() override;

private:
    void SkillPacketCallback(const uint32_t value_id,
                             const uint32_t caster_id,
                             const uint32_t target_id,
                             const uint32_t value,
                             const bool no_target);

    bool DoNeedVisage() const;
    bool RoutineSelfEnches() const;

public:
    const AgentLivingData *livings_data = nullptr;
    std::vector<GW::AgentLiving *> enemies_in_aggro = {};
    bool load_cb_triggered = false;

private:
    std::vector<GW::AgentLiving *> nightmares = {};
    std::vector<GW::AgentLiving *> aatxes = {};
    std::vector<GW::AgentLiving *> graspings = {};

    bool starting_active = false;
    long delay_ms = 200L;
};

class UwMesmer : public ToolboxUIPlugin
{
public:
    UwMesmer()
        : player_data({}), livings_data({}), filtered_livings({}), aatxe_livings({}), dryder_livings({}),
          skele_livings({}), skillbar({}), uw_metadata({}), lt_routine(&player_data, &skillbar, &livings_data)
    {
        if (skillbar.ValidateData())
            skillbar.Load();
    };
    ~UwMesmer(){};

    const char *Name() const override
    {
        return "UwMesmer";
    }

    const char *Icon() const override
    {
        return ICON_FA_COMMENT_DOTS;
    }

    void Initialize(ImGuiContext *, ImGuiAllocFns, HMODULE) override;
    void SignalTerminate() override;
    void Draw(IDirect3DDevice9 *) override;
    bool HasSettings() const override
    {
        return false;
    }
    void Update(float delta) override;

private:
    void DrawSplittedAgents(std::vector<GW::AgentLiving *> livings, const ImVec4 color, std::string_view label);

    DataPlayer player_data;
    AgentLivingData livings_data;
    MesmerSkillbarData skillbar;

    std::vector<GW::AgentLiving *> filtered_livings;
    std::vector<GW::AgentLiving *> aatxe_livings;
    std::vector<GW::AgentLiving *> dryder_livings;
    std::vector<GW::AgentLiving *> nightmare_livings;
    std::vector<GW::AgentLiving *> skele_livings;
    std::vector<GW::AgentLiving *> horseman_livings;
    std::vector<GW::AgentLiving *> keeper_livings;

    LtRoutine lt_routine;
    UwMetadata uw_metadata;
};
