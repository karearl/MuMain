// NewUIOptionWindow.cpp: implementation of the CNewUIOptionWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewUIOptionWindow.h"
#include "NewUISystem.h"
#include "ZzzTexture.h"
#include "DSPlaySound.h"

extern HWND g_hWnd;
extern float g_fScreenRate_x;
extern float g_fScreenRate_y;
extern bool g_bUseWindowMode;

const wchar_t* REGISTRY_PATH_MUCONFIG = L"SOFTWARE\\Webzen\\Mu\\Config";

using namespace SEASON3B;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNewUIOptionWindow::CNewUIOptionWindow()
{
    m_pNewUIMng = NULL;
    m_Pos.x = 0;
    m_Pos.y = 0;

    m_bAutoAttack = true;
    m_bWhisperSound = false;
    m_bSlideHelp = true;
    m_iVolumeLevel = 0;
    m_iRenderLevel = 4;
    m_bRenderAllEffects = true;

    // Initial index set later in PopulateResolutionList after list is filled
    m_iResolutionIndex = 0;
}

CNewUIOptionWindow::~CNewUIOptionWindow()
{
    Release();
}

bool CNewUIOptionWindow::Create(CNewUIManager* pNewUIMng, int x, int y)
{
    if (NULL == pNewUIMng)
        return false;

    m_pNewUIMng = pNewUIMng;
    m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_OPTION, this);
    SetPos(x, y);
    LoadImages();

    // Populate the list AND set the initial index based on current window size
    PopulateResolutionList();

    SetButtonInfo();

    Show(false);
    return true;
}

void CNewUIOptionWindow::SetButtonInfo()
{
    m_BtnClose.ChangeTextBackColor(RGBA(255, 255, 255, 0));
    m_BtnClose.ChangeButtonImgState(true, IMAGE_OPTION_BTN_CLOSE, true);
    //m_BtnClose.ChangeButtonInfo(m_Pos.x + 68, m_Pos.y + 229, 54, 30);
    m_BtnClose.ChangeButtonInfo(m_Pos.x + 68, m_Pos.y + 269, 54, 30);
    m_BtnClose.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
    m_BtnClose.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));


    // Set up resolution selection buttons
    m_BtnResLeft.ChangeTextBackColor(RGBA(255, 255, 255, 0));
    m_BtnResLeft.ChangeButtonImgState(true, IMAGE_OPTION_RESOLUTION_BTN_L, true);
    m_BtnResLeft.ChangeButtonInfo(m_Pos.x + 85, m_Pos.y + 227, 20, 22);
    m_BtnResLeft.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
    m_BtnResLeft.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));

    m_BtnResRight.ChangeTextBackColor(RGBA(255, 255, 255, 0));
    m_BtnResRight.ChangeButtonImgState(true, IMAGE_OPTION_RESOLUTION_BTN_R, true);
    m_BtnResRight.ChangeButtonInfo(m_Pos.x + 150, m_Pos.y + 227, 20, 22);
    m_BtnResRight.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
    m_BtnResRight.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
}

void CNewUIOptionWindow::Release()
{
    UnloadImages();

    if (m_pNewUIMng)
    {
        m_pNewUIMng->RemoveUIObj(this);
        m_pNewUIMng = NULL;
    }
}

void CNewUIOptionWindow::SetPos(int x, int y)
{
    m_Pos.x = x;
    m_Pos.y = y;
}

