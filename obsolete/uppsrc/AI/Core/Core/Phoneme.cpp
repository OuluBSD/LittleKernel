#include "Core.h"

NAMESPACE_UPP

// https://www.occasionalenthusiast.com/phonetic-distance-between-words-with-application-to-the-international-spelling-alphabet/

/*
hut	HH AH T
AO	ought	AO T
AW	cow	K AW
AY	hide	HH AY D
B 	be	B IY
CH	cheese	CH IY Z
D 	dee	D IY
DH	thee	DH IY
EH	Ed	EH D
ER	hurt	HH ER T
EY	ate	EY T
F 	fee	F IY
G 	green	G R IY N
HH	he	HH IY
IH	it	IH T
IY	eat	IY T
JH	gee	JH IY
K 	key	K IY
L 	lee	L IY
M 	me	M IY
N 	knee	N IY
NG	ping	P IH NG
OW	oat	OW T
OY	toy	T OY
P 	pee	P IY
R 	read	R IY D
S 	sea	S IY
SH	she	SH IY
T 	tea	T IY
TH	theta	TH EY T AH
UH	hood	HH UH D
UW	two	T UW
V 	vee	V IY
W 	we	W IY
Y 	yield	Y IY L D
Z 	zee	Z IY
ZH	seizure	S IY ZH ER
- hut: hut [hʌt] (only one syllable)
- ought: ought [ɔt] (only one syllable)
- cow: cow [kaʊ] (only one syllable)
- hide: hide [haɪd] (only one syllable)
- be: be [bi] (only one syllable)
- cheese: cheese [tʃiz] (only one syllable)
- dee: dee [di] (only one syllable)
- thee: thee [ði] (only one syllable)
- Ed: Ed [ɛd] (only one syllable)
- hurt: hurt [hɝt] (only one syllable)
- ate: ate [eɪt] (only one syllable)
- fee: fee [fi] (only one syllable)
- green: green [ɡrin] (only one syllable)
- he: he [hi] (only one syllable)
- it: it [ɪt] (only one syllable)
- eat: eat [it] (only one syllable)
- gee: gee [d͡ʒi] (only one syllable)
- key: key [ki] (only one syllable)
- lee: lee [li] (only one syllable)
- me: me [mi] (only one syllable)
- knee: knee [ni] (only one syllable)
- ping: ping [pɪŋ] (only one syllable)
- oat: oat [oʊt] (only one syllable)
- toy: toy [tɔɪ] (only one syllable)
- pee: pee [pi] (only one syllable)
- read: read [rid] (only one syllable)
- sea: sea [si] (only one syllable)
- she: she [ʃi] (only one syllable)
- tea: tea [ti] (only one syllable)
- theta: the-ta [ˈθi.ta] (two syllables)
- hood: hood [hʊd] (only one syllable)
- two: two [tu] (only one syllable)
- vee: vee [vi] (only one syllable)
- we: we [wi] (only one syllable)
- yield: yield [jild] (only one syllable)
- zee: zee [zi] (only one syllable)
- seizure: sei-zure [ˈsi.ʒər] (two syllables)
*/

