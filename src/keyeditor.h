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
  void render();

  void setXscrollPosition(int xScrollPosition);
  void setYscrollPosition(int yScrollPosition);
  void setXzoomFactor(int xZoomFactor);
  void setYzoomFactor(int yZoomFactor);

  enum class ScrollBarType {
    HorizontalScroll,
    VerticalScroll,
    HorizontalZoom,
    VerticalZoom
  };

private:
  void OnPaint(wxPaintEvent& event);
  void render(wxDC& dc);

  Song* const pSong_;

  int xScrollOffset_{0};
  int yScrollOffset_{0};
  int xZoomFactor_{0};
  int yZoomFactor_{0};

  wxDECLARE_EVENT_TABLE();
};

//-------------------------------------------------------------------------------------------------
// KeyEditorWindow
//-------------------------------------------------------------------------------------------------

class KeyEditorWindow : public wxWindow {
public:
  KeyEditorWindow(wxWindow* pParent, Song* pSong);
  void render();

private:
  void OnScroll(wxScrollEvent& event);
  void OnMouseWheel(wxMouseEvent& event);

  KeyEditorCanvas* pKeyEditorCanvas_{nullptr};
  wxScrollBar* pHorizontalScrollbar_{nullptr};
  wxScrollBar* pVerticalScrollbar_{nullptr};

  Song* const pSong_;

  wxDECLARE_EVENT_TABLE();
};

#endif // _KEY_EDITOR_H
