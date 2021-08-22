#include "trackeditor.h"

//-------------------------------------------------------------------------------------------------
// TrackEditorWindow
//-------------------------------------------------------------------------------------------------

TrackEditorWindow::TrackEditorWindow(wxWindow* pParent)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize) {

  pTrackWindow_ = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  pTrackWindow_->CreateGrid(1, 3);
  pTrackWindow_->HideRowLabels();
  pTrackWindow_->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectNone);

  pTrackWindow_->SetColLabelValue(0, "Selected");
  pTrackWindow_->SetColLabelValue(1, "Track Name");
  pTrackWindow_->SetColLabelValue(2, "Channel");

  pTrackWindow_->SetCellValue(0, 0, "X");
  pTrackWindow_->SetCellValue(0, 1, "Track 1");
  pTrackWindow_->SetCellValue(0, 2, "1");

  for (int row = 0; row < pTrackWindow_->GetNumberRows(); ++row) {
    for (int col = 0; col < pTrackWindow_->GetNumberCols(); ++col) {
      pTrackWindow_->SetReadOnly(row, col);

      pTrackWindow_->SetCellAlignment(row, col, wxALIGN_CENTER, 0);
      pTrackWindow_->SetCellAlignment(row, col, wxALIGN_CENTER, 0);
      pTrackWindow_->SetCellAlignment(row, col, wxALIGN_CENTER, 0);
    }
  }

  pTrackWindow_->SetColLabelSize(pTrackWindow_->GetCharHeight() + 4);

  wxSizer* pTopSizer = new wxBoxSizer(wxHORIZONTAL);
  pTopSizer->Add(pTrackWindow_);

  SetSizer(pTopSizer);
}