const double vowel_distance[PHONOME_VOWEL_COUNT][PHONOME_VOWEL_COUNT] = {
	{0.0000,0.4472,0.2121,0.4921,0.4712,0.5235,0.4743,0.3536,0.7506,0.7517,1.0000,0.6965,0.5490,0.7329,0.8349},
	{0.4472,0.0000,0.4301,0.6182,0.3423,0.1845,0.0707,0.1581,0.4039,0.4301,0.6325,0.6895,0.4281,0.6761,0.8349},
	{0.2121,0.4301,0.0000,0.4440,0.3959,0.4573,0.4243,0.2828,0.6191,0.6083,0.8631,0.5524,0.4291,0.5849,0.6649},
	{0.4921,0.6182,0.4440,0.0000,0.3932,0.6389,0.6141,0.5264,0.7619,0.7531,0.9706,0.3279,0.3765,0.3808,0.4950},
	{0.4712,0.3423,0.3959,0.3932,0.0000,0.2838,0.3334,0.2875,0.4772,0.4854,0.6973,0.3765,0.3164,0.3749,0.5428},
	{0.5235,0.1845,0.4573,0.6389,0.2838,0.0000,0.1559,0.2144,0.2693,0.2915,0.5137,0.6329,0.3030,0.6182,0.7621},
	{0.4743,0.0707,0.4243,0.6141,0.3334,0.1559,0.0000,0.1414,0.3364,0.3606,0.5701,0.6522,0.3870,0.6341,0.7887},
	{0.3536,0.1581,0.2828,0.5264,0.2875,0.2144,0.1414,0.0000,0.4041,0.4123,0.6519,0.5877,0.3526,0.5849,0.7226},
	{0.7506,0.4039,0.6191,0.7619,0.4772,0.2693,0.3364,0.4041,0.0000,0.0500,0.2517,0.6405,0.3864,0.5918,0.7142},
	{0.7517,0.4301,0.6083,0.7531,0.4854,0.2915,0.3606,0.4123,0.0500,0.0000,0.2550,0.6161,0.3765,0.5676,0.6798},
	{1.0000,0.6325,0.8631,0.9706,0.6973,0.5137,0.5701,0.6519,0.2517,0.2550,0.0000,0.7963,0.6046,0.7329,0.8349},
	{0.6965,0.6895,0.5524,0.3279,0.3765,0.6329,0.6522,0.5877,0.6405,0.6161,0.7963,0.0000,0.3528,0.0791,0.1957},
	{0.5490,0.4281,0.4291,0.3765,0.3164,0.3030,0.3870,0.3526,0.3864,0.3765,0.6046,0.3528,0.0000,0.3637,0.5041},
	{0.7329,0.6761,0.5849,0.3808,0.3749,0.6182,0.6341,0.5849,0.5918,0.5676,0.7329,0.0791,0.3637,0.0000,0.2000},
	{0.8349,0.8349,0.6649,0.4950,0.5428,0.7621,0.7887,0.7226,0.7142,0.6798,0.8349,0.1957,0.5041,0.2000,0.0000}
};

