#pragma once
#ifndef BBB_H
#define BBB_H
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "ADK/Ashita.h"

#include <set>
#include <map>
#include <thread>
#include <queue>

#include <json.hpp>

struct event_data {
    int tod;
    int windows;
    int day;
    int start;
    int interval;
    std::vector<int> alerts;
    int custom;
};

struct timer_data {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> tp;
    std::string message;
    int active;
};

std::set<std::string> allowed_mobs;
std::map<std::string, std::string > mob_mapper;
std::map<std::string, struct event_data> event_timers;
std::vector<struct timer_data> time_list;
std::thread my_thread;
std::thread msg_thread;
std::queue<std::string> queued_messages;


int last_check_time;
bool active_timer = true;
bool kill_thread = false;


/**
 * Plugin Information
 *
 * This object holds information specific to this plugin. The CreatePluginInfo exported function
 * uses this object to create information about the plugin which is used internally with Ashita.
 *
 * Please note: Ashita maintains the pointer of this object! Your plugin should never try to delete
 *              this object manually!
 */
plugininfo_t* g_PluginInfo = nullptr;

/**
 * Main Plugin Class Instance
 *
 * The main class the plugin will expose back to Ashita when loaded.
 * This class must inherit from IPlugin in order to be valid!
 *
 * See ExamplePlugin.cpp for more information about the functions in this class.
 */
class bbbPlugin : IPlugin
{
    IAshitaCore* m_AshitaCore;
    ILogManager* m_LogManager;
    uint32_t            m_PluginId;
    IDirect3DDevice8* m_Direct3DDevice;
    int lastTimeStamp;

public:
    // Constructor and Deconstructor
    bbbPlugin(void);
    ~bbbPlugin(void);

public:
    // Plugin Information Callback
    plugininfo_t GetPluginInfo(void) override;

public:
    // Main Initialization and Cleanup Callbacks
    bool Initialize(IAshitaCore* core, ILogManager* log, uint32_t id) override;
    void Release(void) override;

    // Chat Manager Callbacks
    bool HandleCommand(const char* command, int32_t type) override;
    bool HandleIncomingText(int16_t mode, const char* message, int16_t* modifiedMode, char* modifiedMessage, bool blocked) override;
    uint16_t GetIndexFromId(int id);

    // Packet Manager Callbacks
    bool HandleIncomingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked) override;
    std::string HandleGetRequest(std::vector<std::string> & args, int offset);
};

#endif // __BBB_H__
