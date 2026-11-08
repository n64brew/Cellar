/***************************************************************
                      frameinsertvideo.cpp

Frame for conveniently inserting videos 
***************************************************************/

#include "frameinsertvideo.h"


/*==============================
    Main (Constructor)
    Initializes the class
==============================*/

FrameVideoInsert::FrameVideoInsert(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxFlexGridSizer* m_Sizer_Main;
    m_Sizer_Main = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_Main->AddGrowableCol(0);
    m_Sizer_Main->AddGrowableRow(3);
    m_Sizer_Main->SetFlexibleDirection(wxBOTH);
    m_Sizer_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    this->m_Label_Path = new wxStaticText(this, wxID_ANY, wxT("Path:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Path->Wrap(-1);
    m_Sizer_Main->Add(this->m_Label_Path, 0, wxALL, 5);

    this->m_FilePicker_Video = new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_USE_TEXTCTRL);
    m_Sizer_Main->Add(this->m_FilePicker_Video, 0, wxALL|wxEXPAND, 5);

    this->m_Label_Caption = new wxStaticText(this, wxID_ANY, wxT("Caption:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Caption->Wrap(-1);
    m_Sizer_Main->Add(this->m_Label_Caption, 0, wxALL, 5);

    this->m_RichText_Caption = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS);
    m_Sizer_Main->Add(this->m_RichText_Caption, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* m_Sizer_Buttons;
    m_Sizer_Buttons = new wxBoxSizer(wxHORIZONTAL);

    m_Sizer_Buttons->Add(0, 0, 1, wxEXPAND, 5);

    this->m_Button_Add = new wxButton(this, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Buttons->Add(this->m_Button_Add, 0, wxALL, 5);

    this->m_Button_Cancel = new wxButton(this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Buttons->Add(this->m_Button_Cancel, 0, wxALL, 5);

    m_Sizer_Main->Add(m_Sizer_Buttons, 1, wxEXPAND, 5);

    this->SetSizer(m_Sizer_Main);
    this->Layout();

    this->Centre(wxBOTH);

    // Connect Events
    this->m_Button_Add->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameVideoInsert::m_Button_Add_OnButtonClick), NULL, this);
    this->m_Button_Cancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameVideoInsert::m_Button_Cancel_OnButtonClick), NULL, this);
}


/*==============================
    App (Destructor)
    Cleans up the class before deletion
==============================*/

FrameVideoInsert::~FrameVideoInsert()
{
    this->m_Button_Add->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameVideoInsert::m_Button_Add_OnButtonClick), NULL, this);
    this->m_Button_Cancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameVideoInsert::m_Button_Cancel_OnButtonClick), NULL, this);
}


/*==============================
    m_Button_Add_OnButtonClick
    Handles the add button event
    @param Unused
==============================*/

void FrameVideoInsert::m_Button_Add_OnButtonClick(wxCommandEvent& event)
{
    wxString path = this->m_FilePicker_Video->GetPath();
    path.Replace(this->m_BasePath, "");
    wxString str = "<p align=\"center\">\r\n    <video width=\"50%\" controls>\r\n";
    str += "        <source src=\"" + path + "\" type=\"video/mp4\">\r\n    </video>";
    if (this->m_RichText_Caption->GetValue() != "")
    {
        str += "</br>\r\n";
        str += this->m_RichText_Caption->GetValue() + "\r\n";
    }
    else
        str += "\r\n";
    str += "</p>\r\n";
    ((wxTextCtrl*)this->m_Parent)->WriteText(str);
    this->Close();
    event.Skip();
}


/*==============================
    m_Button_Cancel_OnButtonClick
    Handles the cancel button event
    @param Unused
==============================*/

void FrameVideoInsert::m_Button_Cancel_OnButtonClick(wxCommandEvent& event)
{
    this->Close();
    event.Skip();
}


/*==============================
    SetParent
    Set the parent window
    @param The parent to set
==============================*/

void FrameVideoInsert::SetParent(wxWindow* parent, wxString path)
{
    this->m_Parent = parent;
    this->m_BasePath = path;
    this->m_FilePicker_Video->SetInitialDirectory(path);
}