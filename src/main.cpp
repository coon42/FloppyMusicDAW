#include <iostream>

extern "C" {
#include "lib/eMIDI/src/midifile.h"
#include "lib/eMIDI/src/helpers.h"
}

#include "main.h"

//--------------------------------------------------------------------------------------------------------------
// WxApp
//--------------------------------------------------------------------------------------------------------------

bool WxApp::OnInit() {
  MainFrame* frame = new MainFrame("Floppy Music DAW", wxPoint(50, 50), wxSize(450, 340));
  frame->Show(true);

  printf("Floppy Music DAW started.\n");
  return true;
}

//--------------------------------------------------------------------------------------------------------------
// MainFrame
//--------------------------------------------------------------------------------------------------------------

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
}
