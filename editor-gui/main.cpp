/***************************************************************
                            main.cpp

The main code of the static site generator
***************************************************************/

#include "main.h"
#include "frameauthors.h"
#include "framecalendar.h"
#include "framefind.h"
#include "frametags.h"
#include "helper.h"
#include "json.h"
#include "include/md4c/md4c-html.h"
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/treelist.h>
#include <wx/dir.h>
#include <wx/textfile.h>
#include <wx/dirdlg.h>
#include <wx/regex.h>
#include <wx/wx.h>
#include "resource.h"


/*==============================
    Main (Constructor)
    Initializes the class
==============================*/

Main::Main(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    // Initialize the program state
    this->m_WorkingDir = wxGetCwd();
    this->m_SelectedItem = NULL;
    this->MarkModified(false);

    // Create the main frame elements
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);
    wxBoxSizer* m_Sizer_Main;
    m_Sizer_Main = new wxBoxSizer(wxVERTICAL);

    // Create the main ROM panels
    this->m_Panel_ROMs = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* m_Sizer_ROMs;
    m_Sizer_ROMs = new wxBoxSizer(wxVERTICAL);
    this->m_Splitter_ROMs = new wxSplitterWindow(this->m_Panel_ROMs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
    this->m_Splitter_ROMs->SetSashGravity(0);
    this->m_Splitter_ROMs->Connect(wxEVT_IDLE, wxIdleEventHandler(Main::m_Splitter_ROMsOnIdle), NULL, this);

    // Create the ROM tree
    this->m_Panel_ROMs_Tree = new wxPanel(this->m_Splitter_ROMs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_ROMs_Tree;
    m_Sizer_ROMs_Tree = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_ROMs_Tree->AddGrowableCol(0);
    m_Sizer_ROMs_Tree->AddGrowableRow(1);
    m_Sizer_ROMs_Tree->SetFlexibleDirection(wxBOTH);
    m_Sizer_ROMs_Tree->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    this->m_SearchCtrl_ROMs = new wxSearchCtrl(this->m_Panel_ROMs_Tree, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    #ifndef __WXMAC__
        this->m_SearchCtrl_ROMs->ShowSearchButton(true);
    #endif
    this->m_SearchCtrl_ROMs->ShowCancelButton(false);
    m_Sizer_ROMs_Tree->Add(this->m_SearchCtrl_ROMs, 0, wxALL|wxEXPAND, 5);
    this->m_TreeCtrl_ROMs = new wxTreeCtrl(this->m_Panel_ROMs_Tree, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_HIDE_ROOT);
    m_Sizer_ROMs_Tree->Add(this->m_TreeCtrl_ROMs, 0, wxALL|wxEXPAND, 5);
    this->m_Panel_ROMs_Tree->SetSizer(m_Sizer_ROMs_Tree);
    this->m_Panel_ROMs_Tree->Layout();
    m_Sizer_ROMs_Tree->Fit(this->m_Panel_ROMs_Tree);


    // ========================= ROM Page Editor Controls ========================= 

    // Create the primary ROM editor panel
    this->m_Panel_ROMs_Editor = new wxPanel(this->m_Splitter_ROMs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_ROMs_Editor;
    m_Sizer_ROMs_Editor = new wxFlexGridSizer(3, 1, 0, 0);
    m_Sizer_ROMs_Editor->AddGrowableCol(0);
    m_Sizer_ROMs_Editor->AddGrowableRow(0);
    m_Sizer_ROMs_Editor->AddGrowableRow(1);
    m_Sizer_ROMs_Editor->SetFlexibleDirection(wxBOTH);
    m_Sizer_ROMs_Editor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);
    this->m_ScrolledWindow_ROM_Editor = new wxScrolledWindow(this->m_Panel_ROMs_Editor, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
    this->m_ScrolledWindow_ROM_Editor->SetScrollRate(5, 5);
    this->m_ScrolledWindow_ROM_Editor->Hide();
    this->m_ScrolledWindow_ROM_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_Editor;
    m_Sizer_ScrolledWindow_Editor = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_ScrolledWindow_Editor->AddGrowableCol(0);
    m_Sizer_ScrolledWindow_Editor->AddGrowableRow(1);
    m_Sizer_ScrolledWindow_Editor->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_Editor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_Editor_Main;
    m_Sizer_ScrolledWindow_Editor_Main = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ScrolledWindow_Editor_Main->AddGrowableCol(1);
    m_Sizer_ScrolledWindow_Editor_Main->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_Editor_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_Editor_Main->SetMinSize(wxSize(0,0));

    //File
    wxStaticText* m_Label_ROMs_File = new wxStaticText(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxT("File:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMs_File->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Label_ROMs_File, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_TextCtrl_ROMs_File = new wxTextCtrl(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_ROMs_File->SetToolTip(wxT("The filename of the ROM"));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_ROMs_File, 0, wxALL|wxEXPAND, 5);

    // Display name
    wxStaticText* m_Label_ROMs_Name = new wxStaticText(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxT("Display Name:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMs_Name->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Label_ROMs_Name, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_TextCtrl_ROMs_Name = new wxTextCtrl(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    this->m_TextCtrl_ROMs_Name->SetToolTip(wxT("The display name of the ROM"));
    #if wxUSE_SPELLCHECK
        this->m_TextCtrl_ROMs_Name->EnableProofCheck();
    #endif
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_ROMs_Name, 0, wxALL|wxEXPAND, 5);

    // Subtitle
    wxStaticText* m_Label_ROMs_ToolTip = new wxStaticText(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxT("Subtitle:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMs_ToolTip->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Label_ROMs_ToolTip, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_TextCtrl_ROMs_ToolTip = new wxTextCtrl(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    this->m_TextCtrl_ROMs_ToolTip->SetToolTip(wxT("Brief description that shows up when hovering over a ROM with the mouse."));
    #if wxUSE_SPELLCHECK
        this->m_TextCtrl_ROMs_ToolTip->EnableProofCheck();
    #endif
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_ROMs_ToolTip, 0, wxALL|wxEXPAND, 5);

    // Authors
    wxStaticText* m_Label_ROMs_Authors = new wxStaticText(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxT("Authors:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMs_Authors->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Label_ROMs_Authors, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxFlexGridSizer* m_Sizer_ROMs_Authors;
    m_Sizer_ROMs_Authors = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ROMs_Authors->AddGrowableCol(1);
    m_Sizer_ROMs_Authors->SetFlexibleDirection(wxBOTH);
    m_Sizer_ROMs_Authors->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    this->m_Button_ROMs_FindAuthors = new wxBitmapButton(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, icon_authors, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    m_Sizer_ROMs_Authors->Add(this->m_Button_ROMs_FindAuthors, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_TextCtrl_ROMs_Authors = new wxTextCtrl(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_ROMs_Authors->SetToolTip(wxT("The author(s) of the ROM, comma separated"));
    this->m_TextCtrl_ROMs_Authors->Enable(false);
    m_Sizer_ROMs_Authors->Add(this->m_TextCtrl_ROMs_Authors, 0, wxALL|wxEXPAND, 5);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Sizer_ROMs_Authors, 1, wxEXPAND, 5);

    // Date
    wxStaticText* m_Label_ROMs_Date;
    m_Label_ROMs_Date = new wxStaticText(m_ScrolledWindow_ROM_Editor, wxID_ANY, wxT("Date:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMs_Date->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Label_ROMs_Date, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);
    wxFlexGridSizer* m_Sizer_ROMs_Date;
    m_Sizer_ROMs_Date = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ROMs_Date->AddGrowableCol(1);
    m_Sizer_ROMs_Date->SetFlexibleDirection(wxBOTH);
    m_Sizer_ROMs_Date->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    this->m_Button_ROMs_Date = new wxBitmapButton(m_ScrolledWindow_ROM_Editor, wxID_ANY, icon_calendar, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    m_Sizer_ROMs_Date->Add(this->m_Button_ROMs_Date, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_TextCtrl_ROMs_Date = new wxTextCtrl(m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_ROMs_Date->SetToolTip(wxT("The date of this ROM's release."));
    this->m_TextCtrl_ROMs_Date->Enable(false);
    m_Sizer_ROMs_Date->Add(this->m_TextCtrl_ROMs_Date, 0, wxALL|wxEXPAND, 5);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Sizer_ROMs_Date, 1, wxEXPAND, 5);

    // Tags
    wxStaticText* m_Label_ROMs_Tags = new wxStaticText(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxT("Tags:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMs_Tags->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Label_ROMs_Tags, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxFlexGridSizer* m_Sizer_ROMs_Tags;
    m_Sizer_ROMs_Tags = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ROMs_Tags->AddGrowableCol(1);
    m_Sizer_ROMs_Tags->SetFlexibleDirection(wxBOTH);
    m_Sizer_ROMs_Tags->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    this->m_Button_ROMs_Tags = new wxBitmapButton(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, icon_tags, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    m_Sizer_ROMs_Tags->Add(this->m_Button_ROMs_Tags, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_TextCtrl_ROMs_Tags = new wxTextCtrl(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    this->m_TextCtrl_ROMs_Tags->Enable(false);
    this->m_TextCtrl_ROMs_Tags->SetToolTip(wxT("ROM tags, comma separated."));
    m_Sizer_ROMs_Tags->Add(this->m_TextCtrl_ROMs_Tags, 0, wxALL|wxEXPAND, 5);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Sizer_ROMs_Tags, 1, wxEXPAND, 5);

    // Icon
    wxStaticText* m_Label_ROMs_Icon = new wxStaticText(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxT("Icon:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMs_Icon->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Label_ROMs_Icon, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_TextCtrl_ROMs_Icon = new wxTextCtrl(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_ROMs_Icon->SetToolTip(wxT("The icon used to display the rom in the list of ROMs. Requires a relative path (to the working directory) to a 240x125 png."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_ROMs_Icon, 0, wxALL|wxEXPAND, 5);

    // Images
    wxStaticText* m_Label_ROMs_Images = new wxStaticText(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxT("Images:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMs_Images->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Label_ROMs_Images, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_TextCtrl_ROMs_Images = new wxTextCtrl(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_ROMs_Images->SetToolTip(wxT("Images for this ROM, comma separated. Paths should be relative to the working directory."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_ROMs_Images, 0, wxALL|wxEXPAND, 5);

    // URLs
    wxStaticText* m_Label_ROMs_URLs = new wxStaticText(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxT("URLs:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMs_URLs->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Label_ROMs_URLs, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_TextCtrl_ROMs_URLs = new wxTextCtrl(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_ROMs_URLs->SetToolTip(wxT("URLs for this ROM, comma separated."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_ROMs_URLs, 0, wxALL|wxEXPAND, 5);
    m_Sizer_ScrolledWindow_Editor->Add(m_Sizer_ScrolledWindow_Editor_Main, 1, wxEXPAND, 5);

    // Genres

    // Age Rating
    wxStaticText* m_Label_ROMs_AgeRating;
    m_Label_ROMs_AgeRating = new wxStaticText(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxT("Age Rating:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMs_AgeRating->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Label_ROMs_AgeRating, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxFlexGridSizer* m_Sizer_ROMs_ContentWarnings;
    m_Sizer_ROMs_ContentWarnings = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ROMs_ContentWarnings->AddGrowableCol(1);
    m_Sizer_ROMs_ContentWarnings->SetFlexibleDirection(wxBOTH);
    m_Sizer_ROMs_ContentWarnings->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    this->m_ComboBox_ROMs_AgeRating = new wxComboBox(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY);
    this->m_ComboBox_ROMs_AgeRating->Append(wxT("All"));
    this->m_ComboBox_ROMs_AgeRating->Append(wxT("Teens"));
    this->m_ComboBox_ROMs_AgeRating->Append(wxT("Mature"));
    this->m_ComboBox_ROMs_AgeRating->Append(wxT("Adult"));
    m_Sizer_ROMs_ContentWarnings->Add(this->m_ComboBox_ROMs_AgeRating, 0, wxALL, 5);
    this->m_TextCtrl_ROMs_ContentWarning = new wxTextCtrl(m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_ROMs_ContentWarning->SetToolTip(wxT("Potential content warnings"));
    m_Sizer_ROMs_ContentWarnings->Add(this->m_TextCtrl_ROMs_ContentWarning, 0, wxALL|wxEXPAND, 5);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Sizer_ROMs_ContentWarnings, 1, wxEXPAND, 5);

    // AI Disclosure
    wxStaticText* m_Label_ROMs_AIDisclosure;
    m_Label_ROMs_AIDisclosure = new wxStaticText(m_ScrolledWindow_ROM_Editor, wxID_ANY, wxT("AI Disclosure:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMs_AIDisclosure->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(m_Label_ROMs_AIDisclosure, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_TextCtrl_ROMs_AIDisclosure = new wxTextCtrl(m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_ROMs_AIDisclosure->SetToolTip(wxT("AI use disclosure (if applicable)"));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_ROMs_AIDisclosure, 0, wxALL|wxEXPAND, 5);

    // Description
    this->m_TextCtrl_ROMs_Description = new wxTextCtrl(this->m_ScrolledWindow_ROM_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1,-1), wxTE_MULTILINE | wxTE_RICH2);
    this->m_TextCtrl_ROMs_Description->SetToolTip(wxT("The ROM description. Supports markdown."));
    this->m_TextCtrl_ROMs_Description->SetMinSize(wxSize(-1,200));
    #if wxUSE_SPELLCHECK
        this->m_TextCtrl_ROMs_Description->EnableProofCheck();
    #endif
    m_Sizer_ScrolledWindow_Editor->Add(this->m_TextCtrl_ROMs_Description, 0, wxALL|wxEXPAND, 5);
    this->m_ScrolledWindow_ROM_Editor->SetSizer(m_Sizer_ScrolledWindow_Editor);
    this->m_ScrolledWindow_ROM_Editor->Layout();
    m_Sizer_ScrolledWindow_Editor->Fit(this->m_ScrolledWindow_ROM_Editor);
    m_Sizer_ROMs_Editor->Add(this->m_ScrolledWindow_ROM_Editor, 0, wxALL|wxEXPAND, 5);


    // ========================= ROM Category Editor ========================= 

    // Create the ROM category editor panel
    this->m_ScrolledWindow_ROMCategory_Editor = new wxScrolledWindow(this->m_Panel_ROMs_Editor, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
    this->m_ScrolledWindow_ROMCategory_Editor->SetScrollRate(5, 5);
    this->m_ScrolledWindow_ROMCategory_Editor->Hide();
    this->m_ScrolledWindow_ROMCategory_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_ROMCategory_Editor;
    m_Sizer_ScrolledWindow_ROMCategory_Editor = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_ScrolledWindow_ROMCategory_Editor->AddGrowableCol(0);
    m_Sizer_ScrolledWindow_ROMCategory_Editor->AddGrowableRow(1);
    m_Sizer_ScrolledWindow_ROMCategory_Editor->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_ROMCategory_Editor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_ROMCategory_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_ROMCategory_Editor_Main;
    m_Sizer_ScrolledWindow_ROMCategory_Editor_Main = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ScrolledWindow_ROMCategory_Editor_Main->AddGrowableCol(1);
    m_Sizer_ScrolledWindow_ROMCategory_Editor_Main->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_ROMCategory_Editor_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_ROMCategory_Editor_Main->SetMinSize(wxSize(0,0));

    // Folder
    wxStaticText* m_Label_ROMsCategory_Folder = new wxStaticText(this->m_ScrolledWindow_ROMCategory_Editor, wxID_ANY, wxT("Folder:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMsCategory_Folder->Wrap(-1);
    m_Sizer_ScrolledWindow_ROMCategory_Editor_Main->Add(m_Label_ROMsCategory_Folder, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_TextCtrl_ROMsCategory_Folder = new wxTextCtrl(this->m_ScrolledWindow_ROMCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_ROMsCategory_Folder->SetToolTip(wxT("The folder that represents this category"));
    m_Sizer_ScrolledWindow_ROMCategory_Editor_Main->Add(this->m_TextCtrl_ROMsCategory_Folder, 0, wxALL|wxEXPAND, 5);

    // Display name
    wxStaticText* m_Label_ROMsCategory_DisplayName = new wxStaticText(this->m_ScrolledWindow_ROMCategory_Editor, wxID_ANY, wxT("Display Name:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMsCategory_DisplayName->Wrap(-1);
    m_Sizer_ScrolledWindow_ROMCategory_Editor_Main->Add(m_Label_ROMsCategory_DisplayName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    this->m_TextCtrl_ROMsCategory_DisplayName = new wxTextCtrl(this->m_ScrolledWindow_ROMCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    this->m_TextCtrl_ROMsCategory_DisplayName->SetToolTip(wxT("The display name of this category"));
    #if wxUSE_SPELLCHECK
        this->m_TextCtrl_ROMsCategory_DisplayName->EnableProofCheck();
    #endif
    m_Sizer_ScrolledWindow_ROMCategory_Editor_Main->Add(this->m_TextCtrl_ROMsCategory_DisplayName, 0, wxALL|wxEXPAND, 5);

    // Description
    wxStaticText* m_Label_ROMsCategory_Description = new wxStaticText(this->m_ScrolledWindow_ROMCategory_Editor, wxID_ANY, wxT("Description:"), wxDefaultPosition, wxDefaultSize, 0);
    m_Label_ROMsCategory_Description->Wrap(-1);
    m_Sizer_ScrolledWindow_ROMCategory_Editor_Main->Add(m_Label_ROMsCategory_Description, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    m_Sizer_ScrolledWindow_ROMCategory_Editor_Main->Add(0, 0, 1, wxEXPAND, 5);
    m_Sizer_ScrolledWindow_ROMCategory_Editor->Add(m_Sizer_ScrolledWindow_ROMCategory_Editor_Main, 1, wxEXPAND, 5);
    this->m_TextCtrl_ROMsCategory_Description = new wxTextCtrl(this->m_ScrolledWindow_ROMCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH2);
    this->m_TextCtrl_ROMsCategory_Description->SetToolTip(wxT("The filename of the ROM"));
    this->m_TextCtrl_ROMsCategory_Description->SetMinSize(wxSize(-1,200));
    #if wxUSE_SPELLCHECK
        this->m_TextCtrl_ROMsCategory_Description->EnableProofCheck();
    #endif
    m_Sizer_ScrolledWindow_ROMCategory_Editor->Add(this->m_TextCtrl_ROMsCategory_Description, 0, wxALL|wxEXPAND, 5);

    // Layout contents
    this->m_ScrolledWindow_ROMCategory_Editor->SetSizer(m_Sizer_ScrolledWindow_ROMCategory_Editor);
    this->m_ScrolledWindow_ROMCategory_Editor->Layout();
    m_Sizer_ScrolledWindow_ROMCategory_Editor->Fit(this->m_ScrolledWindow_ROMCategory_Editor);
    m_Sizer_ROMs_Editor->Add(this->m_ScrolledWindow_ROMCategory_Editor, 0, wxEXPAND | wxALL, 5);

    // ================================================== 


    // Preview button
    this->m_Button_ROMs_Preview = new wxButton(this->m_Panel_ROMs_Editor, wxID_ANY, wxT("Preview"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Button_ROMs_Preview->Hide();
    this->m_Button_ROMs_Preview->SetMinSize(wxSize(-1,24));
    m_Sizer_ROMs_Editor->Add(this->m_Button_ROMs_Preview, 0, wxALIGN_RIGHT|wxALL, 5);

    // Add everything to the ROMs panel
    this->m_Panel_ROMs_Editor->SetSizer(m_Sizer_ROMs_Editor);
    this->m_Panel_ROMs_Editor->Layout();
    m_Sizer_ROMs_Editor->Fit(this->m_Panel_ROMs_Editor);
    this->m_Splitter_ROMs->SplitVertically(this->m_Panel_ROMs_Tree, this->m_Panel_ROMs_Editor, 200);
    m_Sizer_ROMs->Add(this->m_Splitter_ROMs, 1, wxEXPAND, 5);
    this->m_Panel_ROMs->SetSizer(m_Sizer_ROMs);
    this->m_Panel_ROMs->Layout();
    m_Sizer_ROMs->Fit(this->m_Panel_ROMs);

    // Finalize
    m_Sizer_Main->Add(m_Panel_ROMs, 1, wxEXPAND | wxALL, 5);
    this->SetSizer(m_Sizer_Main);
    this->Layout();

    // Add the menubar
    this->m_Menubar_Main = new wxMenuBar(0);
    this->m_Menu_File = new wxMenu();
    wxMenuItem* m_MenuItem_Save;
    m_MenuItem_Save = new wxMenuItem(this->m_Menu_File, wxID_ANY, wxString(wxT("Save Changes")) + wxT('\t') + wxT("CTRL+S"), wxEmptyString, wxITEM_NORMAL);
    this->m_Menu_File->Append(m_MenuItem_Save);
    this->m_Menu_File->AppendSeparator();
    wxMenuItem* m_MenuItem_FullRecompile;
    m_MenuItem_FullRecompile = new wxMenuItem(this->m_Menu_File, wxID_ANY, wxString(wxT("Full Website Recompile")) + wxT('\t') + wxT("CTRL+R"), wxEmptyString, wxITEM_NORMAL);
    this->m_Menu_File->Append(m_MenuItem_FullRecompile);
    this->m_Menubar_Main->Append(this->m_Menu_File, wxT("File"));
    this->m_Menu_Edit = new wxMenu();
    wxMenuItem* m_MenuItem_Find;
    m_MenuItem_Find = new wxMenuItem(this->m_Menu_Edit, wxID_ANY, wxString(wxT("Find")) + wxT('\t') + wxT("CTRL+F"), wxEmptyString, wxITEM_NORMAL);
    this->m_Menu_Edit->Append(m_MenuItem_Find);
    this->m_Menu_Edit->AppendSeparator();
    this->m_Menubar_Main->Append(this->m_Menu_Edit, wxT("Edit"));
    this->SetMenuBar(this->m_Menubar_Main);

    // Start a timer to fix the splitter location
    this->m_Timer = new wxTimer();
    this->m_Timer->Start(100, wxTIMER_ONE_SHOT);

    // Center the final window
    this->Centre(wxBOTH);

    // Connect Events
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(Main::OnClose));
    this->m_SearchCtrl_ROMs->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_SearchCtrl_ROMsOnText), NULL, this);
    this->m_TreeCtrl_ROMs->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(Main::m_TreeCtrl_ROMs_OnTreeRightClick), NULL, this);
    this->m_TreeCtrl_ROMs->Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_ROMs_OnTreeBeginDrag), NULL, this);
    this->m_TreeCtrl_ROMs->Connect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_ROMs_OnTreeEndDrag), NULL, this);
    this->m_TreeCtrl_ROMs->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(Main::m_TreeCtrl_ROMs_OnTreeEndLabelEdit), NULL, this);
    this->m_TreeCtrl_ROMs->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(Main::m_TreeCtrl_ROMs_OnTreeItemMenu), NULL, this);
    this->m_TreeCtrl_ROMs->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(Main::m_TreeCtrl_ROMs_OnTreeSelChanged), NULL, this);
    this->m_TextCtrl_ROMs_File->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_File_OnText), NULL, this);
    this->m_TextCtrl_ROMs_Name->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_Name_OnText), NULL, this);
    this->m_TextCtrl_ROMs_Icon->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_Icon_OnText), NULL, this);
    this->m_Button_ROMs_FindAuthors->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_ROMs_FindAuthors_OnButton), NULL, this);
    this->m_Button_ROMs_Tags->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_ROMs_Tags_OnButton), NULL, this);
    this->m_TextCtrl_ROMs_Images->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_Images_OnText), NULL, this);
    this->m_Button_ROMs_Date->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_ROMs_Date_OnButton), NULL, this);
    this->m_TextCtrl_ROMs_URLs->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_URLs_OnText), NULL, this);
    this->m_ComboBox_ROMs_AgeRating->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(Main::m_ComboBox_ROMs_AgeRating_OnCombobox), NULL, this);
    this->m_TextCtrl_ROMs_ContentWarning->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_ContentWarning_OnText), NULL, this);
    this->m_TextCtrl_ROMs_AIDisclosure->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_AIDisclosure_OnText), NULL, this);
    this->m_TextCtrl_ROMs_ToolTip->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_ToolTip_OnText), NULL, this);
    this->m_TextCtrl_ROMs_Description->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_Description_OnText), NULL, this);
    this->m_TextCtrl_ROMsCategory_Folder->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMsCategory_Folder_OnText), NULL, this);
    this->m_TextCtrl_ROMsCategory_DisplayName->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMsCategory_DisplayName_OnText), NULL, this);
    this->m_TextCtrl_ROMsCategory_Description->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMsCategory_Description_OnText), NULL, this);
    this->m_Button_ROMs_Preview->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_ROMs_Preview_OnButtonClick), NULL, this);
    this->m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Save_OnMenuSelection), this, m_MenuItem_Save->GetId());
    this->m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_FullRecompile_OnMenuSelection), this, m_MenuItem_FullRecompile->GetId());
    this->m_Menu_Edit->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Find_OnMenuSelection), this, m_MenuItem_Find->GetId());
    this->m_Timer->Bind(wxEVT_TIMER, wxTimerEventHandler(Main::m_Timer_OnTimer), this, this->m_Timer->GetId());

    // Check if we're set up properly
    this->InitialCheck();

    // Load the ROMs from the current working directory
    this->UpdateTree(this->m_TreeCtrl_ROMs, "roms", &this->m_Category_ROMs);
    this->LoadROMs();
    this->m_TreeCtrl_ROMs->SetFocus();
}


