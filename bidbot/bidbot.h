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
#include <regex>
#include "datamap.h"
#include <json.hpp>
#include "zone.h"

int active_timer;
int last_check_time;
int freelot_enable; // send freelot messages

std::thread my_thread;
std::thread msg_thread;
std::queue<std::string> message_queue;


// a deadline to indicate end of bidding
struct timer_data {
    std::chrono::time_point<std::chrono::system_clock> deadline;
    std::string message;
    int active;
};

// collects everything about a bid
struct bid_data {
    int active;
    std::string name; // hopefully to distinguish bids...
    struct timer_data msg1; // for the 60s warning
    struct timer_data msg2; // for the final message
    int itemid; // itemid number
    std::vector<std::pair<std::string, int>> bid_entries; // (player name, bid amount) pairs 
};

std::vector<struct bid_data> bids;



std::string escape_json(const std::string& s) {
    std::ostringstream o;
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        switch (*c) {
        case '"': o << "\\\""; break;
        case '\\': o << "\\\\"; break;
        case '\b': o << "\\b"; break;
        case '\f': o << "\\f"; break;
        case '\n': o << "\\n"; break;
        case '\r': o << "\\r"; break;
        case '\t': o << "\\t"; break;
        default:
            if ('\x00' <= *c && *c <= '\x1f') {
                o << "\\u"
                    << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(*c);
            }
            else {
                o << *c;
            }
        }
    }
    return o.str();
}


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
class bidbotPlugin : IPlugin
{
    IAshitaCore* m_AshitaCore;
    ILogManager* m_LogManager;
    uint32_t            m_PluginId;
    IDirect3DDevice8* m_Direct3DDevice;
    int lastTimeStamp;

    int city_enable; // city
    int upgrade_enable; // upgrades
    int acc_enable; // accessories

public:
    // Constructor and Deconstructor
    bidbotPlugin(void);
    ~bidbotPlugin(void);

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
};

#endif // __BIDBOT_H__
