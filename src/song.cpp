extern "C" {
#include "lib/eMIDI/src/helpers.h"
}

#include "song.h"

//-------------------------------------------------------------------------------------------------
// Song
//-------------------------------------------------------------------------------------------------

void Song::clear() {
  tpqn_ = 0;
  noteBlocks_.clear();
}

void Song::unselectAllNotes() {
  for (NoteBlock& noteBlock : noteBlocks_)
    noteBlock.unselect();
}

void Song::debugPrintAllNoteBlocks() const {
  printf("Note blocks:\n");

  for (const NoteBlock& b : noteBlocks_)
    printf("Note: %s, start: %d, numTicks: %d\n", eMidi_numberToNote(b.note()), b.startTick(), b.numTicks());
}