bool CNewUIOptionWindow::UpdateMouseEvent()
{
    if (!IsVisible()) return true;

    if (m_BtnClose.UpdateMouseEvent()) { 
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_OPTION);
		PlayBuffer(SOUND_CLICK01); return false;
	}

    bool resolutionChanged = false;
    if (m_BtnResLeft.UpdateMouseEvent())
    {
        if (m_iResolutionIndex > 0)
        {
            m_iResolutionIndex--;
            PlayBuffer(SOUND_CLICK01);
            resolutionChanged = true;
            ApplyResolutionChange();
        }
        return false;
    }

    if (m_BtnResRight.UpdateMouseEvent())
    {
        // Check against MEMBER list size
        if (m_iResolutionIndex < (int)m_ResolutionList.size() - 1)
        {
            m_iResolutionIndex++;
            PlayBuffer(SOUND_CLICK01);
            resolutionChanged = true;
            ApplyResolutionChange();
        }
        return false;
    }

    if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(m_Pos.x + 150, m_Pos.y + 43, 15, 15))
    {
        m_bAutoAttack = !m_bAutoAttack;
    }
    if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(m_Pos.x + 150, m_Pos.y + 65, 15, 15))
    {
        m_bWhisperSound = !m_bWhisperSound;
    }
    if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(m_Pos.x + 150, m_Pos.y + 127, 15, 15))
    {
        m_bSlideHelp = !m_bSlideHelp;
    }

    if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(m_Pos.x + 150, m_Pos.y + 210, 15, 15))
    {
        m_bRenderAllEffects = !m_bRenderAllEffects;
    }

    if (CheckMouseIn(m_Pos.x + 33 - 8, m_Pos.y + 104, 124 + 8, 16))
    {
        int iOldValue = m_iVolumeLevel;
        if (MouseWheel > 0)
        {
            MouseWheel = 0;
            m_iVolumeLevel++;
            if (m_iVolumeLevel > 10)
            {
                m_iVolumeLevel = 10;
            }
        }
        else if (MouseWheel < 0)
        {
            MouseWheel = 0;
            m_iVolumeLevel--;
            if (m_iVolumeLevel < 0)
            {
                m_iVolumeLevel = 0;
            }
        }
        if (SEASON3B::IsRepeat(VK_LBUTTON))
        {
            int x = MouseX - (m_Pos.x + 33);
            if (x < 0)
            {
                m_iVolumeLevel = 0;
            }
            else
            {
                float fValue = (10.f * x) / 124.f;
                m_iVolumeLevel = (int)fValue + 1;
            }
        }

        if (iOldValue != m_iVolumeLevel)
        {
            SetEffectVolumeLevel(m_iVolumeLevel);
        }
    }
    if (CheckMouseIn(m_Pos.x + 25, m_Pos.y + 168, 141, 29))
    {
        if (SEASON3B::IsRepeat(VK_LBUTTON))
        {
            int x = MouseX - (m_Pos.x + 25);
            float fValue = (5.f * x) / 141.f;
            m_iRenderLevel = (int)fValue;
        }
    }

    if (CheckMouseIn(m_Pos.x, m_Pos.y, 190, 269) == true)
    {
        return false;
    }

    return true;
}

bool CNewUIOptionWindow::UpdateKeyEvent()
{
    if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_OPTION) == true)
    {
        if (SEASON3B::IsPress(VK_ESCAPE) == true)
        {
            g_pNewUISystem->Hide(SEASON3B::INTERFACE_OPTION);
            PlayBuffer(SOUND_CLICK01);
            return false;
        }
    }

    return true;
}

bool CNewUIOptionWindow::Update()
{
    return true;
}

bool CNewUIOptionWindow::Render()
{
    EnableAlphaTest();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    RenderFrame();
    RenderContents();
    RenderButtons();
    DisableAlphaBlend();
    return true;
}

float CNewUIOptionWindow::GetLayerDepth()	//. 10.5f
{
    return 10.5f;
}

float CNewUIOptionWindow::GetKeyEventOrder()	// 10.f;
{
    return 10.0f;
}

void CNewUIOptionWindow::OpenningProcess()
{
}

void CNewUIOptionWindow::ClosingProcess()
{
    ApplyResolutionChange();
}

