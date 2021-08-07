#include <wx/wx.h>

extern "C" {
#include "lib/eMIDI/src/helpers.h"
}

#include "keyeditor.h"

//-------------------------------------------------------------------------------------------------
// KeyEditorCanvasCanvasSegment
//-------------------------------------------------------------------------------------------------

KeyEditorCanvasSegment::KeyEditorCanvasSegment(KeyEditorCanvas* pParent, const wxSize& size = wxDefaultSize)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, size) {

}

void KeyEditorCanvasSegment::render() {
  wxClientDC dc(this);
  dc.Clear();

  onRender(dc);
}

void KeyEditorCanvasSegment::OnPaint(wxPaintEvent& event) {
  render();
}

KeyEditorCanvas* KeyEditorCanvasSegment::canvas() {
  return static_cast<KeyEditorCanvas*>(GetParent());
}

wxBEGIN_EVENT_TABLE(KeyEditorCanvasSegment, wxWindow)
EVT_PAINT(KeyEditorCanvasSegment::OnPaint)
wxEND_EVENT_TABLE()

//-------------------------------------------------------------------------------------------------
// KeyEditorQuantizationCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorQuantizationCanvas::KeyEditorQuantizationCanvas(KeyEditorCanvas* pParent)
    : KeyEditorCanvasSegment(pParent, wxSize(0, 30)) {

}

void KeyEditorQuantizationCanvas::onRender(wxDC& dc) {
  const int canvasWidth = GetClientSize().GetWidth() - canvas()->xBlockStartOffset();
  const int numSegments = canvasWidth / canvas()->pixelsPerQuarterNote() + canvas()->pixelsPerQuarterNote();

  for (int segment = 0; segment < numSegments; ++segment) {
    const int xOffset = segment * canvas()->pixelsPerQuarterNote();
    const bool isFirst = (canvas()->xScrollOffset() + segment) % 4 == 0;
    const int yEndPos = isFirst ? GetClientSize().GetHeight() : 10;

    if (isFirst) {
      dc.SetPen(wxPen(wxColor(0, 0, 0), 1));
      dc.SetTextForeground(wxColor(0, 0, 0));
    }
    else {
      dc.SetPen(wxPen(wxColor(192, 192, 192), 1));
      dc.SetTextForeground(wxColor(128, 128, 128));
    }

    dc.DrawLine(canvas()->xBlockStartOffset() + xOffset, 0, canvas()->xBlockStartOffset() + xOffset, yEndPos);

    const int labelXoffset = isFirst ? xOffset + 5 : xOffset - 4;
    const int major = 1 + (canvas()->xScrollOffset() + segment) / 4;
    const int minor = 1 + (canvas()->xScrollOffset() + segment) % 4;

    char pBuf[64]{0};

    if (isFirst)
      snprintf(pBuf, sizeof(pBuf), "%d", major);
    else
      snprintf(pBuf, sizeof(pBuf), "%d.%d", major, minor);

    dc.DrawText(pBuf, canvas()->xBlockStartOffset() + labelXoffset, 10);
  }
}

//-------------------------------------------------------------------------------------------------
// KeyEditorPianoCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorPianoCanvas::KeyEditorPianoCanvas(KeyEditorCanvas* pParent)
    : KeyEditorCanvasSegment(pParent, wxSize(50, 0)) {

}

void KeyEditorPianoCanvas::onRender(wxDC& dc) {
  dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick
  dc.SetTextForeground(wxColor(0, 0, 0)); // set text color

  for (int y = 0; y < NUM_MIDI_NOTES; ++y) {
    const int midiNote = NUM_MIDI_NOTES - 1 - y - canvas()->yScrollOffset();

    if (midiNote >= 0) {
      const int yOffset = y * canvas()->blockHeight();
      const char* pNoteStr = eMidi_numberToNote(midiNote);

      dc.DrawText(pNoteStr, 0, yOffset);
    }
  }
}

//-------------------------------------------------------------------------------------------------
// KeyEditorGridCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorGridCanvas::KeyEditorGridCanvas(KeyEditorCanvas* pParent, Song* pSong) : KeyEditorCanvasSegment(pParent),
  pSong_(pSong) {

}

