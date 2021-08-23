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
// SongEvent
//-------------------------------------------------------------------------------------------------

class SongEvent {
public:
  SongEvent(MidiFile* pMidiFile, uint32_t absoluteTick)
      : pMidiFile_(pMidiFile), absoluteTick_(absoluteTick) {}

  virtual ~SongEvent()                          = 0;
  virtual uint8_t eventId() const               = 0;
  virtual Error write(uint32_t deltaTime) const = 0;

  uint32_t absoluteTick() const               { return absoluteTick_; }

protected:
  MidiFile* pMidiFile_{nullptr};

private:
  const uint32_t absoluteTick_;
};

//-------------------------------------------------------------------------------------------------
// NoteEvent
//-------------------------------------------------------------------------------------------------

class NoteEvent : public SongEvent {
public:
  NoteEvent(MidiFile* pMidiFile, uint32_t absoluteTick, uint8_t channel, uint8_t note, uint8_t velocity)
      : SongEvent(pMidiFile, absoluteTick), channel_(channel), note_(note), velocity_(velocity) {}

  uint8_t eventId() const override               = 0;
  Error write(uint32_t deltaTime) const override = 0;

  uint8_t note()     const                    { return note_; }
  uint8_t velocity() const                    { return velocity_; }

private:
  const uint8_t channel_{0};
  const uint8_t note_{0};
  const uint8_t velocity_{0};
};

//-------------------------------------------------------------------------------------------------
// NoteOnEvent
//-------------------------------------------------------------------------------------------------

class NoteOnEvent : public NoteEvent {
public:
  NoteOnEvent(MidiFile* pMidiFile, uint32_t absoluteTick, uint8_t channel, uint8_t note, uint8_t velocity)
      : NoteEvent(pMidiFile, absoluteTick, channel, note, velocity) {}

  uint8_t eventId() const final               { return MIDI_EVENT_NOTE_ON; }
  Error write(uint32_t deltaTime) const final;
};

//-------------------------------------------------------------------------------------------------
// NoteOffEvent
//-------------------------------------------------------------------------------------------------

class NoteOffEvent : public NoteEvent {
public:
  NoteOffEvent(MidiFile* pMidiFile, uint32_t absoluteTick, uint8_t channel, uint8_t note, uint8_t velocity)
      : NoteEvent(pMidiFile, absoluteTick, channel, note, velocity) {}

  uint8_t eventId() const final               { return MIDI_EVENT_NOTE_OFF; }
  Error write(uint32_t deltaTime) const final;
};

//-------------------------------------------------------------------------------------------------
// NoteBlock
//-------------------------------------------------------------------------------------------------

class NoteBlock {
public:
  void setNote(uint8_t midiNote)        { note_ = midiNote; }
  void setStartTick(uint32_t startTick) { startTick_ = startTick; }
  void setNumTicks(uint32_t numTicks)   { numTicks_ = numTicks; }

  void select()                         { isSelected_ = true; }
  void unselect()                       { isSelected_ = false; }

  const uint8_t note() const            { return note_; }
  const uint32_t startTick() const      { return startTick_; }
  const uint32_t numTicks() const       { return numTicks_; }
  const bool isSelected() const         { return isSelected_;}

private:
  uint8_t note_{0};
  uint32_t startTick_{0};
  uint32_t numTicks_{0};
  bool isSelected_{false};
};

//-------------------------------------------------------------------------------------------------
// Track
//-------------------------------------------------------------------------------------------------

class Track {
public:
  Track(std::string name, int midiChannel)
      : name_(name), midiChannel_(midiChannel) {};

  void addNoteBlock(const NoteBlock& noteBlock)   { noteBlocks_.push_back(noteBlock); }
  const std::list<NoteBlock>& noteBlocks() const  { return noteBlocks_; }
  std::list<NoteBlock>& noteBlocks()              { return noteBlocks_; }
  const std::string& name() const                 { return name_; }
  int midiChannel() const                         { return midiChannel_; }

private:
  std::list<NoteBlock> noteBlocks_;
  std::string name_{"Track1"};
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

  void debugPrintAllNoteBlocks() const;
  void unselectAllNotes();
  void importFromMidi0(const std::string& path);
  void exportAsMidi0(const std::string& path) const;

  const uint16_t tpqn() const            { return tpqn_; }

private:
  int currentSelectedTrack_{0};
  uint16_t tpqn_{0};
  std::vector<Track> tracks_;
};

#endif // _SONG_H
