#pragma once
#include "CRenderComponent.h"

#include "AFlipbook.h"

class CFlipbookRender :
    public CRenderComponent
{
private:
    vector<Ptr<AFlipbook>>  m_vecFlipbook;

    int                     m_CurFlipbook;
    int                     m_CurSprite;

    int                     m_RepeatCount;  // -1 : 반복재생, 1 이상이면 재생 횟수
    bool                    m_Finish;
    float                   m_FPS;
    float                   m_AccTime;

    Vec4                    m_Albedo;

public:
    void SetFlipbook(int _Idx, Ptr<AFlipbook> _Flipbook)
    {
        if (m_vecFlipbook.size() <= _Idx)
            m_vecFlipbook.resize(_Idx + 1);
        m_vecFlipbook[_Idx] = _Flipbook;
    }

    void AddFlipbook(Ptr<AFlipbook> _Flipbook) { m_vecFlipbook.push_back(_Flipbook); }

    void Play(int _FlipbookIdx, float _FPS, int _RepeatCount) 
    { 
        m_CurFlipbook = _FlipbookIdx;        
        m_RepeatCount = _RepeatCount;
        m_FPS = _FPS;
    }
    void Stop(int index) {
        m_Finish = true;
        m_CurFlipbook = index;
        m_CurSprite = 0;
        m_RepeatCount = 0;
    }

    GET_SET(Vec4, Albedo);

    Ptr<ASprite> GetCurrentSprite();

private:
    bool CheckFinish();


public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void CreateMaterial() override;

public:
    CFlipbookRender();
    virtual ~CFlipbookRender();
};

