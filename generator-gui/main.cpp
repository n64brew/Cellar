/***************************************************************
                            main.cpp

The main code of the static site generator
***************************************************************/

#include "main.h"
#include "framefind.h"
#include "frameinsertimage.h"
#include "frameinsertvideo.h"
#include "helper.h"
#include "json.h"
#include "include/md4c/md4c-html.h"
#include <wx/event.h>
#include <wx/msgdlg.h>
#include <wx/treelist.h>
#include <wx/dir.h>
#include <wx/textfile.h>
#include <wx/dirdlg.h>


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

    // Create the main project panels
    this->m_Panel_Projects = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* m_Sizer_Projects;
    m_Sizer_Projects = new wxBoxSizer(wxVERTICAL);
    this->m_Splitter_Projects = new wxSplitterWindow(this->m_Panel_Projects, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
    this->m_Splitter_Projects->SetSashGravity(0);
    this->m_Splitter_Projects->Connect(wxEVT_IDLE, wxIdleEventHandler(Main::m_Splitter_ProjectsOnIdle), NULL, this);

    // Create the project tree
    this->m_Panel_Projects_Tree = new wxPanel(this->m_Splitter_Projects, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_Projects_Tree;
    m_Sizer_Projects_Tree = new wxFlexGridSizer(0, 0, 0, 0);
    m_Sizer_Projects_Tree->AddGrowableCol(0);
    m_Sizer_Projects_Tree->AddGrowableRow(0);
    m_Sizer_Projects_Tree->SetFlexibleDirection(wxBOTH);
    m_Sizer_Projects_Tree->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    this->m_TreeCtrl_Projects = new wxTreeCtrl(this->m_Panel_Projects_Tree, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_HIDE_ROOT);
    m_Sizer_Projects_Tree->Add(this->m_TreeCtrl_Projects, 0, wxALL|wxEXPAND, 5);
    this->m_Panel_Projects_Tree->SetSizer(m_Sizer_Projects_Tree);
    this->m_Panel_Projects_Tree->Layout();
    m_Sizer_Projects_Tree->Fit(this->m_Panel_Projects_Tree);

    // Create the primary project editor
    this->m_Panel_Projects_Editor = new wxPanel(this->m_Splitter_Projects, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxFlexGridSizer* m_Sizer_Projects_Editor;
    m_Sizer_Projects_Editor = new wxFlexGridSizer(3, 1, 0, 0);
    m_Sizer_Projects_Editor->AddGrowableCol(0);
    m_Sizer_Projects_Editor->AddGrowableRow(0);
    m_Sizer_Projects_Editor->AddGrowableRow(1);
    m_Sizer_Projects_Editor->SetFlexibleDirection(wxBOTH);
    m_Sizer_Projects_Editor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);
    this->m_ScrolledWindow_Project_Editor = new wxScrolledWindow(this->m_Panel_Projects_Editor, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
    this->m_ScrolledWindow_Project_Editor->SetScrollRate(5, 5);
    this->m_ScrolledWindow_Project_Editor->Hide();
    this->m_ScrolledWindow_Project_Editor->SetMinSize(wxSize(0,0));
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

    // Create the project page editor controls 
    this->m_Label_Projects_File = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("File:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_File->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_File, 0, wxALL, 5);
    this->m_TextCtrl_Projects_File = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_File->SetToolTip(wxT("The filename of the project"));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_File, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_Name = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Display Name:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_Name->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_Name, 0, wxALL, 5);
    this->m_TextCtrl_Projects_Name = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    this->m_TextCtrl_Projects_Name->SetToolTip(wxT("The display name of the project"));
    #if wxUSE_SPELLCHECK
        this->m_TextCtrl_Projects_Name->EnableProofCheck();
    #endif
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_Name, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_Icon = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Icon:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_Icon->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_Icon, 0, wxALL, 5);
    this->m_TextCtrl_Projects_Icon = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_Icon->SetToolTip(wxT("The icon used to display the project in the list of projects. Requires a relative path (to the working directory) to a 240x125 png."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_Icon, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_Tags = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Tags:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_Tags->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_Tags, 0, wxALL, 5);
    this->m_TextCtrl_Projects_Tags = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_Tags->SetToolTip(wxT("Project tags, comma separated."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_Tags, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_Images = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Images:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_Images->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_Images, 0, wxALL, 5);
    this->m_TextCtrl_Projects_Images = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_Images->SetToolTip(wxT("Images for this project, comma separated. Paths should be relative to the working directory."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_Images, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_Date = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Date:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_Date->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_Date, 0, wxALL|wxEXPAND, 5);
    this->m_TextCtrl_Projects_Date = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_Date->SetToolTip(wxT("The date of this project's release."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_Date, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_URLs = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("URLs:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_URLs->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_URLs, 0, wxALL, 5);
    this->m_TextCtrl_Projects_URLs = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_Projects_URLs->SetToolTip(wxT("URLs for this project, comma separated."));
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_URLs, 0, wxALL|wxEXPAND, 5);
    this->m_Label_Projects_ToolTip = new wxStaticText(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxT("Subtitle:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_Projects_ToolTip->Wrap(-1);
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_Label_Projects_ToolTip, 0, wxALL, 5);
    this->m_TextCtrl_Projects_ToolTip = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    this->m_TextCtrl_Projects_ToolTip->SetToolTip(wxT("Brief description that shows up when hovering over a project with the mouse."));
    #if wxUSE_SPELLCHECK
        this->m_TextCtrl_Projects_ToolTip->EnableProofCheck();
    #endif
    m_Sizer_ScrolledWindow_Editor_Main->Add(this->m_TextCtrl_Projects_ToolTip, 0, wxALL|wxEXPAND, 5);
    m_Sizer_ScrolledWindow_Editor->Add(m_Sizer_ScrolledWindow_Editor_Main, 1, wxEXPAND, 5);
    this->m_TextCtrl_Projects_Description = new wxTextCtrl(this->m_ScrolledWindow_Project_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1,-1), wxTE_MULTILINE | wxTE_RICH2);
    this->m_TextCtrl_Projects_Description->SetToolTip(wxT("The project description. Supports markdown."));
    this->m_TextCtrl_Projects_Description->SetMinSize(wxSize(-1,200));
    #if wxUSE_SPELLCHECK
        this->m_TextCtrl_Projects_Description->EnableProofCheck();
    #endif
    m_Sizer_ScrolledWindow_Editor->Add(this->m_TextCtrl_Projects_Description, 0, wxALL|wxEXPAND, 5);
    this->m_ScrolledWindow_Project_Editor->SetSizer(m_Sizer_ScrolledWindow_Editor);
    this->m_ScrolledWindow_Project_Editor->Layout();
    m_Sizer_ScrolledWindow_Editor->Fit(this->m_ScrolledWindow_Project_Editor);
    m_Sizer_Projects_Editor->Add(this->m_ScrolledWindow_Project_Editor, 0, wxALL|wxEXPAND, 5);

    // Create the project category editor 
    this->m_ScrolledWindow_ProjectCategory_Editor = new wxScrolledWindow(this->m_Panel_Projects_Editor, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL);
    this->m_ScrolledWindow_ProjectCategory_Editor->SetScrollRate(5, 5);
    this->m_ScrolledWindow_ProjectCategory_Editor->Hide();
    this->m_ScrolledWindow_ProjectCategory_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_ProjectCategory_Editor;
    m_Sizer_ScrolledWindow_ProjectCategory_Editor = new wxFlexGridSizer(0, 1, 0, 0);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->AddGrowableCol(0);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->AddGrowableRow(1);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->SetMinSize(wxSize(0,0));
    wxFlexGridSizer* m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main;
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main = new wxFlexGridSizer(0, 2, 0, 0);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->AddGrowableCol(1);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->SetFlexibleDirection(wxBOTH);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->SetMinSize(wxSize(0,0));

    // Create the project category editor controls 
    this->m_Label_ProjectsCategory_Folder = new wxStaticText(this->m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxT("Folder:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_ProjectsCategory_Folder->Wrap(-1);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add(this->m_Label_ProjectsCategory_Folder, 0, wxALL, 5);
    this->m_TextCtrl_ProjectsCategory_Folder = new wxTextCtrl(this->m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    this->m_TextCtrl_ProjectsCategory_Folder->Enable(false);
    this->m_TextCtrl_ProjectsCategory_Folder->SetToolTip(wxT("The folder that represents this category"));
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add(this->m_TextCtrl_ProjectsCategory_Folder, 0, wxALL|wxEXPAND, 5);
    this->m_Label_ProjectsCategory_DisplayName = new wxStaticText(this->m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxT("Display Name:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_ProjectsCategory_DisplayName->Wrap(-1);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add(this->m_Label_ProjectsCategory_DisplayName, 0, wxALL, 5);
    this->m_TextCtrl_ProjectsCategory_DisplayName = new wxTextCtrl(this->m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
    this->m_TextCtrl_ProjectsCategory_DisplayName->SetToolTip(wxT("The display name of this category"));
    #if wxUSE_SPELLCHECK
        this->m_TextCtrl_ProjectsCategory_DisplayName->EnableProofCheck();
    #endif
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add(this->m_TextCtrl_ProjectsCategory_DisplayName, 0, wxALL|wxEXPAND, 5);
    this->m_Label_ProjectsCategory_Description = new wxStaticText(this->m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxT("Description:"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Label_ProjectsCategory_Description->Wrap(-1);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add(this->m_Label_ProjectsCategory_Description, 0, wxALL, 5);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main->Add(0, 0, 1, wxEXPAND, 5);
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->Add(m_Sizer_ScrolledWindow_ProjectCategory_Editor_Main, 1, wxEXPAND, 5);
    this->m_TextCtrl_ProjectsCategory_Description = new wxTextCtrl(this->m_ScrolledWindow_ProjectCategory_Editor, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH2);
    this->m_TextCtrl_ProjectsCategory_Description->SetToolTip(wxT("The filename of the project"));
    this->m_TextCtrl_ProjectsCategory_Description->SetMinSize(wxSize(-1,200));
    #if wxUSE_SPELLCHECK
        this->m_TextCtrl_ProjectsCategory_Description->EnableProofCheck();
    #endif
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->Add(this->m_TextCtrl_ProjectsCategory_Description, 0, wxALL|wxEXPAND, 5);
    this->m_ScrolledWindow_ProjectCategory_Editor->SetSizer(m_Sizer_ScrolledWindow_ProjectCategory_Editor);
    this->m_ScrolledWindow_ProjectCategory_Editor->Layout();
    m_Sizer_ScrolledWindow_ProjectCategory_Editor->Fit(this->m_ScrolledWindow_ProjectCategory_Editor);
    m_Sizer_Projects_Editor->Add(this->m_ScrolledWindow_ProjectCategory_Editor, 0, wxEXPAND | wxALL, 5);

    // Create the project preview button
    this->m_Button_Projects_Preview = new wxButton(this->m_Panel_Projects_Editor, wxID_ANY, wxT("Preview"), wxDefaultPosition, wxDefaultSize, 0);
    this->m_Button_Projects_Preview->Hide();
    this->m_Button_Projects_Preview->SetMinSize(wxSize(-1,24));
    m_Sizer_Projects_Editor->Add(this->m_Button_Projects_Preview, 0, wxALIGN_RIGHT|wxALL, 5);

    // Add everything to the projects panel
    this->m_Panel_Projects_Editor->SetSizer(m_Sizer_Projects_Editor);
    this->m_Panel_Projects_Editor->Layout();
    m_Sizer_Projects_Editor->Fit(this->m_Panel_Projects_Editor);
    this->m_Splitter_Projects->SplitVertically(this->m_Panel_Projects_Tree, this->m_Panel_Projects_Editor, 200);
    m_Sizer_Projects->Add(this->m_Splitter_Projects, 1, wxEXPAND, 5);
    this->m_Panel_Projects->SetSizer(m_Sizer_Projects);
    this->m_Panel_Projects->Layout();
    m_Sizer_Projects->Fit(this->m_Panel_Projects);

    // Finalize
    m_Sizer_Main->Add(m_Panel_Projects, 1, wxEXPAND | wxALL, 5);
    this->SetSizer(m_Sizer_Main);
    this->Layout();

    // Add the menubar
    this->m_Menubar_Main = new wxMenuBar(0);
    this->m_Menu_File = new wxMenu();
    wxMenuItem* m_MenuItem_OpenDir;
    m_MenuItem_OpenDir = new wxMenuItem(this->m_Menu_File, wxID_ANY, wxString(wxT("Open Working Directory")) + wxT('\t') + wxT("CTRL+O"), wxEmptyString, wxITEM_NORMAL);
    this->m_Menu_File->Append(m_MenuItem_OpenDir);
    wxMenuItem* m_MenuItem_Save;
    m_MenuItem_Save = new wxMenuItem(this->m_Menu_File, wxID_ANY, wxString(wxT("Save Changes")) + wxT('\t') + wxT("CTRL+S"), wxEmptyString, wxITEM_NORMAL);
    this->m_Menu_File->Append(m_MenuItem_Save);
    this->m_Menu_File->AppendSeparator();
    wxMenuItem* m_MenuItem_FullRecompile;
    m_MenuItem_FullRecompile = new wxMenuItem(this->m_Menu_File, wxID_ANY, wxString(wxT("Full Website Recompile")), wxEmptyString, wxITEM_NORMAL);
    this->m_Menu_File->Append(m_MenuItem_FullRecompile);
    this->m_Menubar_Main->Append(this->m_Menu_File, wxT("File"));
    this->m_Menu_Edit = new wxMenu();
    wxMenuItem* m_MenuItem_Find;
    m_MenuItem_Find = new wxMenuItem(this->m_Menu_Edit, wxID_ANY, wxString(wxT("Find")) + wxT('\t') + wxT("CTRL+F"), wxEmptyString, wxITEM_NORMAL);
    this->m_Menu_Edit->Append(m_MenuItem_Find);
    this->m_Menu_Edit->AppendSeparator();
    wxMenuItem* m_MenuItem_InsertImage;
    m_MenuItem_InsertImage = new wxMenuItem(this->m_Menu_Edit, wxID_ANY, wxString(wxT("Insert Image")) + wxT('\t') + wxT("CTRL+I"), wxEmptyString, wxITEM_NORMAL);
    this->m_Menu_Edit->Append(m_MenuItem_InsertImage);
    wxMenuItem* m_MenuItem_InsertVideo;
    m_MenuItem_InsertVideo = new wxMenuItem(this->m_Menu_Edit, wxID_ANY, wxString(wxT("Insert Video")) + wxT('\t') + wxT("CTRL+B"), wxEmptyString, wxITEM_NORMAL);
    wxMenuItem* m_MenuItem_InsertTOC;
    m_MenuItem_InsertTOC = new wxMenuItem(this->m_Menu_Edit, wxID_ANY, wxString(wxT("Insert Table of Contents")) + wxT('\t') + wxT("CTRL+T"), wxEmptyString, wxITEM_NORMAL);
    this->m_Menu_Edit->Append(m_MenuItem_InsertTOC);
    this->m_Menubar_Main->Append(this->m_Menu_Edit, wxT("Edit"));
    this->SetMenuBar(this->m_Menubar_Main);

    // Start a timer to fix the splitter location
    this->m_Timer = new wxTimer();
    this->m_Timer->Start(100, wxTIMER_ONE_SHOT);

    // Center the final window
    this->Centre(wxBOTH);

    // Connect Events
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(Main::OnClose));
    this->m_TreeCtrl_Projects->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(Main::m_TreeCtrl_Projects_OnTreeRightClick), NULL, this);
    this->m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeBeginDrag), NULL, this);
    this->m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeEndDrag), NULL, this);
    this->m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeEndLabelEdit), NULL, this);
    this->m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeItemMenu), NULL, this);
    this->m_TreeCtrl_Projects->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeSelChanged), NULL, this);
    this->m_TextCtrl_Projects_File->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_File_OnText), NULL, this);
    this->m_TextCtrl_Projects_Name->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Name_OnText), NULL, this);
    this->m_TextCtrl_Projects_Icon->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Icon_OnText), NULL, this);
    this->m_TextCtrl_Projects_Tags->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Tags_OnText), NULL, this);
    this->m_TextCtrl_Projects_Images->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Images_OnText), NULL, this);
    this->m_TextCtrl_Projects_Date->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Date_OnText), NULL, this);
    this->m_TextCtrl_Projects_URLs->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_URLs_OnText), NULL, this);
    this->m_TextCtrl_Projects_ToolTip->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_ToolTip_OnText), NULL, this);
    this->m_TextCtrl_Projects_Description->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Description_OnText), NULL, this);
    this->m_TextCtrl_ProjectsCategory_DisplayName->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ProjectsCategory_DisplayName_OnText), NULL, this);
    this->m_TextCtrl_ProjectsCategory_Description->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ProjectsCategory_Description_OnText), NULL, this);
    this->m_Button_Projects_Preview->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Projects_Preview_OnButtonClick), NULL, this);
    this->m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_OpenDir_OnMenuSelection), this, m_MenuItem_OpenDir->GetId());
    this->m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Save_OnMenuSelection), this, m_MenuItem_Save->GetId());
    this->m_Menu_File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_FullRecompile_OnMenuSelection), this, m_MenuItem_FullRecompile->GetId());
    this->m_Menu_Edit->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_Find_OnMenuSelection), this, m_MenuItem_Find->GetId());
    this->m_Menu_Edit->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_InsertImage_OnMenuSelection), this, m_MenuItem_InsertImage->GetId());
    this->m_Menu_Edit->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_InsertVideo_OnMenuSelection), this, m_MenuItem_InsertVideo->GetId());
    this->m_Menu_Edit->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::m_MenuItem_InsertTOC_OnMenuSelection), this, m_MenuItem_InsertTOC->GetId());
    this->m_Timer->Bind(wxEVT_TIMER, wxTimerEventHandler(Main::m_Timer_OnTimer), this, this->m_Timer->GetId());

    // Load the projects from the current working directory
    this->UpdateTree(this->m_TreeCtrl_Projects, "projects", &this->m_Category_Projects);
    this->LoadProjects();
}


