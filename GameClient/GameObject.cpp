#include "pch.h"
#include "GameObject.h"

#include "LevelMgr.h"
#include "TaskMgr.h"
#include "Source/ScriptMgr.h"

GameObject::GameObject()
	: m_Com{}
	, m_Parent{}
	, m_LayerIdx{ -1 }
	, m_Dead{} {}

GameObject::GameObject(const GameObject& _Origin)
	: Entity(_Origin)
	, m_Com{}
	, m_Parent{}
	, m_LayerIdx{ -1 }
	, m_Dead{} {
	for (UINT i = 0; i < EComponent::Count; ++i) {
		if (_Origin.m_Com[i] == nullptr) continue;
		AddComponent(_Origin.m_Com[i]->Clone());
	}
	for (const auto& script : _Origin.m_vecScripts) {
		AddComponent(script->Clone());
	}
	for (const auto& child : _Origin.m_vecChild) {
		AddChild(child->Clone());
	}
}

GameObject::~GameObject() {}

void GameObject::Begin() {
	for (size_t i = 0, end = m_vecScripts.size(); i < end; ++i)
		m_vecScripts[i]->Begin();

	for (UINT i = 0; i < EComponent::Count; ++i)
		if (nullptr != m_Com[i]) m_Com[i]->Begin();

	for (size_t i = 0, end = m_vecChild.size(); i < end; ++i)
		m_vecChild[i]->Begin();
}

void GameObject::Tick() {
	for (size_t i = 0, end = m_vecScripts.size(); i < end; ++i)
		if (m_vecScripts[i]->GetFirstTick()) m_vecScripts[i]->TickOnce();

	for (size_t i = 0, end = m_vecScripts.size(); i < end; ++i)
		m_vecScripts[i]->Tick();
	
	for (size_t i = 0, end = m_vecChild.size(); i < end; ++i)
		m_vecChild[i]->Tick();
}

void GameObject::LateTick() {
	for (size_t i = 0, end = m_vecScripts.size(); i < end; ++i)
		m_vecScripts[i]->LateTick();

	for (size_t i = 0, end = m_vecChild.size(); i < end; ++i)
		m_vecChild[i]->LateTick();

	for (UINT i = 0; i < EComponent::Count; ++i)
		if (nullptr != m_Com[i]) m_Com[i]->LateTick();
}

void GameObject::FinalTick() {
	for (UINT i = 0; i < EComponent::Count; ++i)
		if (nullptr != m_Com[i]) m_Com[i]->FinalTick();

	// 자신이 소속된 Layer 에 자기자신을 알림(등록)
	RegisterLayer();

	// 자식 오브젝트 FinalTick 호출
	// 만약 Dead 상태인 자식 오브젝트가 있으면, Vector 에서 제거한다.
	auto iter = m_vecChild.begin();
	for (; iter != m_vecChild.end();) {
		(*iter)->FinalTick();

		if ((*iter)->IsDead()) iter = m_vecChild.erase(iter);
		else ++iter;
	}
}

void GameObject::FinalTick_Editor() {
	for (UINT i = 0; i < EComponent::Count; ++i) {
		if (nullptr != m_Com[i]) m_Com[i]->FinalTick();
	}

	// 자식 오브젝트 FinalTick 호출
	// 만약 Dead 상태인 자식 오브젝트가 있으면, Vector 에서 제거한다.
	vector<Ptr<GameObject>>::iterator iter = m_vecChild.begin();
	for (; iter != m_vecChild.end(); ) {
		(*iter)->FinalTick();

		if ((*iter)->IsDead()) iter = m_vecChild.erase(iter);
		else ++iter;
	}
}

void GameObject::RegisterLayer() {
	Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
	Layer* pLayer = pCurLevel->GetLayer(m_LayerIdx);
	pLayer->RegisterObject(this);
}

