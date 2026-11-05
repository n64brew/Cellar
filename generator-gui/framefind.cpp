/***************************************************************
                         framefind.cpp

Frame for find and replace
***************************************************************/

#include "framefind.h"
#include <wx/msgdlg.h>


/*==============================
    Main (Constructor)
    Initializes the class
==============================*/

FrameFind::FrameFind(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxFlexGridSizer* m_Sizer_Main;
    m_Sizer_Main = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_Main->AddGrowableCol(0);
    m_Sizer_Main->AddGrowableRow(0);
    m_Sizer_Main->SetFlexibleDirection(wxBOTH);
    m_Sizer_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    wxFlexGridSizer* m_Sizer_Inputs;
    m_Sizer_Inputs = new wxFlexGridSizer(0, 3, 0, 0);
    m_Sizer_Inputs->AddGrowableCol(1);
    m_Sizer_Inputs->SetFlexibleDirection(wxBOTH);
    m_Sizer_Inputs->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    this->m_Label_Find = new wxStaticText(this, wxID_ANY, wxT("Find:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Find->Wrap(-1);
    m_Sizer_Inputs->Add(this->m_Label_Find, 0, wxALL, 5);

    this->m_Input_Find = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Inputs->Add(this->m_Input_Find, 0, wxALL|wxEXPAND, 5);

    this->m_Button_FindNext = new wxButton(this, wxID_ANY, wxT("Next"), wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Inputs->Add(this->m_Button_FindNext, 0, wxALL, 5);

    this->m_Label_Replace = new wxStaticText(this, wxID_ANY, wxT("Replace:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Replace->Wrap(-1);
    m_Sizer_Inputs->Add(this->m_Label_Replace, 0, wxALL, 5);

    this->m_Input_Replace = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Inputs->Add(this->m_Input_Replace, 0, wxALL|wxEXPAND, 5);

    this->m_Button_Previous = new wxButton(this, wxID_ANY, wxT("Previous"), wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Inputs->Add(this->m_Button_Previous, 0, wxALL, 5);

    m_Sizer_Main->Add(m_Sizer_Inputs, 1, wxEXPAND, 5);

    wxBoxSizer* m_Sizer_Buttons;
    m_Sizer_Buttons = new wxBoxSizer(wxHORIZONTAL);

    this->m_Button_Replace = new wxButton(this, wxID_ANY, wxT("Replace"), wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Buttons->Add(this->m_Button_Replace, 0, wxALL, 5);

    this->m_Button_ReplaceAll = new wxButton(this, wxID_ANY, wxT("Replace All"), wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Buttons->Add(this->m_Button_ReplaceAll, 0, wxALL, 5);

    m_Sizer_Buttons->Add(0, 0, 1, wxEXPAND, 5);

    this->m_Button_Close = new wxButton(this, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Buttons->Add(this->m_Button_Close, 0, wxALL, 5);

    m_Sizer_Main->Add(m_Sizer_Buttons, 1, wxEXPAND, 5);

    this->SetSizer(m_Sizer_Main);
    this->Layout();

    this->Centre(wxBOTH);

    // Connect Events
    this->m_Button_FindNext->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameFind::m_Button_FindNext_OnButtonClick), NULL, this);
    this->m_Button_Previous->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameFind::m_Button_Previous_OnButtonClick), NULL, this);
    this->m_Button_Replace->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameFind::m_Button_Replace_OnButtonClick), NULL, this);
    this->m_Button_ReplaceAll->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameFind::m_Button_ReplaceAll_OnButtonClick), NULL, this);
    this->m_Button_Close->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameFind::m_Button_Close_OnButtonClick), NULL, this);

    // Set focus on the find input
    this->m_Input_Find->SetFocus();
}


/*==============================
    App (Destructor)
    Cleans up the class before deletion
==============================*/

FrameFind::~FrameFind()
{
    this->m_Button_FindNext->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameFind::m_Button_FindNext_OnButtonClick), NULL, this);
    this->m_Button_Previous->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameFind::m_Button_Previous_OnButtonClick), NULL, this);
    this->m_Button_Replace->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameFind::m_Button_Replace_OnButtonClick), NULL, this);
    this->m_Button_ReplaceAll->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameFind::m_Button_ReplaceAll_OnButtonClick), NULL, this);
    this->m_Button_Close->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameFind::m_Button_Close_OnButtonClick), NULL, this);
}


