#ifndef _MAIN_H
#define _MAIN_H

#include <wx/wx.h>

#include "keyeditor.h"

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

  MidiFile midiFile_{0};

  wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT, MainFrame::OnExit)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
wxEND_EVENT_TABLE()

//-------------------------------------------------------------------------------------------------
// WxApp
//-------------------------------------------------------------------------------------------------

class WxApp : public wxApp {
private:
  virtual bool OnInit();
};

wxIMPLEMENT_APP_CONSOLE(WxApp);

#endif // _MAIN_H

