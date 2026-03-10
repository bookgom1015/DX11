#pragma once

#include <vector>
#include <string>

enum SCRIPT_TYPE
{
	CYBERPSYCHOSISSCRIPT,
	ELEVATIONSCRIPT,
	EMPTYSCRIPT,
	ENEMYSCRIPT,
	FLASHLIGHTSCRIPT,
	PARTICLESCRIPT,
	PLATFORMERPLAYERSCRIPT,
	RELICSCRIPT,
	SANDEVISTANSCRIPT,
	TARGETMISSILESCRIPT,
	TRACKINGCAMERASCRIPT,
	Count
};

using namespace std;

class CScript;

class ScriptMgr
{
public:
	static void GetScriptInfo(vector<wstring>& _vec);
	static CScript * GetScript(const wstring& _strScriptName);
	static CScript * GetScript(UINT _iScriptType);
	static const wchar_t * GetScriptName(CScript * _pScript);
	static const wchar_t * GetScriptName(SCRIPT_TYPE type);
};
