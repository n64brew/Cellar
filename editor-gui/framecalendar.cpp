/***************************************************************
                       framecalendar.cpp

Frame for a calendar selector
***************************************************************/

#include "framecalendar.h"
#include "main.h"


/*==============================
    Main (Constructor)
    Initializes the class
==============================*/

FrameCalendar::FrameCalendar(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxFlexGridSizer* m_Sizer_Main;
    m_Sizer_Main = new wxFlexGridSizer(0, 0, 0, 0);
    m_Sizer_Main->AddGrowableCol(0);
    m_Sizer_Main->AddGrowableRow(0);
    m_Sizer_Main->SetFlexibleDirection(wxBOTH);
    m_Sizer_Main->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    this->m_Calendar = new wxCalendarCtrl(this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxCAL_MONDAY_FIRST|wxCAL_SHOW_HOLIDAYS);
    m_Sizer_Main->Add(this->m_Calendar, 0, wxALL|wxEXPAND, 5);
    this->SetSizer(m_Sizer_Main);
    this->Layout();
    this->Centre(wxBOTH);

    // Connect Events
    this->m_Calendar->Connect(wxEVT_CALENDAR_DOUBLECLICKED, wxCalendarEventHandler(FrameCalendar::m_Calendar_OnCalendar), NULL, this);
}


/*==============================
    App (Destructor)
    Cleans up the class before deletion
==============================*/

FrameCalendar::~FrameCalendar()
{
    // Disconnect Events
    this->m_Calendar->Disconnect(wxEVT_CALENDAR_DOUBLECLICKED, wxCalendarEventHandler(FrameCalendar::m_Calendar_OnCalendar), NULL, this);
}


/*==============================
    m_Splitter_ROMsOnIdle
    Handles the rom splitter event
    @param Unused
==============================*/

void FrameCalendar::m_Calendar_OnCalendar(wxCalendarEvent& event)
{
    wxDateTime date = this->m_Calendar->GetDate();
    date.MakeUTC();
    time_t utc = date.GetTicks();
    ((Main*)(this->GetParent()))->UpdateROMDate(utc);
    this->Close();
}


/*==============================
    SetDate
    Sets the initial date on the calendar
    @param The date to use in UTC time
==============================*/

void FrameCalendar::SetDate(time_t date)
{
    wxDateTime utcdate(date != 0 ? date : wxDateTime::Now().GetTicks());
    int year = utcdate.GetYear(wxDateTime::UTC);
    wxDateTime::Month month = utcdate.GetMonth(wxDateTime::UTC);
    int day = utcdate.GetDay(wxDateTime::UTC);
    wxDateTime finaldate(day, month, year);
    this->m_Calendar->SetDate(finaldate);
}