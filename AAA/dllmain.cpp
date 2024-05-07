// dllmain.cpp : Defines the entry point for the DLL application.

#include "pch.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "aaa.h"


/**
 * Constructor and Deconstructor
 */
aaaPlugin::aaaPlugin(void)
    : m_AshitaCore(nullptr)
    , m_LogManager(nullptr)
    , m_PluginId(0)
    , m_Direct3DDevice(nullptr)
{ }
aaaPlugin::~aaaPlugin(void)
{ }

std::vector<std::string> splitString(const std::string& str) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(';');

    auto isNotWhitespace = [](const std::string& s) {
        for (char c : s) {
            if (c != ' ' && c != '\t' && c != '\n') { // Check against common whitespace characters
                return true;
            }
        }
        return false;
        };

    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        if (isNotWhitespace(token)) {
            result.push_back(token);
        }
        start = end + 1;
        end = str.find(';', start);
    }

    // Check the last token after the last delimiter
    std::string lastToken = str.substr(start);
    if (isNotWhitespace(lastToken)) {
        result.push_back(lastToken);
    }

    return result;
}


std::pair<std::string, std::string> splitByColon(const std::string& str) {
    size_t colonPos = str.find(':');
    if (colonPos != std::string::npos) {
        return { str.substr(0, colonPos), str.substr(colonPos + 1) };
    }
    return { str, "" }; // In case there is no colon in the string
}

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

void aaaPlugin::PostToDiscord(std::string mob, std::string tod) {
    httplib::SSLClient cli("discord.com");
    std::string theUrl = this->discordendpoint;

    std::string post_content = "```ansi\n\nTOD Reported by ";
    post_content.append(m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0));
    post_content.append("\n");
    post_content.append("[\u001b[2;31m" + mob + "\u001b[0m]: ");
    post_content.append("\u001b[2;34m" + tod + "\u001b[0m\n```");

    std::string escaped = escape_json(post_content);

    std::string body = "";
    body.append("{\"username\": \"TimmyTOD");
    body.append("\", \"content\": \"");
    body.append(escaped);
    body.append("\"}");

    httplib::Headers headers = { { "Content-Type", "application/json" } };

    // probably not recommended lol
    cli.enable_server_certificate_verification(false);
    theUrl.append("?wait=true");

    if (httplib::Result res = cli.Post(theUrl.c_str(), body.c_str(), "application/json")) {
        nlohmann::json j = nlohmann::json::parse(res->body.c_str());
        std::string msgid = j["id"];
        m_AshitaCore->GetChatManager()->Writef("Discord Message ID: %s\n", msgid.c_str());
        return;
    }
    else {
        std::string myerror = httplib::to_string(res.error());
        m_AshitaCore->GetChatManager()->Writef("Error: %s\n", myerror.c_str());
        return;
    }


}



/**
 * Returns the plugins information structure.
 *
 * @returns {plugininfo_t} The plugin information structure of the plugin.
 */