void GameObject::Render() {
	// 렌더링 관련 기능을 보유한 컴포넌트가 없으면 GameObejct 는 Rendering 될 수 없다.
	if (m_RenderCom != nullptr) {
		Transform()->Binding();
		m_RenderCom->Render();
	}

	for (size_t i = 0, end = m_vecChild.size(); i < end; ++i)
		m_vecChild[i]->Render();
}

void GameObject::SaveToLevelFile(FILE* const _FileStream) {
	auto objectName = GetName();
	SaveWString(_FileStream, objectName);

	// 컴포넌트 
	for (UINT i = 0; i < EComponent::Count; ++i) {
		decltype(auto) comp = m_Com[i];
		if (comp == nullptr) continue;

		// 컴포넌트 타입
		fwrite(&i, sizeof(i), 1, _FileStream);

		// 컴포넌트 내용
		comp->SaveToLevelFile(_FileStream);
	}

	UINT end = EComponent::Count;
	fwrite(&end, sizeof(end), 1, _FileStream);

	// 스크립트
	auto size = m_vecScripts.size();
	fwrite(&size, sizeof(size), 1, _FileStream);

	for (const auto& script : m_vecScripts) {
		auto scriptName = ScriptMgr::GetScriptName(script.Get());
		SaveWString(_FileStream, scriptName);

		script->SaveToLevelFile(_FileStream);
	}

	// 자식 객체
	size_t numChildren = m_vecChild.size();
	fwrite(&numChildren, sizeof(numChildren), 1, _FileStream);

	for (const auto& child : m_vecChild) 
		child->SaveToLevelFile(_FileStream);
}

void GameObject::LoadFromLevelFile(FILE* const _FileStream) {
	auto objectName = LoadWString(_FileStream);
	SetName(objectName);	

	// 컴포너트
	while (true) {
		UINT comType{};
		fread(&comType, sizeof(comType), 1, _FileStream);
		if (comType == EComponent::Count) break;

		Component* component{};
		switch (comType) {
		case EComponent::E_Transform: 
			component = NEW CTransform;
			break;
		case EComponent::E_Camera: 
			component = NEW CCamera;
			break;
		case EComponent::E_Collider2D: 
			component = NEW CCollider2D;
			break;
		case EComponent::E_Collider3D: 
			break;
		case EComponent::E_Light2D: 
			component = NEW CLight2D;
			break;
		case EComponent::E_Light3D: 
			break;
		case EComponent::E_MeshRender: 
			component = NEW CMeshRender;
			break;
		case EComponent::E_BillboardRender: 
			component = NEW CBillboardRender;
			break;
		case EComponent::E_SpriteRender: 
			component = NEW CSpriteRender;
			break;
		case EComponent::E_FlipbookRender:
			component = NEW CFlipbookRender;
			break;
		case EComponent::E_ParticleRender: 			
			break;
		case EComponent::E_TileRender: 
			component = NEW CTileRender;
			break;
		case EComponent::E_Rigidbody: 
			component = NEW CRigidBody;
			break;
		}

		AddComponent(component);
		component->LoadFromLevelFile(_FileStream);
	}

	// 스크립트
	size_t numScripts{};
	fread(&numScripts, sizeof(numScripts), 1, _FileStream);

	for (size_t i = 0; i < numScripts; ++i) {
		auto scriptName = LoadWString(_FileStream);
		Ptr<CScript> script = ScriptMgr::GetScript(scriptName);
		AddComponent(script.Get());

		script->LoadFromLevelFile(_FileStream);
	}

	// 자식 객체
	size_t numChildren{};
	fread(&numChildren, sizeof(numChildren), 1, _FileStream);

	for (size_t i = 0; i < numChildren; ++i) {
		Ptr<GameObject> child = NEW GameObject;
		AddChild(child);
		child->LoadFromLevelFile(_FileStream);
	}
}

