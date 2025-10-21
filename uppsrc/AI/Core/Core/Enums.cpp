#include "Core.h"

NAMESPACE_UPP

const char* ScoreTitles[SCORE_COUNT] = {
	// Statistical measurements
	
		"Idea",
		"Emotion",
		"Hook",
		"Share",
		"Value",
	
	// Human value measurements (meaning)
	
		"Comedy",
		"Sex",
		"Politics",
		"Love",
		"Social issues"
	
};

const char* ScoreDescriptions[SCORE_COUNT] = {
	// Statistical measurements
	
		"High like count means that the idea score is high",
		"High comment count means that the emotion score is high",
		"High listen count means that the hook score is high",
		"High share count means that the share score is high",
		"High bookmark count means that the value score is high",
	
	// Human value measurements (meaning)
	
		"People finding the text funny means that the comedy score is high",
		"People finding the text sensual means that the sex score is high",
		"People finding the text thought-provoking means that the politics score is high",
		"People finding the text romantic means that the love score is high",
		"People finding the text impactful means that the social issues score is high"
	
};

String GetScoreTitle(int score) {
	if (score >= 0 && score < SCORE_COUNT)
		return ScoreTitles[score];
	else
		return "";
}

String GetScoreDescription(int score) {
	if (score >= 0 && score < SCORE_COUNT)
		return ScoreDescriptions[score];
	else
		return "";
}

String GetScoreKey(int score) {
	return "sc(" + IntStr(score) + ")";
}

#if 0
String GetCategoryString(int i) {
	switch (i) {
		case CATEGORY_CODE:				return "Code";
		case CATEGORY_ECS:				return "ECS";
		
		case CATEGORY_ASSET:			return "Asset";
		case CATEGORY_DISPOSABLE:		return "Disposable";
		
		case CATEGORY_PRIVATE:			return "Private";
		case CATEGORY_PUBLIC:			return "Public";
		
		case CATEGORY_MALE:				return "Male";
		case CATEGORY_FEMALE:			return "Female";
		
		case CATEGORY_BUYER:			return "Buyer";
		case CATEGORY_SELLER:			return "Seller";
		
		case CATEGORY_MARKETER:			return "Marketer";
		case CATEGORY_CONSUMER:			return "Consumer";
		
		case CATEGORY_SOUND:			return "Sound";
		case CATEGORY_TEXT:				return "Text";
		
		case CATEGORY_PHOTO:			return "Photo";
		case CATEGORY_VIDEO:			return "Video";
		
		default: return String();
	}
}

String GetCategoryGroupString(int i) {
	switch (i) {
		case CATEGORY_GROUP_PROGRAMMING:	return "Programming";
		case CATEGORY_GROUP_VALUE:			return "Value";
		case CATEGORY_GROUP_VISIBILITY:		return "Visibility";
		case CATEGORY_GROUP_GENDER:			return "Gender";
		case CATEGORY_GROUP_TRANSACTION:	return "Transaction";
		case CATEGORY_GROUP_DESIRABILITY:	return "Desirability";
		case CATEGORY_GROUP_AUDIO_PRODUCT:	return "Audio product";
		case CATEGORY_GROUP_VISUAL_PRODUCT:	return "Visual product";
		
		default: return String();
	}
}

Vector<String> GetCategories() {
	Vector<String> v;
	for(int i = 0; i < CATEGORY_COUNT; i++)
		v << GetCategoryString(i);
	return v;
}

int FindCategory(const String& s) {
	String ls = ToLower(TrimBoth(s));
	if (ls.IsEmpty()) return -1;
	if (IsDigit(ls[0]) || ls[0] == '-') {
		int i = ScanInt(ls);
		if (i < 0 || i >= CATEGORY_COUNT) return -1;
		return i;
	}
	for(int i = 0; i < CATEGORY_COUNT; i++)
		if (ToLower(GetCategoryString(i)) == ls)
			return i;
	return -1;
}
#endif

String GetMarketPriorityKey(int i) {
	switch (i) {
		case MARKETPRIORITY_IN_SALE:		return "In sale";
		case MARKETPRIORITY_SELL_UPCOMING:	return "Upcoming sell";
		case MARKETPRIORITY_SOLD:			return "Sold";
		case MARKETPRIORITY_POSTPONE_SELL:	return "Postpone sell";
		case MARKETPRIORITY_WONT_SELL:		return "Won't sell";
		default: return "";
	}
}

String GetBiographyCategoryEnum(int i) {
	switch (i) {
		#define BIOCATEGORY(x) case BIOCATEGORY_##x: return #x;
		BIOCATEGORY_LIST
		#undef BIOCATEGORY
		default: return String();
	}
}

const char* GetBiographyCategoryEnumCstr(int i) {
	switch (i) {
		#define BIOCATEGORY(x) case BIOCATEGORY_##x: return #x;
		BIOCATEGORY_LIST
		#undef BIOCATEGORY
		default: return String();
	}
}

int FindBiographyCategoryEnum(String s) {
	for(int i = 0; i < BIOCATEGORY_COUNT; i++) {
		if (s == GetBiographyCategoryEnumCstr(i))
			return i;
	}
	return -1;
}

String GetBiographyCategoryKey(int i) {
	return KeyToName(GetBiographyCategoryEnum(i));
}

String GetSocietyRoleScoreEnum(int i) {
	switch (i) {
		#define SOCIETYROLE_SCORE(x) case SOCIETYROLE_SCORE_##x: return #x;
		SOCIETYROLE_SCORE_LIST
		#undef SOCIETYROLE_SCORE
		default: return String();
	}
}

String GetSocietyRoleScoreKey(int i) {return KeyToName(GetSocietyRoleScoreEnum(i));}



String GetSocietyRoleEnum(int i) {
	switch (i) {
		#define SOCIETYROLE(x) case SOCIETYROLE_##x: return #x;
		SOCIETYROLE_LIST
		#undef SOCIETYROLE
		default: return String();
	}
}

String GetSocietyRoleKey(int i) {return KeyToName(GetSocietyRoleEnum(i));}


String GetSocietyRoleDescription(int role_i) {
	switch (role_i) {
		case SOCIETYROLE_MOTHER: return "biological mother figures for a boy with special focus on mother-son relationship";
		case SOCIETYROLE_FATHER: return "biological father figures for a boy with special focus on boy-father relationship";
		case SOCIETYROLE_LITTLE_SISTER: return "biological little sister figures for a boy with special focus on sister-brother relationship";
		case SOCIETYROLE_BIG_SISTER: return "biological big sister figures for a boy with special focus on sister-brother relationship";
		case SOCIETYROLE_LITTLE_BROTHER: return "biological little brother figures for a big brother with special focus on brother-brother relationship";
		case SOCIETYROLE_BIG_BROTHER: return "biological big brother figures for a boy with special focus on brother-brother relationship";
		case SOCIETYROLE_PATERNAL_GRANDMOTHER: return "biological paternal grandmother figures for a boy with special focus on grandmother-grandson relationship";
		case SOCIETYROLE_PATERNAL_GRANDFATHER: return "biological paternal grandfather figures for a boy with special focus on grandfather-grandson relationship";
		case SOCIETYROLE_MATERNAL_GRANDMOTHER: return "biological maternal grandmother figures for a boy with special focus on grandmother-grandson relationship";
		case SOCIETYROLE_MATERNAL_GRANDFATHER: return "biological maternal grandfather figures for a boy with special focus on grandfather-grandson relationship";
		case SOCIETYROLE_MATERNAL_UNCLE: return "biological maternal uncle figures for a boy with special focus on uncle-nephew relationship";
		case SOCIETYROLE_PATERNAL_UNCLE: return "biological paternal uncle figures for a boy with special focus on uncle-nephew relationship";
		case SOCIETYROLE_MATERNAL_AUNT: return "biological maternal aunt figures for a boy with special focus on aunt- nephew relationship";
		case SOCIETYROLE_PATERNAL_AUNT: return "biological paternal aunt figures for a boy with special focus on aunt- nephew relationship";
		case SOCIETYROLE_MATERNAL_FEMALE_COUSIN: return "biological maternal female cousin figures for a male";
		case SOCIETYROLE_MATERNAL_MALE_COUSIN: return "biological maternal male cousin figures for a male";
		case SOCIETYROLE_PATERNAL_FEMALE_COUSIN: return "biological paternal female cousin figures for a male";
		case SOCIETYROLE_PATERNAL_MALE_COUSIN: return "biological paternal male cousin figures for a male";
		case SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_IMAGE_SITE: return "females in shallow image websites (e.g. Instagram) for a young male adult with special focus on impact on self-esteem";
		case SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_IMAGE_SITE: return "males in shallow image websites (e.g. Instagram) for a young male adult with special focus on impact on body image and self-esteem";
		case SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_REEL_VIDEO_SITE: return "females in shallow reel video websites (e.g. TikTok, snapchat) for a young male adult with special focus on impact on personal development";
		case SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_REEL_VIDEO_SITE: return "males in shallow reel video websites (e.g. TikTok, snapchat) for a young male adult with special focus on impact on self-esteem and self-image";
		case SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE: return "females in shallow public message websites (e.g. Twitter, X, Gettr, Mastodon, Truth social, Meta's Threads) for a young male adult with special focus on impact on mental health";
		case SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE: return "males in shallow public message websites (e.g. Twitter, X, Gettr, Mastodon, Truth social, Meta's Threads) for a young male adult with special focus on impact on personal growth and mental health";
		case SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_VIDEO_STREAMING_SITE: return "females in shallow live video streaming websites (e.g. Twitch, kick.com, YouTube Gaming, YouTube) for a young male adult with special focus on impact on mental health";
		case SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_VIDEO_STREAMING_SITE: return "males in shallow live video streaming websites (e.g. Twitch, kick.com, YouTube Gaming, YouTube) for a young male adult with special focus on impact on male identity";
		case SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE: return "females in moderate full profile websites (e.g. Facebook, VK) for a young straight male adult with special focus on impact on romantic relationships";
		case SOCIETYROLE_MALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE: return "males in moderate full profile websites (e.g. Facebook, VK) for a young straight male adult with special focus on impact on their social life";
		case SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_VIDEO_SITE: return "female commentators in moderate social persistent and non-streaming video websites (e.g. YouTube, Vimeo) for a young straight male adult with special focus on impact on self-image and self-esteem";
		case SOCIETYROLE_MALE_IN_INTERNET_MODERATE_VIDEO_SITE: return "male commentators in moderate social persistent and non-streaming video websites (e.g. YouTube, Vimeo) for a young straight male adult with special focus on impact on self-perception and behavior";
		case SOCIETYROLE_FEMALE_IN_INTERNET_PROFSSIONAL_INDUSTRY_SITE: return "females in professional full profile websites (e.g. LinkedIn) from the perspective of a young straight male adult with special focus on mentorship and networking";
		case SOCIETYROLE_MALE_IN_INTERNET_PROFSSIONAL_INDUSTRY_SITE: return "males in professional full profile websites (e.g. LinkedIn) from the perspective of a young straight male adult with special focus on representation of masculinity";
		case SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_MUSIC_SITE: return "females in social music websites (e.g. Soundcloud) from the perspective of a young male adult singer with special focus on collaboration and networking";
		case SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_MUSIC_SITE: return "males in social music websites (e.g. Soundcloud) from the perspective of a young male adult singer with special focus on male-female relationship";
		case SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE: return "females in social programming websites (e.g. Github, Sourceforge) from the perspective of a young male with special focus on mentorship and collaboration";
		case SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE: return "males in social programming websites (e.g. Github, Sourceforge) from the perspective of a young male with special focus on mentorship and skill development";
		case SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC: return "females in music forum websites (e.g. soundonsound.com, forums.songstuff.com, reddit.com) from the perspective of a young male music artist with special focus on possible mentor figures";
		case SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC: return "males in music forum websites (e.g. soundonsound.com, forums.songstuff.com, reddit.com) from the perspective of a young male music artist with special focus on male-male relationships";
		case SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_PROGRAMMING: return "females in computer programming forum websites (e.g. stackoverflow.com, reddit.com) from the perspective of a straight male senior c++ programmer with special focus on the role of women in the field";
		case SOCIETYROLE_MALE_IN_INTERNET_FORUM_PROGRAMMING: return "males in computer programming forum websites (e.g. stackoverflow.com, reddit.com) from the perspective of a straight male senior c++ programmer with special focus on career advancement and professional growth";
		case SOCIETYROLE_FEMALE_IN_INTERNET_WEBSITE_READER: return "females in Internet reading the personal website from the perspective of a straight male (a music artist and a senior c++ programmer) with special focus on their potential relationship";
		case SOCIETYROLE_MALE_IN_INTERNET_WEBSITE_READER: return "males in Internet reading the personal website from the perspective of a straight male (a music artist and a senior c++ programmer) with special focus on representation of masculinity";
		case SOCIETYROLE_FEMALE_IN_INTERNET_MUSIC_COOPERATION_SITE: return "females in music collaboration website (e.g. bandmix.com) from the perspective of a male music artist (singer, guitar player, producer, composer, lyricist) with special focus on collaboration";
		case SOCIETYROLE_MALE_IN_INTERNET_MUSIC_COOPERATION_SITE: return "males in music collaboration website (e.g. bandmix.com) from the perspective of a male music artist (singer, guitar player, producer, composer, lyricist) with special focus on collaboration and creative partnerships";
		case SOCIETYROLE_FEMALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE: return "females in c++ project collaboration website (e.g. GitHub) from the perspective of a male senior c++ programmer with special focus on working relationship";
		case SOCIETYROLE_MALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE: return "males in c++ project collaboration website (e.g. GitHub) from the perspective of a male senior c++ programmer with special focus on working relationship";
		case SOCIETYROLE_FEMALE_PEER_IN_MILITARY_SERVICE: return "same rank peers (not superior nor inferior) females in military service from the perspective of a straight male with special focus on potential romantic relationships";
		case SOCIETYROLE_MALE_PEER_IN_MILITARY_SERVICE: return "same rank peers (not superior nor inferior) males in military service from the perspective of a straight male with special focus on respect and camaraderie";
		case SOCIETYROLE_FEMALE_SUPERIOR_IN_MILITARY_SERVICE: return "superior rank females in military service from the perspective of a straight male with special focus on mentorship and leadership";
		case SOCIETYROLE_MALE_SUPERIOR_IN_MILITARY_SERVICE: return "superior rank males in military service from the perspective of a straight male with special focus on mentorship and camaraderie";
		case SOCIETYROLE_FEMALE_INFERIOR_IN_MILITARY_SERVICE: return "inferior rank females in military service from the perspective of a straight male with special focus on the female-male dynamic";
		case SOCIETYROLE_MALE_INFERIOR_IN_MILITARY_SERVICE: return "inferior rank males in military service from the perspective of a superior straight male with special focus on relationships";
		case SOCIETYROLE_WIFE: return "wife figures for a straight male with special focus on wife-husband relationship";
		case SOCIETYROLE_BEST_MAN: return "best man for a straight male with special focus on best man-groom relationship";
		case SOCIETYROLE_DAUGHTER: return "";
		case SOCIETYROLE_SON: return "biological son figures for a father with special focus on father-boy relationship";
		case SOCIETYROLE_FEMALE_RECRUITER_FOR_WORK: return "female recruiters to get a unemployed white straight male to work with special focus on building a professional relationship";
		case SOCIETYROLE_MALE_RECRUITER_FOR_WORK: return "male recruiters to get a unemployed white straight male to work with special focus on skills and background";
		case SOCIETYROLE_FEMALE_IN_INTERNET_REPUBLICAN_PUBLIC_MESSAGE_SITE: return "females in Republican public message websites (e.g. Gettr, Truth social) from the perspective of a young straight male adult with special focus on their beliefs and values";
		case SOCIETYROLE_MALE_IN_INTERNET_REPUBLICAN_PUBLIC_MESSAGE_SITE: return "males in Republican public message websites (e.g. Gettr, Truth social) from the perspective of a young straight male adult with special focus on political views";
		case SOCIETYROLE_FEMALE_IN_INTERNET_INFLUENCER_FOR_ART_AND_MUSIC: return "female influencers for art and music in Internet and social media from the perspective of a young straight male adult with special focus on their impact";
		case SOCIETYROLE_MALE_IN_INTERNET_INFLUENCER_FOR_ART_AND_MUSIC: return "male influencers for art and music in Internet and social media from the perspective of a young straight male adult with special focus on how they inspire and influence";

		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_PRODUCERS: return "representative of the organization for rights of music producers with special focus on supporting music producers";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_PRODUCERS: return "representative of the organization for interest of music producers with special focus on their role and impact on the industry";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_COMPOSERS: return "representative of the organization for rights of music composers with special focus on their contributions";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_COMPOSERS: return "representative of the organization for interest of music composers with special focus on their roles";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_LYRICISTS: return "representative of the organization for rights of music lyricists with special focus on their role in the music industry";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_LYRICISTS: return "representative of the organization for interest of music lyricists with special focus on their role and impact";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_PUBLISHERS: return "representative of the organization for rights of music publishers with special focus on their role and responsibilities";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_PUBLISHERS: return "representative of the organization for interest of music publishers with special focus on their cooperation";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_PERFORMING_ARTISTS: return "representative of the organization for rights of performing artists with special focus on their work";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_PERFORMING_ARTISTS: return "representative of the organization for interest of performing artists with special focus on their relationship with performers";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_RECORD_COMPANIES: return "representative of the organization for rights of record companies with special focus on maintaining positive relationships with artists";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_RECORD_COMPANIES: return "representative of the organization for interest of record companies with special focus on the organization's relationship with record companies";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_JOURNALISTS: return "representative of the organization for rights of music journalists with special focus on promoting music journalism";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_JOURNALISTS: return "representative of the organization for interest of music journalists with special focus on diversity and inclusion";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSICIANS_AND_MUSIC_WORKERS: return "representative of the organization for rights of musicians and music workers with special focus on monetary and job security";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSICIANS_AND_MUSIC_WORKERS: return "representative of the organization for interest of musicians and music workers with special focus on monetary compensation and support for its members";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_INDUSTRY: return "representative of the organization for rights of music industry with special focus on their role and impact on the music industry";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_INDUSTRY: return "representative of the organization for interest of music industry with special focus on mentorship and talent development";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_EDUCATORS: return "representative of the organization for rights of music educators with special focus on advocating for music education";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_EDUCATORS: return "representative of the organization for interest of music educators with special focus on advocacy and support for music education";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_RECORD_ARCHIVERS: return "representative of the organization for rights of music record archivers with special focus on persistency";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_RECORD_ARCHIVERS: return "representative of the organization for interest of music record archivers with special focus on persistency and dedication";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_ACCOUNTANTS_OF_MUSIC_PRODUCERS: return "representative of the organization for rights of accountants of music producers with special focus on monetary and job security";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_ACCOUNTANTS_OF_MUSIC_PRODUCERS: return "representative of the organization for interest of accountants of music producers with special focus on monetary and job security";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_ARTIST_MANAGERS: return "representative of the organization for rights of music artist managers with special focus on their role";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_ARTIST_MANAGERS: return "representative of the organization for interest of music artist managers with special focus on their role";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_CONSUMERS: return "representative of the organization for rights of music consumers with special focus on small artists and newcomers";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_CONSUMERS: return "representative of the organization for interest of music consumers with special focus on youth";

		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_COMPUTER_PROGRAMMERS: return "representative of the organization for rights of computer programmers with special focus on diversity and inclusion";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_COMPUTER_PROGRAMMERS: return "representative of the organization for interest of computer programmers with special focus on mentor-mentee relationships";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_ACCOUNTANTS_OF_COMPUTER_PROGRAMMERS: return "representative of the organization for rights of accountants of computer programmers with special focus on job security";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_ACCOUNTANTS_OF_COMPUTER_PROGRAMMERS: return "representative of the organization for interest of accountants of computer programmers with special focus on job security";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_SOFTWARE_COMPANIES: return "representative of the organization for rights of software companies with special focus on the organization's goals and impact";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_SOFTWARE_COMPANIES: return "representative of the organization for interest of software companies with special focus on organization-company relationship";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_SOFTWARE_INDUSTRY: return "representative of the organization for rights of software industry as whole with special focus on the welfare of geeky weak male programmers";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_SOFTWARE_INDUSTRY: return "representative of the organization for interest of software industry as whole with special focus on the welfare of geeky weak male programmers";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_SOFTWARE_PROJECT_MANAGERS: return "representative of the organization for rights of software project managers with special focus on managing teams";
		case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_SOFTWARE_PROJECT_MANAGERS: return "representative of the organization for interest of software project managers with special focus on organization-project manager relationship";
		
        case SOCIETYROLE_ANGRY_PERSON_IN_THE_INTERNET: return "angry person in the internet with special focus on effects of anger";
        case SOCIETYROLE_EMPATHETIC_PERSON_IN_THE_INTERNET: return "empathetic person in the internet with special focus on connecting with people";
        case SOCIETYROLE_CURIOUS_PERSON_IN_THE_INTERNET: return "curious person in the internet with special focus on diversity";
        case SOCIETYROLE_ENTHUSIASTIC_PERSON_IN_THE_INTERNET: return "enthusiastic person in the internet with special focus on social media";
        case SOCIETYROLE_SKEPTICAL_PERSON_IN_THE_INTERNET: return "skeptical person in the internet with special focus on handling online interactions";
        case SOCIETYROLE_CONFUSED_PERSON_IN_THE_INTERNET: return "confused person in the internet with special focus on navigating online platforms";
        case SOCIETYROLE_EMOTIONAL_PERSON_IN_THE_INTERNET: return "emotional person in the internet with special focus on digital relationships";
        case SOCIETYROLE_CRITICAL_PERSON_IN_THE_INTERNET: return "critical person in the internet with special focus on online behavior";
        case SOCIETYROLE_MOTIVATIONAL_PERSON_IN_THE_INTERNET: return "motivational person in the internet with special focus on their impact on youth";
        case SOCIETYROLE_SUPPORTIVE_PERSON_IN_THE_INTERNET: return "supportive person in the internet with special focus on mental health and well-being";
        case SOCIETYROLE_ANXIOUS_PERSON_IN_THE_INTERNET: return "anxious person in the internet with special focus on management and treatment";
        case SOCIETYROLE_HUMOROUS_PERSON_IN_THE_INTERNET: return "humorous person in the internet with special focus on their content and style";
        case SOCIETYROLE_DEFENSIVE_PERSON_IN_THE_INTERNET: return "defensive person in the internet with special focus on online behavior and interactions";
        case SOCIETYROLE_OVERWHELMED_PERSON_IN_THE_INTERNET: return "overwhelmed person in the internet with special focus on developing healthy coping mechanisms";
        case SOCIETYROLE_NOSTALGIC_PERSON_IN_THE_INTERNET: return "nostalgic person in the internet with special focus on their online behavior";
        case SOCIETYROLE_OBJECTIVE_PERSON_IN_THE_INTERNET: return "impartial person in the internet with special focus on providing unbiased information";
        
		default: TODO
	}
	return "";
}

