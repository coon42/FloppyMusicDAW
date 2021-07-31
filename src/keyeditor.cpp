#include <wx/wx.h>

extern "C" {
#include "lib/eMIDI/src/helpers.h"
}

#include "keyeditor.h"

//-------------------------------------------------------------------------------------------------
// KeyEditorCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorCanvas::KeyEditorCanvas(wxWindow* pParent, Song* const pSong)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
      pSong_(pSong) {
}

void KeyEditorCanvas::OnPaint(wxPaintEvent& event) {
  wxPaintDC dc(this);
  render(dc);
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

void KeyEditorCanvas::render(wxDC& dc) {
  dc.Clear();

  const wxSize canvasSize = GetClientSize();

  const int pixelsPerQuarterNote = (1 + xZoomFactor_) * 10;
  const int blockHeight = (1 + yZoomFactor_) * 10;
  const int xBlockStartOffset = 50;
  const int yBlockStartOffset = 30;

  const int numMidiNotes = 128;

  // draw divisions
  for (int x = 0; x < canvasSize.GetWidth() / pixelsPerQuarterNote; ++x) {
    const int xOffset = x * pixelsPerQuarterNote;
    const bool isFirst = (xScrollOffset_ + x) % 4 == 0;
    const int yEndPos = isFirst ? yBlockStartOffset : 10;

    if (isFirst) {
      dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick
      dc.SetTextForeground(wxColor(0, 0, 0)); // set text color
    }
    else {
      dc.SetPen(wxPen(wxColor(192, 192, 192), 1)); // black line, 1 pixels thick
      dc.SetTextForeground(wxColor(128, 128, 128)); // set text color
    }

    dc.DrawLine(xBlockStartOffset + xOffset, 0, xBlockStartOffset + xOffset, yEndPos);

    int numBlocksVisibleOnScreen = (canvasSize.GetHeight() - yBlockStartOffset - blockHeight / 2) / blockHeight;

    if (numBlocksVisibleOnScreen + yScrollOffset_ > numMidiNotes)
      numBlocksVisibleOnScreen = numMidiNotes - yScrollOffset_;

    dc.DrawLine(xBlockStartOffset + xOffset, yBlockStartOffset, xBlockStartOffset + xOffset, yBlockStartOffset + numBlocksVisibleOnScreen * blockHeight);

    const int labelXoffset = isFirst ? xOffset + 5 : xOffset - 4;
    const int major = 1 + (xScrollOffset_ + x) / 4;
    const int minor = 1 + (xScrollOffset_ + x) % 4;

    char pBuf[64]{0};

    if (isFirst)
      snprintf(pBuf, sizeof(pBuf), "%d", major);
    else
      snprintf(pBuf, sizeof(pBuf), "%d.%d", major, minor);

    dc.DrawText(pBuf, xBlockStartOffset + labelXoffset, 10);
  }

  // draw note lines
  dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick
  dc.SetTextForeground(wxColor(0, 0, 0)); // set text color

  for (int y = 0; y < numMidiNotes; ++y) {
    const int midiNote = numMidiNotes - 1 - y - yScrollOffset_;

    if (midiNote >= 0) {
      const int yOffset = y * blockHeight;
      dc.DrawLine(xBlockStartOffset, yBlockStartOffset + yOffset, canvasSize.GetWidth(), yBlockStartOffset + yOffset);

      if (midiNote == 0) {
        const int yOffsetLast = (y + 1) * blockHeight;
        dc.DrawLine(xBlockStartOffset, yBlockStartOffset + yOffsetLast, canvasSize.GetWidth(), yBlockStartOffset + yOffsetLast);
      }

      const char* pNoteStr = eMidi_numberToNote(midiNote);
      dc.DrawText(pNoteStr, 0, yBlockStartOffset + yOffset);
    }
  }

  // draw note blocks
  dc.SetBrush(wxBrush(wxColour(0, 255, 0)));

  for (const NoteBlock& noteBlock : pSong_->noteBlocks) {
    int x1 = xBlockStartOffset - xScrollOffset_ * pixelsPerQuarterNote + (noteBlock.startTick * pixelsPerQuarterNote) / pSong_->TPQN;
    const int y1 = yBlockStartOffset + blockHeight * (127 - noteBlock.note - yScrollOffset_);
    int width = (noteBlock.numTicks * pixelsPerQuarterNote) / pSong_->TPQN;

    if ((x1 + width > xBlockStartOffset) && (y1 >= yBlockStartOffset)) {
      if (x1 < xBlockStartOffset) {
        const int cutPixels = xBlockStartOffset - x1;
        x1 += cutPixels;
        width -= cutPixels;
      }

      dc.DrawRectangle(x1, y1, width, blockHeight);
    }
  }
}

void KeyEditorCanvas::setXscrollPosition(int xScrollPosition) {
  xScrollOffset_ = xScrollPosition;

  wxClientDC dc(this);
  render(dc);
}

void KeyEditorCanvas::setYscrollPosition(int yScrollPosition) {
  yScrollOffset_ = yScrollPosition;

  wxClientDC dc(this);
  render(dc);
}

void KeyEditorCanvas::setXzoomFactor(int xZoomFactor) {
  xZoomFactor_ = xZoomFactor;

  wxClientDC dc(this);
  render(dc);
}

void KeyEditorCanvas::setYzoomFactor(int yZoomFactor) {
  yZoomFactor_ = yZoomFactor;

  wxClientDC dc(this);
  render(dc);
}

wxBEGIN_EVENT_TABLE(KeyEditorCanvas, wxWindow)
EVT_PAINT(KeyEditorCanvas::OnPaint)
wxEND_EVENT_TABLE()

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
  pKeyEditorCanvas_->setXscrollPosition(pHorizontalScrollbar_->GetThumbPosition());
  pKeyEditorCanvas_->setYscrollPosition(pVerticalScrollbar_->GetThumbPosition());
  pKeyEditorCanvas_->setXzoomFactor(pHorizontalZoomSlider->GetValue());
  pKeyEditorCanvas_->setYzoomFactor(pVerticalZoomSlider->GetValue());

  pTopSizer->Add(pKeyEditorCanvas_, 1, wxEXPAND);
  pTopSizer->Add(pVerticalBarSizer, 1, wxEXPAND);
  pTopSizer->Add(pHorizontalBarSizer, 1, wxEXPAND);

  pTopSizer->AddGrowableCol(0, 1);
  pTopSizer->AddGrowableRow(0, 1);

  SetSizer(pTopSizer);
}

wxBEGIN_EVENT_TABLE(KeyEditorWindow, wxWindow)
EVT_SCROLL(KeyEditorWindow::OnScroll)
EVT_MOUSEWHEEL(KeyEditorWindow::OnMouseWheel)
wxEND_EVENT_TABLE()
