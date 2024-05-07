// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "bidbot.h"
#include <httplib.h>

struct threaddata {
    IAshitaCore* m_AshitaCore;
} ;

struct discordthreaddata {
    IAshitaCore* m_AshitaCore;
    std::string msg;
};

void postToDiscord(const discordthreaddata &data) {
    httplib::SSLClient cli("discord.com");
    cli.enable_server_certificate_verification(false);
    
    std::string theUrl = "your webhook url";

    char datebuffer[80];
    auto mytime = std::time(nullptr);
    auto mytimep = *std::localtime(&mytime);
    strftime(datebuffer, sizeof(datebuffer), "%d-%m-%Y %H:%M:%S (%z)", &mytimep);

    std::string short_content = "```ansi\n\nBidBot report from ";
    short_content.append(data.m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0));

    short_content.append(" at ");
    short_content.append(datebuffer);
    short_content.append(" in Zone " + zoneMap[data.m_AshitaCore->GetDataManager()->GetParty()->GetMemberZone(0)]);
    short_content.append("\n\n");
    

    short_content.append(data.msg);

    short_content.append("\n```");

    std::string escaped_short_content = escape_json(short_content);

    std::string body = "";

    body.append("{\"username\": \"BidBot");
    body.append("\", \"content\": \"");
    body.append(escaped_short_content);
    body.append("\"}");


    httplib::Headers headers = { { "Content-Type", "application/json" } };

    if (httplib::Result res = cli.Post(theUrl.c_str(), body.c_str(), "application/json")) {
        nlohmann::json j = nlohmann::json::parse(res->body.c_str());
        std::string msgid = j["id"];
        data.m_AshitaCore->GetChatManager()->Writef("Discord Message ID: %s\n", msgid.c_str());
        return;
    }
    else {
        std::string myerror = httplib::to_string(res.error());
        data.m_AshitaCore->GetChatManager()->Writef("Error: %s\n", myerror.c_str());
        return;
    }
}

void message_thread(const threaddata& data) {
    while (active_timer) {
        if (message_queue.size() > 0) {
            data.m_AshitaCore->GetChatManager()->QueueCommand(message_queue.front().c_str(), 0);
            message_queue.pop();
        }
        ::Sleep(1100);
    }
}


