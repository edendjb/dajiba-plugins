#ifndef ZONE_H
#define ZONE_H

#include <map>
#include <vector>
#include "./common/cbasetypes.h"


std::map<int, std::vector<int>> region_map;

void initialize_region_map() {
	region_map[0] = std::vector{ 100, 101, 139, 140, 141, 142, 167, 190 };
	region_map[1] = std::vector{ 102, 103, 108, 193, 196, 248 };
	region_map[2] = std::vector{ 1, 2, 104, 105, 149, 150, 195 };
	region_map[3] = std::vector{ 106, 107, 143, 144, 172, 173, 191 };
	region_map[4] = std::vector{ 109, 110, 147, 148, 197 };
	region_map[5] = std::vector{ 115, 116, 145, 146, 169, 170, 192, 194 };
	region_map[6] = std::vector{ 3, 4, 117, 118, 198, 213, 249 };
	region_map[7] = std::vector{ 7, 8, 119, 120, 151, 152, 200 };
	region_map[8] = std::vector{ 9, 10, 111, 166, 203, 204, 206 };
	region_map[9] = std::vector{ 5, 6, 112, 161, 162, 165 };
	region_map[10] = std::vector{ 126, 127, 157, 158, 179, 184 };
	region_map[11] = std::vector{ 121, 122, 153, 154, 202, 251 };
	region_map[12] = std::vector{ 114, 125, 168, 208, 209, 247 };
	region_map[13] = std::vector{ 113, 128, 174, 201, 212 };
	region_map[14] = std::vector{ 123, 176, 250, 252 };
	region_map[15] = std::vector{ 124, 159, 160, 163, 205, 207, 211 };
	region_map[16] = std::vector{ 130, 177, 178, 180, 181 };
	region_map[17] = std::vector{ 11, 12, 13 };
	region_map[18] = std::vector{ 24, 25, 27, 28, 29, 30, 31, 32 };
	region_map[19] = std::vector{ 230, 231, 232, 233 };
	region_map[20] = std::vector{ 234, 235, 236, 237 };
	region_map[21] = std::vector{ 238, 239, 240, 241, 242 };
	region_map[22] = std::vector{ 243, 244, 245, 246 };
	region_map[23] = std::vector{ 39, 40, 41, 42, 134, 135, 185, 186, 187, 188 };
	region_map[24] = std::vector{ 26 };
	region_map[25] = std::vector{ 14, 16, 17, 18, 19, 20, 21, 22, 23 };
	region_map[26] = std::vector{ 33, 34, 35, 36 };
	region_map[27] = std::vector{ 37, 38 };
	region_map[28] = std::vector{ 48, 50, 52, 71 };
	region_map[29] = std::vector{ 51, 65, 66, 67, 68 };
	region_map[30] = std::vector{ 61, 62, 63, 64 };
	region_map[31] = std::vector{ 53, 54, 56, 57, 69, 78, 79 };
	region_map[32] = std::vector{ 72, 74, 76, 77 };
	region_map[33] = std::vector{ 80, 81 };
	region_map[34] = std::vector{ 82, 84, 85, 86, 175 };
	region_map[35] = std::vector{ 87, 88, 89, 93 };
	region_map[36] = std::vector{ 83, 90, 91, 92, 171 };
	region_map[37] = std::vector{ 94, 95, 96, 129 };
	region_map[38] = std::vector{ 97, 98, 99, 164 };
	region_map[39] = std::vector{ 136 };
	region_map[40] = std::vector{ 137, 138, 155, 156 };
	region_map[41] = std::vector{ 15, 45, 132, 215, 216, 217, 218, 253, 254, 255 };
	region_map[42] = std::vector{ 182 };
	region_map[43] = std::vector{ 43, 44, 183, 287 };
	region_map[44] = std::vector{ 256, 257, 258, 259 };
	region_map[45] = std::vector{ 260, 261, 262, 265, 268, 269, 270, 271 };

}


enum REGION_TYPE
{
    RONFAURE         = 0,
    ZULKHEIM         = 1,
    NORVALLEN        = 2,
    GUSTABERG        = 3,
    DERFLAND         = 4,
    SARUTABARUTA     = 5,
    KOLSHUSHU        = 6,
    ARAGONEU         = 7,
    FAUREGANDI       = 8,
    VALDEAUNIA       = 9,
    QUFIMISLAND      = 10,
    LITELOR          = 11,
    KUZOTZ           = 12,
    VOLLBOW          = 13,
    ELSHIMOLOWLANDS  = 14,
    ELSHIMOUPLANDS   = 15,
    TULIA            = 16,
    MOVALPOLOS       = 17,
    TAVNAZIA         = 18,
    SANDORIA         = 19,
    BASTOK           = 20,
    WINDURST         = 21,
    JEUNO            = 22,
    DYNAMIS          = 23,
    TAVNAZIAN_MARQ   = 24,
    PROMYVION        = 25,
    LUMORIA          = 26,
    LIMBUS           = 27,
    WEST_AHT_URHGAN  = 28,
    MAMOOL_JA_SAVAGE = 29,
    HALVUNG          = 30,
    ARRAPAGO         = 31,
    ALZADAAL         = 32,
    RONFAURE_FRONT   = 33,
    NORVALLEN_FRONT  = 34,
    GUSTABERG_FRONT  = 35,
    DERFLAND_FRONT   = 36,
    SARUTA_FRONT     = 37,
    ARAGONEAU_FRONT  = 38,
    FAUREGANDI_FRONT = 39,
    VALDEAUNIA_FRONT = 40,
    ABYSSEA          = 41,
    THE_THRESHOLD    = 42,
    ABDHALJS         = 43,
    ADOULIN_ISLANDS  = 44,
    EAST_ULBUKA      = 45,

