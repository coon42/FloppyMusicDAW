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
  wxGrid* pTrackWindow_{nullptr};
  Song* const pSong_;
};

#endif // _TRACKEDITOR