const double consonant_distance[PHONOME_CONSONANT_COUNT][PHONOME_CONSONANT_COUNT] = {
	{0.0000,0.5771,0.2493,0.5225,0.3008,0.3515,0.9115,0.5417,0.4039,0.5332,0.3515,0.4309,0.4970,0.1990,0.5417,0.3907,0.4023,0.3190,0.5591,0.2256,0.4713,0.5032,0.3362,0.3496},
	{0.5771,0.0000,0.5771,0.6941,0.5312,0.6280,0.9800,0.1990,0.5957,0.7022,0.6280,0.6280,0.6751,0.5417,0.6494,0.5312,0.4592,0.5417,0.6649,0.5673,0.7022,0.6797,0.5673,0.5004},
	{0.2493,0.5771,0.0000,0.5225,0.3907,0.3515,0.9115,0.5417,0.4039,0.4713,0.4309,0.3515,0.4970,0.3190,0.5417,0.3008,0.4023,0.1990,0.5591,0.3362,0.5332,0.5032,0.2256,0.3496},
	{0.5225,0.6941,0.5225,0.0000,0.5116,0.5782,0.9695,0.6649,0.6115,0.6580,0.5782,0.5782,0.6291,0.5591,0.6649,0.5116,0.5205,0.5591,0.1990,0.4713,0.6580,0.6339,0.4713,0.4810},
	{0.3008,0.5312,0.3907,0.5116,0.0000,0.4626,0.8605,0.5673,0.4176,0.5591,0.3896,0.4626,0.5247,0.2256,0.5673,0.2493,0.2671,0.3362,0.4713,0.1990,0.5004,0.5306,0.3190,0.3331},
	{0.3515,0.6280,0.3515,0.5782,0.4626,0.0000,0.9446,0.5957,0.1990,0.5879,0.4970,0.4970,0.3515,0.4039,0.5957,0.4626,0.4724,0.4039,0.6115,0.4176,0.5879,0.5609,0.4176,0.4285},
	{0.9115,0.9800,0.9115,0.9695,0.8605,0.9446,0.0000,1.0000,0.9234,0.9954,0.9446,0.9446,0.9765,0.8895,1.0000,0.8605,0.8658,0.8895,0.9489,0.8832,0.9954,0.9797,0.8832,0.8884},
	{0.5417,0.1990,0.5417,0.6649,0.5673,0.5957,1.0000,0.0000,0.6280,0.6734,0.5957,0.5957,0.6451,0.5771,0.6181,0.5673,0.5004,0.5771,0.6941,0.5312,0.6734,0.6499,0.5312,0.4592},
	{0.4039,0.5957,0.4039,0.6115,0.4176,0.1990,0.9234,0.6280,0.0000,0.6207,0.5354,0.5354,0.4039,0.3515,0.6280,0.4176,0.4285,0.3515,0.5782,0.4626,0.6207,0.5951,0.4626,0.4724},
	{0.5332,0.7022,0.4713,0.6580,0.5591,0.5879,0.9954,0.6734,0.6207,0.0000,0.5879,0.5324,0.6380,0.5691,0.2671,0.5004,0.5673,0.5116,0.6874,0.5225,0.6665,0.6428,0.4592,0.5312},
	{0.3515,0.6280,0.4309,0.5782,0.3896,0.4970,0.9446,0.5957,0.5354,0.5879,0.0000,0.2493,0.3515,0.4039,0.5957,0.4626,0.4724,0.4746,0.6115,0.3350,0.5324,0.5609,0.4176,0.4285},
	{0.4309,0.6280,0.3515,0.5782,0.4626,0.4970,0.9446,0.5957,0.5354,0.5324,0.2493,0.0000,0.3515,0.4746,0.5957,0.3896,0.4724,0.4039,0.6115,0.4176,0.5879,0.5609,0.3350,0.4285},
	{0.4970,0.6751,0.4970,0.6291,0.5247,0.3515,0.9765,0.6451,0.4039,0.6380,0.3515,0.3515,0.0000,0.5354,0.6451,0.5247,0.5334,0.5354,0.6598,0.4856,0.6380,0.6131,0.4856,0.4949},
	{0.1990,0.5417,0.3190,0.5591,0.2256,0.4039,0.8895,0.5771,0.3515,0.5691,0.4039,0.4746,0.5354,0.0000,0.5771,0.3362,0.3496,0.2493,0.5225,0.3008,0.5116,0.5411,0.3907,0.4023},
	{0.5417,0.6494,0.5417,0.6649,0.5673,0.5957,1.0000,0.6181,0.6280,0.2671,0.5957,0.5957,0.6451,0.5771,0.0000,0.5673,0.5004,0.5771,0.6941,0.5312,0.6734,0.6499,0.5312,0.4592},
	{0.3907,0.5312,0.3008,0.5116,0.2493,0.4626,0.8605,0.5673,0.4176,0.5004,0.4626,0.3896,0.5247,0.3362,0.5673,0.0000,0.2671,0.2256,0.4713,0.3190,0.5591,0.5306,0.1990,0.3331},
	{0.4023,0.4592,0.4023,0.5205,0.2671,0.4724,0.8658,0.5004,0.4285,0.5673,0.4724,0.4724,0.5334,0.3496,0.5004,0.2671,0.0000,0.3496,0.4810,0.3331,0.5673,0.5392,0.3331,0.1990},
	{0.3190,0.5417,0.1990,0.5591,0.3362,0.4039,0.8895,0.5771,0.3515,0.5116,0.4746,0.4039,0.5354,0.2493,0.5771,0.2256,0.3496,0.0000,0.5225,0.3907,0.5691,0.5411,0.3008,0.4023},
	{0.5591,0.6649,0.5591,0.1990,0.4713,0.6115,0.9489,0.6941,0.5782,0.6874,0.6115,0.6115,0.6598,0.5225,0.6941,0.4713,0.4810,0.5225,0.0000,0.5116,0.6874,0.6644,0.5116,0.5205},
	{0.2256,0.5673,0.3362,0.4713,0.1990,0.4176,0.8832,0.5312,0.4626,0.5225,0.3350,0.4176,0.4856,0.3008,0.5312,0.3190,0.3331,0.3907,0.5116,0.0000,0.4592,0.4918,0.2493,0.2671},
	{0.4713,0.7022,0.5332,0.6580,0.5004,0.5879,0.9954,0.6734,0.6207,0.6665,0.5324,0.5879,0.6380,0.5116,0.6734,0.5591,0.5673,0.5691,0.6874,0.4592,0.0000,0.1763,0.5225,0.5312},
	{0.5032,0.6797,0.5032,0.6339,0.5306,0.5609,0.9797,0.6499,0.5951,0.6428,0.5609,0.5609,0.6131,0.5411,0.6499,0.5306,0.5392,0.5411,0.6644,0.4918,0.1763,0.0000,0.4918,0.5011},
	{0.3362,0.5673,0.2256,0.4713,0.3190,0.4176,0.8832,0.5312,0.4626,0.4592,0.4176,0.3350,0.4856,0.3907,0.5312,0.1990,0.3331,0.3008,0.5116,0.2493,0.5225,0.4918,0.0000,0.2671},
	{0.3496,0.5004,0.3496,0.4810,0.3331,0.4285,0.8884,0.4592,0.4724,0.5312,0.4285,0.4285,0.4949,0.4023,0.4592,0.3331,0.1990,0.4023,0.5205,0.2671,0.5312,0.5011,0.2671,0.0000}
};

