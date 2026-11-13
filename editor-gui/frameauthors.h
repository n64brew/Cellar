#pragma once

typedef struct IUnknown IUnknown;

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/srchctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/checklst.h>
#include <wx/dialog.h>
#include <vector>

class FrameAuthors : public wxDialog
{
    private:

    protected:
        std::vector<wxString>* m_Authors;
        std::vector<wxString> m_SelectedAuthors;
        wxSearchCtrl* m_SearchCtrl_Authors;
        wxButton* m_Button_New;
        wxCheckListBox* m_CheckList_Authors;
        wxButton* m_Button_Ok;
        
        void RefreshAuthors();
        void m_SearchCtrl_Authors_OnText(wxCommandEvent& event);
        void m_CheckList_Authors_OnBoxToggled(wxCommandEvent& event);
        void m_Button_New_OnClick(wxCommandEvent& event);
        void m_Button_Ok_OnClick(wxCommandEvent& event);

    public:
        FrameAuthors(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(400, 300), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
        ~FrameAuthors();

        void SetAuthors(std::vector<wxString>* allauthors, std::vector<wxString> romauthors);
        std::vector<wxString>* GetSelectedAuthors();
};
