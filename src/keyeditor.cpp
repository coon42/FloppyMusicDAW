#include <wx/wx.h>

extern "C" {
#include "lib/eMIDI/src/helpers.h"
}

#include "keyeditor.h"

//-------------------------------------------------------------------------------------------------
// KeyEditorCanvasCanvasSegment
//-------------------------------------------------------------------------------------------------

KeyEditorCanvasSegment::KeyEditorCanvasSegment(wxWindow* pParent, const wxSize& size = wxDefaultSize)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, size) {

}

//-------------------------------------------------------------------------------------------------
// KeyEditorQuantizationCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorQuantizationCanvas::KeyEditorQuantizationCanvas(wxWindow* pParent) : KeyEditorCanvasSegment(pParent) {

}

void KeyEditorQuantizationCanvas::render() {

}

//-------------------------------------------------------------------------------------------------
// KeyEditorPianoCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorPianoCanvas::KeyEditorPianoCanvas(wxWindow* pParent) : KeyEditorCanvasSegment(pParent) {

}

void KeyEditorPianoCanvas::render() {

}

//-------------------------------------------------------------------------------------------------
// KeyEditorGridCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorGridCanvas::KeyEditorGridCanvas(wxWindow* pParent, Song* pSong) : KeyEditorCanvasSegment(pParent),
    pSong_(pSong) {

}

void KeyEditorGridCanvas::render() {
  wxClientDC dc(this);
  render(dc);
}

void KeyEditorGridCanvas::render(wxDC& dc) {
  dc.Clear();

  const wxSize canvasSize = GetClientSize();

  // draw divisions
  for (int x = 0; x < canvasSize.GetWidth() / pixelsPerQuarterNote_; ++x) {
    const int xOffset = x * pixelsPerQuarterNote_;
    const bool isFirst = (xScrollOffset_ + x) % 4 == 0;
    const int yEndPos = isFirst ? yBlockStartOffset_ : 10;

    if (isFirst) {
      dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick
      dc.SetTextForeground(wxColor(0, 0, 0)); // set text color
    }
    else {
      dc.SetPen(wxPen(wxColor(192, 192, 192), 1)); // black line, 1 pixels thick
      dc.SetTextForeground(wxColor(128, 128, 128)); // set text color
    }

    dc.DrawLine(xBlockStartOffset_ + xOffset, 0, xBlockStartOffset_ + xOffset, yEndPos);

    int numBlocksVisibleOnScreen = (canvasSize.GetHeight() - yBlockStartOffset_ - blockHeight_ / 2) / blockHeight_;

    if (numBlocksVisibleOnScreen + yScrollOffset_ > numMidiNotes_)
      numBlocksVisibleOnScreen = numMidiNotes_ - yScrollOffset_;

    dc.DrawLine(xBlockStartOffset_ + xOffset, yBlockStartOffset_, xBlockStartOffset_ + xOffset, yBlockStartOffset_ + numBlocksVisibleOnScreen * blockHeight_);

    const int labelXoffset = isFirst ? xOffset + 5 : xOffset - 4;
    const int major = 1 + (xScrollOffset_ + x) / 4;
    const int minor = 1 + (xScrollOffset_ + x) % 4;

    char pBuf[64]{0};

    if (isFirst)
      snprintf(pBuf, sizeof(pBuf), "%d", major);
    else
      snprintf(pBuf, sizeof(pBuf), "%d.%d", major, minor);

    dc.DrawText(pBuf, xBlockStartOffset_ + labelXoffset, 10);
  }

  // draw note lines
  dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick
  dc.SetTextForeground(wxColor(0, 0, 0)); // set text color

  for (int y = 0; y < numMidiNotes_; ++y) {
    const int midiNote = numMidiNotes_ - 1 - y - yScrollOffset_;

    if (midiNote >= 0) {
      const int yOffset = y * blockHeight_;
      dc.DrawLine(xBlockStartOffset_, yBlockStartOffset_ + yOffset, canvasSize.GetWidth(), yBlockStartOffset_ + yOffset);

      if (midiNote == 0) {
        const int yOffsetLast = (y + 1) * blockHeight_;
        dc.DrawLine(xBlockStartOffset_, yBlockStartOffset_ + yOffsetLast, canvasSize.GetWidth(), yBlockStartOffset_ + yOffsetLast);
      }

      const char* pNoteStr = eMidi_numberToNote(midiNote);
      dc.DrawText(pNoteStr, 0, yBlockStartOffset_ + yOffset);
    }
  }

  // draw note blocks
  for (const NoteBlock& noteBlock : pSong_->noteBlocks()) {
    int x1 = xBlockStartOffset_ - xScrollOffset_ * pixelsPerQuarterNote_ + (noteBlock.startTick() * pixelsPerQuarterNote_) / pSong_->tpqn();
    const int y1 = yBlockStartOffset_ + blockHeight_ * (127 - noteBlock.note() - yScrollOffset_);
    int width = (noteBlock.numTicks() * pixelsPerQuarterNote_) / pSong_->tpqn();

    if ((x1 + width > xBlockStartOffset_) && (y1 >= yBlockStartOffset_)) {
      if (x1 < xBlockStartOffset_) {
        const int cutPixels = xBlockStartOffset_ - x1;
        x1 += cutPixels;
        width -= cutPixels;
      }

      if (noteBlock.isSelected())
        dc.SetBrush(wxBrush(wxColour(0, 255, 255)));
      else
        dc.SetBrush(wxBrush(wxColour(0, 255, 0)));

      dc.DrawRectangle(x1, y1, width, blockHeight_);
    }
  }
}

