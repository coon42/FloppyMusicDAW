#include "trackeditor.h"

//-------------------------------------------------------------------------------------------------
// TrackEditorWindow
//-------------------------------------------------------------------------------------------------

TrackEditorWindow::TrackEditorWindow(wxWindow* pParent, Song* pSong)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize), pSong_(pSong) {

  pTrackWindow_ = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  pTrackWindow_->CreateGrid(pSong_->numberOfTracks(), 3);
  pTrackWindow_->HideRowLabels();
  pTrackWindow_->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectNone);

  pTrackWindow_->SetColLabelValue(0, "Selected");
  pTrackWindow_->SetColLabelValue(1, "Track Name");
  pTrackWindow_->SetColLabelValue(2, "Channel");

  pTrackWindow_->SetColLabelSize(pTrackWindow_->GetCharHeight() + 4);

  wxSizer* pTopSizer = new wxBoxSizer(wxHORIZONTAL);
  pTopSizer->Add(pTrackWindow_);

  SetSizer(pTopSizer);

  updateTrackList();
}

void TrackEditorWindow::updateTrackList() {
  pTrackWindow_->DeleteRows(0, pTrackWindow_->GetNumberRows());

  for (int trackNo = 0; trackNo < pSong_->numberOfTracks(); ++trackNo) {
    pTrackWindow_->AppendRows(1);
    pTrackWindow_->SetCellValue(trackNo, 0, trackNo == 0 ? "X" : "");
    pTrackWindow_->SetCellValue(trackNo, 1, pSong_->track(trackNo)->name());
    pTrackWindow_->SetCellValue(trackNo, 2, wxString::Format("%d", pSong_->track(trackNo)->midiChannel() + 1));

    for (int row = 0; row < pTrackWindow_->GetNumberRows(); ++row) {
      for (int col = 0; col < pTrackWindow_->GetNumberCols(); ++col) {
        pTrackWindow_->SetReadOnly(row, col);

        pTrackWindow_->SetCellAlignment(row, col, wxALIGN_CENTER, 0);
        pTrackWindow_->SetCellAlignment(row, col, wxALIGN_CENTER, 0);
        pTrackWindow_->SetCellAlignment(row, col, wxALIGN_CENTER, 0);
      }
    }
  }
}
