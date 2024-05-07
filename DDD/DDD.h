#ifndef DDD_H
#define DDD_H

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#include "json.h"
#include "./ADK/Ashita.h"
#include <tuple>
#include <time.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <map>


#include <stdio.h>
#include "HTTPRequest.hpp"
#include "./common/blowfish.h"
#include "./common/md52.h"
#include "zone.h"

#include <cstring>
#include <string.h>


#define REMOTE_IP "1.2.3.4" // eden search server
#define REMOTE_PORT 1234 // eden search server port
#define BUFFER_SIZE 1024

#define LS_ID 000000 // ls ID here...






enum SEARCHTYPE
{
    SEARCH_NAME          = 0x00, // 00000
    SEARCH_AREA          = 0x01, // 00001
    SEARCH_NATION        = 0x02, // 00010
    SEARCH_JOB           = 0x03, // 00011
    SEARCH_LEVEL         = 0x04, // 00100
    SEARCH_RACE          = 0x05, // 00101
    SEARCH_FLAGS1        = 0x06, // 00110
    SEARCH_ID            = 0x08, // 01000
    SEARCH_PARTY         = 0x0A, // 01010
    SEARCH_LINKSHELL     = 0x0B, // 01011
    SEARCH_FRIEND        = 0x0C, // 01100
    SEARCH_LINKSHELLRANK = 0x0D, // 01101
    SEARCH_UNK0x0E       = 0x0E, // 01110
    SEARCH_RANK          = 0x10, // 10000
    SEARCH_COMMENT       = 0x11, // 10001
    SEARCH_FLAGS2        = 0x14, // 10100
    SEARCH_LANGUAGE      = 0x15, // 10101
};



uint8 empty_request[] = {
0x4C, 0x00, 0x00, 0x00, 0x49, 0x58, 0x46, 0x46,
0x1A, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};



// duplicate of the packet sent after each batch of results is received... hope it works?
// almost works, but it's likely we need to zero out key material here.  
// in the orig decipher, server will take key[20] = request_data[ length
// - 0x18 ]....  I think -0x18 bytes back is a 4 byte portion that
// should be zeroed to make this easier.
uint8 followup_request[]{
0x4C, 0x00, 0x00, 0x00, 0x49, 0x58, 0x46, 0x46,
0x10, 0x00, 0x80, 0x10, 0x00, 0x00, 0x00, 0x00,
0x56, 0x3F, 0xDD, 0x0C, 0x56, 0x3F, 0xDD, 0x0C,
0x56, 0x3F, 0xDD, 0x0C, 0x56, 0x3F, 0xDD, 0x0C,
0x56, 0x3F, 0xDD, 0x0C, 0x56, 0x3F, 0xDD, 0x0C,
0x56, 0x3F, 0xDD, 0x0C, 0x56, 0x3F, 0xDD, 0x0C,
0x56, 0x3F, 0xDD, 0x0C, 0x00, 0x00, 0x00, 0x00,
0x9a, 0x02, 0x95, 0x3f, 0x45, 0x62, 0x09, 0xea,
0x47, 0x99, 0xbb, 0x35, 0x33, 0xdc, 0x8d, 0xaa,
0x00, 0x00, 0x00, 0x00
};





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







//(un)pack functions for Big Endian(BE) targets
uint32 packBitsBE(uint8* target, uint64 value, int32 byteOffset, int32 bitOffset, uint8 lengthInBit);
uint32 packBitsBE(uint8* target, uint64 value, int32 bitOffset, uint8 lengthInBit);
uint64 unpackBitsBE(uint8* target, int32 byteOffset, int32 bitOffset, uint8 lengthInBit);
uint64 unpackBitsBE(uint8* target, int32 bitOffset, uint8 lengthInBit);
//(un)pack functions for Little Endian(LE) targets
uint32 packBitsLE(uint8* target, uint64 value, int32 byteOffset, int32 bitOffset, uint8 lengthInBit);
uint32 packBitsLE(uint8* target, uint64 value, int32 bitOffset, uint8 lengthInBit);
uint64 unpackBitsLE(uint8* target, int32 bitOffset, uint8 lengthInBit);
uint64 unpackBitsLE(const uint8* target, int32 byteOffset, int32 bitOffset, uint8 lengthInBit);