NoteBlock* KeyEditorGridCanvas::currentPointedNoteBlock(int mouseX, int mouseY) {
  for (NoteBlock& noteBlock : pSong_->noteBlocks()) {
    int x1 = xBlockStartOffset_ - xScrollOffset_ * pixelsPerQuarterNote_ + (noteBlock.startTick() * pixelsPerQuarterNote_) / pSong_->tpqn();
    const int y1 = yBlockStartOffset_ + blockHeight_ * (127 - noteBlock.note() - yScrollOffset_);
    int width = (noteBlock.numTicks() * pixelsPerQuarterNote_) / pSong_->tpqn();

    if ((x1 + width > xBlockStartOffset_) && (y1 >= yBlockStartOffset_)) {
      if (x1 < xBlockStartOffset_) {
        const int cutPixels = xBlockStartOffset_ - x1;
        x1 += cutPixels;
        width -= cutPixels;
      }
    }

    if (mouseX > x1 && mouseX < x1 + width && mouseY > y1 && mouseY < y1 + blockHeight_)
      return &noteBlock;
  }

  return nullptr;
}

void KeyEditorGridCanvas::OnMouseLeftDown(wxMouseEvent& event) {
  const char* pClickedTarget = "None";

  pSong_->unselectAllNotes();

  if (NoteBlock* pNoteBlock = currentPointedNoteBlock(event.GetX(), event.GetY())) {
    pNoteBlock->select();
    pClickedTarget = eMidi_numberToNote(pNoteBlock->note());
  }

  printf("clicked on: %s\n", pClickedTarget);
  render();
}

void KeyEditorGridCanvas::OnPaint(wxPaintEvent& event) {
  printf("KeyEditorGridCanvas::OnPaint\n");

  wxPaintDC dc(this);
  render(dc);
}

void KeyEditorGridCanvas::OnMouseMotion(wxMouseEvent& event) {
  // printf("KeyEditorGridCanvas::OnMouseMotion; x: %d, y: %d\n", event.GetX(), event.GetY());
}

void KeyEditorGridCanvas::setXscrollPosition(int xScrollPosition) {
  xScrollOffset_ = xScrollPosition;
  render();
}

void KeyEditorGridCanvas::setYscrollPosition(int yScrollPosition) {
  yScrollOffset_ = yScrollPosition;
  render();
}

void KeyEditorGridCanvas::setXzoomFactor(int xZoomFactor) {
  pixelsPerQuarterNote_ = (1 + xZoomFactor) * 10;
  render();
}