void KeyEditorGridCanvas::onRender(wxDC& dc) {
  const wxSize& canvasSize = GetClientSize();
  const int canvasWidth = canvasSize.GetWidth() / canvas()->pixelsPerQuarterNote();
  const int numSegments = canvasWidth / canvas()->pixelsPerQuarterNote() + canvas()->pixelsPerQuarterNote();

  // draw divisions
  for (int segment = 0; segment < numSegments; ++segment) {
    const int xOffset = segment * canvas()->pixelsPerQuarterNote();
    const bool isFirst = (canvas()->xScrollOffset() + segment) % 4 == 0;

    if (isFirst) {
      dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick
      dc.SetTextForeground(wxColor(0, 0, 0)); // set text color
    }
    else {
      dc.SetPen(wxPen(wxColor(192, 192, 192), 1)); // black line, 1 pixels thick
      dc.SetTextForeground(wxColor(128, 128, 128)); // set text color
    }

    int numBlocksVisibleOnScreen = (canvasSize.GetHeight() - canvas()->blockHeight() / 2) / canvas()->blockHeight();

    if (numBlocksVisibleOnScreen + canvas()->yScrollOffset() > NUM_MIDI_NOTES)
      numBlocksVisibleOnScreen = NUM_MIDI_NOTES - canvas()->yScrollOffset();

    dc.DrawLine(xOffset, 0, xOffset, numBlocksVisibleOnScreen * canvas()->blockHeight());
  }

  // draw note lines
  dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick
  dc.SetTextForeground(wxColor(0, 0, 0)); // set text color

  for (int y = 0; y < NUM_MIDI_NOTES; ++y) {
    const int midiNote = NUM_MIDI_NOTES - 1 - y - canvas()->yScrollOffset();

    if (midiNote >= 0) {
      const int yOffset = y * canvas()->blockHeight();
      dc.DrawLine(0, yOffset, canvasSize.GetWidth(), yOffset);

      if (midiNote == 0) {
        const int yOffsetLast = (y + 1) * canvas()->blockHeight();
        dc.DrawLine(0, yOffsetLast, canvasSize.GetWidth(), yOffsetLast);
      }
    }
  }

  // draw note blocks
  for (const NoteBlock& noteBlock : pSong_->noteBlocks()) {
    int x1 = (noteBlock.startTick() * canvas()->pixelsPerQuarterNote()) / pSong_->tpqn() - canvas()->xScrollOffset() * canvas()->pixelsPerQuarterNote();
    const int y1 = canvas()->blockHeight() * (127 - noteBlock.note() - canvas()->yScrollOffset());
    int width = (noteBlock.numTicks() * canvas()->pixelsPerQuarterNote()) / pSong_->tpqn();

    if ((x1 + width > 0) && (y1 >= 0)) {
      if (x1 < 0) {
        const int cutPixels = -x1;
        x1 += cutPixels;
        width -= cutPixels;
      }

      if (noteBlock.isSelected())
        dc.SetBrush(wxBrush(wxColour(0, 255, 255)));
      else
        dc.SetBrush(wxBrush(wxColour(0, 255, 0)));

      dc.DrawRectangle(x1, y1, width, canvas()->blockHeight());
    }
  }
}

NoteBlock* KeyEditorGridCanvas::currentPointedNoteBlock(int mouseX, int mouseY) {
  for (NoteBlock& noteBlock : pSong_->noteBlocks()) {
    int x1 = (noteBlock.startTick() * canvas()->pixelsPerQuarterNote()) / pSong_->tpqn() - canvas()->xScrollOffset() * canvas()->pixelsPerQuarterNote();
    const int y1 = canvas()->blockHeight() * (127 - noteBlock.note() - canvas()->yScrollOffset());
    int width = (noteBlock.numTicks() * canvas()->pixelsPerQuarterNote()) / pSong_->tpqn();

    if ((x1 + width > 0) && (y1 >= 0)) {
      if (x1 < 0) {
        const int cutPixels = -x1;
        x1 += cutPixels;
        width -= cutPixels;
      }
    }

    if (mouseX > x1 && mouseX < x1 + width && mouseY > y1 && mouseY < y1 + canvas()->blockHeight())
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
  wxPaintDC dc(this);
  onRender(dc);
}

void KeyEditorGridCanvas::OnMouseMotion(wxMouseEvent& event) {
  // printf("KeyEditorGridCanvas::OnMouseMotion; x: %d, y: %d\n", event.GetX(), event.GetY());
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
  pKeyEditorQuantizationCanvas_->render();
  pKeyEditorPianoCanvas_->render();
  pKeyEditorGridCanvas_->render();
}

void KeyEditorCanvas::setXscrollPosition(int xScrollPosition) {
  xScrollOffset_ = xScrollPosition;
  render();
}

void KeyEditorCanvas::setYscrollPosition(int yScrollPosition) {
  yScrollOffset_ = yScrollPosition;
  render();
}

void KeyEditorCanvas::setXzoomFactor(int xZoomFactor) {
  pixelsPerQuarterNote_ = (1 + xZoomFactor) * 10;
  render();
}

void KeyEditorCanvas::setYzoomFactor(int yZoomFactor) {
  blockHeight_ = (1 + yZoomFactor) * 10;
  render();
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
  pVerticalScrollbar_->SetScrollbar(24, 1, NUM_MIDI_NOTES, 1);

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
