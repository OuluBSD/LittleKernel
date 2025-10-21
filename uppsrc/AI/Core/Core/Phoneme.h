#ifndef _AI_Core_Core_Phoneme_h_
#define _AI_Core_Core_Phoneme_h_




#define PHONOME_VOWELS \
	PHONOME_VOWEL(AA,  "ɑ", 134, 11) \
	PHONOME_VOWEL(AE,  "æ", 131, 11) \
	PHONOME_VOWEL(AH,  "ʌ",  88,  8) \
	PHONOME_VOWEL(AO,  "ɔ", 146, 11) \
	PHONOME_VOWEL(AW,  "ʊ", 202, 14) \
	PHONOME_VOWEL(AY,  "ɪ", 160, 12) \
	PHONOME_VOWEL(EH,  "ɛ",  82,  8) \
	PHONOME_VOWEL(ER,  "ɝ", 131, 11) \
	PHONOME_VOWEL(EY,  "e", 133, 11) \
	PHONOME_VOWEL(IH,  "ɪ",  60,  7) \
	PHONOME_VOWEL(IY,  "i", 107,  9) \
	PHONOME_VOWEL(OW,  "o", 155, 12) \
	PHONOME_VOWEL(OY, "ɔɪ", 295, 13) \
	PHONOME_VOWEL(UH,  "ʊ",  69,  7) \
	PHONOME_VOWEL(UW,  "u", 114, 10)

#define PHONOME_VOWEL_ALTS \
	PHONOME_VOWEL_ALT("ɒ", "ɑ") \
	PHONOME_VOWEL_ALT("ə", "e") /* false */ \
	PHONOME_VOWEL_ALT("ê", "ɛ") \
	PHONOME_VOWEL_ALT("a", "ɑ") \
	PHONOME_VOWEL_ALT("ɜ", "ɝ") \
	PHONOME_VOWEL_ALT("ö", "ɔɪ") \
	PHONOME_VOWEL_ALT("ä", "æ") \
	PHONOME_VOWEL_ALT("ạ", "a") \
	PHONOME_VOWEL_ALT("ẹ", "e") \
	PHONOME_VOWEL_ALT("ø", "ɔɪ") \
	PHONOME_VOWEL_ALT("ы", "ɪ") \
	PHONOME_VOWEL_ALT("আ", "ɑ") \
	PHONOME_VOWEL_ALT("ü", "u") \
	PHONOME_VOWEL_ALT("и", "i") \
	PHONOME_VOWEL_ALT("ī", "i") \
	PHONOME_VOWEL_ALT("э", "e") \
	PHONOME_VOWEL_ALT("а", "ɑ") \
	PHONOME_VOWEL_ALT("ë", "e") \
	PHONOME_VOWEL_ALT("û", "u") \
	PHONOME_VOWEL_ALT("є", "e") \
	PHONOME_VOWEL_ALT("ı", "ɪ") \
	PHONOME_VOWEL_ALT("ı", "ɪ") \
	PHONOME_VOWEL_ALT("è", "e") \
	PHONOME_VOWEL_ALT("ő", "ɔɪ") \
	PHONOME_VOWEL_ALT("ɐ", "ɔɪ") \
	PHONOME_VOWEL_ALT("ẽ", "e") \
	PHONOME_VOWEL_ALT("ú", "u") \
	PHONOME_VOWEL_ALT("ụ", "u") \
	PHONOME_VOWEL_ALT("ǻ", "ɑ") \
	PHONOME_VOWEL_ALT("ả", "æ") \
	PHONOME_VOWEL_ALT("ị", "ɪ") \
	PHONOME_VOWEL_ALT("ứ", "u") \
	PHONOME_VOWEL_ALT("ḗ", "ɛ") \
	PHONOME_VOWEL_ALT("ии", "i") \
	PHONOME_VOWEL_ALT("ạ", "ɔɪ") \
	PHONOME_VOWEL_ALT("ồ", "o") \
	PHONOME_VOWEL_ALT("ǻ", "ɔɪ") \
	PHONOME_VOWEL_ALT("ý", "o") \
	PHONOME_VOWEL_ALT("ỳ", "ɛ") \
	PHONOME_VOWEL_ALT("ῳ", "ɔ") \
	PHONOME_VOWEL_ALT("ų", "u") \
	PHONOME_VOWEL_ALT("ẵ", "æ") \
	PHONOME_VOWEL_ALT("ǣ", "e") \
	PHONOME_VOWEL_ALT("ȩ", "ɛ") \
	