void thread_helper(const threaddata& data) {
    while (active_timer) {
        int time_elapsed = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count() - last_check_time;
        if (time_elapsed > 0) {
            if (bids.size() > 0) {
                for (int i = 0; i < bids.size(); ++i) {
                    if (!bids[i].active) {
                        continue;
                    }
                    // for each timer...
                    if (bids[i].msg1.active == 1 && std::chrono::system_clock::now() >= bids[i].msg1.deadline) {
                        std::string msg = "/l " + bids[i].msg1.message + "\n";
                        bids[i].msg1.active = 0;
                        message_queue.push(msg);
                    }
                    if (bids[i].msg2.active == 1 && std::chrono::system_clock::now() >= bids[i].msg2.deadline) {
                        // disable subsequent messages...
                        bids[i].msg2.active = 0;

                        // sort...
                        std::sort(bids[i].bid_entries.begin(), bids[i].bid_entries.end(), [](auto& left, auto& right) {
                            return left.second < right.second;
                        });

                        if (bids[i].bid_entries.size() == 0) {
                            //no bids

                            message_queue.push("/l @@@ [" + bids[i].name + "]  FREE LOT");

                            // note as a freelot
                            freelots.insert(bids[i].itemid);

                            if (freelot_enable) {
                                struct discordthreaddata td;
                                td.m_AshitaCore = data.m_AshitaCore;

                                td.msg = bids[i].name;
                                if (dataMap.count(bids[i].itemid)) {
                                    td.msg.append(" (" + dataMap[bids[i].itemid][0] + " / " + dataMap[bids[i].itemid][3] + ")");
                                }
                                td.msg.append(" => FREE LOT");
                                std::thread discordthread = std::thread(postToDiscord, td);
                                discordthread.detach();
                            }
                        } else {
                            // at least one bid

                            // print out bids in console
                            for (int j = 0; j < bids[i].bid_entries.size(); ++j) {
                                data.m_AshitaCore->GetChatManager()->Writef("  Bid \x1f\xc8%s\x1f\x05;  %s  \x1f\x82 =  \x1f\xc8 %d\x1f\x82\n", bids[i].name.c_str(), bids[i].bid_entries[j].first.c_str(), bids[i].bid_entries[j].second);
                            }
                            int num_winners = 1; // at least one winner to start with
                            int prev_bid = bids[i].bid_entries[bids[i].bid_entries.size() - 1].second; // last entry... should be highest after sort
                            std::vector<std::string> winners;

                            // get list of winners in reverse order
                            winners.push_back(bids[i].bid_entries[bids[i].bid_entries.size() - 1].first);

                            // find all players with the same bid as the highest
                            for (int j = bids[i].bid_entries.size() - 2; j >= 0; --j) {
                                if (prev_bid == bids[i].bid_entries[j].second) {
                                    // collect names of bidders who bid the same, highest amount
                                    winners.push_back(bids[i].bid_entries[j].first);
                                    num_winners += 1;
                                }
                                else {
                                    break;
                                }
                            }

                            struct discordthreaddata td;
                            td.m_AshitaCore = data.m_AshitaCore;

                            td.msg = bids[i].name;
                            if (dataMap.count(bids[i].itemid)) {
                                td.msg.append(" (" + dataMap[bids[i].itemid][0] + " / " + dataMap[bids[i].itemid][3] + ")");
                            }

                            td.msg.append(" => ");

                            std::string win_string = "/l @@@ [" + bids[i].name + "] WINNER";
                            
                            if (winners.size() > 1) {
                                win_string += "S (" + std::to_string(winners.size()) + "-way tie) ";
                                td.msg.append("(" + std::to_string(winners.size()) + "-way tie) ");
                            }
                            if (winners.size() == 1) {
                                win_string += " ";
                            }
                            win_string += "[" + std::to_string(prev_bid) + " pts]";
                            win_string += ": ";

                            td.msg.append( "[\u001b[2;31m" + std::to_string(prev_bid) + " pts\u001b[0m]: " );
                            
                            for (int j = 0; j < winners.size(); ++j) {
                                win_string += winners[j];
                                td.msg.append("\u001b[2;34m");
                                td.msg.append(winners[j]);
                                td.msg.append("\u001b[0m");
                                if (j < winners.size() - 1) {
                                    win_string += ", ";
                                    td.msg.append(", ");
                                }
                            }

                            std::thread discordthread = std::thread(postToDiscord, td);
                            discordthread.detach();
                            message_queue.push(win_string);
                        }
                        // inactivate bid
                        bids[i].active = 0;

                        break;
                    }
                }
            }
        }
        // run slightly more frequently to ensure timers pulse within .5s of scheduled time
        ::Sleep(500);
    }
}

/**
 * Constructor and Deconstructor
 */
bidbotPlugin::bidbotPlugin(void)
    : m_AshitaCore(nullptr)
    , m_LogManager(nullptr)
    , m_PluginId(0)
    , m_Direct3DDevice(nullptr)
{ }
bidbotPlugin::~bidbotPlugin(void)
{ }


/**
 * Returns the plugins information structure.
 *
 * @returns {plugininfo_t} The plugin information structure of the plugin.
 */
plugininfo_t bidbotPlugin::GetPluginInfo(void)
{
    return *g_PluginInfo;
}



/**
 * Invoked when the plugin is loaded, allowing it to prepare for usage.
 *
 * @param {IAshitaCore*} core - The Ashita core object to interact with the various Ashita managers.
 * @param {ILogManager*} log - The log manager used to interact with the current log file.
 * @param {uint32_t} id - The plugins id, or its module base, that identifies it other than its name.
 * @returns {bool} True on success, false otherwise. (If false, the plugin will not be loaded.)
 *
 * @notes
 *
 *      Plugins must return true here or they will be considered invalid and unload immediately.
 *      Returning false means that the plugin failed to initialize and should not be used.
 */