void KeyEditorGridCanvas::setYzoomFactor(int yZoomFactor) {
  blockHeight_ = (1 + yZoomFactor) * 10;
  render();
}

wxBEGIN_EVENT_TABLE(KeyEditorGridCanvas, wxWindow)
EVT_PAINT(KeyEditorGridCanvas::OnPaint)
EVT_MOTION(KeyEditorGridCanvas::OnMouseMotion)
EVT_LEFT_DOWN(KeyEditorGridCanvas::OnMouseLeftDown)
wxEND_EVENT_TABLE()

//-------------------------------------------------------------------------------------------------
// KeyEditorCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorCanvas::KeyEditorCanvas(wxWindow* pParent, Song* const pSong)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize) {

  wxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);

  pKeyEditorQuantizationCanvas_ = new KeyEditorQuantizationCanvas(this);
  pKeyEditorPianoCanvas_ = new KeyEditorPianoCanvas(this);
  pKeyEditorGridCanvas_ = new KeyEditorGridCanvas(this, pSong);

  wxSizer* pPianoGridSizer = new wxBoxSizer(wxHORIZONTAL);
  pPianoGridSizer->Add(pKeyEditorPianoCanvas_, 0, wxEXPAND);
  pPianoGridSizer->Add(pKeyEditorGridCanvas_, 1, wxEXPAND);

  pTopSizer->Add(pKeyEditorQuantizationCanvas_, 0, wxEXPAND);
  pTopSizer->Add(pPianoGridSizer, 1, wxEXPAND);

  SetSizer(pTopSizer);
}

void KeyEditorCanvas::render() {
  pKeyEditorGridCanvas_->render();
}

void KeyEditorCanvas::setXscrollPosition(int xScrollPosition) {
  pKeyEditorGridCanvas_->setXscrollPosition(xScrollPosition);
}

void KeyEditorCanvas::setYscrollPosition(int yScrollPosition) {
  pKeyEditorGridCanvas_->setYscrollPosition(yScrollPosition);
}

void KeyEditorCanvas::setXzoomFactor(int xZoomFactor) {
  pKeyEditorGridCanvas_->setXzoomFactor(xZoomFactor);
}

void KeyEditorCanvas::setYzoomFactor(int yZoomFactor) {
  pKeyEditorGridCanvas_->setYzoomFactor(yZoomFactor);
}

//-------------------------------------------------------------------------------------------------
// KeyEditorWindow
//-------------------------------------------------------------------------------------------------

KeyEditorWindow::KeyEditorWindow(wxWindow* pParent, Song* pSong)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN),
      pSong_(pSong) {

  wxFlexGridSizer* pTopSizer = new wxFlexGridSizer(2, 2, wxSize(0, 0));

  wxSizer* pHorizontalBarSizer = new wxBoxSizer(wxHORIZONTAL);
  pHorizontalScrollbar_ = new wxScrollBar(this, static_cast<int>(KeyEditorCanvas::ScrollBarType::HorizontalScroll), wxPoint(0, 100), wxSize(100, 10), wxHORIZONTAL);
  pHorizontalScrollbar_->SetScrollbar(0, 1, 128, 1);

  wxSlider* pHorizontalZoomSlider = new wxSlider(this, static_cast<int>(KeyEditorCanvas::ScrollBarType::HorizontalZoom), 5, 0, 10, wxPoint(0, 100), wxSize(100, 10), wxHORIZONTAL);

  wxSizer* pVerticalBarSizer = new wxBoxSizer(wxVERTICAL);
  pVerticalScrollbar_ = new wxScrollBar(this, static_cast<int>(KeyEditorCanvas::ScrollBarType::VerticalScroll), wxPoint(100, 0), wxSize(10, 100), wxVERTICAL);

  const int numMidiNotes = 128;
  pVerticalScrollbar_->SetScrollbar(24, 1, numMidiNotes, 1);

  wxSlider* pVerticalZoomSlider = new wxSlider(this, static_cast<int>(KeyEditorCanvas::ScrollBarType::VerticalZoom), 0, 0, 10, wxPoint(100, 0), wxSize(10, 100), wxVERTICAL);

  pHorizontalBarSizer->Add(pHorizontalScrollbar_, 1);
  pHorizontalBarSizer->Add(pHorizontalZoomSlider, 0);

  pVerticalBarSizer->Add(pVerticalScrollbar_, 1);
  pVerticalBarSizer->Add(pVerticalZoomSlider, 0);

  pKeyEditorCanvas_ = new KeyEditorCanvas(this, pSong_);
  pKeyEditorCanvas_->setXzoomFactor(pHorizontalZoomSlider->GetValue());
  pKeyEditorCanvas_->setYzoomFactor(pVerticalZoomSlider->GetValue());
  pKeyEditorCanvas_->setXscrollPosition(pHorizontalScrollbar_->GetThumbPosition());
  pKeyEditorCanvas_->setYscrollPosition(pVerticalScrollbar_->GetThumbPosition());

  pTopSizer->Add(pKeyEditorCanvas_, 1, wxEXPAND);
  pTopSizer->Add(pVerticalBarSizer, 1, wxEXPAND);
  pTopSizer->Add(pHorizontalBarSizer, 1, wxEXPAND);

  pTopSizer->AddGrowableCol(0, 1);
  pTopSizer->AddGrowableRow(0, 1);

  SetSizer(pTopSizer);
}