int GetPhonemeEnum(int c0, int c1, int* cur) {
	static int phonemes[PHONOME_COUNT][2];
	static int vowel_alts[PHONOME_VOWEL_ALT_COUNT][4];
	static int cons_alts[PHONOME_CONSONANT_ALT_COUNT][4];
	if (phonemes[0][0] == 0) {
		int i = 0;
		#define PHONOME_VOWEL(code, str, d, r) \
		{ \
			WString ws = String(str).ToWString(); \
			phonemes[i][0] = ws[0]; \
			phonemes[i][1] = ws.GetCount() > 1 ? ws[1] : 0; \
			i++; \
		}
		PHONOME_VOWELS
		#undef PHONOME_VOWEL
		
		#define PHONOME_CONSONANT(code, str, d, r) \
		{ \
			WString ws = String(str).ToWString(); \
			phonemes[i][0] = ws[0]; \
			phonemes[i][1] = ws.GetCount() > 1 ? ws[1] : 0; \
			i++; \
		}
		PHONOME_CONSONANTS
		#undef PHONOME_CONSONANT
		
		
		
		i = 0;
		#define PHONOME_VOWEL_ALT(str, alt) \
		{ \
			WString ws = String(str).ToWString(); \
			WString ws1 = String(alt).ToWString(); \
			vowel_alts[i][0] = ws[0]; \
			vowel_alts[i][1] = ws.GetCount() > 1 ? ws[1] : 0; \
			vowel_alts[i][2] = ws1[0]; \
			vowel_alts[i][3] = ws1.GetCount() > 1 ? ws1[1] : 0; \
			i++; \
		}
		PHONOME_VOWEL_ALTS
		ASSERT(i == PHONOME_VOWEL_ALT_COUNT);
		#undef PHONOME_VOWEL
		
		i = 0;
		#define PHONOME_CONSONANT_ALT(str, alt) \
		{ \
			WString ws = String(str).ToWString(); \
			WString ws1 = String(alt).ToWString(); \
			cons_alts[i][0] = ws[0]; \
			cons_alts[i][1] = ws.GetCount() > 1 ? ws[1] : 0; \
			cons_alts[i][2] = ws1[0]; \
			cons_alts[i][3] = ws1.GetCount() > 1 ? ws1[1] : 0; \
			i++; \
		}
		PHONOME_CONSONANT_ALTS
		ASSERT(i == PHONOME_CONSONANT_ALT_COUNT);
		#undef PHONOME_CONSONANT
		
	}
	
	int e = -1;
	int len = 0;
	
	for(int i = 0; i < PHONOME_VOWEL_ALT_COUNT; i++) {
		if (vowel_alts[i][0] == c0 && vowel_alts[i][1] == 0) {c0 = vowel_alts[i][2]; c1 = vowel_alts[i][3]; break;}
		if (vowel_alts[i][0] == c0 && vowel_alts[i][1] != 0 && vowel_alts[i][1] == c1) {c0 = vowel_alts[i][2]; c1 = vowel_alts[i][3]; break;}
	}
	for(int i = 0; i < PHONOME_CONSONANT_ALT_COUNT; i++) {
		if (cons_alts[i][0] == c0 && cons_alts[i][1] == 0) {c0 = cons_alts[i][2]; c1 = cons_alts[i][3]; break;}
		if (cons_alts[i][0] == c0 && cons_alts[i][1] != 0 && cons_alts[i][1] == c1) {c0 = cons_alts[i][2]; c1 = cons_alts[i][3]; break;}
	}
	for(int i = 0; i < PHONOME_COUNT; i++) {
		if (phonemes[i][0] == c0 && phonemes[i][1] == 0) {e = i; len = 1;}
		if (phonemes[i][0] == c0 && phonemes[i][1] != 0 && phonemes[i][1] == c1) {e = i; len = 2;}
	}
	
	/*if (e < 0) {
		WString ws;
		ws.Cat(c0);
		LOG(ws.ToString());
		Panic("Unimplemented");
	}*/
	if (cur) *cur += len;
	return e;
}

