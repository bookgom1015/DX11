#ifndef __COMPONENT_INL__
#define __COMPONENT_INL__

EComponent::Type Component::GetType() const { return m_Type; }

GameObject* Component::GetOwner() const { return m_Owner; }

#endif // __COMPONENT_INL__