void KeyEditorWindow::render() {
  pKeyEditorCanvas_->render();
}

void KeyEditorWindow::OnScroll(wxScrollEvent& event) {
  printf("KeyEditorWindow::OnScroll; ");

  switch (static_cast<KeyEditorCanvas::ScrollBarType>(event.GetId())) {
    case KeyEditorCanvas::ScrollBarType::VerticalScroll:
      printf("Vertical Scroll: pos: %d\n", event.GetPosition());

      pKeyEditorCanvas_->setYscrollPosition(event.GetPosition());
      break;

    case KeyEditorCanvas::ScrollBarType::HorizontalScroll:
      printf("Horizontal Scroll: pos: %d\n", event.GetPosition());

      pKeyEditorCanvas_->setXscrollPosition(event.GetPosition());
      break;

    case KeyEditorCanvas::ScrollBarType::VerticalZoom:
      printf("Vertical Zoom: pos: %d\n", event.GetPosition());

      pKeyEditorCanvas_->setYzoomFactor(event.GetPosition());
      break;

    case KeyEditorCanvas::ScrollBarType::HorizontalZoom:
      printf("Horizontal Zoom: pos: %d\n", event.GetPosition());

      pKeyEditorCanvas_->setXzoomFactor(event.GetPosition());
      break;

    default:
      printf("unknown scrollbar ID: %d, pos %d\n", event.GetId(), event.GetPosition());
      // sliders do send this event!?
      break;
  }
}

void KeyEditorWindow::OnMouseWheel(wxMouseEvent& event) {
  printf("KeyEditorWindows::OnMouse; axis: %d, rotation: %d\n", event.GetWheelAxis(), event.GetWheelRotation());

  const int scrollStep = 4;

  switch (event.GetWheelAxis()) {
    case wxMOUSE_WHEEL_VERTICAL:
      pVerticalScrollbar_->SetThumbPosition(pVerticalScrollbar_->GetThumbPosition() + (event.GetWheelRotation() > 0 ? -scrollStep : scrollStep));
      pKeyEditorCanvas_->setYscrollPosition(pVerticalScrollbar_->GetThumbPosition());
      break;

    case wxMOUSE_WHEEL_HORIZONTAL:
      pHorizontalScrollbar_->SetThumbPosition(pHorizontalScrollbar_->GetThumbPosition() + (event.GetWheelRotation() > 0 ? scrollStep : -scrollStep));
      pKeyEditorCanvas_->setXscrollPosition(pHorizontalScrollbar_->GetThumbPosition());
      break;
  }
}

wxBEGIN_EVENT_TABLE(KeyEditorWindow, wxWindow)
EVT_SCROLL(KeyEditorWindow::OnScroll)
EVT_MOUSEWHEEL(KeyEditorWindow::OnMouseWheel)
wxEND_EVENT_TABLE()