void CNewUIOptionWindow::LoadImages()
{
    LoadBitmap(L"Interface\\newui_button_close.tga", IMAGE_OPTION_BTN_CLOSE, GL_LINEAR);
    LoadBitmap(L"Interface\\newui_msgbox_back.jpg", IMAGE_OPTION_FRAME_BACK, GL_LINEAR);
    LoadBitmap(L"Interface\\newui_item_back03.tga", IMAGE_OPTION_FRAME_DOWN, GL_LINEAR);
    LoadBitmap(L"Interface\\newui_option_top.tga", IMAGE_OPTION_FRAME_UP, GL_LINEAR);
    LoadBitmap(L"Interface\\newui_option_back06(L).tga", IMAGE_OPTION_FRAME_LEFT, GL_LINEAR);
    LoadBitmap(L"Interface\\newui_option_back06(R).tga", IMAGE_OPTION_FRAME_RIGHT, GL_LINEAR);
    LoadBitmap(L"Interface\\newui_option_line.jpg", IMAGE_OPTION_LINE, GL_LINEAR);
    LoadBitmap(L"Interface\\newui_option_point.tga", IMAGE_OPTION_POINT, GL_LINEAR);
    LoadBitmap(L"Interface\\newui_option_check.tga", IMAGE_OPTION_BTN_CHECK, GL_LINEAR);
    LoadBitmap(L"Interface\\newui_option_effect03.tga", IMAGE_OPTION_EFFECT_BACK, GL_LINEAR);
    LoadBitmap(L"Interface\\newui_option_effect04.tga", IMAGE_OPTION_EFFECT_COLOR, GL_LINEAR);
    LoadBitmap(L"Interface\\newui_option_volume01.tga", IMAGE_OPTION_VOLUME_BACK, GL_LINEAR);
    LoadBitmap(L"Interface\\newui_option_volume02.tga", IMAGE_OPTION_VOLUME_COLOR, GL_LINEAR);
    LoadBitmap(L"Interface\\InGameShop\\IGS_Storage_Page_Right.tga", IMAGE_OPTION_RESOLUTION_BTN_R, GL_LINEAR);
    LoadBitmap(L"Interface\\InGameShop\\IGS_Storage_Page_Left.tga", IMAGE_OPTION_RESOLUTION_BTN_L, GL_LINEAR);

    // New resolution button images
    //LoadBitmap(L"Interface\\InGameShop\\ingame_Bt_page_L.tga", IMAGE_OPTION_RESOLUTION_BTN_L, GL_LINEAR);
    //LoadBitmap(L"Interface\\InGameShop\\ingame_Bt_page_R.tga", IMAGE_OPTION_RESOLUTION_BTN_R, GL_LINEAR);
}

void CNewUIOptionWindow::UnloadImages()
{
    DeleteBitmap(IMAGE_OPTION_BTN_CLOSE);
    DeleteBitmap(IMAGE_OPTION_FRAME_BACK);
    DeleteBitmap(IMAGE_OPTION_FRAME_DOWN);
    DeleteBitmap(IMAGE_OPTION_FRAME_UP);
    DeleteBitmap(IMAGE_OPTION_FRAME_LEFT);
    DeleteBitmap(IMAGE_OPTION_FRAME_RIGHT);
    DeleteBitmap(IMAGE_OPTION_LINE);
    DeleteBitmap(IMAGE_OPTION_POINT);
    DeleteBitmap(IMAGE_OPTION_BTN_CHECK);
    DeleteBitmap(IMAGE_OPTION_EFFECT_BACK);
    DeleteBitmap(IMAGE_OPTION_EFFECT_COLOR);
    DeleteBitmap(IMAGE_OPTION_VOLUME_BACK);
    DeleteBitmap(IMAGE_OPTION_VOLUME_COLOR);

    // New resolution button images
    DeleteBitmap(IMAGE_OPTION_RESOLUTION_BTN_L);
    DeleteBitmap(IMAGE_OPTION_RESOLUTION_BTN_R);
}

void CNewUIOptionWindow::RenderFrame()
{
    float x, y;
    x = m_Pos.x; // 225
    y = m_Pos.y; // 70

    //RenderImage(IMAGE_OPTION_FRAME_BACK, x, y, 190.f, 269.f);
    //RenderImage(IMAGE_OPTION_FRAME_UP, x, y, 190.f, 64.f);

    RenderImage(IMAGE_OPTION_FRAME_BACK, x, y, 190.f, 309.f); // Renders the gray background of the option menu
    RenderImage(IMAGE_OPTION_FRAME_UP, x, y, 190.f, 64.f); // Renders the header image of the option menu

    y += 64.f;
    //for (int i = 0; i < 16; ++i)
    for (int i = 0; i < 20; ++i) // Renders the Height of the options menu (as i increased the height increased)
    {
        RenderImage(IMAGE_OPTION_FRAME_LEFT, x, y, 21.f, 10.f); // Renders the left side of the option menu
        RenderImage(IMAGE_OPTION_FRAME_RIGHT, x + 190 - 21, y, 21.f, 10.f); // Renders the right side of the option menu
        y += 10.f;
    }

    RenderImage(IMAGE_OPTION_FRAME_DOWN, x, y, 190.f, 45.f); // Renders the bottom of the option menu (width x height)

    y = m_Pos.y + 60.f;
    RenderImage(IMAGE_OPTION_LINE, x + 18, y, 154.f, 2.f);
    y += 22.f;
    RenderImage(IMAGE_OPTION_LINE, x + 18, y, 154.f, 2.f);
    y += 40.f;
    RenderImage(IMAGE_OPTION_LINE, x + 18, y, 154.f, 2.f);
    y += 22.f;
    RenderImage(IMAGE_OPTION_LINE, x + 18, y, 154.f, 2.f);

    y += 60.f;
    RenderImage(IMAGE_OPTION_LINE, x + 18, y, 154.f, 2.f);
}

