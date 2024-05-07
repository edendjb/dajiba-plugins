#define CPPHTTPLIB_OPENSSL_SUPPORT
#define _WIN32
#include "DDD.h"
#include "CommandParse.h"




DDD::DDD(void)
    : m_AshitaCore(nullptr)
    , m_PluginId(0)
    , m_CurrentZoneID(0)
    {
    this->playerNames           = std::vector<std::string>();
    this->playerMap             = std::map<std::string, std::map<std::string, int>>();
    this->NotesList             = std::vector<std::string>();
    this->afkPlayerList         = std::map<std::string, std::vector<std::string>>();
    this->helperPlayersList       = std::map<std::string, std::vector<std::string>>();
    this->REGION_SEARCH         = 0;
    this->windowPoints          = std::map<std::string, int>();
    this->playerForcedPoints    = std::map<std::string, int>();
    this->first_window_points   = 1;
    this->eventName             = "default";
    
    this->botName = "";
    this->webhook = "";
    initialize_region_map();
    }

DDD::~DDD(void)
{
}

plugininfo_t DDD::GetPluginInfo(void)
{
    return *g_PluginInformation;
}

bool DDD::Initialize(IAshitaCore* core, ILogManager * log, uint32_t id)
{
    this->m_AshitaCore = core;
    this->m_PluginId = id;
    blowfish = blowfish_t();
    memcpy(& key[0], & keys[0], 24);

    this->GetConfig();
    initialize_region_map();

    return true;
}

void DDD::Release(void)
{
}

bool DDD::PostToDiscord(std::string eventName, std::string windowName, std::string Notes) {
    httplib::SSLClient cli("discord.com");

    char datebuffer[80];
    auto mytime = std::time(nullptr);
    auto mytimep = *std::localtime(&mytime);
    strftime(datebuffer, sizeof(datebuffer), "%d-%m-%Y %H:%M:%S (%z)", &mytimep);

    std::string short_content = "```ansi\n\nDKP Report from ";
    short_content.append(m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0));

    short_content.append(" at ");
    short_content.append(datebuffer);
    short_content.append("\nEvent: ");
    short_content.append(eventName);
    short_content.append(",  Window: ");

    for (auto it = this->WindowList.begin(); it != this->WindowList.end(); ++it) {
        if (std::next(it) == this->WindowList.end()) {
            short_content.append(*it);
            short_content.append(" (current); ");
        }
        else {
            short_content.append(*it);
            short_content.append(", ");
        }
    }

    short_content.append("\n\n");

    for (auto& pair : this->playerMap) {
        std::string short_chart = "";
        char buf[128];
        sprintf(buf, "%-17s", pair.first.c_str());

        short_content.append(buf);

        int their_points = 0;

        for (auto w : WindowList) {
            int presence = pair.second[w];

            if (presence >= 1) {
                their_points += pair.second[w]; // to track total points collected
            }

            // if they're present on this window, give a green block
            if (presence >= 1) {
                //short_chart.append("Y");
                char pointsThisWindow[8]; // should only be one byte...
                sprintf(pointsThisWindow, "%d", pair.second[w]);
                short_chart.append(pointsThisWindow); // print number of points per window instead 
                continue;
            }
            if (presence == 0) {
                // they weren't present, default 0
                short_chart.append(" ");
                continue;
            }
            if (presence == -1) {
                // they were afk (-1 pushed in when the data was created)
                short_chart.append("A");
                continue;
            }
            if (presence == -2) {
                // they were on the claim team (-2 pushed in)
                short_chart.append("H");
            }
        }
        memset(buf, 0, 128);

        if (this->playerForcedPoints[pair.first] > 0) {
            their_points = this->playerForcedPoints[pair.first];
        }

        sprintf(buf, "%2d ", their_points);

        short_content.append(buf);
        short_content.append(short_chart);
       
        if (this->playerForcedPoints[pair.first] > 0) {
            short_content.append(" [f]");
        }

        short_content.append("\n");
    }

    short_content.append("\nNotes:\n");

    for (std::string noteString : this->NotesList) {
        short_content.append("-");
        short_content.append(noteString);
        short_content.append("\n");
    }

    short_content.append("```");

    std::string escaped_short_content = escape_json(short_content);

    std::string body = "{\"username\": \"";
    body.append(this->botName);
    body.append("\", \"content\": \"");
    body.append(escaped_short_content);
    body.append("\"}");

    m_AshitaCore->GetChatManager()->Writef("Size: %d -- Discord does not allow more than 2000.", strlen(body.c_str()));

    httplib::Headers headers = { { "Content-Type", "application/json" } };

    cli.enable_server_certificate_verification(false);


    std::string theUrl = this->webhook;
    theUrl.append("?wait=true");
    if (httplib::Result res = cli.Post(theUrl.c_str(), body.c_str(), "application/json")) {
        nlohmann::json j = nlohmann::json::parse(res->body.c_str());
        this->messageId = j["id"];
        return true;
    }
    else {
        std::string myerror = httplib::to_string(res.error());
        return true;
    }
}

