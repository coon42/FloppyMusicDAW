#include <wx/wx.h>

extern "C" {
#include "lib/eMIDI/src/helpers.h"
}

#include "keyeditor.h"

//-------------------------------------------------------------------------------------------------
// KeyEditorCanvasCanvasSegment
//-------------------------------------------------------------------------------------------------

KeyEditorCanvasSegment::KeyEditorCanvasSegment(KeyEditorCanvas* pParent, const wxSize& size = wxDefaultSize)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, size) {

}

void KeyEditorCanvasSegment::render() {
  wxClientDC dc(this);
  dc.Clear();

  onRender(dc);
}

void KeyEditorCanvasSegment::OnPaint(wxPaintEvent& event) {
  render();
}

const KeyEditorCanvas* KeyEditorCanvasSegment::canvas() const {
  return static_cast<const KeyEditorCanvas*>(GetParent());
}

KeyEditorCanvas* KeyEditorCanvasSegment::canvas() {
  return static_cast<KeyEditorCanvas*>(GetParent());
}

wxBEGIN_EVENT_TABLE(KeyEditorCanvasSegment, wxWindow)
EVT_PAINT(KeyEditorCanvasSegment::OnPaint)
wxEND_EVENT_TABLE()

//-------------------------------------------------------------------------------------------------
// KeyEditorQuantizationCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorQuantizationCanvas::KeyEditorQuantizationCanvas(KeyEditorCanvas* pParent)
    : KeyEditorCanvasSegment(pParent, wxSize(0, 30)) {

}

void KeyEditorQuantizationCanvas::onRender(wxDC& dc) {
  const int canvasWidth = GetClientSize().GetWidth() - canvas()->xBlockStartOffset();
  const int numSegments = canvasWidth / canvas()->pixelsPerQuarterNote() + canvas()->pixelsPerQuarterNote();

  for (int segment = 0; segment < numSegments; ++segment) {
    const int xOffset = segment * canvas()->pixelsPerQuarterNote();
    const bool isFirst = (canvas()->xScrollOffset() + segment) % 4 == 0;
    const int yEndPos = isFirst ? GetClientSize().GetHeight() : 10;

    if (isFirst) {
      dc.SetPen(wxPen(wxColor(0, 0, 0), 1));
      dc.SetTextForeground(wxColor(0, 0, 0));
    }
    else {
      dc.SetPen(wxPen(wxColor(192, 192, 192), 1));
      dc.SetTextForeground(wxColor(128, 128, 128));
    }

    dc.DrawLine(canvas()->xBlockStartOffset() + xOffset, 0, canvas()->xBlockStartOffset() + xOffset, yEndPos);

    const int labelXoffset = isFirst ? xOffset + 5 : xOffset - 4;
    const int major = 1 + (canvas()->xScrollOffset() + segment) / 4;
    const int minor = 1 + (canvas()->xScrollOffset() + segment) % 4;

    wxString label;

    if (isFirst)
      label = wxString::Format("%d", major);
    else
      label = wxString::Format("%d.%d", major, minor);

    dc.DrawText(label, canvas()->xBlockStartOffset() + labelXoffset, 10);
  }
}

//-------------------------------------------------------------------------------------------------
// KeyEditorPianoCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorPianoCanvas::KeyEditorPianoCanvas(KeyEditorCanvas* pParent)
    : KeyEditorCanvasSegment(pParent, wxSize(50, 0)) {

}

void KeyEditorPianoCanvas::onRender(wxDC& dc) {
  dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick
  dc.SetTextForeground(wxColor(0, 0, 0)); // set text color

  for (int y = 0; y < MIDI_NUM_NOTES; ++y) {
    const int midiNote = MIDI_NUM_NOTES - 1 - y - canvas()->yScrollOffset();

    if (midiNote >= 0) {
      const int yOffset = y * canvas()->blockHeight();
      const char* pNoteStr = eMidi_numberToNote(midiNote);

      dc.DrawText(pNoteStr, 0, yOffset);
    }
  }
}

