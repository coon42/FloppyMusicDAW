#ifndef _TRANSPORT
#define _TRANSPORT

#include <wx/wx.h>

#include "song.h"

//-------------------------------------------------------------------------------------------------
// TransportWindow
//-------------------------------------------------------------------------------------------------

class TransportWindow : public wxWindow {
public:
  TransportWindow(wxWindow* pParent, Song* pSong);

  void update();

private:
  wxSizer* pTopSizer_{nullptr};
  wxStaticText* pTotalTime_{nullptr};
  Song* const pSong_;
};

#endif // _TRANSPORT
