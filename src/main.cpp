#include <iostream>

extern "C" {
#include "lib/eMIDI/src/midifile.h"
#include "lib/eMIDI/src/helpers.h"
}

#include "main.h"

//-------------------------------------------------------------------------------------------------
// MainFrame
//-------------------------------------------------------------------------------------------------

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size) {

  wxMenu* pFileMenu = new wxMenu;
  pFileMenu->Append(new wxMenuItem(pFileMenu, wxID_OPEN, "&Open MIDI File\tCtrl-O", "Open MIDI File"));
  pFileMenu->AppendSeparator();
  pFileMenu->Append(wxID_EXIT);

  wxMenu* pHelpMenu = new wxMenu;
  pHelpMenu->Append(wxID_ABOUT);

  wxMenuBar* pMenuBar = new wxMenuBar;
  pMenuBar->Append(pFileMenu, "&File");
  pMenuBar->Append(pHelpMenu, "&Help");

  SetMenuBar(pMenuBar);
  CreateStatusBar();
  SetStatusText("Ready.");

  new KeyEditorWindow(this);

  Show(true);
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

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT, MainFrame::OnExit)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
wxEND_EVENT_TABLE()

//-------------------------------------------------------------------------------------------------
// WxApp
//-------------------------------------------------------------------------------------------------

bool WxApp::OnInit() {
  new MainFrame("Floppy Music DAW", wxDefaultPosition, wxSize(800, 600));

  printf("Floppy Music DAW started.\n");

  return true;
}

wxIMPLEMENT_APP_CONSOLE(WxApp);