// most of this code was copied from the existing ADK code and/or the
// DarkStar repos...
/************************************************************************
 *                                                                       *
 *  Methods for working with bit arrays.                 								*
 *                                                                       *
 ************************************************************************/

int32 hasBit(uint16 value, const uint8* BitArray, uint32 size)
{
    if (value >= size * 8)
    {
        //ShowError(CL_RED "hasBit: value (%u) is out of range\n" CL_RESET, value);
        return 0;
    }
    return (int32)(BitArray[value >> 3] & (1 << (value % 8)));
}

int32 addBit(uint16 value, uint8* BitArray, uint32 size)
{
    if (!hasBit(value, BitArray, size) && (value < size * 8))
    {
        BitArray[value >> 3] |= (1 << (value % 8));
        return 1;
    }
    return 0;
}

int32 delBit(uint16 value, uint8* BitArray, uint32 size)
{
    if (hasBit(value, BitArray, size))
    {
        BitArray[value >> 3] &= ~(1 << (value % 8));
        return 1;
    }
    return 0;
}

uint32 packBitsBE(uint8* target, uint64 value, int32 bitOffset, uint8 lengthInBit)
{
    return packBitsBE(target, value, 0, bitOffset, lengthInBit);
}

uint32 packBitsBE(uint8* target, uint64 value, int32 byteOffset, int32 bitOffset, uint8 lengthInBit)
{
    byteOffset += (bitOffset >> 3); // correct bitOffsets>=8
    bitOffset %= 8;

    uint64 bitmask = 0xFFFFFFFFFFFFFFFFLL; // Generate bitmask

    bitmask >>= (64 - lengthInBit);
    bitmask <<= bitOffset;

    value <<= bitOffset; // shift value
    value &= bitmask;

    bitmask ^= 0xFFFFFFFFFFFFFFFFLL; // invert bitmask

    if ((lengthInBit + bitOffset) <= 8) // write shifted value to target
    {
        uint8* dataPointer = (uint8*)&target[byteOffset];

        uint8 bitmaskUC = (uint8)bitmask;
        uint8 valueUC   = (uint8)value;

        *dataPointer &= bitmaskUC;
        *dataPointer |= valueUC;
    }
    else if ((lengthInBit + bitOffset) <= 16)
    {
        uint16* dataPointer = (uint16*)&target[byteOffset];

        uint16 bitmaskUC = (uint16)bitmask;
        uint16 valueUC   = (uint16)value;

        *dataPointer &= bitmaskUC;
        *dataPointer |= valueUC;
    }
    else if ((lengthInBit + bitOffset) <= 32)
    {
        uint32* dataPointer = (uint32*)&target[byteOffset];

        uint32 bitmaskUC = (uint32)bitmask;
        uint32 valueUC   = (uint32)value;

        *dataPointer &= bitmaskUC;
        *dataPointer |= valueUC;
    }
    else if ((lengthInBit + bitOffset) <= 64)
    {
        uint64* dataPointer = (uint64*)&target[byteOffset];

        *dataPointer &= bitmask;
        *dataPointer |= value;
    }
    else
    {
        //ShowError("Pack Bits Error: packBitsBE(...) not implemented for targetsizes above 64 bits.\n Targetsize: %d\n", (lengthInBit + bitOffset));
    }
    return ((byteOffset << 3) + bitOffset + lengthInBit);
}

uint64 unpackBitsBE(uint8* target, int32 bitOffset, uint8 lengthInBit)
{
    return unpackBitsBE(target, 0, bitOffset, lengthInBit);
}

uint64 unpackBitsBE(uint8* target, int32 byteOffset, int32 bitOffset, uint8 lengthInBit)
{
    byteOffset += (bitOffset >> 3);
    bitOffset %= 8;

    uint64 bitmask = 0xFFFFFFFFFFFFFFFFLL;

    bitmask >>= (64 - lengthInBit);
    bitmask <<= bitOffset;

    uint64 retVal;

    if ((lengthInBit + bitOffset) <= 8)
    {
        uint8* dataPointer = (uint8*)&target[byteOffset];

        retVal = ((*dataPointer) & (uint8)bitmask) >> bitOffset;
    }
    else if ((lengthInBit + bitOffset) <= 16)
    {
        uint16* dataPointer = (uint16*)&target[byteOffset];

        retVal = ((*dataPointer) & (uint16)bitmask) >> bitOffset;
    }
    else if ((lengthInBit + bitOffset) <= 32)
    {
        uint32* dataPointer = (uint32*)&target[byteOffset];

        retVal = ((*dataPointer) & (uint32)bitmask) >> bitOffset;
    }
    else if ((lengthInBit + bitOffset) <= 64)
    {
        uint64* dataPointer = (uint64*)&target[byteOffset];

        retVal = ((*dataPointer) & (uint64)bitmask) >> bitOffset;
    }
    else
    {
        //ShowError("Unpack Bits Error: unpackBits(...) not implemented for targetsizes above 64 bits.\n Targetsize: %d\n", (lengthInBit + bitOffset));
        return 0;
    }
    return retVal;
}

