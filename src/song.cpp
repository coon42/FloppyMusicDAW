#include <map>
#include <sstream>

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
// EmNoteOnEvent
//-------------------------------------------------------------------------------------------------

Error EmNoteOnEvent::write(uint32_t deltaTime) const {
  return eMidi_writeNoteOnEvent(pMidiFile_, deltaTime, channel(), note(), velocity());
}

//-------------------------------------------------------------------------------------------------
// EmNoteOffEvent
//-------------------------------------------------------------------------------------------------

Error EmNoteOffEvent::write(uint32_t deltaTime) const {
  return eMidi_writeNoteOffEvent(pMidiFile_, deltaTime, channel(), note(), velocity());
}

//-------------------------------------------------------------------------------------------------
// EmMetaSetTempoEvent
//-------------------------------------------------------------------------------------------------

Error EmMetaSetTempoEvent::write(uint32_t deltaTime) const {
  return eMidi_writeSetTempoMetaEvent(pMidiFile_, deltaTime, bpm());
}

//-------------------------------------------------------------------------------------------------
// Song
//-------------------------------------------------------------------------------------------------

void Song::clear() {
  tpqn_ = MIDI_DEFAULT_TPQN;
  tracks_.clear();
  metaTrack_ = MetaTrack(*this);

  tracks_.push_back(ChannelTrack(*this, "Track 1", 0));
}

uint64_t Song::durationUs() const {
  uint64_t longestDuration = 0;

  for (const ChannelTrack& track : tracks_) {
    const uint64_t curDuration = track.durationUs();

    if (curDuration > longestDuration)
      longestDuration = curDuration;
  }

  return longestDuration;
}

void Song::unselectAllEvents() {
  for (SongEvent* pSongEvent : tracks_[0].songEvents())
    pSongEvent->unselect();
}

void Song::debugPrintAllSongEvents() const {
  for (int trackNo = 0; trackNo < tracks_.size(); ++trackNo) {
    const Track& track = tracks_[trackNo];

    printf("Note events of track %d '%s':\n", trackNo + 1, track.name().c_str());

    track.debugPrintAllEvents();
  }   
}

