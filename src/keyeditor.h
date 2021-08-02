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
  KeyEditorGridCanvas(wxWindow* pParent, Song* pSong);
  void render();

  void setXscrollPosition(int xScrollPosition);
  void setYscrollPosition(int yScrollPosition);
  void setXzoomFactor(int xZoomFactor);
  void setYzoomFactor(int yZoomFactor);

private:
  void OnPaint(wxPaintEvent& event);
  void OnMouseMotion(wxMouseEvent& event);
  void OnMouseLeftDown(wxMouseEvent& event);
  void render(wxDC& dc);

  NoteBlock* currentPointedNoteBlock(int mouseX, int mouseY);

  int xScrollOffset_{0};
  int yScrollOffset_{0};
  int pixelsPerQuarterNote_{10};
  int blockHeight_{10};

  const int xBlockStartOffset_ = 50;
  const int yBlockStartOffset_ = 30;
  const int numMidiNotes_ = 128;

  Song* const pSong_;

  wxDECLARE_EVENT_TABLE();
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
  KeyEditorFreeCanvas* pKeyEditorFreeCanvas_{nullptr};
  KeyEditorQuantizationCanvas* pKeyEditorQuantizationCanvas_{nullptr};
  KeyEditorPianoCanvas* pKeyEditorPianoCanvas_{nullptr};
  KeyEditorGridCanvas* pKeyEditorGridCanvas_{nullptr};
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
