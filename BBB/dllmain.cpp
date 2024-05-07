// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "bbb.h"

struct myDelayThreader {
    IAshitaCore* m_AshitaCore;
} ;

void message_thread(const myDelayThreader& data) {
    
    while (true) {
        if (kill_thread) {
            break;
        }
        if (queued_messages.size() > 0) {
            data.m_AshitaCore->GetChatManager()->QueueCommand(queued_messages.front().c_str(), 1);

            queued_messages.pop();
        }
        ::Sleep(1050); // enough time to pass the /l limit
    }
}


void thread_helper(const myDelayThreader& data) {
    while (active_timer) {
        if (kill_thread) {
            break;
        }
        int time_elapsed = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count() - last_check_time;
        if (time_elapsed > 0) {
            if (time_list.size() > 0) {
                for (int i = 0; i < time_list.size(); ++i) {
                    // for each timer...
                    if (std::chrono::system_clock::now() >= time_list[i].tp && time_list[i].active == 1) {
                        time_list[i].active = 0;
                        std::string msg = "/l " + time_list[i].message + "\n";
                        data.m_AshitaCore->GetChatManager()->QueueCommand(msg.c_str(), 0);
                        // only do one...
                        break;
                    }
                }
                // remove inactive timers (ones that were just marked inactive after elapsing in the loop above) using .erase()
                time_list.erase(
                    std::remove_if(
                        time_list.begin(),
                        time_list.end(),
                        [](struct timer_data t) { return t.active == 0;  }),
                    time_list.end()
                );
            }
        }
        // run slightly more frequently to ensure timers pulse within .5s of scheduled time
        ::Sleep(500);
    }
}

/**
 * Constructor and Deconstructor
 */
bbbPlugin::bbbPlugin(void)
    : m_AshitaCore(nullptr)
    , m_LogManager(nullptr)
    , m_PluginId(0)
    , m_Direct3DDevice(nullptr)
{ }
bbbPlugin::~bbbPlugin(void)
{ 

}


/**
 * Returns the plugins information structure.
 *
 * @returns {plugininfo_t} The plugin information structure of the plugin.
 */
plugininfo_t bbbPlugin::GetPluginInfo(void)
{
    return *g_PluginInfo;
}

