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

void KeyEditorCanvas::render(wxDC& dc) {
  const wxSize canvasSize = GetClientSize();

  const int pixelsPerQuarterNote = 50;
  const int ySpacing = 10;
  const int xPadding = 50;
  const int yPadding = 20;

  const int numMidiNotes = 128;

  // draw divisions
  for (int x = 0; x < canvasSize.GetWidth() / pixelsPerQuarterNote; ++x) {
    const int xOffset = x * pixelsPerQuarterNote;
    const bool isFirst = x % 4 == 0;
    const int yEndPos = isFirst ? 30 : 10;

    if (isFirst) {
      dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick
      dc.SetTextForeground(wxColor(0, 0, 0)); // set text color
    }
    else {
      dc.SetPen(wxPen(wxColor(192, 192, 192), 1)); // black line, 1 pixels thick
      dc.SetTextForeground(wxColor(128, 128, 128)); // set text color
    }

    dc.DrawLine(xPadding + xOffset, 0,  xPadding + xOffset, yEndPos);
    dc.DrawLine(xPadding + xOffset, 30, xPadding + xOffset, 30 + numMidiNotes * ySpacing);

    const int labelXoffset = isFirst ? xOffset + 5 : xOffset - 4;

    const int major = 1 + x / 4;
    const int minor = 1 + x % 4;

    char pBuf[64]{0};

    if (isFirst)
      snprintf(pBuf, sizeof(pBuf), "%d", major);
    else
      snprintf(pBuf, sizeof(pBuf), "%d.%d", major, minor);

    dc.DrawText(pBuf, xPadding + labelXoffset, 10);
  }

  // draw note lines
  dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick
  dc.SetTextForeground(wxColor(0, 0, 0)); // set text color

  for (int y = 0; y < numMidiNotes; ++y) {
    const int yOffset = y * ySpacing;
    dc.DrawLine(xPadding, 30 + yOffset, canvasSize.GetWidth(), 30 + yOffset);

    const int midiNote = numMidiNotes - 1 - y;
    const char* pNoteStr = eMidi_numberToNote(midiNote);

    dc.DrawText(pNoteStr, 0, 30 + yOffset);
  }

  // draw note blocks
  dc.SetBrush(wxBrush(wxColour(0, 255, 0)));

  for (const NoteBlock& noteBlock : pSong_->noteBlocks) {
    const int x1 = xPadding + (noteBlock.startTick * pixelsPerQuarterNote) / pSong_->TPQN;
    const int y1 = 30 + ySpacing * (127 - noteBlock.note);
    const int width = (noteBlock.numTicks * pixelsPerQuarterNote) / pSong_->TPQN;

    const int yOffset = ySpacing;
    const int height = yOffset;

    dc.DrawRectangle(x1, y1, width, height);
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
