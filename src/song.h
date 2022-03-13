#ifndef _SONG_H
#define _SONG_H

#include <list>
#include <string>
#include <vector>

extern "C" {
#include "lib/eMIDI/src/midifile.h"
}

constexpr int NUM_MIDI_NOTES = 128;

//-------------------------------------------------------------------------------------------------
// EmMidiEvent
//-------------------------------------------------------------------------------------------------

class EmMidiEvent {
public:
  EmMidiEvent(MidiFile* pMidiFile, uint8_t eventId, uint32_t absoluteTick)
      : pMidiFile_(pMidiFile), eventId_(eventId), absoluteTick_(absoluteTick) {}

  virtual ~EmMidiEvent()                        = 0;  
  virtual Error write(uint32_t deltaTime) const = 0;

  uint8_t eventId() const                     { return eventId_; };
  uint32_t absoluteTick() const               { return absoluteTick_; }

protected:
  MidiFile* pMidiFile_{nullptr};

private:
  const uint8_t eventId_;
  const uint32_t absoluteTick_;
};

//-------------------------------------------------------------------------------------------------
// EmNoteEvent
//-------------------------------------------------------------------------------------------------

class EmNoteEvent : public EmMidiEvent {
public:
  EmNoteEvent(MidiFile* pMidiFile, uint8_t eventId, uint32_t absoluteTick, uint8_t channel, uint8_t note,
      uint8_t velocity)
    : EmMidiEvent(pMidiFile, eventId, absoluteTick), channel_(channel), note_(note), velocity_(velocity) {}
  
  Error write(uint32_t deltaTime) const override = 0;

  uint8_t channel()  const                    { return channel_; }
  uint8_t note()     const                    { return note_; }
  uint8_t velocity() const                    { return velocity_; }

private:
  const uint8_t channel_;
  const uint8_t note_;
  const uint8_t velocity_;
};

//-------------------------------------------------------------------------------------------------
// EmNoteOnEvent
//-------------------------------------------------------------------------------------------------

class EmNoteOnEvent : public EmNoteEvent {
public:
  EmNoteOnEvent(MidiFile* pMidiFile, uint32_t absoluteTick, uint8_t channel, uint8_t note, uint8_t velocity)
      : EmNoteEvent(pMidiFile, MIDI_EVENT_NOTE_ON, absoluteTick, channel, note, velocity) {}
  
  Error write(uint32_t deltaTime) const final;
};

//-------------------------------------------------------------------------------------------------
// EmNoteOffEvent
//-------------------------------------------------------------------------------------------------

class EmNoteOffEvent : public EmNoteEvent {
public:
  EmNoteOffEvent(MidiFile* pMidiFile, uint32_t absoluteTick, uint8_t channel, uint8_t note, uint8_t velocity)
      : EmNoteEvent(pMidiFile, MIDI_EVENT_NOTE_OFF, absoluteTick, channel, note, velocity) {}
  
  Error write(uint32_t deltaTime) const final;
};

//-------------------------------------------------------------------------------------------------
// EmMetaEvent
//-------------------------------------------------------------------------------------------------

class EmMetaEvent : public EmMidiEvent {
public:
  EmMetaEvent(MidiFile* pMidiFile, uint8_t metaEventId, uint32_t absoluteTick)
      : EmMidiEvent(pMidiFile, MIDI_EVENT_META, absoluteTick), metaEventId_(metaEventId) {}
    
  uint8_t metaEventId() const { return metaEventId_; }
  Error write(uint32_t deltaTime) const override = 0;

private:
  const uint8_t metaEventId_;
};

//-------------------------------------------------------------------------------------------------
// EmMetaSetTempoEvent
//-------------------------------------------------------------------------------------------------