/*==============================
    m_Button_FindNext_OnButtonClick
    Handles the find next button event
    @param Unused
==============================*/

void FrameFind::m_Button_FindNext_OnButtonClick(wxCommandEvent& event)
{
    this->FindNext(this->m_Input_Find->GetValue());
    event.Skip();
}


/*==============================
    m_Button_Previous_OnButtonClick
    Handles the find previous button event
    @param Unused
==============================*/

void FrameFind::m_Button_Previous_OnButtonClick(wxCommandEvent& event)
{
    this->FindPrevious(this->m_Input_Find->GetValue());
    event.Skip();
}


/*==============================
    m_Button_Replace_OnButtonClick
    Handles the replace button event
    @param Unused
==============================*/

void FrameFind::m_Button_Replace_OnButtonClick(wxCommandEvent& event)
{
    if (this->FindNext(this->m_Input_Find->GetValue()))
    {
        long start, end;
        wxTextCtrl* textctrl = (wxTextCtrl*)m_Parent;
        textctrl->GetSelection(&start, &end);
        textctrl->Replace(start, end, this->m_Input_Replace->GetValue());
    }
    event.Skip();
}


/*==============================
    m_Button_ReplaceAll_OnButtonClick
    Handles the replace all button event
    @param Unused
==============================*/

void FrameFind::m_Button_ReplaceAll_OnButtonClick(wxCommandEvent& event)
{
    int result = wxMessageBox("Replace all occurrences of '" + this->m_Input_Find->GetValue() + "' with '" + this->m_Input_Replace->GetValue() + "'?","Confirm", wxYES_NO | wxICON_QUESTION, this);
    if (result == wxYES)
    {
        while (this->FindNext(this->m_Input_Find->GetValue()))
        {
            long start, end;
            wxTextCtrl* textctrl = (wxTextCtrl*)m_Parent;
            textctrl->GetSelection(&start, &end);
            textctrl->Replace(start, end, this->m_Input_Replace->GetValue());
        }
    }
    event.Skip();
}


/*==============================
    m_Button_Close_OnButtonClick
    Handles the close button event
    @param Unused
==============================*/

void FrameFind::m_Button_Close_OnButtonClick(wxCommandEvent& event)
{
    this->Close();
    event.Skip();
}


/*==============================
    SetParent
    Set the parent window
    @param The parent to set
==============================*/

void FrameFind::SetParent(wxWindow* parent)
{
    this->m_Parent = parent;
}

bool FrameFind::FindNext(wxString find)
{
    size_t pos;
    long start, end;
    wxTextCtrl* textctrl = (wxTextCtrl*)m_Parent;
    wxString text = textctrl->GetValue();

    // Get the current position and find the next ocurrence
    textctrl->GetSelection(&start, &end);
    pos = text.find(find, end);

    // Wrap around
    if (pos == wxString::npos)
        pos = text.find(find);

    // Found the string
    if (pos != wxString::npos)
    {
        textctrl->SetFocus();
        textctrl->SetInsertionPoint(pos + find.Length());
        textctrl->SetSelection(pos, pos + find.Length());
        return true;
    }
    
    // Failed to find anything
    wxBell();
    return false;
}

bool FrameFind::FindPrevious(wxString find)
{
    size_t pos;
    long start, end;
    wxTextCtrl* textctrl = (wxTextCtrl*)m_Parent;
    wxString text = textctrl->GetValue();

    // Get the current position and find the previous ocurrence
    textctrl->GetSelection(&start, &end);
    if (start > 0)
        pos = text.rfind(find, start - 1);
    else
        pos = wxString::npos;

    // Wrap around
    if (pos == wxString::npos)
        pos = text.rfind(find);

    // Found the string
    if (pos != wxString::npos)
    {
        textctrl->SetFocus();
        textctrl->SetInsertionPoint(pos);
        textctrl->SetSelection(pos, pos + find.Length());
        return true;
    }
    
    // Failed to find anything
    wxBell();
    return false;
}