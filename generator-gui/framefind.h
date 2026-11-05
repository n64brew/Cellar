#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/frame.h>

class FrameFind : public wxFrame
{
    private:

    protected:
        wxStaticText* m_Label_Find;
        wxTextCtrl* m_Input_Find;
        wxButton* m_Button_FindNext;
        wxStaticText* m_Label_Replace;
        wxTextCtrl* m_Input_Replace;
        wxButton* m_Button_Previous;
        wxButton* m_Button_Replace;
        wxButton* m_Button_ReplaceAll;
        wxButton* m_Button_Close;
        wxWindow* m_Parent;

        void m_Button_FindNext_OnButtonClick(wxCommandEvent& event);
        void m_Button_Previous_OnButtonClick(wxCommandEvent& event);
        void m_Button_Replace_OnButtonClick(wxCommandEvent& event);
        void m_Button_ReplaceAll_OnButtonClick(wxCommandEvent& event);
        void m_Button_Close_OnButtonClick(wxCommandEvent& event);

        bool FindNext(wxString find);
        bool FindPrevious(wxString find);

    public:
        FrameFind(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Find"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(400, 150), long style = wxDEFAULT_FRAME_STYLE|wxSTAY_ON_TOP|wxTAB_TRAVERSAL);
        ~FrameFind();

        void SetParent(wxWindow* parent);
};