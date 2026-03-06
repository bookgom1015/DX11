#pragma once

#include "GameObject.h"

#include "EditorUI.h"

class ComponentUI : public EditorUI {
public:
	ComponentUI(EComponent::Type _Type, const string& _Name);
	virtual ~ComponentUI();

protected:
	void OutputTitle(const string& _Title);

public:
	void SetTarget(Ptr<GameObject> _Object);	
	__forceinline Ptr<GameObject> GetTarget() const;

private:
	Ptr<GameObject> m_Target;
	const EComponent::Type m_ComType;
};

#include "ComponentUI.inl"