bool bidbotPlugin::Initialize(IAshitaCore* core, ILogManager* log, uint32_t id)
{
    // Store the variables for later usage..
    this->m_AshitaCore = core;
    this->m_LogManager = log;
    this->m_PluginId = id;

    bids.clear();
    active_timer = 1;
    last_check_time = 0;
 
    struct threaddata td;
    td.m_AshitaCore = m_AshitaCore;
    my_thread = std::thread(thread_helper, td);
    msg_thread = std::thread(message_thread, td);

    this->acc_enable = 1;
    this->city_enable = 1;
    this->upgrade_enable = 1;
    freelot_enable = 0;

    m_AshitaCore->GetChatManager()->Write("\x1f\xc8 BidBot\x1f\x82 Loaded.    Use   /disablecity   /disableup   /disableacc to disable AF types.");
    m_AshitaCore->GetChatManager()->Write("\x1f\xc8 BidBot\x1f\x82 Use   /disablecity   /disableup   /disableacc to disable AF types.");
    m_AshitaCore->GetChatManager()->Write("\x1f\xc8 BidBot\x1f\x82 Use   /enablecity    /enableup    /enableacc  to enable  AF types.");
    m_AshitaCore->GetChatManager()->Write("\x1f\xc8 BidBot\x1f\x82 city = city AF    up = upgrades (-1 items)   acc = accessories (belt/cape)");
    m_AshitaCore->GetChatManager()->Write("\x1f\xc8 BidBot\x1f\x82 Use   /enablefreelot  and  /disablefreelot to start/stop freelot discord receipts.");

    return true;
}

/**
 * Invoked when the plugin is being unloaded, allowing it to cleanup its resources.
 *
 * @notes
 *
 *      Plugins should use this function to cleanup non-Direct3D related objects.
 *          - Delete created font objects.
 *          - Delete created Gui objects.
 *          - Internal memory allocations.
 */
void bidbotPlugin::Release(void)
{ 
    for (int i = 0; i < bids.size(); ++i) {
        bids[i].bid_entries.clear();
    }
    bids.clear();
    active_timer = 0;
    my_thread.join();
    msg_thread.join();

}


/**
 * Invoked when a command is being processed by the game client.
 *
 * Note:
 *      Please note, this handles all things done via the game in terms of commands
 *      and chat. All / commands as well as normal chat you type, macros, etc. will
 *      be processed through here. You should only use this to handle / commands.
 *
 *      If you wish to handle other bits of outgoing text before the client sees it,
 *      then use the HandleOutgoingText callback instead.
 *
 * @param {const char*} command         - The raw command string being processed.
 * @param {uint32_t} type               - The type of the command being processed. (See Ashita::CommandInputType enumeration.)
 * @returns {bool} True if handled and should be blocked, false otherwise.
 *
 * @notes
 *
 *      Plugins can handle and block input from being sent to the game in this function. Returning
 *      true will block the current command from happening at all within the client. Plugins should
 *      only return true when the command passed to this function is being handled by the plugin.
 */