/*==============================
    App (Destructor)
    Cleans up the class before deletion
==============================*/

Main::~Main()
{
    this->m_TreeCtrl_Projects->Disconnect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeBeginDrag), NULL, this);
    this->m_TreeCtrl_Projects->Disconnect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeEndDrag), NULL, this);
    this->m_TreeCtrl_Projects->Disconnect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeEndLabelEdit), NULL, this);
    this->m_TreeCtrl_Projects->Disconnect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeItemMenu), NULL, this);
    this->m_TreeCtrl_Projects->Disconnect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(Main::m_TreeCtrl_Projects_OnTreeSelChanged), NULL, this);
    this->m_TextCtrl_Projects_File->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_File_OnText), NULL, this);
    this->m_TextCtrl_Projects_Name->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Name_OnText), NULL, this);
    this->m_TextCtrl_Projects_Icon->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Icon_OnText), NULL, this);
    this->m_TextCtrl_Projects_Tags->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Tags_OnText), NULL, this);
    this->m_TextCtrl_Projects_Images->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Images_OnText), NULL, this);
    this->m_TextCtrl_Projects_Date->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Date_OnText), NULL, this);
    this->m_TextCtrl_Projects_URLs->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_URLs_OnText), NULL, this);
    this->m_TextCtrl_Projects_ToolTip->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_ToolTip_OnText), NULL, this);
    this->m_TextCtrl_Projects_Description->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_Projects_Description_OnText), NULL, this);
    this->m_TextCtrl_ProjectsCategory_DisplayName->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ProjectsCategory_DisplayName_OnText), NULL, this);
    this->m_TextCtrl_ProjectsCategory_Description->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(Main::m_TextCtrl_ProjectsCategory_Description_OnText), NULL, this);
    this->m_Button_Projects_Preview->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::m_Button_Projects_Preview_OnButtonClick), NULL, this);
    this->m_Timer->Disconnect(wxEVT_TIMER, wxTimerEventHandler(Main::m_Timer_OnTimer), NULL, this);
}