uint32 packBitsLE(uint8* target, uint64 value, int32 bitOffset, uint8 lengthInBit)
{
    return packBitsLE(target, value, 0, bitOffset, lengthInBit);
}

uint32 packBitsLE(uint8* target, uint64 value, int32 byteOffset, int32 bitOffset, uint8 lengthInBit)
{
    byteOffset += (bitOffset >> 3); // correct bitOffsets >= 8
    bitOffset %= 8;

    uint8 bytesNeeded; // calculate how many bytes are needed
    if ((bitOffset + lengthInBit) <= 8)
    {
        bytesNeeded = 1;
    }
    else if ((bitOffset + lengthInBit) <= 16)
    {
        bytesNeeded = 2;
    }
    else if ((bitOffset + lengthInBit) <= 32)
    {
        bytesNeeded = 4;
    }
    else if ((bitOffset + lengthInBit) <= 64)
    {
        bytesNeeded = 8;
    }
    else
    {
        //ShowError("Pack Bits Error: packBitsLE(...) not implemented for targetsizes above 64 bits.\n Targetsize: %d\n", (lengthInBit + bitOffset));
        return 0;
    }

    uint8* modifiedTarget = new uint8[bytesNeeded]; // convert byteOrder to Big Endian

    for (uint8 curByte = 0; curByte < bytesNeeded; ++curByte)
    {
        modifiedTarget[curByte] = target[byteOffset + (bytesNeeded - 1) - curByte];
    }

    int32 newBitOffset = (bytesNeeded << 3) - (bitOffset + lengthInBit); // calculate new bitOffset

    packBitsBE(&modifiedTarget[0], value, 0, newBitOffset, lengthInBit); // write data to modified array

    for (uint8 curByte = 0; curByte < bytesNeeded; ++curByte) // copy back to target
    {
        target[byteOffset + (bytesNeeded - 1) - curByte] = modifiedTarget[curByte];
    }

    {
        delete[] modifiedTarget;
    }
    return ((byteOffset << 3) + bitOffset + lengthInBit);
}

uint64 unpackBitsLE(uint8* target, int32 bitOffset, uint8 lengthInBit)
{
    return unpackBitsLE(target, 0, bitOffset, lengthInBit);
}

uint64 unpackBitsLE(const uint8* target, int32 byteOffset, int32 bitOffset, uint8 lengthInBit)
{
    byteOffset += (bitOffset >> 3);
    bitOffset %= 8;

    uint8 bytesNeeded;
    if ((bitOffset + lengthInBit) <= 8)
    {
        bytesNeeded = 1;
    }
    else if ((bitOffset + lengthInBit) <= 16)
    {
        bytesNeeded = 2;
    }
    else if ((bitOffset + lengthInBit) <= 32)
    {
        bytesNeeded = 4;
    }
    else if ((bitOffset + lengthInBit) <= 64)
    {
        bytesNeeded = 8;
    }
    else
    {
        //ShowError("Unpack Bits Error: packBitsLE(...) not implemented for targetsizes above 64 bits.\n Targetsize: %d\n", (lengthInBit + bitOffset));
        return 0;
    }

    uint64 retVal;

    uint8* modifiedTarget = new uint8[bytesNeeded];

    for (uint8 curByte = 0; curByte < bytesNeeded; ++curByte)
    {
        modifiedTarget[curByte] = target[byteOffset + (bytesNeeded - 1) - curByte];
    }
    if (bytesNeeded == 1)
    {
        uint8 bitmask = 0xFF >> bitOffset;
        retVal        = (modifiedTarget[0] & bitmask) >> (8 - (lengthInBit + bitOffset));
    }
    else
    {
        int32 newBitOffset = (bytesNeeded * 8) - (bitOffset + lengthInBit);
        retVal             = unpackBitsBE(&modifiedTarget[0], 0, newBitOffset, lengthInBit);
    }

    {
        delete[] modifiedTarget;
    }
    return retVal;
}




