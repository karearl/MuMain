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
bool g_bUseWindowMode;

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

    m_iResolutionIndex = 0;
    m_bFullscreen = !g_bUseWindowMode;
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

    LoadOptionsFromRegistry();
    LoadImages();
    PopulateResolutionList();
    SetButtonInfo();

    Show(false);
    return true;
}

void CNewUIOptionWindow::SetButtonInfo()
{
    m_BtnClose.ChangeTextBackColor(RGBA(255, 255, 255, 0));
    m_BtnClose.ChangeButtonImgState(true, IMAGE_OPTION_BTN_CLOSE, true);
    m_BtnClose.ChangeButtonInfo(m_Pos.x + 68, m_Pos.y + 290, 54, 30);
    m_BtnClose.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
    m_BtnClose.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));

    m_BtnResLeft.ChangeTextBackColor(RGBA(255, 255, 255, 0));
    m_BtnResLeft.ChangeButtonImgState(true, IMAGE_OPTION_RESOLUTION_BTN_L, true);
    m_BtnResLeft.ChangeButtonInfo(m_Pos.x + 85, m_Pos.y + 256, 20, 22);
    m_BtnResLeft.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
    m_BtnResLeft.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));

    m_BtnResRight.ChangeTextBackColor(RGBA(255, 255, 255, 0));
    m_BtnResRight.ChangeButtonImgState(true, IMAGE_OPTION_RESOLUTION_BTN_R, true);
    m_BtnResRight.ChangeButtonInfo(m_Pos.x + 150, m_Pos.y + 256, 20, 22);
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

    // Only process resolution button clicks when in windowed mode
    if (!m_bFullscreen) {
        if (m_BtnResLeft.UpdateMouseEvent())
        {
            if (m_iResolutionIndex > 0)
            {
                m_iResolutionIndex--;
                PlayBuffer(SOUND_CLICK01);
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
                ApplyResolutionChange();
            }
            return false;
        }
    }

    if (IsPress(VK_LBUTTON) && CheckMouseIn(m_Pos.x + 150, m_Pos.y + 43, 15, 15))
    {
        m_bAutoAttack = !m_bAutoAttack;
    }
    if (IsPress(VK_LBUTTON) && CheckMouseIn(m_Pos.x + 150, m_Pos.y + 65, 15, 15))
    {
        m_bWhisperSound = !m_bWhisperSound;
    }
    if (IsPress(VK_LBUTTON) && CheckMouseIn(m_Pos.x + 150, m_Pos.y + 127, 15, 15))
    {
        m_bSlideHelp = !m_bSlideHelp;
    }

    if (IsPress(VK_LBUTTON) && CheckMouseIn(m_Pos.x + 150, m_Pos.y + 210, 15, 15))
    {
        m_bRenderAllEffects = !m_bRenderAllEffects;
    }

    // Fixed fullscreen checkbox position, same for both states
    if (IsPress(VK_LBUTTON) && CheckMouseIn(m_Pos.x + 150, m_Pos.y + 232, 15, 15))
    {
        m_bFullscreen = !m_bFullscreen;
        ApplyResolutionChange();
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
        if (IsRepeat(VK_LBUTTON))
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
        if (IsPress(VK_ESCAPE) == true)
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
    DeleteBitmap(IMAGE_OPTION_RESOLUTION_BTN_L);
    DeleteBitmap(IMAGE_OPTION_RESOLUTION_BTN_R);
}