/*==============================
    m_Splitter_ProjectsOnIdle
    Handles the project splitter event
    @param Unused
==============================*/

void Main::m_Splitter_ProjectsOnIdle(wxIdleEvent&)
{
    this->m_Splitter_Projects->SetSashPosition(0);
    this->m_Splitter_Projects->Disconnect(wxEVT_IDLE, wxIdleEventHandler(Main::m_Splitter_ProjectsOnIdle), NULL, this);
}


/*==============================
    m_MenuItem_OpenDir_OnMenuSelection
    Handles the Open Directory menu event
    @param Unused
==============================*/

void Main::m_MenuItem_OpenDir_OnMenuSelection(wxCommandEvent&)
{
    wxDirDialog dir(this, wxString("Select Working Directory"), this->m_WorkingDir);
    if (dir.ShowModal() == wxID_OK)
    {
        this->m_WorkingDir = dir.GetPath();
        this->MarkModified(false);
        this->UpdateTree(this->m_TreeCtrl_Projects, "projects", &this->m_Category_Projects);
        this->LoadProjects();
    }
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
    frame->SetParent(this->m_TextCtrl_Projects_Description);
    frame->Show();
    event.Skip();
}


/*==============================
    m_MenuItem_InsertImage_OnMenuSelection
    Handles the Insert Image menu event
    @param Unused
==============================*/