//-------------------------------------------------------------------------------------------------
// KeyEditorGridCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorGridCanvas::KeyEditorGridCanvas(KeyEditorCanvas* pParent, Song* pSong) : KeyEditorCanvasSegment(pParent),
  pSong_(pSong) {

}

void KeyEditorGridCanvas::onRender(wxDC& dc) {
  const wxSize& canvasSize = GetClientSize();
  const int canvasWidth = canvasSize.GetWidth() / canvas()->pixelsPerQuarterNote();
  const int numSegments = canvasWidth / canvas()->pixelsPerQuarterNote() + canvas()->pixelsPerQuarterNote();

  // draw divisions
  for (int segment = 0; segment < numSegments; ++segment) {
    const int xOffset = segment * canvas()->pixelsPerQuarterNote();
    const bool isFirst = (canvas()->xScrollOffset() + segment) % 4 == 0;

    if (isFirst) {
      dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick
      dc.SetTextForeground(wxColor(0, 0, 0)); // set text color
    }
    else {
      dc.SetPen(wxPen(wxColor(192, 192, 192), 1)); // black line, 1 pixels thick
      dc.SetTextForeground(wxColor(128, 128, 128)); // set text color
    }

    int numBlocksVisibleOnScreen = (canvasSize.GetHeight() - canvas()->blockHeight() / 2) / canvas()->blockHeight();

    if (numBlocksVisibleOnScreen + canvas()->yScrollOffset() > MIDI_NUM_NOTES)
      numBlocksVisibleOnScreen = MIDI_NUM_NOTES - canvas()->yScrollOffset();

    dc.DrawLine(xOffset, 0, xOffset, numBlocksVisibleOnScreen * canvas()->blockHeight());
  }

  // draw note lines
  dc.SetPen(wxPen(wxColor(0, 0, 0), 1)); // black line, 1 pixels thick
  dc.SetTextForeground(wxColor(0, 0, 0)); // set text color

  for (int y = 0; y < MIDI_NUM_NOTES; ++y) {
    const int midiNote = MIDI_NUM_NOTES - 1 - y - canvas()->yScrollOffset();

    if (midiNote >= 0) {
      const int yOffset = y * canvas()->blockHeight();
      dc.DrawLine(0, yOffset, canvasSize.GetWidth(), yOffset);

      if (midiNote == 0) {
        const int yOffsetLast = (y + 1) * canvas()->blockHeight();
        dc.DrawLine(0, yOffsetLast, canvasSize.GetWidth(), yOffsetLast);
      }
    }
  }

  // draw note blocks
  std::list<SongEvent*> eventsOfCurrentTrack = pSong_->currentSelectedTrack()->songEvents();

  for (const SongEvent* pSongEvent : eventsOfCurrentTrack) {
    if (pSongEvent->type() == SongEventType::NoteBlock) {
      const NoteBlock& noteBlock = *static_cast<const NoteBlock*>(pSongEvent);
      const BlockDimensions bd = getVisibleNoteBlockDimensions(noteBlock);

      if (noteBlock.isSelected())
        dc.SetBrush(wxBrush(wxColour(0, 255, 255)));
      else
        dc.SetBrush(wxBrush(wxColour(0, 255, 0)));

      dc.DrawRectangle(bd.x, bd.y, bd.width, canvas()->blockHeight());
    }
  }
}

KeyEditorGridCanvas::BlockDimensions KeyEditorGridCanvas::getAbsoluteNoteBlockDimensions(const NoteBlock& noteBlock) const {
  BlockDimensions bd;
  bd.x = (noteBlock.startTick() * canvas()->pixelsPerQuarterNote()) / pSong_->tpqn();
  bd.y = canvas()->blockHeight() * (127 - noteBlock.note());
  bd.width = (noteBlock.numTicks() * canvas()->pixelsPerQuarterNote()) / pSong_->tpqn();

  return bd;
}