bool DDD::UpdateDiscord(std::string eventName) {
    httplib::SSLClient cli("discord.com");

    char datebuffer[80];
    auto mytime = std::time(nullptr);
    auto mytimep = *std::localtime(&mytime);
    strftime(datebuffer, sizeof(datebuffer), "%d-%m-%Y %H:%M:%S (%z)", &mytimep);

    std::string short_content = "```ansi\n\nDKP Report from ";

    short_content.append(m_AshitaCore->GetDataManager()->GetParty()->GetMemberName(0));
    short_content.append(" at ");
    short_content.append(datebuffer);
    short_content.append(" (updated)");

    short_content.append("\nEvent: ");
    short_content.append(eventName);
    short_content.append(", Window: ");

    for (auto it = this->WindowList.begin(); it != this->WindowList.end(); ++it) {
        if (std::next(it) == this->WindowList.end()) {
            short_content.append(*it);
            short_content.append(" (current); ");
        }
        else {

            short_content.append(*it);
            short_content.append(", ");
        }
    }

    short_content.append("\n\n");

    for (auto& pair : this->playerMap) {
        std::string short_chart = "";
        char buf[128];

        sprintf(buf, "%-17s", pair.first.c_str());

        short_content.append(buf);

        int their_points = 0;

        // to save characters, only do color control when it differs from the last char
        int last_red = 0;
        int last_green = 0;
        int last_yellow = 0;
        for (auto w : WindowList) {
            int presence = pair.second[w];

            if (presence >= 1) {
                their_points += pair.second[w]; // to track total points collected
            }

            // if they're present on this window, give a green block
            if (presence >= 1) {
                char pointsThisWindow[8]; // should only be one byte...
                sprintf(pointsThisWindow, "%d", pair.second[w]);
                short_chart.append(pointsThisWindow); // print number of points per window instead 
                continue;
            }
            if (presence == 0) {
                // they weren't present, default 0
                short_chart.append(" ");
                continue;
            }
            if (presence == -1) {
                // they were afk (-1 pushed in when the data was created)
                short_chart.append("A");
                continue;
            }
            if (presence == -2) {
                // they were on the claim team (-2 pushed in)
                short_chart.append("H");
            }
        }

        memset(buf, 0, 128);

        if (this->playerForcedPoints[pair.first] > 0) {
            their_points = this->playerForcedPoints[pair.first];
        }

        sprintf(buf, "%2d ", their_points);

        short_content.append(buf);
        short_content.append(" ");
        short_content.append(short_chart);


        if (this->playerForcedPoints[pair.first] > 0) {
            short_content.append(" [f]");
        }
        short_content.append("\n");
    }
    short_content.append("\nNotes:\n\n");

    for (std::string noteString : this->NotesList) {
        short_content.append("-");
        short_content.append(noteString);
        short_content.append("\n");
    }

    short_content.append("```");

    std::string short_escaped_content = escape_json(short_content);

    std::string body = "{\"username\": \"";
        body.append(this->botName);
        body.append("\", \"content\": \"");
        body.append(short_escaped_content);
        body.append("\"}");

    m_AshitaCore->GetChatManager()->Writef("Size: %d -- Discord does not allow more than 2000.", strlen(body.c_str()));
    httplib::Headers headers = { { "Content-Type", "application/json" } };

    cli.enable_server_certificate_verification(false);

    std::string theUrl = this->webhook;
    theUrl.append("/messages/");
    theUrl.append(this->messageId);

    if (httplib::Result res = cli.Patch(theUrl.c_str(), body.c_str(), "application/json")) {
        return true;
    }
    else {
        this->m_AshitaCore->GetChatManager()->Write("patch failed.");
        return false;
    }
    return true;
}