#define PHONOME_VOWEL_ALT_COUNT 44

#define PHONOME_CONSONANTS \
	PHONOME_CONSONANT( B, "b",   70,  7) \
	PHONOME_CONSONANT(CH, "ʃ",  127, 10) \
	PHONOME_CONSONANT( D, "d",   72,  8) \
	PHONOME_CONSONANT(DH, "ð",   35,  5) \
	PHONOME_CONSONANT( F, "f",   98,  9) \
	PHONOME_CONSONANT( G, "g",   84,  8) \
	PHONOME_CONSONANT(HH, "h",   56,  6) \
	PHONOME_CONSONANT(JH, "ʒ",  105,  9) \
	PHONOME_CONSONANT( K, "k",  107,  9) \
	PHONOME_CONSONANT( L, "l",   71,  7) \
	PHONOME_CONSONANT( M, "m",   71,  7) \
	PHONOME_CONSONANT( N, "n",   65,  7) \
	PHONOME_CONSONANT(NG, "ŋ",   91,  9) \
	PHONOME_CONSONANT( P, "p",  102,  9) \
	PHONOME_CONSONANT( R, "r",   75,  8) \
	PHONOME_CONSONANT( S, "s",   99,  9) \
	PHONOME_CONSONANT(SH, "ʃ",  126, 10) \
	PHONOME_CONSONANT( T, "t",   83,  8) \
	PHONOME_CONSONANT(TH, "θ",   77,  8) \
	PHONOME_CONSONANT( V, "v",   54,  6) \
	PHONOME_CONSONANT( W, "w",   62,  7) \
	PHONOME_CONSONANT( Y, "j",   57,  7) \
	PHONOME_CONSONANT( Z, "z",   67,  7) \
	PHONOME_CONSONANT(ZH, "ʒə", 110, 10)

