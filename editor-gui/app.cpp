/***************************************************************
                            app.cpp

This file handles the wxWidgets initialization.
***************************************************************/

#include <wx/socket.h>
#include "app.h"
#include "resource.h"

wxIMPLEMENT_APP(App);

wxIcon iconbm_prog = wxNullIcon;
wxIcon icon_authors = wxNullIcon;
wxIcon icon_calendar = wxNullIcon;
wxIcon icon_tags = wxNullIcon;

/*==============================
    App (Constructor)
    Initializes the class
==============================*/

App::App()
{
    
}


/*==============================
    App (Destructor)
    Cleans up the class before deletion
==============================*/

App::~App()
{
    
}


/*==============================
    App::OnInit
    Called when the application is initialized
    @return Whether the application initialized correctly
==============================*/

bool App::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    // Initialize image handlers and icons
    wxInitAllImageHandlers();
	iconbm_prog.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(icon_prog));
    icon_authors.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(icon_authors));
    icon_calendar.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(icon_calendar));
    icon_tags.CopyFromBitmap(wxBITMAP_PNG_FROM_DATA(icon_tags));

    // Show the main window
    this->m_Frame = new Main(NULL);
	this->m_Frame->SetIcon(iconbm_prog);
    this->m_Frame->Show();
    return true;
}