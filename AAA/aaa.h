#pragma once
#ifndef AAA_H
#define AAA_H
#include "ADK/Ashita.h"


#include <set>
#include <map>
#include <locale>
#include <iomanip>
#include "json.hpp"
#include "httplib.h"
#include "date/tz.h"
#include <thread>

std::set<std::string> allowed_mobs;
std::map<std::string, std::string > mob_mapper;




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
class aaaPlugin : IPlugin
{
    IAshitaCore* m_AshitaCore;
    ILogManager* m_LogManager;
    uint32_t            m_PluginId;
    IDirect3DDevice8* m_Direct3DDevice;
    const date::time_zone* zone;
    int lastTimeStamp;
    int usenewtimer;
    int do_discord;
    std::string discordendpoint;
    std::vector<std::string> customMobs;

public:
    // Constructor and Deconstructor
    aaaPlugin(void);
    ~aaaPlugin (void);

public:
    // Plugin Information Callback
    plugininfo_t GetPluginInfo(void) override;

public:
    // Main Initialization and Cleanup Callbacks
    bool Initialize(IAshitaCore* core, ILogManager* log, uint32_t id) override;
    void Release(void) override;
    void PostToDiscord(std::string mob, std::string tod);
    void GetConfig();


    // Chat Manager Callbacks
    bool HandleCommand(const char* command, int32_t type) override;
    bool HandleIncomingText(int16_t mode, const char* message, int16_t* modifiedMode, char* modifiedMessage, bool blocked) override;
    uint16_t GetIndexFromId(int id);

    // Packet Manager Callbacks
    bool HandleIncomingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked) override;
};

#endif // __AAA_H__