bool IsPhonemeVowel(int phoneme) {
	ASSERT(phoneme >= 0 && phoneme < PHONOME_COUNT);
	return phoneme >= 0 && phoneme < PHONOME_VOWEL_COUNT;
}

bool IsPhonemeConsonant(int phoneme) {
	ASSERT(phoneme >= 0 && phoneme < PHONOME_COUNT);
	return phoneme >= PHONOME_VOWEL_COUNT && phoneme < PHONOME_COUNT;
}

int GetPhonemeDuration(int phoneme, int stress) {
	ASSERT(phoneme >= 0 && phoneme < PHONOME_COUNT);
	int ms = 0;
	switch (phoneme) {
		#define PHONOME_VOWEL(a, b, d, r) case PHONOME_##a: ms = d * r; break;
		#define PHONOME_CONSONANT(a, b, d, r) case PHONOME_##a: ms = d * r; break;
		PHONOME_VOWELS
		PHONOME_CONSONANTS
		#undef PHONOME_VOWEL
		#undef PHONOME_CONSONANT
	}
	if (stress == STRESS_NONE) {
		ms = (ms * 8000 + 5) / 10000;
	}
	else if (stress == STRESS_PRIMARY) {
		ms = (ms * 12000 + 5) / 10000;
	}
	return ms;
}

