/***************************************************************
                       frameauthors.cpp

Frame that's used to create authors
***************************************************************/

#include "frameauthors.h"
#include "helper.h"
#include <algorithm>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>


/*==============================
    FrameAuthors (Constructor)
    Initializes the class
==============================*/

FrameAuthors::FrameAuthors(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    // Add the main sizers
    wxFlexGridSizer* m_Sizer_Main;
    m_Sizer_Main = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_Main->AddGrowableCol(0);
    m_Sizer_Main->AddGrowableRow(1);
    m_Sizer_Main->SetFlexibleDirection(wxBOTH);
    m_Sizer_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    wxFlexGridSizer* m_Sizer_Top;
    m_Sizer_Top = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_Top->AddGrowableCol(0);
    m_Sizer_Top->SetFlexibleDirection(wxBOTH);
    m_Sizer_Top->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    // Author search box
    this->m_SearchCtrl_Authors = new wxSearchCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    #ifndef __WXMAC__
        this->m_SearchCtrl_Authors->ShowSearchButton(true);
    #endif
    this->m_SearchCtrl_Authors->ShowCancelButton(false);
    m_Sizer_Top->Add(this->m_SearchCtrl_Authors, 0, wxALL|wxEXPAND, 5);

    // Author new button
    this->m_Button_New = new wxButton(this, wxID_ANY, wxT("New Author"), wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Top->Add(this->m_Button_New, 0, wxALL, 5);
    m_Sizer_Main->Add(m_Sizer_Top, 1, wxEXPAND, 5);

    // Author choice checklist
    wxArrayString m_CheckList_AuthorsChoices;
    m_CheckList_Authors = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_CheckList_AuthorsChoices, wxLB_MULTIPLE|wxLB_SORT);
    m_Sizer_Main->Add(this->m_CheckList_Authors, 0, wxALL|wxEXPAND, 5);

    // Apply button
    this->m_Button_Ok = new wxButton(this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
    m_Sizer_Main->Add(this->m_Button_Ok, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // Finalize the layout
    this->SetSizer(m_Sizer_Main);
    this->Layout();
    this->Centre(wxBOTH);

    // Connect events
    this->m_SearchCtrl_Authors->Bind(wxEVT_TEXT, &FrameAuthors::m_SearchCtrl_Authors_OnText, this);
    this->m_CheckList_Authors->Bind(wxEVT_CHECKLISTBOX, &FrameAuthors::m_CheckList_Authors_OnBoxToggled, this);
    this->m_Button_New->Bind(wxEVT_BUTTON, &FrameAuthors::m_Button_New_OnClick, this);
    this->m_Button_Ok->Bind(wxEVT_BUTTON, &FrameAuthors::m_Button_Ok_OnClick, this);
}


/*==============================
    FrameAuthors (Destructor)
    Cleans up the class before deletion
==============================*/

FrameAuthors::~FrameAuthors()
{
    this->m_SearchCtrl_Authors->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(FrameAuthors::m_SearchCtrl_Authors_OnText), NULL, this);
    this->m_Button_New->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameAuthors::m_Button_New_OnClick), NULL, this);
    this->m_CheckList_Authors->Disconnect(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler(FrameAuthors::m_CheckList_Authors_OnBoxToggled), NULL, this);
    this->m_Button_Ok->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FrameAuthors::m_Button_Ok_OnClick), NULL, this);
}


/*==============================
    SetAuthors
    Sets the available and selected authors
    @param The list of available authors
    @param The authors selected for the ROM
==============================*/

void FrameAuthors::SetAuthors(std::vector<Author*>* allauthors, std::vector<wxString> romauthors)
{
    this->m_Authors = allauthors;
    this->m_SelectedAuthors = romauthors;
    this->RefreshAuthors();
}


/*==============================
    RefreshAuthors
    Updates the author checklist using the search filter
==============================*/

void FrameAuthors::RefreshAuthors()
{
    wxString filter = this->m_SearchCtrl_Authors->GetValue().Lower();
    this->m_CheckList_Authors->Clear();
    for (Author* author : *this->m_Authors)
    {
        int index;
        if (!author->displayname.Lower().Contains(filter))
            continue;
        index = this->m_CheckList_Authors->Append(author->displayname);
        this->m_CheckList_Authors->Check(index, std::find(this->m_SelectedAuthors.begin(), this->m_SelectedAuthors.end(), author->displayname) != this->m_SelectedAuthors.end());
    }
}


/*==============================
    GetSelectedAuthors
    Retrieves the selected authors
    @return A pointer to the list of selected authors
==============================*/

std::vector<wxString>* FrameAuthors::GetSelectedAuthors()
{
    return &this->m_SelectedAuthors;
}


/*==============================
    m_SearchCtrl_Authors_OnText
    Handles changes to the author search filter
    @param Unused
==============================*/

void FrameAuthors::m_SearchCtrl_Authors_OnText(wxCommandEvent&)
{
    this->RefreshAuthors();
}


/*==============================
    m_CheckList_Authors_OnBoxToggled
    Handles changes to the selected authors
    @param The event that was generated
==============================*/

void FrameAuthors::m_CheckList_Authors_OnBoxToggled(wxCommandEvent& event)
{
    int index = event.GetInt();
    wxString author = this->m_CheckList_Authors->GetString(index);
    auto found = std::find(this->m_SelectedAuthors.begin(), this->m_SelectedAuthors.end(), author);
    if (this->m_CheckList_Authors->IsChecked(index))
    {
        if (found == this->m_SelectedAuthors.end())
            this->m_SelectedAuthors.push_back(author);
    }
    else if (found != this->m_SelectedAuthors.end())
        this->m_SelectedAuthors.erase(found);
}


/*==============================
    m_Button_New_OnClick
    Creates an author in the dialog
    @param Unused
==============================*/

void FrameAuthors::m_Button_New_OnClick(wxCommandEvent&)
{
    Author* author;
    wxTextEntryDialog dialog(this, "Author name:", "New Author");
    if (dialog.ShowModal() != wxID_OK)
        return;
    wxString name = dialog.GetValue().Trim(true).Trim(false);
    if (name.IsEmpty())
    {
        wxMessageBox("Enter an author name.", "New Author", wxOK|wxICON_WARNING, this);
        return;
    }
    wxString filename = sanitize_tagname(name);
    if (filename.IsEmpty())
    {
        wxMessageBox("Enter a name that can be used as a filename.", "New Author", wxOK|wxICON_WARNING, this);
        return;
    }
    for (Author* a : *this->m_Authors)
    {
        if (a->displayname.CmpNoCase(name) == 0 || a->filename.CmpNoCase(filename) == 0)
        {
            wxMessageBox("This author already exists.", "New Author", wxOK|wxICON_WARNING, this);
            return;
        }
    }
    author = new Author();
    author->displayname = name;
    author->filename = filename;
    this->m_Authors->push_back(author);
    this->m_SelectedAuthors.push_back(name);
    this->m_SearchCtrl_Authors->ChangeValue(wxEmptyString);
    this->RefreshAuthors();
}


/*==============================
    m_Button_Ok_OnClick
    Applies the selected authors
    @param Unused
==============================*/

void FrameAuthors::m_Button_Ok_OnClick(wxCommandEvent& event)
{
    event.Skip();
    this->EndModal(wxID_OK);
}