    UNKNOWN = 255
};




enum ZONEID
{
    ZONE_RESIDENTIAL_AREA               = 0,
    ZONE_PHANAUET_CHANNEL               = 1,
    ZONE_CARPENTERS_LANDING             = 2,
    ZONE_MANACLIPPER                    = 3,
    ZONE_BIBIKI_BAY                     = 4,
    ZONE_ULEGUERAND_RANGE               = 5,
    ZONE_BEARCLAW_PINNACLE              = 6,
    ZONE_ATTOHWA_CHASM                  = 7,
    ZONE_BONEYARD_GULLY                 = 8,
    ZONE_PSOXJA                         = 9,
    ZONE_THE_SHROUDED_MAW               = 10,
    ZONE_OLDTON_MOVALPOLOS              = 11,
    ZONE_NEWTON_MOVALPOLOS              = 12,
    ZONE_MINE_SHAFT_2716                = 13,
    ZONE_HALL_OF_TRANSFERENCE           = 14,
    ZONE_ABYSSEA_KONSCHTAT              = 15,
    ZONE_PROMYVION_HOLLA                = 16,
    ZONE_SPIRE_OF_HOLLA                 = 17,
    ZONE_PROMYVION_DEM                  = 18,
    ZONE_SPIRE_OF_DEM                   = 19,
    ZONE_PROMYVION_MEA                  = 20,
    ZONE_SPIRE_OF_MEA                   = 21,
    ZONE_PROMYVION_VAHZL                = 22,
    ZONE_SPIRE_OF_VAHZL                 = 23,
    ZONE_LUFAISE_MEADOWS                = 24,
    ZONE_MISAREAUX_COAST                = 25,
    ZONE_TAVNAZIAN_SAFEHOLD             = 26,
    ZONE_PHOMIUNA_AQUEDUCTS             = 27,
    ZONE_SACRARIUM                      = 28,
    ZONE_RIVERNE_SITE_B01               = 29,
    ZONE_RIVERNE_SITE_A01               = 30,
    ZONE_MONARCH_LINN                   = 31,
    ZONE_SEALIONS_DEN                   = 32,
    ZONE_ALTAIEU                        = 33,
    ZONE_GRAND_PALACE_OF_HUXZOI         = 34,
    ZONE_THE_GARDEN_OF_RUHMET           = 35,
    ZONE_EMPYREAL_PARADOX               = 36,
    ZONE_TEMENOS                        = 37,
    ZONE_APOLLYON                       = 38,
    ZONE_DYNAMIS_VALKURM                = 39,
    ZONE_DYNAMIS_BUBURIMU               = 40,
    ZONE_DYNAMIS_QUFIM                  = 41,
    ZONE_DYNAMIS_TAVNAZIA               = 42,
    ZONE_DIORAMA_ABDHALJS_GHELSBA       = 43,
    ZONE_ABDHALJS_ISLE_PURGONORGO       = 44,
    ZONE_ABYSSEA_TAHRONGI               = 45,
    ZONE_OPEN_SEA_ROUTE_TO_AL_ZAHBI     = 46,
    ZONE_OPEN_SEA_ROUTE_TO_MHAURA       = 47,
    ZONE_AL_ZAHBI                       = 48,
    ZONE_49                             = 49, // GM Grid test zone
    ZONE_AHT_URHGAN_WHITEGATE           = 50,
    ZONE_WAJAOM_WOODLANDS               = 51,
    ZONE_BHAFLAU_THICKETS               = 52,
    ZONE_NASHMAU                        = 53,
    ZONE_ARRAPAGO_REEF                  = 54,
    ZONE_ILRUSI_ATOLL                   = 55,
    ZONE_PERIQIA                        = 56,
    ZONE_TALACCA_COVE                   = 57,
    ZONE_SILVER_SEA_ROUTE_TO_NASHMAU    = 58,
    ZONE_SILVER_SEA_ROUTE_TO_AL_ZAHBI   = 59,
    ZONE_THE_ASHU_TALIF                 = 60,
    ZONE_MOUNT_ZHAYOLM                  = 61,
    ZONE_HALVUNG                        = 62,
    ZONE_LEBROS_CAVERN                  = 63,
    ZONE_NAVUKGO_EXECUTION_CHAMBER      = 64,
    ZONE_MAMOOK                         = 65,
    ZONE_MAMOOL_JA_TRAINING_GROUNDS     = 66,
    ZONE_JADE_SEPULCHER                 = 67,
    ZONE_AYDEEWA_SUBTERRANE             = 68,
    ZONE_LEUJAOAM_SANCTUM               = 69,
    ZONE_CHOCOBO_CIRCUIT                = 70,
    ZONE_THE_COLOSSEUM                  = 71,
    ZONE_ALZADAAL_UNDERSEA_RUINS        = 72,
    ZONE_ZHAYOLM_REMNANTS               = 73,
    ZONE_ARRAPAGO_REMNANTS              = 74,
    ZONE_BHAFLAU_REMNANTS               = 75,
    ZONE_SILVER_SEA_REMNANTS            = 76,
    ZONE_NYZUL_ISLE                     = 77,
    ZONE_HAZHALM_TESTING_GROUNDS        = 78,
    ZONE_CAEDARVA_MIRE                  = 79,
    ZONE_SOUTHERN_SAN_DORIA_S           = 80,
    ZONE_EAST_RONFAURE_S                = 81,
    ZONE_JUGNER_FOREST_S                = 82,
    ZONE_VUNKERL_INLET_S                = 83,
    ZONE_BATALLIA_DOWNS_S               = 84,
    ZONE_LA_VAULE_S                     = 85,
    ZONE_EVERBLOOM_HOLLOW               = 86,
    ZONE_BASTOK_MARKETS_S               = 87,
    ZONE_NORTH_GUSTABERG_S              = 88,
    ZONE_GRAUBERG_S                     = 89,
    ZONE_PASHHOW_MARSHLANDS_S           = 90,
    ZONE_ROLANBERRY_FIELDS_S            = 91,
    ZONE_BEADEAUX_S                     = 92,
    ZONE_RUHOTZ_SILVERMINES             = 93,
    ZONE_WINDURST_WATERS_S              = 94,
    ZONE_WEST_SARUTABARUTA_S            = 95,
    ZONE_FORT_KARUGO_NARUGO_S           = 96,
    ZONE_MERIPHATAUD_MOUNTAINS_S        = 97,
    ZONE_SAUROMUGUE_CHAMPAIGN_S         = 98,
    ZONE_CASTLE_OZTROJA_S               = 99,
    ZONE_WEST_RONFAURE                  = 100,
    ZONE_EAST_RONFAURE                  = 101,
    ZONE_LA_THEINE_PLATEAU              = 102,
    ZONE_VALKURM_DUNES                  = 103,
    ZONE_JUGNER_FOREST                  = 104,
    ZONE_BATALLIA_DOWNS                 = 105,
    ZONE_NORTH_GUSTABERG                = 106,
    ZONE_SOUTH_GUSTABERG                = 107,
    ZONE_KONSCHTAT_HIGHLANDS            = 108,
    ZONE_PASHHOW_MARSHLANDS             = 109,
    ZONE_ROLANBERRY_FIELDS              = 110,
    ZONE_BEAUCEDINE_GLACIER             = 111,
    ZONE_XARCABARD                      = 112,
    ZONE_CAPE_TERIGGAN                  = 113,
    ZONE_EASTERN_ALTEPA_DESERT          = 114,
    ZONE_WEST_SARUTABARUTA              = 115,
    ZONE_EAST_SARUTABARUTA              = 116,
    ZONE_TAHRONGI_CANYON                = 117,
    ZONE_BUBURIMU_PENINSULA             = 118,
    ZONE_MERIPHATAUD_MOUNTAINS          = 119,
    ZONE_SAUROMUGUE_CHAMPAIGN           = 120,
    ZONE_THE_SANCTUARY_OF_ZITAH         = 121,
    ZONE_ROMAEVE                        = 122,
    ZONE_YUHTUNGA_JUNGLE                = 123,
    ZONE_YHOATOR_JUNGLE                 = 124,
    ZONE_WESTERN_ALTEPA_DESERT          = 125,
    ZONE_QUFIM_ISLAND                   = 126,
    ZONE_BEHEMOTHS_DOMINION             = 127,
    ZONE_VALLEY_OF_SORROWS              = 128,
    ZONE_GHOYUS_REVERIE                 = 129,
    ZONE_RUAUN_GARDENS                  = 130,
    ZONE_MORDION_GAOL                   = 131,
    ZONE_ABYSSEA_LA_THEINE              = 132,
    ZONE_133                            = 133, // Seems to be the zone background of char select.
    ZONE_DYNAMIS_BEAUCEDINE             = 134,
    ZONE_DYNAMIS_XARCABARD              = 135,
    ZONE_BEAUCEDINE_GLACIER_S           = 136,
    ZONE_XARCABARD_S                    = 137,
    ZONE_CASTLE_ZVAHL_BAILEYS_S         = 138,
    ZONE_HORLAIS_PEAK                   = 139,
    ZONE_GHELSBA_OUTPOST                = 140,
    ZONE_FORT_GHELSBA                   = 141,
    ZONE_YUGHOTT_GROTTO                 = 142,
    ZONE_PALBOROUGH_MINES               = 143,
    ZONE_WAUGHROON_SHRINE               = 144,
    ZONE_GIDDEUS                        = 145,
    ZONE_BALGAS_DAIS                    = 146,
    ZONE_BEADEAUX                       = 147,
    ZONE_QULUN_DOME                     = 148,
    ZONE_DAVOI                          = 149,
    ZONE_MONASTIC_CAVERN                = 150,
    ZONE_CASTLE_OZTROJA                 = 151,
    ZONE_ALTAR_ROOM                     = 152,
    ZONE_THE_BOYAHDA_TREE               = 153,
    ZONE_DRAGONS_AERY                   = 154,
    ZONE_CASTLE_ZVAHL_KEEP_S            = 155,
    ZONE_THRONE_ROOM_S                  = 156,
    ZONE_MIDDLE_DELKFUTTS_TOWER         = 157,
    ZONE_UPPER_DELKFUTTS_TOWER          = 158,
    ZONE_TEMPLE_OF_UGGALEPIH            = 159,
    ZONE_DEN_OF_RANCOR                  = 160,
    ZONE_CASTLE_ZVAHL_BAILEYS           = 161,
    ZONE_CASTLE_ZVAHL_KEEP              = 162,
    ZONE_SACRIFICIAL_CHAMBER            = 163,
    ZONE_GARLAIGE_CITADEL_S             = 164,
    ZONE_THRONE_ROOM                    = 165,
    ZONE_RANGUEMONT_PASS                = 166,
    ZONE_BOSTAUNIEUX_OUBLIETTE          = 167,
    ZONE_CHAMBER_OF_ORACLES             = 168,
    ZONE_TORAIMARAI_CANAL               = 169,
    ZONE_FULL_MOON_FOUNTAIN             = 170,
    ZONE_CRAWLERS_NEST_S                = 171,
    ZONE_ZERUHN_MINES                   = 172,
    ZONE_KORROLOKA_TUNNEL               = 173,
    ZONE_KUFTAL_TUNNEL                  = 174,
    ZONE_THE_ELDIEME_NECROPOLIS_S       = 175,
    ZONE_SEA_SERPENT_GROTTO             = 176,
    ZONE_VELUGANNON_PALACE              = 177,
    ZONE_THE_SHRINE_OF_RUAVITAU         = 178,
    ZONE_STELLAR_FULCRUM                = 179,
    ZONE_LALOFF_AMPHITHEATER            = 180,
    ZONE_THE_CELESTIAL_NEXUS            = 181,
    ZONE_WALK_OF_ECHOES                 = 182,
    ZONE_MAQUETTE_ABDHALJS_LEGION_A     = 183,
    ZONE_LOWER_DELKFUTTS_TOWER          = 184,
    ZONE_DYNAMIS_SAN_DORIA              = 185,
    ZONE_DYNAMIS_BASTOK                 = 186,
    ZONE_DYNAMIS_WINDURST               = 187,
    ZONE_DYNAMIS_JEUNO                  = 188,
    ZONE_189                            = 189, // Southern San d'Oria [S] Residential Area
    ZONE_KING_RANPERRES_TOMB            = 190,
    ZONE_DANGRUF_WADI                   = 191,
    ZONE_INNER_HORUTOTO_RUINS           = 192,
    ZONE_ORDELLES_CAVES                 = 193,
    ZONE_OUTER_HORUTOTO_RUINS           = 194,
    ZONE_THE_ELDIEME_NECROPOLIS         = 195,
    ZONE_GUSGEN_MINES                   = 196,
    ZONE_CRAWLERS_NEST                  = 197,
    ZONE_MAZE_OF_SHAKHRAMI              = 198,
    ZONE_199                            = 199, // Bastok Markets [S] Residential Area
    ZONE_GARLAIGE_CITADEL               = 200,
    ZONE_CLOISTER_OF_GALES              = 201,
    ZONE_CLOISTER_OF_STORMS             = 202,
    ZONE_CLOISTER_OF_FROST              = 203,
    ZONE_FEIYIN                         = 204,
    ZONE_IFRITS_CAULDRON                = 205,
    ZONE_QUBIA_ARENA                    = 206,
    ZONE_CLOISTER_OF_FLAMES             = 207,
    ZONE_QUICKSAND_CAVES                = 208,
    ZONE_CLOISTER_OF_TREMORS            = 209,
    ZONE_GM_HOME                        = 210, // Name comes from leaked official documentation
    ZONE_CLOISTER_OF_TIDES              = 211,
    ZONE_GUSTAV_TUNNEL                  = 212,
    ZONE_LABYRINTH_OF_ONZOZO            = 213,
    ZONE_214                            = 214, // Aht Urhgan Residential Area
    ZONE_ABYSSEA_ATTOHWA                = 215,
    ZONE_ABYSSEA_MISAREAUX              = 216,
    ZONE_ABYSSEA_VUNKERL                = 217,
    ZONE_ABYSSEA_ALTEPA                 = 218,
    ZONE_219                            = 219, // Windurst Waters [S] Residential Area
    ZONE_SHIP_BOUND_FOR_SELBINA         = 220,
    ZONE_SHIP_BOUND_FOR_MHAURA          = 221,
    ZONE_PROVENANCE                     = 222,
    ZONE_SAN_DORIA_JEUNO_AIRSHIP        = 223,
    ZONE_BASTOK_JEUNO_AIRSHIP           = 224,
    ZONE_WINDURST_JEUNO_AIRSHIP         = 225,
    ZONE_KAZHAM_JEUNO_AIRSHIP           = 226,
    ZONE_SHIP_BOUND_FOR_SELBINA_PIRATES = 227,
    ZONE_SHIP_BOUND_FOR_MHAURA_PIRATES  = 228,
    ZONE_229                            = 229,
    ZONE_SOUTHERN_SANDORIA              = 230,
    ZONE_NORTHERN_SANDORIA              = 231,
    ZONE_PORT_SANDORIA                  = 232,
    ZONE_CHATEAU_DORAGUILLE             = 233,
    ZONE_BASTOK_MINES                   = 234,
    ZONE_BASTOK_MARKETS                 = 235,
    ZONE_PORT_BASTOK                    = 236,
    ZONE_METALWORKS                     = 237,
    ZONE_WINDURST_WATERS                = 238,
    ZONE_WINDURST_WALLS                 = 239,
    ZONE_PORT_WINDURST                  = 240,
    ZONE_WINDURST_WOODS                 = 241,
    ZONE_HEAVENS_TOWER                  = 242,
    ZONE_RULUDE_GARDENS                 = 243,
    ZONE_UPPER_JEUNO                    = 244,
    ZONE_LOWER_JEUNO                    = 245,
    ZONE_PORT_JEUNO                     = 246,
    ZONE_RABAO                          = 247,
    ZONE_SELBINA                        = 248,
    ZONE_MHAURA                         = 249,
    ZONE_KAZHAM                         = 250,
    ZONE_HALL_OF_THE_GODS               = 251,
    ZONE_NORG                           = 252,
    ZONE_ABYSSEA_ULEGUERAND             = 253,
    ZONE_ABYSSEA_GRAUBERG               = 254,
    ZONE_ABYSSEA_EMPYREAL_PARADOX       = 255,
    ZONE_WESTERN_ADOULIN                = 256,
    ZONE_EASTERN_ADOULIN                = 257,
    ZONE_RALA_WATERWAYS                 = 258,
    ZONE_RALA_WATERWAYS_U               = 259,
    ZONE_YAHSE_HUNTING_GROUNDS          = 260,
    ZONE_CEIZAK_BATTLEGROUNDS           = 261,
    ZONE_FORET_DE_HENNETIEL             = 262,
    ZONE_YORCIA_WEALD                   = 263,
    ZONE_YORCIA_WEALD_U                 = 264,
    ZONE_MORIMAR_BASALT_FIELDS          = 265,
    ZONE_MARJAMI_RAVINE                 = 266,
    ZONE_KAMIHR_DRIFTS                  = 267,
    ZONE_SIH_GATES                      = 268,
    ZONE_MOH_GATES                      = 269,
    ZONE_CIRDAS_CAVERNS                 = 270,
    ZONE_CIRDAS_CAVERNS_U               = 271,
    ZONE_DHO_GATES                      = 272,
    ZONE_WOH_GATES                      = 273,
    ZONE_OUTER_RAKAZNAR                 = 274,
    ZONE_OUTER_RAKAZNAR_U               = 275,
    ZONE_RAKAZNAR_INNER_COURT           = 276,
    ZONE_RAKAZNAR_TURRIS                = 277,
    ZONE_278                            = 278,
    ZONE_WALK_OF_ECHOES_P2              = 279,
    ZONE_MOG_GARDEN                     = 280,
    ZONE_LEAFALLIA                      = 281,
    ZONE_MOUNT_KAMIHR                   = 282,
    ZONE_SILVER_KNIFE                   = 283,
    ZONE_CELENNIA_MEMORIAL_LIBRARY      = 284,
    ZONE_FERETORY                       = 285,
    ZONE_286                            = 286,
    ZONE_MAQUETTE_ABDHALJS_LEGION_B     = 287,
    ZONE_ESCHA_ZITAH                    = 288,
    ZONE_ESCHA_RUAUN                    = 289,
    ZONE_DESUETIA_EMPYREAL_PARADOX      = 290,
    ZONE_REISENJIMA                     = 291,
    ZONE_REISENJIMA_HENGE               = 292,
    ZONE_REISENJIMA_SANCTORIUM          = 293,
    ZONE_DYNAMIS_SAN_DORIA_D            = 294,
    ZONE_DYNAMIS_BASTOK_D               = 295,
    ZONE_DYNAMIS_WINDURST_D             = 296,
    ZONE_DYNAMIS_JEUNO_D                = 297,
    ZONE_WALK_OF_ECHOES_P1              = 298
};







