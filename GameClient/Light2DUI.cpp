#include "pch.h"
#include "Light2DUI.h"

#include "CLight2D.h"

Light2DUI::Light2DUI() : ComponentUI(EComponent::E_Light2D, "Light2DUI") {}

Light2DUI::~Light2DUI() {}

void Light2DUI::Tick_UI() {
	OutputTitle("Light2D");

	Ptr<CLight2D> pLight2D = GetTarget()->Light2D();

	Vec3 vColor = pLight2D->GetLightColor();
	
	float intensity = pLight2D->GetIntensity();
	float radius = pLight2D->GetRadius();

	ImGui::Text("Color");
	ImGui::SameLine(100.f);
	if (ImGui::ColorEdit3("##LightColor", vColor)) {
		pLight2D->SetLightColor(vColor);
	}

	ImGui::Text("Intensity");
	ImGui::SameLine(100.f);
	if (ImGui::DragFloat("##LightIntensity", &intensity)) {
		pLight2D->SetIntensity(intensity);
	}

	ImGui::Text("Radius");
	ImGui::SameLine(100.f);
	if (ImGui::DragFloat("##LightRadius", &radius)) {
		pLight2D->SetRadius(radius);
	}
}