void CNewUIOptionWindow::RenderContents()
{
    float x, y;
    x = m_Pos.x + 20.f;
    y = m_Pos.y + 46.f;
    RenderImage(IMAGE_OPTION_POINT, x, y, 10.f, 10.f);
    y += 22.f;
    RenderImage(IMAGE_OPTION_POINT, x, y, 10.f, 10.f);
    y += 22.f;
    RenderImage(IMAGE_OPTION_POINT, x, y, 10.f, 10.f);
    y += 40.f;
    RenderImage(IMAGE_OPTION_POINT, x, y, 10.f, 10.f);
    y += 22.f;
    RenderImage(IMAGE_OPTION_POINT, x, y, 10.f, 10.f);

    y += 60.f;
    RenderImage(IMAGE_OPTION_POINT, x, y, 10.f, 10.f);

    // Add point for resolution
    y += 20.f;
    RenderImage(IMAGE_OPTION_POINT, x, y, 10.f, 10.f);

    g_pRenderText->SetFont(g_hFont);
    g_pRenderText->SetTextColor(255, 255, 255, 255);
    g_pRenderText->SetBgColor(0);
    g_pRenderText->RenderText(m_Pos.x + 40, m_Pos.y + 48, GlobalText[386]);
    g_pRenderText->RenderText(m_Pos.x + 40, m_Pos.y + 70, GlobalText[387]);
    g_pRenderText->RenderText(m_Pos.x + 40, m_Pos.y + 92, GlobalText[389]);
    g_pRenderText->RenderText(m_Pos.x + 40, m_Pos.y + 132, GlobalText[919]);
    g_pRenderText->RenderText(m_Pos.x + 40, m_Pos.y + 154, GlobalText[1840]);
    g_pRenderText->RenderText(m_Pos.x + 40, m_Pos.y + 154+60, L"Render Full Effects");
}

void CNewUIOptionWindow::RenderButtons()
{
    m_BtnClose.Render();
    m_BtnResLeft.Render();
    m_BtnResRight.Render();

    // Render Resolution Text using the MEMBER list and current UI index
    if (!m_ResolutionList.empty() && m_iResolutionIndex >= 0 && m_iResolutionIndex < (int)m_ResolutionList.size())
    {
        g_pRenderText->SetFont(g_hFont);
        g_pRenderText->SetTextColor(255, 255, 255, 255);
        g_pRenderText->SetBgColor(0);
        g_pRenderText->RenderText(m_Pos.x + 40, m_Pos.y + 235, L"Resolution:"); // Label
        g_pRenderText->RenderText(m_Pos.x + 110, m_Pos.y + 235, m_ResolutionList[m_iResolutionIndex].Text); // Display text
    }

    if (m_bAutoAttack)
    {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 43, 15, 15, 0, 0);
    }
    else
    {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 43, 15, 15, 0, 15.f);
    }

    if (m_bWhisperSound)
    {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 65, 15, 15, 0, 0);
    }
    else
    {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 65, 15, 15, 0, 15.f);
    }

    if (m_bSlideHelp)
    {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 127, 15, 15, 0, 0);
    }
    else
    {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 127, 15, 15, 0, 15.f);
    }

    RenderImage(IMAGE_OPTION_VOLUME_BACK, m_Pos.x + 33, m_Pos.y + 104, 124.f, 16.f);
    if (m_iVolumeLevel > 0)
    {
        RenderImage(IMAGE_OPTION_VOLUME_COLOR, m_Pos.x + 33, m_Pos.y + 104, 124.f * 0.1f * (m_iVolumeLevel), 16.f);
    }

    RenderImage(IMAGE_OPTION_EFFECT_BACK, m_Pos.x + 25, m_Pos.y + 168, 141.f, 29.f);
    if (m_iRenderLevel >= 0)
    {
        RenderImage(IMAGE_OPTION_EFFECT_COLOR, m_Pos.x + 25, m_Pos.y + 168, 141.f * 0.2f * (m_iRenderLevel + 1), 29.f);
    }

    if (m_bRenderAllEffects)
    {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 210, 15, 15, 0, 0);
    }
    else
    {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 210, 15, 15, 0, 15.f);
    }
}