#define PHONOME_CONSONANT_ALTS \
	PHONOME_CONSONANT_ALT("ɡ", "g") \
	PHONOME_CONSONANT_ALT("k", "k") \
	PHONOME_CONSONANT_ALT("y", "j") \
	PHONOME_CONSONANT_ALT("ɹ", "r") \
	PHONOME_CONSONANT_ALT("ɚ", "r") \
	PHONOME_CONSONANT_ALT("ʰ", "h") \
	PHONOME_CONSONANT_ALT("đ", "d") \
	PHONOME_CONSONANT_ALT("ৎ", "t") \
	PHONOME_CONSONANT_ALT("д", "d") \
	PHONOME_CONSONANT_ALT("ñ", "ŋ") \
	PHONOME_CONSONANT_ALT("ň", "ŋ") \
	PHONOME_CONSONANT_ALT("ņ", "ŋ") \
	PHONOME_CONSONANT_ALT("μ", "m") \
	PHONOME_CONSONANT_ALT("к", "k") \
	PHONOME_CONSONANT_ALT("ෆ", "f") \
	PHONOME_CONSONANT_ALT("ṣ", "s") \
	PHONOME_CONSONANT_ALT("ʂ", "ʃ") \
	PHONOME_CONSONANT_ALT("ł", "l") \
	PHONOME_CONSONANT_ALT("ζ", "z") \
	PHONOME_CONSONANT_ALT("ғ", "ʒ") \
	PHONOME_CONSONANT_ALT("ߨ", "ʒ") \
	PHONOME_CONSONANT_ALT("ʒ", "ʒ") \
	PHONOME_CONSONANT_ALT("ǰ", "ʒ") \
	PHONOME_CONSONANT_ALT("ṣ", "s") \
	PHONOME_CONSONANT_ALT("살", "s") \
	PHONOME_CONSONANT_ALT("ڈ", "d") \
	PHONOME_CONSONANT_ALT("ź", "z") \
	PHONOME_CONSONANT_ALT("ç", "ʒ") \
	PHONOME_CONSONANT_ALT("ḻ", "l") \
	PHONOME_CONSONANT_ALT("Ŝ", "ʃ") \
	PHONOME_CONSONANT_ALT("ẩ", "ʃ") \
	PHONOME_CONSONANT_ALT("в", "b") \
	PHONOME_CONSONANT_ALT("δ", "d") \
	PHONOME_CONSONANT_ALT("φ", "f") \
	PHONOME_CONSONANT_ALT("ń", "ŋ") \
	PHONOME_CONSONANT_ALT("ő", "f") \
	PHONOME_CONSONANT_ALT("ṫ", "t") \
	PHONOME_CONSONANT_ALT("Ʌ", "v") \
	PHONOME_CONSONANT_ALT("∂", "d") \
	PHONOME_CONSONANT_ALT("з", "z") \
	PHONOME_CONSONANT_ALT("п", "p") \
	PHONOME_CONSONANT_ALT("ш", "ʃ") \
	PHONOME_CONSONANT_ALT("ς", "s") \
	PHONOME_CONSONANT_ALT("б", "b") \
	PHONOME_CONSONANT_ALT("ʳ", "h") \
	PHONOME_CONSONANT_ALT("ǂ", "ʒ") \
	PHONOME_CONSONANT_ALT("ɗ", "d") \
	PHONOME_CONSONANT_ALT("ﬁ", "f") \
	PHONOME_CONSONANT_ALT("ﬁ", "s") \
	PHONOME_CONSONANT_ALT("õ", "s") \
	PHONOME_CONSONANT_ALT("↘", "j") \
	PHONOME_CONSONANT_ALT("ǹ", "ɛ") \
	PHONOME_CONSONANT_ALT("ɭ", "l") \
	PHONOME_CONSONANT_ALT("ĥ", "x") \
	PHONOME_CONSONANT_ALT("ȷ", "j") \
	PHONOME_CONSONANT_ALT("ģ", "g") \
	PHONOME_CONSONANT_ALT("ȷ", "j") \
	PHONOME_CONSONANT_ALT("פ", "f") \
	PHONOME_CONSONANT_ALT("ғ", "f") \
	PHONOME_CONSONANT_ALT("↘", "j") \
	PHONOME_CONSONANT_ALT("կ", "k") \
	PHONOME_CONSONANT_ALT("ҥ", "n") \
	PHONOME_CONSONANT_ALT("ǂ", "ʒ") \
	PHONOME_CONSONANT_ALT("ғ", "f") \
	
#define PHONOME_CONSONANT_ALT_COUNT 64

#define PHONOME_VOWEL(a, b, d, r) PHONOME_VOWEL_##a,
enum {
	PHONOME_VOWELS
	PHONOME_VOWEL_COUNT
};
#undef PHONOME_VOWEL

#define PHONOME_CONSONANT(a, b, d, r) PHONOME_CONSONANT_##a,
enum {
	PHONOME_CONSONANTS
	PHONOME_CONSONANT_COUNT
};
#undef PHONOME_CONSONANT


#define PHONOME_VOWEL(a, b, d, r) PHONOME_##a,
#define PHONOME_CONSONANT(a, b, d, r) PHONOME_##a,
enum {
	PHONOME_VOWELS
	PHONOME_CONSONANTS
	PHONOME_COUNT
};
#undef PHONOME_VOWEL
#undef PHONOME_CONSONANT




int GetPhonemeEnum(int c0, int c1, int* cur);
bool IsPhonemeVowel(int phoneme);
bool IsPhonemeConsonant(int phoneme);
int GetPhonemeDuration(int phoneme, int stress);

double GetSpellingDistance(const WString& w0, const WString& w1, bool relative);
double GetSpellingRawDistance(const WString& w0, const WString& w1);
double GetSpellingRelativeDistance(const WString& w0, const WString& w1);
int GetPhonemeRepeats(int phoneme, int stress);
int EstimatePhonemeSyllables(const WString& w);

extern const double vowel_distance[PHONOME_VOWEL_COUNT][PHONOME_VOWEL_COUNT];
extern const double consonant_distance[PHONOME_CONSONANT_COUNT][PHONOME_CONSONANT_COUNT];




#endif
