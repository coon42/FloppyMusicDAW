#include "trackeditor.h"

//-------------------------------------------------------------------------------------------------
// TrackEditorWindow
//-------------------------------------------------------------------------------------------------

TrackEditorWindow::TrackEditorWindow(wxWindow* pParent, Song* pSong)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize), pSong_(pSong) {

  pTrackWindow_ = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  pTrackWindow_->CreateGrid(1, 3);
  pTrackWindow_->HideRowLabels();
  pTrackWindow_->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectNone);

  pTrackWindow_->SetColLabelValue(0, "Selected");
  pTrackWindow_->SetColLabelValue(1, "Track Name");
  pTrackWindow_->SetColLabelValue(2, "Channel");

  pTrackWindow_->SetCellValue(0, 0, "X");
  pTrackWindow_->SetCellValue(0, 1, pSong_->track1()->name());
  pTrackWindow_->SetCellValue(0, 2, wxString::Format("%d", pSong_->track1()->midiChannel() + 1));

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
