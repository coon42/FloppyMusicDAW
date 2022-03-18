#ifndef _MAIN_H
#define _MAIN_H

#include <wx/wx.h>

#include "keyeditor.h"
#include "trackeditor.h"
#include "transport.h"
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
  void OnSaveAs(wxCommandEvent& event);

  void updateTitle();

  TransportWindow* pTransportWindow_{nullptr};
  TrackEditorWindow* pTrackEditorWindow_{nullptr};
  KeyEditorWindow* pKeyEditorWindow_{nullptr};
  Song song_;

  static void onRedrawAllRequest(void* pCtx); // TODO: remove once rendering is fixed!

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
