#include "pch.h"
#include "AssetMgr.h"

#include "PathMgr.h"
#include "EditorMgr.h"

#include "AMesh.h"
#include "AGraphicShader.h"
#include "ATexture.h"

AssetMgr::AssetMgr() : m_Changed(false), m_Quit{ false }, m_Delay{ 300 } {}

AssetMgr::~AssetMgr() {
	// Watcher thread의 ReadDirectoryChangesW 함수 pending 풀어주기
	CancelIoEx(m_DirHandle, nullptr);

	m_Quit = true;
	m_WatcherThread.join();
}

void AssetMgr::Init() {
	LoadTextures();
	LoadSprites();

	CreateEngineMesh();
	CreateEngineShader();
	CreateEngineMaterial();

	LoadLevels();

	m_WatcherThread = thread(
		&AssetMgr::WatchDirectory, this
		, format(L"{}", CONTENT_PATH));
}

bool AssetMgr::IsChanged() {
	bool Changed = m_Changed;
	m_Changed = false;

	return Changed;
}

void AssetMgr::Tick() {
	lock_guard<mutex> lock(m_Mutex);

	const auto now = std::chrono::steady_clock::now();

	for (auto iter = m_Files.begin(); iter != m_Files.end();) {
		auto timePoint = iter->first;

		auto elapsed = chrono::duration_cast<std::chrono::milliseconds>(
			now - timePoint).count();
		if (elapsed < m_Delay) {
			++iter;
			continue;
		}

		auto filePath = iter->second;
		auto delim = filePath.find_first_of(L"\\");
		auto folder = filePath.substr(0, delim);
		
		switch (Util::HashWString(folder)) {
		case Util::HashWString(L"Texture"): {
			auto texture = LOAD(ATexture, filePath);
			AddAsset(texture->GetKey(), texture.Get());
		}
			break;
		case Util::HashWString(L"Sprite"): {
			auto sprite = LOAD(ASprite, filePath);
			AddAsset(sprite->GetKey(), sprite.Get());
		}
			break;
		case Util::HashWString(L"Flipbook"): {
			auto flipbook = LOAD(AFlipbook, filePath);
			AddAsset(flipbook->GetKey(), flipbook.Get());
		}
			break;
		case Util::HashWString(L"Level"): {
			auto level = LOAD(ALevel, filePath);
			AddAsset(level->GetKey(), level.Get());
		}
			break;
		}

		auto idx = iter - m_Files.begin();

		iter_swap(iter, m_Files.end() - 1);
		m_Files.pop_back();

		iter = m_Files.begin() + idx;
	}

	for (size_t i = 0, end = static_cast<size_t>(m_Logs.size()); i < end; ++i) 		
		LOG_INFO(WStrToStr(m_Logs[i]));
	m_Logs.clear();
}

Ptr<Asset> AssetMgr::FindAsset(EAsset::Type _Type, const wstring& _Key) {
	auto iter = m_mapAsset[(UINT)_Type].find(_Key);
	if (iter == m_mapAsset[(UINT)_Type].end()) return nullptr;

	return iter->second;
}

void AssetMgr::AddAsset(const wstring& _Key, Ptr<Asset> _Asset) {	
	auto iter = m_mapAsset[_Asset->GetType()].find(_Key);
	auto end = m_mapAsset[_Asset->GetType()].end();
	// 동일한 Key가 이미 있어도 경로가 같은 파일이면 동일 파일로 인식하여 무시
	assert(iter == end || (iter->second->GetRelativePath() == _Asset->GetRelativePath()));
	if (iter != end) return;

	_Asset->SetKey(_Key);
	m_mapAsset[(UINT)_Asset->GetType()].insert(make_pair(_Key, _Asset));

	m_Changed = true;
}

void AssetMgr::GetAssetNames(EAsset::Type _type, vector<wstring>& _vec) {
	for (const auto& pair : m_mapAsset[(UINT)_type])
		_vec.push_back(pair.first);
}

void AssetMgr::WatchDirectory(const std::wstring& folderPath) {
	m_DirHandle = CreateFileW(
		folderPath.c_str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		nullptr);

	if (m_DirHandle == INVALID_HANDLE_VALUE) {
		lock_guard<mutex> lock(m_Mutex);

		m_Logs.push_back(format(L"CreateFileW failed: {}", GetLastError()));

		return;
	}

	std::vector<BYTE> buffer(16 * 1024);
	DWORD bytesReturned = 0;

	while (!m_Quit) {
		BOOL ok = ReadDirectoryChangesW(
			m_DirHandle,
			buffer.data(),
			static_cast<DWORD>(buffer.size()),
			TRUE, // 하위 폴더까지 감시
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_LAST_WRITE |
			FILE_NOTIFY_CHANGE_CREATION,
			&bytesReturned,
			nullptr,
			nullptr
		);

		if (!ok) {
			lock_guard<mutex> lock(m_Mutex);

			auto err = GetLastError();

			if (err == ERROR_OPERATION_ABORTED)
				break; // CancelIoEx로 종료됨

			m_Logs.push_back(format(L"ReadDirectoryChangesW failed: {}", err));

			// 다른 에러 처리
			continue;
		}

		BYTE* ptr = buffer.data();
		while (!m_Quit) {
			auto* notify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(ptr);

			std::wstring fileName(notify->FileName,
				notify->FileNameLength / sizeof(WCHAR));

			switch (notify->Action) {
			case FILE_ACTION_ADDED: {
				lock_guard<mutex> lock(m_Mutex);

				m_Logs.push_back(format(
					L"[Created] {}", fileName));
				
				m_Files.push_back({ chrono::steady_clock::now(), fileName});
			}
				break;
			case FILE_ACTION_REMOVED: {
				lock_guard<mutex> lock(m_Mutex);

				m_Logs.push_back(format(
					L"[Deleted] {}", fileName));
			}				
				break;
			case FILE_ACTION_MODIFIED: {
				lock_guard<mutex> lock(m_Mutex);

				m_Logs.push_back(format(
					L"[Modified] {}", fileName));
			}
				break;
			case FILE_ACTION_RENAMED_OLD_NAME: {
				lock_guard<mutex> lock(m_Mutex);

				m_Logs.push_back(format(
					L"[Renamed Old] {}", fileName));
			}
				break;
			case FILE_ACTION_RENAMED_NEW_NAME: {
				lock_guard<mutex> lock(m_Mutex);

				m_Logs.push_back(format(
					L"[Renamed New] {}", fileName));
			}
				break;
			default: {
				lock_guard<mutex> lock(m_Mutex);

				m_Logs.push_back(format(
					L"[UnknownNew] {}", fileName));
			}
				break;
			}

			if (notify->NextEntryOffset == 0)
				break;

			ptr += notify->NextEntryOffset;
		}
	}

	CloseHandle(m_DirHandle);
}