extern "C" {
#include "lib/eMIDI/src/helpers.h"
}

#include "song.h"

//-------------------------------------------------------------------------------------------------
// NoteOnEvent
//-------------------------------------------------------------------------------------------------

Error NoteOnEvent::write(uint32_t deltaTime) const {
  return eMidi_writeNoteOnEvent(pMidiFile_, deltaTime, 0, note(), velocity());
}

//-------------------------------------------------------------------------------------------------
// NoteOffEvent
//-------------------------------------------------------------------------------------------------

Error NoteOffEvent::write(uint32_t deltaTime) const {
  return eMidi_writeNoteOffEvent(pMidiFile_, deltaTime, 0, note(), velocity());
}

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

void Song::exportAsMidi0(const std::string& path) const {
  MidiFile midiFile;

  if (Error error = eMidi_create(&midiFile, path.c_str(), tpqn())) {
    eMidi_printError(error);
    return;
  }

  std::list<SongEvent*> eventList;

  for (const NoteBlock& noteBlock : noteBlocks()) {
    eventList.push_back(new NoteOnEvent(&midiFile, noteBlock.startTick(), 0, noteBlock.note(), MIDI_DEFAULT_VELOCITY));
    eventList.push_back(new NoteOffEvent(&midiFile, noteBlock.startTick() + noteBlock.numTicks(), 0, noteBlock.note(), MIDI_DEFAULT_VELOCITY));
  }

  // comparison, not case sensitive.
  auto absoluteTicksAscending = [](const SongEvent* pFirst, const SongEvent* pSecond) -> bool {
    return pFirst->absoluteTick() < pSecond->absoluteTick();
  };

  eventList.sort(absoluteTicksAscending);

  uint32_t lastTick = 0;

  for (SongEvent* pEvent : eventList) {
    const uint32_t deltaTick = pEvent->absoluteTick() - lastTick;

    if (Error error = pEvent->write(deltaTick))
      eMidi_printError(error);

    lastTick = pEvent->absoluteTick();
  }

  if (Error error = eMidi_writeEndOfTrackMetaEvent(&midiFile, 100)) {
    eMidi_printError(error);
    return;
  }

  if (Error error = eMidi_close(&midiFile))
    eMidi_printError(error);
}