KeyEditorGridCanvas::BlockDimensions KeyEditorGridCanvas::getVisibleNoteBlockDimensions(const NoteBlock& noteBlock) const {
  BlockDimensions bd = getAbsoluteNoteBlockDimensions(noteBlock);
  bd.x -= canvas()->xScrollOffset() * canvas()->pixelsPerQuarterNote();
  bd.y -= canvas()->blockHeight() * canvas()->yScrollOffset();

  if ((bd.x + bd.width > 0) && (bd.y >= 0)) {
    if (bd.x < 0) {
      const int cutPixels = -bd.x;
      bd.x += cutPixels;
      bd.width -= cutPixels;
    }
  }

  return bd;
}

KeyEditorGridCanvas::CellPosition KeyEditorGridCanvas::currentPointedCell(int mouseX, int mouseY) {
  const int xOffset = canvas()->xScrollOffset() * canvas()->pixelsPerQuarterNote();
  const int yOffset = canvas()->yScrollOffset() * canvas()->blockHeight();

  CellPosition pos;
  pos.absoluteXindex = (mouseX + xOffset) / canvas()->pixelsPerQuarterNote();
  pos.absoluteYindex = (mouseY + yOffset) / canvas()->blockHeight();

  if (pos.absoluteYindex > (MIDI_NUM_NOTES - 1))
    pos.absoluteYindex = MIDI_NUM_NOTES - 1;

  pos.relativeXindex = pos.absoluteXindex - xOffset / canvas()->pixelsPerQuarterNote();
  pos.relativeYindex = pos.absoluteYindex - yOffset / canvas()->blockHeight();

  return pos;
}

KeyEditorGridCanvas::CellPosition KeyEditorGridCanvas::currentPointedCell() {
  const wxPoint pt = wxGetMousePosition();
  const int mouseX = pt.x - GetScreenPosition().x;
  const int mouseY = pt.y - GetScreenPosition().y;

  return currentPointedCell(mouseX, mouseY);
}

KeyEditorGridCanvas::ResizeArea KeyEditorGridCanvas::noteBlockResizeArea(const NoteBlock& noteBlock, int mouseX, int mouseY) const {
  const BlockDimensions bd = getVisibleNoteBlockDimensions(noteBlock);

  const int margin = 10;

  if (mouseX > bd.x && mouseX < bd.x + margin)
    return ResizeArea::Left;
  else if (mouseX > bd.x + bd.width - margin)
    return ResizeArea::Right;
  else
    return ResizeArea::None;
}

NoteBlock* KeyEditorGridCanvas::currentPointedNoteBlock(int mouseX, int mouseY) {
  for (SongEvent* pSongEvent : pSong_->currentSelectedTrack()->songEvents()) {
    if (pSongEvent->type() == SongEventType::NoteBlock) {
      NoteBlock& noteBlock = *static_cast<NoteBlock*>(pSongEvent);

      const BlockDimensions bd = getVisibleNoteBlockDimensions(noteBlock);

      if (mouseX > bd.x && mouseX < bd.x + bd.width && mouseY > bd.y && mouseY < bd.y + canvas()->blockHeight())
        return &noteBlock;
    }
  }

  return nullptr;
}

void KeyEditorGridCanvas::OnMouseLeftDown(wxMouseEvent& event) {
  const char* pClickedTarget = "None";

  const int mouseX = event.GetX();
  const int mouseY = event.GetY();

  if (NoteBlock* pNoteBlock = currentPointedNoteBlock(mouseX, mouseY)) {
    switch (noteBlockResizeArea(*pNoteBlock, mouseX, mouseY)) {
      case ResizeArea::Left:
        pCurrentEditNoteBlock_ = pNoteBlock;
        editState_ = EditState::ResizingNoteLeft;
        break;

      case ResizeArea::Right:
        pCurrentEditNoteBlock_ = pNoteBlock;
        editState_ = EditState::ResizingNoteRight;
        break;

      case ResizeArea::None:
        const BlockDimensions dm = getVisibleNoteBlockDimensions(*pNoteBlock);
        pCurrentEditNoteBlock_ = pNoteBlock;
        editStartBlockXClickPosition_ = mouseX - dm.x;
        editState_ = EditState::Moving;
        break;
    }

    pSong_->unselectAllEvents();
    pNoteBlock->select();
    pClickedTarget = eMidi_numberToNote(pNoteBlock->note());
  }
  else
    pSong_->unselectAllEvents();

  printf("clicked on: %s\n", pClickedTarget);
  render();
}

