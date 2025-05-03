#ifndef __ZZCHARACTER_H__
#define __ZZCHARACTER_H__

#include "ZzzBMD.h"
#include <unordered_map>

extern Script_Skill MonsterSkill[];
extern CHARACTER* CharactersClient;
extern CHARACTER CharacterView;
extern CHARACTER* Hero;

DWORD GetGuildRelationShipTextColor(BYTE GuildRelationShip);
DWORD GetGuildRelationShipBGColor(BYTE GuildRelationShip);

extern std::unordered_map<int, int> g_CharacterKeyToIndexMap;
extern std::vector<int> g_FreeCharacterIndices;

struct MonsterConfigData {
    // Core Identification & Appearance
    int         modelType = MONSTER_MODEL_UNDEFINED;         // The MODEL_XXX ID for rendering
    std::wstring name = L"";            // Default monster name
    float       scale = 1.0f;           // Default scale

    // Gameplay Classification
    int         kind = KIND_MONSTER;    // Use KIND_MONSTER, KIND_NPC, etc. defines

    // Equipment (Use MODEL_XXX IDs or -1 for none)
    // Weapons
    short       weapon1Type = -1;
    short       weapon1Level = 0;
    BYTE        weapon1ExcellentFlags = 0; // Store flags if needed for effects/stats
    BYTE        weapon1AncientDiscriminator = 0;
    short       weapon2Type = -1;
    short       weapon2Level = 0;
    BYTE        weapon2ExcellentFlags = 0;
    BYTE        weapon2AncientDiscriminator = 0;
    // Armor (for player-model NPCs like Marlon, Guards, Cursed Wizard)
    short       helmType = -1;
    short       helmLevel = 0;
    short       armorType = -1;
    short       armorLevel = 0;
    short       pantsType = -1;
    short       pantsLevel = 0;
    short       glovesType = -1;
    short       glovesLevel = 0;
    short       bootsType = -1;
    short       bootsLevel = 0;
    short       wingsType = -1;
    short       wingsLevel = 0;
    // Note: Helper/Pet type might be too complex for static config if it involves creating separate objects.

    // Rendering & Visual Flags/Settings
    int         hiddenMesh = -1;        // Default: show all meshes.
    int         blendMesh = -1;         // Default: no blend mesh.
    float       blendMeshLight = 1.0f;  // Light intensity for blend mesh effect
    bool        enableShadow = true;    // Whether the object casts a shadow
    bool        m_bRenderShadow = true; // Separate flag from ZzzObject? Keep synced.
    float       alphaTarget = 1.0f;     // Initial alpha target (e.g., 0.4f for GHOST)
    float       m_fEdgeScale = 1.0f;    // Edge scale for outlines

    // Behavior & State Flags
    bool        notRotateOnMagicHit = false; // For static NPCs/objects
    int         initialLevel = 0;       // If monster level needs to be set
    int         initialSubType = 0;     // If SubType needs specific init (Use -1 if needs rand())
    bool        startsBloody = false;   // If it starts with the 'blood' effect
    bool        m_bpcroom = false;      // PC Room flag for helpers?

    // Special Effect Flags (used to trigger logic in ApplyMonsterSpecificLogic or RenderCharacter)
    bool        needsGoldenEffect = false; // Flag for golden monster rendering logic
    bool        isBloodCastleGateOrStatue = false; // Flag for specific BC gate/statue logic

    // Optional: Store bone names and IDs for registration
    std::vector<std::pair<const wchar_t*, int>> bonesToRegister;

    // Default constructor to ensure sane defaults
    MonsterConfigData() = default;
};

void ApplyMonsterSpecificLogic(CHARACTER* c, EMonsterType Type, const MonsterConfigData& config);
void InitializeMonsterData();

const int MAX_MONSTER_TYPE_INDEX = MONSTER_END + 1;
extern MonsterConfigData g_MonsterConfig[MAX_MONSTER_TYPE_INDEX];


CHARACTER* FindCharacterByID(wchar_t* szName);
CHARACTER* FindCharacterByKey(int Key);

void CreateMonsterServer(int Type, vec3_t Position, vec3_t Angle, int Level = 0);
void CreateWeapon(int Type, int Level, CHARACTER* c, int Hand);
void RenderLinkObject(float x, float y, float z, CHARACTER* c, PART_t* f, int Type, int Level, int Option1, bool Link, bool Translate, int RenderType = 0, bool bRightHandItem = true);
void RenderCharacter(CHARACTER* c, OBJECT* o, int Select = 0);
void RenderCharactersClient();
void RenderCharactersServer();
void RenderCharactersShadow();
void MoveCharacterClient(CHARACTER* cc);
void MoveCharactersClient();
void MoveCharactersServer();