bool DDD::GetPlayerData() {
    int sockfd, n, my_counter;
    uint8 buffer[2048];
    char msg[1024];
    struct sockaddr_in serv_addr;
    uint16 inclength; 

    my_counter = 0;

    sockfd = set_up_socket(m_AshitaCore, &serv_addr);

    if (sockfd != -1 && connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
        m_AshitaCore->GetChatManager()->Write("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 ERROR: Connection to search server failed.  Try again later.");
        return true;
    }
    else {
        if (this->REGION_SEARCH) {
            m_AshitaCore->GetChatManager()->Write("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 Sending query to /sea region.");
        }
        else {
            m_AshitaCore->GetChatManager()->Write("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 Sending query to /sea.");
        }
        
    }
    prepare_request(buffer, this->m_CurrentZoneID, this->REGION_SEARCH);
    encipher_data(buffer, sizeof(empty_request));

    if (send(sockfd, (const char*)buffer, sizeof(empty_request), 0) == -1) {
        m_AshitaCore->GetChatManager()->Write("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 ERROR: Failed to send /sea request to search server.");
        return true;
    }
    memset(buffer, 0x00, sizeof(buffer));
    if ((n = recv(sockfd, (char*)buffer, sizeof(buffer), 0)) == -1) {
        m_AshitaCore->GetChatManager()->Write("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 ERROR: Failed to read response from search server.");
        return true;
    }

    memcpy(&inclength, buffer, sizeof(uint16));
    decipher2(buffer, inclength);
    memcpy(&my_total, buffer + 0x0E, sizeof(uint16));
    uint16 players_remaining = my_total;

    if (my_total < 1 || my_total > 64) {
        m_AshitaCore->GetChatManager()->Write("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 WARNING: Found more than 64 players in zone.");
        m_AshitaCore->GetChatManager()->Write("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 WARNING: The search server will not list all of them.");
        m_AshitaCore->GetChatManager()->Write("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 WARNING: Double check the list of players.");
    }

    sprintf(msg, "\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 Found \x1f\07%d\x1f\x82 LS members in zone.", my_total);

    m_AshitaCore->GetChatManager()->Write(msg);

    players_remaining = printResultFromPacket(buffer, this, players_remaining);
    // careful of underflow/overflow on my_total...
    while (players_remaining > 0 && players_remaining < 1024 && my_counter < 4) {
        my_counter++;
        uint16 inclength;


        memset(buffer, 0x00, sizeof(buffer));
        memcpy(buffer, followup_request, sizeof(followup_request));


        encipher_data(buffer, sizeof(followup_request));

        if (send(sockfd, (const char*)buffer, sizeof(followup_request), 0) == -1) {
            m_AshitaCore->GetChatManager()->Write("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 ERROR: Failed to request continuation of player data.");
            return true;
        }

        // prepare for receipt
        memset(buffer, 0x00, sizeof(buffer));

        if ((n = recv(sockfd, (char*)buffer, sizeof(buffer), 0)) == -1) {
            m_AshitaCore->GetChatManager()->Write("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 ERROR: Failed to receive continuation of player data.");
            return true;
        }


        // parse length
        memcpy(&inclength, buffer, sizeof(uint16));


        // decipher incoming
        if (inclength > 0) {
            decipher2(buffer, inclength);


            players_remaining = printResultFromPacket(buffer, this, players_remaining);
        }
    }
    closesocket(sockfd);

    return true;
}