void Main::m_MenuItem_InsertImage_OnMenuSelection(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    wxString basepath = this->m_WorkingDir + wxString("/projects/") + proj->category->foldername + wxString("/");
    FrameImageInsert* frame = new FrameImageInsert(this);
    frame->SetParent(this->m_TextCtrl_Projects_Description, basepath);
    frame->Show();
    event.Skip();
}


/*==============================
    m_MenuItem_InsertVideo_OnMenuSelection
    Handles the Insert Video menu event
    @param Unused
==============================*/

void Main::m_MenuItem_InsertVideo_OnMenuSelection(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    wxString basepath = this->m_WorkingDir + wxString("/projects/") + proj->category->foldername + wxString("/");
    FrameVideoInsert* frame = new FrameVideoInsert(this);
    frame->SetParent(this->m_TextCtrl_Projects_Description, basepath);
    frame->Show();
    event.Skip();
}


/*==============================
    m_MenuItem_InsertTOC_OnMenuSelection
    Handles the Insert TOC menu event
    @param Unused
==============================*/

void Main::m_MenuItem_InsertTOC_OnMenuSelection(wxCommandEvent& event)
{
    this->m_TextCtrl_Projects_Description->WriteText("[TOC]\r\n\r\n");
    event.Skip();
}


/*==============================
    m_MenuItem_FullRecompile_OnMenuSelection
    Handles the Full Recompile menu event
    @param Unused
==============================*/

void Main::m_MenuItem_FullRecompile_OnMenuSelection(wxCommandEvent&)
{
    this->m_FullCompile = true;
    this->Save();
    this->m_FullCompile = false;
}


/*==============================
    m_TreeCtrl_Projects_OnTreeEndLabelEdit
    Handles the project tree label editing event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Projects_OnTreeEndLabelEdit(wxTreeEvent& event)
{
    // Prevent blank display names
    if (event.GetLabel() == "")
    {
        event.Veto();
        return;
    }

    // Handle label editing based on whether it's a category or not
    if (treeitem_iscategory(this->m_TreeCtrl_Projects, event.GetItem())) // Handle category label editing
    {
        wxTreeItemId id = event.GetItem();
        Category* cat = this->FindCategory_Projects(id);
        if (cat != NULL)
        {
            cat->displayname = event.GetLabel();
            if (this->m_SelectedItem == event.GetItem())
                this->m_TextCtrl_ProjectsCategory_DisplayName->ChangeValue(cat->displayname);
            cat->wasmodified = true;
            this->MarkModified();
            return;
        }
    }
    else // Handle project label editing
    {
        Project* proj = this->FindProject(event.GetItem());
        if (proj != NULL)
        {
            proj->displayname = event.GetLabel();
            if (this->m_SelectedItem == event.GetItem())
                this->m_TextCtrl_Projects_Name->ChangeValue(proj->displayname);
            proj->category->wasmodified = true;
            proj->wasmodified = true;
            this->MarkModified();
            return;
        }
    }
    event.Veto();
}


/*==============================
    m_TreeCtrl_Projects_OnTreeRightClick
    Handles the project tree right click event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Projects_OnTreeRightClick(wxMouseEvent& event)
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
    m_TreeCtrl_Projects_OnTreeBeginDrag
    Handles the project tree drag event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Projects_OnTreeBeginDrag(wxTreeEvent& event)
{
    this->m_DraggedItem = event.GetItem();
    event.Allow();
}


/*==============================
    m_TreeCtrl_Projects_OnTreeEndDrag
    Handles the project tree drag ending event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Projects_OnTreeEndDrag(wxTreeEvent& event)
{
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, this->m_SelectedItem))
        this->EndDrag_Project(event);
    else
        this->EndDrag(event, this->m_TreeCtrl_Projects, &this->m_Category_Projects);
}


/*==============================
    m_TreeCtrl_Projects_OnTreeItemMenu
    Handles the project tree right click event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Projects_OnTreeItemMenu(wxTreeEvent& event)
{
    wxMenu menu;
    this->m_SelectedItem = event.GetItem();
    menu.Append(wxID_NEW, wxT("Create project"));
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, event.GetItem()))
        menu.Append(wxID_DELETE, wxT("Delete project"));
    menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::OnPopupClick_Projects), NULL, this);
    PopupMenu(&menu, event.GetPoint());
}


/*==============================
    m_TreeCtrl_Projects_OnTreeSelChanged
    Handles the project tree selection event
    @param The event that was generated
==============================*/

void Main::m_TreeCtrl_Projects_OnTreeSelChanged(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();

    // Handle the item selection
    this->m_SelectedItem = item;
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, item)) // Handle project selection
    {
        wxString wip;
        Project* proj = FindProject(item);

        // Ensure the project exists
        if (proj == NULL)
            return;

        // Fill in the simple text controls
        this->m_TextCtrl_Projects_File->ChangeValue(proj->filename);
        this->m_TextCtrl_Projects_Name->ChangeValue(proj->displayname);
        this->m_TextCtrl_Projects_Icon->ChangeValue(proj->icon);
        this->m_TextCtrl_Projects_ToolTip->ChangeValue(proj->tooltip);
        this->m_TextCtrl_Projects_Description->ChangeValue(proj->description);
        this->m_TextCtrl_Projects_Date->ChangeValue(proj->date);

        // Fill in the tags text control
        wip = wxString("");
        for (wxString str : proj->tags)
            wip += str + wxString(", ");
        this->m_TextCtrl_Projects_Tags->ChangeValue(wip);
        
        // Fill in the images text control
        wip = wxString("");
        for (wxString str : proj->images)
            wip += str + wxString(", ");
        this->m_TextCtrl_Projects_Images->ChangeValue(wip);
        
        // Fill in the url text control
        wip = wxString("");
        for (wxString str : proj->urls)
            wip += str + wxString(", ");
        this->m_TextCtrl_Projects_URLs->ChangeValue(wip);

        // Show the project editor
        this->ShowProjectEditor();
    }
    else if (treeitem_iscategory(this->m_TreeCtrl_Projects, item)) // Handle category selection
    {
        Category* cat = this->FindCategory_Projects(item);

        // Ensure the category exists
        if (cat == NULL)
            return;

        // Fill in the simple text controls
        this->m_TextCtrl_ProjectsCategory_Folder->ChangeValue(cat->foldername);
        this->m_TextCtrl_ProjectsCategory_DisplayName->ChangeValue(cat->displayname);
        this->m_TextCtrl_ProjectsCategory_Description->ChangeValue(cat->description);

        // Show the project category editor
        this->ShowProjectCategoryEditor(true);
    }
    else
        this->ShowProjectEditor(false);
}