void CNewUIOptionWindow::SetAutoAttack(bool bAuto)
{
    m_bAutoAttack = bAuto;
}

bool CNewUIOptionWindow::IsAutoAttack()
{
    return m_bAutoAttack;
}

void CNewUIOptionWindow::SetWhisperSound(bool bSound)
{
    m_bWhisperSound = bSound;
}

bool CNewUIOptionWindow::IsWhisperSound()
{
    return m_bWhisperSound;
}

void CNewUIOptionWindow::SetSlideHelp(bool bHelp)
{
    m_bSlideHelp = bHelp;
}

bool CNewUIOptionWindow::IsSlideHelp()
{
    return m_bSlideHelp;
}

void CNewUIOptionWindow::SetVolumeLevel(int iVolume)
{
    m_iVolumeLevel = iVolume;
}

int CNewUIOptionWindow::GetVolumeLevel()
{
    return m_iVolumeLevel;
}

void CNewUIOptionWindow::SetRenderLevel(int iRender)
{
    m_iRenderLevel = iRender;
}

int CNewUIOptionWindow::GetRenderLevel()
{
    return m_iRenderLevel;
}

void CNewUIOptionWindow::SetRenderAllEffects(bool bRenderAllEffects)
{
    m_bRenderAllEffects = bRenderAllEffects;
}

bool CNewUIOptionWindow::GetRenderAllEffects()
{
    return m_bRenderAllEffects;
}

void CNewUIOptionWindow::PopulateResolutionList()
{
    m_ResolutionList.clear();

    auto AddResolutionToList = [&](int w, int h) {
        RESOLUTION_INFO res;
        res.Width = w;
        res.Height = h;
        swprintf(res.Text, sizeof(res.Text) / sizeof(wchar_t), L"%d x %d", w, h);
        m_ResolutionList.push_back(res);
        };

    // Add resolutions corresponding exactly to indices 0-10 used in OpenInitFile's switch
    AddResolutionToList(640, 480);    // Index 0
    AddResolutionToList(800, 600);    // Index 1
    AddResolutionToList(1024, 768);   // Index 2
    AddResolutionToList(1280, 1024);  // Index 3
    AddResolutionToList(1600, 1200);  // Index 4
    AddResolutionToList(1864, 1400);  // Index 5
    AddResolutionToList(1600, 900);   // Index 6
    AddResolutionToList(1600, 1280);  // Index 7
    AddResolutionToList(1680, 1050);  // Index 8
    AddResolutionToList(1920, 1080);  // Index 9
    AddResolutionToList(2560, 1440);  // Index 10

    m_iResolutionIndex = this->GetCurrentResolutionIndex(); // Call MEMBER function

    // Boundary check the initial index against the *populated* list
    if (m_iResolutionIndex < 0 || m_iResolutionIndex >= (int)m_ResolutionList.size()) {
        g_ConsoleDebug->Write(MCD_ERROR, L"PopulateResolutionList: Initial index %d out of bounds, resetting to 0.", m_iResolutionIndex);
        // Find index matching 800x600 as a better default if index 1 exists
        bool defaultFound = false;
        for (size_t i = 0; i < m_ResolutionList.size(); ++i) {
            if (m_ResolutionList[i].Width == 800 && m_ResolutionList[i].Height == 600) {
                m_iResolutionIndex = (int)i;
                defaultFound = true;
                break;
            }
        }
        if (!defaultFound) m_iResolutionIndex = 0; // Fallback to absolute first if 800x600 isn't there
    }
}

void CNewUIOptionWindow::SetResolutionIndex(int iIndex)
{
    // Validate against the size of the MEMBER list
    if (iIndex >= 0 && iIndex < (int)m_ResolutionList.size())
    {
        m_iResolutionIndex = iIndex;
    }
    else {
        g_ConsoleDebug->Write(MCD_ERROR, L"SetResolutionIndex: Attempted to set invalid index %d.", iIndex);
    }
}