bool DDD::HandleCommand(const char * pszCommand, int nCommandType)
{
    if (pszCommand == nullptr) return false;
    char filename[256];
    
    char datebuffer[80];
    auto mytime = std::time(nullptr);
    auto mytimep = *std::localtime(&mytime);
    strftime(datebuffer, sizeof(datebuffer), "%d-%m-%Y %H:%M:%S (%z)", &mytimep);

    this->REGION_SEARCH = 0;

    if (_strnicmp(pszCommand, "/ddd", 3) == 0) {
        std::vector<std::string> args;
        Ashita::Commands::GetCommandArgs(pszCommand, &args);

        if (args.size() > 1 && args[1] == "help") {
            m_AshitaCore->GetChatManager()->Write("Usage: /ddd <post|summary|reset|update> <event_name> <w1,w2,...,w#> [-k] [-n \"optional notes\"]");
            m_AshitaCore->GetChatManager()->Write("     Do not put spaces unless you encapsulate things in \"quotes\".");
            m_AshitaCore->GetChatManager()->Write("     Example: /ddd post behemoth w2 \"Regret afk\"");
            m_AshitaCore->GetChatManager()->Write("You can use quotes to surround any parameter if you want spaces in it.");
            m_AshitaCore->GetChatManager()->Write("     Ex: /ddd post \"my behemoth d2\" \"window 1\" \"-regret +hat\" ");
            m_AshitaCore->GetChatManager()->Write("The \"summary\" function prints information in your window only");
            m_AshitaCore->GetChatManager()->Write("     (i.e., \"summary\" does not post to Discord)  ");
            m_AshitaCore->GetChatManager()->Write("Use /ddd notes \"more notes\" to add notes to most recent window without adding points.");
            m_AshitaCore->GetChatManager()->Write("The -k flag lets you post new notes (with -n \"notes\") without adding a window. ");
            m_AshitaCore->GetChatManager()->Write("The -h flag lets you specify a list of players on the helper team (default 2 point total). ");
            m_AshitaCore->GetChatManager()->Write("    (i.e., [-h Mistake Peyton] forces those players to get only 2 points for the event) ");
            m_AshitaCore->GetChatManager()->Write("All information is collected over time each time you invoke /ddd");
            m_AshitaCore->GetChatManager()->Write(" For any previous post, you can use \"update\" to edit the previous post with new data");
            
            return true;
        }

        if (args.size() > 1 && args[1] == "reset") {
            m_AshitaCore->GetChatManager()->Write("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 Erasing LS participation information.");
            
            this->playerMap.clear();
            this->playerNames.clear();
            this->messageId = "";
            this->NotesList.clear();
            this->WindowList.clear();
            this->afkPlayerList.clear();
            this->helperPlayersList.clear();
            this->playerForcedPoints.clear();
            this->windowPoints.clear();
            
            return true;
        }

        if (args.size() > 1 && args[1] == "summary") {
            m_AshitaCore->GetChatManager()->Write("Local report:");
            for (auto& pair : this->playerMap) {
                int points = 0;
                for (auto w : WindowList) {
                    points += pair.second[w];
                }
                char buf[80];
                sprintf(buf, " %-20s %2d", pair.first.c_str(), points);
                m_AshitaCore->GetChatManager()->Write(buf);
            }
            m_AshitaCore->GetChatManager()->Write("Nothing sent to discord (use update or post)");
            return true;
        }

        if (args.size() > 2 && args[1] == "notes") {
            std::string notes = "";
            for (int i = 2; i < args.size(); ++i) {
                notes.append(args[i]);
                notes.append(" ");
            }
            this->NotesList.push_back(notes);
            m_AshitaCore->GetChatManager()->Write("Updating notes only.");
            this->UpdateDiscord(this->eventName);
            return true;
        }

        if (args.size() < 4) {
            m_AshitaCore->GetChatManager()->Write("Usage: /ddd <post|summary|reset|update|notes> <event_name> <w1,w2,...,w#> [-k] [-n \"optional notes\"]");
            m_AshitaCore->GetChatManager()->Write("     Do not put spaces unless you encapsulate things in \"quotes\".");
            m_AshitaCore->GetChatManager()->Write("     Example: /ddd post behemoth w2 -n \"Regret afk\"");

            return true;
        }
        std::string notesArg = "";
        std::vector<std::string> afkPlayers;
        std::vector<std::string> helperPlayers;

        // for the first window, pull the value set from the configuration
        int pointsThisWindow = this->first_window_points;
        int forcedPoints = 0;
        int NOTES_ONLY = 0;

        if (args.size() >= 5) {
            int IN_FORCED = 0;
            int IN_NOTES = 0;
            int IN_AFK = 0;
            int NEW_WORD = 0; // track to capitalize first letter of player name to make comparison easier later
            int IN_POINTS = 0;
            int IN_HELPER = 0; // claim team

            for (int i = 4; i < args.size(); ++i) {
                if (args[i] == "-k") {
                    NOTES_ONLY = 1;
                }
                if (args[i] == "-r") {
                    this->REGION_SEARCH = 1;
                    continue;
                }
                if (args[i] == "-a") {
                    IN_NOTES = 0; IN_AFK = 1; NEW_WORD = 1; IN_POINTS = 0; IN_FORCED = 0; IN_HELPER = 0;
                    continue;
                }
                if (args[i] == "-h") {
                    IN_NOTES = 0; IN_AFK = 0; NEW_WORD = 1; IN_POINTS = 0; IN_FORCED = 0; IN_HELPER = 1;
                    continue;
                }
                if (args[i] == "-n") {
                    IN_NOTES = 1; IN_AFK = 0; NEW_WORD = 0; IN_POINTS = 0; IN_FORCED = 0; IN_HELPER = 0;
                    continue;
                }
                if (args[i] == "-p") {
                    IN_NOTES = 0; IN_AFK = 0; NEW_WORD = 0; IN_POINTS = 1; IN_FORCED = 0; IN_HELPER = 0;
                    continue;
                }
                if (args[i] == "-f") {
                    IN_NOTES = 0; IN_AFK = 0; NEW_WORD = 0; IN_POINTS = 0; IN_FORCED = 1; IN_HELPER = 0;
                    continue;
                }
                if (IN_POINTS == 1) {
                    int tmp = pointsThisWindow;
                    pointsThisWindow = atoi(args[i].c_str());
                    if (pointsThisWindow > 9 || pointsThisWindow < 0) {
                        m_AshitaCore->GetChatManager()->Writef("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 Specified points %d is invalid.", pointsThisWindow);
                        pointsThisWindow = tmp; // restore previous...
                    }
                }
                if (IN_FORCED == 1) {
                    forcedPoints = atoi(args[i].c_str());
                }
                if (IN_NOTES == 1) {
                    notesArg.append(args[i]);
                    notesArg.append(" ");
                    continue;
                }
                if (IN_AFK == 1) {
                    std::vector<std::string> words;
                    std::string curword = "";
                    for (auto &chr : args[i]) {
                        if (isalpha(chr)) {
                            if (NEW_WORD) {
                                curword += std::toupper(chr);
                                NEW_WORD = 0;
                            } else {
                                curword += std::tolower(chr);
                            }
                        } else {
                            words.push_back(curword);
                            curword = "";
                            NEW_WORD = 1;
                        }
                    }
                    words.push_back(curword);
                    NEW_WORD = 1;
                    for (auto w : words) {
                        m_AshitaCore->GetChatManager()->Writef("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 \x1f\x07%s\x1f\x82 marked AFK.", w.c_str());
                        afkPlayers.push_back(w);
                    }
                }
                if (IN_HELPER == 1) {
                    std::vector<std::string> words;
                    std::string curword = "";
                    for (auto& chr : args[i]) {
                        if (isalpha(chr)) {
                            if (NEW_WORD) {
                                curword += std::toupper(chr);
                                NEW_WORD = 0;
                            }
                            else {
                                curword += std::tolower(chr);
                            }
                        }
                        else {
                            words.push_back(curword);
                            curword = "";
                            NEW_WORD = 1;
                        }
                    }
                    words.push_back(curword);
                    NEW_WORD = 1;
                    for (auto w : words) {
                        m_AshitaCore->GetChatManager()->Writef("\x1f\xc8[\x1f\x05""DDD\x1f\xc8]\x1f\x82 \x1f\x07%s\x1f\x82 marked for helper team.", w.c_str());
                        helperPlayers.push_back(w);
                    }
                }
            }
        }

        if (args[1] == "post") {


            this->m_CurrentZoneID = m_AshitaCore->GetDataManager()->GetParty()->GetMemberZone(0);
            this->playerNames.clear();

            this->GetPlayerData();



            if (notesArg != "")
                this->NotesList.push_back(notesArg);

            if (afkPlayers.size() > 0) {
                this->afkPlayerList[args[3]] = afkPlayers;
            }
            if (helperPlayers.size() > 0) {
                this->helperPlayersList[args[3]] = helperPlayers;
            }

            if (NOTES_ONLY == 0) {
                this->WindowList.push_back(args[3]);
                for (auto thename : playerNames) {
                    int isafk = 0;
                    int ishelperteam = 0;
                    for (auto afkname : afkPlayers) {
                        if (afkname == thename) {
                            isafk = 1;
                            break;
                        }
                    }
                    for (auto helpername : helperPlayers) {
                        if (helpername == thename) {
                            ishelperteam = 1;
                            break;
                        }
                    }
                    if (!isafk && !ishelperteam) {
                        playerMap[thename][args[3]] = pointsThisWindow;
                    }
                    else {
                        if (isafk) {
                            playerMap[thename][args[3]] = -1;
                        }
                        else if (ishelperteam) {
                            playerMap[thename][args[3]] = -2;
                            playerForcedPoints[thename] = this->helper_team_points;
                        }
                    }
                    if (forcedPoints > 0 && !isafk && !ishelperteam) {
                        playerForcedPoints[thename] = forcedPoints;
                    }
                }

                this->windowPoints[args[3]] = pointsThisWindow;
            }
            this->eventName = args[2];


            if (this->PostToDiscord(args[2], args[3], notesArg)) {
                // update player map for future use.
                return true;
            }
            else {
                return false;
            }
            
        }
        if (args[1] == "update") {
            this->playerNames.clear();

            this->m_CurrentZoneID = m_AshitaCore->GetDataManager()->GetParty()->GetMemberZone(0);
            this->playerNames.clear();

            std::string windowName = args[3];
            this->eventName = args[2];
            if (NOTES_ONLY == 0) {
                for (auto w : this->WindowList) {
                    if (windowName == w) {
                        m_AshitaCore->GetChatManager()->Writef("\x1f\xc8[\x1f\x05""DDD\x1f\xc8] \x1f\x03""ERROR\x1f\x82: Window name \"%s\" already used.  Try again.", w.c_str());
                        return true;
                    }
                }
            }
            this->GetPlayerData();

            if (notesArg != "")
                this->NotesList.push_back(notesArg);

            if (afkPlayers.size() > 0) {
                for (auto p : afkPlayers) {
                    std::transform(p.begin(), p.end(), p.begin(), [](unsigned char c) {return std::tolower(c); });
                }
                this->afkPlayerList[windowName] = afkPlayers;
            }
            if (helperPlayers.size() > 0) {
                for (auto p : helperPlayers) {
                    std::transform(p.begin(), p.end(), p.begin(), [](unsigned char c) {return std::tolower(c); });
                }
                this->helperPlayersList[args[3]] = helperPlayers;
            }


            if (NOTES_ONLY == 0) {
                this->WindowList.push_back(windowName);
                for (auto thename : playerNames) {
                    int isafk = 0;
                    int ishelperteam = 0;
                    for (auto afkname : afkPlayers) {
                        if (afkname == thename) {
                            isafk = 1;
                            break;
                        }
                    }
                    for (auto helpername : helperPlayers) {
                        if (helpername == thename) {
                            ishelperteam = 1;
                            break;
                        }
                    }
                    if (!isafk && !ishelperteam) {
                        playerMap[thename][windowName] = pointsThisWindow;
                    }
                    else {
                        if (isafk) {
                            playerMap[thename][windowName] = -1;
                        }
                        else if (ishelperteam) {
                            playerMap[thename][windowName] = -2;
                            playerForcedPoints[thename] = this->helper_team_points;
                        }
                    }
                    if (forcedPoints > 0 && !isafk && !ishelperteam) {
                        playerForcedPoints[thename] = forcedPoints;
                    }
                }
                this->windowPoints[windowName] = pointsThisWindow;
            }
            this->UpdateDiscord(args[2]);
            return true;
        }
    }
    return false;
}