/*==============================
    App (Destructor)
    Cleans up the class before deletion
==============================*/

Main::~Main()
{
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(Main::OnClose));
    this->m_SearchCtrl_ROMs->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_SearchCtrl_ROMsOnText), NULL, this);
    this->m_TreeCtrl_ROMs->Disconnect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(Main::m_TreeCtrl_ROMs_OnTreeRightClick), NULL, this);
    this->m_TreeCtrl_ROMs->Disconnect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_ROMs_OnTreeBeginDrag), NULL, this);
    this->m_TreeCtrl_ROMs->Disconnect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_ROMs_OnTreeEndDrag), NULL, this);
    this->m_TreeCtrl_ROMs->Disconnect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(Main::m_TreeCtrl_ROMs_OnTreeEndLabelEdit), NULL, this);
    this->m_TreeCtrl_ROMs->Disconnect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(Main::m_TreeCtrl_ROMs_OnTreeItemMenu), NULL, this);
    this->m_TreeCtrl_ROMs->Disconnect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(Main::m_TreeCtrl_ROMs_OnTreeSelChanged), NULL, this);
    this->m_TextCtrl_ROMs_File->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_File_OnText), NULL, this);
    this->m_TextCtrl_ROMs_Name->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_Name_OnText), NULL, this);
    this->m_TextCtrl_ROMs_ToolTip->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_ToolTip_OnText), NULL, this);
    this->m_TextCtrl_ROMs_Icon->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_Icon_OnText), NULL, this);
    this->m_Button_ROMs_FindAuthors->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_ROMs_FindAuthors_OnButton), NULL, this);
    this->m_Button_ROMs_Tags->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_ROMs_Tags_OnButton), NULL, this);
    this->m_TextCtrl_ROMs_Images->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_Images_OnText), NULL, this);
    this->m_Button_ROMs_Date->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_ROMs_Date_OnButton), NULL, this);
    this->m_TextCtrl_ROMs_URLs->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_URLs_OnText), NULL, this);
    //this->m_TextCtrl_ROMs_Genres->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_Genres_OnText), NULL, this);
    this->m_ComboBox_ROMs_AgeRating->Disconnect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(Main::m_ComboBox_ROMs_AgeRating_OnCombobox), NULL, this);
    this->m_TextCtrl_ROMs_ContentWarning->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_ContentWarning_OnText), NULL, this);
    this->m_TextCtrl_ROMs_AIDisclosure->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_AIDisclosure_OnText), NULL, this);
    this->m_TextCtrl_ROMs_Description->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMs_Description_OnText), NULL, this);
    this->m_TextCtrl_ROMsCategory_Folder->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMsCategory_Folder_OnText), NULL, this);
    this->m_TextCtrl_ROMsCategory_DisplayName->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMsCategory_DisplayName_OnText), NULL, this);
    this->m_TextCtrl_ROMsCategory_Description->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ROMsCategory_Description_OnText), NULL, this);
    this->m_Button_ROMs_Preview->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_ROMs_Preview_OnButtonClick), NULL, this);
    this->m_Timer->Disconnect(wxEVT_TIMER, wxTimerEventHandler(Main::m_Timer_OnTimer), NULL, this);
}