plugininfo_t aaaPlugin::GetPluginInfo(void)
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
bool aaaPlugin::Initialize(IAshitaCore* core, ILogManager* log, uint32_t id)
{
    // Store the variables for later usage..
    this->m_AshitaCore = core;
    this->m_LogManager = log;
    this->m_PluginId = id;
    this->customMobs.clear();
    this->GetConfig();
    

    allowed_mobs = {
        "Fafnir",
        "Nidhogg",
        "Behemoth",
        "King Behemoth",
        "King Arthro",
        "Aspidochelone",
        "Adamantoise",
        "Cerberus",
        "Khimaira"
    };

    mob_mapper["Fafnir"]        = "fafnir";
    mob_mapper["Nidhogg"]       = "fafnir";
    mob_mapper["Behemoth"]      = "behe";
    mob_mapper["King Behemoth"] = "behe";
    mob_mapper["Aspidochelone"] = "turtle";
    mob_mapper["Adamantoise"]   = "turtle";
    mob_mapper["King Arthro"]   = "ka";
    mob_mapper["Cerberus"]      = "cerb";
    mob_mapper["Khimaira"]      = "khim";


    // custom mob names are placed in a  "shortname:fullname;" format, e.g.:
    // "argus:Argus;spicymob:Spicy Mob;"
    for (std::string mobName : this->customMobs) {
        std::string shortname, fullname;
        std::pair<std::string, std::string> namepair = splitByColon(mobName);
        shortname = namepair.first;
        fullname = namepair.second;
        if (fullname == "")
            fullname = shortname;

        mob_mapper[fullname] = shortname;
    }

    // timezone detection requires a big load of crap configuration
    // files placed in a special tzdata directory
    date::set_install(std::string(m_AshitaCore->GetAshitaInstallPathA()) + "\\tzdata");

    this->lastTimeStamp = 0;

    // this is the time zone we want to target (alertaru is eastern...)
    zone = date::locate_zone("America/New_York");

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
void aaaPlugin::Release(void)
{ }



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
bool aaaPlugin::HandleCommand(const char* command, int32_t type)
{
    std::vector<std::string> args;
    Ashita::Commands::GetCommandArgs(command, &args);

    HANDLECOMMAND("/aaa") {
        if (!this->usenewtimer) {
            this->usenewtimer = 1;
            m_AshitaCore->GetChatManager()->Write("   usenewtimer=1");
        } else {
            this->usenewtimer = 0;
            m_AshitaCore->GetChatManager()->Write("   usenewtimer=0");
        }
        return false;
    }

    return false;
}


bool aaaPlugin::HandleIncomingText(int16_t mode, const char* message, int16_t* modifiedMode, char* modifiedMessage, bool blocked)
{

    return false;
}

uint16_t aaaPlugin::GetIndexFromId(int id)
{
    auto entMgr = m_AshitaCore->GetDataManager()->GetEntity();
    


    for (int i = 0; i < 0x1800; i++)
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
bool aaaPlugin::HandleIncomingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked)
{
    if (id == 0x38) {
        int curtime = std::time(NULL);
        int diff = curtime - this->lastTimeStamp;
        if (diff < 2) {
            // bail out if we got one within 2 seconds... (e.g., if a mass of mobs die at around the same time)
            return false;
        }

        uint16_t targetid;
        memcpy(&targetid, (char*)data + 0x10 + 1, sizeof(uint16_t));

        auto entMgr = m_AshitaCore->GetDataManager()->GetEntity();
        Ashita::FFXI::ffxi_entity_t* targetEnt = entMgr->GetEntity(targetid);
        if (targetEnt == NULL) {
            m_AshitaCore->GetChatManager()->Write("Could not find target mob.\n");
            return false;
        }

        std::string targetName((char*)entMgr->GetEntity(targetid)->Name);

        if (mob_mapper.count(targetName) && entMgr->GetEntity(targetid)->SpawnFlags == 0x0010) {
            m_AshitaCore->GetChatManager()->Write("Mob death animation (0x38) path.\n");

            const char* cmd = "/l @nm tod %s %s\n";

            char fullcmd[1024];

            // get current time
            auto mytime = date::make_zoned(date::current_zone(), std::chrono::floor<std::chrono::seconds>(
                std::chrono::system_clock::now() +
                std::chrono::seconds(16)
            )
            );

            auto thetime = date::make_zoned(zone, mytime);

            std::string thedate = date::format("%m/%d/%Y %H:%M:%S", thetime);

            std::string mobname;
            if (mob_mapper.count(targetName)) {
                mobname = mob_mapper[targetName];
            }
            else {
                mobname = targetName;
            }
            sprintf_s(fullcmd, 1024, cmd, mobname.c_str(), thedate.c_str());

            std::string newcmd = "/l @settod " + mob_mapper[targetName] + " \"" + thedate + " ET\" ";
            if (targetName == "Nidhogg" || targetName == "Aspidochelone" || targetName == "King Behemoth") {
                // auto set to day 1
                newcmd.append("-d 1");
            }
            else if (targetName == "Fafnir" || targetName == "Adamantoise" || targetName == "Behemoth") {
                // day tracking doesn't really matter otherwise
                newcmd.append("-d +1");
            }

            m_AshitaCore->GetChatManager()->Writef("Formed command.  Executing %s.\n", fullcmd);

            if (!usenewtimer)
                m_AshitaCore->GetChatManager()->QueueCommand(fullcmd, 1);
            else
                m_AshitaCore->GetChatManager()->QueueCommand(newcmd.c_str(), 1);

            if (do_discord) {
                PostToDiscord(targetName, thedate + " (Eastern Time)");
            }

            // completed 
            this->lastTimeStamp = curtime;

        }

    }


    // message_basic
    if (id == 0x29) {
        
        int curtime = std::time(NULL);
        int diff = curtime - this->lastTimeStamp;
        if (diff < 2) {
            // bail out if we got one within 2 seconds... (e.g., if a mass of mobs die at around the same time)
            return false;
        }
        
        // MSGBASIC_DEFEATS = 6
        // MSGBASIC_FALLS   = 20

        uint16_t msgid;
        uint32_t sender;
        uint32_t target;

        uint16_t targetid;

        memcpy(&msgid, (char*)data + 0x18, sizeof(uint16_t));
        memcpy(&sender, (char*)data + 0x04, sizeof(uint32_t));
        memcpy(&target,  (char*)data + 0x08, sizeof(uint32_t));


        // death message
        if (msgid == 6 || msgid == 20) {
            std::string playerName;
            std::string targetName;
            const char *pname = m_AshitaCore->GetDataManager()->GetEntity()->GetName(this->GetIndexFromId(sender));
            const char *tname = m_AshitaCore->GetDataManager()->GetEntity()->GetName(this->GetIndexFromId(target));



            if (pname == NULL) {
                playerName = "(unknown player)";
                if (msgid == 6) {
                    // there should be a player name in this case (they defeat the target)
                    m_AshitaCore->GetChatManager()->Writef("Could not find player for ID %x.\n", sender);
                }
            }
            else {
                playerName = std::string(pname);
            }
            if (tname == NULL) {
                // there should always be a target for these two message types... defeats or falls to the ground
                targetName = "(unknown target)";
                m_AshitaCore->GetChatManager()->Writef("Could not find target for ID %x.  No.\n", target);
                //return false;
            }
            else {
                targetName = std::string(tname);
            }
            
            
            if (!mob_mapper.count(targetName)) {
                // skip death message for this mob
                m_AshitaCore->GetChatManager()->Writef("Could not find %s in allowed mobs.\n", targetName.c_str());
                return false;
            }
            
            m_AshitaCore->GetChatManager()->Write("Mob death message (0x29) path.\n");

            const char* cmd = "/l @nm tod %s %s\n";

            char fullcmd[1024];
            
            // get current time
            auto mytime  = date::make_zoned(date::current_zone(), std::chrono::floor<std::chrono::seconds>(
                                                                    std::chrono::system_clock::now() + 
                                                                    std::chrono::seconds(16)
                                                                  )
                                           ) ;
            
            auto thetime = date::make_zoned(zone, mytime);

            std::string thedate = date::format("%m/%d/%Y %H:%M:%S", thetime);

            std::string mobname;
            if (mob_mapper.count(targetName)) {
                mobname = mob_mapper[targetName];
            }
            else {
                mobname = targetName;
            }
            sprintf_s(fullcmd, 1024, cmd, mobname.c_str(), thedate.c_str());

            std::string newcmd = "/l @settod " + mob_mapper[targetName] + " \"" + thedate + " ET\" ";
            if (targetName == "Nidhogg" || targetName == "Aspidochelone" || targetName == "King Behemoth") {
                // auto set to day 1
                newcmd.append("-d 1");
            }
            else if (targetName == "Fafnir" || targetName == "Adamantoise" || targetName == "Behemoth") {
                // day tracking doesn't really matter otherwise
                newcmd.append("-d +1");
            }

            m_AshitaCore->GetChatManager()->Writef("Formed command.  Executing %s.\n", fullcmd);

            if (!usenewtimer)
                m_AshitaCore->GetChatManager()->QueueCommand(fullcmd, 1);
            else
                m_AshitaCore->GetChatManager()->QueueCommand(newcmd.c_str(), 1);
            
            if (do_discord) {
                PostToDiscord(targetName, thedate + " (Eastern Time)");
            }
            
            // completed 
            this->lastTimeStamp = curtime;
        }
    }
    return false;
}



void aaaPlugin::GetConfig() {
    auto iConfigResult = false;
    auto iTemp = 0;

    iConfigResult = m_AshitaCore->GetConfigurationManager()->Load("AAA", "AAA.xml");

    if (iConfigResult != 0)
    {
        this->usenewtimer = m_AshitaCore->GetConfigurationManager()->get_int32("AAA", "usenewtimer", 1);
        this->customMobs = splitString(m_AshitaCore->GetConfigurationManager()->get_string("AAA", "customMobs"));
        this->do_discord = m_AshitaCore->GetConfigurationManager()->get_int32("AAA", "posttodiscord", 1);
        this->discordendpoint = m_AshitaCore->GetConfigurationManager()->get_string("AAA", "discordendpoint");

        
    }
    else {
        m_AshitaCore->GetConfigurationManager()->set_value("AAA", "usenewtimer", "1");
        m_AshitaCore->GetConfigurationManager()->set_value("AAA", "posttodiscord", "1");
        m_AshitaCore->GetConfigurationManager()->set_value("AAA", "customMobs", "");
        m_AshitaCore->GetConfigurationManager()->set_value("AAA", "discordendpoint", "ur webhook");
        m_AshitaCore->GetConfigurationManager()->Save("AAA", "AAA.xml");

        this->usenewtimer = 1;
        this->customMobs.clear();
        this->do_discord = 1;
        this->discordendpoint = "ur webhook";

    }
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
    strcpy_s(info->Name, sizeof(info->Name), "Wingz TOD Reporter");
    info->InterfaceVersion = ASHITA_INTERFACE_VERSION;
    info->PluginVersion = 1.0f;
    info->Priority = 0;
}

/**
 * Creates an instance of the plugins main class.
 *
 * @returns {IPlugin*} The plugin base class instance created by this plugin.
 */
__declspec(dllexport) IPlugin* __stdcall CreatePlugin(void)
{
    return (IPlugin*)new aaaPlugin;
}
