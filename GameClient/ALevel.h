#pragma once

#include "Asset.h"
#include "Layer.h"

class ALevel : public Asset {
public:
    ALevel();
    virtual ~ALevel();

public:
    void Begin();
    void Tick();
    void LateTick();
    void FinalTick();

    virtual int Save(const wstring& _FilePath) override;
    virtual int Load(const wstring& _FilePath) override;

public:
    void AddObject(int _LayerIdx, Ptr<GameObject> _Object);
    Layer* GetLayer(int _Idx) { assert(0 <= _Idx && _Idx < MAX_LAYER); return &m_arrLayer[_Idx]; }
    void Deregister();
    void CheckCollisionLayer(UINT _LayerIdx1, UINT _LayerIdx2);
    void CheckCollisionLayer(const wstring& _LayerName1, const wstring& _LayerName2);
    UINT* GetCollisionMatrix() { return m_Matrix; }
    Ptr<GameObject> FindObjectByName(const wstring& _Name);

    bool IsChanged() {
        bool Changed = m_Changed;
        m_Changed = false;

        return Changed;
    }

    void SetChanged() { m_Changed = true; }

    ALevel* Clone() { return NEW ALevel(*this); }

private:
    // 하나의 레벨안에 총 32개의 레이어가 존재
    Layer m_arrLayer[MAX_LAYER];
    // 어떤 레이어와, 어떤 레이어가 충돌검사를 진행할지 마킹한 데이터
    UINT m_Matrix[MAX_LAYER];
    // 레벨 안에 오브젝트들의 상태가 변경(오브젝트 추가, 삭제, 계층구조 변경) 
    // 됐는지 확인하는 변수
    bool m_Changed;
};