const Array<RoleProfile>& GetRoleProfile(int role_i) {
	static Array<RoleProfile> v[SOCIETYROLE_COUNT];
	ASSERT(role_i >= 0 && role_i < SOCIETYROLE_COUNT);
	Array<RoleProfile>& list = v[role_i];
	if (list.IsEmpty()) {
		switch (role_i) {
			case SOCIETYROLE_MOTHER: {
				list.Add().Set("Mother-son bonding", "This mother has a close and deep bond with her son, and they share a special connection. She may have a strong intuition about her son's needs and is always there to support and guide him.");
				list.Add().Set("Single mother raising a son", "This mother takes on the responsibility of raising her son on her own, and may struggle to find a balance between being a caregiver and a father figure. She may work hard to provide a positive male role model for her son.");
				list.Add().Set("Mother figure for a boy without a biological mother", "This mother figure steps in to provide love and support for a boy who may have lost his biological mother or does not have a mother figure in his life. She may offer guidance and care, and fill a significant role in his development.");
				list.Add().Set("Mother-son communication", "This mother prioritizes open and honest communication with her son, creating a safe and trusting environment for him to express his thoughts and feelings. She may actively listen and validate his emotions, and help him navigate challenges.");
				list.Add().Set("Protective mother-son relationship", "This mother is fiercely protective of her son and will do anything to ensure his safety and well-being. She may have a strong maternal instinct to shield him from harm, but also encourages him to be independent and make his own choices.");
				list.Add().Set("Mother figure for a boy from a different culture or ethnicity", "This mother figure may come from a different cultural or ethnic background than her son, but embraces and celebrates their differences. She may share her unique perspective and experiences with her son, enriching his understanding of the world.");
				list.Add().Set("Nurturing and supportive mother", "This mother provides a nurturing and caring environment for her son to thrive in. She may prioritize his emotional well-being and offer unconditional love and acceptance, helping him develop a strong sense of self.");
				list.Add().Set("Mother-son relationship with boundaries", "This mother sets clear boundaries with her son and teaches him the importance of respecting others' boundaries as well. She may also encourage her son to have healthy boundaries in his relationships with others.");
				list.Add().Set("Mother figure for a boy with special needs", "This mother figure takes on the role of caregiver, advocate, and support system for a boy with special needs. She may face unique challenges, but her love and dedication to her son never wavers.");
				list.Add().Set("Authoritative mother-son relationship", "This mother establishes clear rules and expectations for her son, while also providing warmth and support. She may promote independence and decision-making skills, while still having a strong influence on her son's choices.");
				break;
			}
			case SOCIETYROLE_FATHER: {
				list.Add().Set("Stay-at-home father", "This father has made the decision to be the primary caregiver for his child, challenging traditional gender roles. He may struggle with societal expectations, but is a loving and dedicated parent who values his relationship with his son.");
				list.Add().Set("Absentee father", "This father is not actively involved in his child's life, for various reasons such as divorce, distance, or personal struggles. This may affect the boy's relationship with his father, as he may yearn for a stronger connection or understanding.");
				list.Add().Set("Helicopter father", "This father tends to be overly involved in his child's life and constantly monitors and controls their activities. While his intentions may be out of love, his overbearing nature can strain the boy-father relationship.");
				list.Add().Set("Authoritative father", "This father sets strict rules and expectations for his child and expects them to be followed. He may struggle to show affection and praise, but ultimately wants his son to be disciplined and successful.");
				list.Add().Set("Emotional and open father", "This father encourages his son to express his feelings and emotions freely, and shares his own vulnerabilities with his child. He values open communication and works to develop a strong emotional bond with his son.");
				list.Add().Set("Non-traditional father figure", "Similar to a non-traditional mother figure, this father figure may not fit into societal expectations of a traditional father, but plays an important role in a person's life. He may be a mentor, teacher, coach, or community leader who provides guidance and support to his son.");
				list.Add().Set("Co-parenting father", "This father shares custody or co-parents with the child's mother, and together they work to provide a stable and loving environment for their child. He values cooperation and compromise for the sake of his son.");
				list.Add().Set("Single father", "This father is raising his child on his own, and may face additional challenges and judgement as a single parent. He is strong and resilient, and is fiercely dedicated to being a loving and supportive father figure for his son.");
				list.Add().Set("Father who struggled with mental health", "This father may have faced personal struggles with mental health, but has worked towards recovery and rebuilding his relationship with his son. He may have made mistakes in the past, but is committed to being present and creating a positive relationship with his son.");
				list.Add().Set("Adoptive father", "This father may have chosen to adopt a child, and may face unique challenges in building a strong father-son relationship. He values love and acceptance, and works to create a strong bond with his adopted son.");
				break;
			}
			case SOCIETYROLE_LITTLE_SISTER: {
				list.Add().Set("Sisterly bond", "This sister shares a deep bond with her brother and may act as his confidant and best friend. They may have a special understanding and connection, supporting each other through thick and thin.");
				list.Add().Set("Older sister taking care of younger brother", "This older sister takes on the role of a caregiver and role model for her younger brother. She may help him with his responsibilities, give him advice, and protect him from harm.");
				list.Add().Set("Sister figure for a boy without a biological sister", "This sister figure steps in to provide love and support for a boy who may have lost his biological sister or does not have a sister figure in his life. She may offer guidance and care, and fill a significant role in his development.");
				list.Add().Set("Sister-brother communication", "This sister prioritizes open and honest communication with her brother, creating a safe and trusting space for him to share his thoughts and feelings. She may offer support and advice, and help him navigate challenges.");
				list.Add().Set("Protective sister-brother relationship", "This sister is fiercely protective of her brother and will do anything to ensure his safety and well-being. She may have a strong sense of responsibility towards him and offers guidance and support in his growth.");
				list.Add().Set("Sister figure for a boy from a different culture or ethnicity", "This sister figure may come from a different cultural or ethnic background than her brother, but embraces and celebrates their differences. She may share her unique perspective and experiences with him, enriching his understanding of the world.");
				list.Add().Set("Nurturing and supportive sister", "This sister provides a nurturing and loving home for her brother, and may be a source of comfort and support for him. She may offer encouragement and help him build his confidence.");
				list.Add().Set("Sister-brother bond with boundaries", "This sister has a strong bond with her brother, but also sets healthy boundaries for their relationship. She may teach him the importance of boundaries and respect in relationships.");
				list.Add().Set("Sister figure for a boy with special needs", "This sister figure takes on the role of caregiver, advocate, and support system for a boy with special needs. She may face unique challenges, but her love and dedication to her brother never wavers.");
				list.Add().Set("Sibling rivalry and love", "This sister and brother may have a tumultuous relationship at times with arguments and competition, but ultimately their love for each other shines through. They may learn important lessons about conflict resolution and forgiveness through their relationship.");
				break;
			}
			case SOCIETYROLE_BIG_SISTER: {
				list.Add().Set("Sibling bond", "This big sister has a close and loving relationship with her younger brother. They may be best friends and support each other through life's ups and downs.");
				list.Add().Set("Mentor big sister", "This sister takes the role of a mentor for her brother, offering guidance and advice based on her own experiences. She may also push him to become his best self.");
				list.Add().Set("Protective big sister", "This sister is fiercely protective of her brother and will do anything to keep him safe. She may also give him tough love and teach him how to navigate the world.");
				list.Add().Set("Big sister figure for a boy without a biological sister", "This big sister figure steps in to fill the role of a sister for a boy who may not have a biological sister. She may offer a sibling-like relationship, offering companionship and support.");
				list.Add().Set("Supportive and nurturing big sister", "This sister provides a nurturing and caring environment for her brother, and he may turn to her for comfort and advice. She may also help him develop a strong sense of self-worth.");
				list.Add().Set("Outgoing big sister", "This sister may be the life of the party, and her outgoing nature may bring out the more adventurous side of her brother. They may have a fun-loving and playful relationship.");
				list.Add().Set("Culturally diverse big sister", "This big sister introduces her brother to different cultures and backgrounds, opening his mind to new perspectives. She may also help him embrace and celebrate his own cultural identity.");
				list.Add().Set("Big sister figure for a boy from a different family structure", "This big sister figure may come from a non-traditional family structure, such as a blended family or a single-parent household. She may offer her brother a different perspective on family dynamics and relationships.");
				list.Add().Set("Supportive big sister during difficult times", "This sister may be a source of strength and support for her brother during a difficult time in his life, such as a divorce or loss of a loved one.");
				list.Add().Set("Big sister role model for success", "This sister may be highly ambitious and a role model for her brother to strive for success. She may offer him guidance and support in pursuing his goals and dreams.");
				break;
			}
			case SOCIETYROLE_LITTLE_BROTHER: {
				list.Add().Set("Protective younger brother", "This younger brother looks up to his big brother and sees him as a role model. He may also feel a strong need to protect and support his older brother.");
				list.Add().Set("Mischievous younger brother", "This younger brother loves to have fun and often turns to his big brother for adventure and excitement. They may share a mischievous bond and enjoy pulling pranks or teasing each other.");
				list.Add().Set("Supportive and understanding brother relationship", "This younger brother is always there to lend an ear and offer advice to his big brother. He may have a natural empathy and understanding of his brother's struggles, and encourages and supports him through tough times.");
				list.Add().Set("Competitive brothers", "These brothers have a strong competitive streak and push each other to be better. They may play sports or games together and enjoy the challenge and rivalry that comes with it.");
				list.Add().Set("Little brother with big aspirations", "This younger brother has big dreams and looks up to his big brother for inspiration and guidance. He may view his older sibling as a mentor and strive to follow in their footsteps.");
				list.Add().Set("Protective younger brother figure for a brother without a biological little brother", "This younger brother figure may step into the role of a protector and confidant for a boy who does not have a biological little brother. He may offer a brotherly bond and companionship to fill a significant void.");
				list.Add().Set("Little brother with special needs", "This younger brother may have special needs, but his big brother loves and accepts him unconditionally. He may also have a unique understanding and patience towards his younger brother's needs.");
				list.Add().Set("Bonded brothers through adoption or blended family", "These brothers may not be biologically related but have formed a strong bond through adoption or a blended family. They may have shared experiences and challenges that have brought them closer together.");
				list.Add().Set("Intellectual brothers", "These brothers share a love for learning and knowledge. They may engage in enriching conversations and debates, and support each other's academic pursuits.");
				list.Add().Set("Little brother who looks up to his big brother's hobbies and interests", "This younger brother may not have the same hobbies and interests as his big brother, but looks up to his sibling and admires their skills and talents. He may often join in and try to emulate his brother's interests.");
				break;
			}
			case SOCIETYROLE_BIG_BROTHER: {
				list.Add().Set("Protective and loving older brother", "This big brother is fiercely protective of his little brother and always has his back. He may take on a caretaker role and serve as a mentor and role model for his younger brother.");
				list.Add().Set("Big brother as a friend and confidant", "This older brother has a close and open relationship with his little brother, and they share a strong bond of friendship. He may offer advice and support to his brother, and they enjoy spending time together.");
				list.Add().Set("Big brother teaching life skills", "This older brother takes on a teaching role and helps his little brother learn important life skills, such as how to ride a bike or tie his shoes. He may also share his knowledge and experience to help his brother navigate challenges.");
				list.Add().Set("Brotherly competition and rivalry", "This big brother and little brother have a competitive relationship, always trying to one-up each other. While they may butt heads at times, they also have a strong bond and a healthy sense of competition.");
				list.Add().Set("Protective brotherhood", "This big brother is not only protective of his little brother, but also of their relationship and bond. He may make sacrifices and put his little brother's needs above his own.");
				list.Add().Set("Older brother as a mentor", "This big brother takes on a mentor role and helps guide his little brother through various milestones and challenges. He may offer advice and share his experiences to support his brother's growth and development.");
				list.Add().Set("Big brother as a role model", "This older brother sets a positive example for his younger brother, whether it's through his academics, career, or personal values. He may inspire his little brother to strive for success and reach his potential.");
				list.Add().Set("Supportive brotherhood", "This big brother provides emotional and practical support for his little brother, always there to lend a listening ear or a helping hand. He may also encourage his brother to pursue his interests and passions.");
				list.Add().Set("Protective brother with tough love", "This big brother may have a tough exterior, but deep down he cares for his little brother and wants the best for him. He may use tough love to guide his brother and help him learn from his mistakes.");
				list.Add().Set("Big brother introducing new experiences", "This older brother exposes his little brother to new and exciting experiences, broadening his perspective and teaching him about the world. He may also encourage him to step out of his comfort zone and try new things.");
				break;
			}
			case SOCIETYROLE_PATERNAL_GRANDMOTHER: {
				list.Add().Set("Loving and nurturing grandmother", "This grandmother showers her grandson with love and affection, and takes great joy in spending time with him. She may spoil him with treats and gifts, and offer a warm and safe space for him to be himself.");
				list.Add().Set("Traditional and wise grandmother", "This grandmother enriches her grandson's life with traditions and wisdom passed down through generations. She may teach him about their family history and cultural values, and offer guidance on navigating life's challenges.");
				list.Add().Set("Long-distance grandmother figure", "This grandmother may live far away from her grandson, but still maintains a strong bond with him through regular communication and visits. She may share stories and memories from her own childhood and create a special connection with her grandson.");
				list.Add().Set("Strong and independent grandmother", "This grandmother is a role model for her grandson, showing him the strength and resilience of a woman. She may instill in him the importance of perseverance and hard work, and inspire him to be his best self.");
				list.Add().Set("Supportive grandmother in a blended family", "This grandmother enters the family through remarriage or blending of families, and offers love and support to her grandson. She may navigate the challenges of blended families with grace and provide a stable and caring presence for her grandson.");
				list.Add().Set("Fun and playful grandmother figure", "This grandmother brings joy and excitement into her grandson's life with her playful and adventurous spirit. She may take him on new experiences and create fond memories together.");
				list.Add().Set("Inspirational and motivational grandmother", "This grandmother is a role model for her grandson, inspiring him to reach for his dreams and pursue his passions. She may offer valuable advice and encouragement, and be proud of his accomplishments.");
				list.Add().Set("Multi-cultural grandmother figure", "This grandmother may come from a different cultural background than her grandson, but embraces and celebrates their differences. She may expose him to new cultures and traditions, broadening his perspectives and understanding of the world.");
				list.Add().Set("Religious and spiritual grandmother", "This grandmother may offer her grandson a spiritual or religious upbringing and guide him in matters of faith. She may instill in him moral values and principles in life.");
				list.Add().Set("Grandmother as a primary caregiver", "This grandmother takes on the role of a primary caregiver either by choice or necessity. She may provide a stable and loving home for her grandson, and have a strong bond with him due to their close living arrangement.");
				break;
			}
			case SOCIETYROLE_PATERNAL_GRANDFATHER: {
				list.Add().Set("Traditional grandfather figure", "This grandfather embodies the traditional values and beliefs of his culture or family. He may be strict yet loving, and passes down family traditions and wisdom to his grandson.");
				list.Add().Set("Supportive grandfather", "This grandfather is a source of unwavering support and encouragement for his grandson. He may attend his grandson's games and events, and offer guidance and advice whenever needed.");
				list.Add().Set("Adventurous and fun-loving grandfather", "This grandfather loves to bring his grandson on adventures and share new experiences with him. He may have a youthful spirit and energy that resonates with his grandson.");
				list.Add().Set("Entrepreneurial grandfather", "This grandfather may be a successful businessperson or entrepreneur, and teaches his grandson about hard work, determination, and how to be financially responsible.");
				list.Add().Set("Long-distance relationship", "This grandfather may live far away from his grandson, but still maintains a close and loving relationship. He may make an effort to stay in touch through phone calls, letters, or video chats.");
				list.Add().Set("Role model grandfather", "This grandfather serves as a positive role model for his grandson, demonstrating qualities such as kindness, responsibility, and integrity. He may also share life lessons and inspire his grandson to be the best version of himself.");
				list.Add().Set("Grandfather figure for a boy from a different culture or ethnicity", "This grandfather figure may come from a different cultural or ethnic background than his grandson, but shares his heritage and traditions with him. He may also offer a unique perspective on life.");
				list.Add().Set("Veteran grandfather", "This grandfather may have served in the military, and shares his stories and experiences with his grandson. He may also instill values of patriotism and honor in his grandson.");
				list.Add().Set("Nurturing and caring grandfather", "This grandfather provides a loving and nurturing presence for his grandson. He may enjoy spending quality time with his grandson and showing him affection and tenderness.");
				list.Add().Set("Grandfather figure for a boy without a paternal grandfather", "This grandfather figure steps in to fill the void of a paternal grandfather in a boy's life. He may offer guidance, love, and support, and becomes an important figure in his grandson's life.");
				break;
			}
			case SOCIETYROLE_MATERNAL_GRANDMOTHER: {
				list.Add().Set("Loving and nurturing maternal grandmother", "This grandmother showers her grandson with love and affection, always making him feel cherished and special. She may also be a source of comfort and support during difficult times.");
				list.Add().Set("Traditional maternal grandmother", "This grandmother holds onto old-fashioned values and traditions, imparting them onto her grandson. She may teach him about his cultural or familial heritage and pass down important life lessons.");
				list.Add().Set("Maternal grandmother raising her grandson", "This grandmother may have taken on the role of primary caregiver for her grandson, either as a single grandmother or as part of a multigenerational household. She may face unique challenges but is dedicated to providing a stable and loving home for her grandson.");
				list.Add().Set("Fun and adventurous maternal grandmother", "This grandmother is always up for trying new things and going on exciting adventures with her grandson. She may inspire a sense of curiosity and wonder in him, and provide a fun and light-hearted approach to life.");
				list.Add().Set("Wise and insightful maternal grandmother", "This grandmother has a wealth of life experience and imparts her wisdom onto her grandson. She may be a source of guidance and advice, helping him navigate challenges and make important decisions.");
				list.Add().Set("Long-distance maternal grandmother", "This grandmother may live far away from her grandson, but still maintains a strong connection with him. She may send letters, care packages, or keep in touch through technology to stay involved in his life.");
				list.Add().Set("Maternal grandmother with a blended family", "This grandmother may have a blended family, with a mix of biological and step-grandchildren. She may work hard to create a loving and inclusive environment for all of her grandchildren, regardless of blood relation.");
				list.Add().Set("Artistically talented maternal grandmother", "This grandmother is skilled in a creative craft or hobby, and may pass down this talent to her grandson. She may encourage his artistic expression and help cultivate his talents.");
				list.Add().Set("Entrepreneurial maternal grandmother", "This grandmother may be a successful business owner or entrepreneur, and may inspire her grandson with her drive and determination. She may also share helpful business advice and mentor her grandson in his own pursuits.");
				list.Add().Set("Nurturing and supportive maternal grandmother", "This grandmother provides a loving and supportive environment for her grandson, much like a mother figure. She may play a significant role in his emotional well-being and development.");
				break;
			}
			case SOCIETYROLE_MATERNAL_GRANDFATHER: {
				list.Add().Set("Wise and experienced grandfather", "This grandfather is knowledgeable and experienced, and shares his wisdom with his grandson. He may act as a mentor and guide in the boy's life, teaching him valuable life lessons.");
				list.Add().Set("Grandfather figure for a grandson without a biological grandfather", "This grandfather figure steps in to fill the role of a grandfather for a boy who may not have a biological grandfather present in his life. He may provide love, support, and guidance to the grandson.");
				list.Add().Set("Fun and playful grandfather", "This grandfather has a playful and lighthearted approach to his relationship with his grandson. He may engage in fun activities with the boy and create special memories for them to cherish.");
				list.Add().Set("Grandfather as a source of family history", "This grandfather takes on the role of sharing family history and traditions with his grandson. He may pass down heirlooms and stories to keep the family's legacy alive.");
				list.Add().Set("Grandfather as a role model", "This grandfather serves as a positive role model for his grandson. He may demonstrate important values and characteristics, such as kindness, resilience, and hard work.");
				list.Add().Set("Grandfather as a confidant", "This grandfather creates a safe and trusting environment for his grandson to open up and share his thoughts and feelings. He may offer valuable advice and support to the boy.");
				list.Add().Set("Grandfather as a mentor in a specific skill or hobby", "This grandfather may have a particular skill or hobby that he shares with his grandson, creating a special bond between them. He may teach the boy a new skill or passion, creating an opportunity for learning and bonding.");
				list.Add().Set("Grandfather as a source of comfort and stability", "This grandfather may serve as a stable and comforting presence in his grandson's life. He may offer support and a sense of security, especially during difficult times.");
				list.Add().Set("Long-distance grandfather-grandson relationship", "This grandfather may not live near his grandson, but they still have a strong and meaningful relationship. They may use technology to stay connected and make the most out of the time they spend together.");
				list.Add().Set("Grandfather figure for a grandson in need of male guidance", "This grandfather steps in to provide love and guidance for a grandson who may not have a positive male role model in his life. He may offer valuable life advice and show the boy unconditional love and support.");
				break;
			}
			case SOCIETYROLE_MATERNAL_UNCLE: {
				list.Add().Set("Fun and adventurous uncle", "This uncle brings excitement and adventure into his nephew's life. He may take him on trips and try new activities together, creating unforgettable memories.");
				list.Add().Set("Supportive and encouraging uncle", "This uncle is always there to support and cheer on his nephew's achievements. He may offer wise advice and be a source of inspiration for his nephew.");
				list.Add().Set("Mentor uncle", "This uncle takes on the role of a mentor for his nephew, providing guidance and teaching valuable life skills. He may also act as a positive role model, helping his nephew develop into a responsible and confident young man.");
				list.Add().Set("Protective and caring uncle", "This uncle is fiercely protective of his nephew and will do anything to ensure his safety and well-being. He may also offer emotional support and be a trusted confidant for his nephew.");
				list.Add().Set("Intellectual and knowledgeable uncle", "This uncle has a thirst for knowledge and shares his passion with his nephew. He may introduce him to new ideas and encourage a love for learning.");
				list.Add().Set("Uncle figure for a boy without a paternal figure", "This uncle becomes a paternal figure for his nephew, providing love, support, and guidance. He may step in as a positive male role model and have a strong influence on his nephew's life.");
				list.Add().Set("Sports-loving uncle", "This uncle shares his love for sports with his nephew and may coach or train him in his favorite sport. He may also attend games and events to support his nephew.");
				list.Add().Set("Creative and artistic uncle", "This uncle has a creative mind and encourages his nephew's imagination and artistic skills. He may also provide a safe space for his nephew to express himself through art forms.");
				list.Add().Set("Technological uncle", "This uncle is tech-savvy and shares his knowledge with his nephew. He may help him with school projects or introduce him to new and exciting technology.");
				list.Add().Set("Uncle figure for a boy with a difficult family situation", "This uncle is a source of stability and support for his nephew in a difficult family situation. He may provide a safe and loving home, and offer a positive role model for his nephew to look up to.");
				break;
			}
			case SOCIETYROLE_PATERNAL_UNCLE: {
				list.Add().Set("Mentor and role model", "This paternal uncle serves as a mentor and role model for his nephew, offering guidance and wisdom based on his own experiences. He may take an active interest in his nephew's life and help him navigate important decisions.");
				list.Add().Set("Supportive and fun-loving uncle", "This uncle is always there to lend a listening ear and offer emotional support to his nephew. He may also bring a sense of fun and playfulness to their relationship, making them both laugh and enjoy each other's company.");
				list.Add().Set("Father figure for a nephew without a father", "This uncle may step in to provide a fatherly presence and guidance for his nephew who does not have a biological father in his life. He may also take on the responsibility of being a positive male role model for his nephew.");
				list.Add().Set("Uncle figure for a boy from a different culture or ethnicity", "This paternal uncle may have a different cultural or ethnic background than his nephew, and may take the opportunity to share his traditions, customs, and unique cultural perspective with him.");
				list.Add().Set("Protective and nurturing uncle", "This uncle looks out for his nephew and always has his back. He may also play a nurturing role, providing emotional support and guidance when needed.");
				list.Add().Set("Uncle-nephew bonding through activities", "This uncle and nephew may bond over shared interests and hobbies, spending time together playing sports, reading, or engaging in other activities they both enjoy.");
				list.Add().Set("Tough love uncle", "This uncle may take a tough love approach, challenging his nephew to be his best self and holding him to high standards. He may also teach important life lessons and offer tough but necessary advice.");
				list.Add().Set("Uncle figure for a nephew with special needs", "This uncle may provide additional support and care for his nephew with special needs, playing an important role in his development and well-being.");
				list.Add().Set("Distant but supportive uncle", "This uncle may live far away from his nephew, but still maintains a loving and supportive relationship through regular communication and visits. He may provide a different perspective and offer a fresh outlook on life to his nephew.");
				list.Add().Set("Companion and friend", "This paternal uncle acts as a companion and friend to his nephew, offering a listening ear and sharing in his interests and hobbies. He may also provide a sense of security and comfort in times of need.");
				break;
			}
			case SOCIETYROLE_MATERNAL_AUNT: {
				list.Add().Set("Motherly aunt figure", "This aunt takes on a maternal role for her nephew, providing love, guidance, and care in the absence of his mother. She may also act as a role model, teaching him important life lessons and offering a different perspective.");
				list.Add().Set("Fun and adventurous aunt", "This aunt is always ready to have a good time with her nephew and encourages him to try new things and explore the world. She may also offer a highly prized sense of freedom and spontaneity in their relationship.");
				list.Add().Set("Nurturing and supportive aunt", "This aunt provides a nurturing and caring environment for her nephew, much like a mother figure. She may be a source of comfort and unwavering support for her nephew, especially during times of need.");
				list.Add().Set("Protective aunt-nephew relationship", "This aunt takes on a protective role for her nephew and is fiercely loyal to him. She may be a source of strength and security for her nephew, offering guidance and protection when needed.");
				list.Add().Set("Wise and insightful aunt", "This aunt is someone her nephew can turn to for advice and guidance. She may have a wealth of life experiences and knowledge to share, and often serves as a mentor and confidant for her nephew.");
				list.Add().Set("Creative and nurturing aunt", "This aunt may have a passion for the arts or creativity and shares this with her nephew. She may teach him new skills and encourage his own creativity, fostering a special bond through shared interests.");
				list.Add().Set("Mother figure for a boy from a different culture or ethnicity", "Similar to the mother figure, this aunt may share a different cultural or ethnic background than her nephew and embrace their differences. She may also serve as a cultural guide and help her nephew navigate his identity.");
				list.Add().Set("Supportive aunt during difficult times", "This aunt may step in to offer support and care for her nephew during difficult times such as a divorce, illness, or loss of a family member. She may become a source of stability and comfort for her nephew during these challenging times.");
				list.Add().Set("Successful career-oriented aunt", "This aunt serves as a role model for her nephew, especially in terms of career and ambition. She may offer guidance and advice on how to achieve success and inspire her nephew to chase his dreams.");
				list.Add().Set("Fun-loving and energetic aunt", "This aunt is always up for an adventure and loves spending time with her nephew. She may help him see the joy and excitement in everyday life and create lasting memories through their bond.");
				break;
			}
			case SOCIETYROLE_PATERNAL_AUNT: {
				list.Add().Set("Fun-loving aunt", "This aunt is playful and energetic, always finding ways to make her nephew laugh and have fun. She may also have a strong sense of adventure and take her nephew on exciting outings and trips.");
				list.Add().Set("Surrogate mother figure", "This aunt steps in to provide maternal care and support for her nephew, whether due to the absence of his biological mother or to supplement her role. She may offer emotional guidance and act as a strong female role model.");
				list.Add().Set("Protective aunt", "This aunt is fiercely protective of her nephew and will do anything to keep him safe and happy. She may also be a strong source of support and advice, and a confidant for her nephew.");
				list.Add().Set("Mentor and guide", "This aunt takes on the role of mentor and guide for her nephew, offering wisdom and guidance in different aspects of his life. She may share her own experiences and provide valuable life lessons.");
				list.Add().Set("Creative and artistic aunt", "This aunt is artistic and encourages her nephew's creativity and expression. She may introduce him to different forms of art, and provide a positive outlet for him to explore and develop his talents.");
				list.Add().Set("Financially supportive aunt", "This aunt may offer financial support to her nephew, whether through gifts, activities, or helping with school expenses. She may also teach him about money management and responsibility.");
				list.Add().Set("Long-distance aunt", "This aunt may not live close to her nephew, but she still maintains a special and close relationship through regular communication and visits. She may also be a source of advice and support for her nephew, despite the distance.");
				list.Add().Set("Aunt figure for a boy from a different cultural or ethnic background", "This aunt may come from a different cultural or ethnic background than her nephew, and she embraces and celebrates their differences. She may also share her unique cultural traditions and experiences with him, enriching his understanding of the world.");
				list.Add().Set("Nurturing and supportive aunt", "This aunt provides a nurturing and caring relationship for her nephew, offering unconditional love and support. She may also teach him valuable life skills and help him develop his self-confidence.");
				list.Add().Set("Aunt figure for a boy with special needs", "This aunt takes on the role of caregiver, advocate, and support system for her nephew with special needs. She may face unique challenges, but her love and dedication to her nephew never wavers.");
				break;
			}
			case SOCIETYROLE_MATERNAL_FEMALE_COUSIN: {
				list.Add().Set("Older cousin figures as a mother", "These female cousins may have a significant age gap with their male cousin, but have taken on a motherly role in his life. They provide guidance, support, and a sense of family for their younger cousin.");
				list.Add().Set("Close cousin figures", "These female cousins are close in age to their male cousin and have a strong bond with him. They may step in to help with parenting duties or offer advice and support in difficult times.");
				list.Add().Set("Protective cousin figures", "These female cousins may have a natural protective instinct towards their male cousin, making sure he is safe and taken care of. They may also be fiercely supportive and stand up for him in family situations.");
				list.Add().Set("Fun and adventurous cousin figures", "These female cousins may not have a traditional maternal role, but they bring excitement and fun into their male cousin's life. They take him on adventures, teach him new things, and provide a sense of joy and freedom.");
				list.Add().Set("Older sister-like cousin figures", "These female cousins may be closer in age to their male cousin's older siblings, but they still take on a motherly role in his life. They may offer guidance and support, and act as a role model for their younger cousin.");
				list.Add().Set("Adopted cousin figures", "These female cousins may have been adopted into the family, but they have a strong bond with their male cousin. They may see him as a younger brother and provide love and support just like a mother would.");
				list.Add().Set("Cousin figures as mothers to their own children", "These female cousins have children of their own and take on the role of mother to their male cousin as well. They may have a special bond as cousins and as parents, creating a strong family dynamic.");
				list.Add().Set("Cousin figures with similar cultural backgrounds as mothers", "These female cousins may share a similar cultural background with their male cousin, and may act as a motherly figure by passing down important cultural values and traditions.");
				list.Add().Set("Cousin figures who have overcome similar struggles as mothers", "These female cousins may have faced and overcome similar struggles as mothers, and can offer support and understanding to their male cousin's mother. They may also provide a nurturing and loving presence in the family.");
				list.Add().Set("Cousin figures who are also single mothers", "These female cousins may be single mothers themselves, and can offer support, guidance, and understanding to their male cousin's mother. They may also have a special bond with their cousin as they navigate the challenges of being a single parent.");
				break;
			}
			case SOCIETYROLE_MATERNAL_MALE_COUSIN: {
				list.Add().Set("Brotherly cousin", "This cousin is more like a brother figure than a cousin, and has been a constant presence in a person's life. He may have been raised together with the person, and they have a deep bond and shared memories.");
				list.Add().Set("Protective cousin", "This cousin may be slightly older and takes on a protective role over the person. He may offer guidance, support, and advice, and stands up for the person when needed.");
				list.Add().Set("Fun-loving cousin", "This cousin is always up for a good time and knows how to make the person laugh. They may share similar interests and enjoy spending time together, whether it's playing sports, going on adventures, or just hanging out.");
				list.Add().Set("Supportive cousin", "This cousin is always there to offer a listening ear and is a great source of emotional support. They may have a close relationship and trust each other with their deepest thoughts and feelings.");
				list.Add().Set("Distant cousin", "This cousin may not have a close relationship with the person, but they still maintain a cordial connection and see each other at family gatherings. They may not be as involved in each other's lives, but still care for each other as family.");
				list.Add().Set("Fatherly cousin", "This cousin may be significantly older and acts as a father figure to the person. He offers guidance, support, and advice, and helps shape the person's values and beliefs.");
				list.Add().Set("Role model cousin", "This cousin may be successful in their career or personal life and serves as a role model for the person. They inspire and motivate the person to strive for their own goals and dreams.");
				list.Add().Set("Mentor cousin", "This cousin takes on a mentorship role and offers guidance and advice to the person. They may have a specific area of expertise or knowledge that they share with the person, helping them grow and develop.");
				list.Add().Set("Absent cousin", "This cousin may not be actively involved in the person's life, and may only be seen or heard from occasionally. They may have their own reasons for being distant, but are still a part of the person's family.");
				list.Add().Set("Brother from another mother cousin", "This cousin is not a biological cousin, but through close family ties, they have become like siblings. They share a strong bond and may have grown up together as close friends.");
				break;
			}
			case SOCIETYROLE_PATERNAL_FEMALE_COUSIN: {
				list.Add().Set("Older cousin mentor", "This cousin is significantly older than the male and serves as a mentor and role model for him. She offers advice, guidance, and support, and may have a big influence on his life choices and decisions.");
				list.Add().Set("Protective cousin", "This cousin is close in age to the male and considers them more of a sibling than a cousin. She is fiercely protective of him and always has his back, whether it's standing up to bullies or giving him relationship advice.");
				list.Add().Set("Fun and adventurous cousin", "This cousin is always up for new experiences and loves to take the male on fun and exciting adventures. She encourages him to step out of his comfort zone and try new things, and they have a great time together.");
				list.Add().Set("Cousin with a similar background", "This cousin may have a similar upbringing, culture, or values as the male, and they share a strong bond because of it. She understands him in ways others might not and they have a special connection.");
				list.Add().Set("Distant cousin turned close friend", "This cousin might not have had a close relationship with the male growing up, but as they grew older, they found common interests and have become good friends. She is a source of comfort and support for the male.");
				list.Add().Set("Workaholic cousin", "This cousin is successful in her career and works hard to achieve her goals. She inspires the male to work hard and pursue his dreams, and supports him in his own aspirations.");
				list.Add().Set("Cousin with a big family", "This cousin comes from a big family and introduces the male to the chaos and joy that comes with it. She shares stories and creates new memories with the male, and helps him appreciate the importance of family.");
				list.Add().Set("Intellectual cousin", "This cousin is highly intelligent and intellectually curious. She challenges the male to think critically and helps him expand his knowledge and understanding of the world.");
				list.Add().Set("Cousin who is always there for emotional support", "This cousin is always there to lend an ear and offer emotional support to the male. She is a source of comfort and understanding during difficult times, and celebrates his successes with him too.");
				list.Add().Set("Mom-like cousin", "This cousin takes on a motherly role in the male's life, whether it's offering advice or taking care of him when he's sick. She loves and cares for him like a son, and their bond is like that of a mother and child.");
				break;
			}
			case SOCIETYROLE_PATERNAL_MALE_COUSIN: {
				list.Add().Set("Protective and supportive cousin", "This cousin is always looking out for their male relative, whether it be giving advice or standing up for them. They are a source of support and encouragement in all aspects of their cousin's life.");
				list.Add().Set("Fun and adventurous cousin", "This cousin is always down for a good time and loves to try new things with their male relative. They are the one to turn to for exciting and spontaneous outings and adventures.");
				list.Add().Set("Role model cousin", "This cousin serves as a positive influence in their male relative's life, inspiring them to be their best self and achieve their goals. They may be a successful professional or have other admirable qualities that their relative looks up to.");
				list.Add().Set("Competitor cousin", "This cousin is close in age and loves to compete with their male relative in everything from sports to video games. They may have a friendly rivalry, but ultimately they push each other to be better.");
				list.Add().Set("Big brother-like cousin", "This older cousin takes on a protective and mentoring role, acting as a big brother figure for their male relative. They may offer guidance and advice, and are always there to lend a listening ear.");
				list.Add().Set("Jokester cousin", "This cousin loves to make their male relative laugh and brings a sense of humor to every situation. They may not take things too seriously, but their lighthearted nature can be a source of comfort and entertainment for their relative.");
				list.Add().Set("Wise and experienced cousin", "This older cousin has a wealth of life experience and is always ready to share their wisdom and knowledge with their male relative. They serve as a source of guidance and support, especially during times of uncertainty.");
				list.Add().Set("Distance cousin", "This cousin may not live close by, but their bond with their male relative remains strong. They may not see each other often, but when they do, they pick up right where they left off and cherish their time together.");
				list.Add().Set("Cousin as a father figure", "This cousin fills the role of a father figure for their male relative, providing a stable and nurturing presence in their life. They may have a deep bond and a strong sense of responsibility towards each other.");
				list.Add().Set("Disconnected cousin", "This cousin may not have a close relationship with their male relative, due to distance, family issues, or other reasons. They may not play a significant role in their relative's life, but still share a family bond.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_IMAGE_SITE: {
				list.Add().Set("The perfect influencer", "This female constantly posts highly edited and curated images of herself that adhere to society's standards of beauty. Her flawless appearance and seemingly perfect life can create unrealistic expectations for the young male adult and negatively impact his self-esteem.");
				list.Add().Set("The fitness guru", "This female promotes a strict fitness and diet regime, often showcasing her toned body and healthy meals on her profile. While her dedication to health and wellness may be inspiring, it can also make the young male adult feel insecure about his own body and lifestyle.");
				list.Add().Set("The travel babe", "This female is constantly traveling to exotic locations and posting photos of her adventures on Instagram. While her life may seem glamorous and carefree, it can also make the young male adult feel envious and question his own life choices.");
				list.Add().Set("The girlfriend goals", "This female is often seen on her boyfriend's profile, taking cute and romantic photos together. Her relationship may seem perfect and unattainable, leading the young male adult to compare his own relationships and potentially feel inadequate.");
				list.Add().Set("The beauty guru", "This female shares makeup tutorials, beauty tips, and flawless selfies on her profile. While her skills and products can be impressive, it can also create unrealistic expectations for the young male adult in terms of male grooming and appearance.");
				list.Add().Set("The body positivity advocate", "This female promotes body positivity and inclusivity in her posts and encourages others to love their bodies. However, her pictures may still adhere to traditional beauty standards, potentially causing the young male adult to compare himself to unrealistic standards.");
				list.Add().Set("The confidence queen", "This female exudes confidence and self-love on her profile, posting empowering quotes and photos of herself embracing her flaws. While her message is positive, it can also make the young male adult feel inadequate or pressure him to have the same level of confidence.");
				list.Add().Set("The fashionista", "This female shares trendy outfits, designer items, and aspirational lifestyle on her profile. While her style may be enviable, it can also create a constant feeling of needing to keep up with the latest trends and fashion, leading to insecurities about one's own appearance.");
				list.Add().Set("The girl boss", "This female is a successful entrepreneur and constantly posts about her business ventures, achievements, and luxurious lifestyle. While her success may be inspiring, it can also make the young male adult feel inadequate or unsuccessful in comparison.");
				list.Add().Set("The friendship goals", "This female shares photos of her tight-knit group of friends, often doing fun and exciting activities together. While their friendships may seem perfect, it can also make the young male adult feel lonelier or like he is missing out.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_IMAGE_SITE: {
				list.Add().Set("The perfect influencer", "This male presents himself as having the perfect body through carefully curated photos and flawless poses. His posts focus on his toned physique and expensive clothing, creating an unattainable aesthetic for others to aspire to.");
				list.Add().Set("The fitness fanatic", "This male is constantly at the gym and posts about his intense workouts and strict diet. He promotes the idea that achieving a certain body type is the key to happiness and success.");
				list.Add().Set("The travel adventurer", "This male portrays an adventurous and carefree lifestyle, often shirtless and in exotic locations. He may perpetuate the idea that only those with a fit and attractive body can have fun and live life to the fullest.");
				list.Add().Set("The partygoer", "This male is always seen at parties and clubs, surrounded by attractive people and alcohol. He may promote the idea that a party lifestyle is desirable and necessary for social acceptance.");
				list.Add().Set("The fashionable male", "This male presents himself as always well-dressed and trendy, often promoting expensive designer clothing. He may create an image that equates material possessions with worth and success.");
				list.Add().Set("The bro", "This male conforms to stereotypical hyper-masculine traits, often posting about working out, sports, and partying. He may perpetuate toxic masculinity and create unrealistic expectations for young males to fit into a certain mold.");
				list.Add().Set("The model", "This male is constantly posing for professional photos, promoting different brands and products. He may create an unattainable standard of physical perfection and pressure to constantly compare oneself to others.");
				list.Add().Set("The foodie", "This male posts about extravagant and indulgent meals, often highlighting unhealthy and high-calorie foods. He may promote an unhealthy relationship with food and discourage healthy eating habits.");
				list.Add().Set("The 'just woke up like this' male", "This male presents himself as effortlessly good-looking and desirable, without having to put in any effort. He may create unrealistic expectations for young males to constantly be attractive and desirable without any insecurities or flaws.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_REEL_VIDEO_SITE: {
				list.Add().Set("The social influencer", "This female on the platform has a large following and is known for her trendy content and aesthetic. She may inspire the young male to emulate her lifestyle and behavior, possibly promoting materialistic values and superficial standards.");
				list.Add().Set("The body-positive activist", "This female promotes body positivity and self-love on the platform, serving as a positive role model for body image and self-acceptance. She may encourage the young male to embrace and appreciate his body, regardless of societal standards.");
				list.Add().Set("The comedic relief", "This female's content revolves around comedic skits and relatable humor, providing the young male with a source of lighthearted entertainment. She may also promote the value of self-care and taking breaks from the pressures of social media.");
				list.Add().Set("The fashion guru", "This female is known for her impeccable style and fashion sense, and may inspire the young male to be more conscious of his appearance and follow trends. She may also promote consumerism and unrealistic beauty standards.");
				list.Add().Set("The fitness influencer", "This female showcases her workout routines and healthy eating habits on the platform, promoting the importance of physical fitness. She may encourage the young male to prioritize his physical health, but may also reinforce the pressure to have a certain body type.");
				list.Add().Set("The political activist", "This female uses the platform to advocate for social and political issues, encouraging the young male to stay informed and use his voice for change. She may also promote critical thinking and active citizenship.");
				list.Add().Set("The DIY queen", "This female shares creative and budget-friendly DIY projects on the platform, inspiring the young male to tap into his creativity and try new things. She may also promote the value of minimizing waste and living sustainably.");
				list.Add().Set("The mental health advocate", "This female uses her platform to spread awareness and reduce the stigma surrounding mental health issues. She may encourage the young male to prioritize his mental well-being and seek help if needed.");
				list.Add().Set("The entrepreneur", "This female shares her journey as a successful business owner on the platform, motivating the young male to pursue his own entrepreneurial dreams. She may also promote the hustle culture and place a strong emphasis on success and financial gain.");
				list.Add().Set("The travel influencer", "This female documents her adventures and promotes the idea of a lavish and indulgent lifestyle, potentially influencing the young male to prioritize traveling and seeking extravagant experiences.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_REEL_VIDEO_SITE: {
				list.Add().Set("Bodybuilder/buff guy", "This male presents a physically fit and muscular appearance on social media, often posting workout routines and progress photos. This can potentially create unrealistic expectations for young male viewers and impact their self-esteem if they do not conform to these standards.");
				list.Add().Set("'Influencer' lifestyle", "This male showcases a lavish and materialistic lifestyle on social media, often with a large following and sponsored posts. This can lead to feelings of inadequacy and comparison for young male viewers, as they may feel pressure to live up to these idealized standards.");
				list.Add().Set("Comedian/Joker", "This male uses humor and entertaining content to gain followers and likes on social media. While it may be entertaining, constantly presenting a facade of being 'funny' or comedic can create pressure for young male viewers to be constantly entertaining and may impact their self-image if they do not feel they meet this standard.");
				list.Add().Set("Traveler/Adventurer", "This male portrays a lifestyle of constant adventure and world travel on social media, creating a sense of FOMO (fear of missing out) for young male viewers. This can lead to feelings of dissatisfaction with one's own life and create unrealistic expectations for what a fulfilling life should look like.");
				list.Add().Set("Fashion/Beauty Guru", "This male presents themselves as an expert on fashion and beauty, often showcasing a perfect and stylish appearance on social media. This can create unrealistic and unattainable standards for young male viewers, leading to feelings of inadequacy if they do not meet these beauty standards.");
				list.Add().Set("Gamer/Streamers", "This male streams or creates content about gaming, often with a focus on competitive gameplay. This can create pressure for young male viewers to constantly excel and perform in gaming to gain recognition and followers, which can impact their self-esteem if they do not meet these expectations.");
				list.Add().Set("Foodie/Recipe Creator", "This male posts about their love for food and creates unique and visually pleasing recipes on social media. While it may be enjoyable to watch, it can also create pressure for young male viewers to have a certain body type or diet, leading to potential self-image issues and disordered eating habits.");
				list.Add().Set("DIY/Crafting Expert", "This male presents themselves as skilled in DIY projects and crafting, often showcasing their latest creations on social media. While this can be inspiring, it can also create pressure for young male viewers to constantly be creating and might affect their self-esteem if they do not have the same level of skill or creativity.");
				list.Add().Set("'Nice guy'/Beta male", "This male presents themselves as 'nice' and often promotes positivity and self-care on social media. While this can be refreshing, it can also create a false sense of perfection and put pressure on young male viewers to constantly project a positive and optimistic image.");
				list.Add().Set("'Party guy'/Socialite", "This male showcases a constant stream of parties, events, and social gatherings on social media, creating a facade of a constantly exciting and glamorous life. This can create feelings of comparison and FOMO for young male viewers, potentially impacting their self-esteem if they feel their own social life does not measure up.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE: {
				list.Add().Set("Cyberbully", "This female uses the anonymity and freedom of expression on public message websites to harass and bully others. Her negative comments and messages can have a severe impact on the mental health of the young male adult, causing anxiety, depression, and low self-esteem.");
				list.Add().Set("Influencer", "This female has a large following on public message websites and shares content about beauty, fashion, and lifestyle. Her seemingly perfect life and curated image can make the young male adult feel inadequate or insecure about himself, leading to comparison and negative thoughts about his own life.");
				list.Add().Set("Troll", "This female purposely posts inflammatory and offensive content on public message websites to provoke and offend others. Her actions can cause distress and anger in the young male adult, affecting his mental well-being.");
				list.Add().Set("Supportive figure", "This female uses public message websites to spread positivity and offer words of encouragement to others. Her messages can be a source of comfort for the young male adult and may have a positive impact on his mental health.");
				list.Add().Set("Disinformation spreader", "This female shares false or misleading information on public message websites, causing confusion and doubt among readers. Her posts can lead the young male adult to question his own beliefs and values, causing stress and anxiety.");
				list.Add().Set("Gatekeeper", "This female controls access to certain groups or communities on public message websites, excluding the young male adult from participating. This exclusion can lead to feelings of isolation and rejection, affecting his mental health.");
				list.Add().Set("Clout chaser", "This female uses public message websites solely for the purpose of gaining attention and validation from others. Her constant need for approval can make the young male adult feel insignificant and unimportant, causing feelings of worthlessness.");
				list.Add().Set("Authentic role model", "This female uses public message websites to share her struggles, flaws, and achievements, inspiring others to embrace their true selves. Her vulnerability and authenticity can have a positive impact on the young male adult, promoting self-acceptance and self-love.");
				list.Add().Set("Conflict instigator", "This female often engages in heated debates and arguments on public message websites, fueling negative emotions and hostility. The young male adult may feel overwhelmed and drained by these interactions, affecting his mental well-being.");
				list.Add().Set("Gatekeeper of mental health resources", "This female uses public message websites to share information and resources about mental health, promoting self-care and support for others. Her posts can be a source of guidance and comfort for the young male adult, advocating the importance of mental health awareness.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE: {
				list.Add().Set("Influence-seeking social media user", "This male is constantly seeking validation and attention on social media, using it to boost his self-esteem. However, this can lead to a negative impact on his personal growth and mental health if he becomes overly dependent on social media for his self-worth.");
				list.Add().Set("Hateful or aggressive commenter", "This male uses public message websites as a platform to spread hate or aggression towards others. He may justify this behavior as free speech, but it can have a damaging effect on his personal growth and relationships.");
				list.Add().Set("Oversharer", "This male shares every aspect of his life on public message websites, from mundane day-to-day activities to personal struggles. While this can create a sense of community and connection, it can also be detrimental to his mental health if he becomes too focused on presenting a perfect image or feels pressure to constantly share.");
				list.Add().Set("Trend follower", "This male is heavily influenced by the opinions and trends on public message websites, often adopting them as his own without critical thinking. This can hinder his personal growth and individuality, as well as potentially leading to harmful behavior.");
				list.Add().Set("Ethical and responsible social media user", "This male uses public message websites in a responsible and ethical manner, considering the impact of his words and actions on others. He may also use it as a tool for personal growth and self-expression, rather than for seeking validation or causing harm.");
				list.Add().Set("Self-promoter", "This male uses public message websites primarily to promote himself or his brand, often at the expense of others. This can create a competitive and self-centered mindset, which may hinder his personal growth and relationships.");
				list.Add().Set("Prolific user", "This male spends a significant amount of time on public message websites, possibly to the detriment of other aspects of his life. This can lead to a negative impact on his mental health and personal growth if he becomes overly reliant on social media for entertainment and validation.");
				list.Add().Set("Critical thinker", "This male approaches public message websites with a critical eye, questioning and analyzing the information and opinions presented. This can lead to personal growth and an open-mindedness, but may also lead to isolation or conflict with others who do not share his views.");
				list.Add().Set("Online activist", "This male uses public message websites as a platform for activism and to raise awareness about issues he is passionate about. While this can be a positive use of social media, the constant barrage of negative news and opinions can also take a toll on his mental health if he does not practice self-care and balance.");
				list.Add().Set("Supportive community member", "This male uses public message websites to connect with and support others, creating a positive and uplifting community. This can have a positive impact on his personal growth and mental health, as well as the well-being of others.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_VIDEO_STREAMING_SITE: {
				list.Add().Set("Superficial streamer", "This female streamer is solely focused on her appearance and uses her physical appearance to gain views and followers. This can contribute to unrealistic body standards and negative self-image for viewers.");
				list.Add().Set("Toxic streamer", "This female streamer displays toxic and negative behavior towards herself and others. This can potentially lead to viewers experiencing feelings of self-doubt and inadequacy.");
				list.Add().Set("Authentic and empowering streamer", "This female streamer promotes body positivity, self-love, and self-acceptance. She uses her platform to inspire and uplift her viewers, promoting a healthy and positive mindset.");
				list.Add().Set("Intellectual streamer", "This female streamer brings intelligence and academic pursuits to her streams, showcasing a well-rounded and impressive personality. This can have a positive impact on viewers, encouraging them to pursue education and intellectual growth.");
				list.Add().Set("Mental health advocate streamer", "This female streamer uses her platform to speak openly about mental health and promote self-care. She may also create a supportive community for viewers struggling with their mental health.");
				list.Add().Set("Competitive gamer streamer", "This female streamer is highly skilled in competitive gaming and uses her platform to showcase her skills and accomplishments. While this can inspire viewers, it can also create an unhealthy comparison and pressure to constantly perform.");
				list.Add().Set("Diverse representation streamer", "This female streamer promotes diversity and representation in the gaming community. She may highlight and support marginalized groups, providing a sense of inclusivity for viewers.");
				list.Add().Set("Provocative streamer", "This female streamer may use provocative or sexualized content to gain attention and followers. This can contribute to unhealthy views on sexuality and objectification of women.");
				list.Add().Set("Family-friendly streamer", "This female streamer creates family-friendly content and promotes a positive and wholesome environment. This can be a refreshing break from more mature content on the platform and can have a positive impact on mental health.");
				list.Add().Set("Collaborative and inclusive streamer", "This female streamer actively collaborates with other streamers and embraces a sense of inclusivity in her community. This can promote a sense of belonging and social connection for viewers.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_VIDEO_STREAMING_SITE: {
				list.Add().Set("The Influencer", "This individual has a large following and their success on the platform is their main source of income. They often promote sponsorships and have a lavish lifestyle, which can lead to feelings of inadequacy for young male viewers who compare themselves to this version of success.");
				list.Add().Set("The Gamer", "This is someone who streams their gaming content and may also participate in gaming competitions. While they may have a charismatic on-screen personality, they can also perpetuate certain stereotypes and toxic masculinity within gaming communities.");
				list.Add().Set("The Streamer", "This person may share a variety of content, from gaming to music to vlogs. They often have a close relationship with their audience and may use their platform to discuss personal experiences or issues, providing a more authentic and relatable role model for young male viewers.");
				list.Add().Set("The Comedian", "This individual is known for their humorous and often self-deprecating content. While they may provide entertainment, their jokes and banter can also reinforce negative body image or harmful beliefs about masculinity.");
				list.Add().Set("The Bodybuilder", "This person showcases their intense workouts and body progress on their channel, promoting a hyper-masculine and often unattainable image. This can create pressure for young men to conform to a certain physical ideal.");
				list.Add().Set("The Coach", "This individual uses their channel to offer coaching or advice for self-improvement and success. While their intentions may be positive, their message can also perpetuate harmful beliefs about toxic masculinity and the need to constantly strive for success.");
				list.Add().Set("The Live Streamer", "This person shares their daily activities and interactions with their audience, often creating a sense of intimacy and closeness. However, the curated nature of their content can also promote a false sense of reality and create unrealistic expectations for young male viewers.");
				list.Add().Set("The Creative", "This individual shares their talent in areas such as art, music, or cooking on their channel. While they may provide a positive and unique outlet for self-expression, their success can also create feelings of comparison and self-doubt for young male viewers.");
				list.Add().Set("The Cosplayer", "This is someone who creates and showcases elaborate costumes and makeup based on their favorite fictional characters. While they may be celebrated for their creativity and skill, they can also perpetuate unrealistic and gendered expectations for young male fans in terms of appearance and interests.");
				list.Add().Set("The Social Activist", "This individual uses their platform to raise awareness and advocate for important social issues. While they may inspire young male viewers to take action, their comments and beliefs about gender and masculinity can also have a strong impact on the development of their audience's identity.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE: {
				list.Add().Set("Social media influencer", "This female is popular on social media, with a large following and a highly curated online presence. She may project an idealized image of herself, which can impact a young male's expectations of a romantic partner.");
				list.Add().Set("Career-focused professional", "This female prioritizes her career and may have a busy and demanding schedule. This can impact a young male's ability to spend time with her and may lead to conflicts in a romantic relationship.");
				list.Add().Set("Travel enthusiast", "This female loves to travel and may have a constantly changing lifestyle. A young male in a romantic relationship with her may need to be adaptable and comfortable with long-distance communication.");
				list.Add().Set("Creative and artistic", "This female is talented in a creative field, such as art, music, or writing. She may have a different perspective on life and relationships, which can be both intriguing and challenging for a young male.");
				list.Add().Set("Social butterfly", "This female enjoys being the life of the party and may have a large circle of friends. This can be intimidating for a young male, who may struggle to keep up with her social life and feel pressure to fit in with her friends.");
				list.Add().Set("Spiritual or philosophical", "This female is deeply interested in spiritual or philosophical concepts and may have a strong sense of purpose and beliefs. This can influence a young male's own beliefs and can create a dynamic of learning and growth in a romantic relationship.");
				list.Add().Set("Feminist activist", "This female is passionate about gender equality and may be vocal about her views. This can be intimidating for a young male who may feel pressure to conform to her ideals and may struggle to understand her perspective.");
				list.Add().Set("Family-oriented", "This female prioritizes her family above all else and may have a close-knit relationship with them. This can be both reassuring and challenging for a young male, as it may impact the level of involvement and acceptance from her family in the relationship.");
				list.Add().Set("Athletic and outdoorsy", "This female loves to stay active and may enjoy outdoor activities such as hiking, camping, or sports. A young male in a romantic relationship with her may need to share similar interests and hobbies to maintain a strong connection.");
				list.Add().Set("Bookworm or intellectual", "This female is knowledgeable and well-read, and enjoys intellectual conversations and debates. A young male in a romantic relationship with her may be challenged to keep up with her intelligence and may feel insecure if he doesn't share the same level of knowledge.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE: {
				list.Add().Set("Social butterfly", "This male is highly active on social media and has a large network of friends. He is constantly attending parties and events, and his social media profiles are filled with group photos and updates about his social life.");
				list.Add().Set("Influencer", "This male uses social media to build his personal brand and cultivate a large following. He may partner with brands and promote products on his profiles, and his popularity gives him access to exclusive events and opportunities.");
				list.Add().Set("Networking guru", "This male leverages social media to expand his professional network and make valuable connections. He may share industry-related content and engage in discussions with other professionals in his field.");
				list.Add().Set("Introvert", "This male may have a social media presence, but he primarily uses it to keep in touch with close friends and family. He may prefer smaller gatherings and may not be as active on social media as others.");
				list.Add().Set("Travel enthusiast", "This male's social media profiles are filled with photos from his travels, showcasing his adventurous spirit and love for new experiences. He may use social media to connect with other travelers and plan future trips.");
				list.Add().Set("Sports fanatic", "This male is passionate about sports and will use social media to share updates and opinions on games, players, and teams. He may also connect with others who share his love for a particular sport.");
				list.Add().Set("Gamer", "This male is heavily involved in the gaming community and uses social media to connect with other gamers, share tips and strategies, and discuss the latest releases. He may also live stream his gameplay for others to watch.");
				list.Add().Set("Music lover", "This male's social media profiles are filled with updates on his favorite bands and artists, as well as concert and festival photos. He may connect with other music lovers and discover new artists through social media.");
				list.Add().Set("Activist", "This male uses social media to raise awareness about social and political issues that he is passionate about. He may organize and participate in protests or events, and use his platform to promote causes he believes in.");
				list.Add().Set("Family-oriented", "This male's social media is a reflection of his love for his family. He may share photos and updates about his family members, and regular family gatherings and vacations are a common feature on his profiles.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_VIDEO_SITE: {
				list.Add().Set("Body positive influencer", "This commentator promotes body positivity and self-love, encouraging young men to embrace their bodies and ignore societal beauty standards. She may offer tips and advice on self-care and self-acceptance.");
				list.Add().Set("Fashion and style expert", "This commentator shares her expertise in fashion and style, providing useful advice for young men to help boost their confidence and self-image. She may also discuss the impact of dressing well on one's self-esteem.");
				list.Add().Set("Self-development coach", "This commentator focuses on personal growth and development, offering tips and techniques for young men to improve their self-image and self-esteem. She may also discuss topics such as self-confidence and self-worth.");
				list.Add().Set("Mental health advocate", "This commentator raises awareness about mental health and shares tips and resources for young men to improve their mental well-being. She may also discuss the importance of self-care and positive self-talk for building self-esteem.");
				list.Add().Set("Social media influencer", "This commentator has a large following on social media and uses her platform to promote self-love and body positivity. She may also collaborate with other influencers to spread positive messages and challenge societal beauty standards.");
				list.Add().Set("Beauty and grooming expert", "This commentator shares tips and tutorials on grooming and hygiene for young men, emphasizing the importance of taking care of oneself for self-image and self-esteem. She may also address toxic masculinity and the pressure to conform to traditional male beauty standards.");
				list.Add().Set("Career and success coach", "This commentator provides advice and guidance for young men on achieving success and fulfilling their potential. She may also discuss the impact of a strong self-image and self-esteem on one's career path and achievements.");
				list.Add().Set("Relationship expert", "This commentator offers insights and advice on navigating romantic relationships, promoting healthy and positive self-image. She may also discuss the impact of toxic relationships on one's self-esteem and self-worth.");
				list.Add().Set("Confidence coach", "This commentator specializes in helping people build confidence and self-assurance, providing strategies and techniques for young men to overcome insecurities and boost their self-esteem. She may also share personal stories and experiences to inspire others.");
				list.Add().Set("Mindfulness and self-awareness advocate", "This commentator promotes the practice of mindfulness and self-awareness for young men to cultivate a positive self-image and self-esteem. She may also share meditation techniques and self-reflection exercises.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_MODERATE_VIDEO_SITE: {
				list.Add().Set("Self-help guru", "This male commentator offers helpful and positive advice for young men struggling with self-esteem and personal development. He may provide tips and techniques for building confidence and achieving goals.");
				list.Add().Set("Fitness and health expert", "This male commentator focuses on physical fitness and healthy lifestyle choices for young men. He may offer workout routines, nutrition tips, and motivation for viewers to improve their physical and mental well-being.");
				list.Add().Set("Relationship and dating coach", "This male commentator offers insights and advice on relationships and dating for young straight men. He may discuss topics such as communication, attraction, and building healthy partnerships.");
				list.Add().Set("Career mentor", "This male commentator provides guidance and inspiration for young men looking to establish a successful career. He may share his own experiences and offer practical tips for career advancement.");
				list.Add().Set("Fashion and grooming influencer", "This male commentator offers style and grooming tips for young men, helping them to enhance their appearance and boost their confidence. He may also promote self-care and self-expression through fashion.");
				list.Add().Set("Political commentator", "This male commentator shares his perspectives and opinions on current events and political issues. He may encourage young men to be informed and engaged citizens and to form their own opinions.");
				list.Add().Set("Comedy and entertainment vlogger", "This male commentator creates comedic content and entertaining videos, providing a source of lightheartedness and laughter for viewers. He may also use his platform to discuss relevant social issues in a humorous manner.");
				list.Add().Set("Finance and budgeting advisor", "This male commentator offers financial advice and tips for young men to manage their money effectively and achieve financial stability. He may also discuss topics such as investing and building credit.");
				list.Add().Set("Men's rights advocate", "This male commentator focuses on advocating for men's rights and raising awareness about issues that affect young men, such as mental health, toxic masculinity, and discrimination against men.");
				list.Add().Set("Documentary filmmaker", "This male commentator creates thought-provoking documentaries that explore various social and cultural issues relevant to young men. He may raise awareness and educate viewers on important topics through his films.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_PROFSSIONAL_INDUSTRY_SITE: {
				list.Add().Set("Established CEO/Executive", "This female professional has achieved a high level of success in her career and serves as an inspiration for other young professionals. She may offer valuable mentorship and networking opportunities for a young adult looking to advance in their career.");
				list.Add().Set("Experienced Industry Expert", "This female professional has a wealth of experience and knowledge in a specific industry. She may offer mentorship and networking opportunities to help a young adult learn more about the field and make valuable connections.");
				list.Add().Set("Entrepreneur/Founder", "This female professional has taken the risk of starting her own business and has found success in doing so. She may offer mentorship and networking opportunities for young adults who are interested in becoming entrepreneurs themselves.");
				list.Add().Set("Career Coach/Consultant", "This female professional offers career coaching and consulting services to help other professionals reach their full potential. She may provide valuable mentorship and networking opportunities for a young adult seeking guidance in their career.");
				list.Add().Set("Academic/Researcher", "This female professional holds a position in academia or conducts research in a specific field. She may offer valuable mentorship and networking opportunities for a young adult interested in pursuing a career in the same field.");
				list.Add().Set("Nonprofit Leader/Activist", "This female professional is dedicated to making a positive impact in the world through her work in a non-profit organization or as an activist. She may offer mentorship and networking opportunities for a young adult interested in making a difference in their career.");
				list.Add().Set("Diversity and Inclusion Specialist", "This female professional is dedicated to promoting diversity and inclusion in the workplace. She may offer valuable mentorship and networking opportunities for a young adult looking to build a more inclusive and diverse network.");
				list.Add().Set("Experienced Manager/Team Leader", "This female professional has strong leadership skills and experience managing teams. She may offer mentorship and networking opportunities for a young adult seeking guidance on how to become a successful leader in their career.");
				list.Add().Set("Marketing/PR Professional", "This female professional has expertise in marketing and public relations. She may offer mentorship and networking opportunities for a young adult interested in learning more about these fields and making valuable connections.");
				list.Add().Set("Professional in Male-Dominated Industry", "This female professional works in a traditionally male-dominated industry and has broken barriers to achieve success. She may offer valuable mentorship and networking opportunities for a young adult looking to do the same.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_PROFSSIONAL_INDUSTRY_SITE: {
				list.Add().Set("Successful businessman", "This male profile showcases a man who is highly accomplished in his career, with impressive credentials and accolades. He may exude confidence and ambition, portraying the idea of a successful and powerful male figure.");
				list.Add().Set("Athlete", "This profile features a man who excels in sports and physical fitness. He may emphasize strength and athleticism, and may also highlight his dedication and discipline in his training.");
				list.Add().Set("Tech entrepreneur", "This male profile showcases a man who is innovative and entrepreneurial, leading a successful tech company or startup. He may emphasize intelligence, creativity, and forward-thinking, portraying the idea of a modern and tech-savvy man.");
				list.Add().Set("Creative professional", "This profile features a man who is a talented artist, designer, or writer. He may highlight his creativity and artistic expression, and may also emphasize his unique point of view and unconventional approach.");
				list.Add().Set("Academic scholar", "This profile showcases a man who is deeply knowledgeable and accomplished in his field of academia. He may emphasize his intelligence, research, and publications, portraying the image of an esteemed and highly educated man.");
				list.Add().Set("Finance executive", "This male profile features a man in a high-level position in the finance industry, such as an investment banker or financial manager. He may highlight his business acumen, financial success, and strategic thinking.");
				list.Add().Set("Military veteran", "This profile showcases a man who has served in the military, highlighting his bravery, dedication, and service to his country. He may also emphasize his leadership skills and ability to work under pressure.");
				list.Add().Set("Humanitarian/NGO worker", "This male profile features a man who is dedicated to making a positive impact in the world through his work in NGOs or humanitarian organizations. He may emphasize empathy, compassion, and a desire to create positive change.");
				list.Add().Set("Politician", "This profile features a male in a political position, emphasizing his leadership abilities, strong convictions, and public service. He may also highlight his achievements and impact in his political career.");
				list.Add().Set("Influencer/Online personality", "This male profile showcases an individual who has gained a large following on social media or other online platforms, often through their appearance, lifestyle, or personality. They may emphasize their popularity and influence among younger generations.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_MUSIC_SITE: {
				list.Add().Set("Collaborative songwriting partner", "This female artist seeks out opportunities to collaborate with other musicians and often partners with male singers to create unique and innovative music. She values the exchange of ideas and learning from others.");
				list.Add().Set("Networking guru", "This female artist is a master at networking on social music websites, connecting with other musicians and creating a strong support system. She actively reaches out to male singers to collaborate and share their music.");
				list.Add().Set("Mentor figure", "This female artist is an experienced and established musician who uses her platform to mentor and guide young male singers, offering advice and support. She may also collaborate with them to help them gain exposure.");
				list.Add().Set("Up-and-coming artist", "This female artist is on the rise in the music industry and is actively seeking collaborations with male singers to expand her fan base and reach new audiences. She may also offer marketing and branding advice to her collaborators.");
				list.Add().Set("Music producer", "This female artist not only creates her own music, but also produces and collaborates with male singers to create professional and polished tracks. She may provide valuable insights and feedback during the production process.");
				list.Add().Set("Vocal coach", "This female artist specializes in vocals and uses her skills to help male singers improve their singing and performance. She collaborates with them on covers or original songs, offering guidance and support.");
				list.Add().Set("Social media influencer", "This female artist has a large following on social media and uses her platform to promote and collaborate with male singers. She may also offer marketing strategies and help her collaborators gain exposure.");
				list.Add().Set("Genre-bending artist", "This female artist is known for experimenting with different genres and is always looking for male singers to add a unique touch to her music. She values collaboration and believes in pushing boundaries.");
				list.Add().Set("Music video director", "This female artist not only creates music, but also directs and produces music videos. She may collaborate with male singers to create visually impactful and creative videos for their songs.");
				list.Add().Set("Supportive fan", "This female artist may not be a musician herself, but she is a passionate fan of music and actively supports and collaborates with male singers she admires. She may promote their music and attend their performances.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_MUSIC_SITE: {
				list.Add().Set("The Confident Performer", "This male artist is confident in his talent and showcases his skills without fear or hesitation. He may have a strong stage presence and charisma, and his music may often focus on themes of empowerment and confidence, especially in relationships with women.");
				list.Add().Set("The Romantic Balladeer", "This male singer is known for his smooth and tender love songs, often expressing vulnerability and deep emotions in his lyrics. He may be seen as a hopeless romantic, and his music may focus on the ups and downs of relationships with women.");
				list.Add().Set("The Edgy Rebel", "This male artist embraces a rebellious and non-conformist image, pushing boundaries and challenging societal norms. His music may often have a raw, aggressive edge to it, and his lyrics may explore unconventional and intense relationships with women.");
				list.Add().Set("The Emotional Storyteller", "This male singer creates music that tells raw and heartfelt stories about his personal experiences, including past relationships. His music may offer a vulnerable and intimate perspective on male-female relationships, often with a sense of nostalgia or longing.");
				list.Add().Set("The Playful Flirt", "This male artist exudes charm and charisma in his music, often incorporating playful and flirtatious lyrics. He may often sing about casual relationships and the excitement of new love, but also explore the complexities of male-female dynamics.");
				list.Add().Set("The Progressive Advocate", "This male singer uses his platform to shed light on social issues, including gender equality, and may incorporate those themes into his music. His lyrics may touch on the challenges of navigating modern-day male-female relationships and promoting healthy communication and mutual respect.");
				list.Add().Set("The R&B Sensation", "This male artist may draw inspiration from R&B and soul music, creating smooth and seductive tracks that explore sensual and intimate relationships with women. His music may have a strong, sultry vibe and showcase his vocal abilities.");
				list.Add().Set("The Collaborative Collaborator", "This male artist is known for his collaborations with female artists, creating powerful and dynamic duets that showcase the chemistry and dynamics between a man and a woman. His music may focus on the importance of communication and balance in relationships.");
				list.Add().Set("The Shy Introvert", "This male singer may have a softer and more introspective style, often exploring deeper emotions and internal struggles in his music. His lyrics may touch on his personal experiences with love and relationships, but from a more introverted and reflective perspective.");
				list.Add().Set("The Multi-talented Virtuoso", "This male artist is not limited to one genre or style, showcasing his versatility and talent in various musical genres and collaborations. His music may offer a diverse range of perspectives on male-female relationships, as he explores different themes and dynamics in each song.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE: {
				list.Add().Set("Experienced mentor", "This female is highly skilled and experienced in her field, and offers mentorship and guidance to young males looking to improve their skills or collaborate on projects. She may also provide opportunities for networking and professional development.");
				list.Add().Set("Peer collaborator", "This female is at a similar skill level as the young male, and is open to collaborating on projects and sharing knowledge. She may also offer a different perspective and fresh ideas.");
				list.Add().Set("Inclusive community leader", "This female actively promotes inclusivity and diversity within the programming community, and creates a safe and welcoming space for all members. She may also organize events and initiatives that promote mentorship and collaboration among young males and other members.");
				list.Add().Set("Inspirational role model", "This female serves as an inspiration to young males by breaking stereotypes and excelling in a male-dominated field. She may share her personal experiences and offer advice and support to those looking to follow a similar path.");
				list.Add().Set("Healthy competition", "This female engages in friendly competition with young males, pushing them to strive for excellence and improve their skills. She may also offer constructive feedback and support their growth.");
				list.Add().Set("Supportive team leader", "This female leads a team of programmers and provides a supportive and collaborative environment for young males to learn and grow. She may also offer mentorship and opportunities for professional development within the team.");
				list.Add().Set("Technical expert", "This female is highly knowledgeable and skilled in a particular area of programming, and may offer specialized mentorship and collaboration opportunities for young males interested in that field.");
				list.Add().Set("Community advocate", "This female actively advocates for the needs and interests of young males in the programming community, and works to create equal opportunities for them. She may also offer mentorship and resources to help them succeed.");
				list.Add().Set("Non-judgmental listener", "This female is approachable and non-judgmental, providing a safe space for young males to share their struggles and seek advice. She may also offer practical solutions and support their growth.");
				list.Add().Set("Cross-cultural collaborator", "This female comes from a different cultural background and may offer a unique perspective and approach to programming. She may also provide mentorship and collaboration opportunities that bridge the cultural gap and promote diversity.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE: {
				list.Add().Set("Coding mentor", "This male user is experienced in coding and uses the social programming website to mentor and teach young males interested in coding. He offers guidance and advice, and may even collaborate with them on projects.");
				list.Add().Set("Open-source contributor", "This male user actively contributes to open-source projects on the social programming website, and encourages young males to do the same. He may also offer tips on how to get started and improve coding skills.");
				list.Add().Set("Career advice", "This male user shares his experiences and insights on career development in the tech industry on the social programming website. He may also offer mentorship and networking opportunities to young males interested in pursuing a similar path.");
				list.Add().Set("Collaborative coding partner", "This male user is open to collaborating on coding projects with young males on the social programming website. He may offer his skills and expertise to help them improve and learn new techniques.");
				list.Add().Set("Role model", "This male user is a successful and respected figure in the programming community, and serves as a role model for young males on the social programming website. He may share his journey and offer inspiration and motivation to others.");
				list.Add().Set("Project manager", "This male user takes on the role of project manager on the social programming website, coordinating and guiding young males on coding projects. He may also offer feedback and guidance to help them improve their skills.");
				list.Add().Set("Technical support", "This male user actively offers technical support and troubleshooting assistance on the social programming website. He may be willing to help young males who are struggling with coding issues or bugs in their projects.");
				list.Add().Set("Code reviewer", "This male user takes on the responsibility of reviewing and providing feedback on code written by young males on the social programming website. He may offer constructive criticism and help them improve their coding skills.");
				list.Add().Set("Mentorship program organizer", "This male user is involved in organizing mentorship programs for young males on the social programming website. He may work with other programming professionals to create opportunities for skill development and career guidance.");
				list.Add().Set("Community leader", "This male user is a leader in the social programming community, and works to create a welcoming and inclusive environment for young males. He may organize events and discussions to promote learning and personal growth.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC: {
				list.Add().Set("Successful female music producer", "This woman has carved out a successful career in the male-dominated field of music production. She serves as a mentor and role model for young male artists, offering advice and support on navigating the industry, as well as technical tips and production tricks.");
				list.Add().Set("Female singer-songwriter", "This woman gained fame and success through her own songwriting and performing. As a mentor, she shares her experiences and offers guidance on songwriting, performance skills, and the creative process.");
				list.Add().Set("Music industry executive", "This woman holds a high-level position in a record label or music management company. She has a deep understanding of the music business and offers valuable insights and advice to young male artists on building their careers.");
				list.Add().Set("Female music journalist", "This woman writes for a popular music website or magazine, and has a vast knowledge of the music industry. She may serve as a mentor by providing coverage and exposure for up-and-coming male artists, as well as offering feedback and constructive criticism.");
				list.Add().Set("Seasoned female musician", "This woman has been in the music industry for many years and has experienced the ups and downs of a music career. She offers guidance and wisdom to young male artists, sharing her own successes and failures and offering practical advice on how to navigate the music industry.");
				list.Add().Set("Female music teacher", "This woman teaches music, either privately or in a school setting. She takes on a mentor role by nurturing and developing the talents of young male artists, providing guidance on technique, creativity, and overall growth as musicians.");
				list.Add().Set("DIY musician", "This woman has found success on her own terms, without the backing of a major label or industry support. She may use her platform to mentor and empower young male artists to pave their own paths and find their unique voices in the music industry.");
				list.Add().Set("Female music therapist", "This woman uses music as a form of therapy for individuals with physical, emotional, or mental disabilities. She may offer mentorship to young male artists by highlighting the healing and transformative power of music and encouraging them to use their talents for good.");
				list.Add().Set("Social media influencer", "This woman has a large following on social media platforms and uses her influence to support and promote young male artists. She may offer mentorship by providing exposure and networking opportunities, as well as sharing valuable insights and advice on building a successful online presence.");
				list.Add().Set("Veteran female artist", "This woman has had a long and successful career in the music industry and has witnessed its evolution over the years. She may serve as a mentor by sharing her experiences and offering guidance on adapting to the ever-changing landscape of the music industry.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC: {
				list.Add().Set("Collaborative male musicians", "These males actively seek out other male musicians to collaborate with, whether it be for songwriting, producing, or performance. They may value the creative energy and support of other male artists.");
				list.Add().Set("Critique-seeking male artists", "These males use music forums as a platform to share their work and receive constructive criticism from other male artists. They may have a desire to constantly improve and perfect their craft.");
				list.Add().Set("Male mentor figures", "These males offer guidance and advice to younger or less experienced male artists on the forum. They may have a wealth of knowledge and experience in the music industry and are willing to share it with others.");
				list.Add().Set("Male fans of male artists", "These males use music forums to connect with other fans of male artists and discuss their music. They may be passionate and knowledgeable about their favorite artists and their music.");
				list.Add().Set("Male competition and rivalry", "In some cases, male artists on music forums may view each other as competitors and engage in friendly competition or unconstructive criticism. They may have a desire to outdo or be recognized as better than others.");
				list.Add().Set("Strong male friendships", "Some male artists on music forums may form strong friendships with each other, sharing a similar passion for music and supporting each other's growth and success.");
				list.Add().Set("Collaborative production teams", "These males come together on music forums to form production teams, working together to create and sell music. They may value the skills and creativity each member brings to the team.");
				list.Add().Set("Male musicians advocating for mental health", "These males use music forums to bring awareness to mental health issues among male musicians and offer support and resources for those struggling.");
				list.Add().Set("Male musicians addressing toxic masculinity", "These males use music forums to address and challenge toxic masculinity in the music industry, promoting a more supportive and inclusive environment for all genders.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_PROGRAMMING: {
				list.Add().Set("Experienced female programmer", "This woman has been in the programming industry for many years and is highly skilled and knowledgeable in various languages and technologies. She may serve as a mentor or role model to other women in the field.");
				list.Add().Set("Active contributor", "This female forum member is actively engaged in discussions and provides valuable insights and advice. She may have a strong presence in the community and is well-respected by other members.");
				list.Add().Set("Advocate for diversity and inclusion", "This woman is a vocal advocate for diversity and inclusion in the tech industry. She may use the forum as a platform to raise awareness about the challenges faced by women and marginalized groups in programming.");
				list.Add().Set("Entry-level female programmer", "This woman is just starting her career in programming and may seek guidance and support from more experienced members. She may also share her own experiences in navigating the industry as a woman.");
				list.Add().Set("Leader in a male-dominated industry", "This woman holds a leadership position in a traditionally male-dominated company or industry. She may offer insights on breaking gender barriers and advocating for women's advancement in tech.");
				list.Add().Set("Self-taught programmer", "This woman is self-taught and may have faced unique challenges in learning and entering the programming industry. She may share her journey with other forum members and offer advice to those in similar situations.");
				list.Add().Set("Entrepreneurial programmer", "This woman runs her own tech company or has started her own programming projects. She may offer a different perspective on the industry and share her experiences in running a business as a woman.");
				list.Add().Set("Experienced in a specific language or technology", "This woman is highly skilled and experienced in a specific language or technology, and offers expertise and advice to other members in that area. She may also be a valuable resource for networking and career opportunities.");
				list.Add().Set("Researcher or academic in computer science", "This woman has expertise and knowledge from the academic side of computer science. She may share insights from her research and offer a different perspective on the industry.");
				list.Add().Set("Woman in a non-technical role in the tech industry", "This woman may not be a programmer, but holds a non-technical role in the tech industry such as project management, marketing, or sales. She may share insights on how women can contribute and succeed in different roles within the field.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_FORUM_PROGRAMMING: {
				list.Add().Set("The seasoned veteran", "This male programmer has been in the industry for many years and serves as a mentor and role model for others. He has a vast knowledge and experience in various programming languages and is highly respected in the community.");
				list.Add().Set("The rising star", "This male programmer is relatively new to the industry but is quickly making a name for himself. He has a strong passion for programming and is eager to learn and take on new challenges. He is often seeking guidance and advice from more experienced programmers.");
				list.Add().Set("The job hopper", "This male programmer is constantly switching jobs in search of higher pay or better opportunities. He may have a strong technical skill set, but his lack of commitment and loyalty may hinder his professional growth.");
				list.Add().Set("The perfectionist", "This male programmer strives for perfection in his work and is often highly critical of himself and others. He may be overly meticulous and have difficulty accepting feedback or collaborating with others.");
				list.Add().Set("The team leader", "This male programmer has strong leadership skills and is often in a position of authority. He is responsible for managing and delegating tasks to other team members, and encourages collaboration and open communication within the team.");
				list.Add().Set("The lone wolf", "This male programmer prefers to work alone and may struggle with teamwork and collaboration. He is highly independent and may have a difficult time adjusting to a team dynamic.");
				list.Add().Set("The social butterfly", "This male programmer is highly sociable and enjoys networking and engaging with others in the programming community. He values building relationships and may have a strong presence on social media platforms.");
				list.Add().Set("The freelancer", "This male programmer works independently and takes on various projects for different clients. He may have a flexible schedule and prioritize work-life balance, but may also struggle with stability and job security.");
				list.Add().Set("The innovator", "This male programmer is constantly pushing the boundaries and coming up with new and innovative ideas. He may have a strong entrepreneurial spirit and is not afraid to take risks to advance his career.");
				list.Add().Set("The burnout", "This male programmer has been working in the industry for many years and is starting to feel the effects of burnout. He may have lost his passion for programming and struggles to stay motivated and current in his skills.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_WEBSITE_READER: {
				list.Add().Set("Fan-turned-girlfriend", "This female may have initially discovered the male's music or programming through the internet and became a loyal fan. As she continued following his work, she developed feelings for him and may try to make a romantic connection with him through his personal website.");
				list.Add().Set("Professional colleague", "This female is also a programmer or musician and stumbled upon the male's website while researching or networking. She may see him as a potential business partner or mentor, and may reach out to collaborate or seek advice from him.");
				list.Add().Set("Potential mentor", "This female is a highly accomplished programmer or musician herself and came across the male's website while looking for emerging talent. She may offer guidance and career advice to the male, and a potential mentor-mentee relationship can develop.");
				list.Add().Set("Social media influencer", "This female is an influencer with a large following on social media who discovered the male's website and was impressed by his work. She may reach out to him for a collaboration, promotion, or sponsorship opportunity.");
				list.Add().Set("Ex-girlfriend", "This female may have dated the male in the past and found his personal website while reminiscing about their relationship. She may reach out to him out of nostalgia or to apologize for any past mistakes.");
				list.Add().Set("Curious admirer", "This female came across the male's website randomly and was intrigued by his work and personality displayed on the site. She may reach out to get to know him better and potentially start a romantic relationship.");
				list.Add().Set("Professional critic", "This female is a professional critic or reviewer for music or programming and stumbled upon the male's website for research or review purposes. She may offer constructive criticism or praise for his work.");
				list.Add().Set("Journalist or interviewer", "This female is a journalist or interviewer and was assigned to cover a story on the male. She may reach out to him for an interview or quote for an article.");
				list.Add().Set("Female programmer/musician seeking inspiration", "This female is a programmer or musician and found the male's website while looking for inspiration for her own work. She may reach out to him to discuss his creative process and potentially collaborate.");
				list.Add().Set("Potential groupie", "This female is attracted to the male's talents and status as a programmer or musician and may try to seduce him through his personal website. She may see him as a potential conquest or adventure.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_WEBSITE_READER: {
				list.Add().Set("Traditional masculine representation", "This male embodies traditional traits associated with masculinity, such as physical strength, emotional stoicism, and assertiveness. He may present himself as dominant and in control, and prioritize traditional gender roles.");
				list.Add().Set("Sensitive and expressive representation", "This male challenges traditional notions of masculinity by openly sharing his emotions and expressing vulnerability. He may reject toxic masculinity and promote a more inclusive and caring definition of masculinity.");
				list.Add().Set("Hypermasculine representation", "This male emphasizes exaggerated masculine traits, such as aggression, violence, and sexual prowess. He may perpetuate harmful stereotypes of masculinity and objectify women.");
				list.Add().Set("Non-binary or gender fluid representation", "This male may reject traditional binaries of gender and instead identify as non-binary or gender fluid. He may challenge societal expectations and norms surrounding masculinity and embrace a more fluid expression of gender.");
				list.Add().Set("LGBT+ representation", "This male is a part of the LGBT+ community and may represent a range of masculine identities, such as gay, bisexual, or transgender. He may challenge stereotypes and offer a diverse perspective on masculinity.");
				list.Add().Set("Intellectual or nerdy representation", "This male is highly intelligent and may prioritize his intellect over traditional notions of masculinity. He may have hobbies such as coding, reading, or gaming, and reject physical strength or aggression as markers of masculinity.");
				list.Add().Set("Anti-masculinity representation", "This male may reject traditional notions of masculinity altogether and instead advocate for a more gender-inclusive and egalitarian society. He may challenge toxic masculinity and actively work towards promoting positive change.");
				list.Add().Set("Feminist representation", "This male supports and champions feminist values, such as equality and dismantling patriarchy. He may actively work towards breaking down rigid gender roles and promoting a more diverse and inclusive expression of masculinity.");
				list.Add().Set("Alternative representation", "This male defies societal norms and expectations by embracing alternative lifestyles, fashion, and hobbies. He may reject traditional ideas of what it means to be masculine and instead embrace individuality and self-expression.");
				list.Add().Set("Vulnerable representation", "This male does not shy away from sharing his vulnerabilities and struggles, breaking the stereotype of men being emotionally stoic. He may advocate for mental health and encourage other men to open up about their emotions as well.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_MUSIC_COOPERATION_SITE: {
				list.Add().Set("Vocalist looking for a guitarist", "This female musician is seeking a male guitarist to collaborate with and make music together. She may have a specific style or genre in mind and is looking for someone with compatible skills.");
				list.Add().Set("Producer with a diverse skillset", "This female producer brings a unique perspective to the music industry and is looking to collaborate with a male artist to create innovative and experimental music. She may have a strong background in various genres and production techniques.");
				list.Add().Set("Singer/songwriter seeking a co-writer", "This female musician is looking for a male co-writer to help bring her ideas to life and create meaningful lyrics and melodies. She may have a distinct sound and is looking for someone who can complement her style.");
				list.Add().Set("Electronic music producer looking for a vocalist", "This female producer specializes in electronic music and is searching for a male vocalist to add vocals to her tracks. She may have a strong vision for her music and is looking for a collaborator who can bring it to life.");
				list.Add().Set("Drummer seeking a bandmate", "This female drummer is looking to form a band with a male artist who shares her passion for music. She may be open to different genres and is looking for a collaborative and dedicated partner to create music with.");
				list.Add().Set("Multi-instrumentalist looking for like-minded musicians", "This female musician is proficient in multiple instruments and is looking for a male artist who shares her love for versatility in music. She may be interested in exploring different genres and experimenting with various instruments.");
				list.Add().Set("Lyricist looking for a partner to write melodies", "This female musician has a talent for writing lyrics and is seeking a male collaborator to help her bring these lyrics to life with a melody. She may have a penchant for storytelling and is looking for someone who can complement her writing style.");
				list.Add().Set("Guitarist seeking a vocalist for acoustic music", "This female guitarist has a love for acoustic music and is looking for a male singer to collaborate with. She may have a specific sound or vibe in mind and is searching for someone who can harmonize with her guitar playing.");
				list.Add().Set("Musician seeking mentorship", "This female musician is looking for an experienced male artist to mentor her and help her develop her skills and career. She may be open to collaborating and learning from someone who has a strong presence in the music industry.");
				list.Add().Set("Pianist looking to collaborate on film scores", "This female pianist has a passion for composing music for films and is searching for a male artist with a similar interest. She may have experience in scoring films and is looking for a versatile collaborator who can bring her musical ideas to fruition.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_MUSIC_COOPERATION_SITE: {
				list.Add().Set("Songwriting partner", "This male artist is looking for a creative songwriter to collaborate with in order to create original music. He may be open to sharing ideas and working together to come up with new and unique material.");
				list.Add().Set("Vocalist collaboration", "This male artist is looking for a male vocalist to collaborate with for his music projects. He may have written music but needs a strong and talented vocalist to bring his songs to life.");
				list.Add().Set("Segment partner", "This male artist is looking for a partner to fill a specific role, such as a guitarist, bassist, or drummer, in order to form a complete band. He may be looking for someone with a similar style and musical taste to create a cohesive sound.");
				list.Add().Set("Collaborative producer", "This male artist is looking for a producer to collaborate with, either to help produce his own music or to combine their skills to create new and unique tracks. He may be open to working on both his and the producer's projects.");
				list.Add().Set("Creative mentor", "This male artist is looking for a more experienced collaborator to act as a mentor and provide guidance in their music career. He may be looking for someone to learn from and bounce ideas off of in order to improve his skills.");
				list.Add().Set("Versatile musician", "This male artist is looking for a versatile musician to collaborate with, someone who can play multiple instruments or has a wide range of musical knowledge. He may be interested in experimenting with different genres and sounds.");
				list.Add().Set("Collaborative composer", "This male artist is looking for a composer to work with in order to create original music. He may be open to co-writing or having the composer write pieces for his projects.");
				list.Add().Set("Genre-specific partner", "This male artist is looking for a partner who specializes in a specific genre, such as rock, hip-hop, or country. He may be looking to create music in that particular style and wants someone who is well-versed in it.");
				list.Add().Set("Cross-genre collaboration", "This male artist is looking to collaborate with someone who brings a different musical perspective and background. He may be interested in creating music that combines elements from multiple genres.");
				list.Add().Set("Collaborator for live performances", "This male artist is looking for someone to collaborate with for live performances, whether it's playing instruments, singing, or supporting with additional vocals. He may want to work closely with this collaborator to ensure a seamless and energetic live show.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE: {
				list.Add().Set("Experienced and collaborative mentor", "This female programmer is highly experienced and has a wealth of knowledge in c++, making her a valuable mentor for other programmers, including the senior male programmer. She is approachable and open to sharing her expertise and experiences, making the working relationship between them productive and collaborative.");
				list.Add().Set("Team-oriented project manager", "This female programmer takes on the role of project manager in a c++ collaboration website and is skilled in organizing and leading a team. She communicates effectively with all team members, including the senior male programmer, to ensure the project runs smoothly and efficiently.");
				list.Add().Set("Offers a fresh perspective", "Bringing a unique perspective to the table, this female programmer offers fresh ideas and approaches to problem-solving, challenging the senior male programmer to see things in a different light. This can lead to a dynamic working relationship and foster creativity.");
				list.Add().Set("Efficient and independent worker", "This female programmer is highly efficient and able to work on tasks independently, making the senior male programmer's job easier. She takes responsibility for her own work and ensures it meets the high standards set by the website.");
				list.Add().Set("Supportive and communicative team member", "This female programmer prioritizes clear and effective communication within the team, making sure everyone is on the same page and working towards the same goals. She is also supportive of her team members, including the senior male programmer, providing encouragement and assistance when needed.");
				list.Add().Set("Collaborative problem-solver", "When faced with challenges, this female programmer actively engages in problem-solving with the senior male programmer and other team members. She brings a positive and collaborative energy to finding solutions, making the working relationship between them effective and productive.");
				list.Add().Set("Detailed and meticulous tester", "This female programmer has an eye for detail and is excellent at testing code for bugs and errors. Her thorough approach helps the senior male programmer catch any issues before releasing the code, making their collaboration more efficient and successful.");
				list.Add().Set("Skilled and adaptable learner", "The c++ programming language is constantly evolving, and this female programmer stays on top of new developments and adapts quickly. Her willingness to learn and try new things makes her a valuable asset to the team and creates a positive working relationship with the senior male programmer.");
				list.Add().Set("Passionate and motivated contributor", "This female programmer has a deep passion for c++ programming and is highly motivated to contribute her skills and knowledge to the website. Her enthusiasm can inspire and motivate the senior male programmer, creating a collaborative and productive working relationship.");
				list.Add().Set("Resourceful problem-solver", "In a fast-paced and ever-changing environment, this female programmer quickly adapts and finds innovative solutions to problems. Her resourcefulness and ability to think outside the box can complement the senior male programmer's expertise, creating a strong working relationship.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE: {
				list.Add().Set("Mentor", "This senior c++ programmer takes on the role of a mentor for younger programmers on the collaboration website. They offer guidance and advice, and may even take on a mentee to help them improve their skills.");
				list.Add().Set("Experienced contributor", "This programmer has been using c++ for many years and is highly skilled in the language. They contribute regularly to projects and may offer their expertise to other users on the website.");
				list.Add().Set("Team player", "This programmer works well in a team and values collaboration and communication. They are open to feedback and suggestions from their team members and strive to create a positive working relationship.");
				list.Add().Set("Code reviewer", "This programmer takes on the responsibility of reviewing code submitted by others on the collaboration website. They provide constructive criticism and ensure that all code meets the necessary standards.");
				list.Add().Set("Project leader", "This programmer takes charge of managing a project on the collaboration website. They delegate tasks, set deadlines, and oversee the overall progress of the project while also contributing their skills.");
				list.Add().Set("Bug hunter", "This programmer has a talent for finding and fixing bugs in code. They may actively search for bugs in others' code or offer their services to help resolve bugs in a project.");
				list.Add().Set("Collaborative partner", "This programmer values working together and seeks out opportunities to collaborate with others. They may actively seek out projects to join or offer to help with existing projects.");
				list.Add().Set("Solo contributor", "This programmer prefers to work independently and may choose to work on solo projects or take on individual tasks in a collaborative project.");
				list.Add().Set("Tech enthusiast", "This programmer is passionate about c++ and loves learning about all its intricacies. They may contribute to the community by sharing their knowledge and participating in discussions and debates.");
				list.Add().Set("Multilingual programmer", "This programmer is skilled in multiple programming languages and may offer their expertise to others in different languages. They may also enjoy contributing to projects that involve various programming languages.");
				break;
			}
			case SOCIETYROLE_FEMALE_PEER_IN_MILITARY_SERVICE: {
				list.Add().Set("Comrades-in-arms", "These female peers are seen as equals in military service and have shared experiences and bonds through their training and missions. There is a sense of camaraderie and mutual respect, but a romantic relationship between them may not be pursued due to the military's strict policies against fraternization.");
				list.Add().Set("Friends turned lovers", "These female peers started off as friends, but as they work together, they develop strong feelings for each other. They may go through ups and downs in their relationship, but their shared background in the military strengthens their bond.");
				list.Add().Set("Mentor-mentee", "This female peer may hold a higher rank or have more experience in the military, and serves as a mentor to the male soldier. There may be a level of admiration or admiration towards this peer, but a romantic relationship may not be pursued due to power dynamics and military regulations.");
				list.Add().Set("The one that got away", "This female peer and the male soldier may have had a brief fling or mutual attraction, but due to their demanding military schedules or being stationed in different locations, their relationship never fully developed. They may still have feelings for each other, but their professional obligations take precedence.");
				list.Add().Set("Co-ed training partners", "These female peers train and work alongside the male soldier, competing and pushing each other to be the best. There may be a level of competitiveness and teasing between them, but also a mutual understanding and respect for each other's abilities.");
				list.Add().Set("Roommates", "These female peers and the male soldier may be assigned to live together in military housing. They learn to navigate each other's habits and routines, and develop a close bond from sharing living quarters. A romantic relationship may develop from this close proximity and constant interaction.");
				list.Add().Set("Cross-cultural relationships", "This female peer may come from a different cultural or ethnic background, and her experiences and perspectives may be different from the male soldier's. They may be drawn to each other's differences and learn from each other, strengthening their relationship.");
				list.Add().Set("Unrequited love", "This female peer may have feelings for the male soldier, but the feelings are not reciprocated. She may struggle with her emotions in a professional setting, but remains a supportive and valuable team member.");
				list.Add().Set("Former partners", "These female peers and the male soldier may have had a romantic relationship in the past, but have since broken up. Despite their history, they maintain a professional attitude and continue to work together effectively in the military.");
				break;
			}
			case SOCIETYROLE_MALE_PEER_IN_MILITARY_SERVICE: {
				list.Add().Set("The trusted comrade", "This peer is someone the straight male in military service can always rely on, whether it's for a mission or personal support. They have built a strong bond of trust and camaraderie through their shared experiences.");
				list.Add().Set("The rival-turned-friend", "Initially, this peer and the straight male may have been competitive or even had conflicts, but over time they developed a mutual respect for each other's abilities and grew a strong friendship.");
				list.Add().Set("The mentor", "This peer is more experienced in the military and serves as a mentor to the straight male. They offer guidance and advice, and often have a mutual respect for each other's skills and dedication to the service.");
				list.Add().Set("The jokester", "This peer brings humor and lightness to the straight male's military experience. They may tease and joke around with each other, but there is still a deep level of respect and friendship.");
				list.Add().Set("The battle buddy", "This peer is the straight male's partner during combat, and they have each other's backs no matter what. Their camaraderie is built on a strong foundation of trust and respect for one another.");
				list.Add().Set("The older brother figure", "This peer may be older in age or rank, but they act as a big brother to the straight male. They offer guidance, advice, and support, and the straight male looks up to them with admiration and respect.");
				list.Add().Set("The straight male in a leadership role", "In this scenario, the straight male is the peer in a leadership position. They earn their peers' respect through their strong leadership skills and ability to make tough decisions.");
				list.Add().Set("The straight male bonding with a fellow newcomer", "This peer may have joined the military around the same time as the straight male and they bond over their shared experience of adjusting to military life. They develop a strong camaraderie through mutual support and understanding.");
				list.Add().Set("The loyal friend", "This peer is always by the straight male's side, no matter what. They have developed a deep bond of trust and respect, and their friendship extends beyond their military service.");
				list.Add().Set("The diverse group of friends", "This peer group may consist of individuals from different backgrounds, cultures, or experiences, but they have a strong shared bond through their military service. They all respect and support each other, despite their differences.");
				break;
			}
			case SOCIETYROLE_FEMALE_SUPERIOR_IN_MILITARY_SERVICE: {
				list.Add().Set("Female commanding officer", "This superior officer holds a high rank in the military and leads a unit of soldiers, including the straight male. She is a strong and confident leader, with a no-nonsense attitude and a dedication to her duty. Through her leadership, she inspires and guides the male soldier, serving as a crucial mentor in his military career.");
				list.Add().Set("Drill sergeant", "This female officer is tough and demanding, responsible for training and pushing the straight male to his limits. She may have a fierce demeanor and use strict disciplinary methods, but also has a keen eye for potential and a determination to help her soldiers succeed.");
				list.Add().Set("Military mentor", "This female officer may not be in a direct leadership role, but still serves as an influential mentor for the straight male soldier. She may have a wealth of experience and knowledge to share, providing valuable guidance and advice for navigating the military world.");
				list.Add().Set("Team leader", "This female leader works closely with the straight male as part of a team in the military. She may have a collaborative and inclusive leadership style, fostering a supportive and cohesive team dynamic. As a mentor, she helps the male soldier develop his skills and encourages him to reach his full potential.");
				list.Add().Set("Superior officer with similar career path", "This female superior holds a rank higher than the straight male, but also shares a similar career path and background. She may have faced similar challenges and can offer relatable insights and support as a mentor for the male soldier.");
				list.Add().Set("Female officer with diverse background", "This superior officer brings a diverse perspective to her leadership and mentorship of the straight male. She may come from a different branch of the military, different cultural or ethnic background, or have unique experiences that broaden the male soldier's understanding of the military world.");
				list.Add().Set("Combat veteran", "This female officer has extensive experience in combat and has faced the rigors and dangers of war. She may hold a leadership position and mentor the straight male soldier in navigating difficult and challenging situations in the military.");
				list.Add().Set("Expert in a specialized field", "This female officer may hold a high rank in a specialized field, such as intelligence, engineering, or medical services. She offers valuable expertise and mentorship to the straight male soldier, helping him develop and succeed in his chosen career path.");
				list.Add().Set("Military parent", "This female officer may not be in a direct leadership position over the straight male, but is a respected and influential figure within the military community. As a parent figure, she offers guidance and support for the male soldier, helping him navigate not only his military career, but also life in the military overall.");
				list.Add().Set("Commanding General", "This female leader holds one of the highest ranks in the military, responsible for commanding a major division or branch. She sets the tone for the entire unit and serves as a mentor and role model for all soldiers, including the straight male.");
				break;
			}
			case SOCIETYROLE_MALE_SUPERIOR_IN_MILITARY_SERVICE: {
				list.Add().Set("NCO (Non-Commissioned Officer)", "A tough and seasoned sergeant who takes charge and leads by example. He instills discipline and respect in his unit, but also takes the time to mentor and guide his subordinates.");
				list.Add().Set("Drill Instructor", "This tough but fair leader is responsible for training and shaping new recruits into soldiers. He is known for his strict and intense training methods, but also for inspiring and creating a bond among his unit.");
				list.Add().Set("Commander", "This high-ranking officer oversees a military unit and makes strategic decisions. He is respected for his leadership skills and is seen as a role model by his subordinates.");
				list.Add().Set("Combat veteran", "This experienced soldier has been in multiple battles and has a deep understanding of the toll of war. He may act as a mentor and confidant for younger soldiers, offering guidance and perspective.");
				list.Add().Set("Chaplain", "This religious figure offers spiritual and emotional support to soldiers, regardless of their faith. He provides a listening ear and a source of comfort for those struggling with the challenges of military service.");
				list.Add().Set("Military doctor", "This physician cares for the physical and mental well-being of soldiers. He may also offer mentorship and guidance, using his experience to help soldiers navigate the unique stressors of military life.");
				list.Add().Set("Special operations leader", "This elite soldier is highly trained and skilled, often leading high-risk missions. He is respected for his bravery and expertise, and may mentor younger soldiers and pass on his knowledge and skills.");
				list.Add().Set("Military lawyer", "This legal officer serves in the military justice system and advises soldiers on legal matters. He may also play a mentorship role, guiding soldiers through the complexities of military law.");
				list.Add().Set("Former prisoner of war", "This resilient survivor has endured and overcome the horrors of captivity. He may serve as a symbol of hope and resilience for soldiers, and offer mentorship and support for those struggling with trauma.");
				list.Add().Set("Military historian", "This expert on military history may play a mentorship role by teaching soldiers about the past and how it relates to their current service. He may also provide a sense of camaraderie by sharing stories and lessons from past soldiers.");
				break;
			}
			case SOCIETYROLE_FEMALE_INFERIOR_IN_MILITARY_SERVICE: {
				list.Add().Set("Female subordinate in a male-dominated unit", "This woman serves in a military unit where the majority of her colleagues are males. She may face challenges in gaining respect and recognition for her skills and dedication due to her gender.");
				list.Add().Set("Female superior in a male-dominated unit", "This woman outranks her male colleagues in a military unit, and may face resistance or hostility from some of them. She may have to work harder to prove herself as a leader and gain the respect of her subordinates.");
				list.Add().Set("Female soldier in a predominantly female unit", "This woman serves in a unit where the majority of her colleagues are also females. She may feel a sense of camaraderie and sisterhood with her fellow female soldiers, but also may struggle with stereotypes and expectations placed on her as a woman in the military.");
				list.Add().Set("Female combat soldier", "This woman serves in combat alongside her male counterparts, often facing physically and emotionally demanding situations. She may have to constantly prove her strength and capability, and may also face discrimination or harassment from some male soldiers.");
				list.Add().Set("Female in a supportive role", "This woman serves in a non-combat role, such as a medic, mechanic, or administrator, supporting the efforts of her male colleagues. She may face challenges in being taken seriously or viewed as an equal by some male soldiers.");
				list.Add().Set("Female in a leadership role", "This woman holds a high-ranking position in the military, overseeing and making decisions for her unit. She may face pressure and scrutiny from her superiors as a woman in a traditionally male-dominated field.");
				list.Add().Set("Female in a traditionally masculine role", "This woman serves in a role that is typically associated with men, such as a pilot, sniper, or tank commander. She may face pushback and skepticism from her male colleagues, but also may find empowerment in breaking gender norms in the military.");
				list.Add().Set("Female in a same-sex relationship", "This woman serves openly in the military while also being in a same-sex relationship. She may face discrimination and challenges, but also may find support and acceptance within her unit.");
				list.Add().Set("Female facing sexual harassment/assault", "This woman experiences sexual harassment or assault from her male colleagues, causing her to fear for her safety and question her place in the military. She may also struggle with reporting the incident due to fear of retaliation or not being taken seriously.");
				list.Add().Set("Female dealing with military sexism", "This woman faces systemic sexism within the military, such as unequal pay, limited opportunities for advancement, and gender-based stereotypes. She may fight for change and equality within the military.");
				break;
			}
			case SOCIETYROLE_MALE_INFERIOR_IN_MILITARY_SERVICE: {
				list.Add().Set("The eager recruit", "This male has just joined the military and is enthusiastic and eager to prove himself. He may look up to his superior as a mentor and seek guidance and approval.");
				list.Add().Set("The experienced veteran", "This male has been in the military for many years and has a wealth of knowledge and experience. He may have a strong bond with his superior and provide guidance to younger soldiers.");
				list.Add().Set("The rebellious troublemaker", "This male may struggle with authority and often challenges his superiors. He may have a strained relationship with his superior and struggle to follow rules and regulations.");
				list.Add().Set("The loyal follower", "This male is fiercely loyal to his superior and will follow their orders without question. He may have a close relationship with his superior and see them as a role model.");
				list.Add().Set("The soldier with personal issues", "This male may be dealing with personal problems such as mental health issues or family struggles. He may turn to his superior for support and guidance in managing these issues.");
				list.Add().Set("The slacker", "This male may lack motivation and often falls behind in his duties. He may have a strained relationship with his superior, who may struggle to motivate and discipline him.");
				list.Add().Set("The aspiring leader", "This male may have ambitions of moving up in rank and is always looking to improve himself. He may seek mentorship and guidance from his superior to achieve his goals.");
				list.Add().Set("The best friend", "This male may have a close and personal relationship with his superior, beyond the military hierarchy. They may share personal stories and support each other through challenges.");
				list.Add().Set("The overachiever", "This male may strive to be the best in all aspects of military life and often exceeds expectations. He may have a competitive relationship with his superior, but also respects and admires them.");
				list.Add().Set("The injured soldier", "This male may have been injured in the line of duty and is still adjusting to life with a disability. He may look to his superior for understanding and support in this difficult transition.");
				break;
			}
			case SOCIETYROLE_WIFE: {
				list.Add().Set("Loving and supportive wife", "This wife is a partner in every sense of the word, providing unwavering love and support for her husband. She may prioritize their relationship and actively work to maintain a strong bond with her husband.");
				list.Add().Set("Working wife and mother", "This wife balances a successful career and being a mother to her children, while also being an equal and supportive partner to her husband. She may work together with her husband to create a harmonious and functional family dynamic.");
				list.Add().Set("Best friend and confidant wife", "This wife isn't just a partner, but also a best friend and confidant to her husband. They share a deep connection and often turn to each other for support and advice.");
				list.Add().Set("Equal partnership wife", "This wife sees her relationship with her husband as a partnership, with both parties having equal say and involvement in decision-making. They may divide responsibilities and actively work together to achieve common goals.");
				list.Add().Set("Stay-at-home wife and mother", "This wife takes on the role of primary caregiver and homemaker, while her husband focuses on providing for their family financially. She may prioritize creating a nurturing and comfortable home for her family.");
				list.Add().Set("Independent and assertive wife", "This wife is confident, independent, and assertive in her relationship with her husband. She may challenge him to be the best version of himself and actively work towards her own personal and professional goals.");
				list.Add().Set("Spiritual and supportive wife", "This wife may prioritize her spiritual beliefs and practices and supports her husband's spiritual growth as well. They may have a strong spiritual connection and prioritize their spiritual bond in their relationship.");
				list.Add().Set("Team players wife", "This wife and her husband function as a cohesive team, working together towards common goals and being each other's biggest cheerleader. They may support each other's personal growth and encourage each other to chase their dreams.");
				list.Add().Set("Healthy communication wife", "This wife prioritizes open and honest communication in her relationship with her husband. She may actively listen, express her feelings and opinions, and respects his thoughts and boundaries.");
				list.Add().Set("All-rounder wife", "This wife is able to balance various roles in her relationship: friend, partner, lover, caregiver, and more. She is able to adapt to different situations and be the support her husband needs in different aspects of their relationship.");
				break;
			}
			case SOCIETYROLE_BEST_MAN: {
				list.Add().Set("Loyal and supportive best man", "This best man is not only a close friend of the groom, but also a loyal and supportive confidant. He may go above and beyond to help the groom with wedding preparations and offer emotional support.");
				list.Add().Set("Long-time best friend", "This best man has been friends with the groom for a significant amount of time and has a deep understanding of their friendship. He may bring sentimental memories and inside jokes to their wedding day.");
				list.Add().Set("Brother as best man", "This best man is the groom's brother, adding a special familial dynamic to their relationship. He may be a constant source of support and guidance for the groom and play a significant role in their childhood memories.");
				list.Add().Set("Funny and entertaining best man", "This best man has a great sense of humor and knows how to lighten the mood. He may be responsible for bringing laughs and smiles at the wedding, making everyone feel at ease.");
				list.Add().Set("Responsible and organized best man", "This best man takes his role seriously and ensures that everything runs smoothly on the wedding day. He may be the go-to person for any issues and is always prepared to handle unexpected situations.");
				list.Add().Set("Emotional and sentimental best man", "This best man may have a more sentimental and emotional bond with the groom, and may express his love and appreciation for their friendship in a heartfelt speech.");
				list.Add().Set("Adventurous and spontaneous best man", "This best man brings an element of fun and adventure to the wedding celebrations. He may plan special surprises or activities for the groom, creating unforgettable memories.");
				list.Add().Set("Knowledgeable and experienced best man", "This best man may have been married before or has a lot of experience attending weddings, and is able to offer valuable advice and support to the groom throughout the wedding planning process.");
				list.Add().Set("Energetic and enthusiastic best man", "This best man brings a contagious energy and enthusiasm to the wedding celebrations. He may be the life of the party and ensure that everyone is having a good time.");
				list.Add().Set("Best man who knows how to keep secrets", "This best man is a trustworthy and reliable friend who knows how to keep the groom's secrets safe. He may be the one the groom turns to for confidential advice and support.");
				break;
			}
			case SOCIETYROLE_DAUGHTER: {
				list.Add().Set("Daddy's girl", "This daughter has a close and loving relationship with her father, often seeking his approval and guidance. She may have a special bond with her father and cherishes their time spent together.");
				list.Add().Set("Estranged daughter", "This daughter may have a strained or distant relationship with her father, due to past conflicts or misunderstandings. She may long for a deeper connection with her father and may work towards rebuilding their relationship.");
				list.Add().Set("Daughter as father's caregiver", "This daughter takes on the role of caregiver for her father, whether due to physical or mental health struggles or simply as he ages. She may balance the roles of daughter and caregiver, while still cherishing her father-daughter relationship.");
				list.Add().Set("Nurturing daughter", "This daughter is empathetic and caring towards her father, often taking on the role of a nurturer. She may provide emotional support and care for her father, helping to strengthen their bond.");
				list.Add().Set("Entrepreneurial daughter", "This daughter may have inherited her father's entrepreneurial spirit and often turns to him for business advice and mentorship. They bond over their shared interests and drive for success.");
				list.Add().Set("Traveling daughter", "This daughter loves to travel and often joins her father on his business trips or family vacations. Together, they explore new places and create memories while strengthening their relationship.");
				list.Add().Set("Daughter as her father's confidante", "This daughter and her father share a close and trusting relationship, where she is able to confide in him and seek his advice. They may have open and honest communication, valuing each other's perspectives.");
				list.Add().Set("Non-traditional daughter figure", "Similar to a non-traditional mother figure, this daughter figure may not fit into traditional expectations of a daughter, but plays an important role in her father's life. She may be a mentor, friend, or caregiver, providing love and support to her father.");
				list.Add().Set("Adult daughter reconnecting with her father", "As an adult, this daughter may have drifted apart from her father, but seeks to reconnect and rebuild their relationship. She may have different life experiences now, but still values her father's guidance and influence.");
				list.Add().Set("Daughter figure for a father without biological daughters", "This daughter figure steps in to provide love and support for a father who may not have biological daughters, or has lost his daughter. She may fill a significant role in his life and offer unique perspectives as a female figure.");
				break;
			}
			case SOCIETYROLE_SON: {
				list.Add().Set("Close and supportive father-son relationship", "This son has a deep and loving relationship with his father, and the two have a strong bond. They may share common interests and values, and are always there to support each other.");
				list.Add().Set("Rebellious son", "This son may challenge his father's authority and values, seeking to establish his own identity. He may push boundaries and struggle to see eye to eye with his father, causing tension in the relationship.");
				list.Add().Set("Estranged son", "This son has a distant or strained relationship with his father, for various reasons such as divorce, distance, or misunderstandings. They may long for a stronger connection and are working to rebuild trust and a healthy relationship.");
				list.Add().Set("Close father-son communication", "This son prioritizes open and honest communication with his father, and values their discussions and advice. He may see his father as a mentor and role model, and seeks guidance in navigating challenges.");
				list.Add().Set("Athlete son", "This son may share a love for sports with his father, and the two bond over games and physical activities. They may have a competitive nature, but ultimately have a strong father-son connection built on shared interests.");
				list.Add().Set("Son with special needs", "This son may require extra care and attention due to physical or cognitive disabilities, and his father plays a crucial role in supporting and advocating for him. They share a unique bond and depend on each other for love and understanding.");
				list.Add().Set("Adopted son", "This son may have been adopted by his father, and while their biological connection may not exist, they have formed a strong and loving father-son bond. They may be navigating the challenges of building a relationship with shared experiences and creating new traditions.");
				list.Add().Set("Son with honor and respect for his father", "This son has a deep admiration and respect for his father, and looks up to him as a role model. He values his father's guidance and tries to emulate his good qualities and values.");
				list.Add().Set("Troubled son seeking a father figure", "This son may not have a strong male role model in his life, and seeks a father figure to guide and support him. He may be looking for a positive influence and guidance from an older male figure.");
				list.Add().Set("Son with a distant or difficult relationship with his father", "This son may have a strained relationship with his father due to past issues or misunderstandings, and they struggle to connect on a deeper level. They are working towards rebuilding their bond and understanding each other better.");
				break;
			}
			case SOCIETYROLE_FEMALE_RECRUITER_FOR_WORK: {
				list.Add().Set("Motivational recruiter", "This female recruiter uses positive reinforcement and motivation to encourage the unemployed male to go back to work. She may offer words of encouragement and support, and highlight the potential benefits of working.");
				list.Add().Set("Networking recruiter", "This recruiter has a wide network and can connect the unemployed male with potential job opportunities. She may offer guidance on how to expand his professional network and make valuable connections.");
				list.Add().Set("Mentor recruiter", "This recruiter takes on a mentorship role for the unemployed male, offering advice and guidance on career development and job search strategies. She may also share her own personal experiences and offer practical tips for success.");
				list.Add().Set("Collaborative recruiter", "This recruiter works closely with the unemployed male, listening to his career goals and collaborating on an action plan to achieve them. She values a strong partnership and communication with the individual to help them land a job.");
				list.Add().Set("Specialized recruiter", "This recruiter specializes in a specific industry or field and has a deep understanding of the job market in that area. She may offer valuable insights and resources to help the unemployed male find a job in his desired field.");
				list.Add().Set("Empathetic recruiter", "This recruiter is empathetic towards the individual's struggles and challenges with finding employment. She strives to create a supportive and understanding environment to help the individual feel more comfortable and confident in their job search.");
				list.Add().Set("Knowledgeable recruiter", "This recruiter is well-versed in the job search process and possesses a vast knowledge of resume writing, interview skills, and job application strategies. She may offer practical advice and tips to help the unemployed male stand out in the job market.");
				list.Add().Set("Flexible recruiter", "This recruiter understands that every individual has their own unique circumstances and may require a flexible approach to job searching. She is willing to cater to the individual's needs and find a job that fits their specific situation.");
				list.Add().Set("Assertive recruiter", "This recruiter is assertive and goal-oriented in her approach, pushing the unemployed male to take action and actively pursue job opportunities. She may also provide tough love and constructive criticism to help the individual improve their job search efforts.");
				list.Add().Set("Cultural recruiter", "This recruiter has a deep understanding of the individual's cultural background and may offer a unique perspective and tailored approach to help them find work. She may also connect them with job opportunities within their own cultural community.");
				break;
			}
			case SOCIETYROLE_MALE_RECRUITER_FOR_WORK: {
				list.Add().Set("Corporate Recruiter", "This recruiter works for a large corporation and is primarily concerned with finding candidates who possess a specific set of skills and qualifications. He may look for candidates with a strong background in a particular industry or with a specific degree.");
				list.Add().Set("Diversity & Inclusion Recruiter", "This recruiter is focused on promoting diversity in the workplace and may be seeking out candidates from underrepresented groups, including racial minorities, LGBTQ+ individuals, and individuals with disabilities.");
				list.Add().Set("Military Veteran Recruiter", "This recruiter specializes in finding jobs for military veterans, utilizing their unique skills and experience gained during their service. He may work with organizations that offer training programs to help veterans transition into the civilian workforce.");
				list.Add().Set("Workforce Development Recruiter", "This recruiter works with organizations that are committed to helping those who face barriers to employment, such as individuals with a criminal record or those who are experiencing homelessness. He may have connections with local community-based organizations to find potential candidates.");
				list.Add().Set("Career Placement Coordinator", "This recruiter works with educational institutions to help students and recent graduates find job opportunities that align with their skills and interests. He may also offer guidance on resume-building and interview skills.");
				list.Add().Set("Freelance Recruiter", "This recruiter works independently and may be contracted by a variety of organizations to help them find candidates for specific positions. He may have a wide network and expertise in identifying and connecting with talented individuals.");
				list.Add().Set("Technology Recruiter", "This recruiter specializes in finding candidates for roles in the technology and IT industry. He may have experience and knowledge in technical skills and may use specialized platforms and tools to identify potential candidates.");
				list.Add().Set("Health Care Recruiter", "This recruiter is focused on filling roles in the healthcare industry, from administrative positions to specialized medical positions. He may have a background in the healthcare field and understands the skills and qualifications needed for these roles.");
				list.Add().Set("Entry-Level Recruiter", "This recruiter focuses on finding jobs for recent graduates or individuals with little to no work experience. He may work closely with employers to identify potential candidates and provide mentorship and training opportunities.");
				list.Add().Set("Executive Recruiter", "This recruiter works with high-level positions in organizations, such as C-suite roles. He may have a vast network and expertise in identifying top talent for these positions, and may also provide coaching and consulting services throughout the hiring process.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_REPUBLICAN_PUBLIC_MESSAGE_SITE: {
				list.Add().Set("Gun rights advocate", "This woman is a strong believer in the Second Amendment and is a vocal advocate for gun rights. She may frequently post about her views on the importance of owning firearms for self-defense and protecting one's freedoms.");
				list.Add().Set("Pro-life activist", "This woman is a vocal supporter of the pro-life movement and frequently shares anti-abortion content on her profile. She may also participate in rallies and events advocating for stricter laws on abortion.");
				list.Add().Set("Fiscal conservative", "This woman strongly believes in limited government and low taxes, and may frequently share articles and posts supporting small government and fiscal responsibility.");
				list.Add().Set("Religious conservative", "This woman is a devout Christian and believes in incorporating traditional Christian values into politics. She may share Bible verses and religious content on her profile and support politicians who align with her religious beliefs.");
				list.Add().Set("Anti-immigration advocate", "This woman holds strong views against immigration, particularly illegal immigration, and may frequently post about the need for stricter immigration laws and border control.");
				list.Add().Set("Anti-LGBTQ rights", "This woman may hold strong anti-LGBTQ views and frequently share content that is critical of the LGBTQ community and their rights. She may support policies that limit LGBTQ rights and freedoms.");
				list.Add().Set("Traditional gender roles advocate", "This woman strongly believes in traditional gender roles and may share content that promotes the idea of a nuclear family with a stay-at-home mother and breadwinner father. She may also advocate for more conservative gender norms and reject ideas of gender fluidity.");
				list.Add().Set("Climate change skeptic", "This woman may hold strong beliefs against climate change and may post content that questions the science behind it. She may also support policies that prioritize economic growth over environmental concerns.");
				list.Add().Set("Strong supporter of former President Donald Trump", "This woman is a vocal supporter of former President Trump and may frequently post about his achievements and defend his actions. She may also share content promoting conspiracy theories about his impeachment and the 2020 election.");
				list.Add().Set("Anti-government regulations", "This woman strongly supports limited government intervention in business and may frequently share content that criticizes regulations and policies that she believes hinder economic growth and individual freedoms.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_REPUBLICAN_PUBLIC_MESSAGE_SITE: {
				list.Add().Set("Traditional conservative", "This male holds traditional conservative values such as limited government, gun rights, and traditional family structures. He believes in upholding traditional morals and may view progressive ideologies as a threat to the country.");
				list.Add().Set("Libertarian", "This male believes in individual liberty and minimal government intervention in personal and economic matters. He may have a strong belief in free-market capitalism and prioritize personal freedoms.");
				list.Add().Set("Nationalist", "This male is fiercely patriotic and believes in putting the country's interests above all else. He may have a strong stance on issues related to immigration and border control.");
				list.Add().Set("Religious", "This male's political views are heavily influenced by his religious beliefs. He may prioritize issues such as abortion, same-sex marriage, and religious freedom.");
				list.Add().Set("Anti-government", "This male holds strong anti-government beliefs and may view the government as corrupt and oppressive. He may support limited or no government control and may be a proponent of libertarianism or anarchy.");
				list.Add().Set("Trump supporter", "This male strongly supports former President Donald Trump and his policies. He may view Trump as a strong leader and defender of conservative values.");
				list.Add().Set("Traditionalist", "This male supports traditional values and may resist progressive changes in society. He may believe in traditional gender roles, traditional marriage, and traditional definitions of family.");
				list.Add().Set("Anti-establishment", "This male goes against the mainstream and traditional beliefs and is drawn to alternative viewpoints. He may reject mainstream media and traditional political parties for their perceived biases and agendas.");
				list.Add().Set("Conspiracy theorist", "This male may hold extreme and unfounded beliefs, often rooted in conspiracy theories. He may believe in QAnon, deep state conspiracies, and other fringe theories.");
				list.Add().Set("White supremacist", "This male holds racist and discriminatory views, often centering around white superiority and opposition to diversity and multiculturalism. He may believe in white nationalism and may be drawn to alt-right movements.");
				break;
			}
			case SOCIETYROLE_FEMALE_IN_INTERNET_INFLUENCER_FOR_ART_AND_MUSIC: {
				list.Add().Set("Art curator and influencer", "This influencer has a deep knowledge and appreciation for art, and shares their expertise with their followers through their online platforms. They may showcase new and emerging artists, as well as promote thought-provoking and diverse works of art.");
				list.Add().Set("Music playlist curator", "This influencer curates playlists on different music platforms, introducing their followers to a wide range of artists and genres. They may also share their personal insights and experiences with music, creating a sense of connection with their audience.");
				list.Add().Set("Tattoo artist influencer", "This influencer is a talented and well-respected tattoo artist, sharing their unique and stunning creations on their social media platforms. They may also provide insight and advice on the tattoo industry, as well as feature other artists and their work.");
				list.Add().Set("Female singer/songwriter influencer", "This influencer may be a successful musician in their own right, but also uses their platform to promote and uplift other female artists in the industry. They may share their own music, as well as collaborate with others to create meaningful and impactful songs.");
				list.Add().Set("Street art photographer", "This influencer has a keen eye for capturing the beauty and diversity of street art, sharing their photographs and stories on social media. They may also use their platform to raise awareness and support for street artists and their work.");
				list.Add().Set("Art and music event coordinator", "This influencer organizes and promotes art and music events, bringing together different artists and audiences in unique and innovative ways. They may also use their platform to promote important causes and social issues through their events.");
				list.Add().Set("Musical instrument instructor", "This influencer offers online lessons and tutorials for various musical instruments, using their skills and expertise to inspire and educate others. They may also use their platform to showcase new and upcoming talent in the music industry.");
				list.Add().Set("Female choreographer and dancer", "This influencer uses their platform to showcase their incredible dancing skills and share tutorials and tips for their followers. They may also collaborate with other dancers and artists to create visually stunning and empowering performances.");
				list.Add().Set("Visual arts and music therapist", "This influencer uses their platform to promote the therapeutic benefits of art and music, and may share their own artistic creations as well. They may also use their platform to educate and advocate for mental health and well-being.");
				list.Add().Set("Art and music writer", "This influencer uses their platform to share in-depth and thought-provoking articles about art and music, ranging from reviews of new projects to cultural critiques. They may also use their platform to showcase underrepresented artists and elevate diverse voices in the art and music industries.");
				break;
			}
			case SOCIETYROLE_MALE_IN_INTERNET_INFLUENCER_FOR_ART_AND_MUSIC: {
				list.Add().Set("Musician influencer", "These influencers are successful musicians who use social media to connect with and inspire their fans. They may share their own music, but also highlight and promote other artists, creating a community of music lovers.");
				list.Add().Set("Street artist influencer", "These influencers are known for their unique and often controversial street art, which they share and promote on social media platforms. They may use their platform to bring attention to important social and political issues.");
				list.Add().Set("Fashion influencer", "These influencers share their personal style and fashion trends on social media, inspiring and influencing their followers' fashion choices. They may also use their platform to promote sustainable and ethical fashion.");
				list.Add().Set("Art curator influencer", "These influencers have a passion for art and use social media to curate and showcase different artists and their works. They may also provide insights and commentary on current art trends and movements.");
				list.Add().Set("Music producer influencer", "These influencers use their social media presence to promote and share their music production skills and techniques. They may also offer advice and mentorship to aspiring producers.");
				list.Add().Set("Music blogger influencer", "These influencers have a strong love for music and share their opinions and reviews on different artists and their work. They may also create playlists and share music recommendations with their followers.");
				list.Add().Set("DJ influencer", "These influencers are DJs who use social media to connect with their fans and promote their music. They may also share their experiences and behind-the-scenes moments from their performances.");
				list.Add().Set("Film director influencer", "These influencers use social media to promote their films and share insights into the filmmaking process. They may also use their platform to advocate for diversity and representation in the film industry.");
				list.Add().Set("Music festival organizer influencer", "These influencers are involved in organizing and curating music festivals and use social media to promote and share information about these events. They may also use their platform to encourage sustainability and responsible consumption at festivals.");
				list.Add().Set("Musical instrument influencer", "These influencers use social media to share their skills and techniques on a specific musical instrument, and promote its use to inspire others to learn and create music. They may also collaborate with other musicians and artists to showcase the versatility of their instrument.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_PRODUCERS: {
				list.Add().Set("The supportive advocate", "This representative actively advocates for the rights of music producers and works to create policies and initiatives that support and protect their work. They may also provide resources and guidance for producers to navigate the industry.");
				list.Add().Set("The experienced producer", "This representative may have a successful career as a music producer and uses their platform to raise awareness and support for other producers. They may mentor and share their knowledge with emerging producers, and use their influence to create positive change in the industry.");
				list.Add().Set("The legal expert", "This representative has a deep understanding of the legal aspects of music production and fights for the rights of producers in legal battles. They may also provide legal advice and services to producers, ensuring they are protected and fairly compensated for their work.");
				list.Add().Set("The educator", "This representative may come from a background in music education or academia, and works to educate and empower producers to understand their rights and navigate the industry. They may also offer workshops, seminars, or online courses focused on producer rights.");
				list.Add().Set("The union leader", "This representative leads a union or organization specifically for music producers and negotiates on their behalf for fair wages, benefits, and working conditions. They may also organize protests or strikes to bring attention to the issues facing producers.");
				list.Add().Set("The community organizer", "This representative focuses on building a strong community of music producers and creating a support network for them. They may organize events, networking opportunities, or online communities where producers can connect and support each other.");
				list.Add().Set("The activist", "This representative is a vocal advocate for social justice and may use their platform to bring attention to the inequalities faced by music producers from marginalized communities. They work towards creating a more inclusive and equitable industry for all producers.");
				list.Add().Set("The liaison with record labels", "This representative acts as a bridge between music producers and record labels, working to ensure producers' rights are protected in contracts and negotiations. They may also fight for fair compensation and credit for producers on album releases.");
				list.Add().Set("The fundraiser", "This representative works to secure funding and resources for programs and initiatives that benefit music producers. They may organize fundraising events, partnerships with corporate sponsors, or grants to support producers in their work.");
				list.Add().Set("The technology advocate", "This representative focuses on the intersection of music production and technology, and works towards promoting and protecting the rights of producers in the digital landscape. They may also provide resources and support for producers to navigate the rapidly changing technological landscape.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_PRODUCERS: {
				list.Add().Set("Record label executive", "This representative works for a record label and is responsible for overseeing the production and distribution of music. They may also play a role in signing and promoting new artists, and have a significant impact on the direction of the music industry.");
				list.Add().Set("Music producer", "This representative is responsible for overseeing the creative and technical aspects of music production, working closely with artists to shape their sound and create successful and marketable tracks. They have a strong influence on the overall sound and trends of the music industry.");
				list.Add().Set("Artist manager", "This representative works directly with artists to help them navigate the industry, negotiate contracts and deals, and provide support and guidance in their careers. They play a vital role in helping artists succeed and have a direct impact on the success of the music industry as a whole.");
				list.Add().Set("Music streaming platform executive", "This representative works for a music streaming platform and is responsible for curating and promoting content on the platform. They may have a significant impact on which artists and songs receive exposure and success, shaping the industry's trends and popular music.");
				list.Add().Set("Music festival organizer", "This representative is responsible for organizing and managing music festivals, which have become a major source of income and exposure for artists. They may have a direct impact on which artists are featured and may also play a role in shaping the industry's trends and popular music.");
				list.Add().Set("Music journalist/critic", "This representative works in media and is responsible for reviewing and analyzing music. They may have a strong influence on public perception of artists and albums, shaping the success and popularity of the music industry.");
				list.Add().Set("Royalty collection society representative", "This representative works for a royalty collection society and is responsible for managing and distributing royalties to music producers and other industry professionals. They have a direct impact on the financial success and stability of the music industry.");
				list.Add().Set("Music lawyer", "This representative is responsible for providing legal advice and representation to artists, record labels, and other industry professionals. They play a crucial role in protecting the rights and interests of those in the music industry and may also have an impact on the direction of the industry.");
				list.Add().Set("Music education advocate", "This representative works to promote the importance and value of music education and may advocate for policies and funding to support it. They play a role in shaping the future of the music industry by nurturing young talent and encouraging diversity and creativity in the industry.");
				list.Add().Set("Collective rights management organization representative", "This representative works for a collective rights management organization, which is responsible for ensuring that music producers are fairly compensated for their work. They may have a direct impact on the financial success and stability of the music industry by advocating for the rights and interests of music producers.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_COMPOSERS: {
				list.Add().Set("Founder", "This person started the organization for rights of music composers and had the vision and drive to create change. They were likely a composer themselves and understood the struggles and challenges faced by their fellow creators.");
				list.Add().Set("Advocate", "This representative is a strong advocate for the rights of music composers and is vocal about the importance of protecting their intellectual property. They may push for policy changes and lobby for better compensation for music creators.");
				list.Add().Set("Legal expert", "This representative is well-versed in copyright law and uses their knowledge to protect the rights of music composers. They may take on legal battles on behalf of composers to ensure that their rights are not infringed upon.");
				list.Add().Set("Music composer representative", "This person is a music composer themselves and acts as a representative for their fellow creators. They provide valuable insight and perspective on the needs and challenges of music composers in the industry.");
				list.Add().Set("Educator", "This representative may focus on educating music creators about their rights and how to protect their work. They may conduct workshops or create resources to empower composers to advocate for themselves.");
				list.Add().Set("Public relations specialist", "This representative focuses on building relationships with the media and the public to raise awareness about the organization and its cause. They may also handle any public relations issues or crises that may arise.");
				list.Add().Set("Fundraiser", "This representative is responsible for securing funds for the organization through donations, grants, or sponsorships. They may also plan fundraising events to support the work of the organization.");
				list.Add().Set("International liaison", "This representative works to build relationships with music organizations and advocates for composers' rights on a global scale. They may collaborate with international organizations to push for better protections for music creators.");
				list.Add().Set("Music industry expert", "This representative has extensive experience and knowledge of the music industry, and uses their expertise to help shape policies and regulations that benefit composers. They may also work to bridge the gap between music creators and industry professionals.");
				list.Add().Set("Social media manager", "This representative utilizes social media platforms to promote the organization and engage with the community. They may also use social media to raise awareness about key issues and advocate for composers' rights.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_COMPOSERS: {
				list.Add().Set("President of a music composers association", "This individual leads and represents a group of music composers, advocating for their rights and promoting their work. They may organize events and opportunities for composers to showcase their talent and connect with other professionals in the industry.");
				list.Add().Set("Liaison with media outlets", "This role involves working closely with media outlets, such as radio stations, television channels, and online platforms, to promote the work of music composers. They may arrange for interviews and features, as well as collaborate on various projects to increase exposure for composers.");
				list.Add().Set("Legal representative", "This individual is responsible for advocating for the legal rights of music composers, such as copyright protection and fair compensation for their work. They may work with lawmakers and government agencies to ensure the interests of composers are represented.");
				list.Add().Set("Outreach coordinator", "This role focuses on building and maintaining relationships with other organizations and institutions that have a shared interest in music composition. They may collaborate on events, workshops, and projects to support and promote composers.");
				list.Add().Set("Fundraiser", "This individual is responsible for raising funds and resources for the organization and its programs. They may seek out sponsorships, grants, and donations to support the work of music composers and the organization's initiatives.");
				list.Add().Set("Educator and mentor", "This role involves providing education and mentorship opportunities for aspiring and established music composers. They may offer workshops, classes, and one-on-one guidance to help composers improve their skills and navigate the industry.");
				list.Add().Set("Communications and marketing specialist", "This individual is in charge of promoting the organization's mission and activities through various communication channels, such as social media, newsletters, and press releases. They may also create marketing campaigns to raise awareness and attract members and supporters to the organization.");
				list.Add().Set("Event planner", "This role involves organizing events and programs for the organization, such as concerts, festivals, and conferences. They may coordinate with venues, performers, and sponsors to ensure the smooth execution of events that promote the work of music composers.");
				list.Add().Set("Diversity and inclusion advocate", "This individual is dedicated to promoting diversity and inclusion in the music composition industry. They may work towards creating equal opportunities for composers from marginalized communities and raising awareness about the importance of representation in music.");
				list.Add().Set("Technology specialist", "This role focuses on utilizing technology to support and enhance the work of music composers. They may explore and implement new software and tools to help composers create, distribute, and protect their work.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_LYRICISTS: {
				list.Add().Set("Advocate for songwriter rights", "This representative actively fights for the rights of music lyricists and works to ensure fair compensation and recognition for their work in the music industry.");
				list.Add().Set("Educator for music lyricists", "This representative may provide training, workshops, or resources for music lyricists to help them navigate the complex industry and protect their rights as creators.");
				list.Add().Set("Collaborator with other organizations", "In addition to representing music lyricists, this representative may also work with other organizations such as music labels, streaming platforms, or copyright agencies to advocate for fair treatment and representation of songwriters.");
				list.Add().Set("Mentor for up-and-coming lyricists", "This representative may take on a mentorship role and offer guidance and support for aspiring music lyricists, helping them develop their skills and navigate the music industry.");
				list.Add().Set("Negotiator for fair deals and contracts", "This representative may negotiate on behalf of music lyricists, ensuring they receive fair compensation and terms for their work. They may also help lyricists understand and navigate complicated contracts.");
				list.Add().Set("Spokesperson for issues affecting music lyricists", "This representative may use their platform to speak out about important issues affecting music lyricists, such as copyright laws, fair compensation, or representation in the industry.");
				list.Add().Set("Ambassador for diversity and inclusion in the music industry", "This representative may work towards promoting diversity and inclusion in the music industry, ensuring that music lyricists from all backgrounds are represented and supported.");
				list.Add().Set("Liaison between lyricists and other industry professionals", "This representative may act as a liaison between music lyricists and other industry professionals, such as music producers or recording artists, to facilitate collaborations and ensure fair treatment for all parties involved.");
				list.Add().Set("Researcher and analyst of industry trends", "This representative may conduct research and analyze industry trends to better understand the current landscape for music lyricists and advocate for changes or improvements in the industry.");
				list.Add().Set("Crisis manager for lyricists facing challenges or injustices", "This representative may offer support and guidance for music lyricists who are facing challenges or injustices in the music industry, helping them navigate through difficult situations and find solutions.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_LYRICISTS: {
				list.Add().Set("Music industry executive", "This representative acts as a liaison between the organization and music industry executives, advocating for the rights and interests of music lyricists. They may negotiate contracts and work to secure fair compensation and recognition for lyricists.");
				list.Add().Set("Legal representative", "This representative ensures that lyricists' work is protected through copyright laws and intellectual property rights. They may also provide legal advice and support, and help lyricists navigate industry contracts and agreements.");
				list.Add().Set("Mentor/Coach", "This representative may be a successful music lyricist themselves, and offers guidance and support to up-and-coming lyricists. They may provide feedback and help nurture their skills and creativity, and advocate for their advancement in the industry.");
				list.Add().Set("Advocate for diversity and inclusion", "This representative actively promotes diversity and inclusion in the music industry, and works to create more opportunities for lyricists from marginalized communities. They may also collaborate with other organizations to address issues of discrimination and representation.");
				list.Add().Set("Educator", "This representative may work with music schools or programs to educate and train future music lyricists. They may also offer workshops and seminars on various aspects of the music industry to educate lyricists on their rights and how to navigate the industry.");
				list.Add().Set("Lobbyist", "This representative engages in advocacy and lobbying efforts to influence government policies and regulations that affect music lyricists. They may also work with lawmakers to introduce and pass legislation that supports the rights and interests of lyricists.");
				list.Add().Set("Public relations representative", "This representative focuses on promoting and elevating the public image of music lyricists, both individually and as a collective. They may organize events, campaigns, and partnerships to increase visibility and recognition for lyricists.");
				list.Add().Set("Community organizer", "This representative seeks to build a strong and supportive community among music lyricists, providing opportunities for networking, collaboration, and mentorship. They may also organize events and resources for lyricists to connect and learn from one another.");
				list.Add().Set("Researcher/Analyst", "This representative conducts research and analyzes data to better understand the challenges and opportunities faced by music lyricists. They may use this information to inform the organization's strategies and initiatives, as well as educate the public and policymakers.");
				list.Add().Set("Crisis management representative", "In times of controversy or conflict, this representative serves as a spokesperson and mediator for the organization and its members. They may handle PR crises and work towards finding resolutions that support the best interests of music lyricists.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_PUBLISHERS: {
				list.Add().Set("CEO of a Music Publisher's Organization", "This person is responsible for overseeing the organization and making strategic decisions for the benefit of music publishers. They may also be involved in developing and implementing policies and initiatives to support the rights of music publishers.");
				list.Add().Set("Lobbyist for music publisher's rights", "This individual advocates for the rights of music publishers and works to influence legislation and policies that affect their industry. They may communicate with lawmakers and government officials to make the case for stronger protections for music publishers.");
				list.Add().Set("Legal counsel for a music publisher's organization", "This position involves providing legal support and advice to the organization and its members on issues related to copyright, licensing, and other legal matters. They may also represent the organization in legal disputes or negotiations.");
				list.Add().Set("Member engagement coordinator", "This role involves engaging and communicating with the organization's members, promoting their interests and encouraging participation in events and initiatives. They may also gather feedback and input from members to inform the organization's decisions and priorities.");
				list.Add().Set("Education and training director", "This individual is responsible for designing and executing educational programs and training sessions for music publishers, on topics such as copyright laws, licensing, and business management. They may also organize workshops and conferences for the organization's members.");
				list.Add().Set("Diversity and inclusion officer", "This position involves promoting diversity and advocating for inclusivity within the music publishing industry. They may develop initiatives and policies to address issues of representation and access for underrepresented groups within the organization and the industry as a whole.");
				list.Add().Set("Marketing and communications manager", "This role involves creating and implementing marketing and communication strategies to promote the organization's initiatives and raise awareness of the issues facing music publishers. They may also handle media relations and maintain the organization's online presence.");
				list.Add().Set("Finance and operations director", "This individual oversees the financial and operational aspects of the organization, including budgeting, financial planning, and managing resources. They may also handle human resources and administrative tasks to ensure the smooth running of the organization.");
				list.Add().Set("International relations advisor", "This role involves representing the organization in international forums and advocating for the rights of music publishers on a global scale. They may also establish relationships with international organizations and collaborate with them on joint initiatives.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_PUBLISHERS: {
				list.Add().Set("Collaborative partnership", "This representative values building cooperative and mutually beneficial partnerships with music publishers. They prioritize open communication and transparent decision-making in their interactions with publishers.");
				list.Add().Set("Advocate for publishers", "This representative is a strong advocate for the rights and interests of music publishers, and actively promotes their work and contributions to the music industry. They may work to improve industry regulations and fight against piracy and copyright infringement.");
				list.Add().Set("Business-minded representative", "This representative is focused on the financial success of both the organization and its member publishers. They may prioritize securing profitable deals and partnerships, and facilitate negotiations and contracts on behalf of publishers.");
				list.Add().Set("Mediator", "In cases of conflicts or disputes between publishers and other industry players, this representative acts as a mediator and helps find a resolution that benefits all parties involved.");
				list.Add().Set("Innovator", "This representative is always seeking new and innovative ways for music publishers to succeed and thrive in the ever-changing music industry. They may introduce new strategies, technologies, or business models to help publishers adapt and grow.");
				list.Add().Set("Relationship builder", "This representative values developing and maintaining strong relationships with music publishers. They may organize networking events, conferences, or workshops to facilitate connections and foster a sense of community within the industry.");
				list.Add().Set("Educator", "This representative may offer educational resources and training opportunities for music publishers to enhance their skills and knowledge in the industry. They may also provide guidance and support for publishers looking to break into new markets or expand their reach.");
				list.Add().Set("Researcher and analyzer", "This representative stays up-to-date on market trends and conducts research to inform decision-making and initiatives for the benefit of music publishers. They may also provide publishers with data and insights to help them make informed business decisions.");
				list.Add().Set("Creative collaborator", "This representative values the artistic side of the music industry and seeks to foster collaborations between publishers and artists. They may organize events or campaigns to promote the work of publishers and showcase their creative talents.");
				list.Add().Set("Global connector", "This representative looks beyond national borders and works to connect music publishers from different countries and cultures. They may facilitate international partnerships and help publishers reach a global audience.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_PERFORMING_ARTISTS: {
				list.Add().Set("Advocate for mental health in the performing arts", "This representative works to raise awareness and provide resources for mental health struggles within the performing arts industry. They may advocate for better support and break the stigma surrounding mental health.");
				list.Add().Set("Equity and diversity advocate", "This representative promotes diversity and inclusivity within the performing arts, addressing issues of discrimination and advocating for equal opportunities for all performers. They work to create a safe and inclusive environment for artists from all backgrounds.");
				list.Add().Set("Artist's union representative", "This representative helps to negotiate and enforce fair contracts and working conditions for performing artists. They may also provide legal support for performers and advocate for their rights.");
				list.Add().Set("Lobbyist for performing arts funding", "This representative works to secure government funding and support for the arts, advocating for the importance and impact of the performing arts on society. They may also work to promote arts education and access to the arts for all.");
				list.Add().Set("Anti-harassment and safety advocate", "This representative fights against harassment and abuse within the performing arts industry, creating policies and resources to promote a safe and respectful environment for all artists. They also work towards holding abusers accountable and providing support for survivors.");
				list.Add().Set("Agent or manager", "This representative works closely with individual performing artists to manage their careers, negotiate contracts, and secure opportunities for their clients. They may also provide guidance and support for their clients in decision-making and personal matters.");
				list.Add().Set("Educational program director", "This representative oversees educational programs and initiatives for performing artists, providing training and resources to help them develop their skills and navigate the industry. They may also work on creating partnerships and collaborations with schools and organizations.");
				list.Add().Set("Public relations representative", "This representative manages the public image and reputation of performing artists, working to promote their work and handle any media or public relations issues. They may also advise artists on their brand and help them navigate the media landscape.");
				list.Add().Set("Event coordinator", "This representative plans and organizes events, such as performances, festivals, and conferences, for performing artists. They work to ensure a successful and impactful event for both the artists and the audience.");
				list.Add().Set("Community engagement coordinator", "This representative works to connect performing artists with their local community, creating opportunities for artists to share their work and engage with audiences. They may also collaborate with community organizations and use the arts to address social issues and promote positive change.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_PERFORMING_ARTISTS: {
				list.Add().Set("Mentor", "This representative serves as a mentor to performing artists, offering guidance, advice, and support in their career development. They may provide valuable industry connections and resources, and act as a source of inspiration and motivation for performers.");
				list.Add().Set("Advocate", "This representative advocates for the interests and rights of performing artists, fighting for fair compensation, safe working conditions, and respectful treatment. They may also work to bring awareness to issues and challenges facing performers in the industry.");
				list.Add().Set("Negotiator", "This representative negotiates contracts and deals on behalf of performing artists, ensuring fair and favorable terms. They may also mediate any disputes or conflicts that arise between artists and their employers.");
				list.Add().Set("Liaison", "This representative acts as a liaison between performing artists and other industry professionals, such as agents, managers, and producers. They facilitate communication and collaboration, and work to foster positive working relationships for everyone involved.");
				list.Add().Set("Financial advisor", "This representative provides financial guidance and support to performing artists, helping them manage their earnings and make informed financial decisions. They may also offer budgeting and investment advice to ensure long-term financial stability for performers.");
				list.Add().Set("Personal assistant", "This representative serves as a personal assistant to performing artists, handling day-to-day tasks and responsibilities so that performers can focus on their craft. They may also provide emotional support and act as a confidant for their clients.");
				list.Add().Set("Creative director", "This representative works closely with performing artists to develop their artistic vision and brand, helping them stand out in a competitive industry. They may also brainstorm and collaborate with artists on creative projects, such as music videos or stage performances.");
				list.Add().Set("Publicist", "This representative manages the public image and reputation of performing artists, creating strategies to promote and market their work. They may also handle crisis management and respond to any negative publicity on behalf of their clients.");
				list.Add().Set("Legal counsel", "This representative provides legal advice and representation to performing artists, protecting their rights and interests in any legal matters. They may also review contracts and negotiate on behalf of artists to ensure their legal rights are upheld.");
				list.Add().Set("Psychologist or therapist", "This representative offers support and counseling to performing artists dealing with mental health issues, such as performance anxiety or burnout. They may also provide coping strategies and resources to help performers manage the pressures and stresses of their career.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_RECORD_COMPANIES: {
				list.Add().Set("Collaborative negotiator", "This organization representative values open and respectful communication in working with artists and their record companies. They prioritize finding mutually beneficial solutions and fostering productive relationships for all parties involved.");
				list.Add().Set("Fair and transparent contract enforcer", "This representative ensures that both artists and record companies are held to their contractual agreements and handles any disputes or issues that may arise. They strive to maintain fairness and transparency in all business dealings.");
				list.Add().Set("Creative advocate", "This representative understands the importance of artistic expression and advocates for the rights of artists within the record company. They may push for fair compensation and creative control for artists, as well as fostering a supportive and collaborative environment.");
				list.Add().Set("Mediator for conflict resolution", "In the case of disagreements or disputes between artists and record companies, this representative serves as a neutral mediator to facilitate open communication and find common ground. They work towards maintaining positive relationships and ensuring the best outcome for both parties.");
				list.Add().Set("Financial strategist", "This representative focuses on the financial aspects of artists' contracts and earnings, ensuring that fair and accurate payments are made by record companies. They may also provide financial planning and budgeting advice for artists to help them succeed in the industry.");
				list.Add().Set("Eco-friendly and socially responsible advocate", "This representative promotes environmentally and socially responsible practices within record companies, working to reduce the industry's negative impact on the planet and promoting ethical treatment of artists and workers.");
				list.Add().Set("Public relations liaison", "This representative acts as a liaison between artists and record companies in all public relations matters. They work to maintain positive public image for both parties and handle any public disputes or controversies in a professional and strategic manner.");
				list.Add().Set("Resourceful problem solver", "In situations where there may be limitations or challenges, this representative uses their resourcefulness and creativity to find innovative solutions that benefit both artists and record companies. They may also anticipate and prevent potential issues before they arise.");
				list.Add().Set("Mentor and coach", "This representative acts as a mentor and coach for artists, providing guidance and support in their careers while also advocating for their rights within the record company. They may offer resources and advice for artists to navigate the industry successfully.");
				list.Add().Set("Cultural diversity and inclusivity promoter", "This representative values and promotes diversity and inclusivity within the record company and the industry as a whole. They may offer resources and support for artists from marginalized communities and ensure fair representation and opportunities for all artists.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_RECORD_COMPANIES: {
				list.Add().Set("Music industry veteran", "This representative has years of experience in the music industry and uses their knowledge and network to advocate for record companies. They have a deep understanding of the market and trends, and work to ensure the success of record companies.");
				list.Add().Set("Legal representative", "This representative is responsible for handling legal matters for record companies, such as contracts, copyright infringement, and licensing. They help protect the rights and interests of record companies in the ever-evolving music industry.");
				list.Add().Set("Public relations specialist", "This representative works on promoting and maintaining a positive image of record companies to the public. They may handle media relations, crisis communication, and marketing campaigns to improve the reputation and visibility of record companies.");
				list.Add().Set("Digital marketing expert", "This representative specializes in using digital platforms and strategies to promote record companies and their artists. They may manage social media accounts, website design, and online advertising to reach a wider audience and increase record sales.");
				list.Add().Set("A&R (Artist and Repertoire) representative", "This representative is responsible for scouting and signing talented artists to record companies. They have a keen ear for emerging talent and work to develop and promote new and existing artists on behalf of record companies.");
				list.Add().Set("Business development manager", "This representative focuses on building partnerships and connections for record companies, whether it's with other industry professionals, brands, or media outlets. They aim to expand the reach and influence of record companies in the competitive music market.");
				list.Add().Set("Tour/Event coordinator", "This representative works on organizing and coordinating tours and events for record companies and their artists. They handle logistics, budgeting, and promotion to ensure successful and profitable live performances for record companies.");
				list.Add().Set("Streaming platform specialist", "This representative has expertise in the streaming industry and works to secure deals and partnerships between record companies and popular streaming platforms. They help increase revenue and visibility for record companies through streaming services.");
				list.Add().Set("Crisis management consultant", "This representative provides valuable support and guidance to record companies during times of crisis and controversy. They help mitigate damage control and protect the reputation of record companies in challenging situations.");
				list.Add().Set("Diversity and inclusion officer", "This representative is responsible for promoting diversity and inclusivity within record companies and addressing any issues of discrimination or inequality. They work towards creating a safe and inclusive environment for all employees and artists within record companies.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_JOURNALISTS: {
				list.Add().Set("Advocate for diversity and inclusivity", "This representative works to promote diversity and inclusivity in the music journalism industry, advocating for underrepresented voices and perspectives to be included.");
				list.Add().Set("Mentor and guide for aspiring music journalists", "This representative offers guidance and mentorship for aspiring music journalists, sharing their own experiences and insights to help them succeed in the industry.");
				list.Add().Set("Activist for ethical journalism", "This representative prioritizes ethical journalism in the music industry and advocates for fair and responsible reporting and treatment of artists.");
				list.Add().Set("Influencer and thought leader", "This representative has established themselves as an influencer and thought leader in the music journalism world, using their platform to promote important and relevant discussions and debates.");
				list.Add().Set("Educator and trainer", "This representative offers educational resources and training for music journalists, helping them develop their skills and knowledge in the field.");
				list.Add().Set("Networker and connector", "This representative values building connections and networks within the music journalism community, bringing together professionals and fostering collaboration.");
				list.Add().Set("Innovator and trendsetter", "This representative is constantly pushing the boundaries and finding new and creative ways to promote music journalism, whether through multimedia or social media platforms.");
				list.Add().Set("Champion for independent and alternative music", "This representative specializes in promoting independent and alternative music, helping to elevate and amplify the voices of emerging artists and genres.");
				list.Add().Set("Connector between music industry and journalism", "This representative serves as a liaison between the music industry and journalism, facilitating communication and understanding between the two worlds.");
				list.Add().Set("Advocate for fair compensation", "This representative works to ensure that music journalists are fairly compensated for their work, advocating for better pay and recognition within the industry.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_JOURNALISTS: {
				list.Add().Set("Champion for diversity and inclusion", "This representative actively promotes diversity and inclusivity within music journalism, advocating for equal representation and recognition of marginalized voices in the industry. They may spearhead initiatives and campaigns to promote diversity and challenge systemic biases.");
				list.Add().Set("Experienced and respected music journalist", "This representative has a successful career in music journalism and is highly regarded in the industry. They may use their platform and influence to amplify the voices of marginalized communities and advocate for their inclusion in the field.");
				list.Add().Set("Emerging journalist", "This representative is new to the field of music journalism, and may be using their platform to bring attention to diverse and underrepresented voices in the industry. They may face challenges in establishing themselves in a competitive field, but are passionate and dedicated to making a positive impact.");
				list.Add().Set("Music industry professional", "This representative may come from a background in the music industry, such as a musician, producer, or publicist, and brings a unique perspective to music journalism. They may advocate for diversity and inclusion in the industry and use their experience to inform their writing and reporting.");
				list.Add().Set("Advocacy organization representative", "This representative may work for an organization that advocates for diversity and inclusivity within the music industry, and is dedicated to promoting equal opportunities for marginalized communities. They may collaborate with music journalists to bring attention to these issues and work towards tangible change.");
				list.Add().Set("Educator and mentor", "This representative may also be a teacher or mentor, providing guidance and support for aspiring music journalists from diverse backgrounds. They may have personal experience in overcoming barriers in the industry and use their knowledge to empower and uplift others.");
				list.Add().Set("Non-traditional music journalist", "Similar to a non-traditional father or mother figure, this representative may not fit into the typical image of a music journalist, but brings a unique perspective and voice to the industry. They may have unconventional methods of reporting and a distinct style, challenging traditional norms and promoting diversity.");
				list.Add().Set("Intersectional advocate", "This representative recognizes and addresses the intersections of different forms of marginalization, such as race, gender, sexuality, and disability, in the music industry. They actively use their platform to uplift and support diverse voices and advocate for equal representation and opportunities.");
				list.Add().Set("Community leader", "This representative may come from a grassroots community organization that supports diversity and inclusion in the music industry. They may work closely with marginalized communities and use their platform to bring attention to their voices and struggles.");
				list.Add().Set("Inclusion officer for a music publication or company", "This representative may work for a music publication or company that prioritizes diversity and inclusion, and is responsible for developing and implementing strategies to create a more inclusive and diverse work environment. They may also work with music journalists to ensure diversity and inclusivity in their reporting.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSICIANS_AND_MUSIC_WORKERS: {
				list.Add().Set("Union representative", "This individual works for a musician's union and is responsible for advocating for fair pay and job security for musicians and music workers. They may negotiate contracts and provide legal representation for union members.");
				list.Add().Set("Music industry lobbyist", "This individual represents the interests of musicians and music workers in the political sphere, advocating for legislation that protects their rights and improves their working conditions.");
				list.Add().Set("Legal aid for musicians", "This lawyer specializes in the legal issues that musicians and music workers may face, such as copyright infringement or contract disputes. They provide legal counsel and representation to ensure fair treatment and protection of their clients' rights.");
				list.Add().Set("Independent music rights advocate", "This individual works independently to protect the rights of musicians and music workers, advocating for fair pay, safe working conditions, and job security. They may also provide resources and support for career development.");
				list.Add().Set("Artist manager", "This professional represents and advocates for the interests of a specific artist or group, including their rights as performers and workers. They negotiate contracts and ensure their clients are compensated fairly for their work.");
				list.Add().Set("Labor union leader", "This individual leads a labor union that represents the rights of all workers, including musicians and music workers. They may organize strikes or protests to demand better working conditions and pay for their members.");
				list.Add().Set("Employee relations specialist", "In this role, the individual works for a music company or organization and is responsible for ensuring fair treatment and job security for employees, including musicians and music workers. They may handle disputes and foster positive employee-employer relationships.");
				list.Add().Set("Diversity and inclusion officer", "This professional works to promote diversity and inclusivity in the music industry, advocating for fair treatment and representation of marginalized musicians and music workers. They may also work to address issues of discrimination and harassment.");
				list.Add().Set("Musician support organization representative", "This individual works for an organization that provides resources and support for musicians and music workers, including legal assistance, career development, and mental health services. They advocate for the rights and well-being of these individuals within the music industry.");
				list.Add().Set("Music industry journalist", "This person reports on news and issues within the music industry, including discussions of monetary and job security for musicians and music workers. They may also provide commentary and analysis on these topics and advocate for positive changes within the industry.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSICIANS_AND_MUSIC_WORKERS: {
				list.Add().Set("Union representative", "This figure represents the interests of unionized musicians and music workers, advocating for better wages, working conditions, and benefits for its members. They may negotiate with employers and provide resources and support for members.");
				list.Add().Set("Non-profit organization representative", "This representative works for a non-profit organization that supports musicians and music workers, providing resources, educational opportunities, and monetary assistance for its members. They may also advocate for policies and laws that benefit the music industry.");
				list.Add().Set("Artist's manager", "This representative works intimately with individual musicians or bands, negotiating contracts, booking shows, and managing their career. They may work to ensure their clients receive fair compensation for their work and advocate for their interests.");
				list.Add().Set("Music industry lobbyist", "This representative is employed by lobbying firms or organizations to advocate for policies and laws that benefit the music industry and its workers. They may work to protect artists' and workers' rights and advocate for fair compensation.");
				list.Add().Set("Arts council representative", "This figure may be a member of a government arts council or agency, advocating for the interests of musicians and music workers in the community. They may work to secure funding and resources for music-related projects and initiatives.");
				list.Add().Set("Collective bargaining representative", "This representative works for a collective bargaining unit that negotiates contracts and working conditions for its members. They may work with both musicians and music industry employers to advocate for fair wages and benefits.");
				list.Add().Set("Music industry lawyer", "This representative specializes in music industry law and may work for law firms or independently. They may advocate for their clients' rights and seek fair compensation for their work, as well as provide legal advice and representation.");
				list.Add().Set("Music journalist or critic", "This representative is an influential voice in the music industry and may advocate for musicians and music workers through their writing or reporting. They may bring attention to issues faced by the industry and push for positive change.");
				list.Add().Set("Human resources representative", "This representative may work for a music industry employer, advocating for fair wages and benefits for workers within the company. They may also address and resolve any conflicts or issues that arise between employees and management.");
				list.Add().Set("Mental health advocate", "This representative may work for an organization or independently to raise awareness about mental health in the music industry. They may also provide support and resources for musicians and music workers struggling with mental health issues, and advocate for better access to mental health services.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_INDUSTRY: {
				list.Add().Set("Music industry lobbyist", "This representative works to influence government policy and legislation that affects the music industry. They may advocate for copyright laws, streaming regulations, and other industry-related issues.");
				list.Add().Set("Music journalist", "This representative creates content about the music industry, including news, reviews, and features. They help to inform and shape public perception of the industry and its artists.");
				list.Add().Set("Music industry lawyer", "This representative specializes in legal matters related to the music industry, such as contract negotiations, copyright infringement, and intellectual property rights. They play a crucial role in protecting the legal rights of artists and ensuring fair compensation for their work.");
				list.Add().Set("Music educator", "This representative is dedicated to educating aspiring musicians and industry professionals. They may teach music theory, production techniques, or business skills related to the music industry.");
				list.Add().Set("Music streaming executive", "This representative works for a streaming platform that distributes music to listeners. They may negotiate deals with record labels and artists, as well as analyze data to make strategic decisions for the platform.");
				list.Add().Set("Music venue owner", "This representative owns or manages a music venue, creating opportunities for artists to perform and for audiences to experience live music. They may also work with booking agents and other industry professionals to curate a diverse lineup of shows.");
				list.Add().Set("Tour manager", "This representative is responsible for managing a music artist's tour, including logistics, budgeting, and team coordination. They play a crucial role in ensuring the success and efficiency of a tour.");
				list.Add().Set("Music festival organizer", "This representative plans and oversees music festivals, coordinating logistics, booking artists, and promoting the event. They are responsible for creating unique and memorable experiences for attendees and maintaining the reputation of the festival.");
				list.Add().Set("Record label executive", "This representative oversees the business operations of a record label, including signing and promoting artists, marketing and distribution strategies, and financial management. They play a major role in shaping the direction and success of a label and its artists.");
				list.Add().Set("Music technology entrepreneur", "This representative develops and implements new technology for the music industry, such as software, apps, and hardware. They help to shape the future of the industry and make it more accessible and efficient for artists and listeners.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_INDUSTRY: {
				list.Add().Set("Industry veteran mentor", "This representative has years of experience in the music industry and has achieved a successful career. They use their expertise to mentor young artists and help them navigate the industry, offering valuable advice and guidance.");
				list.Add().Set("Artist development coach", "This representative works closely with new and emerging artists to help them refine and develop their musical skills. They provide constructive criticism and direction to help artists reach their full potential.");
				list.Add().Set("Diversity and inclusion advocate", "This representative is committed to promoting diversity and inclusion within the music industry. They work to create opportunities for underrepresented artists and educate others on the importance of diversity in music.");
				list.Add().Set("Songwriting mentor", "This representative is a skilled songwriter who uses their talents to mentor and collaborate with up-and-coming songwriters. They help artists craft compelling and authentic songs that resonate with audiences.");
				list.Add().Set("Music business mentor", "This representative focuses on the business side of the music industry, offering guidance on topics such as contracts, negotiation, and marketing. They help artists navigate the industry and make informed decisions for their careers.");
				list.Add().Set("Social media coach", "In today's digital age, this representative helps artists build and maintain a strong online presence. They provide tips and strategies for using social media effectively to connect with fans and promote their music.");
				list.Add().Set("Vocal coach", "This representative specializes in helping artists improve their vocal technique and performance skills. They offer lessons and workshops to help artists develop and maintain healthy and strong voices.");
				list.Add().Set("Production mentor", "This representative is a skilled producer or engineer who mentors artists on the technical aspects of music production. They help artists bring their musical visions to life and create high-quality recordings.");
				list.Add().Set("Mental health advocate", "This representative recognizes the challenges and pressures of the music industry and advocates for the importance of mental health and self-care. They provide support and resources for artists to prioritize their mental well-being.");
				list.Add().Set("Community leader", "This representative works within their local music community to connect artists with resources and opportunities. They may organize workshops, events, or networking opportunities to help support and elevate their community.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_EDUCATORS: {
				list.Add().Set("Dedicated music education advocate", "This individual is passionate about promoting and defending music education within schools and communities. They may volunteer, organize events, or actively speak out to raise awareness for the importance of music education.");
				list.Add().Set("Music education leader", "This person holds a leadership position within a music education organization and actively works to improve and expand music education initiatives. They may also collaborate with schools and government agencies to advocate for music education policies.");
				list.Add().Set("Music educator", "As a music teacher, this person is on the front lines of implementing and promoting music education in the classroom. They may also take on additional responsibilities, such as curriculum development, to further advocate for music education.");
				list.Add().Set("Music industry professional", "This individual may work in the music industry as a performer, producer, or other related role, and uses their platform to speak out in support of music education. They may also partner with music education organizations to provide resources and opportunities for students.");
				list.Add().Set("Parent advocate", "A parent who believes in the benefits of music education may take on the role of advocating for its importance in schools, whether through fundraising, organizing events, or speaking to school administrators and government officials.");
				list.Add().Set("Music education researcher", "This individual conducts research on the impact of music education and uses their findings to advocate for its inclusion in schools. They may also educate the public and decision-makers on the significance of music education.");
				list.Add().Set("Music education student leader", "This student is part of a student-led organization or club focused on promoting and supporting music education. They may organize performances or events to showcase the value of music education, and advocate for its inclusion in schools.");
				list.Add().Set("Community activist", "This individual is a passionate community member who recognizes the importance of music education and actively works to mobilize others to support it. They may engage in grassroots efforts such as petitioning or organizing rallies to raise awareness and advocate for music education.");
				list.Add().Set("Government liaison", "This person works within government agencies to advocate for policies and funding that support music education. They may also work with music education organizations to develop strategies for effective advocacy at the legislative level.");
				list.Add().Set("Diverse representation advocate", "This individual works towards promoting diversity and inclusivity in the music education field. They may advocate for equal access to music education for students from all backgrounds and work to ensure diverse representation among music educators and resources.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_EDUCATORS: {
				list.Add().Set("Advocacy specialist", "This representative is responsible for promoting and advocating for music education on behalf of the organization. They are knowledgeable about policy and legislation related to music education and actively work to influence decision-making processes.");
				list.Add().Set("Education expert", "This representative has a background in music education and uses their expertise to provide guidance and support to music educators. They may offer resources and professional development opportunities to enhance teaching practices.");
				list.Add().Set("Networking coordinator", "This representative focuses on building relationships and connections within the music education community. They may organize events and conferences to facilitate networking and collaboration among music educators.");
				list.Add().Set("Public relations manager", "This representative is responsible for managing the organization's public image and communication with the media. They may work to raise awareness about the value of music education and promote the organization's initiatives.");
				list.Add().Set("Fundraising coordinator", "This representative is in charge of fundraising efforts for the organization, seeking out grants and donations to support music education. They may also oversee fundraising events and campaigns.");
				list.Add().Set("Research and data analyst", "This representative uses data and research to inform the organization's advocacy efforts and decisions. They may conduct surveys and studies to gather information on the state of music education and its impact.");
				list.Add().Set("Technology specialist", "This representative focuses on utilizing technology to enhance music education. They may develop online resources, virtual learning platforms, or innovative teaching tools for music educators to use.");
				list.Add().Set("Diversity and inclusion advocate", "This representative works to promote diversity and inclusion within music education. They may develop initiatives and resources to support underrepresented groups and advocate for equitable access to music education.");
				list.Add().Set("Mental health advocate", "This representative recognizes the importance of mental health and works to support music educators in managing their own mental well-being. They may also advocate for resources and support for students' mental health needs in music education.");
				list.Add().Set("Community liaison", "This representative works to build relationships and partnerships with community organizations and leaders to support and promote music education. They may collaborate on events and projects that benefit music students and educators.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_RECORD_ARCHIVERS: {
				list.Add().Set("Advocating for legal protection", "This representative works tirelessly to advocate for legal protection and recognition of the rights of music record archivers. They may lobby for new legislation and work with policymakers to ensure the preservation and protection of these records.");
				list.Add().Set("Fundraising and resource acquisition", "This representative focuses on securing funding and resources for the organization. They may seek out grants, partnerships, and donations to support the organization's work and goals.");
				list.Add().Set("Collaborative partnerships", "This representative builds relationships and partnerships with other organizations, institutions, and individuals who share a common goal of protecting and preserving music records. They work together to amplify their impact and advocate for change.");
				list.Add().Set("Outreach and education", "This representative is responsible for educating the public and raising awareness about the value and importance of music record archiving. They may give presentations, organize workshops and events, and develop educational materials to promote their cause.");
				list.Add().Set("Community engagement", "This representative prioritizes building a strong and engaged community of supporters for the organization. They may organize volunteer events, community clean-up days, and other initiatives to involve the public in the organization’s work.");
				list.Add().Set("Collaborative research", "This representative works with researchers and academics to conduct studies and collect data on the state of music record archives and the impact of their preservation. This information is used to inform their advocacy efforts and promote the significance of music record archiving.");
				list.Add().Set("Legal representation and support", "This representative works closely with legal experts to provide representation and support for individuals or organizations facing legal challenges related to music record archiving. They may offer pro bono services or provide resources to help navigate legal proceedings.");
				list.Add().Set("Spotlighting archival success stories", "This representative highlights successful efforts in music record archiving and uses these stories to showcase the impact and value of their work. They may also share best practices and case studies to inspire others to get involved in preserving music records.");
				list.Add().Set("Collaborative international efforts", "This representative works with music record archiving organizations from around the world to share knowledge, resources, and support. They may participate in international conferences and initiatives to promote the cause on a global scale.");
				list.Add().Set("Advocating for equal representation", "This representative advocates for equal representation and diversity within the music record archiving field. They may work to address systemic barriers and promote inclusivity within the industry.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_RECORD_ARCHIVERS: {
				list.Add().Set("Preservation expert", "This individual is an expert in preserving music records and is dedicated to ensuring the long-term storage and maintenance of important recordings. They may have a deep passion for music and a strong understanding of the technical aspects of preserving recordings.");
				list.Add().Set("Archivist", "This person is responsible for organizing, cataloging, and preserving music records for future access and research. They have a deep understanding of the historical value of music recordings and work to ensure their accessibility for future generations.");
				list.Add().Set("Music historian", "This individual has a deep knowledge and passion for the history of music, and works to preserve and document various music records to provide insights on past musical practices and trends.");
				list.Add().Set("Technology specialist", "This person specializes in the technological aspects of preserving music records, such as digitizing and storing recordings in various formats to ensure their longevity. They may also stay updated on new technologies and techniques for record preservation.");
				list.Add().Set("Fundraising coordinator", "This individual works to secure funding and donations to support the organization's efforts in preserving music records. They may collaborate with other organizations or plan events to raise awareness and funds for the cause.");
				list.Add().Set("Outreach coordinator", "This person focuses on promoting the organization and its mission to the public through various outreach efforts. They may work on social media campaigns, partnerships with other organizations, and planning events to engage the community and raise awareness.");
				list.Add().Set("Volunteer coordinator", "This individual manages the organization's volunteers and helps to coordinate their efforts in preserving music records. They ensure that volunteers are properly trained and supervised, and work to create a positive and rewarding experience for all involved.");
				list.Add().Set("Event coordinator", "This role is responsible for planning and executing events related to music record preservation, such as concerts, workshops, or conferences. They work to bring together a diverse group of individuals passionate about music and record archiving.");
				list.Add().Set("Legal expert", "This individual helps the organization navigate legal issues and ensure compliance with copyright laws and ethics in record archiving. They may also work on obtaining rights or permissions for preserving and sharing certain recordings.");
				list.Add().Set("Board member", "This is a member of the organization's board of directors and helps guide the direction and decision-making of the organization. They may bring in their expertise in a relevant field or use their connections to further the organization's impact and reach.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_ACCOUNTANTS_OF_MUSIC_PRODUCERS: {
				list.Add().Set("Advocate for equal pay", "This representative strongly advocates for equal pay for accountants and music producers, highlighting the importance of fair compensation for their work.");
				list.Add().Set("Lobbyist for labor rights", "This representative works to improve labor laws and regulations for accountants and music producers, focusing on fair wages, benefits, and working conditions.");
				list.Add().Set("Negotiator for union contracts", "This representative negotiates union contracts on behalf of accountants and music producers, fighting for better job security and benefits for their members.");
				list.Add().Set("Educator for financial literacy", "This representative works to educate accountants and music producers on financial literacy and management, helping them secure their financial future and stability in their careers.");
				list.Add().Set("Mentor for job advancement", "This representative serves as a mentor for young accountants and music producers, providing guidance and support for career development and advancement.");
				list.Add().Set("Legal advocate for contract negotiations", "This representative offers legal support and advice for accountants and music producers during contract negotiations, ensuring their best interests are represented.");
				list.Add().Set("Mediator for conflicts", "This representative mediates conflicts and disputes between accountants, music producers, and their employers, striving for fair and amicable resolutions.");
				list.Add().Set("Financial advisor for retirement planning", "This representative offers financial planning services and retirement advice for accountants and music producers, helping them secure a stable future and financial independence.");
				list.Add().Set("Lobbyist for copyright protection", "This representative lobbies for stronger copyright protection laws for music producers, ensuring they receive fair compensation for their work and creativity.");
				list.Add().Set("Advocate for mental health support", "This representative advocates for mental health support and resources for accountants and music producers, recognizing the challenges and pressures of their demanding careers.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_ACCOUNTANTS_OF_MUSIC_PRODUCERS: {
				list.Add().Set("The advocate for fair pay", "This representative advocates for fair and competitive pay for accountants and music producers, and works to ensure that their contributions and expertise are valued and compensated fairly.");
				list.Add().Set("The negotiator for job security", "This representative negotiates on behalf of accountants and music producers to secure job stability and security. They may also work to implement policies and regulations that protect their clients' job security.");
				list.Add().Set("The strategist for financial success", "This representative strategizes ways to improve the financial success of accountants and music producers, such as finding new income streams, cost-cutting measures, and investment opportunities.");
				list.Add().Set("The peacemaker for conflicts", "This representative serves as a mediator between accountants, music producers, and their clients in cases of conflicts or disputes. They may work to find a mutually beneficial solution and maintain positive relationships.");
				list.Add().Set("The liaison for networking opportunities", "This representative serves as a liaison between accountants, music producers, and potential clients and collaborators, creating networking opportunities and promoting their clients' services.");
				list.Add().Set("The educator for financial literacy", "This representative provides education and resources for accountants and music producers to improve their financial literacy and management skills. This can help them make sound decisions and improve their monetary security.");
				list.Add().Set("The mentor for professional growth", "This representative serves as a mentor for accountants and music producers, offering guidance and support for their professional growth and development. They may also provide resources and connections for career advancement.");
				list.Add().Set("The crisis manager for unforeseen events", "This representative acts as a crisis manager for accountants and music producers in the event of unforeseen circumstances, such as economic downturns or natural disasters. They may offer financial and professional support to help their clients navigate these challenges.");
				list.Add().Set("The advocate for fair working conditions", "This representative advocates for fair and safe working conditions for accountants and music producers, including reasonable hours, breaks, and adequate resources. They may also work to address issues of discrimination, harassment, and unequal treatment.");
				list.Add().Set("The consultant for financial decision-making", "This representative acts as a consultant for accountants and music producers, offering financial expertise to help them make informed and strategic decisions for their businesses and careers.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_ARTIST_MANAGERS: {
				list.Add().Set("Advocate", "This representative serves as a vocal advocate for the rights and interests of music artist managers, working to bring attention to important issues and promote positive change.");
				list.Add().Set("Negotiator", "This representative has strong negotiation skills and works to ensure that music artist managers receive fair contracts and deals for their clients.");
				list.Add().Set("Educator", "This representative provides educational resources and workshops for music artist managers, helping them navigate the complex world of the music industry and stay informed about industry developments.");
				list.Add().Set("Advisor", "This representative offers guidance and advice to music artist managers, providing support and resources to help them make the best decisions for their clients.");
				list.Add().Set("Lobbyist", "This representative works to influence legislation and policies that affect music artist managers and their clients, advocating for fair and ethical practices within the industry.");
				list.Add().Set("Mediator", "This representative acts as a neutral party to help resolve disputes between music artist managers and other industry professionals, promoting healthy and productive relationships within the industry.");
				list.Add().Set("Organizer", "This representative plans and executes events and networking opportunities for music artist managers, fostering a sense of community and collaboration within the industry.");
				list.Add().Set("Investigator", "This representative conducts research and gathers information to better understand the challenges and needs of music artist managers, using this knowledge to inform their advocacy and educational efforts.");
				list.Add().Set("Public Relations Manager", "This representative manages the public image of the organization for rights of music artist managers, promoting its mission and values to the public and industry stakeholders.");
				list.Add().Set("Legal Counsel", "This representative provides legal advice and support to the organization and its members, ensuring that their rights and interests are protected in legal matters.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_ARTIST_MANAGERS: {
				list.Add().Set("Talent scout", "This representative is responsible for finding and identifying potential music artists to be represented by the organization. They may attend concerts, network with industry professionals, and review submissions to find the best talent to work with.");
				list.Add().Set("Contract negotiator", "This representative is skilled in negotiating contracts and deals on behalf of the organization and its artists. They ensure that the terms are favorable for both parties and that the artist's interests are protected.");
				list.Add().Set("Brand promoter", "This representative works to promote and elevate the brand of the organization and its artists. They may utilize social media, advertising, and partnerships to reach a wider audience and increase recognition for the organization and its talent.");
				list.Add().Set("Financial manager", "This representative handles the financial aspect of the organization, including budgeting, accounting, and investment decisions. They may work closely with the artists and their contracts to ensure financial success for both parties.");
				list.Add().Set("Public relations specialist", "This representative manages the public image of the organization and its artists. They may handle media interviews, press releases, and crisis management to protect and enhance the reputation of the organization and its talent.");
				list.Add().Set("Tour manager", "This representative is responsible for planning and coordinating tours for the organization's artists. They handle logistics, travel arrangements, and scheduling to ensure successful and smooth performances on the road.");
				list.Add().Set("Social media manager", "This representative is responsible for creating and maintaining a strong online presence for the organization and its artists. They may create content, engage with fans, and utilize social media strategies to increase visibility and engagement for the organization and its talent.");
				list.Add().Set("Legal advisor", "This representative provides legal counsel and advice to the organization and its artists. They may review contracts and agreements, help resolve disputes, and ensure that all legal aspects of the business are conducted ethically and responsibly.");
				list.Add().Set("Event planner", "This representative coordinates and plans events for the organization and its artists, such as album launches, showcases, and fan meet-and-greets. They work closely with the artists to ensure their vision is brought to life and the event is a success.");
				list.Add().Set("Creative director", "This representative oversees the creative direction and vision of the organization and its artists. They may work with the artists on their music, image, and branding to ensure a cohesive and strong aesthetic for the organization.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_MUSIC_CONSUMERS: {
				list.Add().Set("Advocate for fair compensation", "This representative is dedicated to fighting for fair compensation for small artists and newcomers in the music industry. They work to ensure that these artists receive proper payment for their work and are not taken advantage of by larger organizations.");
				list.Add().Set("Mentor and support system", "This representative acts as a mentor and support system for small artists and newcomers, providing guidance and resources to help them navigate the complex music industry. They may also offer emotional support and encouragement during the challenging early stages of a music career.");
				list.Add().Set("Educator on music industry rights", "This representative educates small artists and newcomers on their rights within the music industry, such as copyright laws and contract negotiations. They aim to empower these artists and help them protect themselves and their work.");
				list.Add().Set("Organizer of networking events", "This representative organizes networking events for small artists and newcomers to connect with industry professionals and each other. They create opportunities for these artists to showcase their work and form relationships that can help advance their careers.");
				list.Add().Set("Champion for diversity and representation", "This representative advocates for diversity and representation in the music industry, specifically for small artists and newcomers from marginalized communities. They work to break down barriers and provide equal opportunities for all artists to succeed.");
				list.Add().Set("Legal aid for artists in disputes", "This representative provides legal aid for small artists and newcomers who may be facing disputes with larger organizations or individuals. They may offer pro bono services or connect these artists with affordable legal resources to help protect their rights and interests.");
				list.Add().Set("Financial support for struggling artists", "This representative works to secure financial support and resources for struggling small artists and newcomers. They may help connect these artists with grants, funding, or other financial opportunities to support their careers.");
				list.Add().Set("Social media advocate", "This representative uses social media platforms to raise awareness about the rights of small artists and newcomers, and to promote their work. They may also use social media as a platform for these artists to share their stories and connect with a wider audience.");
				list.Add().Set("Lobbyist for policy change", "This representative lobbies for policy changes that will benefit small artists and newcomers in the music industry. They may work with government officials and other organizations to push for regulations that protect the rights and interests of these artists.");
				list.Add().Set("Collaborator with bigger organizations", "This representative collaborates with larger organizations in the music industry to promote and support small artists and newcomers. They work to bridge the gap between these different sectors and create opportunities for emerging artists to thrive.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_MUSIC_CONSUMERS: {
				list.Add().Set("Music industry executive", "This representative holds a high-level position in a music company and is responsible for making decisions that affect the interests of music consumers. They may have knowledge and experience in the industry and are dedicated to promoting the best music for youth.");
				list.Add().Set("Music journalist", "This representative reports on music news, trends, and events, and aims to inform and educate youth about the music industry. They may interview artists and provide critical analysis of music, shaping the opinions and tastes of youth.");
				list.Add().Set("Music educator", "This representative may work in a school or community setting, teaching youth about music theory, history, and performance. They may also advocate for the importance of music education in schools and promote opportunities for young musicians.");
				list.Add().Set("Youth ambassador for a music organization", "This representative is a young person who is passionate about music and works with a specific music organization to promote their interests. They may help plan events, engage with other youth, and provide a youth perspective on music-related issues.");
				list.Add().Set("Music streaming platform representative", "This representative may work for a popular music streaming service and represents the interests of music consumers using the platform. They may gather and analyze data to understand youth preferences and work to provide a better user experience.");
				list.Add().Set("Concert promoter", "This representative helps organize and promote music concerts and events for youth to attend. They may work closely with artists, venues, and sponsors to create a fun and safe environment for music fans.");
				list.Add().Set("Music therapist", "This representative uses music as a form of therapy to help youth with physical, emotional, or cognitive challenges. They may work in schools, hospitals, or community centers, and aim to improve the well-being and quality of life for youth through music.");
				list.Add().Set("Marketing specialist for a record label", "This representative works for a record label to promote and market music releases to a youth audience. They may use social media, events, and other strategies to reach and engage with young music consumers.");
				list.Add().Set("Non-profit organization representative", "This representative works for a non-profit organization that supports music education, access to music for underserved communities, or advocacy for fair treatment and compensation of music creators. They may collaborate with other music organizations to amplify their impact.");
				list.Add().Set("Youth council member for a music organization", "Similar to a youth ambassador, this representative is a young person who sits on a council for a music organization. They may provide suggestions and feedback on how the organization can better serve the interests of music consumers, particularly youth.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_COMPUTER_PROGRAMMERS: {
				list.Add().Set("Advocacy for gender diversity", "This representative works to promote gender diversity and inclusivity within the organization, advocating for equal opportunities and representation for all genders in the field of computer programming.");
				list.Add().Set("Addressing wage disparities", "This representative brings attention to and works towards closing gender-based wage gaps within the organization, ensuring fair and equal compensation for all programmers regardless of gender.");
				list.Add().Set("Inclusion for neurodiverse individuals", "This representative focuses on advocating for the inclusion and support of neurodiverse individuals within the organization, promoting an understanding and acceptance of different ways of thinking and processing information.");
				list.Add().Set("Cultural competence and inclusivity", "This representative works towards promoting cultural competence and inclusivity within the organization, acknowledging and celebrating the diversity of backgrounds and experiences within the programming community.");
				list.Add().Set("Accessibility for individuals with disabilities", "This representative advocates for accessibility and accommodations for individuals with disabilities within the organization, promoting inclusivity and equal opportunities for all programmers.");
				list.Add().Set("Anti-discrimination policies", "This representative works to implement and enforce anti-discrimination policies within the organization, ensuring a safe and inclusive environment for all programmers regardless of age, race, gender, sexual orientation, or any other identity.");
				list.Add().Set("Mental health support", "This representative emphasizes the importance of mental health support for programmers within the organization, working towards implementing resources and accommodations for those who may struggle with mental health issues.");
				list.Add().Set("Diversity in leadership", "This representative highlights the importance of diversity in leadership roles within the organization, advocating for equal representation and opportunities for individuals from underrepresented groups.");
				list.Add().Set("Addressing discrimination within the industry", "This representative works towards addressing discrimination and bias within the larger programming industry, promoting inclusivity and diversity throughout the entire field.");
				list.Add().Set("Support for marginalized communities", "This representative works to create a space for support and empowerment for marginalized communities within the organization, promoting diversity, equity, and inclusion for all members.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_COMPUTER_PROGRAMMERS: {
				list.Add().Set("Industry veteran mentor", "This mentor is an experienced and successful computer programmer who has been in the industry for many years. They offer guidance and advice to younger programmers, sharing their knowledge and expertise.");
				list.Add().Set("Peer mentor", "This mentor is a fellow programmer who may be slightly more experienced or advanced in their career, but still close enough in age and experience to provide relatable and relevant advice to their mentee.");
				list.Add().Set("Diversity and inclusion mentor", "This mentor focuses on supporting and encouraging underrepresented groups in the computer programming industry. They may provide guidance on navigating barriers and discrimination, and advocate for diversity and inclusivity.");
				list.Add().Set("Technical mentor", "This mentor specializes in a specific programming language or technology, and offers their expertise to their mentee. They may provide guidance and feedback on technical skills and offer career advice in their specific field.");
				list.Add().Set("Life balance mentor", "This mentor prioritizes work-life balance and helps their mentee find ways to maintain a healthy lifestyle while pursuing a career in computer programming. They may also provide guidance on managing stress and burnout.");
				list.Add().Set("Entrepreneurship mentor", "This mentor has experience in starting their own business or venture in the tech industry, and helps their mentee navigate the world of entrepreneurship. They may offer advice on business strategy, networking, and fundraising.");
				list.Add().Set("Leadership mentor", "This mentor has experience in a managerial or leadership role in the tech industry, and helps their mentee develop leadership skills. They may offer guidance on communication, team building, and other aspects of being a successful leader.");
				list.Add().Set("Remote work mentor", "This mentor has experience working remotely and helps their mentee adjust to a remote work environment. They may offer tips on time management, communication, and staying motivated while working from home.");
				list.Add().Set("Manager-mentee mentor", "This mentor is a current or former manager of their mentee, and offers guidance on navigating the relationship between manager and employee. They may provide advice on communication, performance management, and career advancement.");
				list.Add().Set("Soft skills mentor", "This mentor focuses on the non-technical skills needed to succeed in the tech industry, such as communication, teamwork, and problem-solving. They may offer role-playing exercises and feedback to help their mentee improve in these areas.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_ACCOUNTANTS_OF_COMPUTER_PROGRAMMERS: {
				list.Add().Set("Leading advocate for job security", "This representative is continually fighting for the job security rights of accountants and computer programmers. They may work closely with government officials, industry leaders, and unions to ensure fair treatment and job stability for workers.");
				list.Add().Set("Legal advisor for employment rights", "This representative is a legal expert on employment rights and may offer legal counsel and representation to individual accountants and computer programmers facing job insecurity. They may also work on policy and legislation that protects workers' rights.");
				list.Add().Set("Union representative", "This representative is a strong advocate for unionized accountants and computer programmers, and works with union leaders to negotiate fair contracts and defend members' job security. They may also provide resources and support for workers facing job loss or discrimination.");
				list.Add().Set("Educator on employment rights", "This representative may work for an organization that educates accountants and computer programmers on their employment rights and how to protect themselves from job insecurity. They may offer workshops, seminars, and resources on topics such as contract negotiation, employment law, and workplace harassment.");
				list.Add().Set("Manager for job placement and career development", "This representative works within an organization to help accountants and computer programmers find stable and fulfilling job opportunities. They may provide training, mentoring, and career guidance to help individuals increase their job security and advancement potential.");
				list.Add().Set("HR representative for a company", "This representative works within a company and is responsible for managing employee relations and ensuring fair treatment and job security for accountants and computer programmers. They may handle issues such as layoffs, contract negotiations, and diversity and inclusion initiatives.");
				list.Add().Set("Public relations and communication specialist", "This representative works to promote the organization and advocate for job security rights for accountants and computer programmers in the public sphere. They may manage social media, media relations, and public campaigns to raise awareness and promote change.");
				list.Add().Set("Technology and innovation strategist", "This representative focuses on the intersection of technology and employment rights for accountants and computer programmers. They may work to develop new technologies and strategies that can help protect job security and create more secure job opportunities in the future.");
				list.Add().Set("Mental health advocate", "This representative recognizes the impact of job insecurity on the mental health of accountants and computer programmers, and works to support and promote mental wellness within the industry. They may offer resources and support for workers facing job-related stress and anxiety, and advocate for better work-life balance and mental health initiatives in the workplace.");
				list.Add().Set("Diversity and inclusion advocate", "This representative is committed to promoting diversity and inclusion in the workplace for accountants and computer programmers. They may work to address inequalities and barriers to employment, and create a more welcoming and equitable environment for all individuals.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_ACCOUNTANTS_OF_COMPUTER_PROGRAMMERS: {
				list.Add().Set("HR manager", "This representative is responsible for hiring and managing employees, and ensuring job security for accountants or computer programmers within the organization. They may work to create opportunities for career advancement and provide a positive work environment for their employees.");
				list.Add().Set("Union representative", "This representative advocates for the rights and interests of accountants or computer programmers within the organization through collective bargaining and negotiating for job security provisions such as union contracts.");
				list.Add().Set("Legal counsel", "This representative provides legal support and advice for accountants or computer programmers in employment-related matters, such as contract negotiations and disputes, to ensure job security and fair treatment.");
				list.Add().Set("Diversity and inclusion officer", "This representative works to promote diversity and inclusivity in the workplace for accountants or computer programmers, creating a safe and inclusive environment where all employees have equal opportunities for job security and advancement.");
				list.Add().Set("Performance manager", "This representative evaluates employee performance and provides feedback and support to ensure job security and career growth for accountants or computer programmers within the organization.");
				list.Add().Set("Mentor or coach", "This representative acts as a mentor or coach for accountants or computer programmers, providing guidance and support for job security and career development within the organization.");
				list.Add().Set("Employee engagement manager", "This representative works to promote employee engagement and job satisfaction for accountants or computer programmers, which can contribute to job security and retention within the organization.");
				list.Add().Set("Health and wellness coordinator", "This representative focuses on promoting the physical and mental well-being of accountants or computer programmers, which can help to ensure job security and productivity within the organization.");
				list.Add().Set("Leadership or management team", "The leadership or management team of the organization plays a crucial role in creating and maintaining job security for accountants or computer programmers. They may implement policies and procedures to support employee satisfaction and retention.");
				list.Add().Set("Financial analyst", "This representative monitors the financial health and stability of the organization, which can impact job security for accountants or computer programmers. They may make strategic recommendations to support the security of these employees in the organization.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_SOFTWARE_COMPANIES: {
				list.Add().Set("Founder of a small start-up software company", "This individual recognized the need for an organization to advocate for the rights of small software companies and started the organization with the goal of fighting against monopolistic practices and advocating for fair competition in the industry.");
				list.Add().Set("Software company CEO", "This leader represents a larger software company and prioritizes the organization's goals of protecting intellectual property, promoting innovation, and advocating for policies that benefit the industry as a whole. They may also prioritize initiatives that support diversity and inclusion within the software industry.");
				list.Add().Set("Lobbyist for the organization", "This individual works closely with lawmakers and policymakers to promote the organization's agenda and influence legislation that benefits the software industry. They may also communicate directly with the public to increase awareness and support for the organization's goals.");
				list.Add().Set("Advocate for open-source software", "This member of the organization is passionate about promoting the use of open-source software and advocating for policies that support the free and open sharing of code. They may also work to educate the public about the benefits of open-source software and its impact on the industry.");
				list.Add().Set("Legal counsel for the organization", "This person provides legal expertise and advice to the organization on issues related to copyright, patents, and other legal matters that impact software companies. They may also represent the organization in court cases and help shape legal strategies.");
				list.Add().Set("Researcher on software industry trends", "This individual conducts research and data analysis to provide insights on the state of the software industry and how the organization's goals can best be achieved. They may also use their research to inform the organization's strategies and initiatives.");
				list.Add().Set("Communications director for the organization", "This person is responsible for managing the organization's external communications, including media relations and outreach to the public. They may also handle social media and public relations to promote the organization's goals and impact.");
				list.Add().Set("Technology advocate for developing countries", "This member of the organization focuses on using technology and software to promote economic growth and development in developing countries. They work to increase access to technology and advocate for policies that support their use in these countries.");
				list.Add().Set("Ethics and privacy specialist", "This individual advocates for ethical and responsible use of software and technology, with a focus on protecting user privacy and data. They may also work to educate the public and policymakers on these important issues and promote policies that prioritize user privacy.");
				list.Add().Set("Community outreach coordinator", "This person works to engage and involve the local community in the organization's goals and initiatives, promoting partnerships and collaborations with other organizations and businesses. They may also organize events and programs that bring together members of the community to support the organization's cause.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_SOFTWARE_COMPANIES: {
				list.Add().Set("Lobbyist for software companies", "This representative works to advocate for the interests and needs of software companies to government officials and policymakers. They may lobby for laws and policies that benefit the tech industry, and may also work with individual companies to create a cohesive voice for the industry.");
				list.Add().Set("Tech industry association leader", "This representative leads an organization that represents the collective interests of software companies, often through research, education, and networking opportunities. They may also work to address industry-wide issues and promote the growth and success of member companies.");
				list.Add().Set("Technology advocate for government agencies", "This representative works within government agencies to understand and assess the impact of policies on the tech industry, and advocate for the needs and concerns of software companies. They may also provide guidance and expertise on technology-related matters to policy makers.");
				list.Add().Set("Public relations spokesperson for software companies", "This representative serves as the public face for a specific software company or group of companies, communicating their messages and managing their reputation. They may handle media relations, crisis communication, and strategic messaging for the organization.");
				list.Add().Set("Consultant for software companies", "This representative offers specialized expertise and advice to software companies, helping them to navigate industry trends and challenges. They may also provide assistance with specific projects or issues, and may work with multiple companies at a time.");
				list.Add().Set("Trade show organizer for software industry", "This representative oversees the planning and execution of large-scale trade shows and conferences specifically for the software industry. They work to bring together industry professionals and companies, showcase innovative technology, and facilitate networking and business opportunities.");
				list.Add().Set("Policy analyst for tech industry", "This representative conducts research and analysis on policies and regulations that may impact the software industry, and provides insights and recommendations to companies and organizations. They may also be involved in advocacy efforts and policy development.");
				list.Add().Set("Tech startup accelerator leader", "This representative leads a program or organization that supports and nurtures the growth of new software companies. They may provide resources, mentorship, and networking opportunities to help startups succeed in a competitive industry.");
				list.Add().Set("Technology journalist", "This representative is a journalist or media personality who covers news and developments within the tech industry, and may have a focus on software companies. They provide transparency and analysis to the public, and may also influence public opinion and policy decisions.");
				list.Add().Set("Diversity and inclusion specialist for tech industry", "This representative works to promote diversity and inclusion within the tech industry, specifically for software companies. They may develop programs and initiatives to improve representation, and may also provide guidance and training for companies on how to foster a more inclusive workplace culture.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_SOFTWARE_INDUSTRY: {
				list.Add().Set("Advocate for mental health awareness", "This representative understands the unique challenges and pressures faced by geeky male programmers and actively advocates for mental health awareness and support within the software industry.");
				list.Add().Set("Mentor and role model", "This representative serves as a mentor and role model for geeky male programmers, offering guidance and support in their career development and personal growth.");
				list.Add().Set("Diversity and inclusion champion", "This representative is dedicated to promoting diversity and inclusion within the software industry and works towards creating a more welcoming and inclusive environment for geeky male programmers from all backgrounds.");
				list.Add().Set("Collaborative leader", "This representative values collaboration and teamwork within the software industry and promotes a healthy work culture that supports the well-being of geeky male programmers.");
				list.Add().Set("Advocate for work-life balance", "This representative recognizes the importance of work-life balance for geeky male programmers and advocates for policies and practices that support a healthy balance between work and personal life.");
				list.Add().Set("Advocate for equal pay and opportunities", "This representative fights for equal pay and opportunities for geeky male programmers, challenging the gender pay gap and promoting fair hiring practices within the software industry.");
				list.Add().Set("Mental health and wellness initiatives coordinator", "This representative focuses on organizing mental health and wellness initiatives within the software industry, recognizing the importance of supporting the well-being of geeky male programmers.");
				list.Add().Set("Policy advocate for parental leave and flexible work options", "This representative advocates for policies that support parental leave and flexible work options, recognizing the importance of supporting geeky male programmers who may have caregiving responsibilities.");
				list.Add().Set("Pushing for fair and ethical treatment of workers", "This representative speaks out against unfair and unethical treatment of workers within the software industry, including cases of labor exploitation and discrimination against geeky male programmers.");
				list.Add().Set("Collaborative efforts with therapy and support organizations", "This representative works with therapy and support organizations to provide resources and support for geeky male programmers who may be struggling with mental health or other challenges in their personal lives.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_SOFTWARE_INDUSTRY: {
				list.Add().Set("Female CEO promoting diversity and inclusivity", "This leader of the software industry recognizes and prioritizes the need for diversity in the field and actively works towards creating an inclusive environment for all, including geeky weak male programmers. She ensures equal opportunities and resources for all employees.");
				list.Add().Set("Male mentor advocating for mental health support", "This mentor recognizes the struggles faced by geeky weak male programmers and works to provide mental health support and resources within the organization. He promotes open and honest discussions about mental health and encourages seeking help when needed.");
				list.Add().Set("Non-binary representative promoting gender-inclusivity", "This representative advocates for the inclusion and support of all gender identities within the software industry, including geeky weak male programmers. They work towards creating a safe and welcoming workplace for individuals of all gender identities.");
				list.Add().Set("Leader promoting work-life balance", "This leader recognizes the negative impact of overworking and burnout in the software industry, and advocates for a healthy work-life balance. They prioritize the well-being of geeky weak male programmers and implement policies to prevent burnout.");
				list.Add().Set("Representative for disability rights and accommodations", "This representative ensures that the organization is inclusive and accommodating for individuals with disabilities, including those who may face challenges in the fast-paced and high-stress environment of the software industry. They work towards creating a supportive and accessible workplace for all employees.");
				list.Add().Set("Executive promoting workplace diversity and anti-discrimination policies", "This executive recognizes the importance of diversity and anti-discrimination policies in the software industry, and actively works towards creating a fair and inclusive workplace for all employees, including geeky weak male programmers.");
				list.Add().Set("Employee relations manager addressing toxic work environments", "This manager addresses and takes action against toxic work environments and unhealthy company culture that may impact geeky weak male programmers. They promote a positive and respectful workplace for all employees.");
				list.Add().Set("Tech industry lobbyist advocating for better working conditions", "This lobbyist works to influence policies and legislation that will improve working conditions for all employees in the software industry, including geeky weak male programmers. They strive for fair wages, reasonable work hours, and better benefits for all workers.");
				list.Add().Set("HR representative providing support and resources", "This HR representative acts as a source of support and resources for all employees, including geeky weak male programmers. They address any concerns or issues and work to create a positive and inclusive workplace for all.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_RIGHTS_OF_SOFTWARE_PROJECT_MANAGERS: {
				list.Add().Set("Experienced project manager", "This representative has years of experience in managing software projects and teams, and uses their knowledge and expertise to advocate for the rights and needs of project managers. They may have faced various challenges and have developed strategies for successfully managing teams.");
				list.Add().Set("Young project manager", "This representative may be a new project manager, but is passionate about the field and wants to make a positive impact for other project managers. They may offer fresh perspectives and unique insights into the current challenges faced by project managers.");
				list.Add().Set("Female project manager", "This representative offers a female perspective on project management and advocates for gender equality and representation in the field. They may also prioritize addressing and overcoming barriers faced by women in project management.");
				list.Add().Set("Minority project manager", "This representative represents a minority group in the software project management field and brings attention to the unique challenges and contributions of underrepresented communities. They may work to promote diversity and inclusion in the industry.");
				list.Add().Set("Project manager from a small company", "This representative understands the specific challenges faced by project managers in smaller companies, such as limited resources and tight budgets. They may advocate for better support and resources for project managers in these environments.");
				list.Add().Set("Project manager from a large company", "This representative comes from a large organization and may bring awareness to the different dynamics and challenges faced by project managers in bigger corporations. They may also advocate for better support and recognition for project managers in these environments.");
				list.Add().Set("Global project manager", "This representative has experience managing software projects in different countries and cultures, and brings attention to the global impact of project management. They may advocate for a more global perspective in the field.");
				list.Add().Set("Project manager with remote team experience", "This representative has managed teams remotely and may advocate for better support and resources for virtual project managers. They may also address the challenges of managing remote teams and promote innovative solutions.");
				list.Add().Set("Tech-savvy project manager", "This representative is up-to-date with the latest technology and digital trends in the project management field. They may advocate for the use of new tools and techniques to improve project management processes and efficiency.");
				list.Add().Set("Project manager with a background in psychology or leadership", "This representative brings a deeper understanding of human behavior and leadership to project management, and may advocate for a more people-centered approach. They may also promote the importance of emotional intelligence and team dynamics in project management.");
				break;
			}
			case SOCIETYROLE_REPRESENTATIVE_OF_THE_ORGANIZATION_FOR_INTEREST_OF_SOFTWARE_PROJECT_MANAGERS: {
				list.Add().Set("Supportive and collaborative organization", "This organization values the input and role of project managers, and actively works to support and collaborate with them. They prioritize communication and trust with project managers in order to achieve successful projects.");
				list.Add().Set("Hierarchical organization", "This organization follows a strict hierarchy, with project managers being accountable to higher-level management. They may prioritize efficiency and follow standardized processes, which can sometimes limit the autonomy of project managers.");
				list.Add().Set("Remote or virtual organization", "This organization operates primarily online or in a geographically dispersed manner, which can present unique communication and management challenges for project managers. They may prioritize flexibility and adaptability in order to effectively manage projects.");
				list.Add().Set("Innovative and adaptive organization", "This organization is open to change and encourages innovation, which allows project managers to experiment with new ideas and approaches. They may prioritize creativity and risk-taking in order to drive successful projects.");
				list.Add().Set("Stakeholder-oriented organization", "This organization places high value on the satisfaction of stakeholders, and as a result, project managers may have to balance the needs and expectations of multiple stakeholders in their projects. They may prioritize strong stakeholder communication and relationship-building.");
				list.Add().Set("Goal-oriented organization", "This organization focuses heavily on achieving concrete goals and objectives, and project managers may have to adhere to strict timelines and budgets. They may prioritize efficiency and results-driven approaches in order to meet project goals.");
				list.Add().Set("Mentorship-oriented organization", "This organization promotes mentorship and learning among project managers, which can foster a positive and supportive environment for growth and development. They may prioritize mentorship programs and opportunities for project managers to learn from industry experts.");
				list.Add().Set("Diverse and inclusive organization", "This organization values diversity and inclusivity, which can lead to a varied and inclusive team of project managers. They may prioritize creating an inclusive culture and providing resources for project managers from different backgrounds.");
				list.Add().Set("Deadline-driven organization", "This organization places heavy emphasis on meeting deadlines and delivering projects on time. Project managers may need to work under tight time constraints and manage high-pressure situations. They may prioritize time management and accountability.");
				list.Add().Set("Resource-limited organization", "This organization may have limited resources, such as budget or staff, which can present challenges for project managers. They may prioritize effective resource allocation and creative problem-solving in order to successfully manage projects within limitations.");
				break;
			}
			case SOCIETYROLE_ANGRY_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Online troll", "This person uses the internet to purposely provoke and upset others using offensive or controversial statements. They may feel a sense of power and control when they successfully elicit angry reactions from others.");
                list.Add().Set("Cyberbully", "This person uses the internet to bully and harass others, often targeting specific individuals or groups. They may experience feelings of anger and resentment towards their victims and use the anonymity of the internet to incite more harm.");
                list.Add().Set("Keyboard warrior", "This person is quick to unleash their anger and frustration through aggressive and confrontational comments on social media platforms. They may feel a sense of liberation and empowerment in expressing their emotions online.");
                list.Add().Set("Outrage machine", "This person often seeks out content or topics that they know will trigger their anger and actively fuels their own outrage and the outrage of others. They may engage in online debates and arguments, venting their anger and spreading negativity.");
                list.Add().Set("Victim of online harassment", "This person may frequently encounter hateful and threatening comments or messages directed towards them on the internet. The constant barrage of anger and negativity can have damaging effects on their mental health and well-being.");
                list.Add().Set("Social media addict", "This person is constantly scrolling through their social media feeds, often becoming agitated and angry when they see content or opinions that differ from their own. They may become overly invested and emotionally reactive to what they see online.");
                list.Add().Set("Fear-monger", "This person uses anger and fear to manipulate and control others online. They may spread false or exaggerated information to incite anger and resentment towards a certain group or individual for personal gain.");
                list.Add().Set("Burnout from exposure to angry content", "This person may feel overwhelmed and exhausted from constantly being exposed to angry and inflammatory content on the internet. The constant stream of negative emotions may lead to burnout and a decrease in their overall well-being.");
                list.Add().Set("Impacted by online outrage culture", "This person may find it difficult to navigate online spaces where anger and outrage are constantly being amplified. The toxic environment may cause them to feel fearful, anxious, and on edge.");
                list.Add().Set("Angry at their own online reputation", "This person may have had past instances where their own angry outbursts online have caused them a negative reputation. They may struggle with managing their anger and its effects on their relationships and digital identity.");
				break;
			}
			case SOCIETYROLE_EMPATHETIC_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Online community moderator", "This empathetic individual is responsible for cultivating a positive and inclusive online community. They actively listen to and address the concerns of community members, and foster connections between people.");
                list.Add().Set("Mental health advocate", "This person uses the internet as a platform to spread awareness and understanding about mental health and offer support and resources to those in need. They may share personal stories and encourage open discussions to help people feel less alone.");
                list.Add().Set("Virtual counselor or therapist", "This empathetic person offers online counseling or therapy services, providing a safe and supportive space for individuals to process and work through their emotions and challenges.");
                list.Add().Set("Social media influencer", "This person uses their platform and influence on social media to spread positivity and uplift others. They may share messages of inspiration, offer a listening ear to their followers, and encourage authentic connections.");
                list.Add().Set("Online support group leader", "This empathetic individual leads or moderates online support groups for people going through similar experiences. They offer a non-judgmental space for individuals to share their struggles and receive guidance and support from others.");
                list.Add().Set("Online volunteer", "This person uses the internet to volunteer their time and skills to support various causes and organizations. They may connect with others who share their passion for making a difference and work together to create positive change.");
                list.Add().Set("Content creator", "This empathetic individual creates online content that resonates with others and promotes empathy and understanding. They may use their platform to share diverse perspectives and experiences, and encourage meaningful conversations.");
                list.Add().Set("Online mentor or coach", "This empathetic person offers guidance and support to others through online mentorship or coaching sessions. They may use their own experiences and expertise to connect with and help others navigate their personal and professional challenges.");
                list.Add().Set("Blogger or vlogger", "This person uses their blog or YouTube channel to connect with their audience and share their thoughts, experiences, and advice. They may offer a listening ear to their followers and use their platform to create a sense of community and support.");
                list.Add().Set("Virtual event organizer", "This empathetic individual organizes and hosts virtual events, such as webinars, workshops, or meet-ups, to bring people together and foster connections. They may curate events that focus on empathy and building meaningful relationships.");
				break;
			}
			case SOCIETYROLE_CURIOUS_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Multicultural blogger", "This individual uses their platform on the internet to share their experiences and knowledge about different cultures and perspectives from their own multicultural background.");
                list.Add().Set("Inclusive influencer", "This person actively seeks out diverse perspectives and promotes inclusivity in their content on social media. They may also use their platform to amplify marginalized voices.");
                list.Add().Set("Digital anthropologist", "This individual studies human behavior and culture online, and may focus on understanding diversity and representation in digital communities.");
                list.Add().Set("Intersectional activist", "This person utilizes their online presence to advocate for and bring attention to issues related to intersectionality, such as racism, sexism, ableism, and more.");
                list.Add().Set("Diversity advocate", "This individual is passionate about promoting diversity and inclusion, and may use their online presence to educate others and create conversations about important topics.");
                list.Add().Set("International correspondent", "This person uses the internet to connect with individuals from all over the world, sharing stories and perspectives from different countries and cultures.");
                list.Add().Set("Human rights vlogger", "This individual uses their platform on YouTube or other video-sharing sites to shed light on human rights issues and advocate for diversity and acceptance.");
                list.Add().Set("Gender and sexuality educator", "This person utilizes their online presence to provide information and education about gender and sexuality, and may focus on promoting understanding and acceptance of diverse identities.");
                list.Add().Set("Digital storyteller", "This individual shares personal stories and experiences from their own diverse background, promoting empathy and understanding through digital storytelling.");
                list.Add().Set("Disability advocate", "This person uses their online presence to amplify voices and advocate for the rights and inclusion of individuals with disabilities. They may also provide resources and support for the disabled community.");
				break;
			}
			case SOCIETYROLE_ENTHUSIASTIC_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Multicultural blogger", "This individual uses their platform on the internet to share their experiences and knowledge about different cultures and perspectives from their own multicultural background.");
                list.Add().Set("Inclusive influencer", "This person actively seeks out diverse perspectives and promotes inclusivity in their content on social media. They may also use their platform to amplify marginalized voices.");
                list.Add().Set("Digital anthropologist", "This individual studies human behavior and culture online, and may focus on understanding diversity and representation in digital communities.");
                list.Add().Set("Intersectional activist", "This person utilizes their online presence to advocate for and bring attention to issues related to intersectionality, such as racism, sexism, ableism, and more.");
                list.Add().Set("Diversity advocate", "This individual is passionate about promoting diversity and inclusion, and may use their online presence to educate others and create conversations about important topics.");
                list.Add().Set("International correspondent", "This person uses the internet to connect with individuals from all over the world, sharing stories and perspectives from different countries and cultures.");
                list.Add().Set("Human rights vlogger", "This individual uses their platform on YouTube or other video-sharing sites to shed light on human rights issues and advocate for diversity and acceptance.");
                list.Add().Set("Gender and sexuality educator", "This person utilizes their online presence to provide information and education about gender and sexuality, and may focus on promoting understanding and acceptance of diverse identities.");
                list.Add().Set("Digital storyteller", "This individual shares personal stories and experiences from their own diverse background, promoting empathy and understanding through digital storytelling.");
                list.Add().Set("Disability advocate", "This person uses their online presence to amplify voices and advocate for the rights and inclusion of individuals with disabilities. They may also provide resources and support for the disabled community.");
				break;
			}
			case SOCIETYROLE_SKEPTICAL_PERSON_IN_THE_INTERNET: {
                list.Add().Set("The fact-checker", "This person is diligent about verifying information online before accepting it as truth. They use reputable sources and are not easily swayed by sensationalized content.");
                list.Add().Set("The conspiracy theorist", "This person is highly skeptical of mainstream narratives and often believes in elaborate conspiracy theories. They may be distrustful of the government, media, and other institutions.");
                list.Add().Set("The troll", "This person enjoys stirring up controversy and provoking reactions online. They may argue for the sake of arguing and love to see others get riled up.");
                list.Add().Set("The jaded user", "This person has been burned by false information or scams on the internet before, leading them to be wary and suspicious of everything they see online.");
                list.Add().Set("The cautious user", "This person takes every precaution when sharing personal information or engaging with strangers online. They may use privacy settings and avoid interacting with unknown accounts.");
                list.Add().Set("The influencer skeptic", "This person is highly skeptical of influencers and their motives, often questioning the authenticity of their sponsored content and product endorsements.");
                list.Add().Set("The social media skeptic", "This person is skeptical of the impact of social media on society, believing it to cause harm and perpetuate fake news and unhealthy comparisons.");
                list.Add().Set("The cautionary parent", "This person is highly skeptical of the internet's influence on children and teenagers, closely monitoring their online activities and setting strict rules and restrictions.");
                list.Add().Set("The keyboard warrior", "This person is quick to criticize and call out others online, often hiding behind a screen to voice their opinions and engage in heated debates.");
                list.Add().Set("The skeptic turned believer", "This person used to be skeptical of the internet, but has since learned how to navigate and find trustworthy sources online. They may be more open-minded and accepting of information now.");
				break;
			}
			case SOCIETYROLE_CONFUSED_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Tech-savvy but overwhelmed", "This person is adept at navigating technology and using various online platforms, but may feel overwhelmed with the constant influx of information and updates.");
                list.Add().Set("Social media addict", "This person spends a significant amount of time on social media and has difficulty disconnecting from the online world. They may struggle with finding a balance between online and offline life.");
                list.Add().Set("Constantly seeking validation", "This person feels the need to constantly seek validation and approval from others on social media and may feel obsessed with likes, comments, and followers.");
                list.Add().Set("Fear of missing out", "This person is always trying to stay updated on the latest trends and news on social media and may feel anxious about missing out on any important information.");
                list.Add().Set("Vulnerable to cyberbullying", "This person may be vulnerable to cyberbullying due to their frequent use of online platforms and may struggle with finding ways to cope with negative comments or messages.");
                list.Add().Set("Overwhelming FOMO", "This person is constantly worried about missing out on online opportunities, events, or new information, leading to a fear of missing out (FOMO) on social media.");
                list.Add().Set("Struggling to keep up", "This person may struggle to keep up with the ever-changing landscape of the internet and may feel overwhelmed and left behind by new trends or platforms.");
                list.Add().Set("Unclear about personal boundaries", "This person may have trouble maintaining boundaries online, leading to oversharing personal information or engaging in risky online behaviors.");
                list.Add().Set("Easily influenced", "This person is easily influenced by the opinions and content they see on social media and may struggle with forming their own critical thoughts and opinions.");
                list.Add().Set("Difficulty separating fact from fiction", "This person may have a hard time distinguishing between what is real and what is fabricated online, leading to confusion and potentially falling for false information.");
				break;
			}
			case SOCIETYROLE_EMOTIONAL_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Empathetic online friend", "This person is known for their ability to understand and empathize with others, even through digital communication. They provide a safe and supportive space for others to share their thoughts and feelings.");
                list.Add().Set("Digital influencer", "This person has a large following on social media and is known for their emotional and relatable content. They often offer advice and support to their followers, and have a strong influence on their online community.");
                list.Add().Set("Online counselor or therapist", "This person offers virtual therapy or counseling sessions, providing emotional support and guidance to individuals who may not have access to traditional therapy. They utilize digital communication tools to connect with their clients and help them work through their emotions.");
                list.Add().Set("Digital activist", "This person is passionate about social issues and uses the internet as a platform to raise awareness and advocate for change. They may engage in online discussions and organize digital campaigns to promote causes that are important to them.");
                list.Add().Set("Virtual mentor", "This person serves as a mentor to others through digital channels, offering guidance, advice, and support to those seeking personal or professional development. They leverage technology to connect with and inspire others.");
                list.Add().Set("Online friend with shared experiences", "This person has developed close relationships with others online due to shared experiences, such as dealing with mental health struggles or facing the same challenges. They provide emotional support and understanding to their online community.");
                list.Add().Set("Digital pen pal", "This person connects with others through email or messaging and shares their thoughts and emotions through written communication. They may form deep and meaningful friendships with people from different backgrounds and cultures.");
                list.Add().Set("Virtual counselor", "This person offers emotional support and guidance through chat or video sessions, serving as a listening ear and offering coping strategies for individuals dealing with emotional distress.");
                list.Add().Set("Digital confidant", "This person is trusted by others to keep their personal thoughts and feelings confidential, and offers a non-judgmental and understanding ear for individuals who may not have anyone to confide in.");
                list.Add().Set("Online support group moderator", "This person manages and facilitates an online support group for individuals facing a specific challenge or issue. They provide emotional support and resources for members, and create a sense of community through digital connections.");				break;
			}
			case SOCIETYROLE_CRITICAL_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Cybersecurity expert", "This critical person uses their knowledge and skills to protect individuals and organizations from online threats and attacks. They may also educate the public about safe online practices.");
                list.Add().Set("Social media influencer", "This person wields a significant influence over their followers on various social media platforms, and their behavior online can greatly impact their followers' behavior as well.");
                list.Add().Set("Online activist", "This individual uses the internet as a tool for promoting social and political causes, and their online behavior reflects their strong beliefs and values.");
                list.Add().Set("Online troll", "This individual deliberately engages in provocative and offensive behavior online in order to elicit a reaction from others. They may target specific individuals or groups and often hide behind anonymity.");
                list.Add().Set("Digital parenting expert", "This person specializes in advising parents on how to navigate their children's digital lives and promote responsible online behavior.");
                list.Add().Set("Online bully", "This individual uses the internet to harass, intimidate, or harm others. Their behavior can have serious consequences for their victims and may escalate to cyberbullying.");
                list.Add().Set("Online addiction specialist", "This expert helps individuals recognize and overcome their harmful compulsive behaviors online, such as excessive social media use or internet addiction.");
                list.Add().Set("Cyber law expert", "This person is well-versed in the laws and regulations surrounding the internet and advises individuals and organizations on how to navigate legal issues related to online behavior.");
                list.Add().Set("Online therapist", "This mental health professional offers therapy sessions through online platforms, allowing individuals to receive support and guidance for their mental and emotional well-being in the digital sphere.");
                list.Add().Set("Digital detox coach", "This person helps individuals disconnect from the online world and establish healthy boundaries with technology, promoting a more mindful and balanced approach to online behavior.");
				break;
			}
			case SOCIETYROLE_MOTIVATIONAL_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Influencer", "This person has a large following on social media and uses their platform to inspire and motivate young people. They may share personal stories, positive messages, and offer advice and guidance on various topics such as self-confidence, goal-setting, and pursuing passions.");
                list.Add().Set("Life coach", "This person offers online coaching services to help young people find direction, set goals, and overcome obstacles. They may use a combination of personalized support and motivational techniques to help their clients achieve their full potential.");
                list.Add().Set("Fitness enthusiast", "This person promotes a healthy and active lifestyle through their online presence, encouraging young people to prioritize their physical and mental well-being. They may share workout routines, healthy recipes, and tips for self-care.");
                list.Add().Set("Mental health advocate", "This person uses their online presence to spread awareness and break stigmas surrounding mental health. They may offer resources, support, and positive messages to empower young people to take care of their emotional and mental well-being.");
                list.Add().Set("Entrepreneur", "This person shares their journey as a successful entrepreneur on social media, offering motivation and inspiration to young people who aspire to start their own business ventures. They may also provide advice and share their expertise in their particular industry.");
                list.Add().Set("Activist", "This person uses their platform to raise awareness and advocate for social causes that impact young people. They may share educational resources, organize events, and offer ways for their followers to get involved and make a difference.");
                list.Add().Set("Motivational speaker", "This person may offer virtual talks and workshops on various topics such as personal growth, goal-setting, and finding purpose. They may share their own experiences and insights to inspire and motivate young people to live their best lives.");
                list.Add().Set("Content creator", "This person produces motivational and uplifting content, such as videos, podcasts, or blogs, aimed at inspiring and empowering young people. They may use a combination of personal stories, humor, and positive messaging to connect with their audience.");
                list.Add().Set("Educator", "This person uses their online platform to share lessons and resources that can help young people succeed academically and in life. They may offer study tips, career advice, or share their expertise on a particular subject.");
                list.Add().Set("Role model", "This person leads by example and uses their online presence to show young people how to be kind, confident, and resilient individuals. They may use their own experiences and positive attitude to inspire others to be their best selves.");
				break;
			}
			case SOCIETYROLE_SUPPORTIVE_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Mental health blogger", "This person uses their platform on the internet to share insights, tips, and personal experiences related to mental health. They may also create a community for individuals to connect and support each other.");
                list.Add().Set("Online therapist", "This professional provides therapy sessions through virtual platforms for those seeking mental health support from the comfort of their own home.");
                list.Add().Set("Social media influencer", "This person utilizes their large following on social media to spread awareness and reduce stigma surrounding mental health. They may also share resources and tips for maintaining mental well-being.");
                list.Add().Set("Support group moderator/leader", "This person creates and moderates a support group on the internet for individuals dealing with similar mental health challenges. They may also facilitate discussions, provide resources, and offer a non-judgmental space for members to share their experiences.");
                list.Add().Set("Mental health advocate", "This person uses their online presence to advocate for mental health awareness, policies, and resources. They may also partner with organizations and participate in campaigns to promote mental health initiatives.");
                list.Add().Set("Positive influencer", "This person uses their positive and uplifting content on the internet to spread positivity and promote self-care practices for mental well-being.");
                list.Add().Set("Mental health educator", "This person uses their knowledge and expertise in mental health to educate others, through online courses, workshops, or informational posts. They may also offer personalized advice and support to those seeking guidance.");
                list.Add().Set("Online mentor/coach", "This person offers one-on-one support and guidance to individuals struggling with their mental health, through virtual coaching sessions or mentoring programs.");
                list.Add().Set("Virtual peer support", "This is a virtual platform or forum where individuals can connect with and receive support from peers who have experienced similar mental health challenges. This can provide a sense of community and reduce feelings of isolation.");
                list.Add().Set("Mental health hotline/chat operator", "This person provides support and guidance to individuals in crisis through a hotline or online chat service. They may also provide referrals to mental health resources and services.");
				break;
			}
			case SOCIETYROLE_ANXIOUS_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Self-diagnosing and self-medicating", "This person may spend a lot of time researching their symptoms and seeking out treatments or remedies without seeking professional help. This can lead to misinformation and potentially harmful self-treatment.");
                list.Add().Set("Avoidant of seeking help", "This person may recognize their anxiety but is hesitant or resistant to seeking help from a therapist or doctor. This can be due to feelings of shame or fear of judgment.");
                list.Add().Set("Somatizing anxiety", "This person may experience physical symptoms such as headaches, stomach aches, or muscle tension as a manifestation of their anxiety. They may need help recognizing the connection between their physical symptoms and their anxiety.");
                list.Add().Set("Perfectionism and anxiety", "This person may have high standards for themselves and struggle with perfectionism, leading to heightened levels of anxiety. They may need help learning to embrace imperfection and manage their expectations.");
                list.Add().Set("Social media comparison", "This person may constantly compare themselves to others on social media, leading to feelings of inadequacy and anxiety. They may benefit from taking breaks from social media and focusing on their own self-worth.");
                list.Add().Set("Fear of judgment", "This person may have a fear of being judged or negatively evaluated by others, leading to heightened social anxiety. They may need support in building self-confidence and developing coping strategies.");
                list.Add().Set("Panic attacks", "This person may experience sudden and intense panic attacks, often triggered by specific situations or thoughts. They may benefit from learning relaxation techniques and establishing a safety plan for when they experience a panic attack.");
                list.Add().Set("Avoidance and procrastination", "This person may struggle with avoidance and procrastination as a way of coping with their anxiety. They may need help in breaking down tasks and developing a plan to manage their feelings of overwhelm.");
                list.Add().Set("Comorbidity with depression", "This person may experience both anxiety and depression, and they can often feed into each other. They may benefit from a treatment plan that addresses both conditions.");
                list.Add().Set("Medication shaming", "This person may feel stigmatized or ashamed for taking medication to manage their anxiety. They may need support in understanding that medication can be a valuable tool for managing their symptoms.");
				break;
			}
			case SOCIETYROLE_HUMOROUS_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Relatable comedian", "This person uses everyday experiences and observations to create relatable and hilarious content. They often use self-deprecating humor to connect with their audience.");
                list.Add().Set("Satirist", "This person uses humor to critique and comment on societal issues and norms. They often employ irony and sarcasm to make their point.");
                list.Add().Set("Prankster", "This person's content is centered around pulling pranks and practical jokes on others. They may also capture people's reactions to their pranks.");
                list.Add().Set("Memelord", "This person is known for creating and sharing memes, often using popular culture references and inside jokes. They have a knack for finding humor in current events and trends.");
                list.Add().Set("Stand-up comedian", "This person's content consists of footage from their stand-up comedy shows, where they perform jokes and stories in front of a live audience. They often have a distinct stage presence and delivery style.");
                list.Add().Set("Sketch comedian", "This person creates scripted sketches and skits, often with a group of collaborators. They may parody popular TV shows, movies, or cultural phenomena.");
                list.Add().Set("Vlogger", "This person's humor is incorporated into their daily vlogs, where they share their thoughts and experiences in a funny and entertaining way. They may also incorporate challenges, pranks, and other lighthearted content into their vlogs.");
                list.Add().Set("Improv comedian", "This person's content is spontaneous and unscripted, relying on their skills in improvisation to create humor on the spot. They may also involve audience participation in their videos.");
                list.Add().Set("Parody artist", "This person creates comedic versions of popular songs, movies, or TV shows. They may use clever wordplay and over-the-top performances to add humor to their parodies.");
                list.Add().Set("Cartoonist", "This person uses illustrations and comics to convey their humorous take on life. They may use clever and witty captions to accompany their drawings.");
				break;
			}
			case SOCIETYROLE_DEFENSIVE_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Cyberbully", "This person uses the internet as a means to harass, intimidate, or bully others. They may target specific individuals or engage in trolling behavior, causing harm and distress to their victims.");
                list.Add().Set("Keyboard warrior", "Similar to a cyberbully, this person engages in aggressive and confrontational behavior online, often without consequence due to the anonymity of the internet. They may use inflammatory language and attack others in online discussions.");
                list.Add().Set("Scammer", "This person takes advantage of others through deceit and manipulation on the internet. They may create false identities or use fraudulent tactics to obtain personal information or money from unsuspecting victims.");
                list.Add().Set("Online vigilante", "This person takes it upon themselves to enforce their own version of justice on the internet, often targeting individuals or groups they disagree with or perceive as threats. They may engage in doxxing or other forms of online harassment.");
                list.Add().Set("Passive-aggressive internet user", "This person uses subtle, indirect ways to express their negative feelings or grievances towards others on the internet. This may manifest in snide comments, backhanded compliments, or vague posts.");
                list.Add().Set("Cyberstalker", "This person fixates on someone else online and relentlessly pursues them, often causing fear and discomfort for their victim. They may use different online platforms to monitor their victim's activity and gather personal information.");
                list.Add().Set("Trolling influencer", "This person uses their online influence and large following to stir up controversy and provoke reactions from others. They may make controversial statements or engage in inflammatory behavior in order to gain attention and increase their online presence.");
                list.Add().Set("Toxic commenter", "This person constantly leaves negative and critical comments on other people's posts or content. They may have a habit of spreading negativity and causing conflict on various online platforms.");
                list.Add().Set("Online addict", "This person has an unhealthy obsession with the internet and may struggle to disconnect or limit their usage. They may prioritize their online interactions over real-life relationships and responsibilities.");
                list.Add().Set("Cyberphobe", "This person avoids using the internet and participating in online interactions due to fear or distrust of the internet and online communities. They may have had negative experiences or perceive the internet as a dangerous place.");
				break;
			}
			case SOCIETYROLE_OVERWHELMED_PERSON_IN_THE_INTERNET: {
                list.Add().Set("Perfectionist", "This person feels overwhelmed by their own high expectations and may struggle to cope when things don't go according to plan. They may benefit from learning to let go of perfectionism and adopt a more flexible mindset.");
                list.Add().Set("Overworked professional", "This person is overwhelmed by their workload and may struggle to find a work-life balance. They may benefit from setting boundaries, prioritizing self-care, and seeking support from colleagues or a therapist.");
                list.Add().Set("Procrastinator", "This person may feel overwhelmed by their mounting to-do list and constantly putting things off. They may benefit from breaking tasks into smaller, manageable chunks and learning effective time management techniques.");
                list.Add().Set("Social media addict", "This person may feel overwhelmed by constantly comparing their life to others and feeling the pressure to constantly be connected. They may benefit from creating boundaries around social media usage and engaging in offline activities.");
                list.Add().Set("New parent", "This person may feel overwhelmed by the demands of caring for a new baby and adjusting to a major life change. They may benefit from seeking support from other parents, taking time for self-care, and setting realistic expectations for themselves.");
                list.Add().Set("Caregiver", "This person may feel overwhelmed by the demands of caring for a loved one and may struggle to prioritize their own needs. They may benefit from seeking respite care, asking for help from family and friends, and practicing self-compassion.");
                list.Add().Set("Student", "This person may feel overwhelmed by academic pressure and the expectations of their peers and family. They may benefit from seeking support from professors or a counselor, setting realistic goals, and engaging in stress-reducing activities.");
                list.Add().Set("Recent divorcee", "This person may feel overwhelmed by the emotional toll of a divorce and adjusting to a new life on their own. They may benefit from seeking therapy, joining a support group, and finding new hobbies or interests.");
                list.Add().Set("Chronic illness sufferer", "This person may feel overwhelmed by the physical and emotional toll of managing their health condition. They may benefit from joining a support group, seeking professional help, and practicing self-care and stress management techniques.");
                list.Add().Set("Recent trauma survivor", "This person may feel overwhelmed by the aftermath of a traumatic event and may struggle to cope with their emotions. They may benefit from seeking therapy, attending support groups, and finding healthy outlets for their emotions such as journaling or art.");
				break;
			}
			case SOCIETYROLE_NOSTALGIC_PERSON_IN_THE_INTERNET: {
                list.Add().Set("The constant reminiscer", "This person regularly posts throwback photos and memories on social media, often with a sense of longing for the past. They may also share old songs or movies that hold a special place in their heart.");
                list.Add().Set("The curator", "This person carefully curates their online presence to showcase a nostalgic aesthetic, often using filters or edits to create a retro feel. They may also seek out and share vintage items, fashion, or media online.");
                list.Add().Set("The researcher", "This person has a deep interest in history and often spends time on the internet researching past events, trends, and cultural moments. They may also follow and engage with historical accounts or archives online.");
                list.Add().Set("The millennial nostalgiac", "This person is part of the millennial generation and often reflects on their childhood and teenage years through social media posts and online conversations. They may also gravitate towards nostalgia-themed memes and content.");
                list.Add().Set("The nostalgist influencer", "This person has built a following by sharing nostalgic content and experiences, often with a sense of good-natured humor. They may also collaborate with brands to promote retro or vintage products.");
                list.Add().Set("The escapists", "These individuals use nostalgia to escape from present realities and often consume media or engage with online communities that transport them back to a simpler time. They may also struggle with feelings of anxiety or disillusionment in the present.");
                list.Add().Set("The collector", "This person is passionate about collecting and preserving items from their past or from a particular era. They may share their collections on social media and connect with others who share similar interests.");
                list.Add().Set("The truth-seeker", "This person is motivated by a desire to uncover the truth behind popular nostalgic narratives and may actively engage in debates and discussions online about the accuracy of depictions of the past.");
                list.Add().Set("The critic", "This person is critical of the widespread romanticization of nostalgia and may use their online platform to challenge the idea that the past was always better. They may also call attention to problematic elements of nostalgic media or trends.");
                list.Add().Set("The community-builder", "This person uses their nostalgic interests and experiences to create a sense of community online, connecting with others who share their affection for a particular time period, style, or nostalgia-related topic.");
				break;
			}
			case SOCIETYROLE_OBJECTIVE_PERSON_IN_THE_INTERNET: {
                list.Add().Set("News journalist", "These individuals research and report on current events and issues without any personal bias. They strive to present all sides of a story and provide factual information to the public.");
                list.Add().Set("Fact-checker", "Fact-checkers are responsible for verifying the accuracy of information presented in various media outlets. They often work for news organizations or independent platforms and aim to provide unbiased and verifiable information.");
                list.Add().Set("Wikipedia editor", "Wikipedia relies on volunteer editors to ensure the accuracy and neutrality of information on its platform. These editors strive to maintain a neutral point of view and fact-check information before publishing it.");
                list.Add().Set("Academic researcher", "Researchers in various fields are expected to present their findings and conclusions based on unbiased data and evidence. Their work undergoes scrutiny and peer-review to ensure objectivity.");
                list.Add().Set("Consumer reviews writer", "Many websites and platforms rely on consumer reviews to provide information about products and services. These writers aim to provide unbiased and honest opinions to help readers make informed decisions.");
                list.Add().Set("Moderator on online forums or discussion boards", "Moderators have the responsibility of facilitating discussions on online platforms and ensuring that the conversation is respectful, factual, and productive. They often need to intervene to prevent biased or false information from being shared.");
                list.Add().Set("Fact-based bloggers", "There are bloggers and content creators who focus on providing fact-based information and debunking false claims. They often thoroughly research their topics and provide sources for their information to support their claims.");
                list.Add().Set("Science communicator", "Science communicators aim to present scientific information in an engaging and easily understandable way without introducing personal biases. They often work for science magazines, documentaries, or independent platforms and aim to educate the public about various scientific topics.");
                list.Add().Set("Book researcher", "Book researchers gather information from various sources to fact-check information presented in books and ensure their accuracy. They may also collaborate with authors to provide additional resources and references for their work.");
                list.Add().Set("Open data advocate", "These individuals work to promote the use of open data and access to facts and statistics. They aim to increase transparency and accountability by making unbiased information easily accessible to the public.");
				break;
			}
			default: TODO
		}
	}
	return list;
}


const char* GetPlatformProfileEnum(int i) {
	switch (i) {
		#define PLATFORM_PROFILE(x) case PLATFORM_PROFILE_##x: return #x;
		PLATFORM_PROFILE_LIST
		#undef PLATFORM_PROFILE
		default: return "error";
	}
}

String GetPlatformProfileKey(int i) {return KeyToName(GetPlatformProfileEnum(i));}




const char* GetPlatformAttrEnum(int i) {
	switch (i) {
		#define PLATFORM_ATTR(x) case PLATFORM_ATTR_##x: return #x;
		PLATFORM_ATTR_LIST
		#undef PLATFORM_ATTR
		default: return "error";
	}
}

String GetPlatformAttrKey(int i) {return KeyToName(GetPlatformAttrEnum(i));}

String GetPlatformDescriptionModeKey(int i) {
	switch (i) {
		case PLATDESC_MODE_FINAL: return "engf";
		case PLATDESC_MODE_FINAL_DIALECT: return "engfd";
		case PLATDESC_MODE_FINAL_TRANSLATED: return "transf";
		case PLATDESC_MODE_FINAL_TRANSLATED_DIALECT: return "transfd";
		default: TODO; return "error";
	}
}

String GetPlatformDescriptionLengthKey(int i) {
	switch (i) {
		case PLATDESC_LEN_FULL: return "full";
		case PLATDESC_LEN_1280_CHARS: return "1280";
		case PLATDESC_LEN_160_CHARS: return "160";
		case PLATDESC_LEN_40_CHARS: return "40";
		default: TODO; return "error";
	}
}

int GetPlatformDescriptionLength(int i) {
	switch (i) {
		case PLATDESC_LEN_FULL: return 0;
		case PLATDESC_LEN_1280_CHARS: return 1280;
		case PLATDESC_LEN_160_CHARS: return 160;
		case PLATDESC_LEN_40_CHARS: return 40;
		default: TODO; return 0;
	}
}

int GetLanguageCount() {return LNG_COUNT;}

const char* GetLanguageKey(int i) {
	switch (i) {
		case LNG_NATIVE: return "dataset native";
		case LNG_ENGLISH: return "english";
		case LNG_FINNISH: return "finnish";
		case LNG_SPANISH: return "spanish";
		case LNG_PORTUGUESE: return "portuguese";
		case LNG_KOREAN: return "korean";
		case LNG_JAPANESE: return "japanese";
		case LNG_RUSSIAN: return "russian";
		case LNG_CHINESE: return "chinese";
		default: return "";
	}
}

const char* GetLanguageCode(int i) {
	switch (i) {
		case LNG_NATIVE: return "";
		case LNG_ENGLISH: return "EN-US";
		case LNG_FINNISH: return "FI-FI";
		case LNG_SPANISH: return "ES-ES";
		case LNG_PORTUGUESE: return "PT-PT";
		case LNG_KOREAN: return "KO-KO";
		case LNG_JAPANESE: return "JA-JA";
		case LNG_RUSSIAN: return "RU-RU";
		case LNG_CHINESE: return "ZH-SG";
		default: return "";
	}
}

String GetSnapshotAnalysisKey(int i) {
	switch (i) {
		case SNAPANAL_LYRICS_SUMMARY: return "Lyrics summary";
		case SNAPANAL_LYRICS_PSYCHOANALYSIS: return "Lyrics psychoanalysis";
		case SNAPANAL_LYRICS_SOCIAL_PSYCHOLOGY_ANALYSIS: return "Lyrics social psychology analysis";
		case SNAPANAL_MARKET_VALUE_ANALYSIS: return "Market value analysis";
		case SNAPANAL_MARKETING_SUGGESTION: return "Marketing suggestion";
		case SNAPANAL_ART_SUGGESTION: return "Art suggestion";
		case SNAPANAL_COVER_SUGGESTION: return "Cover image suggestion";
		default: return "<error>";
	}
}


void ContentType::Visit(Vis& v) {
	v.Ver(1)
	(1)	("key",key)
		("p0", parts[0])
		("p1", parts[1])
		("p2", parts[2]);
}

END_UPP_NAMESPACE
