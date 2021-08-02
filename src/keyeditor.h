#ifndef _KEY_EDITOR_H
#define _KEY_EDITOR_H

#include <wx/wx.h>

#include "song.h"

//-------------------------------------------------------------------------------------------------
// KeyEditorFreeCanvas
//-------------------------------------------------------------------------------------------------

class KeyEditorFreeCanvas : public wxWindow {
public:
  KeyEditorFreeCanvas(wxWindow* pParent);
};

//-------------------------------------------------------------------------------------------------
// KeyEditorQuantizationCanvas
//-------------------------------------------------------------------------------------------------

class KeyEditorQuantizationCanvas : public wxWindow {
public:
  KeyEditorQuantizationCanvas(wxWindow* pParent);
};

//-------------------------------------------------------------------------------------------------
// KeyEditorPianoCanvas
//-------------------------------------------------------------------------------------------------

class KeyEditorPianoCanvas : public wxWindow {
public:
  KeyEditorPianoCanvas(wxWindow* pParent);
};

//-------------------------------------------------------------------------------------------------
// KeyEditorGridCanvas
//-------------------------------------------------------------------------------------------------

class KeyEditorGridCanvas : public wxWindow {
public:
  KeyEditorGridCanvas(wxWindow* pParent);
};

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
  void OnMouseMotion(wxMouseEvent& event);
  void OnMouseLeftDown(wxMouseEvent& event);
  void render(wxDC& dc);

  NoteBlock* currentPointedNoteBlock(int mouseX, int mouseY);

  Song* pSong_;

  int xScrollOffset_{0};
  int yScrollOffset_{0};
  int pixelsPerQuarterNote_{0};
  int blockHeight_{0};

  const int xBlockStartOffset_ = 50;
  const int yBlockStartOffset_ = 30;
  const int numMidiNotes_ = 128;

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
