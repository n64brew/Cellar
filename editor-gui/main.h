#pragma once

typedef struct IUnknown IUnknown;

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/srchctrl.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/bmpbuttn.h>
#include <wx/scrolwin.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/splitter.h>
#include <wx/choicebk.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/filefn.h>
#include <wx/timer.h>


/*********************************
           Enumerations
*********************************/

// Helper enum for sorting tagged pages
typedef enum
{
    PAGETYPE_ROM,
} PageType;


/*********************************
            Structures
*********************************/

typedef struct Category_s Category;
typedef struct ROM_s ROM;

// Helper struct for sorting tagged pages
typedef struct TaggedPage_s
{
    PageType type;
    void* page;
} TaggedPage;

// Category structure
typedef struct Category_s {
	int index;
	wxString foldername;
	wxString displayname;
    wxString description;
    bool wasmodified;
	wxTreeItemId treeid;
	std::vector<void*> pages;
} Category;

// ROM page structure
typedef struct ROM_s {
	int index;
	wxString filename;
	wxString displayname;
    wxString icon;
    wxString date;
    wxString tooltip;
    wxString description;
    std::vector<wxString> images;
	std::vector<wxString> urls;
    std::vector<wxString> tags;
    bool wasmodified;
	Category* category;
    wxTreeItemId treeid;
} ROM;


/*********************************
             Classes
*********************************/

class Main : public wxFrame
{
	private:
        bool m_Modified;
        std::vector<Category*> m_Category_ROMs;
		wxString m_WorkingDir;
		wxTreeItemId m_DraggedItem;
        wxTreeItemId m_SelectedItem;

	protected:
        wxChoicebook* m_ChoiceBook_PageSelection;
        wxPanel* m_Panel_ROMs;
        wxSplitterWindow* m_Splitter_ROMs;
        wxPanel* m_Panel_ROMs_Tree;
        wxSearchCtrl* m_SearchCtrl_ROMs;
        wxTreeCtrl* m_TreeCtrl_ROMs;
        wxPanel* m_Panel_ROMs_Editor;
        wxScrolledWindow* m_ScrolledWindow_ROM_Editor;
        wxStaticText* m_Label_ROMs_File;
        wxTextCtrl* m_TextCtrl_ROMs_File;
        wxStaticText* m_Label_ROMs_Name;
        wxTextCtrl* m_TextCtrl_ROMs_Name;
        wxStaticText* m_Label_ROMs_Icon;
        wxTextCtrl* m_TextCtrl_ROMs_Icon;
        wxStaticText* m_Label_ROMs_Tags;
        wxBitmapButton* m_Button_ROMs_Tags;
        wxTextCtrl* m_TextCtrl_ROMs_Tags;
        wxStaticText* m_Label_ROMs_Images;
        wxTextCtrl* m_TextCtrl_ROMs_Images;
        wxStaticText* m_Label_ROMs_Date;
        wxTextCtrl* m_TextCtrl_ROMs_Date;
        wxStaticText* m_Label_ROMs_URLs;
        wxTextCtrl* m_TextCtrl_ROMs_URLs;
        wxStaticText* m_Label_ROMs_ToolTip;
        wxTextCtrl* m_TextCtrl_ROMs_ToolTip;
        wxTextCtrl* m_TextCtrl_ROMs_Description;
        wxScrolledWindow* m_ScrolledWindow_ROMCategory_Editor;
        wxStaticText* m_Label_ROMsCategory_Folder;
        wxTextCtrl* m_TextCtrl_ROMsCategory_Folder;
        wxStaticText* m_Label_ROMsCategory_DisplayName;
        wxTextCtrl* m_TextCtrl_ROMsCategory_DisplayName;
        wxStaticText* m_Label_ROMsCategory_Description;
        wxTextCtrl* m_TextCtrl_ROMsCategory_Description;
        wxButton* m_Button_ROMs_Preview;
        wxMenuBar* m_Menubar_Main;
        wxMenu* m_Menu_File;
        wxMenu* m_Menu_Edit;
        wxTimer* m_Timer;
        
        void m_Splitter_ROMsOnIdle(wxIdleEvent& event);
        void m_SearchCtrl_ROMsOnText(wxCommandEvent& event);
        void m_TreeCtrl_ROMs_OnTreeRightClick(wxMouseEvent& event);
        void m_TreeCtrl_ROMs_OnTreeBeginDrag(wxTreeEvent& event);
        void m_TreeCtrl_ROMs_OnTreeEndDrag(wxTreeEvent& event);
        void m_TreeCtrl_ROMs_OnTreeEndLabelEdit(wxTreeEvent& event);
        void m_TreeCtrl_ROMs_OnTreeItemMenu(wxTreeEvent& event);
        void m_TreeCtrl_ROMs_OnTreeSelChanged(wxTreeEvent& event);
        void m_TextCtrl_ROMs_File_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMs_Name_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMs_Icon_OnText(wxCommandEvent& event);
        void m_Button_ROMs_Tags_OnButton(wxCommandEvent& event);
        void m_TextCtrl_ROMs_Images_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMs_Date_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMs_URLs_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMs_ToolTip_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMs_Description_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMsCategory_Folder_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMsCategory_DisplayName_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMsCategory_Description_OnText(wxCommandEvent& event);
        void m_Button_ROMs_Preview_OnButtonClick(wxCommandEvent& event);
        void m_MenuItem_Save_OnMenuSelection(wxCommandEvent& event);
        void m_MenuItem_FullRecompile_OnMenuSelection(wxCommandEvent& event);
        void m_MenuItem_Find_OnMenuSelection(wxCommandEvent& event);
        void m_Timer_OnTimer(wxTimerEvent& event);
        void OnPopupClick_Categories(wxCommandEvent& event);
        void OnPopupClick_ROMs(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);

	public:
		Main(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(800, 600), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
		~Main();
        void InitialCheck();
        Category* FindCategory_ROMs(wxTreeItemId item);
        Category* FindCategory_Blog(wxTreeItemId item);
        ROM* FindROM(wxTreeItemId item);
		void UpdateTree(wxTreeCtrl* tree, wxString folder, std::vector<Category*>* categorylist);
        void LoadROMs();
        void EndDrag(wxTreeEvent& event, wxTreeCtrl* tree, std::vector<Category*>* categorylist);
        void EndDrag_ROM(wxTreeEvent& event);
		void Save();
        void MarkModified(bool modified=true);
        void ShowROMEditor(bool show=true);
        void ShowROMCategoryEditor(bool show=true);
};