/*==============================
    m_Splitter_ROMsOnIdle
    Handles the rom splitter event
    @param Unused
==============================*/

void Main::m_Splitter_ROMsOnIdle(wxIdleEvent&)
{
    this->m_Splitter_ROMs->SetSashPosition(0);
    this->m_Splitter_ROMs->Disconnect(wxEVT_IDLE, wxIdleEventHandler(Main::m_Splitter_ROMsOnIdle), NULL, this);
}


/*==============================
    m_MenuItem_Save_OnMenuSelection
    Handles the Save menu event
    @param Unused
==============================*/

void Main::m_MenuItem_Save_OnMenuSelection(wxCommandEvent&)
{
    this->Save();
}


/*==============================
    m_MenuItem_Find_OnMenuSelection
    Handles the Find menu event
    @param Unused
==============================*/

void Main::m_MenuItem_Find_OnMenuSelection(wxCommandEvent& event)
{
    FrameFind* frame = new FrameFind(NULL);
    frame->SetParent(this->m_TextCtrl_ROMs_Description);
    frame->Show();
    event.Skip();
}


/*==============================
    m_MenuItem_FullRecompile_OnMenuSelection
    Handles the Full Recompile menu event
    @param Unused
==============================*/

void Main::m_MenuItem_FullRecompile_OnMenuSelection(wxCommandEvent&)
{
    this->Save();
    long ret = wxExecute("./cellar-generator roms", wxEXEC_SYNC);
    if (ret == 0)
        wxMessageBox("Website recompiled successfully!", wxString("Great success"), wxICON_INFORMATION);
    else
        wxMessageBox(wxString::Format("Generator returned error code %d", ret), wxString("Generator error"), wxICON_ERROR);
}


