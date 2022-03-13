#include "trackeditor.h"

//-------------------------------------------------------------------------------------------------
// TrackEditorWindow
//-------------------------------------------------------------------------------------------------

TrackEditorWindow::TrackEditorWindow(wxWindow* pParent, Song* pSong)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize), pSong_(pSong) {

  pTrackListGrid_ = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  pTrackListGrid_->CreateGrid(pSong_->numberOfTracks(), 4);
  pTrackListGrid_->HideRowLabels();
  pTrackListGrid_->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectNone);

  pTrackListGrid_->SetColLabelValue(0, "Selected");
  pTrackListGrid_->SetColLabelValue(1, "Track Name");
  pTrackListGrid_->SetColLabelValue(2, "Channel");
  pTrackListGrid_->SetColLabelValue(3, "Duration");

  pTrackListGrid_->SetColLabelSize(pTrackListGrid_->GetCharHeight() + 4);

  pTopSizer_ = new wxBoxSizer(wxHORIZONTAL);
  pTopSizer_->Add(pTrackListGrid_);

  SetSizer(pTopSizer_);

  updateTrackList();
}

void TrackEditorWindow::updateTrackList() {
  if (pTrackListGrid_->GetNumberRows())
    pTrackListGrid_-> DeleteRows(0, pTrackListGrid_->GetNumberRows());

  for (int trackNo = 0; trackNo < pSong_->numberOfTracks(); ++trackNo) {
    pTrackListGrid_->AppendRows(1);
    pTrackListGrid_->SetCellValue(trackNo, 0, trackNo == 0 ? "X" : "");
    pTrackListGrid_->SetCellValue(trackNo, 1, pSong_->track(trackNo)->name());
    pTrackListGrid_->SetCellValue(trackNo, 2, wxString::Format("%d", pSong_->track(trackNo)->midiChannel() + 1));
        
    const uint64_t us = pSong_->track(trackNo)->durationUs();    
    const uint32_t m  = (us / 60) / 1000000;
    const uint32_t s  = us / 1000000 - m * 60;
    const uint32_t roundedMs = (us - m * 60 * 1000000 - s * 1000000 + 500) / 1000;  
        
    pTrackListGrid_->SetCellValue(trackNo, 3, wxString::Format("%02d:%02d:%03d", m, s, roundedMs));
    
    for (int row = 0; row < pTrackListGrid_->GetNumberRows(); ++row) {
      for (int col = 0; col < pTrackListGrid_->GetNumberCols(); ++col) {
        pTrackListGrid_->SetReadOnly(row, col);
        pTrackListGrid_->SetCellAlignment(row, col, wxALIGN_CENTER, 0);
      }
    }
  }

  GetParent()->Layout();
}

