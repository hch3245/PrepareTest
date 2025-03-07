#pragma once
#include <Windows.h>

#include "Utils.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"


const char* primitives[] = { "Sphere", "Cube", "Triangle" };
int currentPrimitive = 0;
int spawnNum = 1;
char spawnNumStr[16] = "";
char sceneName[128] = "Default";

float fovValue = 53.f;
char fovStr[32] = "";

FVector3 cameraLocation = { -1.037f, 0.716f, 0.330f };
FVector3 cameraRotation = { 0.680f, 1.970f, 0.000f };

FVector3 objectTranslation = { -0.140f, 0.030f, 0.000f };
FVector3 objectRotation = { 0.0f, 0.0f, 0.0f };
FVector3 objectScale = { 1.0f, 1.0f, 1.0f };


char numToStr[32] = "";
char strOut[32] = "";

void centerStr(int blankNum, int num) {
    sprintf_s(numToStr, sizeof(numToStr), "%d", num);

    // 자리수 계산
    int digitCount = strlen(numToStr);
    int paddingSize = max(0, blankNum - digitCount / 2);

    // 최종 문자열을 저장할 버퍼
    memset(strOut, ' ', paddingSize); // 공백 추가
    strOut[paddingSize] = '\0'; // 공백 문자열 종료

    // 숫자 문자열 추가
    strcat_s(strOut, sizeof(strOut), numToStr);
}

void centerStr(int blankNum, float num) {
    sprintf_s(numToStr, sizeof(numToStr), "%.3f", num);

    // 자리수 계산
    int digitCount = strlen(numToStr);
    int paddingSize = max(0, blankNum - digitCount / 2);

    // 최종 문자열을 저장할 버퍼
    memset(strOut, ' ', paddingSize); // 공백 추가
    strOut[paddingSize] = '\0'; // 공백 문자열 종료

    // 숫자 문자열 추가
    strcat_s(strOut, sizeof(strOut), numToStr);
}



void ShowExampleControlPanel() {
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    ImGui::Begin("Jungle Control Panel");
    ImGui::Text("Hello Jungle World!");
    int fps = 7777;
    int deltaTime = 1;
    ImGui::Text("FPS %d (%d ms)", fps, deltaTime);
    ImGui::Separator();

    // 위쪽에 선언함
    ImGui::PushItemWidth(200);
    ImGui::Combo("Primitive", &currentPrimitive, primitives, IM_ARRAYSIZE(primitives));
    ImGui::PopItemWidth();
    ImGui::Button("Spawn");
    ImGui::SameLine();

    if (spawnNum < 1) spawnNum = 1;
    // 숫자를 문자열로 변환
    centerStr(10, spawnNum);

    // 숫자 문자열 추가

    ImGui::PushItemWidth(148);
    ImGui::InputText("Number of spawn", strOut, IM_ARRAYSIZE(strOut));
    spawnNum = atoi(strOut);
    ImGui::PopItemWidth();
    ImGui::Separator();

    ImGui::PushItemWidth(200);
    ImGui::InputText("Scene Name", sceneName, IM_ARRAYSIZE(sceneName));
    ImGui::PopItemWidth();
    ImGui::Button("New Scene");
    ImGui::Button("Save Scene");
    ImGui::Button("Load Scene");

    ImGui::Separator();

    bool bOrthogonal = false;
    ImGui::Checkbox("Orthogonal", &bOrthogonal);


    centerStr(13, fovValue);



    ImGui::PushItemWidth(200);
    ImGui::InputText("FOV", strOut, IM_ARRAYSIZE(strOut));
    fovValue = atof(strOut);
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(67);
    centerStr(4, cameraLocation.x);
    ImGui::InputText("##cameraLocationX", strOut, IM_ARRAYSIZE(strOut));
    cameraLocation.x = atof(strOut);
    ImGui::SameLine();
    centerStr(4, cameraLocation.y);
    ImGui::InputText("##cameraLocationY", strOut, IM_ARRAYSIZE(strOut));
    cameraLocation.y = atof(strOut);
    ImGui::SameLine();
    centerStr(4, cameraLocation.z);
    ImGui::InputText("##cameraLocationZ", strOut, IM_ARRAYSIZE(strOut));
    cameraLocation.z = atof(strOut);
    ImGui::SameLine();
    ImGui::Text("Camera Location");
    ImGui::PopItemWidth();


    ImGui::PushItemWidth(67);
    centerStr(4, cameraRotation.x);;
    ImGui::InputText("##cameraRotationX", strOut, IM_ARRAYSIZE(strOut));
    cameraRotation.x = atof(strOut);
    ImGui::SameLine();
    centerStr(4, cameraRotation.y);
    ImGui::InputText("##cameraRotationY", strOut, IM_ARRAYSIZE(strOut));
    cameraRotation.y = atof(strOut);
    ImGui::SameLine();
    centerStr(4, cameraRotation.z);
    ImGui::InputText("##cameraRotationZ", strOut, IM_ARRAYSIZE(strOut));
    cameraRotation.z = atof(strOut);
    ImGui::SameLine();
    ImGui::Text("Camera Rotation");
    ImGui::PopItemWidth();

    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("Select Gizmo Mode");
    ImGui::Button("Translation", ImVec2(105, 25));
    ImGui::SameLine();
    ImGui::Button("Rotation", ImVec2(105, 25));
    ImGui::SameLine();
    ImGui::Button("Scale", ImVec2(105, 25));
    ImGui::SameLine();


    ImGui::End();

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}