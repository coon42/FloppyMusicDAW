#ifndef _TRACKEDITOR
#define _TRACKEDITOR

#include <wx/wx.h>
#include <wx/grid.h>

#include "song.h"

//-------------------------------------------------------------------------------------------------
// TrackEditorWindow
//-------------------------------------------------------------------------------------------------

class TrackEditorWindow : public wxWindow {
public:
  TrackEditorWindow(wxWindow* pParent, Song* pSong);

  void updateTrackList();

private:
  wxSizer* pTopSizer_{nullptr};
  wxGrid* pTrackListGrid_{nullptr};
  Song* const pSong_;

  void adjustTrackPreviewSize();
  void OnTrackListGridDoubleClick(wxGridEvent& event);

  wxDECLARE_EVENT_TABLE();
};

#endif // _TRACKEDITOR
