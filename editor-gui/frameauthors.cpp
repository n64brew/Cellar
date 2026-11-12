/***************************************************************
                       frameauthors.cpp

TODO
***************************************************************/

#include "frameauthors.h"
#include "main.h"


/*==============================
    Main (Constructor)
    Initializes the class
==============================*/

FrameAuthors::FrameAuthors( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxFlexGridSizer* m_Sizer_Main;
    m_Sizer_Main = new wxFlexGridSizer( 0, 1, 0, 0 );
    m_Sizer_Main->AddGrowableCol( 0 );
    m_Sizer_Main->AddGrowableRow( 1 );
    m_Sizer_Main->SetFlexibleDirection( wxBOTH );
    m_Sizer_Main->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxFlexGridSizer* m_Sizer_Top;
    m_Sizer_Top = new wxFlexGridSizer( 0, 2, 0, 0 );
    m_Sizer_Top->AddGrowableCol( 0 );
    m_Sizer_Top->SetFlexibleDirection( wxBOTH );
    m_Sizer_Top->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_SearchCtrl_Authors = new wxSearchCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    #ifndef __WXMAC__
    m_SearchCtrl_Authors->ShowSearchButton( true );
    #endif
    m_SearchCtrl_Authors->ShowCancelButton( false );
    m_Sizer_Top->Add( m_SearchCtrl_Authors, 0, wxALL|wxEXPAND, 5 );

    m_Button_New = new wxButton( this, wxID_ANY, wxT("New Author"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Top->Add( m_Button_New, 0, wxALL, 5 );


    m_Sizer_Main->Add( m_Sizer_Top, 1, wxEXPAND, 5 );

    wxArrayString m_CheckList_AuthorsChoices;
    m_CheckList_Authors = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_CheckList_AuthorsChoices, wxLB_MULTIPLE|wxLB_SORT );
    m_Sizer_Main->Add( m_CheckList_Authors, 0, wxALL|wxEXPAND, 5 );

    m_Button_Ok = new wxButton( this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Sizer_Main->Add( m_Button_Ok, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );


    this->SetSizer( m_Sizer_Main );
    this->Layout();

    this->Centre( wxBOTH );
}

FrameAuthors::~FrameAuthors()
{
}
