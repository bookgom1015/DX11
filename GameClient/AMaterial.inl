#ifndef __AMATERIAL_INL__
#define __AMATERIAL_INL__

void AMaterial::SetShader(Ptr<AGraphicShader> _Shader) { m_Shader = _Shader; }

Ptr<AGraphicShader> AMaterial::GetShader() const { return m_Shader; }

void AMaterial::SetTexture(TEX_PARAM _Param, Ptr<ATexture> _Texture) { 
    m_Tex[_Param] = _Texture; 
}

Ptr<ATexture> AMaterial::GetTexture(TEX_PARAM _Param) const { 
    return  m_Tex[_Param]; 
}

template<typename T>
void AMaterial::SetScalar(SCALAR_PARAM _Param, const T& _Data) {
    switch (_Param) {
    case INT_0:
    case INT_1:
    case INT_2:
    case INT_3:
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, UINT>)
            m_Const.iArr[_Param] = _Data;
        break;
    case FLOAT_0:
    case FLOAT_1:
    case FLOAT_2:
    case FLOAT_3:
        if constexpr (std::is_same_v<T, float>)
            m_Const.fArr[_Param - FLOAT_0] = _Data;
        break;
    case VEC2_0:
    case VEC2_1:
    case VEC2_2:
    case VEC2_3:
        if constexpr (std::is_same_v<T, Vec2>)
            m_Const.v2Arr[_Param - VEC2_0] = _Data;
        break;
    case VEC4_0:
    case VEC4_1:
    case VEC4_2:
    case VEC4_3:
        if constexpr (std::is_same_v<T, Vec4>)
            m_Const.v4Arr[_Param - VEC4_0] = _Data;
        break;
    case MAT_0:
    case MAT_1:
        if constexpr (std::is_same_v<T, Matrix>)
            m_Const.mat[_Param - MAT_0] = _Data;
        break;
    }
}

template<typename T>
inline T& AMaterial::GetScalar(SCALAR_PARAM _Param) {
    switch (_Param) {
    case INT_0:
    case INT_1:
    case INT_2:
    case INT_3:
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, UINT>)
            return m_Const.iArr[_Param];
        break;
    case FLOAT_0:
    case FLOAT_1:
    case FLOAT_2:
    case FLOAT_3:
        if constexpr (std::is_same_v<T, float>)
            return m_Const.fArr[_Param - FLOAT_0];
        break;
    case VEC2_0:
    case VEC2_1:
    case VEC2_2:
    case VEC2_3:
        if constexpr (std::is_same_v<T, Vec2>)
            return m_Const.v2Arr[_Param - VEC2_0];
        break;
    case VEC4_0:
    case VEC4_1:
    case VEC4_2:
    case VEC4_3:
        if constexpr (std::is_same_v<T, Vec4>)
            return m_Const.v4Arr[_Param - VEC4_0];
        break;
    case MAT_0:
    case MAT_1:
        if constexpr (std::is_same_v<T, Matrix>)
            return m_Const.mat[_Param - MAT_0];
        break;
    }
}


#endif // __AMATERIAL_INL__