#include "pch.h"
#include "EditorMgr.h"

#include "Engine.h"
#include "Device.h"
#include "KeyMgr.h"
#include "RenderMgr.h"
#include "LevelMgr.h"

#include "Menu.h"
#include "Inspector.h"
#include "Outliner.h"
#include "ContentUI.h"
#include "ListUI.h"
#include "TreeUI.h"
#include "Profiler.h"
#include "AtlasEditorUI.h"
#include "TileMapEditorUI.h"

#include "CCamMoveScript.h"

EditorMgr::EditorMgr() : m_ShowDemo(false) {}

EditorMgr::~EditorMgr() {
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void EditorMgr::Init() {
    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;

    io.Fonts->AddFontFromFileTTF(
        "C:\\Windows\\Fonts\\malgun.ttf",
        18.f,
        NULL, 
        io.Fonts->GetGlyphRangesKorean());

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
    io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(Engine::GetInst()->GetMainWndHwnd());
    ImGui_ImplDX11_Init(DEVICE, CONTEXT);

    // 게임 에디터 UI 만들기
    CreateEditorUI();

    // Editor 용 GameObject 만들기
    CreateEditorObject();
}

void EditorMgr::Progress() {
    Tick();
    Render();
}

float EditorMgr::CalcItemSize(string_view text) {
    float spacing = ImGui::GetStyle().ItemSpacing.x;
    float padding = ImGui::GetStyle().FramePadding.x;
    return ImGui::CalcTextSize(text.data()).x + padding * 2.f + spacing;
}

void EditorMgr::RightAlignNextItem(const initializer_list<string_view>& text) {
    float spacing = ImGui::GetStyle().ItemSpacing.x;
    float padding = ImGui::GetStyle().FramePadding.x;

    float totalWidth{};
    for (const auto& txt : text) {
        totalWidth += ImGui::CalcTextSize(txt.data()).x + padding * 2.0f;
        totalWidth += spacing;
    }

    totalWidth -= spacing;

    // 오른쪽으로 커서 이동
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - totalWidth + ImGui::GetCursorPosX());
}

void EditorMgr::AcceptAssetDragDrop(
    string_view _Sender
    , EAsset::Type _Type
    , const std::function<void(Ptr<Asset>)>& func) {
    decltype(auto) PayLoad = ImGui::AcceptDragDropPayload(_Sender.data());
    if (PayLoad) {
        auto data = *(static_cast<DWORD_PTR*>(PayLoad->Data));
        Ptr<Asset> asset = reinterpret_cast<Asset*>(data);

        if (asset->GetType() == _Type) func(asset);
    }
}

Vec2 EditorMgr::GetCursorScreenPos() {
    return Vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
}

Vec2 EditorMgr::GetItemRectSize() {
    return Vec2(ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y);
}

Vec2 EditorMgr::GetItemRectMin() {
    return Vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
}

Vec2 EditorMgr::GetItemRectMax() {
    return Vec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
}

void EditorMgr::Tick() {
    // =============
    // Editor Object
    // =============
    if (LevelMgr::GetInst()->GetLevelState() != ELevelState::E_Playing) {
        for (const auto& Object : m_EditorObject)
            Object->Tick();

        for (const auto& Object : m_EditorObject)
            Object->FinalTick_Editor();
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
       
    ImGui::DockSpaceOverViewport(
        0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    m_FocusedUI = nullptr;

    if (KEY_TAP(KEY::ENTER))
        ImGui::SetWindowFocus(nullptr);

    // DemoUI
    if (m_ShowDemo)
        ImGui::ShowDemoWindow(&m_ShowDemo);

    // EditorUI
    for (const auto& pair : m_mapUI) {
        if(pair.second->IsActive())
            pair.second->Tick();
    }
        
    if (nullptr != m_FocusedUI && m_FocusedUI->GetUIName() != "Scene")
        KeyMgr::GetInst()->SetActive(false);
    else
        KeyMgr::GetInst()->SetActive(true);
}

void EditorMgr::Render() {
    auto rtv = Device::GetInst()->GetRenderTargetView();
    FLOAT clearValues[4] = { 0.1f, 0.1f, 0.1f, 1.f };
    CONTEXT->ClearRenderTargetView(rtv, clearValues);
    CONTEXT->OMSetRenderTargets(1, &rtv, nullptr);

    // Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void EditorMgr::CreateEditorUI() {
    Ptr<EditorUI> pUI{};

    pUI = NEW Menu;
    AddUI(pUI->GetUIName(), pUI);

    m_SceneUI = NEW SceneUI;
    pUI = m_SceneUI.Get();
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW Inspector;
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW Outliner;
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW ContentUI;
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW Profiler;
    AddUI(pUI->GetUIName(), pUI);

    m_LogUI = NEW LogUI;
    pUI = m_LogUI.Get();
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW AtlasEditorUI;
    AddUI(pUI->GetUIName(), pUI);

    pUI = NEW TileMapEditorUI;
    AddUI(pUI->GetUIName(), pUI);
    
    pUI = NEW ListUI;
    pUI->SetModal(true);
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);
}

void EditorMgr::CreateEditorObject() {
    // Editor Camera Object 생성
    Ptr<GameObject> pObject = NEW GameObject;
    pObject->SetName(L"EditorCamera");

    pObject->AddComponent(NEW CTransform);
    pObject->AddComponent(NEW CCamera);
    pObject->AddComponent(NEW CCamMoveScript);

    pObject->Camera()->LayerCheckAll();

    pObject->Camera()->SetProjType(EProjection::E_Orthographic);
    pObject->Camera()->SetFar(10000.f);
    pObject->Camera()->SetFOV(90.f);
    pObject->Camera()->SetOrthoScale(1.f);

    Vec2 vResolution = Device::GetInst()->GetRenderResolution();
    pObject->Camera()->SetAspectRatio(vResolution.x / vResolution.y); // 종횡비(AspectRatio)
    pObject->Camera()->SetWidth(vResolution.x);

    m_EditorObject.push_back(pObject);

    // Editor 용 카메라로서 RenderMgr 에 등록
    RenderMgr::GetInst()->RegisterEditorCamera(pObject->Camera());
}

void EditorMgr::AddUI(const string& _UIName, Ptr<EditorUI> _UI) {
    Ptr<EditorUI> pUI = FindUI(_UIName);
    assert(nullptr == pUI);
    m_mapUI.insert(make_pair(_UIName, _UI));
}

Ptr<EditorUI> EditorMgr::FindUI(const string& _UIName) {
    map<string, Ptr<EditorUI>>::iterator iter = m_mapUI.find(_UIName);

    if (iter == m_mapUI.end())
        return nullptr;

    return iter->second;
}

void EditorMgr::AddLog(const LogEntry& entry) {
    m_LogUI->AddLog(entry);
}

void EditorMgr::AddInfoLog(const std::string& msg) {
    auto entry = LogEntry{
        .Level = LogLevel::E_Info,
        .Message = msg };
    m_LogUI->AddLog(entry);
}