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

protected:
  const KeyEditorCanvas* canvas() const;
  KeyEditorCanvas* canvas();

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

private:
  struct CellPosition {
    int absoluteXindex;
    int absoluteYindex;
    int relativeXindex;
    int relativeYindex;
  };

  struct BlockDimensions {
    int x;
    int y;
    int width;
  };

  enum class ResizeArea {
    None,
    Left,
    Right
  };

  enum class EditState {
    Idle,
  } editState_{EditState::Idle};

  void OnPaint(wxPaintEvent& event);
  void OnMouseMotion(wxMouseEvent& event);
  void OnMouseLeftDown(wxMouseEvent& event);
  void OnMouseLeftUp(wxMouseEvent& event);
  virtual void onRender(wxDC& dc) final;

  CellPosition currentPointedCell(int mouseX, int mouseY);
  CellPosition currentPointedCell();
  ResizeArea noteBlockResizeArea(const NoteBlock& noteBlock, int mouseX, int mouseY) const;
  BlockDimensions getAbsoluteNoteBlockDimensions(const NoteBlock& noteBlock) const;
  BlockDimensions getVisibleNoteBlockDimensions(const NoteBlock& noteBlock) const;
  NoteBlock* currentPointedNoteBlock(int mouseX, int mouseY);
  NoteBlock* pCurrentEditNoteBlock_{nullptr};
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

  int xBlockStartOffset() const    { return xBlockStartOffset_; }
  int yBlockStartOffset() const    { return yBlockStartOffset_; }
  int xScrollOffset() const        { return xScrollOffset_; }
  int yScrollOffset() const        { return yScrollOffset_; }
  int pixelsPerQuarterNote() const { return pixelsPerQuarterNote_; }
  int blockHeight() const          { return blockHeight_; }

private:
  KeyEditorQuantizationCanvas* pKeyEditorQuantizationCanvas_{nullptr};
  KeyEditorPianoCanvas* pKeyEditorPianoCanvas_{nullptr};
  KeyEditorGridCanvas* pKeyEditorGridCanvas_{nullptr};

  const int xBlockStartOffset_ = 50;
  const int yBlockStartOffset_ = 30;
  int xScrollOffset_{0};
  int yScrollOffset_{0};
  int pixelsPerQuarterNote_{10};
  int blockHeight_{10};
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