void KeyEditorGridCanvas::OnMouseLeftUp(wxMouseEvent& event) {
  pCurrentEditNoteBlock_ = nullptr;
  editStartBlockXClickPosition_ = 0;
  editState_ = EditState::Idle;
}

void KeyEditorGridCanvas::OnPaint(wxPaintEvent& event) {
  wxPaintDC dc(this);
  onRender(dc);
}

void KeyEditorGridCanvas::OnMouseMotion(wxMouseEvent& event) {
  const int mouseX = event.GetX();
  const int mouseY = event.GetY();
  const int mouseXabs = event.GetX() + canvas()->xScrollOffset() * canvas()->pixelsPerQuarterNote();

  switch (editState_) {
    case EditState::ResizingNoteRight: {
      const BlockDimensions dm = getAbsoluteNoteBlockDimensions(*pCurrentEditNoteBlock_);
      const int newWidth = mouseXabs - dm.x;

      if (newWidth <= 30)
        break;

      const int newTicks = (newWidth * pSong_->tpqn()) / canvas()->pixelsPerQuarterNote();

      pCurrentEditNoteBlock_->setNumTicks(newTicks);
      render();

      break;
    }

    case EditState::ResizingNoteLeft: {
      const BlockDimensions dm = getAbsoluteNoteBlockDimensions(*pCurrentEditNoteBlock_);
      const int newStart = (mouseXabs * pSong_->tpqn()) / canvas()->pixelsPerQuarterNote();
      const int newWidth = dm.x + dm.width - mouseXabs;

      if (newWidth <= 30)
        break;

      const int newTicks = pCurrentEditNoteBlock_->numTicks() + pCurrentEditNoteBlock_->startTick() - newStart;

      pCurrentEditNoteBlock_->setStartTick(newStart);
      pCurrentEditNoteBlock_->setNumTicks(newTicks);
      render();

      break;
    }

    case EditState::Moving: {
      const BlockDimensions dm = getAbsoluteNoteBlockDimensions(*pCurrentEditNoteBlock_);
      int newStart = ((mouseXabs - editStartBlockXClickPosition_) * pSong_->tpqn()) / canvas()->pixelsPerQuarterNote();

      if (newStart < 0)
        newStart = 0;

      const CellPosition pos = currentPointedCell(mouseX, mouseY);
      const uint8_t newNote = static_cast<uint8_t>(127 - pos.absoluteYindex);

      pCurrentEditNoteBlock_->setNote(newNote);
      pCurrentEditNoteBlock_->setStartTick(newStart);
      render();

      break;
    }

    default: {
      if (NoteBlock* pNoteBlock = currentPointedNoteBlock(mouseX, mouseY)) {
        // TODO: - Only set mouse arrow when area changes.
        //       - Why is default arrow set automatically when leaving a note block?

        switch (noteBlockResizeArea(*pNoteBlock, mouseX, mouseY)) {
          case ResizeArea::Left:
          case ResizeArea::Right:
            wxSetCursor(wxCursor(wxCURSOR_SIZEWE));
            break;

          case ResizeArea::None:
            wxSetCursor(wxCursor(wxCURSOR_SIZING));
            break;
        }
      }
    }
  }
}

wxBEGIN_EVENT_TABLE(KeyEditorGridCanvas, wxWindow)
EVT_PAINT(KeyEditorGridCanvas::OnPaint)
EVT_MOTION(KeyEditorGridCanvas::OnMouseMotion)
EVT_LEFT_DOWN(KeyEditorGridCanvas::OnMouseLeftDown)
EVT_LEFT_UP(KeyEditorGridCanvas::OnMouseLeftUp)
wxEND_EVENT_TABLE()

//-------------------------------------------------------------------------------------------------
// KeyEditorCanvas
//-------------------------------------------------------------------------------------------------

