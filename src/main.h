#ifndef _MAIN_H
#define _MAIN_H

#include <wx/wx.h>

//--------------------------------------------------------------------------------------------------------------
// WxApp
//--------------------------------------------------------------------------------------------------------------

class WxApp : public wxApp {
public:
  virtual bool OnInit();
};

//--------------------------------------------------------------------------------------------------------------
// MainFrame
//--------------------------------------------------------------------------------------------------------------

class MainFrame : public wxFrame {
public:
  MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

  wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT, MainFrame::OnExit)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP_CONSOLE(WxApp);



#endif // _MAIN_H