REGION_TYPE GetCurrentRegion(uint16 ZoneID)
{
    switch (ZoneID)
    {
        case ZONE_BOSTAUNIEUX_OUBLIETTE:
        case ZONE_EAST_RONFAURE:
        case ZONE_FORT_GHELSBA:
        case ZONE_GHELSBA_OUTPOST:
        case ZONE_HORLAIS_PEAK:
        case ZONE_KING_RANPERRES_TOMB:
        case ZONE_WEST_RONFAURE:
        case ZONE_YUGHOTT_GROTTO:
            return REGION_TYPE::RONFAURE;
        case ZONE_GUSGEN_MINES:
        case ZONE_KONSCHTAT_HIGHLANDS:
        case ZONE_LA_THEINE_PLATEAU:
        case ZONE_ORDELLES_CAVES:
        case ZONE_SELBINA:
        case ZONE_VALKURM_DUNES:
            return REGION_TYPE::ZULKHEIM;
        case ZONE_BATALLIA_DOWNS:
        case ZONE_CARPENTERS_LANDING:
        case ZONE_DAVOI:
        case ZONE_THE_ELDIEME_NECROPOLIS:
        case ZONE_JUGNER_FOREST:
        case ZONE_MONASTIC_CAVERN:
        case ZONE_PHANAUET_CHANNEL:
            return REGION_TYPE::NORVALLEN;
        case ZONE_DANGRUF_WADI:
        case ZONE_KORROLOKA_TUNNEL:
        case ZONE_NORTH_GUSTABERG:
        case ZONE_PALBOROUGH_MINES:
        case ZONE_SOUTH_GUSTABERG:
        case ZONE_WAUGHROON_SHRINE:
        case ZONE_ZERUHN_MINES:
            return REGION_TYPE::GUSTABERG;
        case ZONE_BEADEAUX:
        case ZONE_CRAWLERS_NEST:
        case ZONE_PASHHOW_MARSHLANDS:
        case ZONE_QULUN_DOME:
        case ZONE_ROLANBERRY_FIELDS:
            return REGION_TYPE::DERFLAND;
        case ZONE_BALGAS_DAIS:
        case ZONE_EAST_SARUTABARUTA:
        case ZONE_FULL_MOON_FOUNTAIN:
        case ZONE_GIDDEUS:
        case ZONE_INNER_HORUTOTO_RUINS:
        case ZONE_OUTER_HORUTOTO_RUINS:
        case ZONE_TORAIMARAI_CANAL:
        case ZONE_WEST_SARUTABARUTA:
            return REGION_TYPE::SARUTABARUTA;
        case ZONE_BIBIKI_BAY:
        case ZONE_BUBURIMU_PENINSULA:
        case ZONE_LABYRINTH_OF_ONZOZO:
        case ZONE_MANACLIPPER:
        case ZONE_MAZE_OF_SHAKHRAMI:
        case ZONE_MHAURA:
        case ZONE_TAHRONGI_CANYON:
            return REGION_TYPE::KOLSHUSHU;
        case ZONE_ALTAR_ROOM:
        case ZONE_ATTOHWA_CHASM:
        case ZONE_BONEYARD_GULLY:
        case ZONE_CASTLE_OZTROJA:
        case ZONE_GARLAIGE_CITADEL:
        case ZONE_MERIPHATAUD_MOUNTAINS:
        case ZONE_SAUROMUGUE_CHAMPAIGN:
            return REGION_TYPE::ARAGONEU;
        case ZONE_BEAUCEDINE_GLACIER:
        case ZONE_CLOISTER_OF_FROST:
        case ZONE_FEIYIN:
        case ZONE_PSOXJA:
        case ZONE_QUBIA_ARENA:
        case ZONE_RANGUEMONT_PASS:
        case ZONE_THE_SHROUDED_MAW:
            return REGION_TYPE::FAUREGANDI;
        case ZONE_BEARCLAW_PINNACLE:
        case ZONE_CASTLE_ZVAHL_BAILEYS:
        case ZONE_CASTLE_ZVAHL_KEEP:
        case ZONE_THRONE_ROOM:
        case ZONE_ULEGUERAND_RANGE:
        case ZONE_XARCABARD:
            return REGION_TYPE::VALDEAUNIA;
        case ZONE_BEHEMOTHS_DOMINION:
        case ZONE_LOWER_DELKFUTTS_TOWER:
        case ZONE_MIDDLE_DELKFUTTS_TOWER:
        case ZONE_QUFIM_ISLAND:
        case ZONE_STELLAR_FULCRUM:
        case ZONE_UPPER_DELKFUTTS_TOWER:
            return REGION_TYPE::QUFIMISLAND;
        case ZONE_THE_BOYAHDA_TREE:
        case ZONE_CLOISTER_OF_STORMS:
        case ZONE_DRAGONS_AERY:
        case ZONE_HALL_OF_THE_GODS:
        case ZONE_ROMAEVE:
        case ZONE_THE_SANCTUARY_OF_ZITAH:
            return REGION_TYPE::LITELOR;
        case ZONE_CLOISTER_OF_TREMORS:
        case ZONE_EASTERN_ALTEPA_DESERT:
        case ZONE_CHAMBER_OF_ORACLES:
        case ZONE_QUICKSAND_CAVES:
        case ZONE_RABAO:
        case ZONE_WESTERN_ALTEPA_DESERT:
            return REGION_TYPE::KUZOTZ;
        case ZONE_CAPE_TERIGGAN:
        case ZONE_CLOISTER_OF_GALES:
        case ZONE_GUSTAV_TUNNEL:
        case ZONE_KUFTAL_TUNNEL:
        case ZONE_VALLEY_OF_SORROWS:
            return REGION_TYPE::VOLLBOW;
        case ZONE_KAZHAM:
        case ZONE_NORG:
        case ZONE_SEA_SERPENT_GROTTO:
        case ZONE_YUHTUNGA_JUNGLE:
            return REGION_TYPE::ELSHIMOLOWLANDS;
        case ZONE_CLOISTER_OF_FLAMES:
        case ZONE_CLOISTER_OF_TIDES:
        case ZONE_DEN_OF_RANCOR:
        case ZONE_IFRITS_CAULDRON:
        case ZONE_SACRIFICIAL_CHAMBER:
        case ZONE_TEMPLE_OF_UGGALEPIH:
        case ZONE_YHOATOR_JUNGLE:
            return REGION_TYPE::ELSHIMOUPLANDS;
        case ZONE_THE_CELESTIAL_NEXUS:
        case ZONE_LALOFF_AMPHITHEATER:
        case ZONE_RUAUN_GARDENS:
        case ZONE_THE_SHRINE_OF_RUAVITAU:
        case ZONE_VELUGANNON_PALACE:
            return REGION_TYPE::TULIA;
        case ZONE_MINE_SHAFT_2716:
        case ZONE_NEWTON_MOVALPOLOS:
        case ZONE_OLDTON_MOVALPOLOS:
            return REGION_TYPE::MOVALPOLOS;
        case ZONE_LUFAISE_MEADOWS:
        case ZONE_MISAREAUX_COAST:
        case ZONE_MONARCH_LINN:
        case ZONE_PHOMIUNA_AQUEDUCTS:
        case ZONE_RIVERNE_SITE_A01:
        case ZONE_RIVERNE_SITE_B01:
        case ZONE_SACRARIUM:
        case ZONE_SEALIONS_DEN:
            return REGION_TYPE::TAVNAZIA;
        case ZONE_TAVNAZIAN_SAFEHOLD:
            return REGION_TYPE::TAVNAZIAN_MARQ;
        case ZONE_SOUTHERN_SANDORIA:
        case ZONE_NORTHERN_SANDORIA:
        case ZONE_PORT_SANDORIA:
        case ZONE_CHATEAU_DORAGUILLE:
            return REGION_TYPE::SANDORIA;
        case ZONE_BASTOK_MINES:
        case ZONE_BASTOK_MARKETS:
        case ZONE_PORT_BASTOK:
        case ZONE_METALWORKS:
            return REGION_TYPE::BASTOK;
        case ZONE_WINDURST_WATERS:
        case ZONE_WINDURST_WALLS:
        case ZONE_PORT_WINDURST:
        case ZONE_WINDURST_WOODS:
        case ZONE_HEAVENS_TOWER:
            return REGION_TYPE::WINDURST;
        case ZONE_RULUDE_GARDENS:
        case ZONE_UPPER_JEUNO:
        case ZONE_LOWER_JEUNO:
        case ZONE_PORT_JEUNO:
            return REGION_TYPE::JEUNO;
        case ZONE_DYNAMIS_BASTOK:
        case ZONE_DYNAMIS_BEAUCEDINE:
        case ZONE_DYNAMIS_BUBURIMU:
        case ZONE_DYNAMIS_JEUNO:
        case ZONE_DYNAMIS_QUFIM:
        case ZONE_DYNAMIS_SAN_DORIA:
        case ZONE_DYNAMIS_TAVNAZIA:
        case ZONE_DYNAMIS_VALKURM:
        case ZONE_DYNAMIS_WINDURST:
        case ZONE_DYNAMIS_XARCABARD:
            return REGION_TYPE::DYNAMIS;
        case ZONE_PROMYVION_DEM:
        case ZONE_PROMYVION_HOLLA:
        case ZONE_PROMYVION_MEA:
        case ZONE_PROMYVION_VAHZL:
        case ZONE_SPIRE_OF_DEM:
        case ZONE_SPIRE_OF_HOLLA:
        case ZONE_SPIRE_OF_MEA:
        case ZONE_SPIRE_OF_VAHZL:
        case ZONE_HALL_OF_TRANSFERENCE:
            return REGION_TYPE::PROMYVION;
        case ZONE_ALTAIEU:
        case ZONE_EMPYREAL_PARADOX:
        case ZONE_THE_GARDEN_OF_RUHMET:
        case ZONE_GRAND_PALACE_OF_HUXZOI:
            return REGION_TYPE::LUMORIA;
        case ZONE_APOLLYON:
        case ZONE_TEMENOS:
            return REGION_TYPE::LIMBUS;
        case ZONE_AL_ZAHBI:
        case ZONE_AHT_URHGAN_WHITEGATE:
        case ZONE_BHAFLAU_THICKETS:
        case ZONE_THE_COLOSSEUM:
            return REGION_TYPE::WEST_AHT_URHGAN;
        case ZONE_MAMOOL_JA_TRAINING_GROUNDS:
        case ZONE_MAMOOK:
        case ZONE_WAJAOM_WOODLANDS:
        case ZONE_AYDEEWA_SUBTERRANE:
        case ZONE_JADE_SEPULCHER:
            return REGION_TYPE::MAMOOL_JA_SAVAGE;
        case ZONE_HALVUNG:
        case ZONE_MOUNT_ZHAYOLM:
        case ZONE_LEBROS_CAVERN:
        case ZONE_NAVUKGO_EXECUTION_CHAMBER:
            return REGION_TYPE::HALVUNG;
        case ZONE_ARRAPAGO_REEF:
        case ZONE_CAEDARVA_MIRE:
        case ZONE_LEUJAOAM_SANCTUM:
        case ZONE_NASHMAU:
        case ZONE_HAZHALM_TESTING_GROUNDS:
        case ZONE_TALACCA_COVE:
        case ZONE_PERIQIA:
            return REGION_TYPE::ARRAPAGO;
        case ZONE_NYZUL_ISLE:
        case ZONE_ARRAPAGO_REMNANTS:
        case ZONE_ALZADAAL_UNDERSEA_RUINS:
        case ZONE_SILVER_SEA_REMNANTS:
            return REGION_TYPE::ALZADAAL;
        case ZONE_SOUTHERN_SAN_DORIA_S:
        case ZONE_EAST_RONFAURE_S:
            return REGION_TYPE::RONFAURE_FRONT;
        case ZONE_BASTOK_MARKETS_S:
        case ZONE_NORTH_GUSTABERG_S:
        case ZONE_RUHOTZ_SILVERMINES:
        case ZONE_GRAUBERG_S:
            return REGION_TYPE::GUSTABERG_FRONT;
        case ZONE_WINDURST_WATERS_S:
        case ZONE_WEST_SARUTABARUTA_S:
        case ZONE_GHOYUS_REVERIE:
        case ZONE_FORT_KARUGO_NARUGO_S:
            return REGION_TYPE::SARUTA_FRONT;
        case ZONE_BATALLIA_DOWNS_S:
        case ZONE_JUGNER_FOREST_S:
        case ZONE_LA_VAULE_S:
        case ZONE_EVERBLOOM_HOLLOW:
        case ZONE_THE_ELDIEME_NECROPOLIS_S:
            return REGION_TYPE::NORVALLEN_FRONT;
        case ZONE_ROLANBERRY_FIELDS_S:
        case ZONE_PASHHOW_MARSHLANDS_S:
        case ZONE_CRAWLERS_NEST_S:
        case ZONE_BEADEAUX_S:
        case ZONE_VUNKERL_INLET_S:
            return REGION_TYPE::DERFLAND_FRONT;
        case ZONE_SAUROMUGUE_CHAMPAIGN_S:
        case ZONE_MERIPHATAUD_MOUNTAINS_S:
        case ZONE_CASTLE_OZTROJA_S:
        case ZONE_GARLAIGE_CITADEL_S:
            return REGION_TYPE::ARAGONEAU_FRONT;
        case ZONE_BEAUCEDINE_GLACIER_S:
            return REGION_TYPE::FAUREGANDI_FRONT;
        case ZONE_XARCABARD_S:
        case ZONE_CASTLE_ZVAHL_BAILEYS_S:
        case ZONE_CASTLE_ZVAHL_KEEP_S:
        case ZONE_THRONE_ROOM_S:
            return REGION_TYPE::VALDEAUNIA_FRONT;
        case ZONE_ABYSSEA_ALTEPA:
        case ZONE_ABYSSEA_ATTOHWA:
        case ZONE_ABYSSEA_EMPYREAL_PARADOX:
        case ZONE_ABYSSEA_GRAUBERG:
        case ZONE_ABYSSEA_KONSCHTAT:
        case ZONE_ABYSSEA_LA_THEINE:
        case ZONE_ABYSSEA_MISAREAUX:
        case ZONE_ABYSSEA_TAHRONGI:
        case ZONE_ABYSSEA_ULEGUERAND:
        case ZONE_ABYSSEA_VUNKERL:
            return REGION_TYPE::ABYSSEA;
        case ZONE_WALK_OF_ECHOES:
            return REGION_TYPE::THE_THRESHOLD;
        case ZONE_DIORAMA_ABDHALJS_GHELSBA:
        case ZONE_ABDHALJS_ISLE_PURGONORGO:
        case ZONE_MAQUETTE_ABDHALJS_LEGION_A:
        case ZONE_MAQUETTE_ABDHALJS_LEGION_B:
            return REGION_TYPE::ABDHALJS;
        case ZONE_WESTERN_ADOULIN:
        case ZONE_EASTERN_ADOULIN:
        case ZONE_RALA_WATERWAYS:
        case ZONE_RALA_WATERWAYS_U:
            return REGION_TYPE::ADOULIN_ISLANDS;
        case ZONE_CEIZAK_BATTLEGROUNDS:
        case ZONE_FORET_DE_HENNETIEL:
        case ZONE_SIH_GATES:
        case ZONE_MOH_GATES:
        case ZONE_CIRDAS_CAVERNS:
        case ZONE_CIRDAS_CAVERNS_U:
        case ZONE_YAHSE_HUNTING_GROUNDS:
        case ZONE_MORIMAR_BASALT_FIELDS:
            return REGION_TYPE::EAST_ULBUKA;
    }
    return REGION_TYPE::UNKNOWN;
}







#endif
