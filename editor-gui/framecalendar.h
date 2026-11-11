#pragma once

typedef struct IUnknown IUnknown;

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/calctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <cstdint>

class FrameCalendar : public wxFrame
{
    private:

    protected:
        wxCalendarCtrl* m_Calendar;

        virtual void m_Calendar_OnCalendar(wxCalendarEvent& event);


    public:
        FrameCalendar(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Calendar"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(640, 480), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
        ~FrameCalendar();

        void SetDate(time_t date);

};