void MoveEye(OBJECT* o, BMD* b, int Right, int Left, int Right2 = -1, int Left2 = -1, int Right3 = -1, int Left3 = -1);
void DeleteCloth(CHARACTER* c, OBJECT* o = NULL, PART_t* p2 = NULL);

bool CheckFullSet(CHARACTER* c);

void AttackRange(vec3_t Position, float Range, int Damage);

void MoveCharacterPosition(CHARACTER* c);
void ClientSendMonsterHit(CHARACTER* cc, int AttackPoint);
void ClientSendPositionPacket(CHARACTER* c);
void ClientSendPathPacket(unsigned short ID, unsigned char* PathX, unsigned char* PathY, unsigned char PathNum);
void ChangeCharacterExt(int Key, BYTE* Equipment, CHARACTER* pCharacter = NULL, OBJECT* pHelper = NULL);
void ReadEquipmentExtended(int Key, BYTE flags, BYTE* Equipment, CHARACTER* pCharacter = nullptr, OBJECT* pHelper = nullptr);
void ClearCharacters(int Key = -1);
void DeleteCharacter(int Key);
void DeleteCharacter(CHARACTER* c, OBJECT* o);
int FindCharacterIndex(int Key);
int FindCharacterIndexByMonsterIndex(int Type);

void DeadCharacterBuff(OBJECT* o);

int  HangerBloodCastleQuestItem(int Key);
void SetAllAction(int Action);

void ReleaseCharacters(void);
void CreateCharacterPointer(CHARACTER* c, int Type, unsigned char PositionX, unsigned char PositionY, float Rotation = 0.f);
CHARACTER* CreateCharacter(int Key, int Type, unsigned char PositionX, unsigned char PositionY, float Rotation = 0.f);
CHARACTER* CreateHero(int Key, CLASS_TYPE Class, int Skin = 0, float x = 0.f, float y = 0.f, float Rotation = 0.f);
CHARACTER* CreateMonster(EMonsterType Type, int PositionX, int PositionY, int Key = 0);
CHARACTER* CreateHellGate(char* ID, int Key, EMonsterType Index, int x, int y, int CreateFlag);

void SetAttackSpeed();
void SetPlayerShock(CHARACTER* c, int Hit);
void SetPlayerStop(CHARACTER* c);
void SetPlayerWalk(CHARACTER* c);

void SetPlayerAttack(CHARACTER* c);
void SetPlayerDie(CHARACTER* c);
void SetPlayerMagic(CHARACTER* c);
void SetPlayerTeleport(CHARACTER* c);
void SetPlayerHighBowAttack(CHARACTER* c);
void SetCharacterClass(CHARACTER* c);
void SetCharacterScale(CHARACTER* c);
void SetChangeClass(CHARACTER* c);
int LevelConvert(BYTE Level);
float CharacterMoveSpeed(CHARACTER* c);

bool CheckMonsterSkill(CHARACTER* c, OBJECT* o);
bool CheckMonsterInRange(CHARACTER* c, float Range);
bool CharacterAnimation(CHARACTER* c, OBJECT* o);
bool AttackStage(CHARACTER* c, OBJECT* o);

void RenderGuild(OBJECT* o, int Type = -1, vec3_t vPos = NULL);
void RenderLight(OBJECT* o, int Texture, float Scale, int Bone, float x, float y, float z);
void RenderProtectGuildMark(CHARACTER* c);

void MakeElfHelper(CHARACTER* c);
int GetFenrirType(CHARACTER* c);

extern int       EquipmentLevelSet;
extern bool      g_bAddDefense;

void CreateJoint(int Type, vec3_t Position, vec3_t TargetPosition, vec3_t Angle, int SubType = 0, OBJECT* Target = NULL, float Scale = 10.f, short PK = -1, WORD SkillIndex = 0, WORD SkillSerialNum = 0, int iChaIndex = -1, const float* vColor = NULL, short int sTargetIndex = -1);
void CreateJointFpsChecked(int Type, vec3_t Position, vec3_t TargetPosition, vec3_t Angle, int SubType = 0, OBJECT* Target = NULL, float Scale = 10.f, short PK = -1, WORD SkillIndex = 0, WORD SkillSerialNum = 0, int iChaIndex = -1, const float* vColor = NULL, short int sTargetIndex = -1);
bool RenderCharacterBackItem(CHARACTER* c, OBJECT* o, bool bTranslate);
bool IsBackItem(CHARACTER* c, int iType);

bool IsPlayer(CHARACTER* c);
bool IsMonster(CHARACTER* c);

#endif