void CNewUIOptionWindow::RenderFrame()
{
    float x, y;
    x = m_Pos.x; // 225
    y = m_Pos.y; // 70

    RenderImage(IMAGE_OPTION_FRAME_BACK, x, y, 190.f, 329.f); // Renders the gray background of the option menu
    RenderImage(IMAGE_OPTION_FRAME_UP, x, y, 190.f, 64.f); // Renders the header image of the option menu

    y += 64.f;
    for (int i = 0; i < 22; ++i) // Renders the Height of the options menu (as i increased the height increased)
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
    // Base Y positions for menu items
    const float baseY = m_Pos.y + 46.f;
    const float pointX = m_Pos.x + 20.f;
    float pointY = baseY;

    // Set up text settings
    g_pRenderText->SetFont(g_hFont);
    g_pRenderText->SetTextColor(255, 255, 255, 255);
    g_pRenderText->SetBgColor(0);

    // Render all option text labels
    g_pRenderText->RenderText(m_Pos.x + 40, baseY + 2, GlobalText[386]);
    g_pRenderText->RenderText(m_Pos.x + 40, baseY + 24, GlobalText[387]);
    g_pRenderText->RenderText(m_Pos.x + 40, baseY + 46, GlobalText[389]);
    g_pRenderText->RenderText(m_Pos.x + 40, baseY + 86, GlobalText[919]);
    g_pRenderText->RenderText(m_Pos.x + 40, baseY + 108, GlobalText[1840]);
    g_pRenderText->RenderText(m_Pos.x + 40, baseY + 174, L"Render Full Effects");
    g_pRenderText->RenderText(m_Pos.x + 40, baseY + 196, L"Fullscreen");

    // Only show resolution text in windowed mode
    if (!m_bFullscreen) {
        g_pRenderText->RenderText(m_Pos.x + 40, baseY + 218, L"Resolution");
    }

    // Render all points in sequence
    RenderImage(IMAGE_OPTION_POINT, pointX, pointY, 10.f, 10.f); // Auto Attack
    pointY += 22.f;
    RenderImage(IMAGE_OPTION_POINT, pointX, pointY, 10.f, 10.f); // Whisper
    pointY += 22.f;
    RenderImage(IMAGE_OPTION_POINT, pointX, pointY, 10.f, 10.f); // Volume
    pointY += 40.f;
    RenderImage(IMAGE_OPTION_POINT, pointX, pointY, 10.f, 10.f); // Slide Help
    pointY += 22.f;
    RenderImage(IMAGE_OPTION_POINT, pointX, pointY, 10.f, 10.f); // Effects Level
    pointY += 66.f;
    RenderImage(IMAGE_OPTION_POINT, pointX, pointY, 10.f, 10.f); // Render Effects
    pointY += 22.f;
    RenderImage(IMAGE_OPTION_POINT, pointX, pointY, 10.f, 10.f); // Fullscreen

    // Only show resolution point in windowed mode
    if (!m_bFullscreen) {
        pointY += 22.f;
        RenderImage(IMAGE_OPTION_POINT, pointX, pointY, 10.f, 10.f); // Resolution
    }
}