/*==============================
    m_SearchCtrl_ROMsOnText
    Handles inputting text into the search bar
    This is an inelegant, inefficient bodge since wxWidgets 
    does not provide a way to hide tree children.
    @param The event that was generated
==============================*/

void Main::m_SearchCtrl_ROMsOnText(wxCommandEvent& event)
{
    wxString filter = this->m_SearchCtrl_ROMs->GetValue();
    this->UpdateTree(this->m_TreeCtrl_ROMs, "roms", &this->m_Category_ROMs);
    this->LoadROMs();
    if (!filter.IsEmpty())
    {
        filter_tree(this->m_TreeCtrl_ROMs, filter);
        this->m_TreeCtrl_ROMs->ExpandAll();
    }
    event.Skip();
}


/*==============================
    m_TreeCtrl_ROMs_OnTreeEndLabelEdit
    Handles the rom tree label editing event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_ROMs_OnTreeEndLabelEdit(wxTreeEvent& event)
{
    // Prevent blank display names
    if (event.GetLabel() == "")
    {
        event.Veto();
        return;
    }

    // Handle label editing based on whether it's a category or not
    if (treeitem_iscategory(this->m_TreeCtrl_ROMs, event.GetItem())) // Handle category label editing
    {
        wxTreeItemId id = event.GetItem();
        Category* cat = this->FindCategory_ROMs(id);
        if (cat != NULL)
        {
            cat->displayname = event.GetLabel();
            if (this->m_SelectedItem == event.GetItem())
                this->m_TextCtrl_ROMsCategory_DisplayName->ChangeValue(cat->displayname);
            cat->wasmodified = true;
            this->MarkModified();
            return;
        }
    }
    else // Handle rom label editing
    {
        ROM* rom = this->FindROM(event.GetItem());
        if (rom != NULL)
        {
            rom->displayname = event.GetLabel();
            if (this->m_SelectedItem == event.GetItem())
                this->m_TextCtrl_ROMs_Name->ChangeValue(rom->displayname);
            rom->category->wasmodified = true;
            rom->wasmodified = true;
            this->MarkModified();
            return;
        }
    }
    event.Veto();
}


/*==============================
    m_TreeCtrl_ROMs_OnTreeRightClick
    Handles the rom tree right click event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_ROMs_OnTreeRightClick(wxMouseEvent& event)
{
    if (this->m_SelectedItem == NULL)
    {
        wxMenu menu;
        menu.Append(wxID_NEW, wxT("Create category"));
        menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::OnPopupClick_Categories), NULL, this);
        PopupMenu(&menu, event.GetPosition());
    }
    event.Skip();
}


/*==============================
    m_TreeCtrl_ROMs_OnTreeBeginDrag
    Handles the rom tree drag event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_ROMs_OnTreeBeginDrag(wxTreeEvent& event)
{
    this->m_DraggedItem = event.GetItem();
    event.Allow();
}


/*==============================
    m_TreeCtrl_ROMs_OnTreeEndDrag
    Handles the rom tree drag ending event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_ROMs_OnTreeEndDrag(wxTreeEvent& event)
{
    if (!treeitem_iscategory(this->m_TreeCtrl_ROMs, this->m_SelectedItem))
        this->EndDrag_ROM(event);
    else
        this->EndDrag(event, this->m_TreeCtrl_ROMs, &this->m_Category_ROMs);
}


/*==============================
    m_TreeCtrl_ROMs_OnTreeItemMenu
    Handles the rom tree right click event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_ROMs_OnTreeItemMenu(wxTreeEvent& event)
{
    wxMenu menu;
    this->m_SelectedItem = event.GetItem();
    menu.Append(wxID_NEW, wxT("Create rom"));
    if (!treeitem_iscategory(this->m_TreeCtrl_ROMs, event.GetItem()))
        menu.Append(wxID_DELETE, wxT("Delete rom"));
    menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::OnPopupClick_ROMs), NULL, this);
    PopupMenu(&menu, event.GetPoint());
}


/*==============================
    m_TreeCtrl_ROMs_OnTreeSelChanged
    Handles the rom tree selection event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_ROMs_OnTreeSelChanged(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();

    // Handle the item selection
    this->m_SelectedItem = item;
    if (!treeitem_iscategory(this->m_TreeCtrl_ROMs, item)) // Handle rom selection
    {
        wxString wip;
        ROM* rom = FindROM(item);

        // Ensure the rom exists
        if (rom == NULL)
            return;

        // Fill in the simple text controls
        this->m_TextCtrl_ROMs_File->ChangeValue(rom->filename);
        this->m_TextCtrl_ROMs_Name->ChangeValue(rom->displayname);
        this->m_TextCtrl_ROMs_Icon->ChangeValue(rom->icon);
        this->m_TextCtrl_ROMs_ToolTip->ChangeValue(rom->tooltip);
        this->m_ComboBox_ROMs_AgeRating->ChangeValue(rom->agerating);
        this->m_TextCtrl_ROMs_ContentWarning->ChangeValue(rom->contentwarning);
        this->m_TextCtrl_ROMs_AIDisclosure->ChangeValue(rom->aidisclosure);
        this->m_TextCtrl_ROMs_Description->ChangeValue(rom->description);
        if (rom->date != 0)
        {
            wxDateTime utc;
            utc.Set(rom->date);
            this->m_TextCtrl_ROMs_Date->ChangeValue(utc.Format("%B %d, %Y", wxDateTime::UTC));
        }
        else
            this->m_TextCtrl_ROMs_Date->ChangeValue("");

        // Fill in the tags text control
        wip = wxString("");
        for (wxString str : rom->tags)
            wip += str + wxString(", ");
        this->m_TextCtrl_ROMs_Tags->ChangeValue(wip);
        
        // Fill in the images text control
        wip = wxString("");
        for (wxString str : rom->images)
            wip += str + wxString(", ");
        this->m_TextCtrl_ROMs_Images->ChangeValue(wip);
        
        // Fill in the url text control
        wip = wxString("");
        for (wxString str : rom->urls)
            wip += str + wxString(", ");
        this->m_TextCtrl_ROMs_URLs->ChangeValue(wip);

        // Show the rom editor
        this->ShowROMEditor();
    }
    else if (treeitem_iscategory(this->m_TreeCtrl_ROMs, item)) // Handle category selection
    {
        Category* cat = this->FindCategory_ROMs(item);

        // Ensure the category exists
        if (cat == NULL)
            return;

        // Fill in the simple text controls
        this->m_TextCtrl_ROMsCategory_Folder->ChangeValue(cat->foldername);
        this->m_TextCtrl_ROMsCategory_DisplayName->ChangeValue(cat->displayname);
        this->m_TextCtrl_ROMsCategory_Description->ChangeValue(cat->description);

        // Show the rom category editor
        this->ShowROMCategoryEditor(true);
    }
    else
        this->ShowROMEditor(false);
}


/*==============================
    m_TextCtrl_ROMs_File_OnText
    Handles the rom filename text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ROMs_File_OnText(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom == NULL)
        return;
    rom->filename = event.GetString();
    rom->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_ROMs_Name_OnText
    Handles the rom name text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ROMs_Name_OnText(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom == NULL)
        return;
    rom->displayname = event.GetString();
    this->m_TreeCtrl_ROMs->SetItemText(rom->treeid, rom->displayname);
    rom->wasmodified = true;
    rom->category->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_ROMs_Icon_OnText
    Handles the rom icon text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ROMs_Icon_OnText(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom == NULL)
        return;
    rom->icon = event.GetString();
    rom->wasmodified = true;
    rom->category->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_ComboBox_ROMs_AgeRating_OnCombobox
    Handles the rom age rating combo box change event
    @param The event that was generated
==============================*/

