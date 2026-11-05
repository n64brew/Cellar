/***************************************************************
                            app.cpp

This file handles the wxWidgets initialization.
***************************************************************/

#include <wx/socket.h>
#include "app.h"

// Icons
#include "resources/icon_prog.h"

wxIMPLEMENT_APP(App);

wxIcon   iconbm_prog = wxNullIcon;

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
	wxBitmap temp = wxBITMAP_PNG_FROM_DATA(icon_prog);
	iconbm_prog.CopyFromBitmap(temp);

    // Show the main window
    this->m_Frame = new Main(NULL);
	this->m_Frame->SetIcon(iconbm_prog);
    this->m_Frame->Show();
    return true;
}