blowfish_t blowfish;
uint8 key[24];

uint16 my_total;

// key redacted...
uint8 keys[24] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



void encipher_data(uint8* data, uint32 length) {
    md5((uint8*)(key), blowfish.hash, 20);

    blowfish_init((int8*)blowfish.hash, 16, blowfish.P, blowfish.S[0]);

    md5(data + 8, data + length - 0x18 + 0x04, length - 0x18 - 0x04);

    uint8 tmp = (length - 12) / 4;
    tmp -= tmp % 2;

    for (uint8 i = 0; i < tmp; i += 2) {
        blowfish_encipher((uint32*)data + i + 2, (uint32*)data + i + 3, blowfish.P, blowfish.S[0]);
    }
}







void __stdcall decipher2(uint8* data, uint32 length)
{
    md5((uint8*)(key), blowfish.hash, 24);
    blowfish_init((int8*)blowfish.hash, 16, blowfish.P, blowfish.S[0]);

    char outstr[2048];
    snprintf(outstr, 2048, "data: %x,  length: %x\n");

    md5(data + 8, data + length - 0x18 + 0x04, length - 0x18 - 0x04);


    uint8 tmp = (length - 12) / 4;
    tmp -= tmp % 2;

    for (uint8 i = 0; i < tmp; i += 2)
    {
        blowfish_decipher((uint32*)data + i + 2, (uint32*)data + i + 3, blowfish.P, blowfish.S[0]);
    }

    // update key
    memcpy(key + 16, &data[length] - 0x04, 4);
}



int set_up_socket(IAshitaCore* m_AshitaCore, struct sockaddr_in *serv_addr) {
    // set up socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        m_AshitaCore->GetChatManager()->Write("ERROR socket creation.");
        return -1;
    }


    memset(serv_addr, 0, sizeof(struct sockaddr_in));

    serv_addr->sin_family = AF_INET;
    serv_addr->sin_addr.s_addr = inet_addr(REMOTE_IP);
    serv_addr->sin_port = htons(REMOTE_PORT);

    return sockfd;
}


void prepare_request(uint8* buffer, int area_code, int REGION_SEARCH) {
    int bitoffset = 0;
    memset(buffer, 0, 2048);


    // initialize buffer
    memcpy(buffer, empty_request, sizeof(empty_request));


    // default empty name search (not present, ascending order...)
    packBitsLE(&buffer[0x11], SEARCH_NAME, bitoffset, 5);
    bitoffset += 5;
    packBitsLE(&buffer[0x11], 0x00, bitoffset, 1);
    bitoffset += 1;
    packBitsLE(&buffer[0x11], 0x00, bitoffset, 1);
    bitoffset += 1;

    // present search
    packBitsLE(&buffer[0x11], SEARCH_AREA, bitoffset, 5);
    bitoffset += 5;
    packBitsLE(&buffer[0x11], 0x00, bitoffset, 1);
    bitoffset += 1;
    packBitsLE(&buffer[0x11], 0x01, bitoffset, 1);
    bitoffset += 1;

    // encode search target area
    packBitsLE(&buffer[0x11], area_code, bitoffset, 10);
    bitoffset += 10;


    if (REGION_SEARCH == 1) {
        int whichRegion = GetCurrentRegion(area_code);
        std::vector<int> region_zones = region_map[whichRegion];
   
        for (auto z : region_zones) {
            if (area_code == z) continue;

            packBitsLE(&buffer[0x11], SEARCH_AREA, bitoffset, 5);
            bitoffset += 5;
            packBitsLE(&buffer[0x11], 0x00, bitoffset, 1);
            bitoffset += 1;
            packBitsLE(&buffer[0x11], 0x01, bitoffset, 1);
            bitoffset += 1;

            // encode search target area
            packBitsLE(&buffer[0x11], z, bitoffset, 10);
            bitoffset += 10;
        }
    }

    // end area list
    packBitsLE(&buffer[0x11], SEARCH_AREA, bitoffset, 5);
    bitoffset += 5;
    packBitsLE(&buffer[0x11], 0x00, bitoffset, 1);
    bitoffset += 1;
    packBitsLE(&buffer[0x11], 0x00, bitoffset, 1);
    bitoffset += 1;

    // linkshell
    packBitsLE(&buffer[0x11], SEARCH_LINKSHELL, bitoffset, 5);
    bitoffset += 5;
    packBitsLE(&buffer[0x11], LS_ID, bitoffset, 32);
    bitoffset += 32;

    int bytelength = (bitoffset / 8) + 1;


    packBitsLE(&buffer[0x10], bytelength, 0, 8);
}