int EstimatePhonemeSyllables(const WString& w) {
	if (w.GetCount() == 0 || w[0] == '-')
		return 0;
	
	thread_local static Vector<int> phonemes;
	
	phonemes.SetCount(0);
	int count = 1;
	for(int i = 0; i < w.GetCount();) {
		int chr0 = w[i];
		int chr1 = i+1 < w.GetCount() ? w[i+1] : 0;
		
		int len = 0;
		int phoneme = GetPhonemeEnum(chr0, chr1, &len);
		if (!len)
			i++;
		else
			i += len;
		
		if (phoneme >= 0)
			phonemes << phoneme;
	}
	
	bool was_vowel = false;
	int vowel_islands = 0;
	for(int i = 0; i < phonemes.GetCount(); i++) {
		int ph0 = phonemes[i];
		//int ph1 = i+1 < phonemes.GetCount() ? phonemes[i+1] : -1;
		
		bool is_vowel = IsPhonemeVowel(ph0);
		//bool next_cons = ph1 >= 0 ? IsPhonemeConsonant(ph1) : false;
		
		if (!was_vowel && is_vowel)
			vowel_islands++;
		
		was_vowel = is_vowel;
	}
	if  (vowel_islands > 1)
		count += vowel_islands - 1;
	
	return count;
}

int GetPhonemeRepeats(int phoneme, int stress) {
	ASSERT(phoneme >= 0 && phoneme < PHONOME_COUNT);
	int repeats = 0;
	switch (phoneme) {
		#define PHONOME_VOWEL(a, b, d, r) case PHONOME_##a: repeats = r; break;
		#define PHONOME_CONSONANT(a, b, d, r) case PHONOME_##a: repeats = r; break;
		PHONOME_VOWELS
		PHONOME_CONSONANTS
		#undef PHONOME_VOWEL
		#undef PHONOME_CONSONANT
	}
	if (stress == STRESS_NONE) {
		repeats = (repeats * 8000) / 10000;
	}
	else if (stress == STRESS_PRIMARY) {
		repeats = (repeats * 12000) / 10000;
	}
	return repeats;
}