/*==============================
    m_TextCtrl_Projects_File_OnText
    Handles the project filename text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_File_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj == NULL)
        return;
    proj->filename = event.GetString();
    proj->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Projects_Name_OnText
    Handles the project name text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_Name_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj == NULL)
        return;
    proj->displayname = event.GetString();
    this->m_TreeCtrl_Projects->SetItemText(proj->treeid, proj->displayname);
    proj->wasmodified = true;
    proj->category->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Projects_Icon_OnText
    Handles the project icon text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_Icon_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj == NULL)
        return;
    proj->icon = event.GetString();
    proj->wasmodified = true;
    proj->category->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Projects_Tags_OnText
    Handles the project tags text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_Tags_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj != NULL)
    {
        wxArrayString strarray = wxSplit(event.GetString(), ',');
        proj->tags.clear();

        // Split the elements by comma
        for (wxString str : strarray)
        {
            str.Trim(true);
            str.Trim(false);
            if (str != "")
                proj->tags.push_back(str);
        }
        proj->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    m_TextCtrl_Projects_Images_OnText
    Handles the project images text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_Images_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj != NULL)
    {
        wxArrayString strarray = wxSplit(event.GetString(), ',');
        proj->images.clear();

        // Split the elements by comma
        for (wxString str : strarray)
        {
            str.Trim(true);
            str.Trim(false);
            if (str != "")
                proj->images.push_back(str);
        }
        proj->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    m_TextCtrl_Projects_Date_OnText
    Handles the project date text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_Date_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj == NULL)
        return;
    proj->date = event.GetString();
    proj->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Projects_URLs_OnText
    Handles the project date text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_URLs_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj != NULL)
    {
        wxArrayString strarray = wxSplit(event.GetString(), ',');
        proj->urls.clear();

        // Split the elements by comma
        for (wxString str : strarray)
        {
            str.Trim(true);
            str.Trim(false);
            if (str != "")
                proj->urls.push_back(str);
        }
        proj->wasmodified = true;
        this->MarkModified();
    }
}


/*==============================
    m_TextCtrl_Projects_ToolTip_OnText
    Handles the project tooltip text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_ToolTip_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj == NULL)
        return;
    proj->tooltip = event.GetString();
    proj->wasmodified = true;
    proj->category->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_Projects_Description_OnText
    Handles the project description text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_Projects_Description_OnText(wxCommandEvent& event)
{
    Project* proj = this->FindProject(this->m_SelectedItem);
    if (proj == NULL)
        return;
    proj->description = event.GetString();
    proj->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_ProjectsCategory_DisplayName_OnText
    Handles the project category name text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ProjectsCategory_DisplayName_OnText(wxCommandEvent& event)
{
    Category* cat = this->FindCategory_Projects(this->m_SelectedItem);
    if (cat == NULL)
        return;
    cat->displayname = event.GetString();
    this->m_TreeCtrl_Projects->SetItemText(cat->treeid, cat->displayname);
    cat->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_TextCtrl_ProjectsCategory_Description_OnText
    Handles the project category description text control edit event
    @param The event that was generated
==============================*/

void Main::m_TextCtrl_ProjectsCategory_Description_OnText(wxCommandEvent& event)
{
    Category* cat = this->FindCategory_Projects(this->m_SelectedItem);
    if (cat == NULL)
        return;
    cat->description = event.GetString();
    cat->wasmodified = true;
    this->MarkModified();
}


/*==============================
    m_Button_Projects_Preview_OnButtonClick
    Handles the project preview button event
    @param Unused
==============================*/

void Main::m_Button_Projects_Preview_OnButtonClick(wxCommandEvent&)
{
    wxString content;
    wxString url = wxString("");

    // Compile the relevant page(s)
    if (treeitem_iscategory(this->m_TreeCtrl_Projects, this->m_SelectedItem)) // Handle category preview
    {
        Category* cat = this->FindCategory_Projects(this->m_SelectedItem);
        if (cat != NULL)
        {
            CompileProjects_List();
            url = this->m_WorkingDir + wxString("/projects.html") + wxString("#") + cat->foldername;
        }
    }
    else // Handle project preview
    {
        Project* proj = this->FindProject(this->m_SelectedItem);
        if (proj != NULL)
        {
            CompileProjects_Project(proj);
            url = this->m_WorkingDir + wxString("/projects/") + proj->category->foldername + wxString("/") + proj->filename + wxString(".html");
        }
    }

    // Launch the generated page in a browser window
    wxLaunchDefaultBrowser(wxString("file:") + url);
}


/*==============================
    m_Timer_OnTimer
    Handles the timer that starts with the program to fix the
    initial sash positions
    @param Unused
==============================*/

void Main::m_Timer_OnTimer(wxTimerEvent&)
{
    this->m_Splitter_Projects->SetSashPosition(200);
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
    cat->index = this->m_Category_Projects.size();
    cat->displayname = "New";
    cat->description = "";
    cat->wasmodified = false;
    cat->treeid = this->m_TreeCtrl_Projects->AppendItem(this->m_TreeCtrl_Projects->GetRootItem(), cat->displayname != "" ? cat->displayname : cat->foldername);
    cat->pages.clear();
    this->m_Category_Projects.push_back(cat);
}


/*==============================
    OnPopupClick_Projects
    Handles the clicking of a project popup
    @param The event that was generated
==============================*/