bool bidbotPlugin::HandleCommand(const char* command, int32_t type)
{
    UNREFERENCED_PARAMETER(type);

    // Split the command into args..
    std::vector<std::string> args;
    auto count = Ashita::Commands::GetCommandArgs(command, &args);

    // Handle the example slash command..
    HANDLECOMMAND("/makebid")
    {
        // /makebid item
        if (count < 5) {
            m_AshitaCore->GetChatManager()->Write("Must specify bid name.  /makebid blm name name2 slot\n");
            return false;
        }

        for (int i = 0; i < bids.size(); ++i) {
            if (bids[i].name == args[1]) {
                m_AshitaCore->GetChatManager()->Writef("Bid name \"%s\" already running!\n", args[1].c_str());
                return false;
            }
        }

        
        struct bid_data new_data;
        struct timer_data t1;
        struct timer_data t2;

        dataMap[9999] = std::vector<std::string>{ std::string{args[2]}, std::string{args[3]}, std::string{args[1]}, std::string{args[4] } };
        new_data.itemid = 9999;


        t1.deadline = std::chrono::system_clock::now() + std::chrono::seconds(60);
        t1.message = "@@@ [" + args[1] + "] 60 seconds  /tell " + std::string(m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0)) + " " + args[1] + " points";
        t1.active = 1;

        t2.deadline = std::chrono::system_clock::now() + std::chrono::seconds(120);
        t2.message = "@@@ [" + args[1] + "] BID TIME UP  (" + dataMap[9999][0] + ")";
        t2.active = 1;


        new_data.active = 1; // make active
        new_data.bid_entries.clear(); // clear the list if it hasn't been already...
        new_data.name = args[1];
        new_data.msg1 = t1;
        new_data.msg2 = t2;


        std::string cmd = "/l @@@ [" + args[1] + "] 2 MINUTES  (" + dataMap[9999][3] + ")  /tell " + std::string(m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0)) + " " + args[1] + " yourbid";

        m_AshitaCore->GetChatManager()->Write(cmd.c_str());
        message_queue.push(cmd);

        bids.push_back(new_data); // put a copy on the vector here...

        return false;
    }

    HANDLECOMMAND("/cbid") {
        if (count < 2) {
            m_AshitaCore->GetChatManager()->Write("Must specify bid name.  /cbid blm\n");
            return false;
        }

        for (int i = 0; i < bids.size(); ++i) {
            if (bids[i].name == args[1]) {
                // found it
                for (int j = 0; j < bids[i].bid_entries.size(); ++j) {
                    std::string msg = "   BID " + args[1] + "  " + bids[i].bid_entries[j].first + ": " + std::to_string(bids[i].bid_entries[j].second) + "\n";
                    m_AshitaCore->GetChatManager()->Write(msg.c_str());
                }
            }
        }
        return false;
    }

    HANDLECOMMAND("/dbid") {
        if (count < 2) {
            m_AshitaCore->GetChatManager()->Write("Must specify bid name.  /dbid blm\n");
            return false;
        }

        for (int i = 0; i < bids.size(); ++i) {
            if (bids[i].name == args[1]) {
                //found it
                bids[i].bid_entries.clear();
                bids[i].active = 0;
            }
        }
        return false;
    }

    HANDLECOMMAND("/enablecity") {
        this->city_enable = 1;
        this->m_AshitaCore->GetChatManager()->Write("City AF enabled");
        return false;
    }
    HANDLECOMMAND("/enableup") {
        this->upgrade_enable = 1;
        this->m_AshitaCore->GetChatManager()->Write("AF Upgrades (-1 pieces) enabled");
        return false;
    }
    HANDLECOMMAND("/enableacc") {
        this->acc_enable = 1;
        this->m_AshitaCore->GetChatManager()->Write("Accessories (AF belts/capes) enabled");
        return false;
    }

    HANDLECOMMAND("/disablecity") {
        this->city_enable = 0;
        this->m_AshitaCore->GetChatManager()->Write("City AF disabled");
        return false;
    }
    HANDLECOMMAND("/disableup") {
        this->upgrade_enable = 0;
        this->m_AshitaCore->GetChatManager()->Write("AF Ugrades (-1 pieces) disabled");
        return false;
    }
    HANDLECOMMAND("/disableacc") {
        this->acc_enable = 0;
        this->m_AshitaCore->GetChatManager()->Write("Accessories (AF belts/capes) disabled");
        return false;
    }

    HANDLECOMMAND("/enablefreelot") {
        freelot_enable = 1;
        this->m_AshitaCore->GetChatManager()->Write("Freelot messages enabled.");
        return false;
    }

    HANDLECOMMAND("/disablefreelot") {
        freelot_enable = 0;
        this->m_AshitaCore->GetChatManager()->Write("Freelot messages disabled.");
        return false;
    }

    // Return false here to allow unhandled commands to continue to be processed.
    return false;
}


