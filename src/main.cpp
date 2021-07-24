#include <iostream>

extern "C" {
#include "lib/eMIDI/src/midifile.h"
#include "lib/eMIDI/src/helpers.h"
}

#include "main.h"

//-------------------------------------------------------------------------------------------------
// WxApp
//-------------------------------------------------------------------------------------------------

bool WxApp::OnInit() {
  MainFrame* frame = new MainFrame("Floppy Music DAW", wxPoint(50, 50), wxSize(450, 340));
  frame->Show(true);

  printf("Floppy Music DAW started.\n");
  return true;
}

//-------------------------------------------------------------------------------------------------
// KeyEditorCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorCanvas::KeyEditorCanvas(wxWindow* pParent)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize) {
}

void KeyEditorCanvas::OnPaint(wxPaintEvent& event) {
  wxPaintDC dc(this);
  render(dc);
}

void KeyEditorCanvas::render(wxDC& dc) {
  int width;
  int height;
  GetClientSize(&width, &height);

  const int xSpacing = 50;
  const int ySpacing = 10;
  const int xPadding = 50;
  const int yPadding = 20;

  const int numMidiNotes = 128;

  // draw divisions
  for (int x = 0; x < width / xSpacing; ++x) {
    const int xOffset = x * xSpacing;
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
    dc.DrawLine(xPadding, 30 + yOffset, width, 30 + yOffset);

    const int midiNote = numMidiNotes - 1 - y;
    const char* pNoteStr = eMidi_numberToNote(midiNote);

    dc.DrawText(pNoteStr, 0, 30 + yOffset);
  }
}

//-------------------------------------------------------------------------------------------------
// KeyEditorWindow
//-------------------------------------------------------------------------------------------------

KeyEditorWindow::KeyEditorWindow(wxWindow* pParent)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN) {

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

  pTopSizer->Add(new KeyEditorCanvas(this), 1, wxEXPAND);
  pTopSizer->Add(pVerticalBarSizer, 1, wxEXPAND);
  pTopSizer->Add(pHorizontalBarSizer, 1, wxEXPAND);

  pTopSizer->AddGrowableCol(0, 1);
  pTopSizer->AddGrowableRow(0, 1);

  SetSizer(pTopSizer);
}

//-------------------------------------------------------------------------------------------------
// MainFrame
//-------------------------------------------------------------------------------------------------

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
  : wxFrame(NULL, wxID_ANY, title, pos, size) {
  wxMenu *menuFile = new wxMenu;

  wxMenuItem* pGetBpItem = new wxMenuItem(menuFile, wxID_OPEN, "&Open MIDI File\tCtrl-O", "Open MIDI File");
  menuFile->Append(pGetBpItem);

  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");

  SetMenuBar(menuBar);
  CreateStatusBar();
  SetStatusText("Ready.");

  new KeyEditorWindow(this);
}

void MainFrame::OnExit(wxCommandEvent& event) {
  Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event) {
  wxMessageBox("Made by coon\n\n"
               "E-Mail: coon@mailbox.org\n"
               "IRC: coon@hackint.org\n\n"
               "https://www.reddit.com/user/coon42\nhttps://github.com/coon42\n",
               "Floppy Music DAW", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnOpen(wxCommandEvent& event) {
  wxFileDialog openFileDialog(this, _("Open Midi file"), "", "", "MIDI files (*.mid;*.midi)|*.mid;*.midi",
      wxFD_OPEN | wxFD_FILE_MUST_EXIST);

  if (openFileDialog.ShowModal() == wxID_CANCEL)
    return;

  if (Error error = eMidi_open(&midiFile_, openFileDialog.GetPath())) {
    printf("Error on opening midi file!\n");
    return;
  }

  if (Error error = eMidi_printFileInfo(&midiFile_)) {
    printf("Error on printing MIDI file info!\n");
    return;
  }

  if (Error error = eMidi_close(&midiFile_)) {
    printf("Error on closing midi file!\n");
    return;
  }
}
