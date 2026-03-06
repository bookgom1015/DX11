#include "pch.h"
#include "RigidbodyUI.h"

RigidbodyUI::RigidbodyUI() : ComponentUI(EComponent::E_Rigidbody, "RigidbodyUI") {}

RigidbodyUI::~RigidbodyUI() {}

void RigidbodyUI::Tick_UI() {
	OutputTitle("Rigidbody");

	Vec3 vel = GetTarget()->RigidBody()->GetVelocity();

	ImGui::Text("Velocity");
	ImGui::SameLine(100);
	if (ImGui::DragFloat3("##Velocity", vel))
		GetTarget()->Transform()->SetRelativePos(vel);
}