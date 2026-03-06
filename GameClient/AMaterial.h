#pragma once
#include "Asset.h"

#include "AGraphicShader.h"
#include "ATexture.h"

class AMaterial : public Asset {
public:
    AMaterial();
    AMaterial(const AMaterial& _Other);
    virtual ~AMaterial();

public:
    void Binding();
    void Clear();

    virtual int Save(const wstring& _FilePath) override;
    virtual int Load(const wstring& _FilePath) override;
    AMaterial* Clone();

public:
    GET_SET(ERenderDomain::Type, Domain);

    __forceinline void SetShader(Ptr<AGraphicShader> _Shader);
    __forceinline Ptr<AGraphicShader> GetShader() const;

    __forceinline void SetTexture(TEX_PARAM _Param, Ptr<ATexture> _Texture);
    __forceinline Ptr<ATexture> GetTexture(TEX_PARAM _Param) const;

    template<typename T>
    void SetScalar(SCALAR_PARAM _Param, const T& _Data);
    template<typename T>
    T& GetScalar(SCALAR_PARAM _Param);

private:
    Ptr<AGraphicShader>     m_Shader;
    Ptr<ATexture>           m_Tex[TEX_END];
    MtrlConst               m_Const;
    ERenderDomain::Type     m_Domain;
};

#include "AMaterial.inl"