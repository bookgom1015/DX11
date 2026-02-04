#pragma once
#include "Asset.h"

#include "Layer.h"

class ALevel :
    public Asset
{
private:
    Layer       m_arrLayer[MAX_LAYER]; // 하나의 레벨안에 총 32개의 레이어가 존재
    UINT	    m_Matrix[MAX_LAYER];    // 어떤 레이어와, 어떤 레이어가 충돌검사를 진행할지 마킹한 데이터


public:
    void AddObject(int _LayerIdx, Ptr<GameObject> _Object);
    Layer* GetLayer(int _Idx) { assert(0 <= _Idx && _Idx < MAX_LAYER); return &m_arrLayer[_Idx]; }
    void Deregister();

    void CheckCollisionLayer(UINT _LayerIdx1, UINT _LayerIdx2);
    void CheckCollisionLayer(const wstring& _LayerName1, const wstring& _LayerName2);

    UINT* GetCollisionMatrix() { return m_Matrix; }


public:
    void Begin();
    void Tick();
    void FinalTick();
    

public:
    ALevel();
    virtual ~ALevel();
};