void GameObject::AddComponent(Ptr<Component> _Com) {
	// 렌더링 기능 컴포넌트는 하나만 가질 수 있음
	if (dynamic_cast<CRenderComponent*>(_Com.Get())) {
		assert(!m_RenderCom.Get());

		m_RenderCom = (CRenderComponent*)_Com.Get();
	}
	
	// 입력으로 들어온 컴포넌트가 스크립트면, vector 로 관리
	if (_Com->GetType() == EComponent::E_Script) {
		m_vecScripts.push_back((CScript*)_Com.Get());
	}
	// 입력으로 들어온 컴포넌트가 스크립트가 아니면, 알맞은 배열 포인터로 가리킴
	else {
		// 해당 컴포넌트를 이미 가지고 있지 않아야 한다.
		assert(nullptr == m_Com[(UINT)_Com->GetType()]);
		m_Com[_Com->GetType()] = _Com;
	}
		
	_Com->m_Owner = this;
	_Com->Init();
}

void GameObject::RemoveComponent(EComponent::Type _Type, SCRIPT_TYPE _ScriptType) {
	if (_Type == EComponent::E_Script) {
		const auto begin = m_vecScripts.begin();
		const auto end = m_vecScripts.end();
		const auto iter = std::find_if(begin, end, [&, _ScriptType](Ptr<CScript>& s) {
			return s->GetScriptType() == _ScriptType;
		});
		assert(iter != end);

		iter->Get()->CleanUp();

		std::iter_swap(iter, end - 1);
		m_vecScripts.pop_back();
	}
	else {
		auto& comp = m_Com[_Type];
		assert(comp != nullptr);

		comp = nullptr;
	}
}

void GameObject::AddChild(Ptr<GameObject> _Child) {
	// 부모 오브젝트가 있는지 확인
	if (_Child->GetParent().Get()) {
		// 기존 부모 오브젝트와 관계를 해제한다.
		_Child->DisconnectWithParent();
	}
	// 최상위 부모 오브젝트 였다면
	else {
		// 레벨 내부에 있던 오브젝트 라면
		if (_Child->m_LayerIdx != -1) {
			// Layer 에서 최상위 부모로 가리키던 포인터를 제거
			_Child->DeregisterAsParent();
		}		
	}

	m_vecChild.push_back(_Child); 
	_Child->m_Parent = this;

	if (_Child->m_LayerIdx == -1) {
		_Child->m_LayerIdx = m_LayerIdx;

		if (m_LayerIdx != -1 
			&& LevelMgr::GetInst()->GetLevelState() == ELevelState::E_Playing)
			_Child->Begin();
	}

	// 부모 오브젝트가 레벨 소속이면
	if (m_LayerIdx != -1) {
		// 현재 레벨에 변경이 발생했음을 알림
		LevelMgr::GetInst()->GetCurLevel()->SetChanged();
	}
}

void GameObject::DisconnectWithParent() {
	if (!m_Parent) return;

	if (m_LayerIdx != -1) LevelMgr::GetInst()->GetCurLevel()->SetChanged();

	auto iter = m_Parent->m_vecChild.begin();
	for (; iter != m_Parent->m_vecChild.end(); ++iter) {
		if (*iter == this) {
			m_Parent->m_vecChild.erase(iter);
			m_Parent = nullptr;

			return;
		}		
	}

	assert(nullptr);
}

void GameObject::RegisterAsParent() {
	if (m_LayerIdx == -1) return;

	LevelMgr::GetInst()->GetCurLevel()->GetLayer(m_LayerIdx)->AddObject(this);
}

void GameObject::DeregisterAsParent() {
	Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
	Layer* pLayer = pCurLevel->GetLayer(m_LayerIdx);
	pLayer->DeregisterAsParent(this);		
}

void GameObject::Destroy() {
	if (m_Dead) return;

	TaskInfo info{};
	info.Type = ETask::E_DestroyObject;
	info.Param_0 = (DWORD_PTR)this;

	TaskMgr::GetInst()->AddTask(info);
}