bool DDD::Direct3DInitialize(IDirect3DDevice8* lpDevice) { return true; }
void DDD::Direct3DRender(void) {}
bool DDD::HandleIncomingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked) {return false;}
bool DDD::HandleOutgoingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked) {return false;}


extern "C" {
    //__declspec(dllexport) double __stdcall GetIntferfaceVersion(void)
    __declspec(dllexport) double __stdcall GetInterfaceVersion(void)
        //__attribute__ ((visibility ("default") )) double __stdcall GetIntferfaceVersion(void)
    {
        return ASHITA_INTERFACE_VERSION;
    }

    __declspec(dllexport) void __stdcall CreatePluginInfo(plugininfo_t* info)
        //__attribute__ ((visibility ("default") )) void __stdcall CreatePluginInfo(plugininfo_t* info)
    {
        g_PluginInformation = info;
        strcpy_s(g_PluginInformation->Author, sizeof(info->Author), "Dajiba");
        strcpy_s(g_PluginInformation->Name, sizeof(info->Name), "DDD");
        g_PluginInformation->InterfaceVersion = ASHITA_INTERFACE_VERSION;
        g_PluginInformation->PluginVersion = 1.2f;
        g_PluginInformation->Priority = 0;
        
        // it gets here without trouble
    }

    __declspec(dllexport) IPlugin* __stdcall CreatePlugin(void)
        //__attribute__ ((visibility ("default") )) IPlugin* __stdcall CreatePlugin(void)
    {
        return (IPlugin*)new DDD();
    }
}
#pragma region GetConfig
void DDD::GetConfig() {
    auto iConfigResult = false;
    auto iTemp = 0;
    
    iConfigResult = m_AshitaCore->GetConfigurationManager()->Load("DDD", "DDD.xml");

    if (iConfigResult != 0)
    {
        this->botName = std::string(m_AshitaCore->GetConfigurationManager()->get_string("DDD", "botname"));
        this->webhook = std::string(m_AshitaCore->GetConfigurationManager()->get_string("DDD", "webhook"));
        this->first_window_points = m_AshitaCore->GetConfigurationManager()->get_int32("DDD", "first_window_points", 1);
        this->helper_team_points = m_AshitaCore->GetConfigurationManager()->get_int32("DDD", "helper_team_points", 2);
    }
    else {
        m_AshitaCore->GetConfigurationManager()->set_value("DDD", "botname", "DDDBot");
        m_AshitaCore->GetConfigurationManager()->set_value("DDD", "webhook", "your webhook");
        m_AshitaCore->GetConfigurationManager()->set_value("DDD", "first_window_points", "1");
        m_AshitaCore->GetConfigurationManager()->set_value("DDD", "helper_team_points", "2");

        this->botName = "DDDBot";
        this->webhook = "your webhook";
        m_AshitaCore->GetConfigurationManager()->Save("DDD", "DDD.xml");
    }    
}
#pragma endregion


