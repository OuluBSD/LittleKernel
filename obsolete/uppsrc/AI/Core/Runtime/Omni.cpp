#include "Runtime.h"

#if 0

NAMESPACE_UPP

OmniThread::OmniThread() {
	
}

void OmniThread::Visit(Vis& v) {
	v	.VisitT("CompletionThread", (CompletionThread&)*this)
		.VisitT("ChatThread", (ChatThread&)*this)
		.VisitT("SpeechTranscriptionThread", (SpeechTranscriptionThread&)*this)
		.VisitT("SpeechGenerationThread", (SpeechGenerationThread&)*this)
		.VisitT("ImageGenerationThread", (ImageGenerationThread&)*this)
		.VisitT("ImageVisionThread", (ImageVisionThread&)*this)
		.VisitT("MetaEnvThread", (MetaEnvThread&)*this)
	;
}

void OmniThread::OnPhraseEnd(SoundPhrase& p) {
	LOG("TODO");
	
	// get transcript
}

void OmniThread::OnMessageEnd(SoundMessage&) {
	TODO
	
	// get proofread
}

void OmniThread::OnDiscussionEnd(SoundDiscussion&) {
	TODO
}



END_UPP_NAMESPACE

#endif