bool bidbotPlugin::HandleIncomingText(int16_t mode, const char* message, int16_t* modifiedMode, char* modifiedMessage, bool blocked)
{
    //mode 14 = linkshell, mode 12 = tell, mode 6 = outgoing /l
    if (mode != 12) {
        return false;
    }

    std::vector<std::string> args;
    auto count = Ashita::Commands::GetCommandArgs(message, &args);

    if (bids.size() == 0) {
        // no active bids...
        return false;
    }

    int their_bid = 0;
    int their_bid_id = -1;
    int found = 0;
    std::string their_bid_name = "NONE";

    std::string recip = args[0];
    std::string compute_recip = "";
    for (int j = 0; j < recip.size(); ++j) {
        if (recip[j] >= 0x41 && recip[j] <= 0x7a) {
            compute_recip += recip[j];
        }
    }


    // most of the time, only one bid will be active... do this loop to figure out which ones are active
    int num_active_bids = 0;
    std::string active_bid = "";
    for (int i = 0; i < bids.size(); ++i) {
        if (bids[i].active == 1) {
            num_active_bids += 1;
            active_bid = bids[i].name; // if only one bid is active, active_bid will contain its name
        }
    }

    // with more than one active bid, require they specify the name of the bid
    if (count <= 2 && num_active_bids > 1) {
        std::string cmd = "/t " + compute_recip + " " + " bid with /t <me> bid_name pts    active bids: ";
        for (int i = 0; i < bids.size(); ++i) {
            if (bids[i].active) {
                cmd += bids[i].name + " ";
            }
        }
        m_AshitaCore->GetChatManager()->Write(cmd.c_str());
        if (strcmp(compute_recip.c_str(), m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0))) {
            message_queue.push(cmd);
        }
        return false;
    }

    // regex for detecting digits like "50pts"
    std::regex e("^\\d+[a-zA-Z_!$#@]*");

    // with only one active bid and they do something like /t dajiba 50,  we should detect the active bid and register it anyway
    if (count == 2 && num_active_bids == 1) {
        if (std::regex_match(args[1], e)) {
            try{
                their_bid = std::stoi(args[1]);

                // find index of the active bid
                for (int i =0; i<bids.size(); ++i) {
                    if (active_bid == bids[i].name) {
                        their_bid_name = active_bid;
                        their_bid_id = i;
                        break;
                    }
                }
            }
            catch (...) {
                their_bid = 0;
                std::string cmd = "/t " + compute_recip + " invalid bid.  /t <me> bid_name pts   active bids: ";
                for (int i = 0; i < bids.size(); ++i) {
                    cmd += bids[i].name + " ";
                }
                m_AshitaCore->GetChatManager()->Write(cmd.c_str());
                if (strcmp(compute_recip.c_str(), m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0))) {
                    message_queue.push(cmd);
                }
                return false;
            }
            // by this point, we have successfully identified the single active bid, its name, and its index.  Now we can continue...
        }
    }

    // now, regardless of how many active bids there are, we scan through args to figure out if any of them are just an int.
    if (num_active_bids > 0) {
        for (int i = 1; i < count; ++i) {
            if (std::regex_match(args[i], e)) {
                try {
                    their_bid = std::stoi(args[i]);
                }
                catch (...) {
                    their_bid = 0;
                    std::string cmd = "/t " + compute_recip + " " + " invalid bid.  Hat made me do this.";
                    m_AshitaCore->GetChatManager()->Write(cmd.c_str());
                    if (strcmp(compute_recip.c_str(), m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0))) {
                        message_queue.push(cmd);
                    }
                    return false;
                }
            }
        }
    }

    // also, regardless of how many active bids there are, we can scan through and find a bid name anyway
    // this shouldn't be harmful given the check above...
    // if they only provided 1 arg and they only gave a number, this loop won't do anything anyway
    for (int i = 1; i < count; ++i) {
        for (int j = 0; j < bids.size(); ++j) {
            if (args[i] == bids[j].name && bids[j].active == 1) {
                their_bid_name = args[i];
                their_bid_id = j;
            }
        }
    }
    
    // if we couldn't find a number and a bid name in their /tell, then warn them without doing anything
    if (num_active_bids > 0 && (their_bid == 0 || their_bid_name == "NONE")) {
        std::string cmd = "/t " + compute_recip + " invalid bid.  use /t <me> bid_name pts   active bids: ";
        for (int i = 0; i < bids.size(); ++i) {
            if (bids[i].active) {
                cmd += bids[i].name + " ";
            }
        }
        m_AshitaCore->GetChatManager()->Write(cmd.c_str());
        if (strcmp(compute_recip.c_str(), m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0))) {
            message_queue.push(cmd);
        }
        return false;
    }

    if (num_active_bids > 0) {
        // if they already bid before, overwrite with their most recent bid
        for (int i = 0; i < bids[their_bid_id].bid_entries.size(); ++i) {
            if (bids[their_bid_id].bid_entries[i].first == compute_recip) {
                bids[their_bid_id].bid_entries[i].second = their_bid;
                std::string cmd = "/t " + compute_recip + " bid received for \"" + their_bid_name + "\": " + std::to_string(their_bid);
                m_AshitaCore->GetChatManager()->Write(cmd.c_str());
                if (strcmp(compute_recip.c_str(), m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0))) {
                    message_queue.push(cmd);
                }
                found = 1;
                break;
            }
        }

        // if they didn't bid before, put their bid in the list
        if (found == 0) {
            bids[their_bid_id].bid_entries.push_back(std::pair<std::string, int>(compute_recip, their_bid)); // new entry for their bid
            std::string cmd = "/t " + compute_recip + " bid received for \"" + their_bid_name + "\": " + std::to_string(their_bid);
            m_AshitaCore->GetChatManager()->Write(cmd.c_str());
            if (strcmp(compute_recip.c_str(), m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0))) {
                message_queue.push(cmd);
            }
        }
    }


    return false;
}

