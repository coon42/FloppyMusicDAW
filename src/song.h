#ifndef _SONG_H
#define _SONG_H

#include <list>
#include <string>
#include <vector>

//-------------------------------------------------------------------------------------------------
// SongEvent
//-------------------------------------------------------------------------------------------------

enum class SongEventType {
  Undefined,
  NotImplementedEvent,
  NotImplementedMetaEvent,
  SetTempo,
  NoteBlock,
  ProgramChange,
  PitchBend
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
  NotImplementedEvent(uint32_t startTick, uint8_t midiEventId, uint32_t numTicks)
      : midiEventId_(midiEventId) {
    setStartTick(startTick);
    setNumTicks(numTicks);
  }

  SongEvent* clone() const final   { return new NotImplementedEvent(*this); }
  SongEventType type() const final { return SongEventType::NotImplementedEvent; }
  uint8_t midiEventId() const      { return midiEventId_;}

private:
  const uint8_t midiEventId_;
};

//-------------------------------------------------------------------------------------------------
// NotImplementedMetaEvent
//-------------------------------------------------------------------------------------------------

class NotImplementedMetaEvent : public SongEvent {
public:
  NotImplementedMetaEvent(uint32_t startTick, uint8_t midiMetaEventId, uint32_t numTicks)
      : midiMetaEventId_(midiMetaEventId) {
    setStartTick(startTick);
    setNumTicks(numTicks);
  }

  SongEvent* clone() const final   { return new NotImplementedMetaEvent(*this); }
  SongEventType type() const final { return SongEventType::NotImplementedMetaEvent; }
  uint8_t midiMetaEventId() const  { return midiMetaEventId_;}

private:
  const uint8_t midiMetaEventId_;
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
// ProgramChangeEvent
//-------------------------------------------------------------------------------------------------

class ProgramChangeEvent : public SongEvent {
public:
  SongEvent* clone() const final         { return new ProgramChangeEvent(*this); }
  SongEventType type() const final       { return SongEventType::ProgramChange; }

  void setProgram(uint8_t programNumber) { programNumber_ = programNumber; }  
  const uint8_t programNumber() const    { return programNumber_; }

private:
  uint8_t programNumber_{0};
};

//-------------------------------------------------------------------------------------------------
// PitchBendEvent
//-------------------------------------------------------------------------------------------------

class PitchBendEvent : public SongEvent {
public:
  SongEvent* clone() const final                  { return new PitchBendEvent(*this); }
  SongEventType type() const final                { return SongEventType::PitchBend; }

  void setPitchBendValue(uint16_t pitchBendValue) { pitchBendValue_ = pitchBendValue; }
  const uint16_t pitchBendValue() const           { return pitchBendValue_; }

private:
  uint16_t pitchBendValue_{64};
};

//-------------------------------------------------------------------------------------------------
// SetTempoEvent
//-------------------------------------------------------------------------------------------------

class SetTempoEvent : public SongEvent {
public:
  SongEvent* clone() const final   { return new SetTempoEvent(*this); }
  SongEventType type() const final { return SongEventType::SetTempo; }

  void setBpm(float bpm)           { bpm_ = bpm; }
  const float bpm() const          { return bpm_; }

private:
  float bpm_{0}; // TODO: use fixed point arithmetic instead
};

//-------------------------------------------------------------------------------------------------
// Track
//-------------------------------------------------------------------------------------------------

class Song;

class Track {
public:
  Track(const Song& song, std::string name)
      : song_(song), name_(name) {};
  Track(const Track& track);
  Track& operator = (const Track& rhs);
  ~Track();

  void clear();
  void addSongEvent(const SongEvent& songEvent)   { songEvents_.push_back(songEvent.clone()); }
  const std::list<SongEvent*>& songEvents() const { return songEvents_; }
  std::list<SongEvent*>& songEvents()             { return songEvents_; }
  const std::string& name() const                 { return name_; }
  uint32_t numTicks() const;

  void debugPrintAllEvents() const;

protected:
  const Song& song_;

private:
  std::list<SongEvent*> songEvents_;
  std::string name_{"Undefined"};
};

//-------------------------------------------------------------------------------------------------
// ChannelTrack
//-------------------------------------------------------------------------------------------------

class ChannelTrack : public Track {
public:
  ChannelTrack(const Song& song, std::string name, int midiChannel)
    : Track(song, name), midiChannel_(midiChannel) {};

  int midiChannel() const                       { return midiChannel_; }
  uint64_t durationUs() const;                  

private:
  const int midiChannel_{0};
};

//-------------------------------------------------------------------------------------------------
// MetaTrack
//-------------------------------------------------------------------------------------------------

class MetaTrack : public Track {
public:
  MetaTrack(const Song& song)
    : Track(song, "Meta") {};

};

//-------------------------------------------------------------------------------------------------
// Song
//-------------------------------------------------------------------------------------------------

class Song {
public:
  Song()                                        { clear(); }
  void clear();
  void setTpqn(uint16_t tpqn)                   { tpqn_ = tpqn; }
  ChannelTrack* track(int trackNo)              { return &tracks_[trackNo]; }
  const ChannelTrack* track(int trackNo) const  { return &tracks_[trackNo]; }
  MetaTrack* metaTrack()                        { return &metaTrack_; }
  const MetaTrack* metaTrack() const            { return &metaTrack_; }

  size_t numberOfTracks() const                 { return tracks_.size(); }
  uint64_t durationUs() const;
  uint32_t numTicks() const;                    
  int currentSelectedTrackNo() const            { return currentSelectedTrackNo_; }
  const ChannelTrack* currentSelectedTrack() const { return track(currentSelectedTrackNo_); }

  void setCurrentSelectedTrack(int track)       { currentSelectedTrackNo_ = track; }
  void debugPrintAllSongEvents() const;
  void unselectAllEvents();
  void importFromMidi0(const std::string& path);
  void exportAsMidi0(const std::string& path) const;

  const uint16_t tpqn() const                   { return tpqn_; }

  // TODO: remove once rendering is fixed:
  void registerRedrawAllCallback(void(*redrawAllCallback)(void* pCtx), void* pCtx);
  void requestGlobalRedraw();
  // --

private:
  int currentSelectedTrackNo_{0};
  uint16_t tpqn_{0};

  MetaTrack metaTrack_{*this};
  std::vector<ChannelTrack> tracks_;

  // TODO: remove once rendering is fixed:
  void(*pRedrawAllCallback_)(void* pCtx) = nullptr;
  void* pRedrawCallbackCtx_ = nullptr;
  // --
};

#endif // _SONG_H
