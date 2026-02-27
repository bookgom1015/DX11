#pragma once

#include "AssetUI.h"

namespace LogLevel {
    enum Type {
        E_Info = 0,
        E_Warning,
        E_Error,
        E_Critical,
        Count
    };
}

struct LogEntry {
    LogLevel::Type Level;
    std::string Message;
};

class LogUI : public EditorUI {
public:
    LogUI();
    virtual ~LogUI();

public:
    virtual void Tick_UI() override;

public:
    void AddLog(const LogEntry& entry) { mLogs.push_back(entry); }

private:
    vector<LogEntry> mLogs{};

    bool mbAutoScroll{ true };
    bool mbScrollToBottom{ false };
};