uint16_t bidbotPlugin::GetIndexFromId(int id)
{
    auto entMgr = m_AshitaCore->GetDataManager()->GetEntity();
    for (int i = 0; i < 0x900; i++)
    {
        if (entMgr->GetServerId(i) == id)
            return i;
    }
    return 0;
}



/**
 * Invoked when an incoming packet is being handled.
 *
 * @param {uint16_t} id                 - The id of the packet.
 * @param {uint32_t} size               - The size of the packet data.
 * @param {void*} data                  - The raw data of the packet.
 * @param {void*} modified              - The modified data, if any, that has been altered by other plugins/addons.
 * @param {bool} blocked                - Flag if this message has been blocked already by another plugin. (Once blocked, other plugins cannot restore it.)
 * @returns {bool} True if handled and should be blocked, false otherwise.
 *
 * @notes
 *
 *      Plugins can block the packet by returning true in this function.
 *      Plugins can alter the data of the packet by editing the modified data parameter.
 *      Plugins can check if other plugins have blocked the current packet by checking if the blocked param is true.
 *
 *      Please note; altering packets incorrectly or altering the flow of packets incorrectly can have adverse affects
 *      and possibly lead to players being banned. Edit with caution!
 */
bool bidbotPlugin::HandleIncomingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked)
{
    if (id == 0x00D2) {
        int itemid = 0;
        int itemslot = 0;
        int isOldItem = 0;
        memcpy(&itemid, (unsigned char*)data + 0x10, sizeof(int16_t));
        memcpy(&itemslot, (unsigned char*)data + 0x14, sizeof(int8_t));
        memcpy(&isOldItem, (unsigned char*)data + 0x15, sizeof(int8_t));

        if (dataMap.count(itemid) && !isOldItem) {
            //item is in the map of allowed items...

            struct bid_data new_data;
            struct timer_data t1;
            struct timer_data t2;

            std::string bidname = dataMap[itemid][2];

            // disable checkers... std::string::npos is a special return for when a substring is not found...
            // so, of .find _doesn't_ return npos, then it _is_ found
            // this is a heuristic.  Based on the bidname (like drk-up) we determine whether it's an upgrade, an accessory, or (if neither -up nor -acc) a city AF
            if (bidname.find("-up") != std::string::npos && !this->upgrade_enable) {
                std::string cmd = "Ignoring item  \x1f\xc8" + dataMap[itemid][0] + "\x1f\x82 as upgrades (-1 items) are disabled.";
                m_AshitaCore->GetChatManager()->Write(cmd.c_str());
                return false;
            }

            if (bidname.find("-acc") != std::string::npos && !this->acc_enable) {
                std::string cmd = "Ignoring item  \x1f\xc8" + dataMap[itemid][0] + "\x1f\x82 as accessories (belts/capes) are disabled.";
                m_AshitaCore->GetChatManager()->Write(cmd.c_str());
                return false;
            }

            if (bidname.find("-acc") == std::string::npos && bidname.find('-up') == std::string::npos && !this->city_enable) {
                std::string cmd = "Ignoring item \x1f\xc8" + dataMap[itemid][0] + "\x1f\x82 as city AF is disabled.";
                m_AshitaCore->GetChatManager()->Write(cmd.c_str());
                return false;
            }


            if (!bidMap.count(itemid)) {
                bidMap.insert(std::pair<int,int>(itemid, 0));
            }
            bidMap[itemid] += 1;

            bidname += "-" + std::to_string(bidMap[itemid]);
            
            if (freelots.count(itemid)) {
                // we already saw this item as a freelot.
                std::string cmd = "/l @@@ [" + bidname + "] ALREADY FREE LOT  (" + dataMap[itemid][3] + ")";
                message_queue.push(cmd);
                return false;
            }


            t1.deadline = std::chrono::system_clock::now() + std::chrono::seconds(60);
            t1.message = "@@@ [" + bidname + "] 60 seconds"; 
            t1.active = 1;

            t2.deadline = std::chrono::system_clock::now() + std::chrono::seconds(120);
            t2.message = "@@@ [" + bidname + "] BID TIME UP  (" + dataMap[itemid][3] + ")";
            t2.active = 1;

            new_data.active = 1; // make active
            new_data.itemid = itemid; // put itemid in...
            new_data.bid_entries.clear(); // clear the list if it hasn't been already...

            new_data.name = bidname;
            new_data.msg1 = t1;
            new_data.msg2 = t2;

            std::string cmd = "/l @@@ [" + bidname + "] 2 MINUTES  (" + dataMap[itemid][3] + ")  /tell " + std::string(m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0)) + " " + bidname + " yourbid";
            message_queue.push(cmd);

            bids.push_back(new_data); // put a copy on the vector here...

            return false;


        }
    }

    return false;
}



/**
 * Returns the interface version this plugin was compiled with.
 *
 * @returns {double} The Ashita interface version.
 */
__declspec(dllexport) double __stdcall GetInterfaceVersion(void)
{
    return ASHITA_INTERFACE_VERSION;
}

/**
 * Creates and populates the plugins information structure.
 *
 * @returns {double} The Ashita interface version.
 */
__declspec(dllexport) void __stdcall CreatePluginInfo(plugininfo_t* info)
{
    // Store the pointer from Ashita..
    g_PluginInfo = info;

    // Populate the structure with our plugins information..
    strcpy_s(info->Author, sizeof(info->Author), "Dajiba");
    strcpy_s(info->Name, sizeof(info->Name), "Wingz Bid Bot");
    info->InterfaceVersion = ASHITA_INTERFACE_VERSION;
    info->PluginVersion = 3.0f;
    info->Priority = 0;
}

/**
 * Creates an instance of the plugins main class.
 *
 * @returns {IPlugin*} The plugin base class instance created by this plugin.
 */
__declspec(dllexport) IPlugin* __stdcall CreatePlugin(void)
{
    return (IPlugin*)new bidbotPlugin;
}
