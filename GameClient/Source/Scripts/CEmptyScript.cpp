#include "pch.h"
#include "Source/Scripts/CEmptyScript.h"

CEmptyScript::CEmptyScript() : CScript(SCRIPT_TYPE::EMPTYSCRIPT) {
	ADD_PROPERTY(CEmptyScript, "Object", m_Object, Property::Object);
	ADD_PROPERTY(CEmptyScript, "Float", m_Float, Property::Float);
	ADD_PROPERTY(CEmptyScript, "Vec2", m_Vec2, Property::Vec2);
	ADD_PROPERTY(CEmptyScript, "Vec3", m_Vec3, Property::Vec3);
}

CEmptyScript::~CEmptyScript() {}

void CEmptyScript::Tick() {}