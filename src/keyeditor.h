#ifndef _KEY_EDITOR_H
#define _KEY_EDITOR_H

#include <wx/wx.h>

#include "song.h"

class KeyEditorCanvas;

//-------------------------------------------------------------------------------------------------
// KeyEditorCanvasCanvasSegment
//-------------------------------------------------------------------------------------------------

class KeyEditorCanvasSegment : public wxWindow {
public:
  KeyEditorCanvasSegment(KeyEditorCanvas* pParent, const wxSize& size);
  void render();

private:
  void OnPaint(wxPaintEvent& event);
  virtual void onRender(wxDC& dc) = 0;

  wxDECLARE_EVENT_TABLE();
};

//-------------------------------------------------------------------------------------------------
// KeyEditorQuantizationCanvas
//-------------------------------------------------------------------------------------------------

class KeyEditorQuantizationCanvas : public KeyEditorCanvasSegment {
public:
  KeyEditorQuantizationCanvas(KeyEditorCanvas* pParent);

private:
  void onRender(wxDC& dc) final;
};

//-------------------------------------------------------------------------------------------------
// KeyEditorPianoCanvas
//-------------------------------------------------------------------------------------------------

class KeyEditorPianoCanvas : public KeyEditorCanvasSegment {
public:
  KeyEditorPianoCanvas(KeyEditorCanvas* pParent);

private:
  void onRender(wxDC& dc) final;
};

//-------------------------------------------------------------------------------------------------
// KeyEditorGridCanvas
//-------------------------------------------------------------------------------------------------

class KeyEditorGridCanvas : public KeyEditorCanvasSegment {
public:
  KeyEditorGridCanvas(KeyEditorCanvas* pParent, Song* pSong);

  void setXscrollPosition(int xScrollPosition);
  void setYscrollPosition(int yScrollPosition);
  void setXzoomFactor(int xZoomFactor);
  void setYzoomFactor(int yZoomFactor);

private:
  void OnPaint(wxPaintEvent& event);
  void OnMouseMotion(wxMouseEvent& event);
  void OnMouseLeftDown(wxMouseEvent& event);
  virtual void onRender(wxDC& dc) final;

  NoteBlock* currentPointedNoteBlock(int mouseX, int mouseY);

  int xScrollOffset_{0};
  int yScrollOffset_{0};
  int pixelsPerQuarterNote_{10};
  int blockHeight_{10};

  const int xBlockStartOffset_ = 50;
  const int yBlockStartOffset_ = 30;

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
