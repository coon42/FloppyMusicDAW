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

