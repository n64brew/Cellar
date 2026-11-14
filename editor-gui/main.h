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
#include <wx/combobox.h>
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
#include <cstdint>


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

typedef struct Author_s
{
    wxString filename;
    wxString oldfilename;
    wxString displayname;
    wxString image;
    wxString description;
    wxTreeItemId treeid;
} Author;

// Helper struct for sorting tagged pages
typedef struct TaggedPage_s
{
    PageType type;
    void* page;
} TaggedPage;

// Category structure
typedef struct Category_s
{
    int index;
    wxString foldername;
    wxString displayname;
    wxString description;
    bool wasmodified;
    wxTreeItemId treeid;
    std::vector<void*> pages;
} Category;

// ROM page structure
typedef struct ROM_s
{
    int index;
    wxString filename;
    wxString displayname;
    wxString icon;
    time_t date;
    wxString tooltip;
    wxString agerating;
    wxString contentwarning;
    wxString aidisclosure;
    wxString description;
    std::vector<wxString> images;
    std::vector<wxString> urls;
    std::vector<wxString> tags;
    std::vector<wxString> authors;
    std::vector<wxString> genres;
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
        std::vector<Author*> m_Authors;
        std::vector<wxString> m_ObsoleteAuthorFiles;
        wxString m_WorkingDir;
        wxTreeItemId m_DraggedItem;
        wxTreeItemId m_SelectedItem;
        wxTreeItemId m_SelectedAuthor;

    protected:
        wxChoicebook* m_ChoiceBook_PageSelection;
        wxPanel* m_Panel_ROMs;
        wxSplitterWindow* m_Splitter_ROMs;
        wxPanel* m_Panel_ROMs_Tree;
        wxSearchCtrl* m_SearchCtrl_ROMs;
        wxTreeCtrl* m_TreeCtrl_ROMs;
        wxPanel* m_Panel_ROMs_Editor;
        wxScrolledWindow* m_ScrolledWindow_ROM_Editor;
        wxTextCtrl* m_TextCtrl_ROMs_File;
        wxTextCtrl* m_TextCtrl_ROMs_Name;
        wxTextCtrl* m_TextCtrl_ROMs_Icon;
        wxBitmapButton* m_Button_ROMs_FindAuthors;
        wxTextCtrl* m_TextCtrl_ROMs_Authors;
        wxBitmapButton* m_Button_ROMs_Date;
        wxTextCtrl* m_TextCtrl_ROMs_Date;
        wxBitmapButton* m_Button_ROMs_Tags;
        wxTextCtrl* m_TextCtrl_ROMs_Tags;
        wxTextCtrl* m_TextCtrl_ROMs_Images;
        wxTextCtrl* m_TextCtrl_ROMs_URLs;
        wxTextCtrl* m_TextCtrl_ROMs_ToolTip;
        wxComboBox* m_ComboBox_ROMs_AgeRating;
        wxTextCtrl* m_TextCtrl_ROMs_Genres;
        wxTextCtrl* m_TextCtrl_ROMs_ContentWarning;
        wxTextCtrl* m_TextCtrl_ROMs_AIDisclosure;
        wxTextCtrl* m_TextCtrl_ROMs_Description;
        wxScrolledWindow* m_ScrolledWindow_ROMCategory_Editor;
        wxTextCtrl* m_TextCtrl_ROMsCategory_Folder;
        wxTextCtrl* m_TextCtrl_ROMsCategory_DisplayName;
        wxTextCtrl* m_TextCtrl_ROMsCategory_Description;
        wxButton* m_Button_ROMs_Preview;
        wxPanel* m_Panel_Authors;
        wxSplitterWindow* m_Splitter_Authors;
        wxPanel* m_Panel_Authors_Tree;
        wxSearchCtrl* m_SearchCtrl_Authors;
        wxTreeCtrl* m_TreeCtrl_Authors;
        wxPanel* m_Panel_Authors_Editor;
        wxScrolledWindow* m_ScrolledWindow_Author_Editor;
        wxTextCtrl* m_TextCtrl_Authors_File;
        wxTextCtrl* m_TextCtrl_Authors_Name;
        wxTextCtrl* m_TextCtrl_Authors_Image;
        wxTextCtrl* m_TextCtrl_Authors_Description;
        wxButton* m_Button_Authors_Preview;
        wxMenuBar* m_Menubar_Main;
        wxMenu* m_Menu_File;
        wxMenu* m_Menu_Edit;
        wxTimer* m_Timer;
        
        void m_Splitter_ROMsOnIdle(wxIdleEvent& event);
        void m_Splitter_AuthorsOnIdle(wxIdleEvent& event);
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
        void m_Button_ROMs_FindAuthors_OnButton(wxCommandEvent& event);
        void m_Button_ROMs_Tags_OnButton(wxCommandEvent& event);
        void m_TextCtrl_ROMs_Images_OnText(wxCommandEvent& event);
        void m_Button_ROMs_Date_OnButton(wxCommandEvent& event);
        void m_TextCtrl_ROMs_URLs_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMs_ToolTip_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMs_Description_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMsCategory_Folder_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMsCategory_DisplayName_OnText(wxCommandEvent& event);
        void m_ComboBox_ROMs_AgeRating_OnCombobox(wxCommandEvent& event);
        void m_TextCtrl_ROMs_Genres_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMs_ContentWarning_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMs_AIDisclosure_OnText(wxCommandEvent& event);
        void m_TextCtrl_ROMsCategory_Description_OnText(wxCommandEvent& event);
        void m_Button_ROMs_Preview_OnButtonClick(wxCommandEvent& event);
        void m_SearchCtrl_AuthorsOnText(wxCommandEvent& event);
        void m_TreeCtrl_Authors_OnTreeEndLabelEdit(wxTreeEvent& event);
        void m_TreeCtrl_Authors_OnTreeItemMenu(wxTreeEvent& event);
        void m_TreeCtrl_Authors_OnTreeSelChanged(wxTreeEvent& event);
        void m_TextCtrl_Authors_Name_OnText(wxCommandEvent& event);
        void m_TextCtrl_Authors_Image_OnText(wxCommandEvent& event);
        void m_TextCtrl_Authors_Description_OnText(wxCommandEvent& event);
        void m_Button_Authors_Preview_OnButtonClick(wxCommandEvent& event);
        void m_MenuItem_Save_OnMenuSelection(wxCommandEvent& event);
        void m_MenuItem_FullRecompile_OnMenuSelection(wxCommandEvent& event);
        void m_MenuItem_Find_OnMenuSelection(wxCommandEvent& event);
        void m_Timer_OnTimer(wxTimerEvent& event);
        void OnPopupClick_Categories(wxCommandEvent& event);
        void OnPopupClick_ROMs(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
    
    public:
        Main(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(800, 700), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
        ~Main();

        void InitialCheck();
        Category* FindCategory_ROMs(wxTreeItemId item);
        Category* FindCategory_Blog(wxTreeItemId item);
        ROM* FindROM(wxTreeItemId item);
        Author* FindAuthor(wxTreeItemId item);
        void RefreshAuthorTree();
        bool RenameAuthor(Author* author, wxString name);
        void OnPopupClick_Authors(wxCommandEvent& event);
        void UpdateTree(wxTreeCtrl* tree, wxString folder, std::vector<Category*>* categorylist);
        void LoadROMs();
        void EndDrag(wxTreeEvent& event, wxTreeCtrl* tree, std::vector<Category*>* categorylist);
        void EndDrag_ROM(wxTreeEvent& event);
        bool Save();
        void MarkModified(bool modified=true);
        void ShowROMEditor(bool show=true);
        void ShowROMCategoryEditor(bool show=true);
        void UpdateROMTags(wxString tags);
};