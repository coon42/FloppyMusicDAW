#ifndef _KEY_EDITOR_H
#define _KEY_EDITOR_H

#include <wx/wx.h>

#include "song.h"

//-------------------------------------------------------------------------------------------------
// KeyEditorCanvas
//-------------------------------------------------------------------------------------------------

class KeyEditorCanvas : public wxWindow {
public:
  KeyEditorCanvas(wxWindow* pParent, Song* const pSong);

  void setXscrollPosition(int xScrollPosition);
  void setYscrollPosition(int yScrollPosition);

private:
  void OnPaint(wxPaintEvent& event);
  void render(wxDC& dc);

  Song* const pSong_;

  int xScrollOffset_{0};
  int yScrollOffset_{0};

  wxDECLARE_EVENT_TABLE();
};

//-------------------------------------------------------------------------------------------------
// KeyEditorWindow
//-------------------------------------------------------------------------------------------------

class KeyEditorWindow : public wxWindow {
public:
  KeyEditorWindow(wxWindow* pParent, Song* pSong);

private:
  void OnScroll(wxScrollEvent& event);

  KeyEditorCanvas* pKeyEditorCanvas_{nullptr};
  Song* const pSong_;

  wxDECLARE_EVENT_TABLE();
};

#endif // _KEY_EDITOR_H
