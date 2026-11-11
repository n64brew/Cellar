#pragma once

typedef struct IUnknown IUnknown;

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/wrapsizer.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/frame.h>

class FrameTags : public wxFrame
{
    private:

    protected:
        wxStaticText* m_StaticText_SDK;
        wxCheckBox* m_CheckBox_SDKLibultra;
        wxCheckBox* m_CheckBox_SDKLibdragon;
        wxStaticText* m_StaticText_Requirements;
        wxCheckBox* m_CheckBox_RequirementsEPak;
        wxStaticText* m_StaticText_Support;
        wxCheckBox* m_CheckBox_SupportNTSC;
        wxCheckBox* m_CheckBox_SupportPAL;
        wxCheckBox* m_CheckBox_SupportMPAL;
        wxCheckBox* m_CheckBox_Support64Drive;
        wxCheckBox* m_CheckBox_SupportED;
        wxCheckBox* m_CheckBox_SupportSC64;
        wxCheckBox* m_CheckBox_InaccurateEmulator;
        wxCheckBox* m_CheckBox_AccurateEmulator;
        wxStaticText* m_StaticText_Saving;
        wxCheckBox* m_CheckBox_SavingEEPROM;
        wxCheckBox* m_CheckBox_SavingSRAM;
        wxCheckBox* m_CheckBox_SavingFlashRAM;
        wxCheckBox* m_CheckBox_SavingCPak;
        wxStaticText* m_StaticText_Players;
        wxCheckBox* m_CheckBox_Players1;
        wxCheckBox* m_CheckBox_Players2;
        wxCheckBox* m_CheckBox_Players3;
        wxCheckBox* m_CheckBox_Players4;
        wxCheckBox* m_CheckBox_Players0;
        wxCheckBox* m_CheckBox_PlayersMulti;
        wxStaticText* m_StaticText_Other;
        wxCheckBox* m_CheckBox_OtherSource;
        wxCheckBox* m_CheckBox_OtherRumble;
        wxButton* m_Button_Ok;

    public:
        FrameTags(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Tags"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(442, 423), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
        ~FrameTags();
};