void Main::m_ComboBox_ROMs_AgeRating_OnCombobox(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom == NULL)
        return;
    rom->agerating = event.GetString();
    rom->wasmodified = true;
    this->MarkModified();
    event.Skip();
}


/*==============================
    m_TextCtrl_ROMs_ContentWarning_OnText
    Handles the rom content warning text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ROMs_ContentWarning_OnText(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom == NULL)
        return;
    rom->contentwarning = event.GetString();
    rom->wasmodified = true;
    this->MarkModified();
    event.Skip();
}


/*==============================
    m_TextCtrl_ROMs_AIDisclosure_OnText
    Handles the rom ai disclosure text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ROMs_AIDisclosure_OnText(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom == NULL)
        return;
    rom->aidisclosure = event.GetString();
    rom->wasmodified = true;
    this->MarkModified();
    event.Skip();
}


/*==============================
    m_Button_ROMs_FindAuthors_OnButton
    Handles the ROM authors button click event
    @param The event that was generated
==============================*/

void Main::m_Button_ROMs_FindAuthors_OnButton(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom != NULL)
    {
        FrameAuthors* frame = new FrameAuthors(this);
        frame->ShowModal();
    }
    event.Skip();
}


/*==============================
    UpdateROMTags
    Updates the tags of the currently selected ROM
    @param The string with comma separated tags
==============================*/

