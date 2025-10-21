#include "Sound.h"

NAMESPACE_UPP

void SoundPhrase::Finish() {
	owner.owner.owner.OnPhraseEnd(*this);
}

void SoundMessage::Finish() {
	owner.owner.OnMessageEnd(*this);
}

void SoundDiscussion::Finish() {
	owner.OnDiscussionEnd(*this);
}




SoundPhrase& SoundMessage::Add() {
	auto& o = phrases.Add(new SoundPhrase(*this));
	owner.owner.OnPhraseBegin(o);
	return o;
}

SoundMessage& SoundDiscussion::Add() {
	auto& o = messages.Add(new SoundMessage(*this));
	owner.OnMessageBegin(o);
	return o;
}

SoundDiscussion& SoundDiscussionManager::Add() {
	auto& o = discussions.Add(new SoundDiscussion(*this));
	OnDiscussionBegin(o);
	return o;
}

void SoundDiscussionManager::OnPhraseBegin(SoundPhrase& s) {
	WhenPhraseBegin(s);
}

void SoundDiscussionManager::OnMessageBegin(SoundMessage& s) {
	WhenMessageBegin(s);
}

void SoundDiscussionManager::OnDiscussionBegin(SoundDiscussion& s) {
	WhenDiscussionBegin(s);
}




double SoundClipBase::GetDuration() const {
	int r = GetSampleRate();
	return r > 0 ? GetCount() / (double)r : 0;
}

END_UPP_NAMESPACE
