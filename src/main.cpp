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
  pFileMenu->Append(new wxMenuItem(pFileMenu, wxID_SAVEAS, "&Export as Midi 0 file\tCtrl-S", "Export as Midi 0 file"));
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

  song_.registerRedrawAllCallback(onRedrawAllRequest, this); // TODO: remove once rendering is fixed!

  pTransportWindow_ = new TransportWindow(this, &song_);
  pTrackEditorWindow_ = new TrackEditorWindow(this, &song_);
  pKeyEditorWindow_ = new KeyEditorWindow(this, &song_);

  wxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);
  pTopSizer->Add(pTransportWindow_, 0, wxEXPAND);
  pTopSizer->Add(pTrackEditorWindow_, 0, wxEXPAND);
  pTopSizer->Add(pKeyEditorWindow_, 1, wxEXPAND);

  SetSizer(pTopSizer);
  updateTitle();

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

  song_.importFromMidi0(openFileDialog.GetPath().ToStdString());
  updateTitle();

  pTransportWindow_->update();
  pTrackEditorWindow_->updateTrackList();
  pKeyEditorWindow_->setDefaultScrollPositions();

  pKeyEditorWindow_->render();
}

void MainFrame::OnSaveAs(wxCommandEvent& event) {
  wxFileDialog saveFileDialog(this, _("Export as Midi 0 file"), "", "", "MIDI files (*.mid;*.midi)|*.mid;*.midi",
      wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

  if (saveFileDialog.ShowModal() == wxID_CANCEL)
    return;

  song_.exportAsMidi0(saveFileDialog.GetPath().ToStdString());
  updateTitle();
}

void MainFrame::updateTitle() {
  std::string curTrackName = song_.currentSelectedTrack()->name();

  // Cut away 'Track' duplicate, if the tracks name itself is starting with 'Track':
  const std::string trackStr = "Track ";
  
  if (curTrackName.find(trackStr) != std::string::npos)
    curTrackName = curTrackName.substr(trackStr.length());

  SetTitle(wxString::Format("%s - Track %s - Floppy Music DAW", song_.currentSongFileName().c_str(),
      curTrackName.c_str()));
}

// TODO: remove once rendering is fixed:
void MainFrame::onRedrawAllRequest(void* pCtx) {
  MainFrame* pThis = static_cast<MainFrame*>(pCtx);

  pThis->pTransportWindow_->update();
  pThis->pTrackEditorWindow_->updateTrackList();
  pThis->pKeyEditorWindow_->render();
  pThis->updateTitle();
}
// --

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT, MainFrame::OnExit)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
EVT_MENU(wxID_SAVEAS, MainFrame::OnSaveAs)
wxEND_EVENT_TABLE()

//-------------------------------------------------------------------------------------------------
// WxApp
//-------------------------------------------------------------------------------------------------

bool WxApp::OnInit() {
  new MainFrame("", wxDefaultPosition, wxSize(1440, 900));

  printf("Floppy Music DAW started.\n");

  return true;
}

wxIMPLEMENT_APP_CONSOLE(WxApp);