void Main::UpdateROMTags(wxString tags)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom != NULL)
    {
        wxString old = this->m_TextCtrl_ROMs_Tags->GetValue();
        if (old != tags)
        {
            this->m_TextCtrl_ROMs_Tags->SetValue(tags);

            wxArrayString strarray = wxSplit(tags, ',');
            rom->tags.clear();

            // Split the elements by comma
            for (wxString str : strarray)
            {
                str.Trim(true);
                str.Trim(false);
                if (str != "")
                    rom->tags.push_back(str);
            }
            rom->wasmodified = true;
            this->MarkModified();
        }
    }
}


/*==============================
    m_Button_ROMs_Tags_OnButton
    Handles the ROM tags button click event
    @param The event that was generated
==============================*/

void Main::m_Button_ROMs_Tags_OnButton(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom != NULL)
    {
        FrameTags* frame = new FrameTags(this);
        frame->ActivateTags(this->m_TextCtrl_ROMs_Tags->GetValue());
        frame->ShowModal();
    }
    event.Skip();
}


/*==============================
    m_Button_ROMs_Date_OnButton
    Handles the ROM date button click event
    @param The event that was generated
==============================*/

void Main::m_Button_ROMs_Date_OnButton(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom != NULL)
    {
        FrameCalendar* frame = new FrameCalendar(this);
        frame->SetDate(rom->date);
        frame->ShowModal();
    }
    event.Skip();
}


/*==============================
    UpdateROMDate
    Updates the ROM date into a nice readable string
    @param The timestamp
==============================*/

