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

  switch (event.GetOrientation()) {
    case wxVERTICAL:
      printf("vertical: pos: % d\n", event.GetPosition());
      break;

    case wxHORIZONTAL:
      printf("horizontal: pos: % d\n", event.GetPosition());
      // TODO: implement
      break;

    default:
      printf("unknown orientation\n");
      // sliders do send this event!?
      break;
  }
}

void KeyEditorCanvas::render(wxDC& dc) {
  const wxSize canvasSize = GetClientSize();

  const int pixelsPerQuarterNote = 50;
  const int blockHeight = 10;
  const int xBlockStartOffset = 50;
  const int yBlockStartOffset = 30;

  const int numMidiNotes = 128;

  // draw divisions
  for (int x = 0; x < canvasSize.GetWidth() / pixelsPerQuarterNote; ++x) {
    const int xOffset = x * pixelsPerQuarterNote;
    const bool isFirst = x % 4 == 0;
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
    dc.DrawLine(xBlockStartOffset + xOffset, yBlockStartOffset, xBlockStartOffset + xOffset, yBlockStartOffset + numMidiNotes * blockHeight);

    const int labelXoffset = isFirst ? xOffset + 5 : xOffset - 4;

    const int major = 1 + x / 4;
    const int minor = 1 + x % 4;

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
    const int yOffset = y * blockHeight;
    dc.DrawLine(xBlockStartOffset, yBlockStartOffset + yOffset, canvasSize.GetWidth(), yBlockStartOffset + yOffset);

    const int midiNote = numMidiNotes - 1 - y;
    const char* pNoteStr = eMidi_numberToNote(midiNote);

    dc.DrawText(pNoteStr, 0, yBlockStartOffset + yOffset);
  }

  // draw note blocks
  dc.SetBrush(wxBrush(wxColour(0, 255, 0)));

  for (const NoteBlock& noteBlock : pSong_->noteBlocks) {
    const int x1 = xBlockStartOffset + (noteBlock.startTick * pixelsPerQuarterNote) / pSong_->TPQN;
    const int y1 = yBlockStartOffset + blockHeight * (127 - noteBlock.note);
    const int width = (noteBlock.numTicks * pixelsPerQuarterNote) / pSong_->TPQN;

    dc.DrawRectangle(x1, y1, width, blockHeight);
  }
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
  wxScrollBar* pHorizontalScrollbar = new wxScrollBar(this, wxID_ANY, wxPoint(0, 100), wxSize(100, 10));
  wxSlider* pHorizontalSlider = new wxSlider(this, wxID_ANY, 5, 0, 10, wxPoint(0, 100), wxSize(100, 10));

  wxSizer* pVerticalBarSizer = new wxBoxSizer(wxVERTICAL);
  wxScrollBar* pVerticalScrollbar = new wxScrollBar(this, wxID_ANY, wxPoint(100, 0), wxSize(10, 100), wxVERTICAL);
  wxSlider* pVerticalSlider = new wxSlider(this, wxID_ANY, 5, 0, 10, wxPoint(100, 0), wxSize(10, 100), wxVERTICAL);

  pHorizontalBarSizer->Add(pHorizontalScrollbar, 1);
  pHorizontalBarSizer->Add(pHorizontalSlider, 0);

  pVerticalBarSizer->Add(pVerticalScrollbar, 1);
  pVerticalBarSizer->Add(pVerticalSlider, 0);

  pTopSizer->Add(new KeyEditorCanvas(this, pSong_), 1, wxEXPAND);
  pTopSizer->Add(pVerticalBarSizer, 1, wxEXPAND);
  pTopSizer->Add(pHorizontalBarSizer, 1, wxEXPAND);

  pTopSizer->AddGrowableCol(0, 1);
  pTopSizer->AddGrowableRow(0, 1);

  SetSizer(pTopSizer);
}

wxBEGIN_EVENT_TABLE(KeyEditorWindow, wxWindow)
EVT_SCROLL(KeyEditorWindow::OnScroll)
wxEND_EVENT_TABLE()
