#pragma once

#include "ComponentUI.h"

class ScriptUI : public ComponentUI {
public:
    ScriptUI(SCRIPT_TYPE type);
    virtual ~ScriptUI();

public:
    virtual void SetTarget(Ptr<GameObject> _Object) override;
    virtual void Tick_UI() override;

private:
    void PropertyBool(CScript* const script, const Property& prop);
    void PropertyInt(CScript* const script, const Property& prop);
    void PropertyFloat(CScript* const script, const Property& prop);
    void PropertyVec2(CScript* const script, const Property& prop);
    void PropertyVec3(CScript* const script, const Property& prop);
    void PropertyObject(CScript* const script, const Property& prop);

private:
    const SCRIPT_TYPE m_ScriptType;
};