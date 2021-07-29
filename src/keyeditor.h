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

private:
  void OnPaint(wxPaintEvent& event);
  void render(wxDC& dc);

  Song* const pSong_;

  wxDECLARE_EVENT_TABLE();
};

//-------------------------------------------------------------------------------------------------
// KeyEditorWindow
//-------------------------------------------------------------------------------------------------

class KeyEditorWindow : public wxWindow {
public:
  KeyEditorWindow(wxWindow* pParent, Song* pSong);

private:
  KeyEditorCanvas* pKeyEditorCanvas_{nullptr};
  Song* const pSong_;
};

#endif // _KEY_EDITOR_H