class EmMetaSetTempoEvent : public EmMetaEvent {
public:
  uint8_t bpm() const                         { return bpm_; }
  EmMetaSetTempoEvent(MidiFile* pMidiFile, uint32_t absoluteTick, uint32_t bpm)
      : EmMetaEvent(pMidiFile, MIDI_SET_TEMPO, absoluteTick), bpm_(bpm) {}
  
  Error write(uint32_t deltaTime) const final;

private:
  const uint32_t bpm_;
};

//-------------------------------------------------------------------------------------------------
// SongEvent
//-------------------------------------------------------------------------------------------------

enum class SongEventType {
  Undefined,
  NotImplementedEvent,
  NoteBlock
};

class SongEvent {
public:
  virtual SongEvent* clone() const = 0;
  virtual SongEventType type() const = 0;

  void setStartTick(uint32_t startTick)   { startTick_ = startTick; }
  void setNumTicks(uint32_t numTicks)     { numTicks_ = numTicks; }
  void select()                           { isSelected_ = true; }
  void unselect()                         { isSelected_ = false; }

  const uint32_t startTick() const        { return startTick_; }
  const uint32_t numTicks() const         { return numTicks_; }
  const bool isSelected() const           { return isSelected_; }

private:
  uint32_t startTick_{0};
  uint32_t numTicks_{0};
  bool isSelected_{false};
};

//-------------------------------------------------------------------------------------------------
// NotImplementedEvent
//-------------------------------------------------------------------------------------------------

class NotImplementedEvent : public SongEvent {
public:
  NotImplementedEvent(uint32_t startTick, uint32_t numTicks) {
    setStartTick(startTick);
    setNumTicks(numTicks);
  }

  SongEvent* clone() const final   { return new NotImplementedEvent(*this); }
  SongEventType type() const final { return SongEventType::NotImplementedEvent; }
};

//-------------------------------------------------------------------------------------------------
// NoteBlock
//-------------------------------------------------------------------------------------------------

class NoteBlock : public SongEvent {
public:
  SongEvent* clone() const final    { return new NoteBlock(*this); }
  SongEventType type() const final  { return SongEventType::NoteBlock; }

  void setNote(uint8_t midiNote)    { note_ = midiNote; }
  const uint8_t note() const        { return note_; }

private:
  uint8_t note_{0};
};

//-------------------------------------------------------------------------------------------------
// Track
//-------------------------------------------------------------------------------------------------

class Song;

class Track {
public:
  Track(const Song& song, std::string name, int midiChannel)
      : song_(song), name_(name), midiChannel_(midiChannel) {};
  Track(const Track& track);
  ~Track();

  void addSongEvent(const SongEvent& songEvent)   { songEvents_.push_back(songEvent.clone()); }
  const std::list<SongEvent*>& songEvents() const { return songEvents_; }
  std::list<SongEvent*>& songEvents()             { return songEvents_; }
  const std::string& name() const                 { return name_; }
  int midiChannel() const                         { return midiChannel_; }
  uint64_t durationUs() const;

private:
  const Song& song_;
  std::list<SongEvent*> songEvents_;
  std::string name_{"Undefined"};
  int midiChannel_{0};
};

//-------------------------------------------------------------------------------------------------
// Song
//-------------------------------------------------------------------------------------------------

class Song {
public:
  Song()                                 { clear(); }
  void clear();
  void setTpqn(uint16_t tpqn)            { tpqn_ = tpqn; }
  Track* track(int trackNo)              { return &tracks_[trackNo]; }
  const Track* track(int trackNo) const  { return &tracks_[trackNo]; }
  size_t numberOfTracks() const          { return tracks_.size(); }
  uint64_t durationUs() const;

  void debugPrintAllSongEvents() const;
  void unselectAllEvents();
  void importFromMidi0(const std::string& path);
  void exportAsMidi0(const std::string& path) const;

  const uint16_t tpqn() const            { return tpqn_; }

private:
  int currentSelectedTrack_{0};
  uint16_t tpqn_{0};
  std::vector<Track> tracks_;
};

#endif // _SONG_H
