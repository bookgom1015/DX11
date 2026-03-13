#include "pch.h"
#include "Light2DUI.h"

#include "CLight2D.h"

Light2DUI::Light2DUI() : ComponentUI(EComponent::E_Light2D, "Light2DUI") {}

Light2DUI::~Light2DUI() {}

void Light2DUI::Tick_UI() {
	OutputTitle("Light2D");

	Ptr<CLight2D> pLight2D = GetTarget()->Light2D();

	auto type = pLight2D->GetLightType();

	Vec3 vColor = pLight2D->GetLightColor();
	
	float intensity = pLight2D->GetIntensity();
	float radius = pLight2D->GetRadius();

	ImGui::Text("Color");
	ImGui::SameLine(100.f);
	if (ImGui::ColorPicker3("##LightColor", vColor)) 
		pLight2D->SetLightColor(vColor);

	if (type == ELight::E_Directional) {
		ImGui::Text("Ambient");
		ImGui::SameLine(100.f);

		Vec3 ambient = pLight2D->GetAmbient();
		if (ImGui::ColorPicker3("##AmbientColor", ambient))
			pLight2D->SetAmbient(ambient);
	}

	ImGui::Text("Intensity");
	ImGui::SameLine(100.f);
	if (ImGui::DragFloat("##LightIntensity", &intensity, 0.1f)) 
		pLight2D->SetIntensity(max(intensity, 1e-6f));

	ImGui::Text("Radius");
	ImGui::SameLine(100.f);
	if (ImGui::DragFloat("##LightRadius", &radius, 1.f))
		pLight2D->SetRadius(max(radius, 1e-6f));

	if (type == ELight::E_Spot) {
		ImGui::Text("Angle");
		ImGui::SameLine(100.f);

		float angle = pLight2D->GetAngle() * RadToDeg;
		if (ImGui::DragFloat("##LightAngle", &angle, 0.1f))
			pLight2D->SetAngle(min(max(angle, 10.f), 180.f - 1e-6f) * DegToRad);
	}
}