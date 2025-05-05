// NewUIOptionWindow.cpp: implementation of the CNewUIOptionWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewUIOptionWindow.h"
#include "NewUISystem.h"
#include "ZzzTexture.h"
#include "DSPlaySound.h"

using namespace SEASON3B;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SEASON3B::CNewUIOptionWindow::CNewUIOptionWindow()
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
    m_iResolutionIndex = 0;  // Default to first resolution
}

SEASON3B::CNewUIOptionWindow::~CNewUIOptionWindow()
{
    Release();
}

bool SEASON3B::CNewUIOptionWindow::Create(CNewUIManager* pNewUIMng, int x, int y)
{
    if (NULL == pNewUIMng)
        return false;

    m_pNewUIMng = pNewUIMng;
    m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_OPTION, this);
    SetPos(x, y);
    LoadImages();
    SetButtonInfo();

    PopulateResolutionList();

    Show(false);
    return true;
}

void SEASON3B::CNewUIOptionWindow::SetButtonInfo()
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

void SEASON3B::CNewUIOptionWindow::Release()
{
    UnloadImages();

    if (m_pNewUIMng)
    {
        m_pNewUIMng->RemoveUIObj(this);
        m_pNewUIMng = NULL;
    }
}

void SEASON3B::CNewUIOptionWindow::SetPos(int x, int y)
{
    m_Pos.x = x;
    m_Pos.y = y;
}