double GetSpellingDistance(const WString& w0, const WString& w1, bool relative) {
	static thread_local Vector<int> ph0, ph1;
	static thread_local Vector<double> mat;
	
	if (w0.IsEmpty() || w1.IsEmpty())
		return INT_MAX;
	
	// Explanation:
	// https://www.occasionalenthusiast.com/phonetic-distance-between-words-with-application-to-the-international-spelling-alphabet/#appendix-c
	
	
	// Make phoneme time series vectors
	//const int step = 50;
	for(int i = 0; i < 2; i++) {
		Vector<int>& ph = i == 0 ? ph0 : ph1;
		const WString& w = i == 0 ? w0 : w1;
		ph.SetCount(0);
		const wchar* it = w.Begin();
		const wchar* end = w.End();
		int vowel_count = 0;
		bool double_len = false;
		while (it < end) {
			// TODO stress
			if (it[0] == 712 || // ˈ
				it[0] == 716 || // ˌ
				it[0] == 183 || // ˌ
				it[0] == '.'
				) {
				it++;
				continue;
			}
			if (it[0] == ':' ||
				it[0] == 720) {
				double_len = true;
				it++;
				continue;
			}
			int cur = 0;
			int phoneme = GetPhonemeEnum(it[0], it[1], &cur);
			if (phoneme < 0) {
				it++;
				continue;
			}
			bool is_vowel = IsPhonemeVowel(phoneme);
			int stress = STRESS_SECONDARY;
			if (is_vowel) {
				if (!vowel_count)
					stress = STRESS_PRIMARY;
				vowel_count++;
			}
			/*int duration = GetPhonemeDuration(phoneme, stress);
			int steps = duration / step;
			int begin0 = ph.GetCount();
			int end0 = begin0 + steps;*/
			int repeats = GetPhonemeRepeats(phoneme, stress);
			if (double_len) {
				double_len = false;
				repeats *= 2;
			}
			int begin0 = ph.GetCount();
			int end0 = begin0 + repeats;
			ph.SetCount(end0, phoneme);
			if (!cur) break;
			it += cur;
		}
	}
	
	
	// Make distance matrix (see Appendix C)
	// - reverse second string
	int cols = ph0.GetCount();
	int rows = ph1.GetCount();
	int total = cols * rows;
	mat.SetCount(total);
	
	double* dist_it = mat.Begin();
	const int* it1 = ph1.Begin();
	const int* end1 = ph1.End();
	while (it1 != end1) {
		int c1 = *it1;
		bool is_vowel1 = c1 < PHONOME_VOWEL_COUNT;
		const int* it0 = ph0.End()-1;
		const int* end0 = ph0.Begin()-1;
		while (it0 != end0) {
			int c0 = *it0;
			bool is_vowel0 = c0 < PHONOME_VOWEL_COUNT;
			double distance;
			if (is_vowel0 != is_vowel1) {
				distance = 1.0;
			}
			else if (is_vowel0) {
				distance = vowel_distance[c0][c1];
			}
			else {
				int con0 = c0 - PHONOME_VOWEL_COUNT;
				int con1 = c1 - PHONOME_VOWEL_COUNT;
				distance = consonant_distance[con0][con1];
			}
			*dist_it++ = distance;
			it0--;
		}
		it1++;
	}
	ASSERT(dist_it == mat.End());
	
	
	// Sum each of the grid diagonals to get the final convolution value
	const double* distances = mat.Begin();
	double lowest_dist = DBL_MAX;
	int lowest_col , lowest_row;
	int count = max(rows, cols);
	int common = min(rows, cols);
	// Loop left column rows down
	{
		for (int row = 0; row < rows; row++) {
			int row0 = row;
			double distance_sum = 0;
			int max_row_calc = row+1;
			int max_col_calc = min(common, max_row_calc);
			int skipped = count - max_col_calc;
			for (int col = 0; col < max_col_calc; col++) {
				int pos = row0 * cols + col;
				double distance = distances[pos];
				distance_sum += distance;
				row0--;
			}
			distance_sum += skipped * 0.75; // add 'null' distance. see web page
			if (distance_sum < lowest_dist) {
				//lowest_col = 0;
				//lowest_row = row;
				lowest_dist = distance_sum;
			}
		}
	}
	// Loop bottom row columns right
	{
		for (int col = 1; col < cols; col++) {
			int col0 = col;
			double distance_sum = 0;
			int max_col_calc = cols - col;
			int max_row_calc = min(common, max_col_calc);
			int skipped = count - max_row_calc;
			int end_row = rows - max_row_calc;
			for (int row = rows-1; row >= end_row; row--) {
				int pos = row * cols + col0;
				double distance = distances[pos];
				distance_sum += distance;
				col0++;
			}
			distance_sum += skipped * 0.75; // add 'null' distance. see web page
			if (distance_sum < lowest_dist) {
				//lowest_col = col;
				//lowest_row = rows-1;
				lowest_dist = distance_sum;
			}
		}
	}
	
	if (0) {
		DUMPC(ph0);
		DUMPC(ph1);
		DUMP(lowest_col);
		DUMP(lowest_row);
	}
	
	
	
	
	if (relative) {
		double av_ph_cnt = (cols + rows) * 0.5;
		double av_ph_cnt_pow;
		// distance difference sensitivity (*2)
		switch (4) {
			case 6: av_ph_cnt_pow = av_ph_cnt * av_ph_cnt * av_ph_cnt; break;
			case 5: av_ph_cnt_pow = pow(av_ph_cnt, 2.5); break;
			case 4: av_ph_cnt_pow = av_ph_cnt * av_ph_cnt; break;
			case 3: av_ph_cnt_pow = pow(av_ph_cnt, 1.5); break;
			case 2: av_ph_cnt_pow = av_ph_cnt; break;
			default: break;
		}
		double rel_dist = lowest_dist / av_ph_cnt_pow;
		return rel_dist;
	}
	else {
		return lowest_dist;
	}
}

double GetSpellingRawDistance(const WString& w0, const WString& w1) {
	return GetSpellingDistance(w0, w1, false);
}

double GetSpellingRelativeDistance(const WString& w0, const WString& w1) {
	return GetSpellingDistance(w0, w1, true);
}

END_UPP_NAMESPACE
