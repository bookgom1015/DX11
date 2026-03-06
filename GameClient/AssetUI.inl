#ifndef __ASSETUI_INL__
#define __ASSETUI_INL__

void AssetUI::SetTargetAsset(Ptr<Asset> _Asset) { m_TargetAsset = _Asset; }

Ptr<Asset> AssetUI::GetTargetAsset() const { return  m_TargetAsset; }

#endif // __ASSETUI_INL__