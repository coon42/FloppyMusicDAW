#include "transport.h"

//-------------------------------------------------------------------------------------------------
// TransportWindow
//-------------------------------------------------------------------------------------------------

TransportWindow::TransportWindow(wxWindow* pParent, Song* pSong)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize), pSong_(pSong) {

  wxButton* pRewind = new wxButton(this, wxID_ANY, "<<");
  wxButton* pStop = new wxButton(this, wxID_ANY, "[ ]");
  wxButton* pPlay = new wxButton(this, wxID_ANY, ">");

  pRewind->Disable();
  pStop->Disable();
  pPlay->Disable();

  wxSizer* pButtonSizer = new wxBoxSizer(wxHORIZONTAL);
  pButtonSizer->Add(pRewind);
  pButtonSizer->Add(pStop);
  pButtonSizer->Add(pPlay);

  wxWindow* pTxtContainer = new wxWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
  pTotalTime_ = new wxStaticText(pTxtContainer, wxID_ANY, "Undefined");

  wxSizer* pTimeSizer = new wxBoxSizer(wxVERTICAL);
  pTimeSizer->Add(pTxtContainer, wxSizerFlags().Right());

  pTopSizer_ = new wxBoxSizer(wxHORIZONTAL);
  pTopSizer_->Add(pButtonSizer, 0, wxALIGN_BOTTOM);
  pTopSizer_->Add(pTimeSizer, 1, wxALIGN_CENTER);

  SetSizer(pTopSizer_);

  update();
}

void TransportWindow::update() {
  uint64_t us = pSong_->durationUs();

  uint32_t m  = (us / 60) / 1000000;
  uint32_t s  = us        / 1000000 - m * 60;
  uint32_t roundedMs = (us - m * 60 * 1000000 - s * 1000000 + 500) / 1000;

  pTotalTime_->SetLabelMarkup(wxString::Format(
      "<span background='black' foreground='lime' size='%d'>%02d:%02d:%03d</span>", 30 * 1024, m, s, roundedMs));
}

