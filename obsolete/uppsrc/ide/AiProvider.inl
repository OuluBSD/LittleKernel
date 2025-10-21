
NAMESPACE_UPP

inline
AiServiceProviderManager::AiServiceProviderManager() {
	
}

inline
AiServiceProvider& AiServiceProviderManager::Add() {
	return providers.Add();
}

inline
void AiServiceProviderManager::Remove(int i) {
	if (i >= 0 && i < providers.GetCount())
		providers.Remove(i);
}

inline
AiServiceProvider& AiServiceProviderManager::operator[](int i) {
	return providers[i];
}

inline
int AiServiceProviderManager::GetCount() const {
	return providers.GetCount();
}


inline
AiServiceProviderManager& AiManager() {
	static AiServiceProviderManager s;
	return s;
}

inline
bool AiServiceProvider::IsFeatureAudioToText() const
{
	return false
#if HAVE_OPENAI
		|| type == OPENAI
#endif
		|| type == API_WHISPERFILE_TRANSCRIPT
		;
}

inline
bool AiServiceProvider::IsApiOpenAICompletion() const
{
	return false
#if HAVE_OPENAI
		|| type == OPENAI
		|| type == API_OPENAI_COMPLETION
#endif
		;
}

inline
bool AiServiceProvider::IsApiOpenAIChat() const
{
	return false
#if HAVE_OPENAI
		|| type == OPENAI
		|| type == API_OPENAI_CHAT
#endif
		;
}

inline
bool AiServiceProvider::IsApiOpenAIImage() const
{
	return false
#if HAVE_OPENAI
		|| type == OPENAI
		|| type == API_OPENAI_IMAGE
#endif
		;
}

inline
bool AiServiceProvider::IsApiOpenAIVision() const
{
	return false
#if HAVE_OPENAI
		|| type == OPENAI
		|| type == API_OPENAI_VISION
#endif
		;
}

inline
String AiServiceProvider::GetTypeString() const
{
	return GetTypeString(type);
}

END_UPP_NAMESPACE
