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

class FrameAuthors : public wxDialog
{
    private:

    protected:
        wxSearchCtrl* m_SearchCtrl_Authors;
        wxButton* m_Button_New;
        wxCheckListBox* m_CheckList_Authors;
        wxButton* m_Button_Ok;

    public:
        FrameAuthors(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(400, 300), long style = wxDEFAULT_DIALOG_STYLE);
        ~FrameAuthors();

};