void CNewUIOptionWindow::RenderButtons()
{
    m_BtnClose.Render();

    // Standard checkboxes and controls
    if (m_bAutoAttack) {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 43, 15, 15, 0, 0);
    }
    else {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 43, 15, 15, 0, 15.f);
    }

    if (m_bWhisperSound) {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 65, 15, 15, 0, 0);
    }
    else {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 65, 15, 15, 0, 15.f);
    }

    if (m_bSlideHelp) {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 127, 15, 15, 0, 0);
    }
    else {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 127, 15, 15, 0, 15.f);
    }

    RenderImage(IMAGE_OPTION_VOLUME_BACK, m_Pos.x + 33, m_Pos.y + 104, 124.f, 16.f);
    if (m_iVolumeLevel > 0) {
        RenderImage(IMAGE_OPTION_VOLUME_COLOR, m_Pos.x + 33, m_Pos.y + 104, 124.f * 0.1f * (m_iVolumeLevel), 16.f);
    }

    RenderImage(IMAGE_OPTION_EFFECT_BACK, m_Pos.x + 25, m_Pos.y + 168, 141.f, 29.f);
    if (m_iRenderLevel >= 0) {
        RenderImage(IMAGE_OPTION_EFFECT_COLOR, m_Pos.x + 25, m_Pos.y + 168, 141.f * 0.2f * (m_iRenderLevel + 1), 29.f);
    }

    if (m_bRenderAllEffects) {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 210, 15, 15, 0, 0);
    }
    else {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 210, 15, 15, 0, 15.f);
    }

    // Fullscreen checkbox - at a fixed position
    if (m_bFullscreen) {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 232, 15, 15, 0, 0);
    }
    else {
        RenderImage(IMAGE_OPTION_BTN_CHECK, m_Pos.x + 150, m_Pos.y + 232, 15, 15, 0, 15.f);
    }

    // Resolution controls - only shown in windowed mode
    if (!m_bFullscreen) {
        m_BtnResLeft.Render();
        m_BtnResRight.Render();

        if (!m_ResolutionList.empty() && m_iResolutionIndex >= 0 && m_iResolutionIndex < (int)m_ResolutionList.size()) {
            g_pRenderText->RenderText(m_Pos.x + 115, m_Pos.y + 264, m_ResolutionList[m_iResolutionIndex].Text);
        }
    }
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
    //AddResolutionToList(1024, 768);   // Index 2
    AddResolutionToList(1280, 1024);  // Index 3
    //AddResolutionToList(1600, 1200);  // Index 4
    //AddResolutionToList(1864, 1400);  // Index 5
    //AddResolutionToList(1600, 900);   // Index 6
    //AddResolutionToList(1600, 1280);  // Index 7
    //AddResolutionToList(1680, 1050);  // Index 8
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
    swprintf(defaultRes.Text, sizeof(defaultRes.Text) / sizeof(wchar_t), L"800 x 600");
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

        // Check if only the fullscreen mode has changed
        DWORD currentStyle = GetWindowLong(g_hWnd, GWL_STYLE);
        bool currentlyFullscreen = !(currentStyle & WS_CAPTION); // No caption means fullscreen
        bool fullscreenChanged = (m_bFullscreen != currentlyFullscreen);
        bool resolutionChanged = (targetRes.Width != currentWidth || targetRes.Height != currentHeight);

        // If neither fullscreen nor resolution changed, no need to do anything
        if (!fullscreenChanged && !resolutionChanged) {
            g_ConsoleDebug->Write(MCD_NORMAL, L"ApplyResolutionChange: No changes needed.");

            // Save the index just in case it wasn't saved correctly before
            HKEY hKey; DWORD dwDisp; LONG lResult = RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_PATH_MUCONFIG, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);
            if (lResult == ERROR_SUCCESS) {
                RegSetValueEx(hKey, L"Resolution", 0, REG_DWORD, (const BYTE*)&selectedIndex, sizeof(int));

                // Save fullscreen mode as well
                DWORD windowMode = m_bFullscreen ? 0 : 1; // Inverse of fullscreen is window mode
                RegSetValueEx(hKey, L"WindowMode", 0, REG_DWORD, (const BYTE*)&windowMode, sizeof(DWORD));

                RegCloseKey(hKey);
            }
            return; // No change needed
        }

        g_ConsoleDebug->Write(MCD_NORMAL, L"Applying resolution: %s (%dx%d), Index: %d, Fullscreen: %s",
            targetRes.Text, targetRes.Width, targetRes.Height, selectedIndex,
            m_bFullscreen ? L"Yes" : L"No");

        // --- Step 1: Update Global State ---
        WindowWidth = targetRes.Width;
        WindowHeight = targetRes.Height;
        g_fScreenRate_x = (float)WindowWidth / 640.f;
        g_fScreenRate_y = (float)WindowHeight / 480.f;
        g_bUseWindowMode = !m_bFullscreen; // Update the global window mode variable

        // --- Step 2: Resize Window Frame ---
        if (g_hWnd && IsWindow(g_hWnd))
        {
            DWORD dwStyle = GetWindowLong(g_hWnd, GWL_STYLE);
            DWORD dwExStyle = GetWindowLong(g_hWnd, GWL_EXSTYLE);

            if (m_bFullscreen) {
                // First make sure we're in windowed mode with the correct resolution
                dwStyle |= (WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
                dwExStyle |= WS_EX_APPWINDOW;
                SetWindowLong(g_hWnd, GWL_STYLE, dwStyle);
                SetWindowLong(g_hWnd, GWL_EXSTYLE, dwExStyle);

                // Apply the resolution in windowed mode first
                BOOL bMenu = GetMenu(g_hWnd) != NULL;
                RECT rc = { 0, 0, WindowWidth, WindowHeight };
                AdjustWindowRectEx(&rc, dwStyle, bMenu, dwExStyle);
                int nFrameWidth = rc.right - rc.left;
                int nFrameHeight = rc.bottom - rc.top;
                int x = (GetSystemMetrics(SM_CXSCREEN) - nFrameWidth) / 2;
                int y = (GetSystemMetrics(SM_CYSCREEN) - nFrameHeight) / 2;
                SetWindowPos(g_hWnd, HWND_TOP, x, y, nFrameWidth, nFrameHeight,
                    SWP_NOZORDER | SWP_FRAMECHANGED);

                // Small delay to let the window update
                Sleep(50);

                // Now switch to fullscreen
                dwStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
                dwExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
                SetWindowLong(g_hWnd, GWL_STYLE, dwStyle);
                SetWindowLong(g_hWnd, GWL_EXSTYLE, dwExStyle);

                // Set fullscreen mode at current resolution
                int screenWidth = GetSystemMetrics(SM_CXSCREEN);
                int screenHeight = GetSystemMetrics(SM_CYSCREEN);
                SetWindowPos(g_hWnd, HWND_TOP, 0, 0, screenWidth, screenHeight,
                    SWP_NOZORDER | SWP_FRAMECHANGED);
            }
            else {
                // Switch to windowed mode with specified resolution
                dwStyle |= (WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
                dwExStyle |= WS_EX_APPWINDOW;

                // Apply new styles
                SetWindowLong(g_hWnd, GWL_STYLE, dwStyle);
                SetWindowLong(g_hWnd, GWL_EXSTYLE, dwExStyle);

                BOOL bMenu = GetMenu(g_hWnd) != NULL;

                RECT rc = { 0, 0, WindowWidth, WindowHeight };
                AdjustWindowRectEx(&rc, dwStyle, bMenu, dwExStyle);

                int nFrameWidth = rc.right - rc.left;
                int nFrameHeight = rc.bottom - rc.top;
                int x = (GetSystemMetrics(SM_CXSCREEN) - nFrameWidth) / 2;
                int y = (GetSystemMetrics(SM_CYSCREEN) - nFrameHeight) / 2;

                // Resize the window. This *should* trigger WM_SIZE.
                SetWindowPos(g_hWnd, HWND_TOP, x, y, nFrameWidth, nFrameHeight,
                    SWP_NOZORDER | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
            }

            // Force a repaint to refresh the UI
            InvalidateRect(g_hWnd, NULL, TRUE);
            UpdateWindow(g_hWnd);

            g_ConsoleDebug->Write(MCD_NORMAL, L"ApplyResolutionChange: Window style updated for %s mode.",
                m_bFullscreen ? L"fullscreen" : L"windowed");

            // --- Step 3: Save the new settings ---
            HKEY hKey;
            DWORD dwDisp;
            LONG lResult = RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_PATH_MUCONFIG, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);
            if (lResult == ERROR_SUCCESS)
            {
                // Save resolution index
                if (RegSetValueEx(hKey, L"Resolution", 0, REG_DWORD, (const BYTE*)&selectedIndex, sizeof(int)) == ERROR_SUCCESS) {
                    g_ConsoleDebug->Write(MCD_NORMAL, L"ApplyResolutionChange: Saved resolution index %d to registry.", selectedIndex);
                }
                else {
                    g_ConsoleDebug->Write(MCD_ERROR, L"ApplyResolutionChange: Failed to write 'Resolution' to registry (Error %ld).", GetLastError());
                }

                // Save window mode (inverse of fullscreen)
                DWORD windowMode = m_bFullscreen ? 0 : 1;
                if (RegSetValueEx(hKey, L"WindowMode", 0, REG_DWORD, (const BYTE*)&windowMode, sizeof(DWORD)) == ERROR_SUCCESS) {
                    g_ConsoleDebug->Write(MCD_NORMAL, L"ApplyResolutionChange: Saved window mode %d to registry.", windowMode);
                }
                else {
                    g_ConsoleDebug->Write(MCD_ERROR, L"ApplyResolutionChange: Failed to write 'WindowMode' to registry (Error %ld).", GetLastError());
                }

                RegCloseKey(hKey);
            }
            else {
                g_ConsoleDebug->Write(MCD_ERROR, L"ApplyResolutionChange: Failed to open/create registry key %s for saving (Error %ld).", REGISTRY_PATH_MUCONFIG, lResult);
            }
        }
        else {
            g_ConsoleDebug->Write(MCD_ERROR, L"ApplyResolutionChange: Invalid main window handle (g_hWnd). Cannot resize window.");
        }
    }
    else {
        g_ConsoleDebug->Write(MCD_ERROR, L"ApplyResolutionChange: Resolution list empty or index invalid!");
    }
}

