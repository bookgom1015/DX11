#ifndef __CFLIPBOOKRENDERER_INL__
#define __CFLIPBOOKRENDERER_INL__

void CFlipbookRender::SetFlipbook(int _Idx, Ptr<AFlipbook> _Flipbook) {
    if (m_vecFlipbook.size() <= _Idx) m_vecFlipbook.resize(_Idx + 1);
    m_vecFlipbook[_Idx] = _Flipbook;
}

void CFlipbookRender::AddFlipbook(Ptr<AFlipbook> _Flipbook) { 
    m_vecFlipbook.push_back(_Flipbook); 
}

void CFlipbookRender::Play(int _FlipbookIdx, float _FPS, int _RepeatCount) {
    m_CurFlipbook = _FlipbookIdx;
    m_RepeatCount = _RepeatCount;
    m_FPS = _FPS;
}

void CFlipbookRender::Stop() {
    m_Finish = true;
    m_CurFlipbook = 3;
    m_CurSprite = 0;
    m_RepeatCount = 0;
}

void CFlipbookRender::SetMotionBlurEffect(bool status) {
    m_MotionBlurEnabled = status;
}

#endif // __CFLIPBOOKRENDERER_INL__