void Main::OnPopupClick_Projects(wxCommandEvent& event)
{
    Project* proj;
    Category* cat;
    int index = 0;
    wxTreeItemId cat_id = this->m_SelectedItem;

    // Find the category of the element
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, cat_id))
        cat_id = this->m_TreeCtrl_Projects->GetItemParent(cat_id);
    cat = this->FindCategory_Projects(cat_id);

    // If the category doesn't exist, stop
    if (cat == NULL)
        return;

    // Decide what to do based on the event ID
    switch (event.GetId())
    {
        case wxID_NEW: // Create a new project
            proj = new Project();
            proj->index = cat->pages.size();
            proj->filename = "new";
            proj->displayname = "New Project";
            proj->icon = "";
            proj->date = "";
            proj->tooltip = "";
            proj->description = "";
            proj->wasmodified = true;
            proj->images.clear();
            proj->urls.clear();
            proj->tags.clear();
            proj->category = cat;
            proj->treeid = this->m_TreeCtrl_Projects->AppendItem(cat_id, proj->displayname);
            cat->pages.push_back(proj);
            this->m_TreeCtrl_Projects->Expand(cat->treeid);
            this->m_TreeCtrl_Projects->SelectItem(proj->treeid);
            this->m_SelectedItem = proj->treeid;
            cat->wasmodified = true;
            this->MarkModified();
            break;
        case wxID_DELETE: // Delete an existing project
            proj = this->FindProject(this->m_SelectedItem);
            if (proj != NULL)
            {
                cat->pages.erase(cat->pages.begin() + proj->index);
                this->m_TreeCtrl_Projects->Delete(proj->treeid);
                if (wxFileExists(this->m_WorkingDir + wxString("/projects/") + cat->foldername + wxString("/") + proj->filename + wxString(".html")))
                    wxRemoveFile(this->m_WorkingDir + wxString("/projects/") + cat->foldername + wxString("/") + proj->filename + wxString(".html"));
                if (wxFileExists(this->m_WorkingDir + wxString("/projects/") + cat->foldername + wxString("/markdown/") + proj->filename + wxString(".md")))
                    wxRemoveFile(this->m_WorkingDir + wxString("/projects/") + cat->foldername + wxString("/markdown/") + proj->filename + wxString(".md"));
                delete proj;
                for (void* projptr : cat->pages)
                {
                    proj = (Project*)projptr;
                    proj->index = index++;
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
    FindCategory_Projects
    Finds a project category object using a tree item id
    @param  The tree item ID to find the category of
    @return The category that was found, or NULL
==============================*/

Category* Main::FindCategory_Projects(wxTreeItemId item)
{
    for (Category* cat : this->m_Category_Projects)
        if (cat->treeid == item)
            return cat;
    return NULL;
}


/*==============================
    FindProject
    Finds a project object using a tree item id
    @param  The tree item ID to find the project of
    @return The project that was found, or NULL
==============================*/

Project* Main::FindProject(wxTreeItemId item)
{
    wxTreeItemId cat_id;
    Category* cat_elem;

    if (treeitem_iscategory(this->m_TreeCtrl_Projects, item))
        return NULL;

    cat_id = this->m_TreeCtrl_Projects->GetItemParent(item);
    cat_elem = this->FindCategory_Projects(cat_id);
    if (cat_elem == NULL)
        return NULL;

    for (void* proj : cat_elem->pages)
        if (((Project*)proj)->treeid == item)
            return (Project*)proj;
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
            delete (Project*)child;
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
    LoadProjects
    Load all projects stored in the JSON file
==============================*/

void Main::LoadProjects()
{
    json_loadprojects(this->m_WorkingDir, &this->m_Category_Projects, this->m_TreeCtrl_Projects);
    this->ShowProjectEditor(false);
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
            Project* proj = (Project*)child;
            proj->treeid = tree->InsertItem(src_elem->treeid, proj->index, proj->displayname);
            proj->category->wasmodified = true;
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
    EndDrag_Project
    Handle the drag end event for projects properly
    @param The generated event
==============================*/

void Main::EndDrag_Project(wxTreeEvent& event)
{
    wxTreeItemId src = this->m_DraggedItem;
    wxTreeItemId dest = event.GetItem();
    this->m_DraggedItem = NULL;

    // Ensure we have a valid destination node
    if (!dest.IsOk() || src == dest)
        return;

    // Ignore placing the source directly above the destination, since it won't move at all.
    if (this->m_TreeCtrl_Projects->GetPrevSibling(src) == dest)
        return;

    // Only work on project nodes
    if (!treeitem_iscategory(this->m_TreeCtrl_Projects, dest))
    {
        Category* cat;
        std::vector<void*>* pages;
        int index = 0;
        Project* proj_src = this->FindProject(src);
        Project* proj_dest = this->FindProject(dest);

        // Ensure a valid source and destination
        if (proj_src == NULL || proj_dest == NULL)
            return;

        // Ensure both projects are in the same category
        cat = proj_src->category;
        pages = &cat->pages;
        if (cat != proj_dest->category)
            return;

        // Move the elements
        if (proj_dest->index < proj_src->index)
        {
            pages->erase(pages->begin() + proj_src->index);
            pages->insert(pages->begin() + proj_dest->index + 1, proj_src);
        }
        else
        {
            pages->erase(pages->begin() + proj_src->index);
            pages->insert(pages->begin() + proj_dest->index, proj_src);
        }
        this->m_TreeCtrl_Projects->Delete(src);
        proj_src->treeid = this->m_TreeCtrl_Projects->InsertItem(cat->treeid, dest, proj_src->displayname);

        // Correct the index values
        for (void* projptr : *pages)
        {
            Project* proj = (Project*)projptr;
            proj->index = index++;
        }

        // Mark the project and its category as modified
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
    json_save(this->m_WorkingDir, &this->m_Category_Projects);

    // Compile the website
    this->CompileProjects();
    this->CompileTags();
    this->CompileHomePage();

    // Mark the program as no longer modified
    this->MarkModified(false);
}


/*==============================
    CompileProjects
    Compile all project-related pages
==============================*/

void Main::CompileProjects()
{
    this->CompileProjects_List();

    // Now create the page for each project
    for (Category* cat : this->m_Category_Projects)
        for (void* page : cat->pages)
            this->CompileProjects_Project((Project*)page);
}


/*==============================
    CompileProjects_List
    Compile the project list page
==============================*/

void Main::CompileProjects_List()
{
    bool modified = false;
    wxTextFile out(this->m_WorkingDir + wxString("/projects.html"));
    wxDateTime today = wxDateTime::Today();
    wxString html_final = wxString("");
    wxString html_categories = wxString("");

    // Check if a category was modified, if not, then skip page generation
    if (!this->m_FullCompile)
    {
        for (Category* cat : this->m_Category_Projects)
        {
            if (cat->wasmodified)
            {
                modified = true;
                break;
            }
        }
        if (!modified)
            return;
    }

    // Read the projects page template
    html_final = string_fromfile(this->m_WorkingDir + "/templates/projects.html");

    // Generate the page from the section + project templates
    for (Category* cat : this->m_Category_Projects)
    {
        wxString html_projects = wxString("");
        wxString relativepath = wxString("projects/") + cat->foldername + wxString("/");
        wxString desccopy = cat->description;
        const char* mdstr = md_sanitize(&desccopy)->mb_str();
        wxString desc;

        // If there's no projects in this category, skip it
        if (cat->pages.size() == 0)
            continue;

        // Generate the project blocks
        for (void* page : cat->pages)
        {
            Project* proj = (Project*)page;
            html_projects += string_fromfile(this->m_WorkingDir + "/templates/projects_project.html");
            html_projects.Replace("_TEMPLATE_PROJECT_URL_", relativepath + proj->filename + wxString(".html"));
            html_projects.Replace("_TEMPLATE_PROJECT_TITLE_", proj->displayname);
            html_projects.Replace("_TEMPLATE_PROJECT_IMAGE_", relativepath + proj->icon);
            html_projects.Replace("_TEMPLATE_PROJECT_TOOLTIP_", proj->tooltip);
            html_projects.Append("\r\n");
        }

        // Generate the section blocks
        html_categories += string_fromfile(this->m_WorkingDir + "/templates/projects_section.html");
        html_categories.Replace("_TEMPLATE_TITLE_", cat->displayname);
        html_categories.Replace("_TEMPLATE_HREF_", cat->foldername);
        md_html(mdstr, strlen(mdstr), md4c_funcptr_handlestr, &desc, MD_FLAG_NOHTMLBLOCKS | MD_FLAG_HEADINGAUTOID, 0, new MD_TOC_OPTIONS{.depth=5, .toc_placeholder="[TOC]"});
        if (desc.Length() == 0 && cat->description.Length() > 0)
        {
            wxMessageDialog dialog(this, wxString("Markdown parsing was unsuccessful for '") + cat->displayname + wxString("'! Do you have some unsupported unicode?"), wxString("MD Generation Failure"), wxICON_EXCLAMATION);
            dialog.ShowModal();
        }
        desc.Replace("<p>", "<p align=\"left\">");
        html_categories.Replace("_TEMPLATE_SECTION_DESCRIPTION_", *md_unsanitize(&desc));
        html_categories.Replace("_TEMPLATE_PROJECT_LIST_", html_projects);
        html_categories.Append("\r\n");

        // Mark the category as unmodified
        cat->wasmodified = false;
    }

    // Finalize the projects page
    html_final.Replace("_TEMPLATE_PROJECTS_LIST_", html_categories);
    html_final.Replace("_TEMPLATE_PROJECTS_DATE_", today.Format("%b %d %Y"));

    // Dump the html for the projects page
    if (!out.Exists())
        out.Create();
    out.Open();
    out.Clear();
    out.AddLine(html_final);
    out.Write();
    out.Close();
}


/*==============================
    CompileProjects_Project
    Compile a project page
    @param The project page to compile
==============================*/

void Main::CompileProjects_Project(Project* proj)
{
    wxString html_final = wxString("");
    Category* cat = proj->category;
    wxString relativepath = wxString("projects/") + cat->foldername + wxString("/");
    wxString projoutpath = this->m_WorkingDir + wxString("/") + relativepath + proj->filename + wxString(".html");
    wxTextFile projout(projoutpath);
    wxString html_md = wxString(""), pagetags = wxString("");
    wxString desccopy = proj->description;
    const char* mdstr = md_sanitize(&desccopy)->mb_str();
    std::vector<wxString> images;
    std::vector<wxString> youtubes;
    std::vector<wxString> mp4s;

    // Skip this project if it wasn't modified
    if (!this->m_FullCompile && !proj->wasmodified)
        return;

    // Replace most of the basic page info
    html_final = string_fromfile(this->m_WorkingDir + "/templates/project.html");
    html_final.Replace("_TEMPLATE_PROJECTS_TITLE_", proj->displayname);
    html_final.Replace("_TEMPLATE_PROJECTS_DATE_", proj->date);
    md_html(mdstr, strlen(mdstr), md4c_funcptr_handlestr, &html_md, MD_FLAG_NOHTMLBLOCKS | MD_FLAG_HEADINGAUTOID, 0, new MD_TOC_OPTIONS{.depth=5, .toc_placeholder="[TOC]"});
    if (html_md.Length() == 0 && proj->description.Length() > 0)
    {
        printf("Hello\n%s\n", mdstr);
        wxMessageDialog dialog(this, wxString("Markdown parsing was unsuccessful for '") + proj->displayname + wxString("'! Do you have some unsupported unicode?"), wxString("MD Generation Failure"), wxICON_EXCLAMATION);
        dialog.ShowModal();
    }
    html_final.Replace("_TEMPLATE_PROJECTS_DESCRIPTION_", *md_unsanitize(&html_md));
    html_final.Replace("_TEMPLATE_PROJECTS_CATEGORY_", proj->category->foldername);
    html_final.Replace("_TEMPLATE_PROJECT_URL_", relativepath + proj->filename + wxString(".html"));
    html_final.Replace("_TEMPLATE_PROJECT_IMAGE_", relativepath + proj->icon);
    html_final.Replace("_TEMPLATE_PROJECT_TOOLTIP_", proj->tooltip);

    // Handle image carousel
    for (wxString str : proj->images)
    {
        str.Replace("\"", wxString(""));
        if (str.Contains("youtube.com"))
            youtubes.push_back(str.AfterFirst('=').Left(11));
        else if (str.Contains("youtu.be"))
            youtubes.push_back(str.Mid(str.Find('/', true), 11));
        else if (str.Contains(".mp4"))
            mp4s.push_back(str);
        else
            images.push_back(str);
    }
    if (images.size() > 0 || youtubes.size() > 0 || mp4s.size() > 0)
    {
        int i;
        wxString carousel = string_fromfile(this->m_WorkingDir + "/templates/project_carousel.html");
        wxString carousel_object = wxString("");
        wxString carousel_list = wxString("");
        if (images.size() > 0)
        {
            carousel_object += string_fromfile(this->m_WorkingDir + "/templates/project_carousel_object_img.html");
            carousel_object.Replace("_TEMPLATE_PROJECTS_CAROUSEL_IMAGE_", images[0]);
        }
        if (youtubes.size() > 0)
        {
            carousel_object += string_fromfile(this->m_WorkingDir + "/templates/project_carousel_object_youtube.html");
            carousel_object.Replace("_TEMPLATE_PROJECTS_CAROUSEL_YOUTUBE_", youtubes[0]);
            if (images.size() == 0)
                carousel_object.Replace("_TEMPLATE_PROJECTS_CAROUSEL_SHOULDDISPLAYYOUTUBE_", "");
            else
                carousel_object.Replace("_TEMPLATE_PROJECTS_CAROUSEL_SHOULDDISPLAYYOUTUBE_", "display:none");
        }
        if (mp4s.size() > 0)
        {
            carousel_object += string_fromfile(this->m_WorkingDir + "/templates/project_carousel_object_mp4.html");
            carousel_object.Replace("_TEMPLATE_PROJECTS_CAROUSEL_MP4_", mp4s[0]);
            if (images.size() == 0 && youtubes.size() == 0)
                carousel_object.Replace("_TEMPLATE_PROJECTS_CAROUSEL_SHOULDDISPLAYMP4_", "");
            else
                carousel_object.Replace("_TEMPLATE_PROJECTS_CAROUSEL_SHOULDDISPLAYMP4_", "display:none");
        }
        i=0;
        for (wxString str : mp4s)
        {
            wxString obj = string_fromfile(this->m_WorkingDir + "/templates/project_carousel_list_mp4.html");
            wxString thumb = str;
            if (i == 0 && images.size() == 0 && youtubes.size() == 0)
                obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_LIST_SELECTED_", "selected");
            else
                obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_LIST_SELECTED_", "");
            thumb.Replace(".mp4", ".jpg");
            obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_MP4THUMB_", thumb);
            obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_MP4_", str);
            carousel_list += obj;
            i++;
        }
        i=0;
        for (wxString str : youtubes)
        {
            wxString obj = string_fromfile(this->m_WorkingDir + "/templates/project_carousel_list_youtube.html");
            if (i == 0 && images.size() == 0)
                obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_LIST_SELECTED_", "selected");
            else
                obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_LIST_SELECTED_", "");
            obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_YOUTUBE_", str);
            carousel_list += obj;
            i++;
        }
        i=0;
        for (wxString str : images)
        {
            wxString obj = string_fromfile(this->m_WorkingDir + "/templates/project_carousel_list_img.html");
            if (i == 0)
                obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_LIST_SELECTED_", "selected");
            else
                obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_LIST_SELECTED_", "");
            obj.Replace("_TEMPLATE_PROJECTS_CAROUSEL_IMAGE_", str);
            carousel_list += obj;
            i++;
        }
        carousel.Replace("_TEMPLATE_PROJECTS_CAROUSEL_OBJECTS_", carousel_object);
        carousel.Replace("_TEMPLATE_PROJECTS_CAROUSEL_LIST_", carousel_list);
        html_final.Replace("_TEMPLATE_PROJECTS_IMAGES_", carousel);
    }
    else
        html_final.Replace("_TEMPLATE_PROJECTS_IMAGES_", wxString(""));

    // Handle page tags
    for (wxString tag : proj->tags)
    {
        wxString str = string_fromfile(this->m_WorkingDir + "/templates/project_tag.html");
        str.Replace("_TEMPLATE_TAG_URL_", wxString("../../tags/") + sanitize_tagname(tag) + wxString(".html"));
        str.Replace("_TEMPLATE_TAG_NAME_", tag);
        pagetags += str;
        if (tag != proj->tags.back())
            pagetags += wxString(", ");
    }
    html_final.Replace("_TEMPLATE_PROJECTS_TAGS_", pagetags);

    // Handle page URLS
    if (proj->urls.size() > 0)
    {
        wxString url_list = wxString("");
        for (wxString str : proj->urls)
        {
            wxString obj = string_fromfile(this->m_WorkingDir + "/templates/project_url.html");
            str.Replace("\"", wxString(""));
            if (str.Contains("github.com"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadgh.png");
            else if (str.Contains("gitlab.com"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadgl.png");
            else if (str.Contains("store.steampowered.com"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadst.png");
            else if (str.Contains("steamcommunity.com"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadsw.png");
            else if (str.Contains("youtube.com") || str.Contains("youtu.be"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadyt.png");
            else if (str.Contains("knockout.chat"))
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadko.png");
            else
                obj.Replace("_TEMPLATE_PROJECTS_URL_BADGE_", "../downloadzp.png");
            obj.Replace("_TEMPLATE_PROJECTS_URL_", str);
            url_list += obj;
        }
        html_final.Replace("_TEMPLATE_PROJECTS_LINKS_", url_list);
    }
    else
        html_final.Replace("_TEMPLATE_PROJECTS_LINKS_", wxString(""));

    // Generate the page itself
    if (!projout.Exists())
        projout.Create();
    projout.Open();
    projout.Clear();
    projout.AddLine(html_final);
    projout.Write();
    projout.Close();

    // Mark the project as no longer modified
    proj->wasmodified = false;
}


/*==============================
    CompileTags
    Compile the tag pages
==============================*/

void Main::CompileTags()
{
    bool cont;
    wxString filename;
    wxString tagspath = this->m_WorkingDir + wxString("/tags");
    wxDir tagsdir(tagspath);
    std::map<wxString, std::vector<TaggedPage>> tagmap;

    // Go through all projects and add each one to the tag map
    for (Category* cat : this->m_Category_Projects)
    {
        for (void* page : cat->pages)
        {
            for (wxString tag : ((Project*)page)->tags)
            {
                std::map<wxString, std::vector<TaggedPage>>::iterator foundtag = tagmap.find(tag);
                if (foundtag == tagmap.end())
                    foundtag = tagmap.insert(std::pair<wxString, std::vector<TaggedPage>>(tag, std::vector<TaggedPage>())).first;
                foundtag->second.push_back({PAGETYPE_PROJECT, page});
            }
        }
    }

    // Purge the tags folder of html files
    cont = tagsdir.GetFirst(&filename, "*.html");
    while (cont)
    {
        wxRemoveFile(tagspath + wxString("/") + filename);
        cont = tagsdir.GetNext(&filename);
    }

    // Now, alphabetically sort all the pages in the tags, and make their respective pages
    for (std::pair<wxString, std::vector<TaggedPage>> it : tagmap)
    {
        wxString tagname_page;
        wxString pagelist;
        wxString html_final = string_fromfile(this->m_WorkingDir + "/templates/tag.html");
        std::sort(it.second.begin(), it.second.end(), taggedpage_sorter);

        // For a valid tag, its name needs to be sanitized
        tagname_page = sanitize_tagname(it.first);

        // Fill the tag page
        html_final.Replace("_TEMPLATE_TAG_NAME_", it.first);
        html_final.Replace("_TEMPLATE_TAG_CLEANNAME_", wxString("tags/") + tagname_page + wxString(".html"));
        for (TaggedPage tp : it.second)
        {
            wxString item = string_fromfile(this->m_WorkingDir + "/templates/tag_item.html");
            Project* proj = (Project*)tp.page;
            item.Replace("_TEMPLATE_TAG_PAGENAME_", proj->displayname);
            item.Replace("_TEMPLATE_TAG_PAGEURL_", wxString("../projects/") + proj->category->foldername + wxString("/") + proj->filename + wxString(".html"));
            pagelist += item + wxString("\r\n");
        }
        html_final.Replace("_TEMPLATE_TAG_PAGELIST_", pagelist);

        // Generate the page itself
        wxTextFile out(tagspath + wxString("/") + tagname_page + wxString(".html"));
        if (!out.Exists())
            out.Create();
        out.Open();
        out.Clear();
        out.AddLine(html_final);
        out.Write();
        out.Close();
    }
}


/*==============================
    CompileTags
    Compile the homepage
==============================*/

void Main::CompileHomePage()
{
    int count;
    wxString homepath = this->m_WorkingDir + wxString("/index.html");
    wxTextFile fileout(homepath);
    wxString html_entry;
    wxString html_final = string_fromfile(this->m_WorkingDir + "/templates/index.html");
    std::vector<Project*> latestprojects;

    // Get the list of projects with dates
    for (Category* cat : this->m_Category_Projects)
    {
        for (void* page : cat->pages)
        {
            Project* proj = (Project*)page;
            if (proj->date != "")
                latestprojects.push_back(proj);
        }
    }    

    // Paste the 5 most recent projects
    count = 0;
    html_entry = wxString("");
    std::sort(latestprojects.begin(), latestprojects.end(), &project_sorter_date);
    for (Project* proj : latestprojects)
    {
        wxDateTime date;
        wxString tableelem;
        if (count == 5)
            break;
        tableelem = string_fromfile(this->m_WorkingDir + "/templates/index_latest.html");
        if (date_fromstring(proj->date, &date))
            tableelem.Replace("_TEMPLATE_LATEST_ENTRY_DATE_", date.Format("%d/%m/%Y"));
        else
            tableelem.Replace("_TEMPLATE_LATEST_ENTRY_DATE_", "??/??/????");
        tableelem.Replace("_TEMPLATE_LATEST_ENTRY_CATEGORY_", proj->category->displayname);
        tableelem.Replace("_TEMPLATE_LATEST_ENTRY_NAME_", proj->displayname);
        tableelem.Replace("_TEMPLATE_ENTRY_CATEGORY_", wxString("projects.html#") + proj->category->foldername);
        tableelem.Replace("_TEMPLATE_ENTRY_PAGE_", wxString("projects/") + proj->category->foldername + wxString("/") + proj->filename + wxString(".html"));
        html_entry += tableelem + wxString("\r\n");
        count++;
    }
    html_final.Replace("_TEMPLATE_LATEST_PROJECTS_", html_entry);

    // Generate the page itself
    if (!fileout.Exists())
        fileout.Create();
    fileout.Open();
    fileout.Clear();
    fileout.AddLine(html_final);
    fileout.Write();
    fileout.Close();
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
    ShowProjectEditor
    Show or hide the project editor panel
    @param Whether to show the editor panel 
==============================*/

void Main::ShowProjectEditor(bool show)
{
    if (show)
    {
        this->m_ScrolledWindow_Project_Editor->Show();
        this->m_ScrolledWindow_ProjectCategory_Editor->Hide();
        this->m_Button_Projects_Preview->Show();
    }
    else
    {
        this->m_ScrolledWindow_Project_Editor->Hide();
        this->m_ScrolledWindow_ProjectCategory_Editor->Hide();
        this->m_Button_Projects_Preview->Hide();
    }
    this->m_Panel_Projects_Editor->Layout();
}


/*==============================
    ShowProjectCategoryEditor
    Show or hide the project category editor panel
    @param Whether to show the editor panel 
==============================*/

void Main::ShowProjectCategoryEditor(bool show)
{
    if (show)
    {
        this->m_ScrolledWindow_Project_Editor->Hide();
        this->m_ScrolledWindow_ProjectCategory_Editor->Show();
        this->m_Button_Projects_Preview->Show();
    }
    else
    {
        this->m_ScrolledWindow_Project_Editor->Hide();
        this->m_ScrolledWindow_ProjectCategory_Editor->Hide();
        this->m_Button_Projects_Preview->Hide();
    }
    this->m_Panel_Projects_Editor->Layout();
}