bool CNewUIOptionWindow::LoadOptionsFromRegistry()
{
    HKEY hKey;
    DWORD dwType = REG_DWORD;
    DWORD dwSize = sizeof(DWORD);
    DWORD dwValue = 0;

    // Default values if registry read fails
    bool bSuccess = false;
    m_bFullscreen = !g_bUseWindowMode; // Use global default

    // Open the registry key
    LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, REGISTRY_PATH_MUCONFIG, 0, KEY_READ, &hKey);
    if (lResult == ERROR_SUCCESS)
    {
        // Read WindowMode value (inverse of fullscreen)
        lResult = RegQueryValueEx(hKey, L"WindowMode", 0, &dwType, (LPBYTE)&dwValue, &dwSize);
        if (lResult == ERROR_SUCCESS && dwType == REG_DWORD)
        {
            m_bFullscreen = (dwValue == 0); // 0 = fullscreen, 1 = windowed
            g_bUseWindowMode = !m_bFullscreen; // Update global variable
            bSuccess = true;
        }
        else
        {
            g_ConsoleDebug->Write(MCD_ERROR, L"LoadOptionsFromRegistry: Failed to read 'WindowMode' from registry (Error %ld)", lResult);
        }

        // Read Resolution index (we're already doing this in GetCurrentResolutionIndex)
        dwSize = sizeof(DWORD);
        lResult = RegQueryValueEx(hKey, L"Resolution", 0, &dwType, (LPBYTE)&dwValue, &dwSize);
        if (lResult == ERROR_SUCCESS && dwType == REG_DWORD)
        {
            // Just use this as debug info, actual resolution setting happens elsewhere
            g_ConsoleDebug->Write(MCD_NORMAL, L"LoadOptionsFromRegistry: Read Resolution index %d", dwValue);
        }

        RegCloseKey(hKey);
    }
    else
    {
        g_ConsoleDebug->Write(MCD_ERROR, L"LoadOptionsFromRegistry: Failed to open registry key %s (Error %ld)", REGISTRY_PATH_MUCONFIG, lResult);
    }

    return bSuccess;
}


#pragma region getters/setters
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

void CNewUIOptionWindow::SetFullscreen(bool bFullscreen)
{
    m_bFullscreen = bFullscreen;
}

bool CNewUIOptionWindow::GetFullscreen()
{
    return m_bFullscreen;
}
#pragma endregion