#ifndef _SONG_H
#define _SONG_H

#include <list>

struct NoteBlock {
  uint8_t note;
  uint32_t startTick;
  uint32_t numTicks;
};

struct Song {
  uint16_t TPQN;
  std::list<NoteBlock> noteBlocks;
};

#endif // _SONG_H
