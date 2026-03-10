#include "pch.h"
#include "ScriptMgr.h"

#include "Scripts/CCyberPsychosisScript.h"
#include "Scripts/CElevationScript.h"
#include "Scripts/CEmptyScript.h"
#include "Scripts/CEnemyScript.h"
#include "Scripts/CFlashLightScript.h"
#include "Scripts/CParticleScript.h"
#include "Scripts/CPlatformerPlayerScript.h"
#include "Scripts/CRelicScript.h"
#include "Scripts/CSandevistanScript.h"
#include "Scripts/CTargetMissileScript.h"
#include "Scripts/CTrackingCameraScript.h"

void ScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CCyberPsychosisScript");
	_vec.push_back(L"CElevationScript");
	_vec.push_back(L"CEmptyScript");
	_vec.push_back(L"CEnemyScript");
	_vec.push_back(L"CFlashLightScript");
	_vec.push_back(L"CParticleScript");
	_vec.push_back(L"CPlatformerPlayerScript");
	_vec.push_back(L"CRelicScript");
	_vec.push_back(L"CSandevistanScript");
	_vec.push_back(L"CTargetMissileScript");
	_vec.push_back(L"CTrackingCameraScript");
}

CScript * ScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CCyberPsychosisScript" == _strScriptName)
		return new CCyberPsychosisScript;
	if (L"CElevationScript" == _strScriptName)
		return new CElevationScript;
	if (L"CEmptyScript" == _strScriptName)
		return new CEmptyScript;
	if (L"CEnemyScript" == _strScriptName)
		return new CEnemyScript;
	if (L"CFlashLightScript" == _strScriptName)
		return new CFlashLightScript;
	if (L"CParticleScript" == _strScriptName)
		return new CParticleScript;
	if (L"CPlatformerPlayerScript" == _strScriptName)
		return new CPlatformerPlayerScript;
	if (L"CRelicScript" == _strScriptName)
		return new CRelicScript;
	if (L"CSandevistanScript" == _strScriptName)
		return new CSandevistanScript;
	if (L"CTargetMissileScript" == _strScriptName)
		return new CTargetMissileScript;
	if (L"CTrackingCameraScript" == _strScriptName)
		return new CTrackingCameraScript;
	return nullptr;
}

CScript * ScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::CYBERPSYCHOSISSCRIPT:
		return new CCyberPsychosisScript;
		break;
	case (UINT)SCRIPT_TYPE::ELEVATIONSCRIPT:
		return new CElevationScript;
		break;
	case (UINT)SCRIPT_TYPE::EMPTYSCRIPT:
		return new CEmptyScript;
		break;
	case (UINT)SCRIPT_TYPE::ENEMYSCRIPT:
		return new CEnemyScript;
		break;
	case (UINT)SCRIPT_TYPE::FLASHLIGHTSCRIPT:
		return new CFlashLightScript;
		break;
	case (UINT)SCRIPT_TYPE::PARTICLESCRIPT:
		return new CParticleScript;
		break;
	case (UINT)SCRIPT_TYPE::PLATFORMERPLAYERSCRIPT:
		return new CPlatformerPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::RELICSCRIPT:
		return new CRelicScript;
		break;
	case (UINT)SCRIPT_TYPE::SANDEVISTANSCRIPT:
		return new CSandevistanScript;
		break;
	case (UINT)SCRIPT_TYPE::TARGETMISSILESCRIPT:
		return new CTargetMissileScript;
		break;
	case (UINT)SCRIPT_TYPE::TRACKINGCAMERASCRIPT:
		return new CTrackingCameraScript;
		break;
	}
	return nullptr;
}

const wchar_t * ScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::CYBERPSYCHOSISSCRIPT:
		return L"CCyberPsychosisScript";
		break;

	case SCRIPT_TYPE::ELEVATIONSCRIPT:
		return L"CElevationScript";
		break;

	case SCRIPT_TYPE::EMPTYSCRIPT:
		return L"CEmptyScript";
		break;

	case SCRIPT_TYPE::ENEMYSCRIPT:
		return L"CEnemyScript";
		break;

	case SCRIPT_TYPE::FLASHLIGHTSCRIPT:
		return L"CFlashLightScript";
		break;

	case SCRIPT_TYPE::PARTICLESCRIPT:
		return L"CParticleScript";
		break;

	case SCRIPT_TYPE::PLATFORMERPLAYERSCRIPT:
		return L"CPlatformerPlayerScript";
		break;

	case SCRIPT_TYPE::RELICSCRIPT:
		return L"CRelicScript";
		break;

	case SCRIPT_TYPE::SANDEVISTANSCRIPT:
		return L"CSandevistanScript";
		break;

	case SCRIPT_TYPE::TARGETMISSILESCRIPT:
		return L"CTargetMissileScript";
		break;

	case SCRIPT_TYPE::TRACKINGCAMERASCRIPT:
		return L"CTrackingCameraScript";
		break;

	}
	return nullptr;
}

const wchar_t * ScriptMgr::GetScriptName(SCRIPT_TYPE type)
{
	switch (type)
	{
	case SCRIPT_TYPE::CYBERPSYCHOSISSCRIPT:
		return L"CCyberPsychosisScript";
		break;

	case SCRIPT_TYPE::ELEVATIONSCRIPT:
		return L"CElevationScript";
		break;

	case SCRIPT_TYPE::EMPTYSCRIPT:
		return L"CEmptyScript";
		break;

	case SCRIPT_TYPE::ENEMYSCRIPT:
		return L"CEnemyScript";
		break;

	case SCRIPT_TYPE::FLASHLIGHTSCRIPT:
		return L"CFlashLightScript";
		break;

	case SCRIPT_TYPE::PARTICLESCRIPT:
		return L"CParticleScript";
		break;

	case SCRIPT_TYPE::PLATFORMERPLAYERSCRIPT:
		return L"CPlatformerPlayerScript";
		break;

	case SCRIPT_TYPE::RELICSCRIPT:
		return L"CRelicScript";
		break;

	case SCRIPT_TYPE::SANDEVISTANSCRIPT:
		return L"CSandevistanScript";
		break;

	case SCRIPT_TYPE::TARGETMISSILESCRIPT:
		return L"CTargetMissileScript";
		break;

	case SCRIPT_TYPE::TRACKINGCAMERASCRIPT:
		return L"CTrackingCameraScript";
		break;

	}
	return nullptr;
}