#ifndef _KEY_EDITOR_H
#define _KEY_EDITOR_H

#include <wx/wx.h>

//-------------------------------------------------------------------------------------------------
// KeyEditorCanvas
//-------------------------------------------------------------------------------------------------

class KeyEditorCanvas : public wxWindow {
public:
  KeyEditorCanvas(wxWindow* pParent);

private:
  void OnPaint(wxPaintEvent& event);
  void render(wxDC& dc);

  wxDECLARE_EVENT_TABLE();
};

//-------------------------------------------------------------------------------------------------
// KeyEditorWindow
//-------------------------------------------------------------------------------------------------

class KeyEditorWindow : public wxWindow {
public:
  KeyEditorWindow(wxWindow* pParent);

private:
  KeyEditorCanvas* pKeyEditorCanvas_{nullptr};
};

#endif // _KEY_EDITOR_H
