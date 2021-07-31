#include <iostream>
#include <map>

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

  pKeyEditorWindow_ = new KeyEditorWindow(this, &song_);

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

void MainFrame::generateNoteBlocks(MidiFile* pMidiFile) {
  song_.TPQN = pMidiFile->header.division.tpqn.TPQN;
  song_.noteBlocks.clear();
  std::map<uint8_t, NoteBlock> onNotes;

  printf("MIDI Events:\n");

  uint32_t currentTick = 0;
  MidiEvent midiEvent;
  while (eMidi_readEvent(pMidiFile, &midiEvent) == EMIDI_OK) {
    currentTick += midiEvent.deltaTime;

    printf("%8d - ", currentTick);

    if (Error error = eMidi_printMidiEvent(&midiEvent))
      break;

    switch (midiEvent.eventId) {
      case MIDI_EVENT_NOTE_ON: {
        const uint8_t note = midiEvent.params.msg.noteOn.note;

        if (onNotes.find(note) == onNotes.end()) { // ignore, double additional note on event if already active
          if (midiEvent.params.msg.noteOn.velocity > 0) {
            NoteBlock block;
            block.note = midiEvent.params.msg.noteOn.note;
            block.startTick = currentTick;

            onNotes[note] = block;
          }
          else {
            onNotes[note].numTicks = currentTick - onNotes[note].startTick;
            song_.noteBlocks.push_back(onNotes[note]);
            onNotes.erase(note);
          }
        }

        break;
      }

      case MIDI_EVENT_NOTE_OFF: {
        const uint8_t note = midiEvent.params.msg.noteOff.note;

        if (onNotes.find(note) != onNotes.end()) { // ignore, if there is no matching note on event active
          NoteBlock& noteBlock = onNotes[note];
          noteBlock.numTicks = currentTick - noteBlock.startTick;

          song_.noteBlocks.push_back(onNotes[note]);
          onNotes.erase(note);
        }

        break;
      }

      default:
        break;
    }
  }

  printf("Note blocks:\n");

  for (const NoteBlock& b : song_.noteBlocks)
    printf("Note: %s, start: %d, numTicks: %d\n", eMidi_numberToNote(b.note), b.startTick, b.numTicks);
}

void MainFrame::openMidiFile(const std::string& path) {
  MidiFile midiFile{0};

  if (Error error = eMidi_open(&midiFile, path.c_str())) {
    printf("Error on opening midi file!\n");
    return;
  }

  generateNoteBlocks(&midiFile);

  if (Error error = eMidi_printFileInfo(&midiFile)) {
    printf("Error on printing MIDI file info!\n");
    return;
  }

  if (Error error = eMidi_close(&midiFile)) {
    printf("Error on closing midi file!\n");
    return;
  }
}

void MainFrame::OnOpen(wxCommandEvent& event) {
  wxFileDialog openFileDialog(this, _("Open Midi file"), "", "", "MIDI files (*.mid;*.midi)|*.mid;*.midi",
      wxFD_OPEN | wxFD_FILE_MUST_EXIST);

  if (openFileDialog.ShowModal() == wxID_CANCEL)
    return;

  openMidiFile(openFileDialog.GetPath().ToStdString());
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
  new MainFrame("Floppy Music DAW", wxDefaultPosition, wxSize(1440, 900));

  printf("Floppy Music DAW started.\n");

  return true;
}

wxIMPLEMENT_APP_CONSOLE(WxApp);