KeyEditorCanvas::KeyEditorCanvas(wxWindow* pParent, Song* const pSong)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize) {

  wxSizer* pTopSizer = new wxBoxSizer(wxVERTICAL);

  pKeyEditorQuantizationCanvas_ = new KeyEditorQuantizationCanvas(this);
  pKeyEditorPianoCanvas_ = new KeyEditorPianoCanvas(this);
  pKeyEditorGridCanvas_ = new KeyEditorGridCanvas(this, pSong);

  wxSizer* pPianoGridSizer = new wxBoxSizer(wxHORIZONTAL);
  pPianoGridSizer->Add(pKeyEditorPianoCanvas_, 0, wxEXPAND);
  pPianoGridSizer->Add(pKeyEditorGridCanvas_, 1, wxEXPAND);

  pTopSizer->Add(pKeyEditorQuantizationCanvas_, 0, wxEXPAND);
  pTopSizer->Add(pPianoGridSizer, 1, wxEXPAND);

  SetSizer(pTopSizer);
}

void KeyEditorCanvas::render() {
  pKeyEditorQuantizationCanvas_->render();
  pKeyEditorPianoCanvas_->render();
  pKeyEditorGridCanvas_->render();
}

void KeyEditorCanvas::setXscrollPosition(int xScrollPosition) {
  xScrollOffset_ = xScrollPosition;
  render();
}

void KeyEditorCanvas::setYscrollPosition(int yScrollPosition) {
  yScrollOffset_ = yScrollPosition;
  render();
}

void KeyEditorCanvas::setXzoomFactor(int xZoomFactor) {
  pixelsPerQuarterNote_ = (1 + xZoomFactor) * 10;
  render();
}

void KeyEditorCanvas::setYzoomFactor(int yZoomFactor) {
  blockHeight_ = (1 + yZoomFactor) * 10;
  render();
}

//-------------------------------------------------------------------------------------------------
// KeyEditorWindow
//-------------------------------------------------------------------------------------------------

KeyEditorWindow::KeyEditorWindow(wxWindow* pParent, Song* pSong)
    : wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN),
      pSong_(pSong) {

#ifdef __linux__
  constexpr int controlBarWidth = 26; // minimum size allowed by GTK
#elif _WIN32
  constexpr int controlBarWidth = 10;
#endif // __linux__ | _WIN32_

  constexpr int zoomSliderWidth = 100;

  pHorizontalScrollbar_ = new wxScrollBar(this, static_cast<int>(KeyEditorCanvas::ScrollBarType::HorizontalScroll), wxDefaultPosition, wxSize(0, controlBarWidth), wxHORIZONTAL);
  pHorizontalZoomSlider_ = new wxSlider(this, static_cast<int>(KeyEditorCanvas::ScrollBarType::HorizontalZoom), 0, 0, 1, wxDefaultPosition, wxSize(zoomSliderWidth, controlBarWidth), wxHORIZONTAL);

  pVerticalScrollbar_ = new wxScrollBar(this, static_cast<int>(KeyEditorCanvas::ScrollBarType::VerticalScroll), wxDefaultPosition, wxSize(controlBarWidth, 0), wxVERTICAL);
  pVerticalZoomSlider_ = new wxSlider(this, static_cast<int>(KeyEditorCanvas::ScrollBarType::VerticalZoom), 0, 0, 1, wxDefaultPosition, wxSize(controlBarWidth, zoomSliderWidth), wxVERTICAL);

  wxSizer* pHorizontalBarSizer = new wxBoxSizer(wxHORIZONTAL);
  pHorizontalBarSizer->Add(pHorizontalScrollbar_, 1);
  pHorizontalBarSizer->Add(pHorizontalZoomSlider_, 0);

  wxSizer* pVerticalBarSizer = new wxBoxSizer(wxVERTICAL);
  pVerticalBarSizer->Add(pVerticalScrollbar_, 1);
  pVerticalBarSizer->Add(pVerticalZoomSlider_, 0);

  pKeyEditorCanvas_ = new KeyEditorCanvas(this, pSong_);

  wxFlexGridSizer* pTopSizer = new wxFlexGridSizer(2, 2, wxSize(0, 0));
  pTopSizer->Add(pKeyEditorCanvas_, 1, wxEXPAND);
  pTopSizer->Add(pVerticalBarSizer, 1, wxEXPAND);
  pTopSizer->Add(pHorizontalBarSizer, 1, wxEXPAND);

  pTopSizer->AddGrowableCol(0, 1);
  pTopSizer->AddGrowableRow(0, 1);

  SetSizer(pTopSizer);

  setDefaultScrollPositions();
}

