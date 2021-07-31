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

class Song {
public:
  void clear();
  void setTpqn(uint16_t tpqn)                    { tpqn_ = tpqn; }
  void addNoteBlock(const NoteBlock& noteBlock)  { noteBlocks_.push_back(noteBlock); }
  void debugPrintAllNoteBlocks() const;

  const uint16_t tpqn() const                    { return tpqn_; }
  const std::list<NoteBlock>& noteBlocks() const { return noteBlocks_; }

private:
  uint16_t tpqn_{0};
  std::list<NoteBlock> noteBlocks_;
};

#endif // _SONG_H