int CNewUIOptionWindow::GetResolutionIndex()
{
    // Validate the current index before returning
    if (m_iResolutionIndex >= 0 && m_iResolutionIndex < (int)m_ResolutionList.size()) {
        return m_iResolutionIndex;
    }
    // If the list is empty or index is bad somehow, return default
    g_ConsoleDebug->Write(MCD_ERROR, L"GetResolutionIndex: Current index %d out of bounds or list empty, returning 0.", m_iResolutionIndex);
    return 0;
}


int CNewUIOptionWindow::GetCurrentResolutionIndex()
{
    // Default value if registry read fails or value is invalid
    int currentSystemIndex = 1; // Default used in OpenInitFile seems to be 1 (800x600) if read fails

    // --- Read the value from the Registry ---
    HKEY hKey;
    DWORD dwSize = sizeof(int);
    LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, REGISTRY_PATH_MUCONFIG, 0, KEY_READ, &hKey);

    if (lResult == ERROR_SUCCESS)
    {
        lResult = RegQueryValueEx(hKey, L"Resolution", 0, NULL, (LPBYTE)&currentSystemIndex, &dwSize);
        if (lResult != ERROR_SUCCESS)
        {
            g_ConsoleDebug->Write(MCD_ERROR, L"GetCurrentResolutionIndex: Failed to read 'Resolution' from registry (Error %ld). Using default 1.", lResult);
            currentSystemIndex = 1; // Default to 1 if read fails
        }
        RegCloseKey(hKey);

        // Ensure index 0 from registry (which might mean 640x480) is handled if needed.
        // Your OpenInitFile seems to default to 1 if registry value is 0 or invalid.
        // Let's mimic that potential behavior for consistency, assuming index 0 isn't typically saved.
        if (currentSystemIndex == 0) {
            g_ConsoleDebug->Write(MCD_ERROR, L"GetCurrentResolutionIndex: Read index 0 from registry, treating as default index 1.");
            currentSystemIndex = 1;
        }

    }
    else {
        g_ConsoleDebug->Write(MCD_ERROR, L"GetCurrentResolutionIndex: Could not open registry key %s (Error %ld). Using default 1.", REGISTRY_PATH_MUCONFIG, lResult);
        currentSystemIndex = 1; // Use default if key cannot be opened
    }


    // --- Validate against the *member* list ---
    // This check should ideally happen AFTER m_ResolutionList is populated.
    // The constructor call might be too early. Let's move the call to PopulateResolutionList.

    // We return the read/default index here. Validation happens in PopulateResolutionList.
    return currentSystemIndex;
}


CNewUIOptionWindow::RESOLUTION_INFO CNewUIOptionWindow::GetCurrentResolution()
{
    int validIndex = GetResolutionIndex(); // Use validated index
    if (!m_ResolutionList.empty()) {
         // Check again just in case GetResolutionIndex returned default 0 but list was empty
         if (validIndex >= 0 && validIndex < (int)m_ResolutionList.size()) {
             return m_ResolutionList[validIndex]; // Use member list
         }
    }

    // Absolute fallback
    g_ConsoleDebug->Write(MCD_ERROR, L"GetCurrentResolution: Failed to get valid resolution, returning default.");
    RESOLUTION_INFO defaultRes = { 800, 600 };
    swprintf(defaultRes.Text, sizeof(defaultRes.Text)/sizeof(wchar_t), L"800 x 600");
    return defaultRes;
}

