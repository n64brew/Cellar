#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/filepicker.h>
#include <wx/textctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/frame.h>

class FrameVideoInsert : public wxFrame
{
    private:

    protected:
        wxStaticText* m_Label_Path;
        wxFilePickerCtrl* m_FilePicker_Video;
        wxStaticText* m_Label_Caption;
        wxRichTextCtrl* m_RichText_Caption;
        wxButton* m_Button_Add;
        wxButton* m_Button_Cancel;
        wxWindow* m_Parent;
        wxString  m_BasePath;

        void m_Button_Add_OnButtonClick(wxCommandEvent& event);
        void m_Button_Cancel_OnButtonClick(wxCommandEvent& event);

    public:
        FrameVideoInsert(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Insert Video"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(400, 300), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
        ~FrameVideoInsert();

        void SetParent(wxWindow* parent, wxString path);
};