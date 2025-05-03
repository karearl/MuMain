void OpenTextData();

void DeleteNpcs();
void OpenNpc(int Type);
void DeleteMonsters();
void OpenMonsterModel(int Type); // EMonsterModelType

void OpenModel(int Type, wchar_t* Dir, wchar_t* ModelFileName, ...);
void OpenInterfaceData();
void OpenBasicData(HDC hDC);
void OpenMainData();
void ReleaseMainData();
//void OpenWorld(int World);
//void SaveWorld(int Wordl);
void OpenFont();
void OpenLogoSceneData();
void ReleaseLogoSceneData();
void OpenCharacterSceneData();
void ReleaseCharacterSceneData();

void OpenPlayerTextures();
void OpenItemTextures();

void OpenSkillHotKey(const wchar_t* File);
void SaveOptions();
void SaveMacro(const wchar_t* FileName);
void OpenMacro(const wchar_t* FileName);

extern wchar_t AbuseFilter[][20];
extern wchar_t AbuseNameFilter[][20];
extern int  AbuseFilterNumber;
extern int  AbuseNameFilterNumber;
extern std::wstring g_strSelectedML;