void CNewUIOptionWindow::ApplyResolutionChange()
{
    int selectedIndex = GetResolutionIndex(); // Get the validated index currently selected in the UI

    // Get current dimensions to avoid unnecessary changes
    RECT currentRect;
    int currentWidth = 0;
    int currentHeight = 0;
    if (g_hWnd && GetClientRect(g_hWnd, &currentRect)) {
        currentWidth = currentRect.right - currentRect.left;
        currentHeight = currentRect.bottom - currentRect.top;
    }
    else {
        g_ConsoleDebug->Write(MCD_ERROR, L"ApplyResolutionChange: Cannot get current client rect to check if change needed.");
        // Proceed anyway, assuming change is needed or initial state is unknown
    }

    if (!m_ResolutionList.empty() && selectedIndex >= 0 && selectedIndex < (int)m_ResolutionList.size())
    {
        RESOLUTION_INFO targetRes = m_ResolutionList[selectedIndex];

        // Check if the resolution actually needs changing
        if (targetRes.Width == currentWidth && targetRes.Height == currentHeight) {
            g_ConsoleDebug->Write(MCD_NORMAL, L"ApplyResolutionChange: Resolution %dx%d already set.", targetRes.Width, targetRes.Height);
            // Save the index just in case it wasn't saved correctly before (optional)
            HKEY hKey; DWORD dwDisp; LONG lResult = RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_PATH_MUCONFIG, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);
            if (lResult == ERROR_SUCCESS) { RegSetValueEx(hKey, L"Resolution", 0, REG_DWORD, (const BYTE*)&selectedIndex, sizeof(int)); RegCloseKey(hKey); }
            return; // No change needed
        }

        g_ConsoleDebug->Write(MCD_NORMAL, L"Applying resolution: %s (%dx%d), Index: %d", targetRes.Text, targetRes.Width, targetRes.Height, selectedIndex);

        // --- Step 1: Update Global State ---
        WindowWidth = targetRes.Width;
        WindowHeight = targetRes.Height;
        g_fScreenRate_x = (float)WindowWidth / 640.f;
        g_fScreenRate_y = (float)WindowHeight / 480.f;
        

        // --- Step 2: Resize Window Frame ---
        if (g_hWnd && IsWindow(g_hWnd))
        {
            DWORD dwStyle = GetWindowLong(g_hWnd, GWL_STYLE);
            DWORD dwExStyle = GetWindowLong(g_hWnd, GWL_EXSTYLE);
            BOOL bMenu = GetMenu(g_hWnd) != NULL;

            RECT rc = { 0, 0, WindowWidth, WindowHeight };
            AdjustWindowRectEx(&rc, dwStyle, bMenu, dwExStyle);

            int nFrameWidth = rc.right - rc.left;
            int nFrameHeight = rc.bottom - rc.top;
            int x = (GetSystemMetrics(SM_CXSCREEN) - nFrameWidth) / 2;
            int y = (GetSystemMetrics(SM_CYSCREEN) - nFrameHeight) / 2;

            // Resize the window. This *should* trigger WM_SIZE.
            if (!SetWindowPos(g_hWnd, HWND_TOP, x, y, nFrameWidth, nFrameHeight, SWP_NOZORDER | SWP_SHOWWINDOW)) {
                g_ConsoleDebug->Write(MCD_ERROR, L"ApplyResolutionChange: SetWindowPos failed! Error: %d", GetLastError());
                // Optionally revert globals here if SetWindowPos fails?
            }
            else {
                g_ConsoleDebug->Write(MCD_NORMAL, L"ApplyResolutionChange: SetWindowPos called successfully.");
                // NOTE: Graphics update now depends on the WM_SIZE handler in WndProc
            }

            // --- Step 3: Save the new index AFTER attempting the resize ---
            HKEY hKey;
            DWORD dwDisp;
            LONG lResult = RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_PATH_MUCONFIG, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);
            if (lResult == ERROR_SUCCESS)
            {
                if (RegSetValueEx(hKey, L"Resolution", 0, REG_DWORD, (const BYTE*)&selectedIndex, sizeof(int)) == ERROR_SUCCESS) {
                    g_ConsoleDebug->Write(MCD_NORMAL, L"ApplyResolutionChange: Saved resolution index %d to registry.", selectedIndex);
                }
                else {
                    g_ConsoleDebug->Write(MCD_ERROR, L"ApplyResolutionChange: Failed to write 'Resolution' to registry (Error %ld).", GetLastError());
                }
                RegCloseKey(hKey);
            }
            else {
                g_ConsoleDebug->Write(MCD_ERROR, L"ApplyResolutionChange: Failed to open/create registry key %s for saving (Error %ld).", REGISTRY_PATH_MUCONFIG, lResult);
            }
            // Update any live config variable if needed:
            // m_Resolution = selectedIndex; // If m_Resolution is a member/global tracking the saved state

        }
        else {
            g_ConsoleDebug->Write(MCD_ERROR, L"ApplyResolutionChange: Invalid main window handle (g_hWnd). Cannot resize window.");
        }
    }
    else {
        g_ConsoleDebug->Write(MCD_ERROR, L"ApplyResolutionChange: Resolution list empty or index invalid!");
    }
}