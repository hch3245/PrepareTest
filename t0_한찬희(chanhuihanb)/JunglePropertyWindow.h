#pragma once
#include <Windows.h>

#include "Utils.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include "JungleControlPanel.h"

FVector3 objectTranslation = { -0.140f, 0.030f, 0.0f };
FVector3 objectRotation = { 0.0f, 0.0f, 0.0f };
FVector3 objectScale = { 1.0f, 1.0f, 1.0f };


void ShowExamplePropertyWindow() {
	ImGui::Begin("Jungle Property Window ");

    ImGui::PushItemWidth(67);
    centerStr(4, objectTranslation.x);
    ImGui::InputText("##objectTranslationX", strOut, IM_ARRAYSIZE(strOut));
    cameraRotation.x = atof(strOut);
    ImGui::SameLine();
    centerStr(4, objectTranslation.y);
    ImGui::InputText("##objectTranslationY", strOut, IM_ARRAYSIZE(strOut));
    cameraRotation.y = atof(strOut);
    ImGui::SameLine();
    centerStr(4, objectTranslation.z);
    ImGui::InputText("##objectTranslationZ", strOut, IM_ARRAYSIZE(strOut));
    cameraRotation.z = atof(strOut);
    ImGui::SameLine();
    ImGui::Text("Translation");
    ImGui::PopItemWidth();


    ImGui::PushItemWidth(67);
    centerStr(4, objectRotation.x);
    ImGui::InputText("##objectRotationX", strOut, IM_ARRAYSIZE(strOut));
    cameraRotation.x = atof(strOut);
    ImGui::SameLine();
    centerStr(4, objectRotation.y);
    ImGui::InputText("##objectRotationY", strOut, IM_ARRAYSIZE(strOut));
    cameraRotation.y = atof(strOut);
    ImGui::SameLine();
    centerStr(4, objectRotation.z);
    ImGui::InputText("##objectRotationZ", strOut, IM_ARRAYSIZE(strOut));
    cameraRotation.z = atof(strOut);
    ImGui::SameLine();
    ImGui::Text("Rotation");
    ImGui::PopItemWidth();


    ImGui::PushItemWidth(67);
    centerStr(4, objectScale.x);
    ImGui::InputText("##objectScaleX", strOut, IM_ARRAYSIZE(strOut));
    cameraRotation.x = atof(strOut);
    ImGui::SameLine();
    centerStr(4, objectScale.y);
    ImGui::InputText("##objectScaleY", strOut, IM_ARRAYSIZE(strOut));
    cameraRotation.y = atof(strOut);
    ImGui::SameLine();
    centerStr(4, objectScale.z);
    ImGui::InputText("##objectScaleZ", strOut, IM_ARRAYSIZE(strOut));
    cameraRotation.z = atof(strOut);
    ImGui::SameLine();
    ImGui::Text("Scale");
    ImGui::PopItemWidth();


	ImGui::End();
}