plugininfo_t* g_PluginInformation = nullptr;

class DDD : IPlugin
{
    DDD(const DDD &cpy);


    DWORD m_PluginId;

private:
    int m_CurrentZoneID;
    std::string whichEvent;
    


public:
    DDD(void);
    ~DDD(void);

    IAshitaCore* m_AshitaCore;

    plugininfo_t GetPluginInfo(void) override;

    bool GetPlayerData();
    bool PostToDiscord(std::string eventName, std::string windowName, std::string Notes);
    bool PostToDiscordLong(std::string eventName, std::string windowName, std::string Notes);
    bool UpdateDiscord(std::string eventName);
    bool UpdateDiscordLong(std::string eventName);
    bool PostOrUpdateDiscord(std::string eventName);

    bool PostOrUpdateDiscordLong(std::string eventName);

    bool Initialize(IAshitaCore* core, ILogManager* log, uint32_t id) override;
    void Release(void) override;
    bool HandleCommand( const char * pszCommand, int nCommandType) override;
//    bool HandleIncomingText(int16_t mode, const char* message, int16_t modifiedMode, char* modifiedMessage, bool blocked) override;

    void GetConfig(); 
    bool Direct3DInitialize(IDirect3DDevice8* lpDevice) override;
    void Direct3DRender(void) override;
    bool HandleIncomingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked) override;
    bool HandleOutgoingPacket(uint16_t id, uint32_t size, void* data, void* modified, bool blocked) override;


    std::vector<std::string> playerNames;
    std::string botName;
    std::string eventName;
    std::string webhook;
    std::map<std::string, std::map<std::string, int>> playerMap;
    std::map<std::string, std::vector<std::string>> afkPlayerList;
    std::map<std::string, std::vector<std::string>> helperPlayersList;
    std::string messageId;
    std::vector<std::string> NotesList;
    std::vector<std::string> WindowList;
    std::map<std::string, int> windowPoints;
    std::map<std::string, int> playerForcedPoints;
    int REGION_SEARCH;
    int first_window_points;
    int helper_team_points;
};


uint16 printResultFromPacket(uint8* data, DDD* object, uint16 players_remaining) {
    uint32 m_offset = 192;
    uint32 size_offset = m_offset / 8;
    m_offset += 8;

    uint8 search_name_holder;
    uint8 char_name_holder;
    uint8 search_area_holder;
    uint16 search_zone_holder;

    uint16 num_results;
    memcpy(&num_results, data + 0x0E, sizeof(uint16));

    uint16 total_size;
    memcpy(&total_size, data + 0x08, sizeof(uint16));

    uint8 size_data = data[size_offset];

    char names[20];

    for (int i = 0; i <= 20; i++) {

        memset(names, 0x00, sizeof(char) * 20);
        size_data = data[size_offset];

        if ((m_offset / 8) >= total_size) break;

        search_name_holder = unpackBitsLE(data, m_offset, 5);
        m_offset += 5;
        char_name_holder = unpackBitsLE(data, m_offset, 4);
        m_offset += 4;

        for (uint8 c = 0; c < char_name_holder; ++c) {
            names[c] = unpackBitsLE(data, m_offset, 7);
            m_offset += 7;
        }

        search_area_holder = unpackBitsLE(data, m_offset, 5);
        m_offset += 5;

        search_zone_holder = unpackBitsLE(data, m_offset, 10);
        m_offset += 10;

        object->playerNames.push_back(std::string(names));
        players_remaining -= 1;

        my_total -= 1;

        if (players_remaining == 0 || players_remaining > 1024) break;

        size_offset += size_data + 1;
        m_offset = size_offset * 8;
        m_offset += 8;
    }
    return players_remaining;
}


#endif