bool SEASON3B::CNewUIOptionWindow::UpdateMouseEvent()
{
    if (m_BtnClose.UpdateMouseEvent() == true)
    {
        g_pNewUISystem->Hide(SEASON3B::INTERFACE_OPTION);
        return false;
    }

    if (m_BtnResLeft.UpdateMouseEvent() == true)
    {
        if (m_iResolutionIndex > 0)
        {
            m_iResolutionIndex--;
            PlayBuffer(SOUND_CLICK01);  // Play click sound
        }
        return false;
    }

    if (m_BtnResRight.UpdateMouseEvent() == true)
    {
        if (m_iResolutionIndex < (int)m_ResolutionList.size() - 1)
        {
            m_iResolutionIndex++;
            PlayBuffer(SOUND_CLICK01);  // Play click sound
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

bool SEASON3B::CNewUIOptionWindow::UpdateKeyEvent()
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

bool SEASON3B::CNewUIOptionWindow::Update()
{
    return true;
}

bool SEASON3B::CNewUIOptionWindow::Render()
{
    EnableAlphaTest();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    RenderFrame();
    RenderContents();
    RenderButtons();
    DisableAlphaBlend();
    return true;
}

float SEASON3B::CNewUIOptionWindow::GetLayerDepth()	//. 10.5f
{
    return 10.5f;
}

float SEASON3B::CNewUIOptionWindow::GetKeyEventOrder()	// 10.f;
{
    return 10.0f;
}

void SEASON3B::CNewUIOptionWindow::OpenningProcess()
{
}

void SEASON3B::CNewUIOptionWindow::ClosingProcess()
{
    ApplyResolutionChange();
}

void SEASON3B::CNewUIOptionWindow::LoadImages()
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

void SEASON3B::CNewUIOptionWindow::UnloadImages()
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

void SEASON3B::CNewUIOptionWindow::RenderFrame()
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

void SEASON3B::CNewUIOptionWindow::RenderContents()
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

void SEASON3B::CNewUIOptionWindow::RenderButtons()
{
    m_BtnClose.Render();

    m_BtnResLeft.Render();
    m_BtnResRight.Render();

    if (!m_ResolutionList.empty() && m_iResolutionIndex >= 0 && m_iResolutionIndex < (int)m_ResolutionList.size())
    {
        g_pRenderText->SetFont(g_hFont);
        g_pRenderText->SetTextColor(255, 255, 255, 255);
        g_pRenderText->SetBgColor(0);
        g_pRenderText->RenderText(m_Pos.x + 40, m_Pos.y + 235, L"Resolution:");
        g_pRenderText->RenderText(m_Pos.x + 110, m_Pos.y + 235, m_ResolutionList[m_iResolutionIndex].Text);
        //g_pRenderText->RenderText(m_Pos.x + 140, m_Pos.y + 235, m_ResolutionList[m_iResolutionIndex].Text);
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

void SEASON3B::CNewUIOptionWindow::SetAutoAttack(bool bAuto)
{
    m_bAutoAttack = bAuto;
}

bool SEASON3B::CNewUIOptionWindow::IsAutoAttack()
{
    return m_bAutoAttack;
}

void SEASON3B::CNewUIOptionWindow::SetWhisperSound(bool bSound)
{
    m_bWhisperSound = bSound;
}

bool SEASON3B::CNewUIOptionWindow::IsWhisperSound()
{
    return m_bWhisperSound;
}

void SEASON3B::CNewUIOptionWindow::SetSlideHelp(bool bHelp)
{
    m_bSlideHelp = bHelp;
}

bool SEASON3B::CNewUIOptionWindow::IsSlideHelp()
{
    return m_bSlideHelp;
}

void SEASON3B::CNewUIOptionWindow::SetVolumeLevel(int iVolume)
{
    m_iVolumeLevel = iVolume;
}

int SEASON3B::CNewUIOptionWindow::GetVolumeLevel()
{
    return m_iVolumeLevel;
}

void SEASON3B::CNewUIOptionWindow::SetRenderLevel(int iRender)
{
    m_iRenderLevel = iRender;
}

int SEASON3B::CNewUIOptionWindow::GetRenderLevel()
{
    return m_iRenderLevel;
}

void SEASON3B::CNewUIOptionWindow::SetRenderAllEffects(bool bRenderAllEffects)
{
    m_bRenderAllEffects = bRenderAllEffects;
}

bool SEASON3B::CNewUIOptionWindow::GetRenderAllEffects()
{
    return m_bRenderAllEffects;
}

void SEASON3B::CNewUIOptionWindow::PopulateResolutionList()
{
    m_ResolutionList.clear();

    // Add common resolutions
    RESOLUTION_INFO res;

    // 800x600
    res.Width = 800;
    res.Height = 600;
    swprintf(res.Text, L"800 x 600");
    m_ResolutionList.push_back(res);

    // 1024x768
    res.Width = 1024;
    res.Height = 768;
    swprintf(res.Text, L"1024 x 768");
    m_ResolutionList.push_back(res);

    // 1280x720
    res.Width = 1280;
    res.Height = 720;
    swprintf(res.Text, L"1280 x 720");
    m_ResolutionList.push_back(res);

    // 1366x768
    res.Width = 1366;
    res.Height = 768;
    swprintf(res.Text, L"1366 x 768");
    m_ResolutionList.push_back(res);

    // 1920x1080
    res.Width = 1920;
    res.Height = 1080;
    swprintf(res.Text, L"1920 x 1080");
    m_ResolutionList.push_back(res);

    // Find and set the current resolution index based on the current game resolution
    // This would depend on how the game stores current resolution
    // For now, just default to the first resolution
    m_iResolutionIndex = 0;
}


void SEASON3B::CNewUIOptionWindow::SetResolutionIndex(int iIndex)
{
    if (iIndex >= 0 && iIndex < (int)m_ResolutionList.size())
    {
        m_iResolutionIndex = iIndex;
    }
}

int SEASON3B::CNewUIOptionWindow::GetResolutionIndex()
{
    return m_iResolutionIndex;
}

SEASON3B::CNewUIOptionWindow::RESOLUTION_INFO SEASON3B::CNewUIOptionWindow::GetCurrentResolution()
{
    if (!m_ResolutionList.empty() && m_iResolutionIndex >= 0 && m_iResolutionIndex < (int)m_ResolutionList.size())
    {
        return m_ResolutionList[m_iResolutionIndex];
    }

    // Return default resolution if list is empty or index is invalid
    RESOLUTION_INFO defaultRes;
    defaultRes.Width = 800;
    defaultRes.Height = 600;
    swprintf(defaultRes.Text, L"800 x 600");
    return defaultRes;
}

// Step 15: Add a method to apply resolution changes
void SEASON3B::CNewUIOptionWindow::ApplyResolutionChange()
{
    if (!m_ResolutionList.empty() && m_iResolutionIndex >= 0 && m_iResolutionIndex < (int)m_ResolutionList.size())
    {
        RESOLUTION_INFO res = m_ResolutionList[m_iResolutionIndex];
        // Call the game's resolution change function (this would depend on the engine)
        // For example:
        // g_pGameEngine->ChangeResolution(res.Width, res.Height);
        g_ConsoleDebug->Write(MCD_RECEIVE, L"%dx%d", res.Width, res.Height);
        g_ConsoleDebug->Write(MCD_ERROR, L"%dx%d", res.Width, res.Height);
    }
}
