#include <map>

extern "C" {
#include "lib/eMIDI/src/helpers.h"
}

#include "song.h"

//-------------------------------------------------------------------------------------------------
// EmMidiEvent
//-------------------------------------------------------------------------------------------------

EmMidiEvent::~EmMidiEvent() {

}

//-------------------------------------------------------------------------------------------------
// NoteOnEvent
//-------------------------------------------------------------------------------------------------

Error NoteOnEvent::write(uint32_t deltaTime) const {
  return eMidi_writeNoteOnEvent(pMidiFile_, deltaTime, channel(), note(), velocity());
}

//-------------------------------------------------------------------------------------------------
// NoteOffEvent
//-------------------------------------------------------------------------------------------------

Error NoteOffEvent::write(uint32_t deltaTime) const {
  return eMidi_writeNoteOffEvent(pMidiFile_, deltaTime, channel(), note(), velocity());
}

//-------------------------------------------------------------------------------------------------
// Song
//-------------------------------------------------------------------------------------------------

void Song::clear() {
  tpqn_ = MIDI_DEFAULT_TPQN;
  tracks_.clear();

  tracks_.push_back(Track("Track 1", 0));
}

void Song::unselectAllNotes() {
  for (NoteBlock& noteBlock : tracks_[0].noteBlocks())
    noteBlock.unselect();
}

void Song::debugPrintAllNoteBlocks() const {
  for (int trackNo = 0; trackNo < tracks_.size(); ++trackNo) {
    const Track& track = tracks_[trackNo];

    printf("Note blocks of track %d '%s':\n", trackNo + 1, track.name().c_str());

    for (const NoteBlock& b : track.noteBlocks())
      printf("Note: %s, start: %d, numTicks: %d\n", eMidi_numberToNote(b.note()), b.startTick(), b.numTicks());
  }
}

void Song::importFromMidi0(const std::string& path) {
  MidiFile midiFile{0};

  if (Error error = eMidi_open(&midiFile, path.c_str())) {
    printf("Error on opening midi file!\n");
    return;
  }

  clear();
  setTpqn(midiFile.header.division.tpqn.TPQN);

  std::map<uint8_t, NoteBlock> onNotes;

  uint32_t currentTick = 0;
  MidiEvent midiEvent;
  while (eMidi_readEvent(&midiFile, &midiEvent) == EMIDI_OK) {
    currentTick += midiEvent.deltaTime;

    switch (midiEvent.eventId) {
      case MIDI_EVENT_NOTE_ON: {
        const uint8_t note = midiEvent.params.msg.noteOn.note;

        if (onNotes.find(note) == onNotes.end()) { // ignore, double additional note on event if already active
          if (midiEvent.params.msg.noteOn.velocity > 0) {
            NoteBlock block;
            block.setNote(midiEvent.params.msg.noteOn.note);
            block.setStartTick(currentTick);

            onNotes[note] = block;
          }
          else {
            onNotes[note].setNumTicks(currentTick - onNotes[note].startTick());
            tracks_[0].addNoteBlock(onNotes[note]);
            onNotes.erase(note);
          }
        }

        break;
      }

      case MIDI_EVENT_NOTE_OFF: {
        const uint8_t note = midiEvent.params.msg.noteOff.note;

        if (onNotes.find(note) != onNotes.end()) { // ignore, if there is no matching note on event active
          NoteBlock& noteBlock = onNotes[note];
          noteBlock.setNumTicks(currentTick - noteBlock.startTick());

          tracks_[0].addNoteBlock(onNotes[note]);
          onNotes.erase(note);
        }

        break;
      }

      default:
        break;
    }
  }

  if (Error error = eMidi_printFileInfo(&midiFile)) {
    printf("Error on printing MIDI file info!\n");
    return;
  }

  if (Error error = eMidi_close(&midiFile)) {
    printf("Error on closing midi file!\n");
    return;
  }
}

void Song::exportAsMidi0(const std::string& path) const {
  MidiFile midiFile;

  if (Error error = eMidi_create(&midiFile, path.c_str(), tpqn())) {
    eMidi_printError(error);
    return;
  }

  std::list<EmMidiEvent*> eventList;

  for (const Track& track : tracks_) {
    for (const NoteBlock& noteBlock : track.noteBlocks()) {
      eventList.push_back(new NoteOnEvent(&midiFile, noteBlock.startTick(), track.midiChannel(), noteBlock.note(), MIDI_DEFAULT_VELOCITY));
      eventList.push_back(new NoteOffEvent(&midiFile, noteBlock.startTick() + noteBlock.numTicks(), track.midiChannel(), noteBlock.note(), MIDI_DEFAULT_VELOCITY));
    }
  }

  // comparison, not case sensitive.
  auto absoluteTicksAscending = [](const EmMidiEvent* pFirst, const EmMidiEvent* pSecond) -> bool {
    return pFirst->absoluteTick() < pSecond->absoluteTick();
  };

  eventList.sort(absoluteTicksAscending);

  uint32_t lastTick = 0;

  for (const EmMidiEvent* pEvent : eventList) {
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