void initializeFromJson() {
    event_timers.clear();
    time_list.clear();

    httplib::Client cli("https://magic_url/manage-events.php");
    auto res = cli.Get("/events/");

    if (res->status != 200) {
        return;
    }

    nlohmann::json data = nlohmann::json::parse(res->body);

    for (auto& it : data.items()) {
        std::string event_name = it.key();
        event_timers[event_name] = event_data {
            it.value()["tod"].get<int>(),
            it.value()["windows"].get<int>(),
            it.value()["day"].get<int>(),
            it.value()["start"].get<int>(),
            it.value()["interval"].get<int>(),
            it.value()["alerts"].get<std::vector<int>>(),
            it.value()["custom"].get<int>()
        };

        // now, let's populate our vector times with the appropriate strings...
        // for this event, compute:
        //   tod + start
        // then, for however many windows there are:
        //   + interval
        // then, for each alert interval:
        //   - alerts... (becasue we want the alerts to happen _before_ the event itself, e.g., 30 seconds before W1)
        // for each of those times, determine if it's past now
        //  if the time is in the future, add it to our list


        int start_time = event_timers[event_name].tod + event_timers[event_name].start;
        for (int i = 0; i < event_timers[event_name].windows; ++i) {

            // put in a warning like " W1 in 60 minutes!"
            int warning_event_time = start_time + (event_timers[event_name].interval * (i-1)); // one interval before the actual time
            std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> warning_event_tp{ std::chrono::seconds {warning_event_time} };

            if (std::chrono::system_clock::now() < warning_event_tp) {
                // if it's in the future
                
                std::string time_til_msg = std::to_string(event_timers[event_name].interval) + "s";
                // make a string like "60s"
                if (event_timers[event_name].interval >= 60) {
                    // replace with "1min" as appropriate
                    time_til_msg = std::to_string(event_timers[event_name].interval / 60) + "min";
                }
                if (event_timers[event_name].interval == 0) {
                    time_til_msg = "NOW";
                }
                // actual message
                std::string msg = " ~~ [" + event_name + "] W" + std::to_string(i + 1) + " starting in " + time_til_msg + " ~~ ";
                if (event_timers[event_name].day > 0) {
                    msg += " (day " + std::to_string(event_timers[event_name].day) + ")";
                }
                // put it in the time_list for the helper thread to process
                time_list.push_back({ warning_event_tp, msg, 1 });
            }

            for (int j = 0; j < event_timers[event_name].alerts.size(); ++j) {
                // (tod + start) + (interval * window#) - (alert offset)
                int this_event_time = start_time + (event_timers[event_name].interval * i) - (event_timers[event_name].alerts[j]);
                std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> tp{ std::chrono::seconds{ this_event_time } };
                if (std::chrono::system_clock::now() < tp) {
                    // the computed alert time is in the future...
                    int time_til = event_timers[event_name].alerts[j];
                    std::string time_til_msg = std::to_string(time_til) + "s";
                    if (time_til >= 60) {
                        time_til_msg = std::to_string(time_til / 60) + "min";
                    }
                    if (time_til == 0) {
                        time_til_msg = "NOW";
                    }
                    std::string msg = " ~~ [" + event_name + "] W" + std::to_string( i + 1 ) + " starting in " + time_til_msg + " ~~ ";
                    
                    if (event_timers[event_name].day > 0) {
                        msg.append(" (day " + std::to_string(event_timers[event_name].day) + ")");
                    }
                    time_list.push_back({ tp, msg, 1 });
                }
                else {
                }
            }
        }
    }
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
bool bbbPlugin::Initialize(IAshitaCore* core, ILogManager* log, uint32_t id)
{
    // Store the variables for later usage..
    this->m_AshitaCore = core;
    this->m_LogManager = log;
    this->m_PluginId = id;
    

    allowed_mobs = {
        "Fafnir",
        "Nidhogg",
        "Behemoth",
        "King Behemoth",
        "King Arthro",
        "Aspidochelone",
        "Adamantoise"
    };

    mob_mapper["Fafnir"] = "fafnir";
    mob_mapper["Nidhogg"] = "fafnir";
    mob_mapper["faf"] = "fafnir";
    mob_mapper["fafnir"] = "fafnir";
    mob_mapper["nidhogg"] = "fafnir";
    mob_mapper["Behemoth"] = "behe";
    mob_mapper["behemoth"] = "behe";
    mob_mapper["King Behemoth"] = "behe";
    mob_mapper["kb"] = "behe";
    mob_mapper["hide daddy"] = "behe";
    mob_mapper["Aspidochelone"] = "turtle";
    mob_mapper["aspid"] = "turtle";
    mob_mapper["adamantoise"] = "turtle";
    mob_mapper["aspidochelone"] = "turtle";
    mob_mapper["toitle"] = "turtle";
    mob_mapper["Adamantoise"] = "turtle";
    mob_mapper["King Arthro"] = "ka";
    mob_mapper["crab"] = "ka";
    

    last_check_time = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count();

    active_timer = true;
    initializeFromJson();

    struct myDelayThreader td;
    td.m_AshitaCore = m_AshitaCore;
    my_thread = std::thread(thread_helper, td);// .detach();
    msg_thread = std::thread(message_thread, td);

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
void bbbPlugin::Release(void)
{
    kill_thread = true;
    my_thread.join();
    msg_thread.join();
}

//      /settod event_name tod [-c x] [-d x] [-n x] [-s x] [-i x] [-a x] [-w x]
//          0       1       2     3      4      5      6      7      8      9
// for LS:
// should be [1]<Dajiba> @settod event_name "x y" z
//                  0       1        2        3   4

// for /tell:
// should be Dajiba>> @settod event_name "x y" z
//              0         1      2          3  4

std::string bbbPlugin::HandleGetRequest(std::vector<std::string> & args, int offset) {
    int count = args.size();
    std::string path = "/w/e/?a=s";
    std::string res;

    
    if (count >= (2 + offset)) {
        std::replace(args[1 + offset].begin(), args[1 + offset].end(), ' ', '_');  // convert spaces to _

        path += "&e=" + args[1 + offset];
    }

    // for flexibility, people often want to do "/settod event date time tz"  with date, time, and tz space-delimited. 
    // the problem is that ashita's argparser will treat those as separate arguments
    // instead, we're going to have this flag_start variable to indicate when flags begin in the string
    // then, we'll use this if block to pick up args[2+offset], then search the rest of the args to see if any begin with a valid flag
    // if there's a flag later in the arg list, we'll set the flag_start to that index and continue from there
    int flag_start = 3 + offset;

    if (count >= (3 + offset)) {
        path += "&t=" + args[2 + offset];

        if (count >= (4 + offset)) {
            for (int i = (3+offset); i < count; ++i) {
                // if we see one of the supported flags
                if (args[i] == "-c" || args[i] == "-d" || args[i] == "-n" || args[i] == "-x" || args[i] == "-s" || args[i] == "-i" || args[i] == "-a" || args[i] == "-w") {
                    flag_start = i; // set the flag_start to that index
                    break; // break out of the loop
                }
                // otherwise, append it to the path, plus a space
                path += " " + args[i];
            }
        }


        //path += "&t=" + args[2 + offset];
    }
    // should have at least 2 more args in arg list starting at flag_start...
    // if flag_start is 3, then size should be at least 5 because index 3 is the flag, index 4 is the val
    // should also be an even number of remaining things in the arg list
    if (count >= flag_start + 2 && (count - flag_start) % 2 == 0) {
        //int my_count = 3 + offset;
        int my_count = flag_start;
        while (my_count < count) {
            std::string paramname = args[my_count];
            std::string paramval = args[my_count + 1];

            // claimer
            if (paramname == "-c") {
                path += "&c=" + paramval;
            }
            // day
            else if (paramname == "-d") {
                path += "&d=" + paramval;
            }
            // target name (e.g., Fafnir vs. Nidhogg)
            else if (paramname == "-n") {
                path += "&n=" + paramval;
            }
            // extra notes
            else if (paramname == "-x") {
                path += "&x=" + paramval;
            }
            // minimum wait time (usually set for built in mobs, 21 hours for HNM)
            else if (paramname == "-s") {
                path += "&start=" + paramval;
            }
            // interval (how much time between windows, usually built in, 30 mins for HNM)
            else if (paramname == "-i") {
                path += "&interval=" + paramval;
            }
            // alert spec (e.g., 180 60 30 10 5 for 3min, 1min, 30s, 10s, 5s before each window)
            else if (paramname == "-a") {
                path += "&alerts=" + paramval;
            }
            // how many windows (usually built in, 7 windows for HNM)
            else if (paramname == "-w") {
                path += "&windows=" + paramval;
            }

            my_count += 2;
        }
    }

    httplib::Client cli("https://magic url/manage-events.php");
    auto response = cli.Get(path);
    if (response->status == 200) {
        res = response->body;

        m_AshitaCore->GetChatManager()->Write(res.c_str());

        initializeFromJson();
        return res;
    }
    else {
        std::string res = "HTTP Request failed.";
        m_AshitaCore->GetChatManager()->Write("HTTP Request Failed.\n");
        return res;
    }
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
bool bbbPlugin::HandleCommand(const char* command, int32_t type)
{
    UNREFERENCED_PARAMETER(type);

    // Split the command into args..
    std::vector<std::string> args;
    auto count = Ashita::Commands::GetCommandArgs(command, &args);

    // Handle the example slash command..
    HANDLECOMMAND("/settod")
    {
        this->HandleGetRequest(args, 0);


    }
    HANDLECOMMAND("/disabletimers") {
        active_timer = false;
        my_thread.join();
        msg_thread.join();
        m_AshitaCore->GetChatManager()->Write("Timers disabled... can use /enabletimers.\n");
        
        return false;
    }
    HANDLECOMMAND("/enabletimers") {
        if (active_timer) {
            // already active...
            m_AshitaCore->GetChatManager()->Write("Timers already enabled... /disabletimers first.\n");
            return false;
        }
        active_timer = true;
        initializeFromJson();
        struct myDelayThreader td;
        td.m_AshitaCore = m_AshitaCore;
        my_thread = std::thread(thread_helper, td);
        msg_thread = std::thread(message_thread, td);

    }

    // Return false here to allow unhandled commands to continue to be processed.
    return false;
}


bool bbbPlugin::HandleIncomingText(int16_t mode, const char* message, int16_t* modifiedMode, char* modifiedMessage, bool blocked)
{
    //mode 14 = linkshell, mode 12 = tell, mode 6 = outgoing /l
    if (mode != 14 && mode != 12 && mode != 6) {
        return false;
    }

    std::vector<std::string> args;
    auto count = Ashita::Commands::GetCommandArgs(message, &args);

    // for LS:
    // should be [1]<Dajiba> stuff "x y" z
    //            0     1      2     3   4

    // for /tell:
    // should be Dajiba>> incoming "x y" z
    //              0         1      2   3
    int offset = 0;

    // param 0 is /settod, start with param 1
    // param 1 = event name
    if (count >= 2 + offset) {
        if (count >= 3 + offset) {
            if (args[1 + offset] == "@nm" && args[2 + offset] == "tod") {
                std::string res = HandleGetRequest(args, offset + 2);
                if (mode == 14 || mode == 6) {
                    std::string cmd = "/l " + res;
                    m_AshitaCore->GetChatManager()->QueueCommand(cmd.c_str(), 0);
                    return false;
                }
            }
        }
        if (args[1 + offset] == "@settod") {
            
            std::string res = "";
            if (mode == 14 || mode == 6) {
                res = HandleGetRequest(args, offset + 1);
            }
            
            if (mode == 14 || mode == 6) {
                std::string cmd = "/l " + res;
                m_AshitaCore->GetChatManager()->QueueCommand(cmd.c_str(), 0);
            }
            else {
                
                std::string recip = args[0];
                std::string compute_recip = "";
                for (int j = 0; j < recip.size(); ++j) {
                    if (recip[j] >= 0x41 && recip[j] <= 0x7a) {
                        compute_recip += recip[j];
                    }
                }
                
                std::string cmd = "/t " + compute_recip + " nope lol.";
                m_AshitaCore->GetChatManager()->QueueCommand(cmd.c_str(), 0);
            }
            return false;
        }
        if (   ( count >= 3 && (mode == 14 || mode == 6) && args[1] == "@gettod")   || (  count >= 4 && (mode == 14 || mode == 6) && args[1] == "@nm" && args[2] == "r")   ) {
            
            std::string event_name = args[2];
            if (args[1] == "@nm" && args[2] == "r") {
                event_name = args[3];
            }
            if (event_timers.count(event_name)) {
                int start_time = event_timers[event_name].tod + event_timers[event_name].start;

                auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                int64_t timeRemaining = start_time - currentTime;
                std::string cmd;
                if (timeRemaining > 0) {
                    int hours = 0;
                    int minutes = 0;
                    int seconds = timeRemaining;

                    if (seconds > 60) {
                        minutes = seconds / 60;
                        seconds = seconds % 60;
                    }
                    if (minutes > 60) {
                        hours = minutes / 60;
                        minutes = minutes % 60;
                    }

                    cmd = "/l ~~  [" + event_name + "]  ";
                    if (hours > 0) {
                        cmd += std::to_string(hours) + "h ";
                    }
                    if (minutes > 0) {
                        cmd += std::to_string(minutes) + "m ";
                    }
                    cmd += std::to_string(seconds) + "s ";
                    if (event_timers[event_name].windows > 0) {
                        cmd += "  until W1";
                    }
                    if (event_timers[event_name].day > 0) {
                        cmd += " (Day " + std::to_string(event_timers[event_name].day) + ")";
                    }
                    m_AshitaCore->GetChatManager()->QueueCommand(cmd.c_str(), 1);
                }
                else {
                    int reportedTime = 0;
                    for (int i = 0; i < event_timers[event_name].windows; ++i) {
                        int64_t timeRemaining = start_time + (i * event_timers[event_name].interval) - currentTime;
                        if (timeRemaining < 0) {
                            continue;
                        }
                        int days = 0;
                        int hours = 0;
                        int minutes = 0;
                        int seconds = timeRemaining;

                        if (seconds > 60) {
                            minutes = seconds / 60;
                            seconds = seconds % 60;
                        }
                        if (minutes > 60) {
                            hours = minutes / 60;
                            minutes = minutes % 60;
                        }
                        if (hours > 24) {
                            days = hours / 24;
                            hours = hours % 60;
                        }

                        cmd = "/l ~~  [" + event_name + "]  ";
                        if (days > 0) {
                            cmd += std::to_string(days) + "d ";
                        }
                        if (hours > 0) {
                            cmd += std::to_string(hours) + "h ";
                        }
                        if (minutes > 0) {
                            cmd += std::to_string(minutes) + "m ";
                        }
                        cmd += std::to_string(seconds) + "s ";
                        if (event_timers[event_name].windows > 0) {
                            cmd += "  until W" + std::to_string(i + 1);
                        }
                        if (event_timers[event_name].day > 0) {
                            cmd += " (Day " + std::to_string(event_timers[event_name].day) + ")";
                        }
                        m_AshitaCore->GetChatManager()->QueueCommand(cmd.c_str(), 1);
                        reportedTime = 1;
                        break;
                    }
                    if (!reportedTime) {
                        int64_t timeRemaining = start_time + (event_timers[event_name].windows * event_timers[event_name].interval) - currentTime;
                        
                        int days = 0;
                        int hours = 0;
                        int minutes = 0;
                        int seconds = 0 - timeRemaining; // invert

                        if (seconds > 60) {
                            minutes = seconds / 60;
                            seconds = seconds % 60;
                        }
                        if (minutes > 60) {
                            hours = minutes / 60;
                            minutes = minutes % 60;
                        }
                        if (hours > 24) {
                            days = hours / 24;
                            hours = hours % 60;
                        }


                        std::string cmd = "/l [" + event_name + "] is out of date. Last reported ";
                        if (days) {
                            cmd += std::to_string(days) + "d ";
                        }
                        if (hours) {
                            cmd += std::to_string(hours) + "h ";
                        }
                        if (minutes) {
                            cmd += std::to_string(minutes) + "m ";
                        }
                        cmd += std::to_string(seconds) + "s ";

                        cmd += " ago.";
                        m_AshitaCore->GetChatManager()->QueueCommand(cmd.c_str(), 1);
                    }
                }

            } else {
                std::string cmd = "/l [" + event_name + "] not found...";
                m_AshitaCore->GetChatManager()->QueueCommand(cmd.c_str(), 1);
            }
        }
    }
    

    return false;
}

uint16_t bbbPlugin::GetIndexFromId(int id)
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
bool bbbPlugin::HandleIncomingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked)
{
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
    strcpy_s(info->Name, sizeof(info->Name), "Wingz Event Timer");
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
    return (IPlugin*)new bbbPlugin;
}
