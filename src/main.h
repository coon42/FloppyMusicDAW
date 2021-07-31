#ifndef _MAIN_H
#define _MAIN_H

#include <wx/wx.h>

#include "keyeditor.h"
#include "song.h"

//-------------------------------------------------------------------------------------------------
// MainFrame
//-------------------------------------------------------------------------------------------------

class MainFrame : public wxFrame {
public:
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnOpen(wxCommandEvent& event);

  void generateNoteBlocks(MidiFile* pMidiFile);
  void openMidiFile(const std::string& path);

  KeyEditorWindow* pKeyEditorWindow_{nullptr};
  Song song_{0};

  wxDECLARE_EVENT_TABLE();
};

//-------------------------------------------------------------------------------------------------
// WxApp
//-------------------------------------------------------------------------------------------------

class WxApp : public wxApp {
private:
  virtual bool OnInit();
};

#endif // _MAIN_H
