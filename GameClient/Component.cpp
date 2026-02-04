#include "pch.h"
#include "Component.h"

#include "GameObject.h"


#define GET_OTHER_COMPONENT_BODY(COM_NAME) C##COM_NAME* Component::COM_NAME() { return GetOwner()->COM_NAME().Get(); }


Component::Component(COMPONENT_TYPE _Type)
	: m_Owner(nullptr)
	, m_Type(_Type)
{
}

Component::~Component()
{
}

GET_OTHER_COMPONENT_BODY(Transform);
GET_OTHER_COMPONENT_BODY(MeshRender);
GET_OTHER_COMPONENT_BODY(BillboardRender);
GET_OTHER_COMPONENT_BODY(Camera);
GET_OTHER_COMPONENT_BODY(Collider2D);
GET_OTHER_COMPONENT_BODY(RigidBody);

GET_OTHER_COMPONENT_BODY(SpriteRender);
