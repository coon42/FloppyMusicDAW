#ifndef _TRACKEDITOR
#define _TRACKEDITOR

#include <wx/wx.h>
#include <wx/grid.h>

//-------------------------------------------------------------------------------------------------
// TrackEditorWindow
//-------------------------------------------------------------------------------------------------

class TrackEditorWindow : public wxWindow {
public:
  TrackEditorWindow(wxWindow* pParent);

private:
  wxGrid* pTrackWindow_{nullptr};
};

#endif // _TRACKEDITOR