void KeyEditorWindow::render() {
  pKeyEditorCanvas_->render();
}

void KeyEditorWindow::setDefaultScrollPositions() {
  pHorizontalScrollbar_->SetScrollbar(0, 1, 128, 1);
  pVerticalScrollbar_->SetScrollbar(24, 1, MIDI_NUM_NOTES, 1);

  pHorizontalZoomSlider_->SetMin(0);
  pHorizontalZoomSlider_->SetMax(10);
  pHorizontalZoomSlider_->SetValue(5);

  pVerticalZoomSlider_->SetMin(0);
  pVerticalZoomSlider_->SetMax(10);
  pVerticalZoomSlider_->SetValue(0);

  pKeyEditorCanvas_->setXzoomFactor(pHorizontalZoomSlider_->GetValue());
  pKeyEditorCanvas_->setYzoomFactor(pVerticalZoomSlider_->GetValue());
  pKeyEditorCanvas_->setXscrollPosition(pHorizontalScrollbar_->GetThumbPosition());
  pKeyEditorCanvas_->setYscrollPosition(pVerticalScrollbar_->GetThumbPosition());
}

void KeyEditorWindow::OnScroll(wxScrollEvent& event) {
  printf("KeyEditorWindow::OnScroll; ");

  switch (static_cast<KeyEditorCanvas::ScrollBarType>(event.GetId())) {
    case KeyEditorCanvas::ScrollBarType::VerticalScroll:
      printf("Vertical Scroll: pos: %d\n", event.GetPosition());

      pKeyEditorCanvas_->setYscrollPosition(event.GetPosition());
      break;

    case KeyEditorCanvas::ScrollBarType::HorizontalScroll:
      printf("Horizontal Scroll: pos: %d\n", event.GetPosition());

      pKeyEditorCanvas_->setXscrollPosition(event.GetPosition());
      break;

    case KeyEditorCanvas::ScrollBarType::VerticalZoom:
      printf("Vertical Zoom: pos: %d\n", event.GetPosition());

      pKeyEditorCanvas_->setYzoomFactor(event.GetPosition());
      break;

    case KeyEditorCanvas::ScrollBarType::HorizontalZoom:
      printf("Horizontal Zoom: pos: %d\n", event.GetPosition());

      pKeyEditorCanvas_->setXzoomFactor(event.GetPosition());
      break;

    default:
      printf("unknown scrollbar ID: %d, pos %d\n", event.GetId(), event.GetPosition());
      // sliders do send this event!?
      break;
  }
}

void KeyEditorWindow::OnMouseWheel(wxMouseEvent& event) {
  const int scrollStep = 4;

  switch (event.GetWheelAxis()) {
    case wxMOUSE_WHEEL_VERTICAL:
      pVerticalScrollbar_->SetThumbPosition(pVerticalScrollbar_->GetThumbPosition() + (event.GetWheelRotation() > 0 ? -scrollStep : scrollStep));
      pKeyEditorCanvas_->setYscrollPosition(pVerticalScrollbar_->GetThumbPosition());
      break;

    case wxMOUSE_WHEEL_HORIZONTAL:
      pHorizontalScrollbar_->SetThumbPosition(pHorizontalScrollbar_->GetThumbPosition() + (event.GetWheelRotation() > 0 ? scrollStep : -scrollStep));
      pKeyEditorCanvas_->setXscrollPosition(pHorizontalScrollbar_->GetThumbPosition());
      break;
  }
}

wxBEGIN_EVENT_TABLE(KeyEditorWindow, wxWindow)
EVT_SCROLL(KeyEditorWindow::OnScroll)
EVT_MOUSEWHEEL(KeyEditorWindow::OnMouseWheel)
wxEND_EVENT_TABLE()