void Main::UpdateROMDate(time_t date)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom != NULL && rom->date != date)
    {
        wxDateTime utc(date);
        this->m_TextCtrl_ROMs_Date->ChangeValue(utc.Format("%B %d, %Y", wxDateTime::UTC));
        rom->date = date;
        rom->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    m_TextCtrl_ROMs_Images_OnText
    Handles the rom images text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ROMs_Images_OnText(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom != NULL)
    {
        wxArrayString strarray = wxSplit(event.GetString(), ',');
        rom->images.clear();

        // Split the elements by comma
        for (wxString str : strarray)
        {
            str.Trim(true);
            str.Trim(false);
            if (str != "")
                rom->images.push_back(str);
        }
        rom->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    m_TextCtrl_ROMs_URLs_OnText
    Handles the rom date text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ROMs_URLs_OnText(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom != NULL)
    {
        wxArrayString strarray = wxSplit(event.GetString(), ',');
        rom->urls.clear();

        // Split the elements by comma
        for (wxString str : strarray)
        {
            str.Trim(true);
            str.Trim(false);
            if (str != "")
                rom->urls.push_back(str);
        }
        rom->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    m_TextCtrl_ROMs_ToolTip_OnText
    Handles the rom tooltip text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ROMs_ToolTip_OnText(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom == NULL)
        return;
    rom->tooltip = event.GetString();
    rom->wasmodified = true;
    rom->category->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_ROMs_Description_OnText
    Handles the rom description text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ROMs_Description_OnText(wxCommandEvent& event)
{
    ROM* rom = this->FindROM(this->m_SelectedItem);
    if (rom == NULL)
        return;
    rom->description = event.GetString();
    rom->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_ROMsCategory_Folder_OnText
    Handles the rom category folder text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ROMsCategory_Folder_OnText(wxCommandEvent& event)
{
    Category* cat = this->FindCategory_ROMs(this->m_SelectedItem);
    if (cat == NULL)
        return;
    cat->foldername = event.GetString();

    // Replace all invalid characters with an underscore
    wxRegEx invalid(wxT("[<>:\"/\\\\|?*\\x00-\\x1F]"));
    invalid.ReplaceAll(&cat->foldername, wxT("_"));
    cat->foldername.Trim(true);
    cat->foldername.Trim(false);
    if (cat->foldername.IsEmpty())
        cat->foldername = wxString("new");

    cat->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_ROMsCategory_DisplayName_OnText
    Handles the rom category name text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ROMsCategory_DisplayName_OnText(wxCommandEvent& event)
{
    Category* cat = this->FindCategory_ROMs(this->m_SelectedItem);
    if (cat == NULL)
        return;
    cat->displayname = event.GetString();
    this->m_TreeCtrl_ROMs->SetItemText(cat->treeid, cat->displayname);
    cat->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_ROMsCategory_Description_OnText
    Handles the rom category description text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ROMsCategory_Description_OnText(wxCommandEvent& event)
{
    Category* cat = this->FindCategory_ROMs(this->m_SelectedItem);
    if (cat == NULL)
        return;
    cat->description = event.GetString();
    cat->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_Button_ROMs_Preview_OnButtonClick
    Handles the rom preview button event
    @param Unused
==============================*/

void Main::m_Button_ROMs_Preview_OnButtonClick(wxCommandEvent& event)
{
    wxString content;
    wxString url = wxString("");

    // Save changes
    this->Save();

    // Compile the relevant page(s)
    if (treeitem_iscategory(this->m_TreeCtrl_ROMs, this->m_SelectedItem)) // Handle category preview
    {
        Category* cat = this->FindCategory_ROMs(this->m_SelectedItem);
        if (cat != NULL)
        {
            long ret = wxExecute("./cellar-generator roms", wxEXEC_SYNC);
            if (ret != 0)
            {
                wxMessageBox(wxString::Format("Generator returned error code %d", ret), wxString("Generator error"), wxICON_ERROR);
                event.Skip();
                return;
            }
            url = this->m_WorkingDir + wxString("/roms.html") + wxString("#") + cat->foldername;
        }
    }
    else // Handle romect preview
    {
        ROM* rom = this->FindROM(this->m_SelectedItem);
        if (rom != NULL)
        {
            long ret = wxExecute("./cellar-generator roms -single " + rom->category->foldername + "/" + rom->filename, wxEXEC_SYNC);
            if (ret != 0)
            {
                wxMessageBox(wxString::Format("Generator returned error code %d", ret), wxString("Generator error"), wxICON_ERROR);
                event.Skip();
                return;
            }
            url = this->m_WorkingDir + wxString("/roms/") + rom->category->foldername + wxString("/") + rom->filename + wxString(".html");
        }
    }

    // Launch the generated page in a browser window
    wxLaunchDefaultBrowser(wxString("file:") + url);
    event.Skip();
}


/*==============================
    m_Timer_OnTimer
    Handles the timer that starts with the program to fix the
    initial sash positions
    @param Unused
==============================*/

void Main::m_Timer_OnTimer(wxTimerEvent&)
{
    this->m_Splitter_ROMs->SetSashPosition(200);
}


/*==============================
    OnPopupClick_Categories
    Handles the clicking of a category popup
    @param The event that was generated
==============================*/

void Main::OnPopupClick_Categories(wxCommandEvent& event)
{
    Category* cat = new Category();
    cat->foldername = "new";
    cat->index = this->m_Category_ROMs.size();
    cat->displayname = "New";
    cat->description = "";
    cat->wasmodified = false;
    cat->treeid = this->m_TreeCtrl_ROMs->AppendItem(this->m_TreeCtrl_ROMs->GetRootItem(), cat->displayname != "" ? cat->displayname : cat->foldername);
    cat->pages.clear();
    this->m_Category_ROMs.push_back(cat);
    event.Skip();
}


/*==============================
    OnPopupClick_ROMs
    Handles the clicking of a rom popup
    @param The event that was generated
==============================*/

void Main::OnPopupClick_ROMs(wxCommandEvent& event)
{
    ROM* rom;
    Category* cat;
    int index = 0;
    wxTreeItemId cat_id = this->m_SelectedItem;

    // Find the category of the element
    if (!treeitem_iscategory(this->m_TreeCtrl_ROMs, cat_id))
        cat_id = this->m_TreeCtrl_ROMs->GetItemParent(cat_id);
    cat = this->FindCategory_ROMs(cat_id);

    // If the category doesn't exist, stop
    if (cat == NULL)
        return;

    // Decide what to do based on the event ID
    switch (event.GetId())
    {
        case wxID_NEW: // Create a new rom
            rom = new ROM();
            rom->index = cat->pages.size();
            rom->filename = "new";
            rom->displayname = "New ROM";
            rom->icon = "";
            rom->date = 0;
            rom->tooltip = "";
            rom->description = "";
            rom->agerating = "";
            rom->contentwarning = "";
            rom->aidisclosure = "";
            rom->wasmodified = true;
            rom->images.clear();
            rom->urls.clear();
            rom->tags.clear();
            rom->category = cat;
            rom->treeid = this->m_TreeCtrl_ROMs->AppendItem(cat_id, rom->displayname);
            cat->pages.push_back(rom);
            this->m_TreeCtrl_ROMs->Expand(cat->treeid);
            this->m_TreeCtrl_ROMs->SelectItem(rom->treeid);
            this->m_SelectedItem = rom->treeid;
            cat->wasmodified = true;
            this->MarkModified();
            break;
        case wxID_DELETE: // Delete an existing rom
            rom = this->FindROM(this->m_SelectedItem);
            if (rom != NULL)
            {
                cat->pages.erase(cat->pages.begin() + rom->index);
                this->m_TreeCtrl_ROMs->Delete(rom->treeid);
                if (wxFileExists(this->m_WorkingDir + wxString("/roms/") + cat->foldername + wxString("/") + rom->filename + wxString(".html")))
                    wxRemoveFile(this->m_WorkingDir + wxString("/roms/") + cat->foldername + wxString("/") + rom->filename + wxString(".html"));
                if (wxFileExists(this->m_WorkingDir + wxString("/roms/") + cat->foldername + wxString("/markdown/") + rom->filename + wxString(".md")))
                    wxRemoveFile(this->m_WorkingDir + wxString("/roms/") + cat->foldername + wxString("/markdown/") + rom->filename + wxString(".md"));
                delete rom;
                for (void* romptr : cat->pages)
                {
                    rom = (ROM*)romptr;
                    rom->index = index++;
                }
                cat->wasmodified = true;
                this->MarkModified();
            }
            break;
    }
}


/*==============================
    OnClose
    Handles the frame close event
    @param  The event that was generated
==============================*/

void Main::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto() && this->m_Modified)
    {
        if (wxMessageBox("You have unsaved changes. Continue?", "Unsaved changes", wxICON_QUESTION | wxYES_NO) != wxYES)
        {
            event.Veto();
            return;
        }
    }
    event.Skip();
}


/*==============================
    InitialCheck
    Performs an initial check to make sure the program
    is set up properly
==============================*/

void Main::InitialCheck()
{
    wxString errors = wxString("");
    if (!wxDirExists(this->m_WorkingDir + wxString("/") + wxString("templates")))
        errors += wxString("Templates folder is missing. Are you sure the editor is in the repository's root?\n");
    #ifdef _WIN32
        if (!wxFileExists(this->m_WorkingDir + wxString("/") + wxString("cellar-generator.exe")))
            errors += wxString("Cellar Generator is missing. Did you remember to compile it and place it next to this program?\n");
    #else
        if (!wxFileExists(this->m_WorkingDir + wxString("/") + wxString("cellar-generator")))
            errors += wxString("Cellar Generator is missing. Did you remember to compile it and place it next to this program?\n");
    #endif
    if (errors != "")
    {
        errors = wxString("The following errors were encountered:\n\n") + errors + wxString("\nThe website might not compile properly!");
        wxMessageBox(errors, wxString("Initialization error"), wxICON_ERROR);
    }

}


/*==============================
    FindCategory_ROMs
    Finds a rom category object using a tree item id
    @param  The tree item ID to find the category of
    @return The category that was found, or NULL
==============================*/

Category* Main::FindCategory_ROMs(wxTreeItemId item)
{
    for (Category* cat : this->m_Category_ROMs)
        if (cat->treeid == item)
            return cat;
    return NULL;
}


/*==============================
    FindROM
    Finds a rom object using a tree item id
    @param  The tree item ID to find the rom of
    @return The rom that was found, or NULL
==============================*/

ROM* Main::FindROM(wxTreeItemId item)
{
    wxTreeItemId cat_id;
    Category* cat_elem;

    if (treeitem_iscategory(this->m_TreeCtrl_ROMs, item))
        return NULL;

    cat_id = this->m_TreeCtrl_ROMs->GetItemParent(item);
    cat_elem = this->FindCategory_ROMs(cat_id);
    if (cat_elem == NULL)
        return NULL;

    for (void* rom : cat_elem->pages)
        if (((ROM*)rom)->treeid == item)
            return (ROM*)rom;
    return NULL;
}


/*==============================
    UpdateTree
    Updates a given tree list
    @param  A pointer to the tree to update
    @param  The folder of the tree we're updating
    @param  A pointer of the category list to update
==============================*/

void Main::UpdateTree(wxTreeCtrl* tree, wxString folder, std::vector<Category*>* categorylist)
{
    bool cont;
    int index;
    wxTreeItemId root;
    wxString filename;
    wxDir pagepath;

    // First, free all memory
    for (Category* cat : *categorylist)
    {
        for (void* child : cat->pages)
            delete (ROM*)child;
        delete cat;
    }

    // Initialize the trees
    categorylist->clear();
    tree->DeleteAllItems();
    root = tree->AddRoot("Root");

    // If the folder doesn't exist, create it
    if (!wxDirExists(pagepath.GetName()))
        wxMkDir(pagepath.GetName(), wxS_DIR_DEFAULT);
    pagepath.Open(this->m_WorkingDir + wxString("/")+folder);
    if (!pagepath.IsOpened())
        return;

    // First, open the relevant JSON file and add folders that are registered in the JSON file
    index = json_loadcategories(this->m_WorkingDir + wxString("/") + folder + wxString("/") + folder + wxString(".json"), categorylist);

    // Now, add all folders which aren't in the JSON
    cont = pagepath.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
    while (cont)
    {
        bool hasfile = false;

        // Check if the folder already exists in our list
        for (Category* cat : *categorylist)
        {
            if (cat->foldername == filename)
            {
                hasfile = true;
                break;
            }
        }

        // If it doesn't, create it
        if (!hasfile)
        {
            Category* cat = new Category();
            cat->foldername = filename;
            cat->index = index++;
            cat->displayname = filename;
            cat->description = wxString("");
            cat->treeid = NULL;
            cat->pages.clear();
            categorylist->push_back(cat);
        }

        // Check the next folder
        cont = pagepath.GetNext(&filename);
    }

    // Finally, generate the tree
    for (Category* cat : *categorylist)
        cat->treeid = tree->AppendItem(root, cat->displayname);
}


/*==============================
    LoadROMs
    Load all roms stored in the JSON file
==============================*/

void Main::LoadROMs()
{
    json_loadroms(this->m_WorkingDir, &this->m_Category_ROMs, this->m_TreeCtrl_ROMs);
    this->ShowROMEditor(false);
}


/*==============================
    EndDrag
    Handle the drag end event properly
    @param The generated event
    @param The affected tree
    @param The affected category list
==============================*/

void Main::EndDrag(wxTreeEvent& event, wxTreeCtrl* tree, std::vector<Category*>* categorylist)
{
    wxTreeItemId src = this->m_DraggedItem;
    wxTreeItemId dest = event.GetItem();
    Category* src_elem = NULL;
    Category* dest_elem = NULL;
    this->m_DraggedItem = NULL;

    // If the destination is not a category item, then get the parent
    if (!treeitem_iscategory(tree, dest))
        dest = tree->GetItemParent(dest);

    // Ensure we have a valid destination node
    if (!dest.IsOk() || src == dest)
        return;

    // Ignore placing the source directly above the destination, since it won't move at all.
    if (tree->GetPrevSibling(src) == dest)
        return;

    // Only allow moving categories into categories
    if (treeitem_iscategory(tree, src) && treeitem_iscategory(tree, dest))
    {
        int index = 0;

        // Get the category elements themselves
        for (Category* cat : *categorylist)
        {
            if (cat->treeid == src)
                src_elem = cat;
            if (cat->treeid == dest)
                dest_elem = cat;
        }

        // Move the elements
        if (dest_elem->index < src_elem->index)
        {
            categorylist->erase(categorylist->begin() + src_elem->index);
            categorylist->insert(categorylist->begin() + dest_elem->index + 1, src_elem);
        }
        else
        {
            categorylist->erase(categorylist->begin() + src_elem->index);
            categorylist->insert(categorylist->begin() + dest_elem->index, src_elem);
        }
        tree->Delete(src);
        src_elem->treeid = tree->InsertItem(tree->GetRootItem(), dest, src_elem->displayname);

        // Reinsert the children
        for (void* child : src_elem->pages)
        {
            ROM* rom = (ROM*)child;
            rom->treeid = tree->InsertItem(src_elem->treeid, rom->index, rom->displayname);
            rom->category->wasmodified = true;
        }

        // Correct the index values
        for (Category* cat : *categorylist)
        {
            cat->index = index++;
            cat->wasmodified = true;
        }

        // Mark the program as modified
        this->MarkModified();
    }
}


/*==============================
    EndDrag_ROM
    Handle the drag end event for roms properly
    @param The generated event
==============================*/

void Main::EndDrag_ROM(wxTreeEvent& event)
{
    wxTreeItemId src = this->m_DraggedItem;
    wxTreeItemId dest = event.GetItem();
    this->m_DraggedItem = NULL;

    // Ensure we have a valid destination node
    if (!dest.IsOk() || src == dest)
        return;

    // Ignore placing the source directly above the destination, since it won't move at all.
    if (this->m_TreeCtrl_ROMs->GetPrevSibling(src) == dest)
        return;

    // Only work on rom nodes
    if (!treeitem_iscategory(this->m_TreeCtrl_ROMs, dest))
    {
        Category* cat;
        std::vector<void*>* pages;
        int index = 0;
        ROM* rom_src = this->FindROM(src);
        ROM* rom_dest = this->FindROM(dest);

        // Ensure a valid source and destination
        if (rom_src == NULL || rom_dest == NULL)
            return;

        // Ensure both roms are in the same category
        cat = rom_src->category;
        pages = &cat->pages;
        if (cat != rom_dest->category)
            return;

        // Move the elements
        if (rom_dest->index < rom_src->index)
        {
            pages->erase(pages->begin() + rom_src->index);
            pages->insert(pages->begin() + rom_dest->index + 1, rom_src);
        }
        else
        {
            pages->erase(pages->begin() + rom_src->index);
            pages->insert(pages->begin() + rom_dest->index, rom_src);
        }
        this->m_TreeCtrl_ROMs->Delete(src);
        rom_src->treeid = this->m_TreeCtrl_ROMs->InsertItem(cat->treeid, dest, rom_src->displayname);

        // Correct the index values
        for (void* romptr : *pages)
        {
            ROM* rom = (ROM*)romptr;
            rom->index = index++;
        }

        // Mark the rom and its category as modified
        cat->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    Save
    Save the entire website and compile it
==============================*/

void Main::Save()
{
    // Save the json files
    json_save(this->m_WorkingDir, &this->m_Category_ROMs);;

    // Mark the program as no longer modified
    this->MarkModified(false);
}


/*==============================
    MarkModified
    Mark the work as modified
==============================*/

void Main::MarkModified(bool modified)
{
    if (modified)
    {
        if (!this->m_Modified)
        {
            this->SetTitle(this->m_WorkingDir + wxString(" *"));
            this->m_Modified = true;
        }
    }
    else
    {
        this->SetTitle(this->m_WorkingDir);
        this->m_Modified = false;
    }
}


/*==============================
    ShowROMEditor
    Show or hide the rom editor panel
    @param Whether to show the editor panel 
==============================*/

void Main::ShowROMEditor(bool show)
{
    if (show)
    {
        this->m_ScrolledWindow_ROM_Editor->Show();
        this->m_ScrolledWindow_ROMCategory_Editor->Hide();
        this->m_Button_ROMs_Preview->Show();
    }
    else
    {
        this->m_ScrolledWindow_ROM_Editor->Hide();
        this->m_ScrolledWindow_ROMCategory_Editor->Hide();
        this->m_Button_ROMs_Preview->Hide();
    }
    this->m_Panel_ROMs_Editor->Layout();
}


/*==============================
    ShowROMCategoryEditor
    Show or hide the rom category editor panel
    @param Whether to show the editor panel 
==============================*/

void Main::ShowROMCategoryEditor(bool show)
{
    if (show)
    {
        this->m_ScrolledWindow_ROM_Editor->Hide();
        this->m_ScrolledWindow_ROMCategory_Editor->Show();
        this->m_Button_ROMs_Preview->Show();
    }
    else
    {
        this->m_ScrolledWindow_ROM_Editor->Hide();
        this->m_ScrolledWindow_ROMCategory_Editor->Hide();
        this->m_Button_ROMs_Preview->Hide();
    }
    this->m_Panel_ROMs_Editor->Layout();
}