void Song::importFromMidi0(const std::string& path) {
  MidiFile midiFile{0};

  if (Error error = eMidi_open(&midiFile, path.c_str())) {
    printf("Error on opening midi file!\n");
    return;
  }

  clear();
  tracks_.clear();

  setTpqn(midiFile.header.division.tpqn.TPQN);

  using OnNotesMap = std::map<uint8_t, NoteBlock>;
  std::map<uint8_t, OnNotesMap> trackOnNotes;
  std::map<uint8_t, uint8_t> channelToTrackNo;

  uint32_t currentTick = 0;
  MidiEvent midiEvent;

  while (eMidi_readEvent(&midiFile, &midiEvent) == EMIDI_OK) {
    if (midiEvent.eventId == 0xFF) // skip meta events for now.
      continue;

    const int eventId = midiEvent.eventId & 0xF0;
    const int channel = midiEvent.eventId & 0x0F;

    if (channelToTrackNo.find(channel) == channelToTrackNo.end()) {
      trackOnNotes[channel] = OnNotesMap();

      std::ostringstream trackName;
      trackName << "Track " << channel + 1;
      tracks_.push_back(ChannelTrack(*this, trackName.str(), channel));

      channelToTrackNo[channel] = static_cast<uint8_t>(tracks_.size() - 1);
    }

    const int trackNo = channelToTrackNo[channel];

    Track* pTrack = &tracks_[trackNo];
    OnNotesMap& onNotes = trackOnNotes[channel];

    currentTick += midiEvent.deltaTime;

    auto noteOn = [&](uint8_t note) {
      NoteBlock noteBlock;
      noteBlock.setNote(midiEvent.params.msg.noteOn.note);
      noteBlock.setStartTick(currentTick);

      onNotes[note] = noteBlock;
    };

    auto noteOff = [&](uint8_t note) {
      NoteBlock& noteBlock = onNotes[note];
      noteBlock.setNumTicks(currentTick - noteBlock.startTick());
      pTrack->addSongEvent(onNotes[note]);
      onNotes.erase(note);
    };

    switch (eventId) {
      case MIDI_EVENT_NOTE_ON: {
        const uint8_t note = midiEvent.params.msg.noteOn.note;

        if (midiEvent.params.msg.noteOn.velocity > 0) {
          if (onNotes.find(note) == onNotes.end()) // ignore, double additional note on event if already active
            noteOn(note);
        }
        else // Velocity of 0 means note off:
          noteOff(note);

        break;
      }

      case MIDI_EVENT_NOTE_OFF: {
        const uint8_t note = midiEvent.params.msg.noteOff.note;

        if (onNotes.find(note) != onNotes.end()) // ignore, if there is no matching note on event active
          noteOff(note);

        break;
      }

      default:
        pTrack->addSongEvent(NotImplementedEvent(currentTick, eventId, 0));
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

  for (const ChannelTrack& track : tracks_) {
    for (const SongEvent* pSongEvent : track.songEvents()) {
      if (pSongEvent->type() == SongEventType::NoteBlock) {
        const NoteBlock& noteBlock = *static_cast<const NoteBlock*>(pSongEvent);

        eventList.push_back(new EmNoteOnEvent(&midiFile, noteBlock.startTick(), track.midiChannel(), noteBlock.note(), MIDI_DEFAULT_VELOCITY));
        eventList.push_back(new EmNoteOffEvent(&midiFile, noteBlock.startTick() + noteBlock.numTicks(), track.midiChannel(), noteBlock.note(), MIDI_DEFAULT_VELOCITY));
      }
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

//-------------------------------------------------------------------------------------------------
// Track
//-------------------------------------------------------------------------------------------------

Track::Track(const Track& track)
  : Track(track.song_, track.name_) {
  operator = (track);
}

Track::~Track() {
  clear();
}

Track& Track::operator = (const Track& rhs) {
  clear();

  for (const SongEvent* pSongEvent : rhs.songEvents_)
    songEvents_.push_back(pSongEvent->clone());

  return *this;
}

void Track::clear() {
  for (SongEvent* pSongEvent : songEvents_)
    delete pSongEvent;

  songEvents_.clear();
}

void Track::debugPrintAllEvents() const {
  for (const SongEvent* pSongEvent : songEvents_) {
    switch (pSongEvent->type()) {
      case SongEventType::NotImplementedEvent: {
        const NotImplementedEvent& ne = *static_cast<const NotImplementedEvent*>(pSongEvent);

        printf("Not implemented event: ID: 0x%02X (%s)\n", ne.midiEventId(), eMidi_eventToStr(ne.midiEventId()));
      }

      case SongEventType::NoteBlock: {
        const NoteBlock& b = *static_cast<const NoteBlock*>(pSongEvent);

        printf("Note: %s, start: %d, numTicks: %d\n", eMidi_numberToNote(b.note()), b.startTick(), b.numTicks());
      }
    }    
  }
}

//-------------------------------------------------------------------------------------------------
// ChannelTrack
//-------------------------------------------------------------------------------------------------

uint64_t ChannelTrack::durationUs() const {
  static const uint32_t c = 60000000;
  uint32_t bpm = 120;
  uint32_t uspqn = c / bpm;
  uint32_t tpqn = song_.tpqn();

  uint64_t usTotal = 0;
  uint32_t lastTick = 0;

  for (const SongEvent* pEvent : songEvents()) {
    const uint32_t deltaTick = pEvent->startTick() + pEvent->numTicks() - lastTick;

    // TODO: set bpm on tempo change event

    usTotal += (deltaTick * uspqn) / tpqn;

    lastTick = pEvent->startTick() + pEvent->numTicks();
  }

  return usTotal;
}
