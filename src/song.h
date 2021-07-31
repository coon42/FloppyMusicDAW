#ifndef _SONG_H
#define _SONG_H

#include <list>

//-------------------------------------------------------------------------------------------------
// NoteBlock
//-------------------------------------------------------------------------------------------------

class NoteBlock {
public:
  void setNote(uint8_t midiNote)        { note_ = midiNote; }
  void setStartTick(uint32_t startTick) { startTick_ = startTick; }
  void setNumTicks(uint32_t numTicks)   { numTicks_ = numTicks; }

  const uint8_t note() const            { return note_; }
  const uint32_t startTick() const      { return startTick_; }
  const uint32_t numTicks() const       { return numTicks_; }

private:
  uint8_t note_{0};
  uint32_t startTick_{0};
  uint32_t numTicks_{0};
};

//-------------------------------------------------------------------------------------------------
// Song
//-------------------------------------------------------------------------------------------------

struct Song {
  uint16_t TPQN;
  std::list<NoteBlock> noteBlocks;
};

#endif // _SONG_H
