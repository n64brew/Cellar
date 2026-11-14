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
#include <wx/dialog.h>
#include <cstdint>

class FrameCalendar : public wxDialog
{
    private:

    protected:
        wxCalendarCtrl* m_Calendar;

        void m_Calendar_OnCalendar(wxCalendarEvent& event);

    public:
        FrameCalendar(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Calendar"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(640, 480), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
        ~FrameCalendar();

        void SetDate(time_t date);
        wxDateTime GetDate();
};