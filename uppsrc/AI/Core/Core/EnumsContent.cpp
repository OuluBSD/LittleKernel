#include "Core.h"

// TODO: these all must be moved to a user file


NAMESPACE_UPP


const Index<String>& GetTypecasts();
int GetTypecastCount();

const Index<String>& GetProfiles();
int GetProfileCount();

const Index<String>& GetPrimary();
int GetPrimaryCount();

const Index<String>& GetSecondary();
int GetSecondaryCount();


const VectorMap<String,String>& GetContents();
int GetContentCount();

const Vector<ContentType>& GetContrasts();
const Vector<ContentType>& GetGenerics();
const Index<String>& GetRoles();

VectorMap<String,Vector<String>>& GetTypecastSingersMale();
VectorMap<String,Vector<String>>& GetTypecastSingersFemale();
VectorMap<String,Vector<String>>& GetTypecastRappersMale();
VectorMap<String,Vector<String>>& GetTypecastRappersFemale();
VectorMap<String,Vector<String>>& GetTypecastSingers(bool gender);
VectorMap<String,Vector<String>>& GetTypecastRappers(bool gender);
VectorMap<String,Vector<String>>& GetTypecastArtists(bool rapper, bool gender);
VectorMap<String,Vector<String>>& GetRoleCompanies(bool unsafe, bool gender);


const Vector<String>& GetContrastParts();


const Index<String>& GetPersonas();
const Vector<ContentType>& GetNiches();
int GetNicheCount();
VectorMap<String,Vector<String>>& GetPersonaSafe(bool gender);
VectorMap<String,Vector<String>>& GetPersonaUnsafe(bool gender);
VectorMap<String,Vector<String>>& GetPersonaNiches(bool unsafe, bool gender);


const Index<String>& GetCharacters();
const Vector<ContentType>& GetTropes();
int GetTropeCount();
VectorMap<String,Vector<String>>& GetCharacterSafeMale();
VectorMap<String,Vector<String>>& GetCharacterSafeFemale();
VectorMap<String,Vector<String>>& GetCharacterUnsafeMale();
VectorMap<String,Vector<String>>& GetCharacterUnsafeFemale();
VectorMap<String,Vector<String>>& GetCharacterSafe(bool gender);
VectorMap<String,Vector<String>>& GetCharacterUnsafe(bool gender);
VectorMap<String,Vector<String>>& GetCharacterTropes(bool unsafe, bool gender);



const Index<String>& GetStyles();
const Vector<ContentType>& GetApproaches();
int GetApproachCount();

VectorMap<String,Vector<String>>& GetStyleSafeMale();
VectorMap<String,Vector<String>>& GetStyleSafeFemale();
VectorMap<String,Vector<String>>& GetStyleUnsafeMale();
VectorMap<String,Vector<String>>& GetStyleUnsafeFemale();
VectorMap<String,Vector<String>>& GetStyleSafe(bool gender);
VectorMap<String,Vector<String>>& GetStyleUnsafe(bool gender);
VectorMap<String,Vector<String>>& GetStyleApproaches(bool unsafe, bool gender);




const Index<String>& GetPersuasiveElements();
const Vector<ContentType>& GetCallToActions();
int GetCallToActionCount();

VectorMap<String,Vector<String>>& GetTriggerSafeMale();
VectorMap<String,Vector<String>>& GetTriggerSafeFemale();
VectorMap<String,Vector<String>>& GetTriggerUnsafeMale();
VectorMap<String,Vector<String>>& GetTriggerUnsafeFemale();
VectorMap<String,Vector<String>>& GetTriggerSafe(bool gender);
VectorMap<String,Vector<String>>& GetTriggerUnsafe(bool gender);
VectorMap<String,Vector<String>>& GetPersuasiveTriggers(bool unsafe, bool gender);



const Index<String>& GetProgramGenres();
const Vector<ContentType>& GetProgrammingApproaches();



int GetTypeclassCount(int appmode) {
	return GetTypeclasses(appmode).GetCount();
}

const Index<String>& GetTypeclasses(int appmode) {
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	switch (appmode) {
		case DB_SONG: return GetTypecasts();
		case DB_TWITTER: return GetRoles();
		case DB_BLOG: return GetPersonas();
		case DB_DIALOG: return GetCharacters();
		case DB_STORYBOARD: return GetStyles();
		case DB_CODE: return GetProgramGenres();
		default: break;
	}
	Panic("Invalid appmode");
	return Single<Index<String>>();
}

int GetContentCount(int appmode) {
	return GetContents(appmode).GetCount();
}

const Vector<ContentType>& GetContents(int appmode) {
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	switch (appmode) {
		case DB_SONG: return GetContrasts();
		case DB_TWITTER: return GetGenerics();
		case DB_BLOG: return GetNiches();
		case DB_DIALOG: return GetTropes();
		case DB_STORYBOARD: return GetApproaches();
		case DB_CODE: return GetProgrammingApproaches();
		default: break;
	}
	Panic("Invalid appmode");
	return Single<Vector<ContentType>>();
}

const Vector<String>& GetContentParts(int appmode) {
	thread_local static Vector<String> list_[DB_COUNT];
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	auto& list = list_[appmode];
	if (list.IsEmpty()) {
		const auto& v = GetContents(appmode);
		for(int i = 0; i < v.GetCount(); i++) {
			const auto& it = v[i];
			for(int j = 0; j < PART_COUNT; j++) {
				list.Add() = it.key + " #" + IntStr(j+1) + ": " + it.parts[j];
			}
		}
	}
	return list;
}

const Vector<String>& GetContrastParts() {
	return GetContentParts(DB_SONG);
}

VectorMap<String,Vector<String>>& GetTypeclassEntities(int appmode, bool unsafe, bool gender) {
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	switch (appmode) {
		case DB_SONG: return GetTypecastArtists(unsafe, gender);
		case DB_TWITTER: return GetRoleCompanies(unsafe, gender);
		case DB_BLOG: return GetPersonaNiches(unsafe, gender);
		case DB_DIALOG: return GetCharacterTropes(unsafe, gender);
		case DB_STORYBOARD: return GetStyleApproaches(unsafe, gender);
		case DB_CODE: break;
	}
	Panic("Invalid appmode");
	return Single<VectorMap<String,Vector<String>>>();
}















const Index<String>& GetTypecasts() {
	thread_local static Index<String> list;
	if (list.IsEmpty()) {
		list.Add("Heartbroken/lovesick");
		list.Add("Rebel/anti-establishment");
		list.Add("Political activist");
		list.Add("Social justice advocate");
		list.Add("Party/club");
		list.Add("Hopeful/dreamer");
		list.Add("Confident/empowered");
		list.Add("Vulnerable/raw");
		list.Add("Romantic/love-driven");
		list.Add("Failure/loser");
		list.Add("Spiritual/faithful");
		list.Add("Passionate/determined");
		list.Add("Reflective/self-reflective");
		list.Add("Witty/sarcastic");
		list.Add("Melancholic/sad");
		list.Add("Humble/down-to-earth");
		list.Add("Charismatic/charming");
		list.Add("Resilient/overcoming adversity");
		list.Add("Carefree/joyful");
		list.Add("Dark/mysterious");
		list.Add("Comical/humorous");
		list.Add("Controversial/provocative");
		list.Add("Nostalgic/sentimental");
		list.Add("Wise/philosophical");
		list.Add("Angry/outspoken");
		list.Add("Calm/peaceful.");
		list.Add("Confident/self-assured");
		list.Add("Self-destructive/self-sabotaging");
		list.Add("Hopeful/optimistic");
		list.Add("Fearful/anxious");
		list.Add("Eccentric/quirky");
		list.Add("Sensitive/emotional");
		list.Add("Bitter/resentful");
		list.Add("Unique/nonconformist");
		list.Add("Free-spirited/nonconformist");
		list.Add("Sultry/seductive");
		list.Add("Inspirational/motivational");
		list.Add("Authentic/real");
		list.Add("Mysterious/enigmatic");
		list.Add("Carefree/bohemian");
		list.Add("Street-smart/tough");
		list.Add("Romantic/idealistic");
		list.Add("Nurturing/motherly");
		list.Add("Dark/tormented");
		list.Add("Remorseful/regretful");
		list.Add("Bold/brave");
		list.Add("Outcast/rebel");
		list.Add("Lost/disconnected");
		list.Add("Tough/badass");
		list.Add("Sincere/genuine");
		list.Add("Honest/vulnerable");
		list.Add("Innocent/naive");
		list.Add("Bold/risk-taking");
	}
	return list;
}

int GetTypecastCount() {
	return GetTypecasts().GetCount();
}

const Index<String>& GetProfiles() {
	thread_local static Index<String> list;
	if (list.IsEmpty()) {
		// "singer is ..."
        list.Add("a third-party observer/commentator");
        list.Add("expressing personal emotions/thoughts");
        list.Add("conveying a message or lesson");
        list.Add("embodying a character or persona");
        list.Add("a storyteller or messenger for a community or culture");
        list.Add("reflecting on past experiences or memories");
        list.Add("interpreting or analyzing the scripts for the listener");
        list.Add("challenging societal norms or addressing social issues");
        list.Add("invoking a particular mood or atmosphere through vocals");
        list.Add("having a dialogue with another singer");
        list.Add("having a dialogue with the audience");
        list.Add("using abstract or poetic language to convey feelings or ideas");
        list.Add("highlighting the beauty or poeticism of the scripts through their performance");
        list.Add("asking questions and exploring different perspectives on the topic of the scripts");
        list.Add("using irony or satire to convey a message or make a statement");
        list.Add("evoking nostalgia or longing through the scripts");
        list.Add("using personal experiences to give depth and authenticity to the scripts ");
        list.Add("using humor or wit to engage with the scripts");
        list.Add("challenging the listener's perspective or beliefs with the scripts");
        list.Add("using vocal techniques or styles to add layers of meaning to the scripts");
        list.Add("creating a sense of intimacy or connection through their performance of the scripts");
        list.Add("breaking societal expectations and norms through their interpretation of the scripts");
        list.Add("offering a unique perspective on a commonly addressed topic in the scripts");
        list.Add("creating a soundtrack or anthem for a specific group or community");
        list.Add("using repetition or emphasis to emphasize the importance of the scripts");
        list.Add("using double entendres or wordplay to add depth and complexity to the scripts ");
        list.Add("reflecting on personal growth or transformation through the scripts");
        list.Add("embodying a specific emotion or feeling portrayed in the scripts");
        list.Add("representing a marginalized or underrepresented group through the scripts");
        list.Add("using imagery or metaphors to convey a deeper meaning behind the scripts");
        list.Add("expressing vulnerability or raw emotion through the scripts");
        list.Add("narrating a specific event or experience through the scripts");
        list.Add("using the scripts to convey a sense of empowerment or strength");
        list.Add("engaging in introspection and self-reflection through the scripts");
        list.Add("confronting personal demons or struggles in the scripts ");
        list.Add("using the scripts to express social or cultural commentary");
	}
	return list;
}

int GetProfileCount() {
	return GetProfiles().GetCount();
}





const Vector<ContentType>& GetContrasts() {
	thread_local static Vector<ContentType> list;
	if (list.IsEmpty()) {
		list.Add().Set("Seductive intro", "a seductive and sultry melody draws the listener in", "the scripts talk about a passionate and intense relationship", "the mood shifts as the singer realizes they are not truly in love");
		list.Add().Set("Rise and fall", "the beat builds and intensifies, creating a sense of excitement and anticipation", "the scripts tell a story of overcoming obstacles and achieving success", "the energy drops suddenly and the singer reflects on the sacrifices and struggles that came with their success");
		list.Add().Set("Fun and games", "a carefree and lively melody sets the tone for a carefree party anthem", "the scripts are about enjoying life and living in the moment", "the party comes to an end and the reality of responsibilities and consequences sink in");
		list.Add().Set("Love at first sight", "a romantic and dreamy melody introduces the concept of falling in love at first sight", "the scripts describe the intense feelings and desires that come with falling for someone instantly", "the singer wakes up from the fantasy and realizes");
		list.Add().Set("Struggle and triumph", "a slower and melancholic melody sets the scene for a character facing challenges and adversity", "the scripts depict the struggles and hardships they have faced", "the pace picks up and the music becomes more triumphant as the character overcomes their struggles and achieves success");
		list.Add().Set("Ups and downs", "a catchy and upbeat melody reflects the highs of a new relationship", "the scripts delve into the challenges and conflicts that arise within the relationship", "the music slows down as the couple try to work through their problems and find a resolution");
		list.Add().Set("Escape to paradise", "a tropical and laid-back beat transports the listener to a paradise destination", "the scripts describe a desire to escape from reality and find solace in a beautiful location", "the singer comes back to reality and faces the consequences of leaving everything behind");
		list.Add().Set("Rebellious spirit", "a rebellious and edgy guitar riff sets the rebellious tone of the song", "the scripts speak of breaking rules and societal expectations", "the song ends with the realization that rebellion can have consequences");
		list.Add().Set("Broken and mended", "a somber and melancholic melody reflects a heartbroken state", "the scripts describe the pain and sadness of a broken relationship", "the tone shifts as the singer begins to heal and move on from the heartbreak");
		list.Add().Set("Chase your dreams", "an uplifting and motivational melody encourages listeners to chase their dreams", "the scripts tell a story of overcoming obstacles and pursuing one's passions", "the song concludes with a sense of fulfillment and the realization that the journey towards achieving dreams is never-ending");
		list.Add().Set("Dark secrets", "a haunting and mysterious introduction sets the tone for secrets and deceit", "the scripts reveal dark secrets and hidden motives among the characters", "the song ends with a sense of betrayal and the consequences of keeping secrets");
		list.Add().Set("Rags to riches", "a humble and modest melody represents the beginnings of a character's journey", "the scripts describe the climb to success and wealth", "the music becomes more grandiose as the character achieves their dreams and reflects on their journey");
		list.Add().Set("Lost and found", "a haunting and melancholic melody portrays a sense of being lost and alone", "the scripts depict a journey of self-discovery and finding one's place in the world", "the music becomes more uplifting as the character finds a sense of belonging and purpose");
		list.Add().Set("Ignite the fire", "an energetic and intense beat sparks excitement and passion", "the scripts describe the power and intensity of a new love or passion", "the music dies down as the flame fades and the singer is left with the memories of the passion that once consumed them");
		list.Add().Set("From the ashes", "a slow and mournful melody sets the scene for a character who has hit rock bottom", "the scripts depict the struggles and hardships they have faced", "the music picks up as the character rises from the ashes and rebuilds their life" );
		list.Add().Set("Fame and fortune", "a flashy and upbeat melody represents the allure of fame and fortune", "the scripts describe the glamorous lifestyle and perks that come with success", "the song ends with a cautionary tale about the emptiness and pitfalls of a life solely focused on money and fame");
		list.Add().Set("Healing in the darkness", "a haunting and ethereal melody reflects a state of darkness and pain", "the scripts speak of finding light and healing in the darkest times", "the music builds to a triumphant and uplifting finale as the singer finds strength and hope in their struggles");
		list.Add().Set("City lights and lonely nights", "a bustling and energetic beat represents the excitement of the city at night", "the scripts tell a story of chasing dreams and living life to the fullest in the city", "the song ends with a sense of loneliness and longing for something more meaningful outside of the fast-paced city life");
		list.Add().Set("Breaking the mold", "a unique and unconventional melody sets the tone for breaking the norm", "the scripts describe defying expectations and being true to oneself", "the song ends with a sense of liberation and empowerment as the singer embraces their individuality");
		list.Add().Set("Haunted by the past", "a haunting and eerie melody reflects the weight of a character's past traumas", "the scripts delve into the pain and struggles of moving on from the past", "the music becomes more hopeful as the character learns to let go and move forward");
		list.Add().Set("Wild and free", "a carefree and adventurous melody embodies the thrill of living life on the edge", "the scripts describe the rush and excitement of taking risks and living in the moment", "the song concludes with a reminder that with freedom comes consequences and responsibilities");
		list.Add().Set("Clash of opinions", "a catchy and upbeat melody sets the tone for a heated argument", "the scripts depict conflicting opinions and viewpoints", "the song ends with the understanding that sometimes it's best to agree to disagree and move on" );
		list.Add().Set("Long distance love", "a soft and tender melody represents the longing and distance in a relationship", "the scripts tell a story of the struggles and sacrifices of maintaining a long distance love", "the song ends with a sense of hope and determination to make the relationship work");
		list.Add().Set("Finding inner strength", "a slow and contemplative melody represents a character facing inner struggles", "the scripts speak of finding courage and strength from within to overcome challenges", "the song crescendos as the singer embraces their inner strength and triumphs over their struggles");
		list.Add().Set("Living a double life", "a mysterious and seductive beat sets the stage for a character leading a secretive life", "the scripts tell the story of juggling two separate identities and the dangers that come with it", "the song concludes with the realization that living a lie is destructive and unsustainable");
		list.Add().Set("Caught in the spotlight", "a bright and flashy melody reflects the thrill of being in the spotlight", "the scripts depict the pressure and challenges of fame and constantly being in the public eye", "the music slows down as the singer reflects on the toll fame has taken on their personal life");
		list.Add().Set("Love and war", "a powerful and intense beat represents the passionate and tumultuous nature of love", "the scripts depict a couple's constant battle and struggle to make their relationship work", "the song ends with a bittersweet realization that love can be both beautiful and painful");
		list.Add().Set("The art of letting go", "a slow and somber melody sets the tone for learning to let go", "the scripts describe the struggles of moving on and leaving the past behind", "the music builds to a hopeful and empowering finale as the singer finally finds the strength to let go");
		list.Add().Set("Living in the moment", "an upbeat and carefree melody represents living life with no regrets", "the scripts encourage taking chances and embracing every moment", "the song ends with a reminder to cherish the present and not dwell on the past or worry about the future");
		list.Add().Set("Conquering fears", "a tense and ominous melody reflects the fear and anxiety a character faces", "the scripts speak of overcoming fears and finding courage to face them", "the music becomes triumphant and uplifting as the character conquers their fears and grows stronger" );
		/*list.Add().Set("Heart vs. Mind", "a gentle and emotional melody sets the stage for a character torn between their heart and their logical mind", "the scripts describe the internal struggle between following one's emotions and making rational decisions", "the song ends on a reflective note as the character finds a balance between their heart and mind");
		list.Add().Set("Surviving the storm", "a stormy and intense melody represents facing difficult and challenging times", "the scripts speak of resilience and perseverance through tough situations", "the music calms down as the singer finds strength and hope in surviving the storm");
		list.Add().Set("Living a lie", "a dark and deceptive melody reflects the deception in a character's life", "the scripts depict the consequences and chaos that come with living a lie", "the song ends with the realization that living a lie can destroy relationships and one's own sense of self");
		list.Add().Set("Forgotten memories", "a melancholic and haunting melody sets the scene for a character's forgotten memories", "the scripts unravel the mystery and pain behind the forgotten memories", "the music becomes reflective and haunting as the singer realizes the true impact of their forgotten memories on their life");
		list.Add().Set("Breaking free", "a powerful and empowering melody reflects the desire to break free from constraints and expectations", "the scripts describe the journey of breaking through obstacles and finding independence", "the song ends on a triumphant note as the character finally breaks free and finds freedom and happiness" );
		list.Add().Set("Fake love", "a catchy and upbeat melody represents the facade of a fake love", "the scripts reveal the deceit and manipulation in a fake relationship", "the song ends with the realization of the emptiness and pain of a love built on lies and false promises");
		list.Add().Set("Strangers in love", "a soft and romantic melody represents the initial attraction and connection between two strangers", "the scripts follow the journey of getting to know each other and falling in love", "the song ends with the uncertainty and fear of whether their love will last or if they were always destined to be strangers");
		list.Add().Set("Guy in club", "an upbeat and danceable melody sets the scene for a night out at the club", "the scripts depict the excitement and charm of a guy at the club", "the song ends on a reflective note as the singer realizes the emptiness and lack of substance in these interactions");
		list.Add().Set("Angels and demons", "a haunting and eerie melody sets the tone for the duality within a character", "the scripts describe the battle between good and evil within oneself", "the music becomes more chaotic as the character struggles to find balance between their angels and demons");
		list.Add().Set("Living on the edge", "a fast-paced and thrilling beat represents the risky and dangerous lifestyle of living on the edge", "the scripts depict the rush and adrenaline that comes with constantly pushing boundaries", "the song ends with a sense of exhaustion and questioning if the thrill is worth the consequences");*/
	}
	return list;
}

VectorMap<String,Vector<String>>& GetTypecastSingersFemale() {
	thread_local static VectorMap<String,Vector<String>> list;
	if (!list.IsEmpty()) return list;
	
	{auto& tc = list.Add("Heartbroken/lovesick");
	tc.Add("Adele");
	tc.Add("Sam Smith");
	tc.Add("Beyoncé");
	tc.Add("Selena Gomez");
	tc.Add("Halsey");}

	{auto& tc = list.Add("Rebel/anti-establishment");
	tc.Add("Billie Eilish");
	tc.Add("Demi Lovato");
	tc.Add("Miley Cyrus");
	tc.Add("Alanis Morissette");
	tc.Add("Gwen Stefani");}

	{auto& tc = list.Add("Political activist");
	tc.Add("Joan Baez");
	tc.Add("Nina Simone");
	tc.Add("Ani DiFranco");
	tc.Add("Melissa Etheridge");
	tc.Add("Patti Smith");}

	{auto& tc = list.Add("Social justice advocate");
	tc.Add("Alicia Keys");
	tc.Add("Janelle Monáe");
	tc.Add("Lauryn Hill");
	tc.Add("India.Arie");
	tc.Add("Tracy Chapman");}

	{auto& tc = list.Add("Party/club");
	tc.Add("Rihanna");
	tc.Add("Cardi B");
	tc.Add("Jennifer Lopez");
	tc.Add("Shakira");
	tc.Add("Nicki Minaj");}

	{auto& tc = list.Add("Hopeful/dreamer");
	tc.Add("Demi Lovato");
	tc.Add("Selena Gomez");
	tc.Add("Alessia Cara");}

	{auto& tc = list.Add("Confident/empowered");
	tc.Add("Beyoncé");
	tc.Add("Ariana Grande");
	tc.Add("Christina Aguilera");
	tc.Add("Lizzo");
	tc.Add("Meghan Trainor");}

	{auto& tc = list.Add("Vulnerable/raw");
	tc.Add("Adele");
	tc.Add("Lana Del Rey");
	tc.Add("Birdy");
	tc.Add("Lorde");
	tc.Add("Florence + The Machine");}

	{auto& tc = list.Add("Romantic/love-driven");
	tc.Add("Selena Gomez");
	tc.Add("Ariana Grande");
	tc.Add("Halsey");
	tc.Add("Camila Cabello");}

	{auto& tc = list.Add("Failure/loser");
	tc.Add("Lorde");
	tc.Add("Demi Lovato");
	tc.Add("Selena Gomez");
	tc.Add("Sia");
	tc.Add("Amy Winehouse");}

	{auto& tc = list.Add("Spiritual/faithful");
	tc.Add("Stevie Nicks");
	tc.Add("Florence Welch");
	tc.Add("Norah Jones");
	tc.Add("Tori Amos");
	tc.Add("Sarah McLachlan");}

	{auto& tc = list.Add("Passionate/determined");
	tc.Add("P!nk");
	tc.Add("Kelly Clarkson");
	tc.Add("Beyoncé");
	tc.Add("Alicia Keys");}

	{auto& tc = list.Add("Reflective/self-reflective");
	tc.Add("Lana Del Rey");
	tc.Add("Adele");
	tc.Add("Halsey");
	tc.Add("Billie Eilish");
	tc.Add("Sara Bareilles");}

	{auto& tc = list.Add("Witty/sarcastic");
	tc.Add("Alanis Morissette");
	tc.Add("Lily Allen");
	tc.Add("Pink");
	tc.Add("Avril Lavigne");
	tc.Add("Fergie");}

	{auto& tc = list.Add("Melancholic/sad");
	tc.Add("Lana Del Rey");
	tc.Add("Adele");
	tc.Add("Billie Eilish");
	tc.Add("Halsey");
	tc.Add("Lorde");}

	{auto& tc = list.Add("Humble/down-to-earth");
	tc.Add("Norah Jones");
	tc.Add("Colbie Caillat");
	tc.Add("Ingrid Michaelson");
	tc.Add("Sara Bareilles");
	tc.Add("religion of bankersel");}

	{auto& tc = list.Add("Charismatic/charming");
	tc.Add("Selena Gomez");
	tc.Add("Ariana Grande");
	tc.Add("Jennifer Lopez");
	tc.Add("Shakira");
	tc.Add("Beyoncé");}

	{auto& tc = list.Add("Resilient/overcoming adversity");
	tc.Add("Demi Lovato");
	tc.Add("Alicia Keys");
	tc.Add("Beyoncé");
	tc.Add("Christina Aguilera");}

	{auto& tc = list.Add("Carefree/joyful");
	tc.Add("Beyoncé");
	tc.Add("Selena Gomez");
	tc.Add("Alicia Keys");
	tc.Add("Sia");}

	{auto& tc = list.Add("Dark/mysterious");
	tc.Add("Lana Del Rey");
	tc.Add("Billie Eilish");
	tc.Add("Florence Welch");
	tc.Add("Lorde");
	tc.Add("Halsey");}

	{auto& tc = list.Add("Comical/humorous");
	tc.Add("Amy Schumer");
	tc.Add("Nicki Minaj");
	tc.Add("Dua Lipa");}

	{auto& tc = list.Add("Controversial/provocative");
	tc.Add("Madonna");
	tc.Add("Miley Cyrus");
	tc.Add("Nicki Minaj");
	tc.Add("Rihanna");}

	{auto& tc = list.Add("Nostalgic/sentimental");
	tc.Add("Lana Del Rey");
	tc.Add("Sara Bareilles");
	tc.Add("Adele");
	tc.Add("Norah Jones");
	tc.Add("Norah jones");}

	{auto& tc = list.Add("Wise/philosophical");
	tc.Add("Joni Mitchell");
	tc.Add("Stevie Nicks");
	tc.Add("Alanis Morissette");
	tc.Add("religion of bankersel");
	tc.Add("Patti Smith");}

	{auto& tc = list.Add("Angry/outspoken");
	tc.Add("P!nk");
	tc.Add("Lorde");
	tc.Add("Beyoncé");
	tc.Add("Ariana Grande");
	tc.Add("Avril Lavigne");}

	{auto& tc = list.Add("Calm/peaceful");
	tc.Add("Norah Jones");
	tc.Add("Sara Bareilles");
	tc.Add("Colbie Caillat");
	tc.Add("Alicia Keys");
	tc.Add("Birdy");}

	{auto& tc = list.Add("Confident/self-assured");
	tc.Add("Beyoncé");
	tc.Add("Ariana Grande");
	tc.Add("Christina Aguilera");
	tc.Add("Rihanna");
	tc.Add("Adele");}
	
	{auto& tc = list.Add("Self-destructive/self-sabotaging");
	tc.Add("Amy Winehouse");
	tc.Add("Lana Del Rey");
	tc.Add("Sia");
	tc.Add("Demi Lovato");
	tc.Add("Halsey");}

	{auto& tc = list.Add("Hopeful/optimistic");
	tc.Add("Ariana Grande");
	tc.Add("Selena Gomez");
	tc.Add("Kelly Clarkson");
	tc.Add("Mariah Carey");}

	{auto& tc = list.Add("Fearful/anxious");
	tc.Add("Halsey");
	tc.Add("Lana Del Rey");
	tc.Add("Alessia Cara");
	tc.Add("Sia");
	tc.Add("Adele");}

	{auto& tc = list.Add("Eccentric/quirky");
	tc.Add("Björk");
	tc.Add("Florence Welch");
	tc.Add("Sia");
	tc.Add("Grimes");}

	{auto& tc = list.Add("Sensitive/emotional");
	tc.Add("Adele");
	tc.Add("Beyoncé");
	tc.Add("Selena Gomez");
	tc.Add("Halsey");
	tc.Add("Norah Jones");}

	{auto& tc = list.Add("Bitter/resentful");
	tc.Add("Alanis Morissette");
	tc.Add("Lorde");
	tc.Add("Billie Eilish");
	tc.Add("Avril Lavigne");
	tc.Add("Lana Del Rey");}

	{auto& tc = list.Add("Unique/nonconformist");
	tc.Add("Alanis Morissette");
	tc.Add("P!nk");
	tc.Add("Björk");
	tc.Add("Halsey");}

	{auto& tc = list.Add("Free-spirited/nonconformist");
	tc.Add("Miley Cyrus");
	tc.Add("Jimi Hendrix");
	tc.Add("Janis Joplin");
	tc.Add("Pink Floyd");
	tc.Add("Led Zeppelin");}

	{auto& tc = list.Add("Sultry/seductive");
	tc.Add("Lana Del Rey");
	tc.Add("Beyoncé");
	tc.Add("Rihanna");
	tc.Add("Madonna");
	tc.Add("Selena Gomez");}

	{auto& tc = list.Add("Inspirational/motivational");
	tc.Add("Demi Lovato");
	tc.Add("Alicia Keys");
	tc.Add("Christina Aguilera");
	tc.Add("Beyoncé");}

	{auto& tc = list.Add("Authentic/real");
	tc.Add("Adele");
	tc.Add("Pink");
	tc.Add("Beyoncé");
	tc.Add("Alanis Morissette");
	tc.Add("Ariana Grande");}

	{auto& tc = list.Add("Mysterious/enigmatic");
	tc.Add("Lana Del Rey");
	tc.Add("Lorde");
	tc.Add("Billie Eilish");
	tc.Add("Sia");
	tc.Add("Florence Welch");}

	{auto& tc = list.Add("Carefree/bohemian");
	tc.Add("Florence Welch");
	tc.Add("Lana Del Rey");
	tc.Add("Norah Jones");
	tc.Add("Sia");
	tc.Add("Stevie Nicks");}

	{auto& tc = list.Add("Street-smart/tough");
	tc.Add("Cardi B");
	tc.Add("Beyoncé");
	tc.Add("Nicki Minaj");
	tc.Add("Rihanna");
	tc.Add("Missy Elliott");}

	{auto& tc = list.Add("Romantic/idealistic");
	tc.Add("Ariana Grande");
	tc.Add("Beyoncé");
	tc.Add("Selena Gomez");
	tc.Add("Lana Del Rey");}

	{auto& tc = list.Add("Nurturing/motherly");
	tc.Add("Dolly Parton");
	tc.Add("Shania Twain");
	tc.Add("Adele");
	tc.Add("Norah Jones");
	tc.Add("Christina Aguilera");}

	{auto& tc = list.Add("Dark/tormented");
	tc.Add("Lana Del Rey");
	tc.Add("Billie Eilish");
	tc.Add("Lorde");
	tc.Add("Halsey");
	tc.Add("Adele");}

	{auto& tc = list.Add("Remorseful/regretful");
	tc.Add("Adele");
	tc.Add("Beyoncé");
	tc.Add("Pink");
	tc.Add("Alanis Morissette");
	tc.Add("Lana Del Rey");}

	{auto& tc = list.Add("Bold/brave");
	tc.Add("P!nk");
	tc.Add("Beyoncé");
	tc.Add("Demi Lovato");}

	{auto& tc = list.Add("Outcast/rebel");
	tc.Add("Billie Eilish");
	tc.Add("Demi Lovato");
	tc.Add("Miley Cyrus");
	tc.Add("Alanis Morissette");
	tc.Add("Beyoncé");}

	{auto& tc = list.Add("Lost/disconnected");
	tc.Add("Lana Del Rey");
	tc.Add("Adele");
	tc.Add("Florence + The Machine");
	tc.Add("Halsey");
	tc.Add("Lorde");}

	{auto& tc = list.Add("Tough/badass");
	tc.Add("Beyoncé");
	tc.Add("Rihanna");
	tc.Add("P!nk");
	tc.Add("Madonna");
	tc.Add("Cardi B");}

	{auto& tc = list.Add("Sincere/genuine");
	tc.Add("Adele");
	tc.Add("Kelly Clarkson");
	tc.Add("Sara Bareilles");
	tc.Add("Norah Jones");
	tc.Add("Christina Aguilera");}

	{auto& tc = list.Add("Honest/vulnerable");
	tc.Add("Adele");
	tc.Add("Halsey");
	tc.Add("Lana Del Rey");
	tc.Add("Lorde");
	tc.Add("Sia");}

	{auto& tc = list.Add("Innocent/naive");
	tc.Add("Selena Gomez");
	tc.Add("Ariana Grande");
	tc.Add("Demi Lovato");}

	{auto& tc = list.Add("Bold/risk-taking");
	tc.Add("Pink");
	tc.Add("Madonna");
	tc.Add("Beyoncé");
	tc.Add("Miley Cyrus");}
	
	ASSERT(list.GetCount() == GetTypecastCount());
	return list;
}

VectorMap<String,Vector<String>>& GetTypecastRappersFemale() {
	thread_local static VectorMap<String,Vector<String>> list;
	if (!list.IsEmpty()) return list;
	

	{auto& tc = list.Add("Heartbroken/lovesick");
	tc.Add("Missy Elliott");
	tc.Add("Lauryn Hill");
	tc.Add("Queen Latifah");
	tc.Add("Aaliyah");
	tc.Add("Lil Kim");
	tc.Add("Eve");}

	{auto& tc = list.Add("Rebel/anti-establishment");
	tc.Add("Cardi B");
	tc.Add("Nicki Minaj");
	tc.Add("Lauryn Hill");
	tc.Add("Queen Latifah");}

	{auto& tc = list.Add("Political activist");
	tc.Add("Lauryn Hill");
	tc.Add("Queen Latifah");
	tc.Add("Eve");
	tc.Add("Dead Prez");
	tc.Add("Angel Haze");}

	{auto& tc = list.Add("Social justice advocate");
	tc.Add("Lauryn Hill");
	tc.Add("Queen Latifah");
	tc.Add("Angel Haze");
	tc.Add("Rapsody");
	tc.Add("Noname");
	tc.Add("Jean Grae");}

	{auto& tc = list.Add("Party/club");
	tc.Add("Missy Elliott");
	tc.Add("Cardi B");
	tc.Add("Nicki Minaj");
	tc.Add("Lil Kim");
	tc.Add("Salt-N-Pepa");
	tc.Add("Eve");
	tc.Add("Megan Thee Stallion");
	tc.Add("Da Brat");}

	{auto& tc = list.Add("Hopeful/dreamer");
	tc.Add("Lauryn Hill");
	tc.Add("Nicki Minaj");
	tc.Add("J. Cole");
	tc.Add("Rapsody");
	tc.Add("Lizzo");}

	{auto& tc = list.Add("Confident/empowered");
	tc.Add("Missy Elliott");
	tc.Add("Cardi B");
	tc.Add("Nicki Minaj");
	tc.Add("Queen Latifah");
	tc.Add("Lizzo");
	tc.Add("Eve");
	tc.Add("City Girls");}

	{auto& tc = list.Add("Vulnerable/raw");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Missy Elliott");
	tc.Add("Nicki Minaj");
	tc.Add("Angel Haze");}

	{auto& tc = list.Add("Romantic/love-driven");
	tc.Add("Cardi B");
	tc.Add("Nicki Minaj");
	tc.Add("Lauryn Hill");
	tc.Add("Lil Kim");
	tc.Add("Queen Latifah");
	tc.Add("Aaliyah");}

	{auto& tc = list.Add("Failure/loser");
	tc.Add("Missy Elliott");
	tc.Add("Cardi B");
	tc.Add("Nicki Minaj");
	tc.Add("Lauryn Hill");
	tc.Add("Lil Kim");}

	{auto& tc = list.Add("Spiritual/faithful");
	tc.Add("Lauryn Hill");
	tc.Add("Queen Latifah");
	tc.Add("Rapsody");
	tc.Add("Noname");
	tc.Add("Aaliyah");
	tc.Add("Lizzo");}

	{auto& tc = list.Add("Passionate/determined");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Angel Haze");
	tc.Add("Rapsody");
	tc.Add("Nicki Minaj");}

	{auto& tc = list.Add("Reflective/self-reflective");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Nicki Minaj");
	tc.Add("Rapsody");
	tc.Add("Angel Haze");
	tc.Add("Queen Latifah");}

	{auto& tc = list.Add("Witty/sarcastic");
	tc.Add("Missy Elliott");
	tc.Add("Cardi B");
	tc.Add("Nicki Minaj");
	tc.Add("Lil Kim");
	tc.Add("City Girls");}

	{auto& tc = list.Add("Melancholic/sad");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Nicki Minaj");
	tc.Add("Aaliyah");
	tc.Add("Lizzo");}

	{auto& tc = list.Add("Humble/down-to-earth");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Rapsody");
	tc.Add("Aaliyah");
	tc.Add("Megan Thee Stallion");}

	{auto& tc = list.Add("Charismatic/charming");
	tc.Add("Missy Elliott");
	tc.Add("Cardi B");
	tc.Add("Nicki Minaj");
	tc.Add("Lil Kim");
	tc.Add("Lauryn Hill");
	tc.Add("Rapsody");
	tc.Add("Aaliyah");
	tc.Add("Queen Latifah");}

	{auto& tc = list.Add("Resilient/overcoming adversity");
	tc.Add("Lauryn Hill");
	tc.Add("Rapsody");
	tc.Add("Megan Thee Stallion");
	tc.Add("Nicki Minaj");
	tc.Add("Lizzo");}

	{auto& tc = list.Add("Carefree/joyful");
	tc.Add("Missy Elliott");
	tc.Add("Cardi B");
	tc.Add("Nicki Minaj");
	tc.Add("Eve");
	tc.Add("Megan Thee Stallion");
	tc.Add("Da Brat");}

	{auto& tc = list.Add("Dark/mysterious");
	tc.Add("Nicki Minaj");
	tc.Add("Lil Kim");
	tc.Add("Rapsody");
	tc.Add("Angel Haze");
	tc.Add("Aaliyah");}

	{auto& tc = list.Add("Comical/humorous");
	tc.Add("Missy Elliott");
	tc.Add("Eve");
	tc.Add("Da Brat");
	tc.Add("City Girls");}

	{auto& tc = list.Add("Controversial/provocative");
	tc.Add("Cardi B");
	tc.Add("Nicki Minaj");
	tc.Add("Lil Kim");
	tc.Add("Da Brat");
	tc.Add("Megan Thee Stallion");}

	{auto& tc = list.Add("Nostalgic/sentimental");
	tc.Add("Lauryn Hill");
	tc.Add("Queen Latifah");
	tc.Add("Noname");
	tc.Add("Aaliyah");
	tc.Add("Lil Kim");}

	{auto& tc = list.Add("Wise/philosophical");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Queen Latifah");
	tc.Add("Rapsody");
	tc.Add("Angel Haze");}

	{auto& tc = list.Add("Angry/outspoken");
	tc.Add("Cardi B");
	tc.Add("Missy Elliott");
	tc.Add("Da Brat");
	tc.Add("Megan Thee Stallion");
	tc.Add("Nicki Minaj");}

	{auto& tc = list.Add("Calm/peaceful");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Angel Haze");
	tc.Add("Rapsody");
	tc.Add("Aaliyah");
	tc.Add("Queen Latifah");}

	{auto& tc = list.Add("Confident/self-assured");
	tc.Add("Nicki Minaj");
	tc.Add("Missy Elliott");
	tc.Add("Busta Rhymes");
	tc.Add("Cardi B");
	tc.Add("Eve");
	tc.Add("Megan Thee Stallion");
	tc.Add("Lil Kim");}

	{auto& tc = list.Add("Self-destructive/self-sabotaging");
	tc.Add("Lauryn Hill");
	tc.Add("Rapsody");
	tc.Add("Noname");
	tc.Add("Megan Thee Stallion");
	tc.Add("Lil Kim");
	tc.Add("Lizzo");}

	{auto& tc = list.Add("Hopeful/optimistic");
	tc.Add("J. Cole");
	tc.Add("Rapsody");
	tc.Add("Lauryn Hill");
	tc.Add("Megan Thee Stallion");
	tc.Add("Queen Latifah");}

	{auto& tc = list.Add("Fearful/anxious");
	tc.Add("Missy Elliott");
	tc.Add("Nicki Minaj");
	tc.Add("Lil Kim");
	tc.Add("Cardi B");
	tc.Add("Megan Thee Stallion");}

	{auto& tc = list.Add("Eccentric/quirky");
	tc.Add("Missy Elliott");
	tc.Add("Lauryn Hill");
	tc.Add("Angel Haze");
	tc.Add("Busta Rhymes");
	tc.Add("Rapsody");}

	{auto& tc = list.Add("Sensitive/emotional");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Nicki Minaj");
	tc.Add("Aaliyah");
	tc.Add("Megan Thee Stallion");}

	{auto& tc = list.Add("Bitter/resentful");
	tc.Add("Noname");
	tc.Add("Missy Elliott");
	tc.Add("Cardi B");
	tc.Add("Lil Kim");
	tc.Add("Queen Latifah");}

	{auto& tc = list.Add("Unique/nonconformist");
	tc.Add("Noname");
	tc.Add("Lauryn Hill");
	tc.Add("Angel Haze");
	tc.Add("Nicki Minaj");
	tc.Add("Lil Kim");}

	{auto& tc = list.Add("Free-spirited/nonconformist");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Angel Haze");
	tc.Add("Missy Elliott");
	tc.Add("Nicki Minaj");
	tc.Add("Queen Latifah");}

	{auto& tc = list.Add("Sultry/seductive");
	tc.Add("Lil Kim");
	tc.Add("Nicki Minaj");
	tc.Add("Cardi B");
	tc.Add("Eve");
	tc.Add("Aaliyah");}

	{auto& tc = list.Add("Inspirational/motivational");
	tc.Add("Rapsody");
	tc.Add("Lauryn Hill");
	tc.Add("Nicki Minaj");
	tc.Add("Missy Elliott");
	tc.Add("Megan Thee Stallion");}

	{auto& tc = list.Add("Authentic/real");
	tc.Add("Lauryn Hill");
	tc.Add("Rapsody");
	tc.Add("Angel Haze");
	tc.Add("Noname");
	tc.Add("Queen Latifah");}

	{auto& tc = list.Add("Mysterious/enigmatic");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Angel Haze");
	tc.Add("Rapsody");
	tc.Add("Aaliyah");}

	{auto& tc = list.Add("Carefree/bohemian");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Angel Haze");
	tc.Add("Queen Latifah");
	tc.Add("Janelle Monáe");}

	{auto& tc = list.Add("Street-smart/tough");
	tc.Add("Missy Elliott");
	tc.Add("Nicki Minaj");
	tc.Add("Lil Kim");
	tc.Add("Eve");
	tc.Add("Rapsody");
	tc.Add("Queen Latifah");}

	{auto& tc = list.Add("Romantic/idealistic");
	tc.Add("Lauryn Hill");
	tc.Add("Aaliyah");
	tc.Add("Meg Thee Stallion");
	tc.Add("Nicki Minaj");
	tc.Add("Lil Kim");
	tc.Add("Queen Latifah");}

	{auto& tc = list.Add("Nurturing/motherly");
	tc.Add("Missy Elliott");
	tc.Add("Erykah Badu");
	tc.Add("Queen Latifah");
	tc.Add("Lauryn Hill");
	tc.Add("Rapsody");
	tc.Add("MC Lyte");}

	{auto& tc = list.Add("Dark/tormented");
	tc.Add("Nicki Minaj");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Megan Thee Stallion");
	tc.Add("Angel Haze");}

	{auto& tc = list.Add("Remorseful/regretful");
	tc.Add("Lauryn Hill");
	tc.Add("Aaliyah");
	tc.Add("Noname");
	tc.Add("Nicki Minaj");
	tc.Add("Megan Thee Stallion");}

	{auto& tc = list.Add("Bold/brave");
	tc.Add("Missy Elliott");
	tc.Add("Cardi B");
	tc.Add("Nicki Minaj");
	tc.Add("Lil Kim");
	tc.Add("Queen Latifah");
	tc.Add("Angel Haze");
	tc.Add("Azealia Banks");}

	{auto& tc = list.Add("Outcast/rebel");
	tc.Add("Missy Elliott");
	tc.Add("Nicki Minaj");
	tc.Add("Lil Kim");
	tc.Add("Queen Latifah");
	tc.Add("Angel Haze");
	tc.Add("Azealia Banks");}

	{auto& tc = list.Add("Lost/disconnected");
	tc.Add("Lauryn Hill");
	tc.Add("Noname");
	tc.Add("Megan Thee Stallion");
	tc.Add("Erykah Badu");
	tc.Add("Aaliyah");}

	{auto& tc = list.Add("Tough/badass");
	tc.Add("Cardi B");
	tc.Add("Missy Elliott");
	tc.Add("Nicki Minaj");
	tc.Add("Lil Kim");
	tc.Add("Megan Thee Stallion");
	tc.Add("Rapsody");
	tc.Add("Da Brat");}

	{auto& tc = list.Add("Sincere/genuine");
	tc.Add("Lauryn Hill");
	tc.Add("Rapsody");
	tc.Add("Noname");
	tc.Add("Angel Haze");
	tc.Add("Queen Latifah");}

	{auto& tc = list.Add("Honest/vulnerable");
	tc.Add("Noname");
	tc.Add("Lauryn Hill");
	tc.Add("Rapsody");
	tc.Add("Aaliyah");
	tc.Add("Megan Thee Stallion");}

	{auto& tc = list.Add("Innocent/naive");
	tc.Add("Missy Elliott");
	tc.Add("Aaliyah");
	tc.Add("Janelle Monáe");
	tc.Add("Noname");}

	{auto& tc = list.Add("Bold/risk-taking");
	tc.Add("Missy Elliott");
	tc.Add("Cardi B");
	tc.Add("Nicki Minaj");
	tc.Add("Busta Rhymes");
	tc.Add("Angel Haze");
	tc.Add("Rapsody");}

	ASSERT(list.GetCount() == GetTypecastCount());
	return list;
}

VectorMap<String,Vector<String>>& GetTypecastSingersMale() {
	thread_local static VectorMap<String,Vector<String>> list;
	if (!list.IsEmpty()) return list;
	
	{auto& tc = list.GetAdd("Heartbroken/lovesick");
	tc.Add("Sam Smith");
	tc.Add("Harry Styles");
	tc.Add("Ed Sheeran");
	tc.Add("John Mayer");
	tc.Add("The Weeknd");}

	{auto& tc = list.GetAdd("Rebel/anti-establishment");
	tc.Add("John Lennon");
	tc.Add("Kurt Cobain");
	tc.Add("Bob Dylan");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Freddie Mercury");}

	{auto& tc = list.GetAdd("Political activist");
	tc.Add("Bob Marley");
	tc.Add("Michael Franti");
	tc.Add("Neil Young");
	tc.Add("Bono");
	tc.Add("Woody Guthrie");}

	{auto& tc = list.GetAdd("Social justice advocate");
	tc.Add("Marvin Gaye");
	tc.Add("Stevie Wonder");
	tc.Add("John Legend");
	tc.Add("Kendrick Lamar");
	tc.Add("Common");}

	{auto& tc = list.GetAdd("Party/club");
	tc.Add("Pitbull");
	tc.Add("Usher");
	tc.Add("Justin Timberlake");
	tc.Add("Bruno Mars");
	tc.Add("Jason Derulo");}

	{auto& tc = list.GetAdd("Hopeful/dreamer");
	tc.Add("Imagine Dragons");
	tc.Add("Coldplay");
	tc.Add("Phillip Phillips");
	tc.Add("John Mayer");
	tc.Add("Red Hot Chili Peppers");}

	{auto& tc = list.GetAdd("Confident/empowered");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Van Halen");
	tc.Add("Green Day");
	tc.Add("Blink-182");
	tc.Add("Nirvana");}

	{auto& tc = list.GetAdd("Vulnerable/raw");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Sam Smith");
	tc.Add("Ed Sheeran");
	tc.Add("Sia");
	tc.Add("Amy Winehouse");}

	{auto& tc = list.GetAdd("Romantic/love-driven");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Ed Sheeran");
	tc.Add("John Legend");
	tc.Add("John Mayer");
	tc.Add("Sam Smith");}

	{auto& tc = list.GetAdd("Failure/loser");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Air Supply");
	tc.Add("Bon Jovi");
	tc.Add("Shawn Mendes");
	tc.Add("Fall Out Boy");}

	{auto& tc = list.GetAdd("Spiritual/faithful");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Johnny Cash");
	tc.Add("Jason Mraz");
	tc.Add("Hillsong United");
	tc.Add("Chris Tomlin");}

	{auto& tc = list.GetAdd("Passionate/determined");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Elton John");
	}

	{auto& tc = list.GetAdd("Reflective/self-reflective");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Leonard Cohen");
	tc.Add("Bob Dylan");
	tc.Add("James Taylor");
	tc.Add("Tracy Chapman");
	tc.Add("Jack Johnson");}

	{auto& tc = list.GetAdd("Witty/sarcastic");
	tc.Add("Eminem");
	tc.Add("Kendrick Lamar");
	tc.Add("Arctic Monkeys");
	tc.Add("Childish Gambino");}

	{auto& tc = list.GetAdd("Melancholic/sad");
	tc.Add("Hozier");
	tc.Add("Lana Del Rey");
	tc.Add("The Weeknd");}

	{auto& tc = list.GetAdd("Humble/down-to-earth");
	tc.Add("Ed Sheeran");
	tc.Add("Bruno Mars");
	tc.Add("Jason Mraz");
	tc.Add("John Mayer");}

	{auto& tc = list.GetAdd("Charismatic/charming");
	tc.Add("Bruno Mars");
	tc.Add("Justin Timberlake");
	tc.Add("Frank Sinatra");
	tc.Add("Michael Buble");
	tc.Add("Harry Connick Jr.");}

	{auto& tc = list.GetAdd("Resilient/overcoming adversity");
	tc.Add("Eminem");}

	{auto& tc = list.GetAdd("Carefree/joyful");
	tc.Add("Maroon 5");
	tc.Add("Jason Mraz");
	tc.Add("Jack Johnson");}

	{auto& tc = list.GetAdd("Dark/mysterious");
	tc.Add("The Weeknd");
	tc.Add("David Bowie");}

	{auto& tc = list.GetAdd("Comical/humorous");
	tc.Add("Weird Al Yankovic");
	tc.Add("Flight of the Conchords");
	tc.Add("Stephen Lynch");
	tc.Add("Rodney Carrington");
	tc.Add("The Lonely Island");}

	{auto& tc = list.GetAdd("Controversial/provocative");
	tc.Add("Marilyn Manson");
	tc.Add("Die Antwoord");}

	{auto& tc = list.GetAdd("Nostalgic/sentimental");
	tc.Add("The Beatles");
	tc.Add("Fleetwood Mac");
	tc.Add("Elton John");
	tc.Add("Billy Joel");
	tc.Add("James Taylor");}

	{auto& tc = list.GetAdd("Wise/philosophical");
	tc.Add("Bob Dylan");
	tc.Add("Leonard Cohen");
	tc.Add("Neil Young");
	tc.Add("Paul Simon");
	tc.Add("Tom Petty");}

	{auto& tc = list.GetAdd("Angry/outspoken");
	tc.Add("Rage Against the Machine");
	tc.Add("Green Day");
	tc.Add("Eminem");
	tc.Add("Kanye West");
	tc.Add("System of a Down");}

	{auto& tc = list.GetAdd("Calm/peaceful");
	tc.Add("Jack Johnson");
	tc.Add("Jason Mraz");
	tc.Add("James Taylor");
	tc.Add("John Mayer");
	tc.Add("Ben Howard");}

	{auto& tc = list.GetAdd("Confident/self-assured");
	tc.Add("Eminem");}

	{auto& tc = list.GetAdd("Self-destructive/self-sabotaging");
	tc.Add("Nirvana");
	tc.Add("Kurt Cobain");
	tc.Add("Nine Inch Nails");}

	{auto& tc = list.GetAdd("Hopeful/optimistic");
	tc.Add("Coldplay");
	tc.Add("Phillip Phillips");
	tc.Add("Imagine Dragons");
	tc.Add("U2");
	tc.Add("OneRepublic");}

	{auto& tc = list.GetAdd("Fearful/anxious");
	tc.Add("Twenty One Pilots");}

	{auto& tc = list.GetAdd("Eccentric/quirky");
	tc.Add("David Bowie");
	tc.Add("Prince");
	tc.Add("Tame Impala");}

	{auto& tc = list.GetAdd("Sensitive/emotional");
	tc.Add("James Bay");
	tc.Add("James Taylor");
	tc.Add("Passenger");}

	{auto& tc = list.GetAdd("Bitter/resentful");
	tc.Add("Nirvana");
	tc.Add("Nine Inch Nails");}

	{auto& tc = list.GetAdd("Unique/nonconformist");
	tc.Add("David Bowie");
	tc.Add("Prince");}

	{auto& tc = list.GetAdd("Free-spirited/nonconformist");
	tc.Add("Johnny Cash");
	tc.Add("Bob Dylan");
	tc.Add("Jimi Hendrix");}

	{auto& tc = list.GetAdd("Sultry/seductive");
	tc.Add("Chris Isaak");
	tc.Add("Frank Sinatra");}

	{auto& tc = list.GetAdd("Inspirational/motivational");
	tc.Add("Michael Jackson");
	tc.Add("Imagine Dragons");}

	{auto& tc = list.GetAdd("Authentic/real");
	tc.Add("John Mayer");
	tc.Add("Jason Mraz");
	tc.Add("Ed Sheeran");
	tc.Add("Hozier");}

	{auto& tc = list.GetAdd("Mysterious/enigmatic");
	tc.Add("Billie Eilish");
	tc.Add("David Bowie");
	tc.Add("Florence + The Machine");
	tc.Add("The Weeknd");}

	{auto& tc = list.GetAdd("Carefree/bohemian");
	tc.Add("Fleetwood Mac");
	tc.Add("Grateful Dead");
	tc.Add("Jack Johnson");
	tc.Add("Ben Howard");
	tc.Add("Bob Marley");}

	{auto& tc = list.GetAdd("Street-smart/tough");
	tc.Add("N.W.A");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("The Notorious B.I.G.");}

	{auto& tc = list.GetAdd("Romantic/idealistic");
	tc.Add("John Legend");
	tc.Add("John Mayer");
	tc.Add("Bruno Mars");
	tc.Add("Sam Smith");
	tc.Add("Rex Orange County");}

	{auto& tc = list.GetAdd("Nurturing/motherly");
	tc.Add("Red Hot Chili Peppers");}

	{auto& tc = list.GetAdd("Dark/tormented");
	tc.Add("The Weeknd");
	tc.Add("Kanye West");
	tc.Add("Kendrick Lamar");}

	{auto& tc = list.GetAdd("Remorseful/regretful");
	tc.Add("Justin Bieber");
	tc.Add("Eminem");
	tc.Add("Maroon 5");
	tc.Add("Coldplay");}

	{auto& tc = list.GetAdd("Bold/brave");
	tc.Add("Red Hot Chili Peppers");}

	{auto& tc = list.GetAdd("Outcast/rebel");
	tc.Add("Nirvana");
	tc.Add("Green Day");
	tc.Add("Eminem");
	tc.Add("Kurt Cobain");}

	{auto& tc = list.GetAdd("Lost/disconnected");
	tc.Add("Radiohead");
	tc.Add("Tame Impala");
	tc.Add("The Weeknd");}

	{auto& tc = list.GetAdd("Tough/badass");
	tc.Add("AC/DC");
	tc.Add("Metallica");
	tc.Add("Lynyrd Skynyrd");
	tc.Add("Guns N' Roses");
	tc.Add("Queen");}

	{auto& tc = list.GetAdd("Sincere/genuine");
	tc.Add("Ed Sheeran");
	tc.Add("John Mayer");
	tc.Add("Jason Mraz");
	tc.Add("James Bay");
	tc.Add("Hozier");}

	{auto& tc = list.GetAdd("Honest/vulnerable");
	tc.Add("Sam Smith");
	tc.Add("James Bay");
	tc.Add("Ed Sheeran");}

	{auto& tc = list.GetAdd("Innocent/naive");
	tc.Add("One Direction");
	tc.Add("Shawn Mendes");
	tc.Add("Justin Bieber");
	tc.Add("Bruno Mars");}

	{auto& tc = list.GetAdd("Bold/risk-taking");
	tc.Add("Kanye West");}




	{auto& tc = list.GetAdd("Heartbroken/lovesick");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Nirvana");
	tc.Add("The Beatles");
	tc.Add("Bob Dylan");
	tc.Add("The Doors");}

	{auto& tc = list.GetAdd("Rebel/anti-establishment");
	tc.Add("The Offspring");
	tc.Add("The Clash");
	tc.Add("Rage Against the Machine");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");
	tc.Add("Green Day");}

	{auto& tc = list.GetAdd("Political activist");
	tc.Add("Bob Marley");
	tc.Add("Bob Dylan");
	tc.Add("Public Enemy");
	tc.Add("Rage Against the Machine");
	tc.Add("The Clash");}

	{auto& tc = list.GetAdd("Social justice advocate");
	tc.Add("Bob Marley");
	tc.Add("Public Enemy");
	tc.Add("The Clash");
	tc.Add("Marvin Gaye");
	tc.Add("N.W.A");}

	{auto& tc = list.GetAdd("Party/club");
	tc.Add("The Offspring");
	tc.Add("Limp Bizkit");
	tc.Add("Jay-Z");
	tc.Add("Madonna");
	tc.Add("The Beastie Boys");
	tc.Add("Beyonce");}

	{auto& tc = list.GetAdd("Hopeful/dreamer");
	tc.Add("The Beach Boys");
	tc.Add("ABBA");
	tc.Add("Elton John");
	tc.Add("Bon Jovi");
	tc.Add("U2");}

	{auto& tc = list.GetAdd("Confident/empowered");
	tc.Add("The Offspring");
	tc.Add("Queen");
	tc.Add("AC/DC");}

	{auto& tc = list.GetAdd("Vulnerable/raw");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Bon Iver");
	tc.Add("The National");}

	{auto& tc = list.GetAdd("Romantic/love-driven");
	tc.Add("The Offspring");
	tc.Add("The Beatles");
	tc.Add("Ed Sheeran");
	tc.Add("Coldplay");}

	{auto& tc = list.GetAdd("Failure/loser");
	tc.Add("The Offspring");
	tc.Add("Green Day");
	tc.Add("The Smiths");
	tc.Add("Radiohead");
	tc.Add("Blink-182");
	tc.Add("Morrissey");}

	{auto& tc = list.GetAdd("Spiritual/faithful");
	tc.Add("U2");
	tc.Add("Bob Dylan");
	tc.Add("Sufjan Stevens");
	tc.Add("Mumford & Sons");}

	{auto& tc = list.GetAdd("Passionate/determined");
	tc.Add("The Offspring");
	tc.Add("Bruce Springsteen");
	tc.Add("Nirvana");}

	{auto& tc = list.GetAdd("Reflective/self-reflective");
	tc.Add("Radiohead");
	tc.Add("The National");
	tc.Add("Red Hot Chili Peppers");}

	{auto& tc = list.GetAdd("Witty/sarcastic");
	tc.Add("The Offspring");
	tc.Add("Flight of the Conchords");
	tc.Add("The Lonely Island");
	tc.Add("Weird Al Yankovic");
	tc.Add("Tenacious D");
	tc.Add("The Lonely Island");}

	{auto& tc = list.GetAdd("Melancholic/sad");
	tc.Add("The Smiths");
	tc.Add("Joy Division");
	tc.Add("The Cure");}

	{auto& tc = list.GetAdd("Humble/down-to-earth");
	tc.Add("Johnny Cash");
	tc.Add("Bob Dylan");
	tc.Add("Mumford & Sons");
	tc.Add("Ed Sheeran");}

	{auto& tc = list.GetAdd("Charismatic/charming");
	tc.Add("The Rolling Stones");
	tc.Add("Justin Timberlake");
	tc.Add("Maroon 5");}

	{auto& tc = list.GetAdd("Resilient/overcoming adversity");
	tc.Add("The Offspring");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("The Doors");
	tc.Add("Green Day");
	tc.Add("Foo Fighters");
	}

	{auto& tc = list.GetAdd("Carefree/joyful");
	tc.Add("The Offspring");
	tc.Add("The Beach Boys");
	tc.Add("Jack Johnson");
	tc.Add("The Beatles");}

	{auto& tc = list.GetAdd("Dark/mysterious");
	tc.Add("Joy Division");
	tc.Add("Nine Inch Nails");
	tc.Add("Radiohead");
	tc.Add("The Smashing Pumpkins");
	tc.Add("PJ Harvey");}

	{auto& tc = list.GetAdd("Comical/humorous");
	tc.Add("The Offspring");
	tc.Add("Flight of the Conchords");
	tc.Add("Tenacious D");
	tc.Add("The Lonely Island");
	tc.Add("Tim Minchin");}

	{auto& tc = list.GetAdd("Controversial/provocative");
	tc.Add("The Offspring");
	tc.Add("Marilyn Manson");
	tc.Add("Rage Against the Machine");}

	{auto& tc = list.GetAdd("Nostalgic/sentimental");
	tc.Add("The Offspring");
	tc.Add("The Beatles");
	tc.Add("Fleetwood Mac");
	tc.Add("Queen");
	tc.Add("The Eagles");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Bon Jovi");}

	{auto& tc = list.GetAdd("Wise/philosophical");
	tc.Add("The Offspring");
	tc.Add("Bob Dylan");
	tc.Add("Leonard Cohen");
	tc.Add("Simon & Garfunkel");}

	{auto& tc = list.GetAdd("Angry/outspoken");
	tc.Add("The Offspring");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Rage Against the Machine");
	tc.Add("Green Day");}

	{auto& tc = list.GetAdd("Calm/peaceful");
	tc.Add("Jack Johnson");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Bob Marley");
	tc.Add("Feist");
	tc.Add("Ben Harper");}

	{auto& tc = list.GetAdd("Self-destructive/self-sabotaging");
	tc.Add("Kurt Cobain");
	tc.Add("Nirvana");
	tc.Add("Red Hot Chili Peppers");
	}

	{auto& tc = list.GetAdd("Hopeful/optimistic");
	tc.Add("U2");
	tc.Add("Coldplay");
	tc.Add("Imagine Dragons");
	tc.Add("The Lumineers");
	tc.Add("Mumford & Sons");}

	{auto& tc = list.GetAdd("Fearful/anxious");
	tc.Add("The Offspring");
	tc.Add("Radiohead");
	tc.Add("The National");
	tc.Add("Arcade Fire");
	tc.Add("Bon Iver");}

	{auto& tc = list.GetAdd("Eccentric/quirky");
	tc.Add("The Offspring");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("The Flaming Lips");
	tc.Add("David Bowie");
	tc.Add("They Might Be Giants");
	tc.Add("Neutral Milk Hotel");}

	{auto& tc = list.GetAdd("Sensitive/emotional");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Bon Iver");
	tc.Add("Sam Smith");
	tc.Add("James Blake");}

	{auto& tc = list.GetAdd("Bitter/resentful");
	tc.Add("Marilyn Manson");
	tc.Add("The Offspring");
	}

	{auto& tc = list.GetAdd("Unique/nonconformist");
	tc.Add("The Offspring");
	tc.Add("David Bowie");
	tc.Add("Queen");
	tc.Add("The Velvet Underground");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Prince");}

	{auto& tc = list.GetAdd("Free-spirited/nonconformist");
	tc.Add("The Offspring");
	tc.Add("Bob Dylan");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("The Grateful Dead");
	tc.Add("Fleetwood Mac");
	tc.Add("Joan Baez");}

	{auto& tc = list.GetAdd("Sultry/seductive");
	tc.Add("Billie Holiday");
	tc.Add("The Weeknd");
	tc.Add("Marvin Gaye");}

	{auto& tc = list.GetAdd("Inspirational/motivational");
	tc.Add("U2");
	tc.Add("Coldplay");
	tc.Add("Bruce Springsteen");}

	{auto& tc = list.GetAdd("Authentic/real");
	tc.Add("The Offspring");
	tc.Add("Bon Iver");
	tc.Add("Frank Ocean");
	tc.Add("Kendrick Lamar");}

	{auto& tc = list.GetAdd("Mysterious/enigmatic");
	tc.Add("Radiohead");
	tc.Add("The Black Keys");
	tc.Add("Tom Waits");
	tc.Add("Portishead");}

	{auto& tc = list.GetAdd("Carefree/bohemian");
	tc.Add("The Offspring");
	tc.Add("Edward Sharpe and the Magnetic Zeros");
	tc.Add("Bob Marley");
	tc.Add("Jack Johnson");
	tc.Add("The Grateful Dead");
	tc.Add("The Beatles");}

	{auto& tc = list.GetAdd("Street-smart/tough");
	tc.Add("The Offspring");
	tc.Add("Red Hot Chili Peppers");
	}

	{auto& tc = list.GetAdd("Romantic/idealistic");
	tc.Add("The Beatles");
	tc.Add("Bob Dylan");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Coldplay");}

	{auto& tc = list.GetAdd("Nurturing/motherly");
	tc.Add("Johnny Cash");}

	{auto& tc = list.GetAdd("Dark/tormented");
	tc.Add("Joy Division");
	tc.Add("Nine Inch Nails");
	tc.Add("Elliott Smith");
	tc.Add("Nick Cave and the Bad Seeds");
	tc.Add("The Cure");}

	{auto& tc = list.GetAdd("Remorseful/regretful");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Kurt Cobain");
	tc.Add("The National");}

	{auto& tc = list.GetAdd("Bold/brave");
	tc.Add("The Offspring");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("The Clash");
	tc.Add("Bruce Springsteen");}

	{auto& tc = list.GetAdd("Outcast/rebel");
	tc.Add("The Offspring");
	tc.Add("Nirvana");
	tc.Add("The Smiths");
	tc.Add("Eminem");
	tc.Add("Green Day");
	tc.Add("Red Hot Chili Peppers");}

	{auto& tc = list.GetAdd("Lost/disconnected");
	tc.Add("Radiohead");
	tc.Add("Pink Floyd");
	tc.Add("Nine Inch Nails");
	tc.Add("The National");
	tc.Add("Red Hot Chili Peppers");}

	{auto& tc = list.GetAdd("Tough/badass");
	tc.Add("AC/DC");
	tc.Add("Metallica");
	tc.Add("Guns N' Roses");
	tc.Add("Led Zeppelin");
	tc.Add("The Rolling Stones");
	tc.Add("The Offspring");
	tc.Add("Red Hot Chili Peppers");
	}

	{auto& tc = list.GetAdd("Sincere/genuine");
	tc.Add("John Mayer");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Fleetwood Mac");
	tc.Add("The Offspring");
	tc.Add("James Taylor");}

	{auto& tc = list.GetAdd("Honest/vulnerable");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("Ryan Adams");
	tc.Add("Sufjan Stevens");
	tc.Add("Phoebe Bridgers");}

	{auto& tc = list.GetAdd("Innocent/naive");
	tc.Add("The Beach Boys");
	tc.Add("Simon & Garfunkel");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("The Beatles");}

	{auto& tc = list.GetAdd("Bold/risk-taking");
	tc.Add("Red Hot Chili Peppers");
	tc.Add("The Offspring");
	tc.Add("Nirvana");}
	
	ASSERT(list.GetCount() == GetTypecastCount());
	return list;
}

VectorMap<String,Vector<String>>& GetTypecastRappersMale() {
	thread_local static VectorMap<String,Vector<String>> list;
	if (!list.IsEmpty()) return list;
	
	{auto& tc = list.Add("Heartbroken/lovesick");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Drake");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");}

	{auto& tc = list.Add("Rebel/anti-establishment");
	tc.Add("Run-DMC");
	tc.Add("N.W.A");
	tc.Add("Beastie Boys");
	tc.Add("Public Enemy");
	tc.Add("Rage Against the Machine");}

	{auto& tc = list.Add("Political activist");
	tc.Add("Tupac Shakur");
	tc.Add("Public Enemy");
	tc.Add("Rage Against the Machine");
	tc.Add("Jay-Z");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Social justice advocate");
	tc.Add("Public Enemy");
	tc.Add("Run-DMC");
	tc.Add("N.W.A");
	tc.Add("Jay-Z");
	tc.Add("Grandmaster Flash and the Furious Five");}

	{auto& tc = list.Add("Party/club");
	tc.Add("The Beastie Boys");
	tc.Add("Run-DMC");
	tc.Add("Limp Bizkit");
	tc.Add("Jay-Z");
	tc.Add("Drake");}

	{auto& tc = list.Add("Hopeful/dreamer");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");
	tc.Add("Drake");}

	{auto& tc = list.Add("Confident/empowered");
	tc.Add("Jay-Z");
	tc.Add("Eminem");
	tc.Add("Tupac Shakur");
	tc.Add("Drake");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Vulnerable/raw");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");
	tc.Add("Drake");}

	{auto& tc = list.Add("Romantic/love-driven");
	tc.Add("Drake");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Failure/loser");
	tc.Add("Eminem");
	tc.Add("Tupac Shakur");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");
	tc.Add("Drake");}

	{auto& tc = list.Add("Spiritual/faithful");
	tc.Add("Tupac Shakur");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");
	tc.Add("Limp Bizkit");
	tc.Add("Jay-Z");}

	{auto& tc = list.Add("Passionate/determined");
	tc.Add("Eminem");
	tc.Add("Tupac Shakur");
	tc.Add("Jay-Z");
	tc.Add("N.W.A");
	tc.Add("Drake");}

	{auto& tc = list.Add("Reflective/self-reflective");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");
	tc.Add("DrakeConfident/self-assured");}

	{auto& tc = list.Add("Witty/sarcastic");
	tc.Add("Eminem");
	tc.Add("Beastie Boys");
	tc.Add("Tupac Shakur");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");}

	{auto& tc = list.Add("Melancholic/sad");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Drake");
	tc.Add("Limp Bizkit");}

	{auto& tc = list.Add("Humble/down-to-earth");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Drake");
	tc.Add("Public Enemy");}

	{auto& tc = list.Add("Charismatic/charming");
	tc.Add("Jay-Z");
	tc.Add("Tupac Shakur");
	tc.Add("Drake");
	tc.Add("Eminem");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Resilient/overcoming adversity");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Carefree/joyful");
	tc.Add("Jay-Z");
	tc.Add("Drake");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");
	tc.Add("Limp Bizkit");}

	{auto& tc = list.Add("Dark/mysterious");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Limp Bizkit");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Comical/humorous");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Beastie Boys");
	tc.Add("Run-DMC");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Controversial/provocative");
	tc.Add("Eminem");
	tc.Add("Tupac Shakur");
	tc.Add("N.W.A");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");}

	{auto& tc = list.Add("Nostalgic/sentimental");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Limp Bizkit");
	tc.Add("Jay-Z");
	tc.Add("Drake");}

	{auto& tc = list.Add("Wise/philosophical");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Angry/outspoken");
	tc.Add("Rage Against the Machine");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");
	tc.Add("Run-DMC");
	tc.Add("Eminem");}

	{auto& tc = list.Add("Calm/peaceful");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Limp Bizkit");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Confident/self-assured");
	tc.Add("Jay-Z");
	tc.Add("Drake");
	tc.Add("Eminem");
	tc.Add("Tupac Shakur");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Self-destructive/self-sabotaging");
	tc.Add("Eminem");
	tc.Add("Tupac Shakur");
	tc.Add("Limp Bizkit");
	tc.Add("Jay-Z");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Hopeful/optimistic");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Limp Bizkit");
	tc.Add("Jay-Z");
	tc.Add("Drake");}

	{auto& tc = list.Add("Fearful/anxious");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Drake");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");}

	{auto& tc = list.Add("Eccentric/quirky");
	tc.Add("Beastie Boys");
	tc.Add("Limp Bizkit");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Sensitive/emotional");
	tc.Add("Eminem");
	tc.Add("Tupac Shakur");
	tc.Add("Jay-Z");
	tc.Add("N.W.A");
	tc.Add("Limp Bizkit");}

	{auto& tc = list.Add("Bitter/resentful");
	tc.Add("Eminem");
	tc.Add("Tupac Shakur");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");
	tc.Add("Limp Bizkit");}

	{auto& tc = list.Add("Unique/nonconformist");
	tc.Add("Beastie Boys");
	tc.Add("Limp Bizkit");
	tc.Add("Public Enemy");
	tc.Add("N.W.A");
	tc.Add("Run-DMC");}

	{auto& tc = list.Add("Free-spirited/nonconformist");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");
	tc.Add("Run-DMC");}

	{auto& tc = list.Add("Sultry/seductive");
	tc.Add("Drake");
	tc.Add("Jay-Z");
	tc.Add("Eminem");
	tc.Add("Tupac Shakur");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Inspirational/motivational");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");
	tc.Add("Drake");}

	{auto& tc = list.Add("Authentic/real");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Mysterious/enigmatic");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Limp Bizkit");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Carefree/bohemian");
	tc.Add("Jay-Z");
	tc.Add("Drake");
	tc.Add("N.W.A");
	tc.Add("Run-DMC");
	tc.Add("Eminem");}

	{auto& tc = list.Add("Street-smart/tough");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");
	tc.Add("Limp Bizkit");}

	{auto& tc = list.Add("Romantic/idealistic");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");
	tc.Add("Drake");}

	{auto& tc = list.Add("Nurturing/motherly");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");}

	{auto& tc = list.Add("Dark/tormented");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Remorseful/regretful");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");
	tc.Add("Drake");}

	{auto& tc = list.Add("Bold/brave");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");}

	{auto& tc = list.Add("Outcast/rebel");
	tc.Add("Tupac Shakur");
	tc.Add("N.W.A");
	tc.Add("Jay-Z");
	tc.Add("Eminem");
	tc.Add("Public Enemy");}

	{auto& tc = list.Add("Lost/disconnected");
	tc.Add("Tupac Shakur");
	tc.Add("Jay-Z");
	tc.Add("Drake");
	tc.Add("Limp Bizkit");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Tough/badass");
	tc.Add("Jay-Z");
	tc.Add("N.W.A");
	tc.Add("Eminem");
	tc.Add("Tupac Shakur");
	tc.Add("Public Enemy");}

	{auto& tc = list.Add("Sincere/genuine");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");
	tc.Add("Drake");}

	{auto& tc = list.Add("Honest/vulnerable");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");
	tc.Add("Jay-Z");}

	{auto& tc = list.Add("Innocent/naive");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("Public Enemy");
	tc.Add("N.W.A");}

	{auto& tc = list.Add("Bold/risk-taking");
	tc.Add("Tupac Shakur");
	tc.Add("Eminem");
	tc.Add("Jay-Z");
	tc.Add("N.W.A");
	tc.Add("Public Enemy");}
	
	ASSERT(list.GetCount() == GetTypecastCount());
	return list;
}




VectorMap<String,Vector<String>>& GetTypecastSingers(bool gender) {
	if (!gender)
		return GetTypecastSingersMale();
	else
		return GetTypecastSingersFemale();
}

VectorMap<String,Vector<String>>& GetTypecastRappers(bool gender) {
	if (!gender)
		return GetTypecastRappersMale();
	else
		return GetTypecastRappersFemale();
}

VectorMap<String,Vector<String>>& GetTypecastArtists(bool rapper, bool gender) {
	if (!rapper)
		return GetTypecastSingers(gender);
	else
		return GetTypecastRappers(gender);
}





const Vector<String>& InlineRapperList() {
	static Vector<String> v;
	static Vector<String> list;
	if (list.IsEmpty()) {
		list.Add("Eminem");
		list.Add("Kendrick Lamar");
		list.Add("MF DOOM");
		list.Add("Big L");
		list.Add("Aesop Rock");
		list.Add("Busta Rhymes");
		list.Add("Earl Sweatshirt");
		list.Add("Tech N9ne");
		list.Add("Logic");
		list.Add("Kool G Rap ");
		list.Add("Royce da 5'9");
		list.Add("Rakim");
		list.Add("Black Thought");
		list.Add("Canibus");
		list.Add("Blackalicious");
		list.Add("Danny Brown");
		list.Add("Big Pun");
		list.Add("GZA/Genius");
		list.Add("R.A. the Rugged Man");
		list.Add("Jean Grae");
	}
	return list;
}

const Vector<String>& OtherRapperList() {
	static Vector<String> list;
	if (list.IsEmpty()) {
		list.Add("J. Cole");
		list.Add("Nicki Minaj");
		list.Add("Tyler, The Creator");
		list.Add("Chance the Rapper");
		list.Add("Mac Miller");
		list.Add("Travis Scott");
		list.Add("J.I.D.");
		list.Add("Tierra Whack");
		list.Add("Noname");
		list.Add("Vic Mensa");
		list.Add("A$AP Rocky");
		list.Add("Lil Wayne");
		list.Add("Method Man");
		list.Add("Ghostface Killah");
		list.Add("Jay-Z");
		list.Add("Andre 3000");
		list.Add("Nas");
		list.Add("Lauryn Hill");
	}
	return list;
}





















const Index<String>& GetRoles() {
	static Index<String> list;
	if (list.IsEmpty()) {
		list.Add("Influencer");
		list.Add("Activist");
		list.Add("Expert");
		list.Add("Comedian");
		list.Add("Politician");
		list.Add("Social media personality");
		list.Add("Marketer");
		list.Add("Journalist");
		list.Add("Writer/author");
		list.Add("Celebrity/entertainer");
		list.Add("Blogger");
		list.Add("Entrepreneur");
		list.Add("Educator");
		list.Add("Student");
		list.Add("Parent");
		list.Add("Athlete");
		list.Add("Music fan");
		list.Add("Foodie");
		list.Add("Traveler");
		list.Add("Developer/programmer");
		list.Add("Entity/creative");
		list.Add("Scientist/researcher");
		list.Add("Environmentalist");
		list.Add("Animal lover/activist");
		list.Add("Fashionista");
		list.Add("Homemaker");
		list.Add("Philanthropist");
		list.Add("Socialite");
		list.Add("Food/drink critic");
		list.Add("Gamer");
		list.Add("Fitness enthusiast");
		list.Add("Health/wellness guru");
		list.Add("Spiritual leader");
		list.Add("Parenting advice");
		list.Add("Career coach/advisor");
		list.Add("Travel blogger");
		list.Add("Book lover/reader");
		list.Add("DIY enthusiast");
		list.Add("Pet lover/owner");
		list.Add("Movie/TV critic");
		list.Add("Beauty/fashion blogger");
		list.Add("Tech geek");
		list.Add("Nature lover");
		list.Add("Political commentator");
		list.Add("Relationship expert");
		list.Add("Human rights activist");
		list.Add("Social justice warrior");
		list.Add("Music reviewer");
		list.Add("Interior design enthusiast");
		list.Add("Self-help guru");
		list.Add("Life coach");
		list.Add("Mental health advocate");
		list.Add("Promoter/event organizer");
		list.Add("Financial advisor");
		list.Add("Food blogger");
		list.Add("Sports enthusiast");
		list.Add("Fashion designer");
		list.Add("Makeup artist");
		list.Add("Gardening enthusiast");
		list.Add("Geek/nerd");
		list.Add("History buff");
		list.Add("Business owner");
		list.Add("Legal expert");
		list.Add("Parenting blogger");
		list.Add("Senior citizen/retiree");
		list.Add("Marriage counselor");
		list.Add("Wine connoisseur");
		list.Add("Youth advocate");
		list.Add("Success coach");
		list.Add("Career woman/man");
		list.Add("Fitness coach");
		list.Add("Political blogger");
		list.Add("Blogger/influencer relations");
		list.Add("Adult entertainer");
		list.Add("Adult content creator");
		list.Add("Adult industry critic");
		list.Add("Adult content reviewer");
	}
	return list;
}

int GetRoleCount() {
	return GetRoles().GetCount();
}

const Vector<ContentType>& GetGenerics() {
	static Vector<ContentType> list;
	if (list.IsEmpty()) {
		list.Add().Set("Rise to fame", "a person shares their journey and successes", "shares achievements and milestones", "shares their expertise and advice for others to achieve success");
		list.Add().Set("Call to Action", "person speaks out on important social or political issues", "uses their platform and influence to promote change", "urges others to take action and make a difference");
		list.Add().Set("Everyday life updates", "person shares personal and relatable experiences", "offers glimpses into their daily routines", "shares thoughts and opinions on current events or pop culture");
		list.Add().Set("Brand promotion", "person promotes their products or services", "creates hype and anticipation for upcoming snaps or events", "collaborates with other brands and influencers to expand reach and exposure");
		list.Add().Set("Entertainment", "person shares jokes and comedic content", "reacts to memes and trending topics", "creates funny and entertaining videos/performances");
		list.Add().Set("Advocacy and awareness", "person raises awareness for important causes", "shares personal stories and experiences", "educates and informs their followers on important issues");
		list.Add().Set("Political opinions and debates", "person shares their political views and beliefs", "engages in debates and discussions with others", "campaigns for a particular candidate or party");
		list.Add().Set("Behind the scenes", "person offers an inside look into their creative process or work", "shares sneak peeks of upcoming projects", "takes followers on a virtual tour of their workspace");
		list.Add().Set("Travel adventures", "person shares photos and videos from their travels", "explores new places and cultures", "shares tips and recommendations for others interested in traveling");
		list.Add().Set("Fitness journey", "person shares their fitness goals and progress", "creates workout videos and tutorials", "motivates and inspires others to prioritize their health and fitness");
		list.Add().Set("Food and cooking", "person shares their favorite recipes and cooking tips", "reviews restaurants and food products", "documents their food adventures and cooking experiments");
		list.Add().Set("Tech and innovation", "person shares news and updates on the latest technology and innovations", "offers tech advice and recommendations", "discusses the impact of technology on society");
		list.Add().Set("Self-care and wellness", "person promotes self-care practices and mindfulness", "shares motivational quotes and tips for self-improvement", "discusses mental health and self-care");
		list.Add().Set("Relationships and love", "person shares their own experiences with relationships and dating", "offers advice and support to others going through similar situations", "discusses different types of relationships and love");
		list.Add().Set("Fashion and style", "person showcases their own fashion and style", "collaborates with fashion brands and influencers for sponsored content", "offers fashion tips and advice");
		list.Add().Set("Music and concerts", "person shares their favorite music and entities", "attends and covers concerts and music festivals", "discusses the impact of music on culture and society");
		list.Add().Set("DIY and crafts", "person shares DIY tutorials and projects", "offers tips and tricks for crafting and home decor", "encourages others to unleash their creativity");
		list.Add().Set("Animal lover", "person shares photos and videos of their pets", "raises awareness for animal rights and welfare", "promotes adoption and rescue organizations");
		list.Add().Set("Reviews and recommendations", "person shares their thoughts and opinions on products, services, and experiences", "provides honest reviews and recommendations", "collaborates with brands for sponsored reviews");
		list.Add().Set("Beauty and makeup", "person shares makeup tutorials and beauty tips", "collaborates with beauty brands for sponsored content", "discusses body positivity and self-love");
		list.Add().Set("Bookworm", "person shares their current reads and book recommendations", "participates in book clubs and discussions", "writes book reviews and author interviews");
		list.Add().Set("Gaming and esports", "person shares their favorite games and gaming setup", "streams their gameplay for followers", "discusses the latest news and trends in the gaming industry");
		list.Add().Set("Education and learning", "person shares their educational journey and tips for academic success", "creates educational content and resources", "discusses the importance of education and lifelong learning");
		list.Add().Set("Nature and conservation", "person shares photos and videos of nature and wildlife", "raises awareness for environmental conservation and sustainability", "discusses ways to protect and preserve the planet");
		list.Add().Set("Entrepreneurship", "person shares their experience and lessons as a business owner", "offers business advice and strategies", "collaborates with other entrepreneurs and businesses for networking and growth");
		list.Add().Set("Art and creativity", "person showcases their artistic talents and creations", "collaborates with other entities and galleries for exposure", "discusses the impact of art on society");
		list.Add().Set("Health and medical advice", "person shares medical advice and resources", "discusses the latest news and research in the healthcare industry", "raises awareness for health issues and campaigns");
		list.Add().Set("Celebrity gossip", "person shares the latest celebrity news and rumors", "participates in discussions and debates about celebrities and their personal lives", "creates humorous content and memes related to celebrity culture");
		list.Add().Set("Music producer and songwriter", "person shares their own original music and songwriting process", "collaborates with other entities and producers for music projects", "shares behind the scenes footage of music production and studio work");
		list.Add().Set("Influencer on the rise", "person documents their journey as an up-and-coming social media influencer", "shares tips and strategies for growing a following and establishing a brand on social media", "collaborates with brands for sponsored content and partnerships");
		/*list.Add().Set("Wedding planning", "person shares their own wedding planning journey and tips", "offers wedding planning services and advice for followers", "collaborates with wedding vendors and venues");
		list.Add().Set("Political news junkie", "person shares news articles and updates on politics and current events", "discusses and analyses political policies and decisions", "participates in online discussions and debates with others");
		list.Add().Set("Philanthropy and charity", "person raises money and awareness for charitable causes", "participates in fundraising events and campaigns", "uses their platform for good and to make a positive impact on society");
		list.Add().Set("Career and job advice", "person shares their career journey and tips for professional growth", "offers job search strategies and resume advice", "discusses workplace culture and trends");
		list.Add().Set("Senior citizen lifestyle", "person shares their experiences and struggles as a senior citizen", "discusses issues related to aging and retirement", "offers advice and support to other seniors");
		list.Add().Set("Travel photography", "person captures and shares stunning photos of different locations around the world", "offers photography tips and advice", "collaborates with travel brands for sponsored content");
		list.Add().Set("Culinary adventures", "person shares their culinary experiences and food adventures", "tries new and unique foods and cuisines", "discovers hidden food gems in their city");
		list.Add().Set("LGBTQ+ advocate", "person shares their personal experiences and insights as a member of the LGBTQ+ community", "raises awareness for LGBTQ+ rights and issues", "participates in Pride events and campaigns");
		list.Add().Set("Motivational speaker", "person shares inspirational quotes and words of wisdom", "offers advice and support to followers", "shares their own personal journey and obstacles overcome");
		list.Add().Set("News and current events", "person discusses and provides updates on important news and current events", "engages in discussions and debates on controversial topics", "shares unbiased and factual information");
		list.Add().Set("Fitness challenges", "person creates fitness challenges and workouts for followers to join", "tracks their progress and invites others to do the same", "offers prizes and incentives for completion");
		list.Add().Set("Hiking and outdoors", "person shares photos and videos of their outdoor adventures", "offers tips and recommendations for hiking and camping", "advocates for environmental conservation and protection");
		list.Add().Set("Parenting humor", "person shares funny and relatable parenting memes and content", "creates humorous videos and sketches about the realities of parenting", "engages in lighthearted discussions and debates with other parents");
		list.Add().Set("Financial literacy", "person shares budgeting tips and financial advice", "discusses the importance of saving and investing", "collaborates with financial advisors and experts for educational content");
		list.Add().Set("Pop culture analysis", "person shares their thoughts and opinions on trends and pop culture phenomena", "discusses the impact of popular media on society", "creates content that deconstructs and analyzes popular culture");
		list.Add().Set("Mental health advocacy", "person shares their personal struggles and journey with mental health", "raises awareness and fights stigma associated with mental illness", "offers resources and support to those struggling with mental health issues");
		list.Add().Set("Raising a special needs child", "person shares their experiences and challenges raising a child with special needs", "offers support and resources to other parents in similar situations", "advocates for improved rights and accommodations for individuals with disabilities");
		list.Add().Set("Marketing and branding strategies", "person shares their expertise in marketing and branding", "offers tips and advice for entrepreneurs and small business owners", "collaborates with brands for sponsored content and partnerships");
		list.Add().Set("Conspiracy theories", "person shares theories and evidence supporting various conspiracy theories", "participates in discussions and debates with other believers and skeptics", "creates content that dives deep into the world of conspiracy");
		list.Add().Set("Tech gadgets and reviews", "person shares reviews and recommendations for the latest tech gadgets and devices", "offers tech advice and tutorials", "collaborates with tech brands for sponsored content and reviews");
		list.Add().Set("The single life", "person shares their experiences and thoughts on being single", "discusses dating and relationships", "creates content that challenges the societal norms and expectations surrounding being in a relationship");
		list.Add().Set("Online activism", "person uses their platform and influence to advocate for social and political issues", "participates in online campaigns and hashtags", "encourages their followers to take action and make a difference");
		list.Add().Set("Music producer", "person shares their music production process and techniques", "offers tips and tricks for aspiring producers", "collaborates with other entities for music projects");
		list.Add().Set("Singer in a rock band", "person shares music from their band and performances at concerts and festivals", "creates music videos and behind the scenes footage", "discusses the rock music genre and its evolution");
		list.Add().Set("Running and fitness challenges", "person participates in running challenges and documents their progress", "encourages followers to join them in achieving fitness goals", "advocates for running as a form of physical and mental health");
		list.Add().Set("Rapper", "person shares their original rap components and music videos", "documents the process of creating and producing rap music", "discusses the hip hop and rap culture");
		list.Add().Set("Rap music producer", "person shares their production techniques and collaborations with rap entities", "discusses the evolution and trends in rap music production", "offers advice and resources for aspiring rap music producers");
		list.Add().Set("Luxury lifestyle", "person shares their luxurious travels, fashion, and lifestyle", "collaborates with luxury brands for sponsored content", "creates content that showcases and celebrates the finer things in life");
		list.Add().Set("Being a new parent", "person shares the ups and downs of being a new parent", "offers advice and support for other new parents", "documents their child's growth and milestones");
		list.Add().Set("Cheap local traveller", "person documents their budget-friendly travels to local destinations", "offers tips and recommendations for affordable travel", "advocates for responsible and sustainable tourism");
		list.Add().Set("Bakery owner", "person shares their baking journey and creates content featuring their bakery", "offers baking tips and recipes", "promotes their bakery and engages with customers online");
		list.Add().Set("Music photographer", "person shares their concert photography and behind the scenes shots", "offers photography tips and advice", "collaborates with music entities and festivals for content and coverage");
		list.Add().Set("Metal music producer", "person shares their process of producing metal music and collaborations with metal entities", "discusses the history and sub-genres of metal music", "offers resources and advice for aspiring metal music producers");
		list.Add().Set("Self-taught artist", "person documents their art journey and progress as a self-taught artist", "shares tutorials and tips for aspiring entities", "collaborates with other entities for inspiration and growth");
		list.Add().Set("Beet enthusiasist", "person shares their love for beets and creates original recipes and dishes featuring beets", "discusses the health benefits and uses of beets", "participates in online discussions with other beet lovers");
		list.Add().Set("Rock music producer", "person shares their process of producing rock music and collaborations with rock entities", "discusses the evolution and trends of rock music production", "offers resources and advice for aspiring rock music producers");
		list.Add().Set("News commentator", "person shares their perspectives and analysis on current events and news stories", "discusses politics, society, and culture", "participates in debates and discussions with others");
		list.Add().Set("Left-wing commentator", "person shares their liberal views and critiques on politics and social issues", "offers alternative perspectives and solutions to current problems", "advocates for progressive change and activism");
		list.Add().Set("Music business owner", "person shares their experiences and challenges as a music business owner", "offers advice and resources for running a successful music business", "collaborates with other professionals in the music industry");
		list.Add().Set("Cheap local hiker", "person documents their budget-friendly hikes and nature adventures in their local area", "offers tips and recommendations for affordable outdoor activities", "advocates for environmental conservation and responsible hiking");
		list.Add().Set("Downhill skiing enthusiast", "person shares footage and photos from their downhill skiing adventures", "discusses techniques and gear for skiing", "collaborates with ski resorts and brands for sponsored content");
		list.Add().Set("Kayak owner", "person shares their kayaking trips and adventures", "offers tips and recommendations for beginner kayakers", "discusses the benefits and environmental impact of kayaking");
		list.Add().Set("Podcaster", "person hosts and produces their own podcast on a specific topic or theme", "invites guests to share their expertise and opinions", "promotes and engages with listeners on social media");
		list.Add().Set("Music podcaster", "person hosts and produces a podcast featuring interviews and discussions with music entities and industry professionals", "discusses the latest trends and happenings in the music industry", "collaborates with independent musicians and record labels for content");
		list.Add().Set("Music production podcaster", "person hosts and produces a podcast on the topic of music production", "features interviews and discussions with music producers and engineers", "offers tips and resources for aspiring music producers");
		list.Add().Set("Food vlogger", "person creates videos documenting their culinary adventures and recipes", "collaborates with restaurants and brands for sponsored content", "offers cooking and food-related tips and tutorials");
		list.Add().Set("Adventure seeker", "person shares their adrenaline-fueled travels and activities", "documents extreme sports and outdoor adventures", "advocates for embracing new challenges and getting out of one's comfort zone");
		list.Add().Set("Metal band vlogger", "person documents their life and career as a member of a metal band", "shares videos of live performances and behind the scenes footage", "discusses the metal music community and culture");
		list.Add().Set("Fitness model", "person shares their fitness journey and offers workout and nutrition advice", "collaborates with fitness and sports brands for sponsored content", "promotes body positivity and healthy living");
		list.Add().Set("EDM producer vlogger", "person documents their journey and process of producing electronic dance music", "shares behind the scenes footage of festivals and events", "collaborates with other EDM entities and DJs for content");
		list.Add().Set("C++ programmer", "person shares their coding projects and programming tips and resources for C++", "discusses advancements and updates in C++ programming language", "participates in online discussions and collaborations with fellow C++ programmers");
		list.Add().Set("Small business owner", "person shares their experiences and challenges of owning a small business", "discusses entrepreneurship and offers advice for other small business owners", "collaborates with other entrepreneurs and local businesses for a stronger community");
		list.Add().Set("Daily life vlogger", "person shares snippets of their daily life and activities", "creates a personal connection with followers by providing a glimpse into their life", "engages with followers and responds to comments and questions");
		list.Add().Set("Beauty guru", "person creates makeup tutorials, product reviews, and beauty hacks", "collaborates with beauty brands for sponsored content", "promotes self-love and confidence through makeup");
		list.Add().Set("Animal rescue advocate", "person shares their experiences rescuing and caring for animals in need", "promotes adoption and responsible pet ownership", "participates in online campaigns and donations for animal rescue organizations");
		list.Add().Set("Food critic", "person shares reviews and critiques of restaurants and dishes", "creates food-related content such as recipes and food challenges", "collaborates with restaurants and food brands for sponsored content");
		list.Add().Set("Travel photographer", "person shares their travel photography from destinations around the world", "offers tips for improving photography skills and capturing unique shots", "collaborates with tourism boards and travel brands for sponsored content");
		list.Add().Set("Music business thoughts", "person shares their opinions and ideas on the music industry and its future", "discusses the impact of technology and social media on the music business", "collaborates with other industry professionals for discussions and debates");
		list.Add().Set("Thoughts about the music business" , "person shares their experiences and insights as a music industry professional", "offers advice and resources for aspiring musicians and industry professionals", "discusses the challenges and opportunities in the music business");
		list.Add().Set("Thoughts about other entities as an music artist", "person shares their thoughts and analysis on other music entities and their work", "engages in discussions and debates about different music genres and styles", "creates content that promotes and supports other talent");
		list.Add().Set("Starting a business from scratch", "person documents their journey and struggles of starting a business from the ground up", "offers advice and resources for aspiring entrepreneurs", "encourages and motivates others to pursue their dreams");
		list.Add().Set("Art collector", "person shares their art collection and discusses the stories behind each piece", "offers tips for building an art collection and investing in art", "collaborates with entities and galleries for sponsored content and events");
		list.Add().Set("Music listener", "person shares their music playlist and recommendations for different music genres", "engages in discussions and debates about music and entities", "creates content that celebrates the power and influence of music");
		list.Add().Set("Metal music listener", "person shares their metal music playlist and attends concerts and festivals", "participates in online discussions and debates about metal music", "creates content that promotes and celebrates the metal music community");
		*/
	}
	return list;
}

int GetGenericCount() {
	return GetGenerics().GetCount();
}


VectorMap<String,Vector<String>>& GetRolesSafeMale() {
	thread_local static VectorMap<String,Vector<String>> list;
	if (!list.IsEmpty()) return list;
	
	{auto& tc = list.Add("Influencer");
	tc.Add("Barack Obama");
	tc.Add("Dwayne \"The Rock\" Johnson");
	tc.Add("Justin Timberlake");
	tc.Add("Will Smith");
	tc.Add("Neil Patrick Harris");}

	{auto& tc = list.Add("Activist");
	tc.Add("Malala Yousafzai");
	tc.Add("Colin Kaepernick");
	tc.Add("Leonardo DiCaprio");
	tc.Add("Emma Watson");
	tc.Add("George Takei");}

	{auto& tc = list.Add("Expert");
	tc.Add("Dr. Fauci");
	tc.Add("Bill Gates");
	tc.Add("Neil deGrasse Tyson");
	tc.Add("Anderson Cooper");
	tc.Add("Trevor Noah");}

	{auto& tc = list.Add("Comedian");
	tc.Add("John Mulaney");
	tc.Add("Hasan Minhaj");
	tc.Add("Steve Carell");
	tc.Add("Andy Samberg");
	tc.Add("Jerry Seinfeld");}

	{auto& tc = list.Add("Politician");
	tc.Add("Joe Biden");
	tc.Add("Bernie Sanders");
	tc.Add("Justin Trudeau");
	tc.Add("Jacinda Ardern");
	tc.Add("Emmanuel Macron");}

	{auto& tc = list.Add("Social media personality");
	tc.Add("Ryan Seacrest");
	tc.Add("Felix Kjellberg (PewDiePie)");
	tc.Add("MrBeast");
	tc.Add("David Dobrik");
	tc.Add("Jeffree Star");}

	{auto& tc = list.Add("Marketer");
	tc.Add("Gary Vaynerchuk");
	tc.Add("Seth Godin");
	tc.Add("Tim Ferriss");
	tc.Add("Neil Patel");
	tc.Add("Grant Cardone");}

	{auto& tc = list.Add("Journalist");
	tc.Add("Anderson Cooper");
	tc.Add("Lester Holt");
	tc.Add("Christiane Amanpour");
	tc.Add("Robin Roberts");
	tc.Add("Jake Tapper");}

	{auto& tc = list.Add("Writer/author");
	tc.Add("Stephen King");
	tc.Add("J.K. Rowling");
	tc.Add("John Green");
	tc.Add("James Patterson");
	tc.Add("Malcolm Gladwell");}

	{auto& tc = list.Add("Celebrity/entertainer");
	tc.Add("Tom Hanks");
	tc.Add("Chris Pratt");
	tc.Add("Lin-Manuel Miranda");
	tc.Add("Leonardo DiCaprio");
	tc.Add("Drake");}

	{auto& tc = list.Add("Blogger");
	tc.Add("Seth Godin");
	tc.Add("Tim Ferriss");
	tc.Add("Neil Patel");
	tc.Add("Rand Fishkin");
	tc.Add("James Clear");}

	{auto& tc = list.Add("Entrepreneur");
	tc.Add("Elon Musk");
	tc.Add("Richard Branson");
	tc.Add("Mark Cuban");
	tc.Add("Daymond John");
	tc.Add("Peter Thiel");}

	{auto& tc = list.Add("Educator");
	tc.Add("Bill Nye");
	tc.Add("Neil deGrasse Tyson");
	tc.Add("Ken Robinson");
	tc.Add("Salman Khan");
	tc.Add("Sir David Attenborough");}

	{auto& tc = list.Add("Student");
	tc.Add("Greta Thunberg");
	tc.Add("Malala Yousafzai");
	tc.Add("Shawn Mendes");
	tc.Add("Zendaya");
	tc.Add("Millie Bobby Brown");}

	{auto& tc = list.Add("Parent");
	tc.Add("Barack Obama");
	tc.Add("Neil Patrick Harris");
	tc.Add("John Legend");
	tc.Add("Ashton Kutcher");
	tc.Add("Dwayne \"The Rock\" Johnson");}

	{auto& tc = list.Add("Athlete");
	tc.Add("LeBron James");
	tc.Add("Tom Brady");
	tc.Add("Cristiano Ronaldo");
	tc.Add("Lionel Messi");
	tc.Add("Serena Williams");}

	{auto& tc = list.Add("Music fan");
	tc.Add("Ed Sheeran");
	tc.Add("Harry Styles");
	tc.Add("Billie Eilish");
	tc.Add("Bruno Mars");
	tc.Add("Lizzo");}

	{auto& tc = list.Add("Foodie");
	tc.Add("Anthony Bourdain");
	tc.Add("Jamie Oliver");
	tc.Add("Rachael Ray");
	tc.Add("Gordon Ramsey");
	tc.Add("Samantha Lee");}

	{auto& tc = list.Add("Traveler");
	tc.Add("Anthony Bourdain");
	tc.Add("Rick Steves");
	tc.Add("Bear Grylls");
	tc.Add("Anthony Melchiorri");
	tc.Add("Andrew Zimmern");}

	{auto& tc = list.Add("Developer/programmer");
	tc.Add("Linus Torvalds");
	tc.Add("Mark Zuckerberg");
	tc.Add("Elon Musk");
	tc.Add("Tim Cook");
	tc.Add("Jack Dorsey");}

	{auto& tc = list.Add("Entity/creative");
	tc.Add("Walt Disney");
	tc.Add("J.K. Rowling");
	tc.Add("Stan Lee");
	tc.Add("George Lucas");
	tc.Add("Steven Spielberg");}

	{auto& tc = list.Add("Scientist/researcher");
	tc.Add("Neil deGrasse Tyson");
	tc.Add("Bill Nye");
	tc.Add("Stephen Hawking");
	tc.Add("Jane Goodall");
	tc.Add("Edward O. Wilson");}

	{auto& tc = list.Add("Environmentalist");
	tc.Add("Leonardo DiCaprio");
	tc.Add("Jane Goodall");
	tc.Add("Jeff Corwin");
	tc.Add("Ed Begley Jr.");
	tc.Add("David Attenborough");}

	{auto& tc = list.Add("Animal lover/activist");
	tc.Add("Ellen DeGeneres");
	tc.Add("Dwayne \"The Rock\" Johnson");
	tc.Add("Jessica Chastain");
	tc.Add("Ian Somerhalder");
	tc.Add("Mandy Moore");}

	{auto& tc = list.Add("Fashionista");
	tc.Add("Tim Gunn");
	tc.Add("Tan France");
	tc.Add("Ralph Lauren");
	tc.Add("Tom Ford");
	tc.Add("Alexander Wang");}

	{auto& tc = list.Add("Homemaker");
	tc.Add("Drew Barrymore");
	tc.Add("Joanna Gaines");
	tc.Add("Martha Stewart");
	tc.Add("Nate Berkus");
	tc.Add("Chef Jamie Oliver");}

	{auto& tc = list.Add("Philanthropist");
	tc.Add("Bill Gates");
	tc.Add("Warren Buffett");
	tc.Add("Melinda Gates");
	tc.Add("Elon Musk");
	tc.Add("Mark Zuckerberg");}

	{auto& tc = list.Add("Socialite");
	tc.Add("Paris Hilton");
	tc.Add("Kendall Jenner");
	tc.Add("Nicky Hilton");
	tc.Add("Ivanka Trump");
	tc.Add("Olivia Palermo");}

	{auto& tc = list.Add("Food/drink critic");
	tc.Add("Anthony Bourdain");
	tc.Add("Gordan Ramsey");
	tc.Add("Ruth Reichl");
	tc.Add("Guy Fieri");
	tc.Add("Andrew Zimmern");}

	{auto& tc = list.Add("Gamer");
	tc.Add("PewDiePie");
	tc.Add("Markiplier");
	tc.Add("DanTDM");
	tc.Add("Ninja");
	tc.Add("Shroud");}

	{auto& tc = list.Add("Fitness enthusiast");
	tc.Add("David Beckham");
	tc.Add("Usain Bolt");
	tc.Add("Jillian Michaels");
	tc.Add("Michael Phelps");
	tc.Add("Cristiano Ronaldo");}

	{auto& tc = list.Add("Health/wellness guru");
	tc.Add("Deepak Chopra");
	tc.Add("Dr. Oz");
	tc.Add("Mehmet Oz");
	tc.Add("Andrew Weil");
	tc.Add("Jillian Michaels");}

	{auto& tc = list.Add("Spiritual leader");
	tc.Add("Dalai Lama");
	tc.Add("Pope Francis");
	tc.Add("Joel Osteen");
	tc.Add("Thich Nhat Hanh");
	tc.Add("Deepak Chopra");}

	{auto& tc = list.Add("Parenting advice");
	tc.Add("Barack Obama");
	tc.Add("Ashton Kutcher");
	tc.Add("Chip Gaines");
	tc.Add("Kevin Jonas");
	tc.Add("Dwayne \"The Rock\" Johnson");}

	{auto& tc = list.Add("Career coach/advisor");
	tc.Add("Tony Robbins");
	tc.Add("Tim Ferriss");
	tc.Add("Marie Forleo");
	tc.Add("Gary Vaynerchuk");
	tc.Add("Mel Robbins");}

	{auto& tc = list.Add("Travel blogger");
	tc.Add("Chris Burkard");
	tc.Add("Matt Kepnes (Nomadic Matt)");
	tc.Add("Brooke Saward (World of Wanderlust)");
	tc.Add("Lauren Bullen (Gypsea Lust)");
	tc.Add("Kiersten Rich (The Blonde Abroad)");}

	{auto& tc = list.Add("Book lover/reader");
	tc.Add("Neil Gaiman");
	tc.Add("Stephen King");
	tc.Add("John Green");
	tc.Add("James Patterson");
	tc.Add("J.K. Rowling");}

	{auto& tc = list.Add("DIY enthusiast");
	tc.Add("Bob Vila");
	tc.Add("Ty Pennington");
	tc.Add("Chip Gaines");
	tc.Add("Tom Silva");
	tc.Add("Adam Savage");}

	{auto& tc = list.Add("Pet lover/owner");
	tc.Add("Ellen DeGeneres");
	tc.Add("Drew Barrymore");
	tc.Add("Mandy Moore");
	tc.Add("Chris Evans");
	tc.Add("Patrick Stewart");}

	{auto& tc = list.Add("Movie/TV critic");
	tc.Add("Roger Ebert");
	tc.Add("Leonard Maltin");
	tc.Add("Peter Travers");
	tc.Add("Gene Siskel");
	tc.Add("Richard Roeper");}

	{auto& tc = list.Add("Beauty/fashion blogger");
	tc.Add("Michelle Phan");
	tc.Add("Zoella");
	tc.Add("Huda Kattan");
	tc.Add("Rumi Neely");
	tc.Add("Tanya Burr");}

	{auto& tc = list.Add("Tech geek");
	tc.Add("Elon Musk");
	tc.Add("Bill Gates");
	tc.Add("Mark Zuckerberg");
	tc.Add("Jack Ma");
	tc.Add("Larry Page");}

	{auto& tc = list.Add("Nature lover");
	tc.Add("David Attenborough");
	tc.Add("Jane Goodall");
	tc.Add("Jacques Cousteau");
	tc.Add("Sir David Attenborough");
	tc.Add("Bear Grylls");}

	{auto& tc = list.Add("Political commentator");
	tc.Add("Anderson Cooper");
	tc.Add("Rachel Maddow");
	tc.Add("Don Lemon");
	tc.Add("Sean Hannity");
	tc.Add("Tucker Carlson");}

	{auto& tc = list.Add("Relationship expert");
	tc.Add("Esther Perel");
	tc.Add("John Gottman");
	tc.Add("Brene Brown");
	tc.Add("Gary Chapman");
	tc.Add("Sue Johnson");}

	{auto& tc = list.Add("Human rights activist");
	tc.Add("Malala Yousafzai");
	tc.Add("Colin Kaepernick");
	tc.Add("Martin Luther King Jr.");
	tc.Add("Greta Thunberg");
	tc.Add("Mahatma Gandhi");}

	{auto& tc = list.Add("Social justice warrior");
	tc.Add("Colin Kaepernick");
	tc.Add("Malala Yousafzai");
	tc.Add("Jesse Williams");
	tc.Add("Emma Gonzalez");
	tc.Add("Megan Rapinoe");}

	{auto& tc = list.Add("Music reviewer");
	tc.Add("Anthony Fantano");
	tc.Add("Rolling Stone magazine");
	tc.Add("Pitchfork");
	tc.Add("NME");
	tc.Add("Complex magazine");}

	{auto& tc = list.Add("Interior design enthusiast");
	tc.Add("Nate Berkus");
	tc.Add("Jonathan Adler");
	tc.Add("Bobby Berk");
	tc.Add("Kelly Wearstler");
	tc.Add("Emily Henderson");}

	{auto& tc = list.Add("Self-help guru");
	tc.Add("Tony Robbins");
	tc.Add("Brené Brown");
	tc.Add("Marie Forleo");
	tc.Add("Mel Robbins");
	tc.Add("Robin Sharma");}

	{auto& tc = list.Add("Life coach");
	tc.Add("Tony Robbins");
	tc.Add("Mel Robbins");
	tc.Add("Marie Forleo");
	tc.Add("Gabby Bernstein");
	tc.Add("Deepak Chopra");}

	{auto& tc = list.Add("Mental health advocate");
	tc.Add("Prince Harry");
	tc.Add("Lady Gaga");
	tc.Add("Glenn Close");
	tc.Add("Demi Lovato");
	tc.Add("Stephen Fry");}

	{auto& tc = list.Add("Promoter/event organizer");
	tc.Add("Ryan Seacrest");
	tc.Add("Guy Oseary");
	tc.Add("Simon Cowell");
	tc.Add("Mark Burnett");
	tc.Add("Live Nation");}

	{auto& tc = list.Add("Financial advisor");
	tc.Add("Warren Buffett");
	tc.Add("Dave Ramsey");
	tc.Add("Suze Orman");
	tc.Add("Robert Kiyosaki");
	tc.Add("Tony Robbins");}

	{auto& tc = list.Add("Food blogger");
	tc.Add("Ina Garten");
	tc.Add("Chrissy Teigen");
	tc.Add("Ree Drummond");
	tc.Add("Joy Wilson (Joy the Baker)");
	tc.Add("Melissa Clark");}

	{auto& tc = list.Add("Sports enthusiast");
	tc.Add("Steph Curry");
	tc.Add("Michael Phelps");
	tc.Add("Serena Williams");
	tc.Add("Usain Bolt");
	tc.Add("Tom Brady");}

	{auto& tc = list.Add("Fashion designer");
	tc.Add("Tom Ford");
	tc.Add("Alexander Wang");
	tc.Add("Marc Jacobs");
	tc.Add("Christian Siriano");
	tc.Add("Michael Kors");}

	{auto& tc = list.Add("Makeup artist");
	tc.Add("Bobbi Brown");
	tc.Add("Pat McGrath");
	tc.Add("Charlotte Tilbury");
	tc.Add("Mario Dedivanovic");
	tc.Add("Lisa Eldridge");}

	{auto& tc = list.Add("Gardening enthusiast");
	tc.Add("Monty Don");
	tc.Add("Chris Beardshaw");
	tc.Add("Joe Lamp'l");
	tc.Add("Diarmuid Gavin");
	tc.Add("Carol Klein");}

	{auto& tc = list.Add("Geek/nerd");
	tc.Add("Neil deGrasse Tyson");
	tc.Add("Mark Zuckerberg");
	tc.Add("Elon Musk");
	tc.Add("Bill Nye");
	tc.Add("Linus Thorvalds");}

	{auto& tc = list.Add("History buff");
	tc.Add("Ken Burns");
	tc.Add("Dan Carlin");
	tc.Add("David McCullough");
	tc.Add("Hampton Sides");
	tc.Add("Simon Sebag Montefiore");}

	{auto& tc = list.Add("Business owner");
	tc.Add("Mark Cuban");
	tc.Add("Tara Reed");
	tc.Add("Lori Cheek");
	tc.Add("Gary Vaynerchuk");
	tc.Add("Renae Christine");}

	{auto& tc = list.Add("Legal expert");
	tc.Add("Dan Abrams");
	tc.Add("Alan Dershowitz");
	tc.Add("Gloria Allred");
	tc.Add("Jose Baez");
	tc.Add("Kim Kardashian-West (as a legal advocate)");}

	{auto& tc = list.Add("Parenting blogger");
	tc.Add("Ashton Kutcher");
	tc.Add("Chip Gaines");
	tc.Add("Kevin Jonas");
	tc.Add("Dwayne \"The Rock\" Johnson");
	tc.Add("Neil Patrick Harris");}

	{auto& tc = list.Add("Senior citizen/retiree");
	tc.Add("Betty White");
	tc.Add("Jane Fonda");
	tc.Add("Morgan Freeman");
	tc.Add("Ian McKellen");
	tc.Add("Angela Lansbury");}

	{auto& tc = list.Add("Marriage counselor");
	tc.Add("John Gottman");
	tc.Add("Esther Perel");
	tc.Add("Brene Brown");
	tc.Add("Gary Chapman");
	tc.Add("Sue Johnson");}

	{auto& tc = list.Add("Wine connoisseur");
	tc.Add("Gary Vaynerchuk");
	tc.Add("Jancis Robinson");
	tc.Add("James Suckling");
	tc.Add("Tim Atkin");
	tc.Add("Karen MacNeil");}

	{auto& tc = list.Add("Youth advocate");
	tc.Add("Malala Yousafzai");
	tc.Add("Greta Thunberg");
	tc.Add("David Hogg");
	tc.Add("Emma Gonzalez");
	tc.Add("Ruth Bader Ginsburg");}

	{auto& tc = list.Add("Success coach");
	tc.Add("Tony Robbins");
	tc.Add("Tim Ferriss");
	tc.Add("Marie Forleo");
	tc.Add("Deepak Chopra");
	tc.Add("Darren Hardy");}

	{auto& tc = list.Add("Career woman/man");
	tc.Add("Sheryl Sandberg");
	tc.Add("Warren Buffett");
	tc.Add("Elon Musk");
	tc.Add("Tim Cook");
	tc.Add("Jack Ma");}

	{auto& tc = list.Add("Fitness coach");
	tc.Add("Jillian Michaels");
	tc.Add("Tony Horton");
	tc.Add("Shaun T");
	tc.Add("Tracy Anderson");
	tc.Add("Kayla Itsines");}

	{auto& tc = list.Add("Political blogger");
	tc.Add("Michelle Obama");
	tc.Add("Joe Scarborough");
	tc.Add("Meghan McCain");
	tc.Add("Ana Navarro");
	tc.Add("Van Jones");}

	{auto& tc = list.Add("Blogger/influencer relations");
	tc.Add("Joe Pulizzi");
	tc.Add("Lee Odden");
	tc.Add("Kristi Hines");
	tc.Add("Larry Kim");
	tc.Add("Neal Schaffer");}

	{auto& tc = list.Add("Adult entertainer");
	tc.Add("Ron Jeremy");
	tc.Add("Sasha Grey");
	tc.Add("Tera Patrick");
	tc.Add("Jenna Jameson");
	tc.Add("Jesse Jane");}

	{auto& tc = list.Add("Adult content creator");
	tc.Add("Belle Delphine");
	tc.Add("OnlyFans");
	tc.Add("AbbyPoblador");
	tc.Add("Sara Jean Underwood");
	tc.Add("Casey Calvert");}

	{auto& tc = list.Add("Adult industry critic");
	tc.Add("Erin Gloria Ryan");
	tc.Add("EJ Dickson");
	tc.Add("Lux Alptraum");
	tc.Add("Amanda Marcotte");
	tc.Add("Dan Savage");}

	{auto& tc = list.Add("Adult content reviewer");
	tc.Add("Sunny Leone");
	tc.Add("Riley Reid");
	tc.Add("Blair Williams");
	tc.Add("April Flores");
	tc.Add("Kayden Kross");}
	
	return list;
}

VectorMap<String,Vector<String>>& GetRolesSafeFemale() {
	thread_local static VectorMap<String,Vector<String>> list;
	if (!list.IsEmpty()) return list;

	{auto& tc = list.Add("Influencer");
	tc.Add("Ellen DeGeneres");
	tc.Add("Taylor Swift");
	tc.Add("Beyoncé");
	tc.Add("Jennifer Aniston");
	tc.Add("Oprah Winfrey");}

	{auto& tc = list.Add("Activist");
	tc.Add("Malala Yousafzai");
	tc.Add("Emma Watson");
	tc.Add("Greta Thunberg");
	tc.Add("Meghan Markle");
	tc.Add("Ellen Page");}

	{auto& tc = list.Add("Expert");
	tc.Add("Brené Brown");
	tc.Add("Dr. Jane Goodall");
	tc.Add("Melinda Gates");
	tc.Add("Michelle Obama");
	tc.Add("Dr. Jill Biden");}

	{auto& tc = list.Add("Comedian");
	tc.Add("Amy Poehler");
	tc.Add("Mindy Kaling");
	tc.Add("Tina Fey");
	tc.Add("Kristen Wiig");
	tc.Add("Ali Wong");}

	{auto& tc = list.Add("Politician");
	tc.Add("Kamala Harris");
	tc.Add("Alexandria Ocasio-Cortez");
	tc.Add("Nancy Pelosi");
	tc.Add("Jacinda Ardern");
	tc.Add("Angela Merkel");}

	{auto& tc = list.Add("Social Media Personality");
	tc.Add("Jenna Marbles");
	tc.Add("Lilly Singh");
	tc.Add("Zoella");
	tc.Add("Liza Koshy");
	tc.Add("Lele Pons");}

	{auto& tc = list.Add("Marketer");
	tc.Add("Sheryl Sandberg");
	tc.Add("Joanne Bradford");
	tc.Add("Carla Hendra");
	tc.Add("Mari Smith");
	tc.Add("Randi Zuckerberg");}

	{auto& tc = list.Add("Journalist");
	tc.Add("Christiane Amanpour");
	tc.Add("Rachel Maddow");
	tc.Add("Gwen Ifill");
	tc.Add("Soledad O'Brien");
	tc.Add("Christiane Amanpour");}

	{auto& tc = list.Add("Writer/Author");
	tc.Add("J.K. Rowling");
	tc.Add("Margaret Atwood");
	tc.Add("Chimamanda Ngozi Adichie");
	tc.Add("Roxane Gay");
	tc.Add("Elizabeth Gilbert");}

	{auto& tc = list.Add("Celebrity/Entertainer");
	tc.Add("Jennifer Lawrence");
	tc.Add("Lucy Hale");
	tc.Add("Mindy Kaling");
	tc.Add("Jennifer Lopez");
	tc.Add("Kerry Washington");}

	{auto& tc = list.Add("Blogger");
	tc.Add("Chiara Ferragni (The Blonde Salad)");
	tc.Add("Aimee Song (Song of Style)");
	tc.Add("Julia Engel (Gal Meets Glam)");
	tc.Add("Camila Coelho");
	tc.Add("Sazan Hendrix");}

	{auto& tc = list.Add("Entrepreneur");
	tc.Add("Sara Blakely (Spanx)");
	tc.Add("Sophia Amoruso (Girlboss)");
	tc.Add("Whitney Wolfe Herd (Bumble)");
	tc.Add("Katrina Lake (Stitch Fix)");
	tc.Add("Jessica Alba (The Honest Company)");}

	{auto& tc = list.Add("Educator");
	tc.Add("Melinda Gates");
	tc.Add("Angela Duckworth");
	tc.Add("Esther Wojcicki");
	tc.Add("Oprah Winfrey");
	tc.Add("Michelle Obama");}

	{auto& tc = list.Add("Student");
	tc.Add("Greta Thunberg");
	tc.Add("Malala Yousafzai");
	tc.Add("Emma González");
	tc.Add("Malala Fund");
	tc.Add("Team Rubicon");}

	{auto& tc = list.Add("Parent");
	tc.Add("Kristen Bell");
	tc.Add("Chrissy Teigen");
	tc.Add("Hilaria Baldwin");
	tc.Add("Gabrielle Union");
	tc.Add("Jessica Alba");}

	{auto& tc = list.Add("Athlete");
	tc.Add("Serena Williams");
	tc.Add("Simone Biles");
	tc.Add("Lindsey Vonn");
	tc.Add("Alex Morgan");
	tc.Add("Megan Rapinoe");}

	{auto& tc = list.Add("Music fan");
	tc.Add("Adele");
	tc.Add("Beyoncé");
	tc.Add("Rihanna");
	tc.Add("Ariana Grande");
	tc.Add("Billie Eilish");}

	{auto& tc = list.Add("Foodie");
	tc.Add("Padma Lakshmi");
	tc.Add("Chrissy Teigen");
	tc.Add("Ina Garten");
	tc.Add("Rachel Ray");
	tc.Add("Chrissy Tiegen");}

	{auto& tc = list.Add("Traveler");
	tc.Add("Rosa Park (Rosa's Musings)");
	tc.Add("Liz Carlson (Young Adventuress)");
	tc.Add("Kristin Luna (Camels & Chocolate)");
	tc.Add("Kiersten Rich (The Blonde Abroad)");
	tc.Add("Julia Dimon (Travel Junkie Julia)");}

	{auto& tc = list.Add("Developer/Programmer");
	tc.Add("Reshma Saujani (Girls Who Code)");
	tc.Add("Laura Kassovic (MbientLab)");
	tc.Add("Stephanie Hurlburt (Binomial)");
	tc.Add("Komal Singh (Square)");
	tc.Add("Tracy Chou (US Digital Service)");}

	{auto& tc = list.Add("Entity/Creative");
	tc.Add("Tavi Gevinson (Rookie Magazine)");
	tc.Add("Jenna Wortham (The New York Times)");
	tc.Add("Roxane Gay (Writer)");
	tc.Add("Mindy Kaling (Actress/Writer/Producer)");
	tc.Add("Olivia Wilde (Actress/Director/Producer)");}

	{auto& tc = list.Add("Scientist/Researcher");
	tc.Add("Jane Goodall");
	tc.Add("Temple Grandin");
	tc.Add("Jennifer Doudna");
	tc.Add("Mayim Bialik (PhD in Neuroscience)");
	tc.Add("Jill Bolte Taylor (Neuroscientist)");}

	{auto& tc = list.Add("Environmentalist");
	tc.Add("Jane Goodall");
	tc.Add("Greta Thunberg");
	tc.Add("Sylvia Earle");
	tc.Add("Isatou Ceesay (Plastic Recycling Advocate)");
	tc.Add("Tessa Tennant (Sustainable Investment Pioneer)");}

	{auto& tc = list.Add("Animal Lover/Activist");
	tc.Add("Jane Goodall");
	tc.Add("Temple Grandin");
	tc.Add("Ellie Goulding (UN Environment Global Goodwill Ambassador)");
	tc.Add("Rachael Ray (Animal Welfare Advocate)");
	tc.Add("Maisie Williams (Animal Rights Activist)");}

	{auto& tc = list.Add("Fashionista");
	tc.Add("Chiara Ferragni");
	tc.Add("Blake Lively");
	tc.Add("Anna Wintour (Editor-in-Chief of Vogue)");
	tc.Add("Rachel Zoe (Fashion Designer/Stylist/TV Personality)");
	tc.Add("Diane von Furstenberg (Fashion Designer/Philanthropist)");}

	{auto& tc = list.Add("Homemaker");
	tc.Add("Joanna Gaines");
	tc.Add("Martha Stewart");
	tc.Add("Ina Garten");
	tc.Add("Ree Drummond (The Pioneer Woman)");
	tc.Add("Nigella Lawson");}

	{auto& tc = list.Add("Philanthropist");
	tc.Add("Melinda Gates");
	tc.Add("Oprah Winfrey");
	tc.Add("Priscilla Chan (Chan Zuckerberg Initiative)");
	tc.Add("Laurene Powell Jobs (Emerson Collective)");
	tc.Add("Sheryl Sandberg (Lean In Foundation)");}

	{auto& tc = list.Add("Socialite");
	tc.Add("Amal Clooney");
	tc.Add("Meghan Markle");
	tc.Add("Paris Hilton");
	tc.Add("Huda Kattan");
	tc.Add("Anouska Paris");}

	{auto& tc = list.Add("Food/Drink Critic");
	tc.Add("Padma Lakshmi");
	tc.Add("Ina Garten");
	tc.Add("Giada De Laurentiis");
	tc.Add("Chrissy Teigen");
	tc.Add("Rachael Ray");}

	{auto& tc = list.Add("Gamer");
	tc.Add("Rachel Quirico");
	tc.Add("Ashley Jenkins");
	tc.Add("Mari Takahashi");
	tc.Add("Andrea Rene");
	tc.Add("Trisha Hershberger");}

	{auto& tc = list.Add("Fitness Enthusiast");
	tc.Add("Kayla Itsines");
	tc.Add("Karena Dawn and Katrina Scott (Tone It Up)");
	tc.Add("Massy Arias (Manko Fit)");
	tc.Add("Jen Widerstrom");
	tc.Add("Linn Jacobsson (Linnea Enmark)");}

	{auto& tc = list.Add("Health/Wellness Guru");
	tc.Add("Dr. Oz");
	tc.Add("Dr. Deepa Verma");
	tc.Add("Dr. Cathy Kapica");
	tc.Add("Dr. Lisa A. Price");
	tc.Add("Dr. Shiza Shahid");}

	{auto& tc = list.Add("Spiritual Leader");
	tc.Add("Oprah Winfrey");
	tc.Add("Marianne Williamson");
	tc.Add("Deepak Chopra");
	tc.Add("Gabrielle Bernstein");
	tc.Add("Eckhart Tolle");}

	{auto& tc = list.Add("Parenting Advice");
	tc.Add("Joanna Gaines");
	tc.Add("Jessica Alba");
	tc.Add("Mayim Bialik");
	tc.Add("Ellen DeGeneres");
	tc.Add("Busy Philipps");}

	{auto& tc = list.Add("Career Coach/Advisor");
	tc.Add("Oprah Winfrey");
	tc.Add("Melinda Gates");
	tc.Add("Sheryl Sandberg");
	tc.Add("Marie Forleo");
	tc.Add("Brené Brown");}

	{auto& tc = list.Add("Travel Blogger");
	tc.Add("Liz Carlson (Young Adventuress)");
	tc.Add("Kristin Addis (Be My Travel Muse)");
	tc.Add("Julia Engel (Gal Meets Glam)");
	tc.Add("Kate McCulley (Adventure Kate)");
	tc.Add("Polkadot Passport (Nicole Warne)");}

	{auto& tc = list.Add("Book Lover/Reader");
	tc.Add("Emma Watson");
	tc.Add("Reese Witherspoon");
	tc.Add("Oprah Winfrey");
	tc.Add("Jenna Bush Hager");
	tc.Add("Jessica Alba");}

	{auto& tc = list.Add("DIY Enthusiast");
	tc.Add("Martha Stewart");
	tc.Add("Homey Oh My (Amy Kim)");
	tc.Add("Jennifer Perkins");
	tc.Add("Studio DIY (Kelly Mindell)");
	tc.Add("A Beautiful Mess (Jess and Emma)");}

	{auto& tc = list.Add("Pet Lover/Owner");
	tc.Add("Ellen DeGeneres");
	tc.Add("Amanda Seyfried");
	tc.Add("Ricky Gervais");
	tc.Add("Ian Somerhalder");
	tc.Add("Marnie The Dog");}

	{auto& tc = list.Add("Movie/TV Critic");
	tc.Add("Rotten Tomatoes");
	tc.Add("MovieBob (Bob Chipman)");
	tc.Add("Renegade Cut (Leon Thomas)");
	tc.Add("Nostalgia Critic (Doug Walker)");
	tc.Add("Anya Volz");}

	{auto& tc = list.Add("Beauty/Fashion Blogger");
	tc.Add("Huda Kattan");
	tc.Add("Michelle Phan");
	tc.Add("Zoella");
	tc.Add("Camila Coelho");
	tc.Add("Jackie Aina");}

	{auto& tc = list.Add("Tech Geek");
	tc.Add("Kara Swisher");
	tc.Add("Anil Dash");
	tc.Add("Marques Brownlee (MKBHD)");
	tc.Add("Justine Ezarik (iJustine)");
	tc.Add("Emily Lakdawalla (The Planetary Society)");}

	{auto& tc = list.Add("Nature Lover");
	tc.Add("Jane Goodall");
	tc.Add("Anna McNuff (British Adventurer)");
	tc.Add("Alexia Zuberer (The Wayward Walrus)");
	tc.Add("Lauren Bath (Digital Marketing Consultant)");
	tc.Add("Anna McNuff (Champions Mentality)");}

	{auto& tc = list.Add("Political Commentator");
	tc.Add("Joy Reid");
	tc.Add("Ana Navarro");
	tc.Add("Rachel Maddow");
	tc.Add("Amy Goodman");
	tc.Add("Maggie Haberman");}

	{auto& tc = list.Add("Relationship Expert");
	tc.Add("Esther Perel");
	tc.Add("Dr. Jenn Mann");
	tc.Add("Dr. Laura Berman");
	tc.Add("Marie Forleo");
	tc.Add("Sarah Jones");}

	{auto& tc = list.Add("Human Rights Activist");
	tc.Add("Malala Yousafzai");
	tc.Add("Emma Gonzalez");
	tc.Add("Tarana Burke (founder of #MeToo movement)");
	tc.Add("Amnesty International");
	tc.Add("Laverne Cox");}

	{auto& tc = list.Add("Social Justice Warrior");
	tc.Add("Patrisse Cullors (Black Lives Matter)");
	tc.Add("DeRay Mckesson (Civil Rights Activist)");
	tc.Add("Linda Sarsour (Women's Rights Activist)");
	tc.Add("Cecile Richards (Planned Parenthood)");
	tc.Add("Opal Tometi (Black Lives Matter)");}

	{auto& tc = list.Add("Music Reviewer");
	tc.Add("Jon Pareles (The New York Times)");
	tc.Add("Jeremy Larson (Pitchfork)");
	tc.Add("Jenny Eliscu (Rolling Stone)");
	tc.Add("Dave Bry (The Guardian)");
	tc.Add("Lindsay Zoladz (Vulture)");}

	{auto& tc = list.Add("Interior Design Enthusiast");
	tc.Add("Joanna Gaines");
	tc.Add("Amber Lewis");
	tc.Add("Shea McGee");
	tc.Add("Kate Marker");
	tc.Add("Erin Hiemstra (Apartment 34)");}

	{auto& tc = list.Add("Self-Help Guru");
	tc.Add("Brené Brown");
	tc.Add("Marie Forleo");
	tc.Add("Rachel Hollis");
	tc.Add("Gabby Bernstein");
	tc.Add("Mel Robbins");}

	{auto& tc = list.Add("Life Coach");
	tc.Add("Marie Forleo");
	tc.Add("Tony Robbins");
	tc.Add("Gabby Bernstein");
	tc.Add("Mel Robbins");
	tc.Add("Brendon Burchard");}

	{auto& tc = list.Add("Mental Health Advocate");
	tc.Add("Glennon Doyle");
	tc.Add("Simone Biles");
	tc.Add("Jen Gotch (founder of Ban.do)");
	tc.Add("Michelle Williams");
	tc.Add("Demi Lovato");}

	{auto& tc = list.Add("Promoter/Event Organizer");
	tc.Add("Oprah Winfrey");
	tc.Add("Michelle Obama");
	tc.Add("Marie Forleo");
	tc.Add("Melinda Gates");
	tc.Add("Arianna Huffington");}

	{auto& tc = list.Add("Financial Advisor");
	tc.Add("Suze Orman");
	tc.Add("Elizabeth Warren");
	tc.Add("Mellody Hobson");
	tc.Add("Rachel Cruze");
	tc.Add("Farnoosh Torabi");}

	{auto& tc = list.Add("Food Blogger");
	tc.Add("Joy Bauer");
	tc.Add("Alison Roman");
	tc.Add("Tieghan Gerard (Half Baked Harvest)");
	tc.Add("Angela Davis (The Kitchenista Diaries)");
	tc.Add("Joy McCarthy (Joyous Health)");}

	{auto& tc = list.Add("Sports Enthusiast");
	tc.Add("Katie Sowers");
	tc.Add("Julie Ertz");
	tc.Add("Lindsey Vonn");
	tc.Add("Sue Bird");
	tc.Add("Alex Morgan");}

	{auto& tc = list.Add("Fashion Designer");
	tc.Add("Tory Burch");
	tc.Add("Stella McCartney");
	tc.Add("Diane Von Furstenberg");
	tc.Add("Donna Karan");
	tc.Add("Carolina Herrera");}

	{auto& tc = list.Add("Makeup Artist");
	tc.Add("Bobbi Brown");
	tc.Add("Pat McGrath");
	tc.Add("Lisa Eldridge");
	tc.Add("Charlotte Tilbury");
	tc.Add("Jaclyn Hill");}

	{auto& tc = list.Add("Gardening Enthusiast");
	tc.Add("Monty Don");
	tc.Add("Martha Stewart");
	tc.Add("Melinda Myers");
	tc.Add("Nikki Jardin");
	tc.Add("Maureen Gilmer");}

	{auto& tc = list.Add("Geek/Nerd");
	tc.Add("Felicia Day");
	tc.Add("Mindy Kaling");
	tc.Add("Aisha Tyler");
	tc.Add("Ashley Eckstein");
	tc.Add("Gwendoline Christie");}

	{auto& tc = list.Add("History Buff");
	tc.Add("Melinda Gates");
	tc.Add("Emma Gonzalez");
	tc.Add("Brené Brown");
	tc.Add("Michelle Obama");
	tc.Add("Jane Goodall");}

	{auto& tc = list.Add("Business Owner");
	tc.Add("Sara Blakely (Spanx)");
	tc.Add("Sophia Amoruso (Girlboss)");
	tc.Add("Katrina Lake (Stitch Fix)");
	tc.Add("Jenn Hyman (Rent The Runway)");
	tc.Add("Jessica Alba (The Honest Company)");}

	{auto& tc = list.Add("Legal Expert");
	tc.Add("Ruth Bader Ginsburg");
	tc.Add("Sonia Sotomayor");
	tc.Add("Jeanine Pirro");
	tc.Add("Ari Melber");
	tc.Add("Laura Coates");}

	{auto& tc = list.Add("Parenting Blogger");
	tc.Add("Anna White (Maestro Classics)");
	tc.Add("Ellen Seidman (Love That Max)");
	tc.Add("Gabrielle Blair (Design Mom)");
	tc.Add("Kelly Wickham Hurst (Mocha Momma)");
	tc.Add("Sarah Bessey (An Inch of Gray)");}

	{auto& tc = list.Add("Senior Citizen/Retiree");
	tc.Add("Rita Moreno");
	tc.Add("Judi Dench");
	tc.Add("Maggie Smith");
	tc.Add("Betty White");
	tc.Add("Jane Fonda");}

	{auto& tc = list.Add("Marriage Counselor");
	tc.Add("Esther Perel");
	tc.Add("Dr. Sue Johnson");
	tc.Add("Rachel Sussman");
	tc.Add("Les Parrot");
	tc.Add("Gary Chapman");}

	{auto& tc = list.Add("Wine Connoisseur");
	tc.Add("Jancis Robinson");
	tc.Add("Jamie Goode");
	tc.Add("Alice Feiring");
	tc.Add("Elizabeth Schneider");
	tc.Add("Emma Rice");}

	{auto& tc = list.Add("Youth advocate");
	tc.Add("Malala Yousafzai");
	tc.Add("Greta Thunberg");
	tc.Add("Mari Copeny");
	tc.Add("Jazz Jennings");
	tc.Add("Yara Shahidi");}

	{auto& tc = list.Add("Success coach");
	tc.Add("Mel Robbins");
	tc.Add("Marie Forleo");
	tc.Add("Tony Robbins");
	tc.Add("Gabrielle Bernstein");
	tc.Add("Brené Brown");}

	{auto& tc = list.Add("Career woman");
	tc.Add("Indra Nooyi");
	tc.Add("Sheryl Sandberg");
	tc.Add("Arianna Huffington");
	tc.Add("Mary Barra");
	tc.Add("Oprah Winfrey");}

	{auto& tc = list.Add("Fitness coach");
	tc.Add("Jillian Michaels");
	tc.Add("Kayla Itsines");
	tc.Add("Tracy Anderson");
	tc.Add("Simone De La Rue");
	tc.Add("Koya Webb");}

	{auto& tc = list.Add("Political blogger");
	tc.Add("Rachel Maddow");
	tc.Add("Ana Navarro");
	tc.Add("Symone Sanders");
	tc.Add("Sarah Cooper");
	tc.Add("Melissa Harris-Perry");}

	{auto& tc = list.Add("Blogger/influencer relations");
	tc.Add("Aimee Song");
	tc.Add("Chiara Ferragni");
	tc.Add("Amber Fillerup");
	tc.Add("Negin Mirsalehi");
	tc.Add("Danielle Bernstein");}

	{auto& tc = list.Add("Adult entertainer");
	tc.Add("Jenna Jameson");
	tc.Add("Stormy Daniels");
	tc.Add("Janice Griffith");
	tc.Add("Asa Akira");
	tc.Add("Briana Banks");}

	{auto& tc = list.Add("Adult content creator");
	tc.Add("Asa Akira");
	tc.Add("Angela White");
	tc.Add("Lena Paul");
	tc.Add("Kendra Sunderland");
	tc.Add("Casey Calvert");}

	{auto& tc = list.Add("Adult industry critic");
	tc.Add("Nina Hartley");
	tc.Add("Stormy Daniels");
	tc.Add("Kendra Holliday");
	tc.Add("Jaclyn Friedman");
	tc.Add("Jenna Jameson");}

	{auto& tc = list.Add("Adult content reviewer");
	tc.Add("Aria Taylor");
	tc.Add("Lily LaBeau");
	tc.Add("Riley Reid");
	tc.Add("Asa Akira");
	tc.Add("Stoya");}
	
	return list;
}

VectorMap<String,Vector<String>>& GetRolesUnsafeMale() {
	thread_local static VectorMap<String,Vector<String>> list;
	if (!list.IsEmpty()) return list;
	{auto& tc = list.Add("Influencer");
	tc.Add("Logan Paul");
	tc.Add("Jake Paul");
	tc.Add("Jeffree Star");
	tc.Add("PewDiePie");
	tc.Add("Shane Dawson");}

	{auto& tc = list.Add("Activist");
	tc.Add("Alex Jones");
	tc.Add("Milo Yiannopoulos");
	tc.Add("Tommy Robinson");
	tc.Add("Richard Spencer");
	tc.Add("Gavin McInnes");}

	{auto& tc = list.Add("Expert");
	tc.Add("Jordan Peterson");
	tc.Add("Sam Harris");
	tc.Add("Mike Cernovich");
	tc.Add("Ben Shapiro");
	tc.Add("Stefan Molyneux");}

	{auto& tc = list.Add("Comedian");
	tc.Add("Louis C.K.");
	tc.Add("Bill Maher");
	tc.Add("Ricky Gervais");
	tc.Add("Jim Jefferies");
	tc.Add("Russell Brand");}

	{auto& tc = list.Add("Politician");
	tc.Add("Donald Trump");
	tc.Add("Nigel Farage");
	tc.Add("Boris Johnson");
	tc.Add("Matteo Salvini");
	tc.Add("Jair Bolsonaro ");}

	{auto& tc = list.Add("Social media personality");
	tc.Add("Logan Paul");
	tc.Add("Jake Paul");
	tc.Add("James Charles");
	tc.Add("Tana Mongeau");
	tc.Add("Lele Pons");}

	{auto& tc = list.Add("Marketer");
	tc.Add("Tai Lopez");
	tc.Add("Tim Sykes");
	tc.Add("Grant Cardone");
	tc.Add("Dan Bilzerian");
	tc.Add("Anthony Morrison");}

	{auto& tc = list.Add("Journalist");
	tc.Add("Tomi Lahren");
	tc.Add("Charlie Kirk");
	tc.Add("Milo Yiannopoulos");
	tc.Add("Alex Jones");
	tc.Add("Paul Joseph Watson");}

	{auto& tc = list.Add("Writer/author");
	tc.Add("Jordan Peterson");
	tc.Add("Ben Shapiro");
	tc.Add("Milo Yiannopoulos");
	tc.Add("David Horowitz");
	tc.Add("Alex Jones");}

	{auto& tc = list.Add("Celebrity/entertainer");
	tc.Add("Kanye West");
	tc.Add("Chris Brown");
	tc.Add("Johnny Depp");
	tc.Add("Mel Gibson");
	tc.Add("Alec Baldwin");}

	{auto& tc = list.Add("Blogger");
	tc.Add("Milo Yiannopoulos");
	tc.Add("Mike Cernovich");
	tc.Add("Dave Rubin");
	tc.Add("Paul Joseph Watson");
	tc.Add("James Allsup");}

	{auto& tc = list.Add("Entrepreneur");
	tc.Add("Gary Vaynerchuk");
	tc.Add("Tai Lopez");
	tc.Add("Tim Ferriss");
	tc.Add("Grant Cardone");
	tc.Add("Daymond John");}

	{auto& tc = list.Add("Educator");
	tc.Add("Jordan Peterson");
	tc.Add("Ben Shapiro");
	tc.Add("Dave Rubin");
	tc.Add("Tomi Lahren");
	tc.Add("Milo Yiannopoulos");}

	{auto& tc = list.Add("Student");
	tc.Add("Milo Yiannopoulos");
	tc.Add("Richard Spencer");
	tc.Add("Jack Posobiec");
	tc.Add("Laura Loomer");
	tc.Add("Ben Shapiro");}

	{auto& tc = list.Add("Parent");
	tc.Add("Jordan Peterson");
	tc.Add("Ben Shapiro");
	tc.Add("Milo Yiannopoulos");
	tc.Add("Mike Cernovich");
	tc.Add("Alex Jones");}

	{auto& tc = list.Add("Athlete");
	tc.Add("Colin Kaepernick");
	tc.Add("Kyrie Irving");
	tc.Add("Dennis Rodman");
	tc.Add("Marshawn Lynch");
	tc.Add("Greg Hardy");}

	{auto& tc = list.Add("Music fan");
	tc.Add("Kanye West");
	tc.Add("Chris Brown");
	tc.Add("Tyga");
	tc.Add("Migos");
	tc.Add("Kodak Black");}

	{auto& tc = list.Add("Foodie");
	tc.Add("Gordon Ramsay");
	tc.Add("Guy Fieri");
	tc.Add("Anthony Bourdain");
	tc.Add("Bobby Flay");
	tc.Add("Wolfgang Puck");}

	{auto& tc = list.Add("Traveler");
	tc.Add("Anthony Bourdain");
	tc.Add("Rick Steves");
	tc.Add("Bear Grylls");
	tc.Add("Johnny Depp");
	tc.Add("Jack Osbourne");}

	{auto& tc = list.Add("Developer/programmer");
	tc.Add("Mark Zuckerberg");
	tc.Add("Elon Musk");
	tc.Add("Jack Dorsey");
	tc.Add("Bill Gates");
	tc.Add("Jeff Bezos");}

	{auto& tc = list.Add("Entity/creative");
	tc.Add("Kanye West");
	tc.Add("Donald Glover");
	tc.Add("Banksy");
	tc.Add("Tyler The Creator");
	tc.Add("Amanda Palmer");}

	{auto& tc = list.Add("Scientist/researcher");
	tc.Add("Richard Dawkins");
	tc.Add("Sam Harris");
	tc.Add("Neil deGrasse Tyson");
	tc.Add("Stephen Hawking");
	tc.Add("Bill Nye");}

	{auto& tc = list.Add("Environmentalist");
	tc.Add("Donald Trump");
	tc.Add("Rush Limbaugh");
	tc.Add("Alex Jones");
	tc.Add("Mike Pence");
	tc.Add("Ted Cruz");}

	{auto& tc = list.Add("Animal lover/activist");
	tc.Add("Ricky Gervais");
	tc.Add("Steve Irwin");
	tc.Add("PETA");
	tc.Add("Robert Irwin");
	tc.Add("Jim Carrey");}

	{auto& tc = list.Add("Fashionista");
	tc.Add("Kanye West");
	tc.Add("Jared Leto");
	tc.Add("Pharrell Williams");
	tc.Add("Billy Porter");
	tc.Add("Harry Styles");}

	{auto& tc = list.Add("Homemaker");
	tc.Add("Jordan Peterson");
	tc.Add("Dave Ramsey");
	tc.Add("Ron Paul");
	tc.Add("Alex Jones");
	tc.Add("Ben Shapiro");}

	{auto& tc = list.Add("Philanthropist");
	tc.Add("George Soros");
	tc.Add("Bill Gates");
	tc.Add("Warren Buffett");
	tc.Add("Mark Zuckerberg");
	tc.Add("Carlos Slim");}

	{auto& tc = list.Add("Socialite");
	tc.Add("Paris Hilton");
	tc.Add("Kim Kardashian-West");
	tc.Add("Khloe Kardashian");
	tc.Add("Kylie Jenner");
	tc.Add("Kendall Jenner");}

	{auto& tc = list.Add("Food/drink critic");
	tc.Add("Anthony Bourdain");
	tc.Add("Gordon Ramsay");
	tc.Add("Guy Fieri");
	tc.Add("Bobby Flay");
	tc.Add("Andrew Zimmern");}

	{auto& tc = list.Add("Gamer");
	tc.Add("PewDiePie");
	tc.Add("Ninja");
	tc.Add("Markiplier");
	tc.Add("Jacksepticeye");
	tc.Add("FaZe Rug");}

	{auto& tc = list.Add("Fitness enthusiast");
	tc.Add("Greg Doucette");
	tc.Add("Bradley Martyn");
	tc.Add("Christian Guzman");
	tc.Add("CT Fletcher");
	tc.Add("Joe Rogan");}

	{auto& tc = list.Add("Health/wellness guru");
	tc.Add("Dr. Oz");
	tc.Add("Deepak Chopra");
	tc.Add("Gwyneth Paltrow");
	tc.Add("Jillian Michaels");
	tc.Add("Kourtney Kardashian");}

	{auto& tc = list.Add("Spiritual leader");
	tc.Add("Joel Osteen");
	tc.Add("Pat Robertson");
	tc.Add("Jim Bakker");
	tc.Add("Benny Hinn");
	tc.Add("Deepak Chop");}

	{auto& tc = list.Add("Parenting advice");
	tc.Add("Jordan Peterson");
	tc.Add("Ben Shapiro");
	tc.Add("Milo Yiannopoulos");
	tc.Add("Mike Cernovich");
	tc.Add("Alex Jones");}

	{auto& tc = list.Add("Career coach/advisor");
	tc.Add("Gary Vaynerchuk");
	tc.Add("Dan Lok");
	tc.Add("Tony Robbins");
	tc.Add("Grant Cardone");
	tc.Add("Tim Ferriss");}

	{auto& tc = list.Add("Travel blogger");
	tc.Add("Mike Posner");
	tc.Add("Sam Kolder");
	tc.Add("Daniel Kordan");
	tc.Add("Mark Wiens");
	tc.Add("Yellow Brick Cinema");}

	{auto& tc = list.Add("Book lover/reader");
	tc.Add("Jordan Peterson");
	tc.Add("Ben Shapiro");
	tc.Add("Dave Rubin");
	tc.Add("Milo Yiannopoulos");
	tc.Add("Stephen King");}

	{auto& tc = list.Add("DIY enthusiast");
	tc.Add("Adam Savage");
	tc.Add("Bob Vila");
	tc.Add("Ty Pennington");
	tc.Add("Mike Holmes");
	tc.Add("Tim Taylor (from \"Home Improvement\")");}

	{auto& tc = list.Add("Pet lover/owner");
	tc.Add("Joe Exotic");
	tc.Add("Tia Torres");
	tc.Add("Caesar Milan");
	tc.Add("Jackson Galaxy");
	tc.Add("Steve Irwin");}

	{auto& tc = list.Add("Movie/TV critic");
	tc.Add("Charlie Sheen");
	tc.Add("Alec Baldwin");
	tc.Add("Kevin Spacey");
	tc.Add("James Franco");
	tc.Add("Johnny Depp");}

	{auto& tc = list.Add("Beauty/fashion blogger");
	tc.Add("Jeffree Star");
	tc.Add("James Charles");
	tc.Add("Tati Westbrook");
	tc.Add("Huda Kattan");
	tc.Add("Manny MUA");}

	{auto& tc = list.Add("Tech geek");
	tc.Add("Mark Zuckerberg");
	tc.Add("Elon Musk");
	tc.Add("Bill Gates");
	tc.Add("Jeff Bezos");
	tc.Add("Steve Jobs");}

	{auto& tc = list.Add("Nature lover");
	tc.Add("Bear Grylls");
	tc.Add("Steve Irwin");
	tc.Add("David Attenborough");
	tc.Add("David Bellamy");
	tc.Add("Jeff Corwin");}

	{auto& tc = list.Add("Political commentator");
	tc.Add("Ben Shapiro");
	tc.Add("Milo Yiannopoulos");
	tc.Add("Steven Crowder");
	tc.Add("Glenn Beck");
	tc.Add("Rush Limbaugh");}

	{auto& tc = list.Add("Relationship expert");
	tc.Add("Jordan Peterson");
	tc.Add("Ben Shapiro");
	tc.Add("Milo Yiannopoulos");
	tc.Add("Mike Cernovich");
	tc.Add("Alex Jones");}

	{auto& tc = list.Add("Human rights activist");
	tc.Add("George Soros");
	tc.Add("Noam Chomsky");
	tc.Add("Salman Rushdie");
	tc.Add("Chelsea Manning");
	tc.Add("Malala Yousafzai");}

	{auto& tc = list.Add("Social justice warrior");
	tc.Add("Shaun King");
	tc.Add("Linda Sarsour");
	tc.Add("Keith Ellison");
	tc.Add("Al Sharpton");
	tc.Add("Maxine Waters");}

	{auto& tc = list.Add("Music reviewer");
	tc.Add("Anthony Fantano");
	tc.Add("Todd in the Shadows");
	tc.Add("Needle Drop");
	tc.Add("TheReportOfTheWeek");
	tc.Add("theneedledrop");}

	{auto& tc = list.Add("Interior design enthusiast");
	tc.Add("Bobby Berk");
	tc.Add("Nate Berkus");
	tc.Add("Chip and Joanna Gaines");
	tc.Add("Ryan Korban");
	tc.Add("Jeremiah Brent");}

	{auto& tc = list.Add("Self-help guru");
	tc.Add("Tony Robbins");
	tc.Add("Deepak Chopra");
	tc.Add("Joel Osteen");
	tc.Add("Eckhart Tolle");
	tc.Add("Louise Hay");}

	{auto& tc = list.Add("Life coach");
	tc.Add("Jordan Peterson");
	tc.Add("Oprah Winfrey");
	tc.Add("Marie Forleo");
	tc.Add("Mel Robbins");
	tc.Add("Tony Robbins");}

	{auto& tc = list.Add("Mental health advocate");
	tc.Add("Pete Davidson");
	tc.Add("Demi Lovato");
	tc.Add("Lady Gaga");
	tc.Add("Michael Phelps");
	tc.Add("Dwayne \"The Rock\" Johnson");}

	{auto& tc = list.Add("Promoter/event organizer");
	tc.Add("Billy McFarland (Fyre Festival)");
	tc.Add("Ja Rule (Fyre Festival)");
	tc.Add("Jordan Belfort (The Wolf of Wall Street)");
	tc.Add("Harvey Weinstein");
	tc.Add("Donald Trump (Miss Universe pageant)");}

	{auto& tc = list.Add("Financial advisor");
	tc.Add("Grant Cardone");
	tc.Add("Dave Ramsey");
	tc.Add("Robert Kiyosaki");
	tc.Add("Suze Orman");
	tc.Add("Jim Cramer");}

	{auto& tc = list.Add("Food blogger");
	tc.Add("Adam Richman");
	tc.Add("Andrew Zimmern");
	tc.Add("Anthony Bourdain");
	tc.Add("Samin Nosrat");
	tc.Add("Jamie Oliver");}

	{auto& tc = list.Add("Sports enthusiast");
	tc.Add("Stephen A. Smith");
	tc.Add("Skip Bayless");
	tc.Add("Colin Cowherd");
	tc.Add("Clay Travis");
	tc.Add("Shannon Sharpe");}

	{auto& tc = list.Add("Fashion designer");
	tc.Add("Marc Jacobs");
	tc.Add("Michael Kors");
	tc.Add("Karl Lagerfeld");
	tc.Add("Tom Ford");
	tc.Add("Marc Ecko");}

	{auto& tc = list.Add("Makeup artist");
	tc.Add("Jeffree Star");
	tc.Add("James Charles");
	tc.Add("Manny MUA");
	tc.Add("NikkieTutorials");
	tc.Add("Tati Westbrook");}

	{auto& tc = list.Add("Gardening enthusiast");
	tc.Add("Monty Don");
	tc.Add("Alan Titchmarsh");
	tc.Add("Chris Packham");
	tc.Add("Joe Swift");
	tc.Add("Carol Klein");}

	{auto& tc = list.Add("Geek/nerd");
	tc.Add("Elon Musk");
	tc.Add("Neil DeGrasse Tyson");
	tc.Add("Bill Gates");
	tc.Add("Mark Zuckerberg");
	tc.Add("Richard Dawkins");}

	{auto& tc = list.Add("History buff");
	tc.Add("Dan Carlin");
	tc.Add("Ken Burns");
	tc.Add("David McCullough");
	tc.Add("Ron Chernow");
	tc.Add("Shelby Foote");}

	{auto& tc = list.Add("Business owner");
	tc.Add("Mark Cuban");
	tc.Add("Richard Branson");
	tc.Add("Elon Musk");
	tc.Add("Jeff Bezos");
	tc.Add("Bill Gates");}

	{auto& tc = list.Add("Legal expert");
	tc.Add("Alan Dershowitz");
	tc.Add("Joseph diGenova");
	tc.Add("Rudy Giuliani");
	tc.Add("Ken Starr");
	tc.Add("Jay Sekulow");}

	{auto& tc = list.Add("Parenting blogger");
	tc.Add("Jordan Peterson");
	tc.Add("Ben Shapiro");
	tc.Add("Milo Yiannopoulos");
	tc.Add("Mike Cernovich");
	tc.Add("Alex Jones");}

	{auto& tc = list.Add("Senior citizen/retiree");
	tc.Add("George Soros");
	tc.Add("Warren Buffett");
	tc.Add("Richard Branson");
	tc.Add("Larry King");
	tc.Add("Rupert Murdoch");}

	{auto& tc = list.Add("Marriage counselor");
	tc.Add("Dr. Phil McGraw");
	tc.Add("Dr. Laura Schlessinger");
	tc.Add("Dr. Ruth Westheimer");
	tc.Add("Dr. John Gottman");
	tc.Add("Steve Harvey");}

	{auto& tc = list.Add("Wine connoisseur");
	tc.Add("Gary Vaynerchuk");
	tc.Add("Anthony Bourdain");
	tc.Add("Tim Ferriss");
	tc.Add("Master Sommelier");
	tc.Add("Jancis Robinson");}

	{auto& tc = list.Add("Youth advocate");
	tc.Add("David Hogg");
	tc.Add("Greta Thunberg");
	tc.Add("Parkland students");
	tc.Add("Malala Yousafzai");
	tc.Add("Emma Gonzalez");}

	{auto& tc = list.Add("Success coach");
	tc.Add("Dan Lok");
	tc.Add("Tony Robbins");
	tc.Add("Gary Vaynerchuk");
	tc.Add("Grant Cardone");
	tc.Add("Brendon Burchard");}

	{auto& tc = list.Add("Career woman/man");
	tc.Add("Ivanka Trump");
	tc.Add("Sheryl Sandberg");
	tc.Add("Rachel Hollis");
	tc.Add("Melinda Gates");
	tc.Add("Indra Nooyi");}

	{auto& tc = list.Add("Fitness coach");
	tc.Add("Greg Doucette");
	tc.Add("Bradley Martyn");
	tc.Add("Amanda Bucci");
	tc.Add("CT Fletcher");
	tc.Add("Chris Jones");}

	{auto& tc = list.Add("Political blogger");
	tc.Add("Milo Yiannopoulos");
	tc.Add("Tucker Carlson");
	tc.Add("Piers Morgan");
	tc.Add("Ben Shapiro");
	tc.Add("Steve Bannon");}

	{auto& tc = list.Add("Blogger/influencer relations");
	tc.Add("Jeffree Star");
	tc.Add("James Charles");
	tc.Add("Tana Mongeau");
	tc.Add("Logan Paul");
	tc.Add("Jake Paul");}

	{auto& tc = list.Add("Adult entertainer");
	tc.Add("Stormy Daniels");
	tc.Add("Ron Jeremy");
	tc.Add("Jenna Jameson");
	tc.Add("Lisa Ann");
	tc.Add("Sasha Grey");}

	{auto& tc = list.Add("Adult content creator");
	tc.Add("James Deen");
	tc.Add("Stoya");
	tc.Add("Asa Akira");
	tc.Add("Riley Reid");
	tc.Add("Johnny Sins");}

	{auto& tc = list.Add("Adult industry critic");
	tc.Add("Gail Dines");
	tc.Add("Shelley Lubben");
	tc.Add("Pamela Stenzel");
	tc.Add("Donny Pauling");
	tc.Add("Nicholas Kristof");}

	{auto& tc = list.Add("Adult content reviewer");
	tc.Add("Keemstar");
	tc.Add("iDubbbz");
	tc.Add("h3h3 Productions");
	tc.Add("Ethan Klein");
	tc.Add("Michael McCrudden");}

	return list;
}

VectorMap<String,Vector<String>>& GetRolesUnsafeFemale() {
	thread_local static VectorMap<String,Vector<String>> list;
	if (!list.IsEmpty()) return list;

	{auto& tc = list.Add("Influencer");
	tc.Add("Kylie Jenner");
	tc.Add("Kim Kardashian-West");
	tc.Add("Amanda Bynes");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Activist");
	tc.Add("Azealia Banks");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Kathy Griffin");
	tc.Add("Wendy Williams");}

	{auto& tc = list.Add("Expert");
	tc.Add("Ann Coulter");
	tc.Add("Chelsea Handler");
	tc.Add("Wendy Williams");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Comedian");
	tc.Add("Kathy Griffin");
	tc.Add("Chelsea Handler");
	tc.Add("Roseanne Barr");
	tc.Add("Wendy Williams");
	tc.Add("Azealia Banks");}

	{auto& tc = list.Add("Politician");
	tc.Add("Ann Coulter");
	tc.Add("Roseanne Barr");
	tc.Add("Amanda Bynes");
	tc.Add("Kim Kardashian-West");
	tc.Add("Azealia Banks");}

	{auto& tc = list.Add("Social media personality");
	tc.Add("Kylie Jenner");
	tc.Add("Kim Kardashian-West");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Wendy Williams");}

	{auto& tc = list.Add("Marketer");
	tc.Add("Kim Kardashian-West");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Kathy Griffin");}

	{auto& tc = list.Add("Journalist");
	tc.Add("Ann Coulter");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Wendy Williams");
	tc.Add("Kathy Griffin");}

	{auto& tc = list.Add("Writer/author");
	tc.Add("Chelsea Handler");
	tc.Add("Amanda Bynes");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Celebrity/entertainer");
	tc.Add("Kim Kardashian-West");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Roseanne Barr");
	tc.Add("Kylie Jenner");}

	{auto& tc = list.Add("Blogger");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");
	tc.Add("Kathy Griffin");}

	{auto& tc = list.Add("Entrepreneur");
	tc.Add("Kylie Jenner");
	tc.Add("Kim Kardashian-West");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Azealia Banks");}

	{auto& tc = list.Add("Educator");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Amanda Bynes");
	tc.Add("Azealia Banks");
	tc.Add("Ann Coulter");}

	{auto& tc = list.Add("Student");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Azealia Banks");
	tc.Add("Roseanne Barr");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Parent");
	tc.Add("Kim Kardashian-West");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Roseanne Barr");
	tc.Add("Azealia Banks");}

	{auto& tc = list.Add("Athlete");
	tc.Add("Kim Kardashian-West");
	tc.Add("Lindsay Lohan");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");
	tc.Add("Azealia Banks");}

	{auto& tc = list.Add("Music fan");
	tc.Add("Azealia Banks");
	tc.Add("Kim Kardashian-West");
	tc.Add("Lindsay Lohan");
	tc.Add("Amanda Bynes");
	tc.Add("Wendy Williams");}

	{auto& tc = list.Add("Foodie");
	tc.Add("Amanda Bynes");
	tc.Add("Kylie Jenner");
	tc.Add("Lindsay Lohan");
	tc.Add("Roseanne Barr");
	tc.Add("Azealia Banks");}

	{auto& tc = list.Add("Traveler");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");
	tc.Add("Roseanne Barr");
	tc.Add("Azealia Banks");}

	{auto& tc = list.Add("Developer/programmer");
	tc.Add("Chelsea Handler");
	tc.Add("Amanda Bynes");
	tc.Add("Azealia Banks");
	tc.Add("Roseanne Barr");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Entity/creative");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Azealia Banks");
	tc.Add("Roseanne Barr");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Scientist/researcher");
	tc.Add("Ann Coulter");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Wendy Williams");
	tc.Add("Amanda Bynes");}

	{auto& tc = list.Add("Environmentalist");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Amanda Bynes");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Animal lover/activist");
	tc.Add("Amanda Bynes");
	tc.Add("Chelsea Handler");
	tc.Add("Azealia Banks");
	tc.Add("Roseanne Barr");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Fashionista");
	tc.Add("Kylie Jenner");
	tc.Add("Kim Kardashian-West");
	tc.Add("Lindsay Lohan");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Homemaker");
	tc.Add("Roseanne Barr");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Philanthropist");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Roseanne Barr");
	tc.Add("Kathy Griffin");}

	{auto& tc = list.Add("Socialite");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");
	tc.Add("Roseanne Barr");
	tc.Add("Azealia Banks");}

	{auto& tc = list.Add("Food/drink critic");
	tc.Add("Amanda Bynes");
	tc.Add("Azealia Banks");
	tc.Add("Kim Kardashian-West");
	tc.Add("Lindsay Lohan");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Gamer");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Wendy Williams");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Fitness enthusiast");
	tc.Add("Amanda Bynes");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");
	tc.Add("Rosie O'Donnell");}

	{auto& tc = list.Add("Health/wellness guru");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Spiritual leader");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Parenting advice");
	tc.Add("Amanda Bynes");
	tc.Add("Chelsea Handler");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Career coach/advisor");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Kim Kardashian-West");
	tc.Add("Chelsea Handler");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Travel blogger");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Roseanne Barr");
	tc.Add("Kim Kardashian-West");
	tc.Add("Azealia Banks");}

	{auto& tc = list.Add("Book lover/reader");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("DIY enthusiast");
	tc.Add("Amanda Bynes");
	tc.Add("Chelsea Handler");
	tc.Add("Kim Kardashian-West");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Pet lover/owner");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");
	tc.Add("Azealia Banks");
	tc.Add("Chelsea Handler");}

	{auto& tc = list.Add("Movie/TV critic");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Beauty/fashion blogger");
	tc.Add("Kylie Jenner");
	tc.Add("Kim Kardashian-West");
	tc.Add("Roseanne Barr");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");}

	{auto& tc = list.Add("Tech geek");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");
	tc.Add("Chelsea Handler");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Nature lover");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");
	tc.Add("Chelsea Handler");}

	{auto& tc = list.Add("Political commentator");
	tc.Add("Ann Coulter");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");}

	{auto& tc = list.Add("Relationship expert");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Kim Kardashian-West");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");}

	{auto& tc = list.Add("Human rights activist");
	tc.Add("Roseanne Barr");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Chelsea Handler");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Social justice warrior");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Music reviewer");
	tc.Add("Amanda Bynes");
	tc.Add("Azealia Banks");
	tc.Add("Roseanne Barr");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Interior design enthusiast");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");
	tc.Add("Kim Kardashian-West");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Self-help guru");
	tc.Add("Azealia Banks");
	tc.Add("Chelsea Handler");
	tc.Add("Amanda Bynes");
	tc.Add("Kim Kardashian-West");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Life coach");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Mental health advocate");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Promoter/event organizer");
	tc.Add("Roseanne Barr");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Chelsea Handler");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Financial advisor");
	tc.Add("Azealia Banks");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Amanda Bynes");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Food blogger");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");
	tc.Add("Azealia Banks");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Sports enthusiast");
	tc.Add("Kim Kardashian-West");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Fashion designer");
	tc.Add("Kim Kardashian-West");
	tc.Add("Kylie Jenner");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Makeup artist");
	tc.Add("Kylie Jenner");
	tc.Add("Kim Kardashian-West");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Gardening enthusiast");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Geek/nerd");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Chelsea Handler");
	tc.Add("Kim Kardashian-West");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("History buff");
	tc.Add("Roseanne Barr");
	tc.Add("Amanda Bynes");
	tc.Add("Chelsea Handler");
	tc.Add("Azealia Banks");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Business owner");
	tc.Add("Kim Kardashian-West");
	tc.Add("Amanda Bynes");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Legal expert");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Parenting blogger");
	tc.Add("Kim Kardashian-West");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Azealia Banks");
	tc.Add("Roseanne Barr");}

	{auto& tc = list.Add("Senior citizen/retiree");
	tc.Add("Azealia Banks");
	tc.Add("Chelsea Handler");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");
	tc.Add("Ann Coulter");}

	{auto& tc = list.Add("Marriage counselor");
	tc.Add("Amanda Bynes");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");
	tc.Add("Kim Kardashian-West");
	tc.Add("Chelsea Handler");}

	{auto& tc = list.Add("Wine connoisseur");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Azealia Banks");
	tc.Add("Roseanne Barr");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Youth advocate");
	tc.Add("Azealia Banks");
	tc.Add("Chelsea Handler");
	tc.Add("Kim Kardashian-West");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Success coach");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Chelsea Handler");
	tc.Add("Kim Kardashian-West");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Career woman/man");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");
	tc.Add("Kim Kardashian-West");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Fitness coach");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");
	tc.Add("Chelsea Handler");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Political blogger");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Lindsay Lohan");
	tc.Add("Chelsea Handler");
	tc.Add("Kim Kardashian-West");}

	{auto& tc = list.Add("Blogger/influencer relations");
	tc.Add("Azealia Banks");
	tc.Add("Amanda Bynes");
	tc.Add("Roseanne Barr");
	tc.Add("Kim Kardashian-West");
	tc.Add("Lindsay Lohan");}

	{auto& tc = list.Add("Adult entertainer");
	tc.Add("Azealia Banks");
	tc.Add("Jonathan Cheban");
	tc.Add("Kim Kardashian-West");
	tc.Add("Lindsay Lohan");
	tc.Add("Amanda Bynes");}

	{auto& tc = list.Add("Adult content creator");
	tc.Add("Kim Kardashian-West");
	tc.Add("Amanda Bynes");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");
	tc.Add("Joe Rogan");}

	{auto& tc = list.Add("Adult industry critic");
	tc.Add("Azealia Banks");
	tc.Add("Lindsay Lohan");
	tc.Add("Amanda Bynes");
	tc.Add("Kim Kardashian-West");
	tc.Add("Chelsea Handler");}

	{auto& tc = list.Add("Adult content reviewer");
	tc.Add("Amanda Bynes");
	tc.Add("Azealia Banks");
	tc.Add("Roseanne Barr");
	tc.Add("Kim Kardashian-West");
	tc.Add("Chelsea Handler");}
	
	return list;
}

VectorMap<String,Vector<String>>& GetRolesSafe(bool gender) {
	if (!gender)
		return GetRolesSafeMale();
	else
		return GetRolesSafeFemale();
}

VectorMap<String,Vector<String>>& GetRolesUnsafe(bool gender) {
	if (!gender)
		return GetRolesUnsafeMale();
	else
		return GetRolesUnsafeFemale();
}

VectorMap<String,Vector<String>>& GetRoleCompanies(bool unsafe, bool gender) {
	if (!unsafe)
		return GetRolesSafe(gender);
	else
		return GetRolesUnsafe(gender);
}















const Index<String>& GetPersonas() {
	static Index<String> list;
	
	if (list.IsEmpty()) {
		list.Add("Expert/guru");
		list.Add("Aspiring/learner");
		list.Add("Creative/innovative");
		list.Add("Influencer/trendsetter");
		list.Add("Relatable/real");
		list.Add("Hilarious/entertaining");
		list.Add("Inspiring/motivator");
		list.Add("Honest/truth-teller");
		list.Add("Authentic/transparent");
		list.Add("Opinionated/thought-provoking");
		list.Add("Down-to-earth/genuine");
		list.Add("Sarcastic/witty");
		list.Add("Chill/laid-back");
		list.Add("Sensible/practical");
		list.Add("Spirited/energetic");
		list.Add("Curious/adventurous");
		list.Add("Analytical/problem-solver");
		list.Add("Supportive/encouraging");
		list.Add("Bold/brave");
		list.Add("Collaborative/connector");
		list.Add("Aspirational/motivator");
		list.Add("Personal/relatable");
		list.Add("Inquisitive/thoughtful");
		list.Add("Educator/teacher");
	}
	return list;
}

const Vector<ContentType>& GetNiches() {
	thread_local static Vector<ContentType> list;
	
	if (list.IsEmpty()) {
        list.Add().Set("Fashion/Beauty", "Luxury",  "Minimalism", "Streetwear");
        list.Add().Set("Food", "Healthy", "Indulgent", "Budget-friendly");
        list.Add().Set("Travel", "Adventure", "Luxury", "Off-the-Beaten Path");
        list.Add().Set("Parenting", "Helicopter", "Free-range", "Attachment");
        list.Add().Set("Lifestyle", "Minimalist", "Maximalist", "Intentional");
        list.Add().Set("DIY/Crafts", "Eco-friendly", "Budget-friendly", "Upcycling");
        list.Add().Set("Personal Development", "Self-improvement", "Self-discovery", "Self-acceptance");
        list.Add().Set("Fitness/Wellness", "Intense", "Mindful", "Playful");
        list.Add().Set("Entertainment/Pop Culture", "Mainstream", "Alternative", "Niche");
        list.Add().Set("Finance/Money Management", "Savings", "Investing", "Debt-free");
        list.Add().Set("Career/Business", "Corporate", "Entrepreneurship", "Side Hustle");
        list.Add().Set("Politics/Current Events", "Conservative", "Liberal", "Moderate");
        list.Add().Set("Health/Mental Health", "Physical", "Emotional", "Spiritual");
        list.Add().Set("Home Decor/Interior Design", "Modern", "Vintage", "Eclectic");
        list.Add().Set("Personal Finance/Investing", "Frugal", "Luxurious", "Practical");
        list.Add().Set("Self-Care/Self-Love", "Hygge", "Self-Pampering", "Mindfulness");
        list.Add().Set("Books/Literature", "Classic", "Chick-Lit", "Thriller");
        list.Add().Set("Technology/Gadgets", "High-Tech", "Low-Tech", "Eco-Friendly");
        list.Add().Set("Environmental/Sustainability", "Green Living", "Zero Waste", "Eco-Conscious");
        list.Add().Set("Relationships", "Monogamous", "Polyamorous", "Open");
        list.Add().Set("Mental Health/Wellness", "Therapy", "Holistic", "Alternative");
        list.Add().Set("Culture/Identity", "Multicultural", "Individualism", "Collectivism");
        list.Add().Set("Social Causes/Activism", "Environmental", "Human Rights", "Animal Welfare");
        list.Add().Set("Conspiracy theories/Real news", "Conspiracy", "Skepticism", "Fact-Checking");
        list.Add().Set("Party politics", "Left-Wing", "Right-Wing", "Moderate");
        list.Add().Set("Religion/Spirituality", "Organized Religion", "New Age", "Atheism");
        list.Add().Set("Music production/songwriting", "Pop", "Indie", "Hip-Hop");
        list.Add().Set("Adventure Travel/Budget Travel", "Safari", "Backpacking", "Luxury Resorts");
        list.Add().Set("Feminism/Empowerment", "Third-Wave", "Intersectional", "Radical");
        list.Add().Set("Design/Architecture", "Modern", "Gothic", "Rustic");
        list.Add().Set("Sports/Fitness", "Team Sports", "Individual Sports", "Outdoor Adventures");
        list.Add().Set("Education/teaching/tutoring", "Traditional", "Progressive", "Montessori");
        list.Add().Set("Beauty/Self-Care", "Natural", "Glam", "DIY");
        list.Add().Set("Computer Science/Programming", "Front-End", "Back-End", "Web Development");
        list.Add().Set("Fine Arts/Crafts", "Painting", "Pottery", "Knitting");
        list.Add().Set("Film/TV/Streaming", "Hollywood Blockbusters", "Indie Films", "Web Series");
        list.Add().Set("Craft beer/wine/cocktails", "Craft Beer", "Wine Tasting", "Mixology");
        list.Add().Set("Psychology/Sociology", "Behaviorism", "Psychoanalysis", "Humanistic");
        list.Add().Set("Science/Technology", "Artificial Intelligence", "Space Exploration", "Bioengineering");
        list.Add().Set("Home-renovation/DIY", "Renovation", "Crafts", "Interior Design");
        list.Add().Set("Gaming/E-Sports", "Console Gaming", "PC Gaming", "Esports");
        list.Add().Set("Mindfulness/Meditation", "Meditation", "Yoga", "Mind-Body Connection");
        list.Add().Set("Pop Culture/Entertainment", "Celebrity Gossip", "Fan Culture", "Subcultures");
        list.Add().Set("Photography/Videography", "Portrait Photography", "Documentary Filmmaking", "Drone Videography");
        list.Add().Set("Thrifting/Sustainable Fashion", "Thrifty Fashion", "Sustainable Fashion", "Upcycled Fashion");
        list.Add().Set("Pet/Animal Care", "Dog Owners", "Cat Lovers", "Bird Enthusiasts");
        list.Add().Set("Homeschooling/Unschooling", "Structured Homeschooling", "Unschooling", "Roadschooling");
        list.Add().Set("Minimalism/Decluttering", "Minimalist Living", "KonMari Method", "Digital Minimalism");
        list.Add().Set("Stock Market/Investing", "Value Investing", "Day Trading", "Cryptocurrency");
	}
	return list;
}

int GetNicheCount() {
	return GetNiches().GetCount();
}


VectorMap<String,Vector<String>>& GetPersonaSafeMale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
	
    {auto& tc = list.Add("Expert/guru");
    tc.Add("Tim Ferriss (productivity/self-improvement)");
    tc.Add("Gary Vaynerchuk (marketing/branding)");
    tc.Add("Tony Robbins (self-help/motivational)");
    tc.Add("Ryan Holiday (stoicism/life advice)");
    tc.Add("Lewis Howes (business/mindset)");
    tc.Add("Tai Lopez (entrepreneurship/investing)");
    tc.Add("Dave Asprey (biohacking/productivity)");
    tc.Add("Seth Godin (marketing/authorship) ");}

    {auto& tc = list.Add("Aspiring/learner");
    tc.Add("Matt D'Avella (minimalism/creativity)");
    tc.Add("Chris Guillebeau (travel/entrepreneurship)");
    tc.Add("Cal Newport (productivity/education)");
    tc.Add("Mark Manson (self-help/personal development)");
    tc.Add("Derek Sivers (entrepreneurship/lifestyle design)");
    tc.Add("Andrew Fiebert (finance/investing)");
    tc.Add("Chase Jarvis (photography/creativity)");
    tc.Add("Nathan Barry (writing/productivity)");}

    {auto& tc = list.Add("Creative/innovative");
    tc.Add("David Chang (food/restaurant industry)");
    tc.Add("Austin Kleon (writing/creativity)");
    tc.Add("Hugh Howey (writing/self-publishing)");
    tc.Add("Casey Neistat (film/vlogging)");
    tc.Add("Jason Silva (artificial intelligence/philosophy)");
    tc.Add("Jamie Oliver (food/health)");
    tc.Add("Marcus Samuelsson (food/culinary entrepreneurship)");
    tc.Add("David Sedaris (humor/writing)");}

    {auto& tc = list.Add("Influencer/trendsetter");
    tc.Add("Brian Kelly (travel/rewards points)");
    tc.Add("Jeffree Star (beauty/makeup)");
    tc.Add("Casey Neistat (film/vlogging)");
    tc.Add("Gary Vaynerchuk (marketing/branding)");
    tc.Add("Tony Robbins (self-help/motivational)");
    tc.Add("Scott Disick (fashion/lifestyle)");
    tc.Add("James Charles (beauty/makeup)");
    tc.Add("Jeremy Fragrance (fragrances/grooming)");}

    {auto& tc = list.Add("Relatable/real");
    tc.Add("Pete Holmes (comedy/relationships)");
    tc.Add("Conan O'Brien (comedy/entertainment)");
    tc.Add("Judd Apatow (comedy/fiction)");
    tc.Add("Aziz Ansari (comedy/relationships)");
    tc.Add("Trevor Noah (comedy/politics)");
    tc.Add("Steve Martin (comedy/writing)");
    tc.Add("Jon Favreau (cooking/food)");
    tc.Add("Joe Rogan (comedy/podcasting)");}

    {auto& tc = list.Add("Hilarious/entertaining");
    tc.Add("Kevin Hart (comedy/motivational)");
    tc.Add("Bo Burnham (comedy/music)");
    tc.Add("Dave Chappelle (comedy/politics)");
    tc.Add("Ali Wong (comedy/parenting)");
    tc.Add("Seth Rogen (comedy/film)");
    tc.Add("Conan O'Brien (comedy/entertainment)");
    tc.Add("Ryan Reynolds (comedy/acting)");
    tc.Add("Russell Brand (comedy/spirituality)");}

    {auto& tc = list.Add("Inspiring/motivator");
    tc.Add("Tony Robbins (self-help/motivational)");
    tc.Add("Mel Robbins (self-help/productivity)");
    tc.Add("Gabby Bernstein (self-help/spirituality)");
    tc.Add("Gary Vaynerchuk (marketing/branding)");
    tc.Add("Tim Ferriss (productivity/self-improvement)");
    tc.Add("Lewis Howes (business/mindset)");}

    {auto& tc = list.Add("Honest/truth-teller");
    tc.Add("James Clear (productivity/habit formation)");
    tc.Add("Seth Godin (marketing/entrepreneurship)");
    tc.Add("Gary Vaynerchuk (entrepreneurship/marketing)");
    tc.Add("Mark Manson (self-help/personal development)");
    tc.Add("Simon Sinek (leadership/management)");
    tc.Add("Tim Ferriss (lifestyle design/productivity)");
    tc.Add("Ryan Holiday (philosophy/stoicism)");
    tc.Add("Ben Shapiro (political commentator/commentary)");}

    {auto& tc = list.Add("Authentic/transparent");
    tc.Add("James Clear (productivity/habit formation)");
    tc.Add("Mark Manson (self-help/personal development)");
    tc.Add("Gary Vaynerchuk (entrepreneurship/marketing)");
    tc.Add("Ramit Sethi (finance/personal development)");
    tc.Add("Seth Godin (marketing/entrepreneurship)");
    tc.Add("Tim Ferriss (lifestyle design/productivity)");
    tc.Add("Chase Jarvis (photography/entrepreneurship)");
    tc.Add("Casey Neistat (filmmaking/entrepreneurship)");}

    {auto& tc = list.Add("Opinionated/thought-provoking");
    tc.Add("Tim Urban (psychology/productivity)");
    tc.Add("Malcolm Gladwell (sociology/authorship)");
    tc.Add("Jordan Peterson (philosophy/psychology)");
    tc.Add("Ben Shapiro (politics/current events)");
    tc.Add("Seth Godin (marketing/authorship)");
    tc.Add("Trevor Noah (comedy/politics)");
    tc.Add("Dave Rubin (media/interviews)");
    tc.Add("Tucker Carlson (journalism/opinions)");}

    {auto& tc = list.Add("Down-to-earth/genuine");
    tc.Add("Jocko Willink (fitness/motivation)");
    tc.Add("Mark Manson (personal development/relationships)");
    tc.Add("James Clear (productivity/habit formation)");
    tc.Add("Matt D'Avella (minimalism/documentary)");
    tc.Add("Casey Neistat (vlogging/creativity)");
    tc.Add("John Green (young adult author/YouTuber)");
    tc.Add("David Sedaris (humor/essays)");
    tc.Add("Jon Acuff (career/entrepreneurship)");}

    {auto& tc = list.Add("Sarcastic/witty");
    tc.Add("Ryan Reynolds (celebrity/entertainment)");
    tc.Add("Conan O'Brien (late night comedy)");
    tc.Add("John Oliver (satirical news/politics)");
    tc.Add("Dave Barry (humor/columnist)");
    tc.Add("Andy Borowitz (satirical news/writing)");
    tc.Add("Aziz Ansari (comedy/relationships)");
    tc.Add("Stephen Fry (comedian/author)");
    tc.Add("James Corden (late night comedy/music)");}

    {auto& tc = list.Add("Chill/laid-back");
    tc.Add("Tim Ferriss (lifestyle design/productivity)");
    tc.Add("Joe Rogan (comedy/podcasting)");
    tc.Add("Casey Neistat (vlogging/travel)");
    tc.Add("Gary Vaynerchuk (entrepreneurship/marketing)");
    tc.Add("Chase Jarvis (photography/creativity)");
    tc.Add("Anthony Bourdain (food/travel)");
    tc.Add("Rhett & Link (comedy/sketches)");
    tc.Add("Jim Gaffigan (comedy/food)");}

    {auto& tc = list.Add("Sensible/practical");
    tc.Add("Dave Ramsey (finance/money management)");
    tc.Add("Adam Grant (business/organizational psychology)");
    tc.Add("Thomas Frank (productivity/education)");
    tc.Add("Tim Ferriss (lifestyle design/productivity)");
    tc.Add("Tony Robbins (personal development/motivation)");
    tc.Add("SmarterEveryDay (science/education)");
    tc.Add("Ryan Holiday (media/philosophy)");
    tc.Add("Chris Guillebeau (travel/personal development)");}

    {auto& tc = list.Add("Spirited/energetic");
    tc.Add("Tony Robbins (personal development/motivation)");
    tc.Add("Gary Vaynerchuk (entrepreneurship/marketing)");
    tc.Add("Lewis Howes (business/personal development)");
    tc.Add("Tim Ferriss (lifestyle design/productivity)");
    tc.Add("Brendon Burchard (self-help/motivation)");
    tc.Add("Eric Thomas (motivational speaker/inspiration)");
    tc.Add("Jack Canfield (personal development/success)");
    tc.Add("Grant Cardone (entrepreneurship/sales)");}

    {auto& tc = list.Add("Curious/adventurous");
    tc.Add("Bear Grylls (outdoors/adventure)");
    tc.Add("Anthony Bourdain (food/travel)");
    tc.Add("The Bucket List Family (travel/family adventures)");
    tc.Add("Richard Branson (entrepreneurship/adventure)");
    tc.Add("Tim Ferriss (lifestyle design/adventure)");
    tc.Add("Alex Honnold (rock climbing/adventure)");
    tc.Add("Chris Burkard (photography/adventure)");
    tc.Add("Roman Atwood (vlogging/family adventures)");}

    {auto& tc = list.Add("Analytical/problem-solver");
    tc.Add("Neil deGrasse Tyson (science/astrophysics)");
    tc.Add("Bill Nye (science/education)");
    tc.Add("Malcolm Gladwell (psychology/sociology)");
    tc.Add("Simon Sinek (leadership/management)");
    tc.Add("Daymond John (entrepreneurship/business)");
    tc.Add("Tim Ferriss (lifestyle design/productivity)");
    tc.Add("Cal Newport (computer science/productivity)");
    tc.Add("James Clear (productivity/habit formation)");}

    {auto& tc = list.Add("Supportive/encouraging");
    tc.Add("Brendon Burchard (self-help/motivation)");
    tc.Add("Tony Robbins (personal development/motivation)");
    tc.Add("Brené Brown (self-acceptance/empowerment)");
    tc.Add("Nick Vujicic (inspirational speaker/disability advocate)");
    tc.Add("Jay Shetty (personal development/philosophy)");
    tc.Add("Trent Shelton (motivational speaker/personal development)");
    tc.Add("John O'Leary (inspirational speaker/personal growth)");
    tc.Add("Mel Robbins (personal development/overcoming fears)");}

    {auto& tc = list.Add("Bold/brave");
    tc.Add("David Goggins (motivational speaker/ultra-endurance athlete)");
    tc.Add("Jocko Willink (leadership/discipline)");
    tc.Add("Gary Vaynerchuk (entrepreneurship/marketing)");
    tc.Add("Lewis Howes (personal development/podcast host)");
    tc.Add("Lewis Hamilton (athletics/formula one racing)");
    tc.Add("Casey Neistat (filmmaking/entrepreneurship)");
    tc.Add("Mark Manson (self-help/personal development)");
    tc.Add("Grant Cardone (sales/motivation)");}

    {auto& tc = list.Add("Collaborative/connector");
    tc.Add("Tim Urban (blogging/writing)");
    tc.Add("Jeff Goins (blogging/writing)");
    tc.Add("Pat Flynn (entrepreneurship/blogging)");
    tc.Add("Michael Hyatt (leadership/productivity)");
    tc.Add("Ramit Sethi (finance/personal development)");
    tc.Add("Chase Jarvis (photography/entrepreneurship)");
    tc.Add("Greg McKeown (productivity/minimalism)");
    tc.Add("Simon Sinek (leadership/management)");}

    {auto& tc = list.Add("Aspirational/motivator");
    tc.Add("Tony Robbins (personal development/motivation)");
    tc.Add("Gary Vaynerchuk (entrepreneurship/marketing)");
    tc.Add("Brendon Burchard (self-help/motivation)");
    tc.Add("Eric Thomas (motivational speaker/entrepreneur)");
    tc.Add("Grant Cardone (sales/motivation)");
    tc.Add("Robin Sharma (leadership/productivity)");
    tc.Add("Mel Robbins (personal development/overcoming fears)");
    tc.Add("Jay Shetty (personal development/philosophy)");}

    {auto& tc = list.Add("Personal/relatable' :");
    tc.Add("The Minimalists (minimalism/lifestyle design)");
    tc.Add("Tim Ferriss (lifestyle design/productivity)");
    tc.Add("Mark Manson (self-help/personal development)");
    tc.Add("Chris Guillebeau (travel/entrepreneurship)");
    tc.Add("Muneeb Ali (entrepreneurship/productivity)");
    tc.Add("Ramit Sethi (finance/personal development)");
    tc.Add("Casey Neistat (filmmaking/entrepreneurship)");
    tc.Add("Simon Sinek (leadership/management)");}

    {auto& tc = list.Add("Inquisitive/thoughtful' :");
    tc.Add("Tim Urban (blogging/writing)");
    tc.Add("Yuval Noah Harari (history/philosophy)");
    tc.Add("Sam Harris (philosophy/science)");
    tc.Add("Ryan Holiday (philosophy/stoicism)");
    tc.Add("Cal Newport (computer science/productivity)");
    tc.Add("James Clear (productivity/habit formation)");
    tc.Add("Malcolm Gladwell (psychology/sociology)");
    tc.Add("Mark Manson (self-help/personal development)");}

    {auto& tc = list.Add("Educator/teacher");
    tc.Add("Seth Godin (marketing/entrepreneurship)");
    tc.Add("Chris Guillebeau (travel/entrepreneurship)");
    tc.Add("Pat Flynn (entrepreneurship/blogging)");
    tc.Add("James Altucher (entrepreneurship/writing)");
    tc.Add("Ray Dalio (finance/investing)");
    tc.Add("Simon Sinek (leadership/management)");
    tc.Add("Ramit Sethi (finance/personal development)");
    tc.Add("Tim Ferriss (lifestyle design/productivity) ");
    tc.Add("John Maxwell (leadership/influence)");
    tc.Add("Bo Eason (storytelling/performance)");}
	ASSERT(GetPersonas().GetCount() == list.GetCount());
	return list;
}

VectorMap<String,Vector<String>>& GetPersonaSafeFemale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
    {auto& tc = list.Add("Expert/guru");
    tc.Add("Marie Forleo (entrepreneurship/personal development)");
    tc.Add("Rachel Hollis (personal development/entrepreneurship)");
    tc.Add("Amanda Kloots (fitness/health)");
    tc.Add("Mel Robbins (self-improvement/motivation)");
    tc.Add("Gretchen Rubin (happiness/habits)");
    tc.Add("Jen Sincero (finance/self-help)");
    tc.Add("Amy Porterfield (online marketing/business)");
    tc.Add("Sophia Amoruso (fashion/entrepreneurship)");}

    {auto& tc = list.Add("Aspiring/learner");
    tc.Add("Jen Hatmaker (faith/personal development)");
    tc.Add("Luvvie Ajayi (humor/speaking)");
    tc.Add("Abby Wambach (sports/leadership)");
    tc.Add("Rupi Kaur (poetry/writing)");
    tc.Add("Mimi Ikonn (lifestyle/design)");
    tc.Add("Michelle Schroeder-Gardner (finance/blogging)");
    tc.Add("Jenna Kutcher (photography/marketing)");
    tc.Add("Rachel Brathen (yoga/wellness)");}

    {auto& tc = list.Add("Creative/innovative");
    tc.Add("Brene Brown (personal development/writing)");
    tc.Add("Justina Blakeney (interior design/creativity)");
    tc.Add("Joy Cho (design/branding)");
    tc.Add("Emily Henderson (interior design/home decor)");
    tc.Add("Grace Bonney (art/design/entrepreneurship)");
    tc.Add("Elsie Larson (DIY/crafts/creativity)");
    tc.Add("Susan Kare (graphic design/technology)");
    tc.Add("Gabby Bernstein (spirituality/creativity)");}

    {auto& tc = list.Add("Influencer/trendsetter");
    tc.Add("Chiara Ferragni (fashion/trends)");
    tc.Add("Aimee Song (fashion/lifestyle)");
    tc.Add("Camila Coelho (beauty/lifestyle)");
    tc.Add("Tess Holliday (body positivity/fashion)");
    tc.Add("Huda Kattan (beauty/influencer marketing)");
    tc.Add("Leandra Medine (fashion/humor)");
    tc.Add("Nikkie Tutorials (beauty/trends)");
    tc.Add("Jenn Im (fashion/lifestyle)");}

    {auto& tc = list.Add("Relatable/real");
    tc.Add("Sarah Knight (self-help/humor)");
    tc.Add("Jenny Lawson (humor/memoir)");
    tc.Add("Glennon Doyle Melton (emotional wellness/self-discovery)");
    tc.Add("Hannah Hart (humor/LGBTQ+ advocate)");
    tc.Add("Jameela Jamil (body positivity/mental health advocacy)");
    tc.Add("Luvvie Ajayi (humor/speaking)");
    tc.Add("Nora McInerny (personal essays/grief)");
    tc.Add("Felicia Day (geek culture/humor)");}
    
    {auto& tc = list.Add("Hilarious/entertaining");
    tc.Add("Colleen Ballinger (comedy/entertainment)");
    tc.Add("Grace Helbig (comedy/entertainment)");
    tc.Add("Lilly Singh (comedy/entertainment)");
    tc.Add("Mindy Kaling (comedy/writing)");
    tc.Add("Iliza Shlesinger (comedy/writing)");
    tc.Add("Phoebe Robinson (comedy/podcasting)");
    tc.Add("Hannah Hart (comedy/entertainment)");
    tc.Add("Issa Rae (comedy/writing)");}

    {auto& tc = list.Add("Inspiring/motivator");
    tc.Add("Mel Robbins (self-improvement/motivation)");
    tc.Add("Brene Brown (personal development/writing)");
    tc.Add("Elizabeth Gilbert (creativity/self-discovery)");
    tc.Add("Rachel Hollis (personal development/entrepreneurship)");
    tc.Add("Gabby Bernstein (spirituality/creativity)");
    tc.Add("Mimi Ikonn (lifestyle/design)");
    tc.Add("Jen Hatmaker (faith/personal development)");
    tc.Add("Abbey Wambach (sports/leadership)");}

    {auto& tc = list.Add("Honest/truth-teller");
    tc.Add("Glennon Doyle Melton (emotional wellness/self-discovery)");
    tc.Add("Brene Brown (personal development/writing)");
    tc.Add("Nora McInerny (personal essays/grief)");
    tc.Add("Jenna Kutcher (body positivity/self-acceptance)");
    tc.Add("Jen Hatmaker (faith/personal development)");
    tc.Add("Elizabeth Gilbert (women's issues/empowerment)");
    tc.Add("Mindy Kaling (body positivity/writing)");
    tc.Add("Jameela Jamil (body positivity/mental health advocacy)");}

    {auto& tc = list.Add("Authentic/transparent");
    tc.Add("Glennon Doyle Melton (emotional wellness/self-discovery)");
    tc.Add("Jen Hatmaker (faith/personal development)");
    tc.Add("Brené Brown (personal development/writing)");
    tc.Add("Nora McInerny (personal essays/grief)");
    tc.Add("Rachel Brathen (yoga/wellness)");
    tc.Add("Mimi Ikonn (lifestyle/design)");
    tc.Add("Jameela Jamil (body positivity/mental health advocacy)");
    tc.Add("Luvvie Ajayi (humor/speaking)");}

    {auto& tc = list.Add("Opinionated/thought-provoking");
    tc.Add("Elizabeth Gilbert (creativity/self-discovery)");
    tc.Add("Glennon Doyle Melton (emotional wellness/self-discovery)");
    tc.Add("Chimamanda Ngozi Adichie (feminism/race)");
    tc.Add("Amandla Stenberg (activism/pop culture)");
    tc.Add("Roxane Gay (feminism/writing)");
    tc.Add("Rashida Jones (feminism/lifestyle)");
    tc.Add("Shonda Rhimes (entertainment/writing)");
    tc.Add("Roxane Gay (feminism/writing)");}

    {auto& tc = list.Add("Down-to-earth/genuine");
    tc.Add("Fredricka Whitfield (journalism/honesty)");
    tc.Add("Abby Wambach (sports/leadership)");
    tc.Add("Oprah Winfrey (empowerment/authenticity)");
    tc.Add("Ellen DeGeneres (comedy/relatability)");
    tc.Add("Issa Rae (comedy/writing)");
    tc.Add("Keanu Reeves (film/philanthropy)");
    tc.Add("Paul Rudd (film/humor)");
    tc.Add("Amy Poehler (comedy/writing)");}

    {auto& tc = list.Add("Sarcastic/witty");
    tc.Add("Samantha Bee (comedy/political satire)");
    tc.Add("Mindy Kaling (body positivity/writing)");
    tc.Add("Jameela Jamil (body positivity/mental health advocacy)");
    tc.Add("Tina Fey (comedy/writing)");
    tc.Add("Iliza Shlesinger (comedy/writing)");
    tc.Add("Phoebe Robinson (comedy/podcasting)");
    tc.Add("Amy Poehler (comedy/writing)");
    tc.Add("Issa Rae (comedy/writing)");}

    {auto& tc = list.Add("Chill/laid-back");
    tc.Add("Bethenny Frankel (business/relaxed lifestyle)");
    tc.Add("Amy Schumer (comedy/normcore)");
    tc.Add("Jennifer Lawrence (film/relaxed lifestyle)");
    tc.Add("Mindy Kaling (body positivity/writing)");
    tc.Add("Issa Rae (comedy/writing)");
    tc.Add("Jameela Jamil (body positivity/mental health advocacy)");
    tc.Add("Abby Wambach (sports/leadership)");
    tc.Add("Keanu Reeves (film/philanthropy)");}

    {auto& tc = list.Add("Sensible/practical");
    tc.Add("Marie Forleo (entrepreneurship/personal development)");
    tc.Add("Mimi Ikonn (lifestyle/design)");
    tc.Add("Ramit Sethi (finance/personal development)");
    tc.Add("Pat Flynn (entrepreneurship/blogging)");
    tc.Add("Gretchen Rubin (happiness/habits)");
    tc.Add("Brene Brown (personal development/writing)");
    tc.Add("Joanna Gaines (home design/practicality)");
    tc.Add("Ruth Soukup (productivity/organization)");}

    {auto& tc = list.Add("Spirited/energetic");
    tc.Add("Tony Robbins (personal development/energy)");
    tc.Add("Jen Hatmaker (faith/personal development)");
    tc.Add("Brene Brown (personal development/writing)");
    tc.Add("Oprah Winfrey (empowerment/authenticity)");
    tc.Add("Elizabeth Gilbert (creativity/self-discovery)");
    tc.Add("Rachel Hollis (personal development/entrepreneurship)");
    tc.Add("Gabby Bernstein (spirituality/creativity)");
    tc.Add("Marie Forleo (entrepreneurship/personal development)");}

    {auto& tc = list.Add("Curious/adventurous");
    tc.Add("Elizabeth Gilbert (creativity/self-discovery)");
    tc.Add("Abby Wambach (sports/leadership)");
    tc.Add("Brené Brown (personal development/writing)");
    tc.Add("Cheryl Strayed (adventure/self-discovery)");
    tc.Add("Amanda Lindhout (journalism/adventure)");
    tc.Add("Gretchen Rubin (happiness/habits)");
    tc.Add("Shonda Rhimes (entertainment/writing)");
    tc.Add("Elizabeth Gilbert (women's issues/empowerment)");}

    {auto& tc = list.Add("Analytical/problem-solver");
    tc.Add("Malala Yousafzai (activism/problem-solving)");
    tc.Add("Melinda Gates (philanthropy/problem-solving)");
    tc.Add("Elizabeth Holmes (entrepreneurship/problem-solving)");
    tc.Add("Marissa Mayer (technology/problem-solving)");
    tc.Add("Kari Byron (science/innovation)");
    tc.Add("Sheryl Sandberg (business/leadership)");
    tc.Add("Priyanka Chopra (activism/social issues)");
    tc.Add("Melinda Gates (business/women's issues)");}

    {auto& tc = list.Add("Supportive/encouraging");
    tc.Add("Marie Forleo (entrepreneurship/personal development)");
    tc.Add("Jen Hatmaker (faith/personal development)");
    tc.Add("Brene Brown (personal development/writing)");
    tc.Add("Oprah Winfrey (empowerment/authenticity)");
    tc.Add("Elizabeth Gilbert (women's issues/empowerment)");
    tc.Add("Rachel Hollis (personal development/entrepreneurship)");
    tc.Add("Gabby Bernstein (spirituality/creativity)");
    tc.Add("Mimi Ikonn (lifestyle/design)");}

    {auto& tc = list.Add("Bold/brave");
    tc.Add("Brene Brown (personal development/writing)");
    tc.Add("Brené Brown (personal development/writing)");
    tc.Add("Malala Yousafzai (activism/problems-solving)");
    tc.Add("Abby Wambach (sports/leadership)");
    tc.Add("Amanda Lindhout (journalism/adventure)");
    tc.Add("J.K. Rowling (writing/activism)");
    tc.Add("Roxane Gay (feminism/writing)");
    tc.Add("Issa Rae (comedy/writing)");}

    {auto& tc = list.Add("Collaborative/connector");
    tc.Add("Marie Forleo (entrepreneurship/personal development)");
    tc.Add("Brene Brown (personal development/writing)");
    tc.Add("Oprah Winfrey (empowerment/authenticity)");
    tc.Add("Elizabeth Gilbert (creativity/self-discovery)");
    tc.Add("Melinda Gates (philanthropy/collaboration)");
    tc.Add("Rachel Hollis (personal development/entrepreneurship)");
    tc.Add("Sheryl Sandberg (business/leadership)");
    tc.Add("Amy Poehler (comedy/activism)");}

    {auto& tc = list.Add("Aspirational/motivator");
    tc.Add("Tony Robbins (personal development/motivation)");
    tc.Add("Oprah Winfrey (empowerment/authenticity)");
    tc.Add("Elizabeth Gilbert (creativity/self-discovery)");
    tc.Add("Jen Hatmaker (faith/personal development)");
    tc.Add("Rachel Hollis (personal development/entrepreneurship)");
    tc.Add("Gabrielle Bernstein (spirituality/creativity)");
    tc.Add("Marie Forleo (entrepreneurship/personal development)");
    tc.Add("Mimi Ikonn (lifestyle/design)");}

    {auto& tc = list.Add("Personal/relatable");
    tc.Add("Marie Forleo (entrepreneurship/personal development)");
    tc.Add("Brene Brown (personal development/writing)");
    tc.Add("Oprah Winfrey (empowerment/authenticity)");
    tc.Add("Elizabeth Gilbert (creativity/self-discovery)");
    tc.Add("Rachel Hollis (personal development/entrepreneurship)");
    tc.Add("Abby Wambach (sports/leadership)");
    tc.Add("Amy Poehler (comedy/friendship)");
    tc.Add("Jen Hatmaker (faith/personal development)");}

    {auto& tc = list.Add("Inquisitive/thoughtful");
    tc.Add("Brené Brown (personal development/writing)");
    tc.Add("Brene Brown (personal development/writing)");
    tc.Add("Elizabeth Gilbert (creativity/self-discovery)");
    tc.Add("Brene Brown (personal development/writing)");
    tc.Add("Jen Hatmaker (faith/personal development)");
    tc.Add("Glennon Doyle Melton (emotional wellness/self-discovery)");
    tc.Add("Negin Farsad (comedy/politics)");
    tc.Add("Shonda Rhimes (entertainment/writing)");}

    {auto& tc = list.Add("Educator/teacher");
    tc.Add("Brene Brown (personal development/writing)");
    tc.Add("Rachel Hollis (personal development/entrepreneurship)");
    tc.Add("Amanda Lindhout (journalism/adventure)");
    tc.Add("Glennon Doyle Melton (emotional wellness/self-discovery)");
    tc.Add("Jen Hatmaker (faith/personal development)");
    tc.Add("Elizabeth Gilbert (creativity/self-discovery)");
    tc.Add("Oprah Winfrey (empowerment/authenticity)");
    tc.Add("Abby Wambach (sports/leadership)");}
	
	ASSERT(GetPersonas().GetCount() == list.GetCount());
	return list;
}

VectorMap<String,Vector<String>>& GetPersonaUnsafeMale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
	
    {auto& tc = list.Add("Expert/guru");
    tc.Add("Dave Asprey (health/wellness)");
    tc.Add("Gary Vaynerchuk (entrepreneurship/marketing)");
    tc.Add("Tony Robbins (personal development/coaching)");
    tc.Add("Grant Cardone (sales/entrepreneurship)");
    tc.Add("Tai Lopez (entrepreneurship/self-improvement)");}

    {auto& tc = list.Add("Aspiring/learner");
    tc.Add("Tim Urban (self-improvement/curiosity)");
    tc.Add("Jocko Willink (leadership/discipline)");
    tc.Add("Lewis Howes (personal development/sports)");
    tc.Add("Richard Branson (entrepreneurship/adventure)");
    tc.Add("Ryan Holiday (self-improvement/philosophy)");}

    {auto& tc = list.Add("Creative/innovative");
    tc.Add("Casey Neistat (filmmaking/entrepreneurship)");
    tc.Add("Jesse Driftwood (filmmaking/photography)");
    tc.Add("Austin Kleon (writing/creativity)");
    tc.Add("Steve Jobs (technology/innovation)");
    tc.Add("Elon Musk (technology/innovation)");}

    {auto& tc = list.Add("Influencer/trendsetter");
    tc.Add("James Charles (beauty/influencer)");
    tc.Add("Aja Dang (fashion/beauty/lifestyle)");
    tc.Add("Chiara Ferragni (fashion/influencer)");
    tc.Add("Joe Wicks (fitness/health/food)");
    tc.Add("Joshua Weissman (food/cooking)");}

    {auto& tc = list.Add("Relatable/real");
    tc.Add("James Clear (self-improvement/productivity)");
    tc.Add("Humble The Poet (poetry/self-reflection)");
    tc.Add("Tim Ferriss (self-experimentation/personal development)");
    tc.Add("Kevin O'Leary (entrepreneurship/personal finance)");
    tc.Add("Casey Neistat (filmmaking/lifestyle)");}

    {auto& tc = list.Add("Hilarious/entertaining");
    tc.Add("Kevin Hart (comedy/entertainment)");
    tc.Add("Trevor Noah (comedy/commentary)");
    tc.Add("The Try Guys (comedy/lifestyle)");
    tc.Add("Brodie Smith (sports/entertainment)");
    tc.Add("John Oliver (comedy/commentary)");}

    {auto& tc = list.Add("Inspiring/motivator");
    tc.Add("Les Brown (motivational speaking/coaching)");
    tc.Add("Inky Johnson (motivational speaking/leadership)");
    tc.Add("David Goggins (mental endurance/motivation)");
    tc.Add("Eric Thomas (motivational speaking/sports)");
    tc.Add("Nick Vujicic (motivational speaking/disability awareness)");}

    {auto& tc = list.Add("Honest/truth-teller");
    tc.Add("Jordan Peterson (psychology/philosophy)");
    tc.Add("Ryan Holiday (self-improvement/philosophy)");
    tc.Add("Sam Harris (philosophy/science)");
    tc.Add("Naval Ravikant (entrepreneurship/philosophy)");
    tc.Add("Joe Rogan (comedy/sports/current events)");}

    {auto& tc = list.Add("Authentic/transparent");
    tc.Add("Brene Brown (vulnerability/research)");
    tc.Add("Simon Sinek (leadership/communication)");
    tc.Add("Lewis Howes (personal development/sports)");
    tc.Add("Casey Neistat (filmmaking/lifestyle)");
    tc.Add("Gary Vaynerchuk (social media/marketing)");}

    {auto& tc = list.Add("Opinionated/thought-provoking");
    tc.Add("Ben Shapiro (political commentary/conservatism)");
    tc.Add("Malcolm Gladwell (social science/writing)");
    tc.Add("Jordan Peterson (psychology/philosophy)");
    tc.Add("Sam Harris (philosophy/science)");
    tc.Add("Dan Lok (entrepreneurship/business)");}

    {auto& tc = list.Add("Down-to-earth/genuine");
    tc.Add("Matthew Hussey (dating/relationships)");
    tc.Add("David Goggins (mental endurance/motivation)");
    tc.Add("Jocko Willink (leadership/discipline)");
    tc.Add("Jaco de Bruyn (fitness/health)");
    tc.Add("James Clear (self-improvement/productivity)");}

    {auto& tc = list.Add("Sarcastic/witty");
    tc.Add("John Oliver (comedy/commentary)");
    tc.Add("Hasan Minhaj (comedy/commentary)");
    tc.Add("Dave Chappelle (comedy/social commentary)");
    tc.Add("Trevor Noah (comedy/commentary)");
    tc.Add("George Carlin (comedy/social commentary)");}

    {auto& tc = list.Add("Chill/laid-back");
    tc.Add("Casey Neistat (filmmaking/lifestyle)");
    tc.Add("The Rock (fitness/entertainment)");
    tc.Add("Alfie Deyes (vlogging/lifestyle)");
    tc.Add("Mark Wiens (travel/food)");
    tc.Add("Kien Lam (travel/photography)");}

    {auto& tc = list.Add("Sensible/practical");
    tc.Add("Dave Ramsey (personal finance/debt-free living)");
    tc.Add("Eric Ries (entrepreneurship/lean startup)");
    tc.Add("Tim Ferriss (self-experimentation/productivity)");
    tc.Add("Mark Manson (self-improvement/relationships)");
    tc.Add("Naval Ravikant (entrepreneurship/philosophy/biography)");}

    {auto& tc = list.Add("Spirited/energetic");
    tc.Add("Tai Lopez (entrepreneurship/self-improvement)");
    tc.Add("Jocko Willink (leadership/discipline)");
    tc.Add("Tony Robbins (motivational speaking/coaching)");
    tc.Add("Les Brown (motivational speaking/motivation)");
    tc.Add("Gary Vaynerchuk (social media/marketing)");}

    {auto& tc = list.Add("Curious/adventurous");
    tc.Add("Bear Grylls (outdoor/adventure)");
    tc.Add("Elon Musk (technology/innovation)");
    tc.Add("David Goggins (mental endurance/motivation)");
    tc.Add("Richard Branson (entrepreneurship/adventure)");
    tc.Add("Tim Ferriss (adventure/productivity)");}

    {auto& tc = list.Add("Analytical/problem-solver");
    tc.Add("Ray Dalio (investing/economics)");
    tc.Add("Bill Gates (technology/philanthropy)");
    tc.Add("Neil deGrasse Tyson (science/astrophysics)");
    tc.Add("Jordan Peterson (psychology/philosophy)");
    tc.Add("Aaron Swartz (technology/activism)");}

    {auto& tc = list.Add("Supportive/encouraging");
    tc.Add("Eric Thomas (motivational speaking/sports)");
    tc.Add("Inky Johnson (motivational speaking/leadership)");
    tc.Add("Brene Brown (vulnerability/research)");
    tc.Add("Tim Ferriss (self-experimentation/productivity)");
    tc.Add("Dave Ramsey (personal finance/debt-free living)");}

    {auto& tc = list.Add("Bold/brave");
    tc.Add("Tim Ferriss (self-experimentation/productivity)");
    tc.Add("David Goggins (mental endurance/motivation)");
    tc.Add("Malcolm Gladwell (social science/writing)");
    tc.Add("Joe Rogan (comedy/sports/current events)");
    tc.Add("Elon Musk (technology/innovation)");}

    {auto& tc = list.Add("Collaborative/connector");
    tc.Add("Tim Ferriss (self-experimentation/productivity)");
    tc.Add("Tony Robbins (motivational speaking/coaching)");
    tc.Add("Lewis Howes (personal development/sports)");
    tc.Add("Larry Page (technology/innovation)");
    tc.Add("Peter Thiel (venture capital/entrepreneurship)");}

    {auto& tc = list.Add("Aspirational/motivator");
    tc.Add("Tony Robbins (motivational speaking/coaching)");
    tc.Add("Les Brown (motivational speaking/motivation)");
    tc.Add("Eric Thomas (motivational speaking/sports)");
    tc.Add("Tai Lopez (entrepreneur/self-improvement)");
    tc.Add("Steve Jobs (technology/innovation)");}

    {auto& tc = list.Add("Personal/relatable");
    tc.Add("Humble The Poet (poetry/self-reflection)");
    tc.Add("Logan Paul (vlogging/lifestyle)");
    tc.Add("Tim Ferriss (self-experimentation/productivity)");
    tc.Add("James Altucher (self-improvement/entrepreneurship)");
    tc.Add("Jocko Willink (leadership/discipline)");}

    {auto& tc = list.Add("Inquisitive/thoughtful");
    tc.Add("Neil deGrasse Tyson (science/astrophysics)");
    tc.Add("Dan Harris (mindfulness/meditation)");
    tc.Add("Sam Harris (philosophy/science)");
    tc.Add("Tim Ferriss (self-experimentation/productivity)");
    tc.Add("Malcolm Gladwell (social science/writing)");}

    {auto& tc = list.Add("Educator/teacher");
    tc.Add("Sir Ken Robinson (education/creativity)");
    tc.Add("Russell Brunson (sales/marketing)");
    tc.Add("Simon Sinek (leadership/communication)");
    tc.Add("Stephen Covey (personal development/leadership)");
    tc.Add("Jordan Peterson (psychology/philosophy)");}
	
	ASSERT(GetPersonas().GetCount() == list.GetCount());
	return list;
}

VectorMap<String,Vector<String>>& GetPersonaUnsafeFemale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
	{auto& tc = list.Add("Expert/guru");
    tc.Add("Marie Forleo (entrepreneurship/coaching)");
    tc.Add("Rachel Hollis (personal development/wellness)");
    tc.Add("Jenna Kutcher (marketing/photography)");
    tc.Add("Gretchen Rubin (productivity/lifestyle)");
    tc.Add("Mel Robbins (motivational speaker/author)");}

    {auto& tc = list.Add("Aspiring/learner");
    tc.Add("Rachel Brathen (yoga/mental health)");
    tc.Add("Joy Cho (design/crafts)");
    tc.Add("Mimi Ikonn (fashion/beauty)");
    tc.Add("Erin Loechner (slow living/parenting)");
    tc.Add("Kelsey Miller (body positivity/feminism)");}

    {auto& tc = list.Add("Creative/innovative");
    tc.Add("Ingrid Nilsen (lifestyle/entertainment)");
    tc.Add("Aimee Song (fashion/interior design)");
    tc.Add("Justine Ezarik (technology/gaming)");
    tc.Add("Chrissy Teigen (humor/food)");
    tc.Add("Louise Pentland (diy/mom blogger)");}

    {auto& tc = list.Add("Influencer/trendsetter");
    tc.Add("Chiara Ferragni (fashion/beauty)");
    tc.Add("Huda Kattan (makeup/beauty)");
    tc.Add("Kristina Bazan (fashion/travel)");
    tc.Add("Ascia AKF (fashion/lifestyle)");
    tc.Add("Aimee Song (fashion/interior design)");}

    {auto& tc = list.Add("Relatable/real");
    tc.Add("Constance Hall (parenting/women's issues)");
    tc.Add("Grace Helbig (lifestyle/youtuber)");
    tc.Add("Hannah Hart (comedy/lifestyle)");
    tc.Add("Laura Lejeune (mental health/wellness)");
    tc.Add("Alisha Marie (lifestyle/diy)");}

    {auto& tc = list.Add("Hilarious/entertaining");
    tc.Add("Lilly Singh (comedy/entertainment)");
    tc.Add("Colleen Ballinger (comedy/entertainment)");
    tc.Add("GloZell Green (comedy/lifestyle)");
    tc.Add("Mamrie Hart (comedy/entertainment)");
    tc.Add("Jenny Lawson (humor/writing)");}

    {auto& tc = list.Add("Inspiring/motivator");
    tc.Add("Gabi Gregg (body positivity/fashion)");
    tc.Add("Elizabeth Gilbert (creativity/writing)");
    tc.Add("Tara Stiles (yoga/wellness)");
    tc.Add("Lalah Delia (self-care/spirituality)");
    tc.Add("Jen Sincero (personal development/money)");}

    {auto& tc = list.Add("Honest/truth-teller");
    tc.Add("Glennon Doyle (feminism/mental health)");
    tc.Add("Amy Schumer (comedy/feminism)");
    tc.Add("Michelle Obama (activism/women's issues)");
    tc.Add("Brené Brown (vulnerability/shame)");
    tc.Add("Esther Perel (relationships/sexuality)");}

    {auto& tc = list.Add("Authentic/transparent");
    tc.Add("Jen Hatmaker (faith/lifestyle)");
    tc.Add("Rachel Held Evans (faith/writing)");
    tc.Add("Leandra Medine (fashion/lifestyle)");
    tc.Add("Alexandria Ocasio-Cortez (politics/activism)");
    tc.Add("Glennon Doyle (feminism/mental health)");}

    {auto& tc = list.Add("Opinionated/thought-provoking");
    tc.Add("Anita Sarkeesian (media criticism/feminism)");
    tc.Add("Roxane Gay (writing/feminism)");
    tc.Add("Chimamanda Ngozi Adichie (literature/feminism)");
    tc.Add("Rupi Kaur (poetry/feminism)");
    tc.Add("Bree Newsome Bass (activism/politics)");}

    {auto& tc = list.Add("Down-to-earth/genuine");
    tc.Add("Candice Kumai (healthy living/recipe developer)");
    tc.Add("Constance Wu (acting/activism)");
    tc.Add("Chrissy Teigen (humor/motherhood)");
    tc.Add("Sophia Bush (activism/lifestyle)");
    tc.Add("Rachel Brathen (yoga/mental health)");}

    {auto& tc = list.Add("Sarcastic/witty");
    tc.Add("Samantha Irby (humor/lifestyle)");
    tc.Add("Mindy Kaling (comedy/writing)");
    tc.Add("Phoebe Robinson (comedy/pop culture)");
    tc.Add("Ali Wong (comedy/parenting)");
    tc.Add("Jenny Lawson (humor/writing)");}

    {auto& tc = list.Add("Chill/laid-back");
    tc.Add("Kate Hudson (fitness/wellness)");
    tc.Add("Agyness Deyn (fashion/activism)");
    tc.Add("Zoë Foster Blake (beauty/lifestyle)");
    tc.Add("Kristen Bell (acting/mental health)");
    tc.Add("Tess Holliday (body positivity/lifestyle)");}

    {auto& tc = list.Add("Sensible/practical");
    tc.Add("Joanna Gaines (design/home renovation)");
    tc.Add("Marie Forleo (entrepreneurship/business)");
    tc.Add("Grace Bonney (design/writing)");
    tc.Add("Kristina Bazan (fashion/entrepreneurship)");
    tc.Add("Sophia Amoruso (fashion/entrepreneurship)");}

    {auto& tc = list.Add("Spirited/energetic");
    tc.Add("Gabby Bernstein (self-help/guru)");
    tc.Add("Jen Sincero (self-help/writing)");
    tc.Add("Rachel Hollis (self-help/entrepreneurship)");
    tc.Add("Cat & Nat (parenting/entertainment)");
    tc.Add("Lilly Singh (comedy/entertainment)");}

    {auto& tc = list.Add("Curious/adventurous");
    tc.Add("Chrissy Teigen (food/travel)");
    tc.Add("The Bucket List Family (travel/adventure)");
    tc.Add("Gigi Gorgeous (beauty/identity)");
    tc.Add("Hannah Hart (food/travel/writing)");
    tc.Add("Cara Delevingne (fashion/identity)");}

    {auto& tc = list.Add("Analytical/problem-solver");
    tc.Add("Gretchen Rubin (self-help/writing)");
    tc.Add("Amy Porterfield (entrepreneurship/online marketing)");
    tc.Add("Tasha Eurich (psychology/self-discovery)");
    tc.Add("Jasmine Star (photography/marketing)");
    tc.Add("Susan Cain (psychology/self-improvement)");}

    {auto& tc = list.Add("Suppportive/encouraging");
    tc.Add("Brene Brown (self-help/writing)");
    tc.Add("Rachel Hollis (self-help/entrepreneurship)");
    tc.Add("Tara Mohr (self-improvement/career)");
    tc.Add("Gabby Bernstein (self-help/guru)");
    tc.Add("Mel Robbins (motivation/speaking)");}

    {auto& tc = list.Add("Bold/brave");
    tc.Add("Michelle Obama (politics/activism)");
    tc.Add("Lizzo (music/body positivity)");
    tc.Add("Lena Dunham (writing/activism)");
    tc.Add("Brené Brown (self-help/writing)");
    tc.Add("Malala Yousafzai (activism/education)");}

    {auto& tc = list.Add("Collaborative/connector");
    tc.Add("Jenna Kutcher (entrepreneurship/photography)");
    tc.Add("Bethany Mota (fashion/entrepreneurship)");
    tc.Add("Jess Lively (self-help/business)");
    tc.Add("Lauryn Evarts Bosstick (wellness/entrepreneurship)");
    tc.Add("Aimee Song (fashion/entrepreneurship)");}

    {auto& tc = list.Add("Aspirational/motivator");
    tc.Add("Rachel Hollis (self-help/entrepreneurship)");
    tc.Add("Marie Forleo (entrepreneurship/business)");
    tc.Add("Jen Sincero (self-help/writing)");
    tc.Add("Gretchen Rubin (self-help/writing)");
    tc.Add("Mel Robbins (motivation/speaking)");}

    {auto& tc = list.Add("Personal/relatable");
    tc.Add("Esther Perel (relationships/therapy)");
    tc.Add("Stephanie Buttermore (fitness/body positivity)");
    tc.Add("Brene Brown (self-help/writing)");
    tc.Add("Sali Hughes (beauty/writing)");
    tc.Add("Glennon Doyle (self-help/writing)");}

    {auto& tc = list.Add("Inquisitive/thoughtful");
    tc.Add("Elizabeth Gilbert (writing/life advice)");
    tc.Add("Sally Rooney (writing/philosophy)");
    tc.Add("Durga Chew-Bose (writing/personal essays)");
    tc.Add("Rebecca Solnit (writing/feminism)");
    tc.Add("Roxane Gay (writing/feminism)");}

    {auto& tc = list.Add("Educator/teacher");
    tc.Add("Rachel Cargle (activism/social justice)");
    tc.Add("Brené Brown (self-help/writing)");
    tc.Add("Marie Forleo (entrepreneurship/business)");
    tc.Add("Emily Weiss (beauty/entrepreneurship)");
    tc.Add("Mel Robbins (motivation/speaking)");}
    
	ASSERT(GetPersonas().GetCount() == list.GetCount());
	return list;
}

VectorMap<String,Vector<String>>& GetPersonaSafe(bool gender) {
	if (!gender)
		return GetPersonaSafeMale();
	else
		return GetPersonaSafeFemale();
}

VectorMap<String,Vector<String>>& GetPersonaUnsafe(bool gender) {
	if (!gender)
		return GetPersonaUnsafeMale();
	else
		return GetPersonaUnsafeFemale();
}

VectorMap<String,Vector<String>>& GetPersonaNiches(bool unsafe, bool gender) {
	if (!unsafe)
		return GetPersonaSafe(gender);
	else
		return GetPersonaUnsafe(gender);
}




















const Index<String>& GetCharacters() {
	static Index<String> list;
	
	if (list.IsEmpty()) {
		list.Add("The Influencer");
		list.Add("The Authority");
		list.Add("The Critic");
		list.Add("The Storyteller");
		list.Add("The Trendsetter");
		list.Add("The Jokester");
		list.Add("The Insider");
		list.Add("The Activist");
		list.Add("The Creative");
		list.Add("The Adventurer");
		list.Add("The Lifestyle Guru");
		list.Add("Non-Niche Niche");
		list.Add("The Activator");
		list.Add("The Connector");
		list.Add("The Opinionated");
		list.Add("The Educator");
		list.Add("The Lifestyle Enthusiast");
		list.Add("The Humanitarian");
		list.Add("The Newbie");
	}
	return list;
}

const Vector<ContentType>& GetTropes() {
	thread_local static Vector<ContentType> list;
	
	if (list.IsEmpty()) {
		list.Add().Set("The Hero", "Chosen One", "Reluctant Hero", "Fallen Hero");
		list.Add().Set("The Villain", "Pure Evil", "Tragic Villain", "Anti-Villain");
		list.Add().Set("The Love Interest", "Damsel in Distress", "Rebel Love Interest", "Soulmate");
		list.Add().Set("The Mentor", "Wise Old Mentor", "Reluctant Mentor", "Betrayed Mentor");
		list.Add().Set("The Sidekick", "Loyal Sidekick", "Rebellious Sidekick", "Incompetent Sidekick");
		list.Add().Set("The Rebel", "Outlaw", "Revolutionary", "Anarchist");
		list.Add().Set("The Anti-Hero", "Dark Anti-Hero", "Sarcastic Anti-Hero", "Redeemed Anti-Hero");
		list.Add().Set("The Damsel in Distress", "Helpless Damsel", "Fiery Damsel", "Rescuer Damsel");
		list.Add().Set("The Wise Old Man/Woman", "Sage", "Trickster", "Cynic");
		list.Add().Set("The Femme Fatale", "Manipulative Femme Fatale", "Redeemed Femme Fatale", "Empowered Femme Fatale");
		list.Add().Set("The Fearless Leader", "Charismatic Leader", "Ruthless Leader", "Nurturing Leader");
		list.Add().Set("The Outcast", "Loner", "Misunderstood", "Rebel");
		list.Add().Set("The Jester", "Funny Jester", "Dark Jester", "Heartfelt Jester");
		list.Add().Set("The Loner", "Sole Survivor", "Tragic Shut-In", "Recluse");
		list.Add().Set("The Innocent", "Naive Innocent", "Pure Innocent", "Sheltered Innocent");
		list.Add().Set("The Queen/King", "Power-Hungry Monarch", "Loyal Ruler", "Reluctant Ruler");
		list.Add().Set("The Badass", "Fighter", "Rebel", "Anti-Hero");
		list.Add().Set("The Chosen One", "Reluctant Chosen One", "Redeemed Chosen One", "Corrupted Chosen One");
		list.Add().Set("The Scapegoat", "Blamed Scapegoat", "Innocent Scapegoat", "Scapegoat-Turned-Villain");
		list.Add().Set("The Fallen Hero", "Betrayed Hero", "Redeemed Villain", "Anti-Villain");
		list.Add().Set("The Misunderstood", "Angsty Misunderstood", "Empathetic Misunderstood", "Rebellious Misunderstood");
		list.Add().Set("The Alpha/Beta/Omega", "Alpha Leader", "Beta Follower", "Omega Outcast");
		list.Add().Set("The Shapeshifter", "Uncontrollable Shapeshifter", "Controlled Shapeshifter", "Shapeshifter-Turned-Hero");
		list.Add().Set("The Trickster", "Clever Trickster", "Self-Serving Trickster", "Redeemed Trickster");
		list.Add().Set("The Charming Rogue", "Smooth-Talking Rogue", "Cocky Gambler", "Rogue with a Heart of Gold");
		list.Add().Set("The Underdog", "Underestimated Underdog", "Overachieving Underdog", "Underdog-Turned-Champion");
		list.Add().Set("The Joker", "Class Clown", "Dark Joker", "Rebel Joker");
		list.Add().Set("The Survivor", "Sole Survivor", "Traumatized Survivor", "Survivor Gives Up");
	}
	return list;
}

int GetTropeCount() {
	return GetTropes().GetCount();
}


VectorMap<String,Vector<String>>& GetCharacterSafeMale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
	
	

	{auto& tc = list.Add("The Influencer");
	tc.Add("Gary Vaynerchuk (business/entrepreneurship)");
	tc.Add("Casey Neistat (vlogger/lifestyle)");
	tc.Add("Grant Cardone (sales/motivation)");
	tc.Add("Chiara Ferragni (fashion/beauty)");
	tc.Add("Peter McKinnon (photography/videography)");}

	{auto& tc = list.Add("The Authority");
	tc.Add("Tony Robbins (motivational speaker/life coach)");
	tc.Add("Tim Ferriss (entrepreneurship/productivity)");
	tc.Add("Donald Trump (politics/leadership)");
	tc.Add("Barack Obama (politics/leadership)");
	tc.Add("Malcolm Gladwell (author/behavioral psychology)");
	tc.Add("Simon Sinek (leadership/communication)");}

	{auto& tc = list.Add("The Critic");
	tc.Add("Gordon Ramsay (chef/TV personality)");
	tc.Add("Jordan Peterson (professor/author)");
	tc.Add("Seth Godin (marketing/entrepreneurship)");
	tc.Add("Niki Nakayama (chef/restaurateur)");
	tc.Add("Brian Koppelman (screenwriter/film producer)");}

	{auto& tc = list.Add("The Storyteller");
	tc.Add("Stephen King (author/screenwriter)");
	tc.Add("Tom Hanks (actor/producer)");
	tc.Add("Neil Gaiman (author/graphic novelist)");
	tc.Add("Lin-Manuel Miranda (writer/actor)");
	tc.Add("JJ Abrams (director/producer/writer)");}

	{auto& tc = list.Add("The Trendsetter");
	tc.Add("Rihanna (singer/fashion designer)");
	tc.Add("Kanye West (rapper/producer/fashion designer)");
	tc.Add("Virgil Abloh (fashion designer/creative director)");
	tc.Add("Jeffree Star (beauty influencer/entrepreneur)");
	tc.Add("Tyler, The Creator (rapper/producer/fashion designer");}

	{auto& tc = list.Add("The Jokester");
	tc.Add("Dave Chappelle (comedian/actor)");
	tc.Add("Stephen Colbert (comedian/TV host)");
	tc.Add("Ryan Reynolds (actor/comedian)");
	tc.Add("Aziz Ansari (comedian/actor)");
	tc.Add("Kevin Hart (comedian/actor)");}

	{auto& tc = list.Add("The Insider");
	tc.Add("Edward Snowden (former CIA employee/whistleblower)");
	tc.Add("Chelsea Manning (former US Army soldier/whistleblower)");
	tc.Add("Julian Assange (founder of Wikileaks/journalist)");
	tc.Add("Daniel Ellsberg (former US military analyst/whistleblower)");
	tc.Add("Eric Snowden (retired NSA employee/whistleblower)");}

	{auto& tc = list.Add("The Activist");
	tc.Add("Malala Yousafzai (education activist/Nobel Peace Prize winner)");
	tc.Add("Greta Thunberg (environmental activist)");
	tc.Add("Patrisse Cullors (co-founder of Black Lives Matter)");
	tc.Add("Martin Luther King Jr. (civil rights activist)");
	tc.Add("Mahatma Gandhi (leader of Indian independence movement)");}

	{auto& tc = list.Add("The Creative");
	tc.Add("Banksy (street artist/political activist)");
	tc.Add("Quentin Tarantino (film director/screenwriter)");
	tc.Add("Ai Weiwei (conceptual artist/activist)");
	tc.Add("David Bowie (singer/actor)");
	tc.Add("Salvador Dali (surrealist artist)");}

	{auto& tc = list.Add("The Adventurer");
	tc.Add("Bear Grylls (adventurer/TV host)");
	tc.Add("Lewis Pugh (ocean advocate/extreme swimmer)");
	tc.Add("Howard Schultz (businessman/CEO)");
	tc.Add("Alex Honnold (rock climber/adventurer)");
	tc.Add("Chris Hadfield (astronaut/author)");}

	{auto& tc = list.Add("The Lifestyle Guru");
	tc.Add("Marie Kondo (organizing consultant/TV personality)");
	tc.Add("Tony Robbins (motivational speaker/life coach)");
	tc.Add("Chrissy Teigen (TV host/model/cookbook author)");
	tc.Add("Martha Stewart (lifestyle expert/TV personality)");
	tc.Add("Tim Ferriss (entrepreneurship/productivity)");}

	{auto& tc = list.Add("Non-Niche Niche");
	tc.Add("David Beckham (former professional soccer player/model)");
	tc.Add("Oprah Winfrey (media mogul/TV host/philanthropist)");
	tc.Add("Elon Musk (entrepreneur/CEO of Tesla and SpaceX)");
	tc.Add("Ellen DeGeneres (comedian/TV host)");
	tc.Add("David Attenborough (naturalist/documentary host)");}

	{auto& tc = list.Add("The Activator");
	tc.Add("Tony Robbins (author/motivational speaker/life coach)");
	tc.Add("Malala Yousafzai (activist/Nobel Peace Prize winner)");
	tc.Add("Richard Branson (entrepreneur/CEO of Virgin Group)");
	tc.Add("Trevor Noah (comedian/TV host/activist)");
	tc.Add("Tim Ferriss (author/podcaster/lifestyle guru)");}

	{auto& tc = list.Add("The Connector");
	tc.Add("Barack Obama (former president/public speaker/author)");
	tc.Add("Simon Sinek (author/inspirational speaker/leadership expert)");
	tc.Add("Brené Brown (researcher/author/podcaster)");
	tc.Add("Bill Gates (entrepreneur/philanthropist)");
	tc.Add("Jay Shetty (former monk/podcaster/author)");}

	{auto& tc = list.Add("The Opinionated");
	tc.Add("Ben Shapiro (political commentator/commentary)");
	tc.Add("Steven Crowder (political commentator/commentary)");
	tc.Add("Joe Rogan (podcasting/politics)");
	tc.Add("Tim Pool (journalism/politics)");
	tc.Add("Tucker Carlson (political commentator/commentary)");
	tc.Add("Dave Rubin (political commentator/commentary)");
	tc.Add("Jordan Peterson (psychology/philosophy)");}
	
	{auto& tc = list.Add("The Educator");
	tc.Add("Neil deGrasse Tyson (astrophysicist/science communicator)");
	tc.Add("Tom Hanks (actor/producer/performer)");
	tc.Add("Malcolm Gladwell (author/podcaster/thought leader)");
	tc.Add("Tony Hawk (professional skateboarder/entrepreneur)");
	tc.Add("Cory Booker (politician/author/activist)");}

	{auto& tc = list.Add("The Lifestyle Enthusiast");
	tc.Add("Tim Ferriss (lifestyle design/productivity)");
	tc.Add("Lewis Howes (entrepreneurship/personal development)");
	tc.Add("Gary Vaynerchuk (entrepreneurship/marketing)");
	tc.Add("Derek Sivers (entrepreneurship/lifestyle design)");
	tc.Add("Casey Neistat (filmmaking/entrepreneurship)");
	tc.Add("Jamie Oliver (food/health)");
	tc.Add("Dave Ramsey (finance/personal development)");
	tc.Add("Tony Robbins (personal development/motivation)");}
	
	{auto& tc = list.Add("The Humanitarian");
	tc.Add("Mahatma Gandhi (activist/leader)");
	tc.Add("Leonardo DiCaprio (actor/environmental activist/philanthropist)");
	tc.Add("Paul Walker (actor/humanitarian)");
	tc.Add("Justin Trudeau (politician/activist)");
	tc.Add("Will Smith (actor/philanthropist/rapper)");}

	{auto& tc = list.Add("The Newbie");
	tc.Add("Justin Bieber (singer)");
	tc.Add("Shawn Mendes (singer/songwriter/activist)");
	tc.Add("Joe Jonas (singer/songwriter/actor)");
	tc.Add("John Boyega (actor/activist) ");
	tc.Add("Timothée Chalamet (actor/activist)");}
	
	ASSERT(GetCharacters().GetCount() == list.GetCount());
	return list;
}

VectorMap<String,Vector<String>>& GetCharacterSafeFemale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
    
	{auto& tc = list.Add("The Influencer");
	tc.Add("Sarah Huckabee Sanders (former White House press secretary)");
	tc.Add("Nikki Haley (former UN ambassador/former governor)");
	tc.Add("Ann Coulter (author/commentator) ");
	tc.Add("Mary Matalin (political consultant/commentator) ");
	tc.Add("Condoleezza Rice (former secretary of state/political scientist)");}

	{auto& tc = list.Add("The Authority");
	tc.Add("Elizabeth Dole (former senator/cabinet member)");
	tc.Add("Kellyanne Conway (former White House counselor)");
	tc.Add("Condoleezza Rice (former secretary of state/political scientist)");
	tc.Add("Carly Fiorina (former business executive/presidential candidate)");
	tc.Add("Susan Collins (senator)");}

	{auto& tc = list.Add("The Critic");
	tc.Add("Megyn Kelly (journalist/commentator) ");
	tc.Add("Laura Ingraham (political commentator/talk show host) ");
	tc.Add("Michelle Malkin (political commentator/author) ");
	tc.Add("Candace Owens (political commentator/activist)");
	tc.Add("Tomi Lahren (political commentator/activist)");}

	{auto& tc = list.Add("The Storyteller");
	tc.Add("Laura Bush (former first lady/educator)");
	tc.Add("Peggy Noonan (author/columnist/speechwriter)");
	tc.Add("Jenna Ellis (political commentator/author/lawyer)");
	tc.Add("Sarah Palin (former governor/vp candidate/author)");
	tc.Add("Mercedes Schlapp (political commentator/former White House director of strategic communications)");}

	{auto& tc = list.Add("The Trendsetter");
	tc.Add("Ivanka Trump (advisor to the president)");
	tc.Add("Betsy DeVos (secretary of education/former businesswoman)");
	tc.Add("Lara Trump (political consultant/TV producer)");
	tc.Add("Jaelen King (social media strategist/activist)");
	tc.Add("Angela \"Bay\" Buchanan (political commentator/strategist)");}

	{auto& tc = list.Add("The Jokester");
	tc.Add("Heather Nauert (former State Department spokeswoman/TV host)");
	tc.Add("Dana Perino (former White House press secretary/political commentator)");
	tc.Add("Allie Beth Stuckey (political commentator/comedian)");
	tc.Add("Gretchen Carlson (TV anchor/women's rights advocate)");
	tc.Add("Nia-Malika Henderson (senior political reporter/correspondent)");}

	{auto& tc = list.Add("The Insider");
	tc.Add("Hope Hicks (former White House communications director)");
	tc.Add("Elise Stefanik (congresswoman)");
	tc.Add("Mary-Kate Fisher (political correspondent/journalist)");
	tc.Add("Jenna Bush Hager (journalist/author/TV personality)");
	tc.Add("Candice Miller (congresswoman/former Michigan secretary of state)");}

	{auto& tc = list.Add("The Activist");
	tc.Add("Kimberly Guilfoyle (political commentator/activist)");
	tc.Add("Candace Owens (political commentator/activist)");
	tc.Add("Nikki Haley (former UN ambassador/governor)");
	tc.Add("Tomi Lahren (political commentator/activist)");
	tc.Add("Marsha Blackburn (senator/activist)");}

	{auto& tc = list.Add("The Creative");
	tc.Add("Melissa Francis (TV host/author/actress)");
	tc.Add("Inez Stepman (senior policy analyst/author/activist)");
	tc.Add("Lauren Simonetti (TV reporter/news anchor)");
	tc.Add("Mindy Finn (political strategist/digital marketer)");
	tc.Add("Meghan McCain (TV host/columnist/author)");}


	{auto& tc = list.Add("The Adventurer");
	tc.Add("Joni Ernst (senator/military veteran)");
	tc.Add("Betsy DeVos (secretary of education/former businesswoman)");
	tc.Add("Sarah Palin (former governor/VP candidate/author)");
	tc.Add("Carly Fiorina (former business executive/presidential candidate)");
	tc.Add("Nikki Haley (former UN ambassador/governor)");}

	{auto& tc = list.Add("The Lifestyle Guru");
	tc.Add("Ivanka Trump (advisor to the president)");
	tc.Add("Melania Trump (First Lady/entrepreneur)");
	tc.Add("Elaine Chao (secretary of transportation/former secretary of labor)");
	tc.Add("Kayleigh McEnany (former White House press secretary/political commentator)");
	tc.Add("Leslie Sanchez (political commentator/author/strategist)");}

	{auto& tc = list.Add("Non-Niche Niche");
	tc.Add("Anna Paulina Luna (political commentator/activist)");
	tc.Add("Aja Smith (political commentator/activist/speaker)");
	tc.Add("Ashley Pratte (political commentator/strategist)");
	tc.Add("Buffy Wicks (political strategist/activist)");
	tc.Add("Sarah Chamberlain (CEO/activist/strategist)");}

	{auto& tc = list.Add("The Activator");
	tc.Add("Lara Trump (political consultant/TV producer)");
	tc.Add("Candice Miller (congresswoman/former Michigan secretary of state)");
	tc.Add("Jaelen King (social media strategist/activist)");
	tc.Add("Kelli Ward (congresswoman/political commentator)");
	tc.Add("Marsha Blackburn (senator/activist)");}

	{auto& tc = list.Add("The Connector");
	tc.Add("Nikki Haley (former UN ambassador/governor)");
	tc.Add("Elaine Chao (secretary of transportation/former secretary of labor)");
	tc.Add("Sarah Huckabee Sanders (former White House press secretary)");
	tc.Add("Maria Bartiromo (journalist/TV anchor)");
	tc.Add("Liz Cheney (congresswoman/lawyer/political commentator)");}

	{auto& tc = list.Add("The Opinionated");
	tc.Add("Tomi Lahren (political commentator/activist)");
	tc.Add("Kimberly Guilfoyle (political commentator/activist)");
	tc.Add("Candace Owens (political commentator/activist)");
	tc.Add("Ann Coulter (author/commentator)");
	tc.Add("S.E. Cupp (political commentator/TV host)");}

	{auto& tc = list.Add("The Educator");
	tc.Add("Betsy DeVos (secretary of education/former businesswoman)");
	tc.Add("Candice Miller (congresswoman/former Michigan secretary of state)");
	tc.Add("Nikki Haley (former UN ambassador/governor)");
	tc.Add("Carly Fiorina (former business executive/presidential candidate)");
	tc.Add("Elaine Chao (secretary of transportation/former secretary of labor)");}

	{auto& tc = list.Add("The Lifestyle Enthusiast");
	tc.Add("Ivanka Trump (advisor to the president)");
	tc.Add("Sarah Palin (former governor/VP candidate/author)");
	tc.Add("Nikki Haley (former UN ambassador/governor)");
	tc.Add("Melania Trump (First Lady/entrepreneur)");
	tc.Add("Lara Trump (political consultant/TV producer)");}

	{auto& tc = list.Add("The Humanitarian");
	tc.Add("Ivanka Trump (advisor to the president)");
	tc.Add("Betsy DeVos (secretary of education/former businesswoman)");
	tc.Add("Liz Cheney (congresswoman/lawyer/political commentator)");
	tc.Add("Heather Nauert (former State Department spokesperson/TV host)");
	tc.Add("Kayleigh McEnany (former White House press secretary/political commentator)");}

	{auto& tc = list.Add("The Newbie");
	tc.Add("Madison Cawthorn (congressman/youngest in Congress)");
	tc.Add("Marjorie Taylor Greene (congresswoman/controversial figure)");
	tc.Add("Kat Cammack (congresswoman/youngest woman in Congress)");
	tc.Add("Nicole Malliotakis (congresswoman/first Greek-American woman elected to Congress)");
	tc.Add("Young Kim (congresswoman/first Korean-American Republican woman elected to Congress)");}
	
	ASSERT(GetCharacters().GetCount() == list.GetCount());
	return list;
}

VectorMap<String,Vector<String>>& GetCharacterUnsafeMale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
	
	{auto& tc = list.Add("The Influencer");
	tc.Add("Kanye West (rapper/producer/fashion designer)");
	tc.Add("Elon Musk (entrepreneur/inventor/CEO)");
	tc.Add("Conor McGregor (MMA fighter/entrepreneur)");
	tc.Add("Howard Stern (radio personality/author/actor)");
	tc.Add("Art Bell (radio host)");}

	{auto& tc = list.Add("The Authority");
	tc.Add("Alex Jones (radio host/conspiracy theorist)");
	tc.Add("Donald TV-Boss (former US President/lawyer/author)");
	tc.Add("Joe Rogan (comedian/podcast host/actor)");
	tc.Add("Russell Brand (comedian/actor/author)");
	tc.Add("Tucker Carlson (political commentator/TV host/editor)");}

	{auto& tc = list.Add("The Critic");
	tc.Add("Simon Cowell (TV producer/judge/entertainment mogul)");
	tc.Add("Gordon Ramsay (celebrity chef/restaurateur/TV personality)");
	tc.Add("Simon Cowell (TV producer/judge/entertainment mogul)");
	tc.Add("Howard Stern (radio personality/author/actor)");
	tc.Add("Piers Morgan (TV presenter/journalist/author)");}

	{auto& tc = list.Add("The Storyteller");
	tc.Add("Morgan Freeman (actor/narrator/producer)");
	tc.Add("Quentin Tarantino (filmmaker/actor/screenwriter)");
	tc.Add("Neil Gaiman (author/screenwriter/comic book writer)");
	tc.Add("David Sedaris (humorist/author/radio contributor)");
	tc.Add("Wes Anderson (filmmaker/screenwriter/producer)");}

	{auto& tc = list.Add("The Trendsetter");
	tc.Add("Kanye West (rapper/producer/fashion designer)");
	tc.Add("Jeffree Star (makeup artist/entrepreneur/YouTube personality)");
	tc.Add("David Beckham (former professional soccer player/entrepreneur/model)");
	tc.Add("Andrew Yang (entrepreneur/politician/author)");
	tc.Add("David Dobrik (YouTube personality/actor/documentary filmmaker)");}

	{auto& tc = list.Add("The Jokester");
	tc.Add("Kevin Hart (comedian/actor/producer)");
	tc.Add("Seth Rogen (actor/comedian/screenwriter)");
	tc.Add("Dave Chappelle (comedian/actor/screenwriter)");
	tc.Add("Ricky Gervais (comedian/actor/screenwriter)");
	tc.Add("Bill Burr (comedian/actor/producer)");}

	{auto& tc = list.Add("The Insider");
	tc.Add("Julian Assange (journalist/hacker/activist)");
	tc.Add("Edward Snowden (whistleblower/activist/former CIA employee)");
	tc.Add("Mark Felt (former FBI agent/Deep Throat informant)");
	tc.Add("Bob Woodward (journalist/author)");
	tc.Add("Julian Edelman (professional football player/author)");}

	{auto& tc = list.Add("The Activist");
	tc.Add("Colin Kaepernick (former professional football player/activist)");
	tc.Add("Martin Luther King Jr. (activist/minister/leader of the Civil Rights Movement)");
	tc.Add("Greta Thunberg (environmental activist)");
	tc.Add("Malcolm X (human rights activist/minister/leader of the Black Power movement)");
	tc.Add("Harvey Milk (gay rights activist/politician)");}

	{auto& tc = list.Add("The Creative");
	tc.Add("Wes Anderson (filmmaker/screenwriter/producer)");
	tc.Add("Salvador Dali (artist/surrealist)");
	tc.Add("Andy Warhol (artist/filmmaker)");
	tc.Add("Damien Hirst (artist)");
	tc.Add("Banksy (street artist/filmmaker/activist)");}

	{auto& tc = list.Add("The Adventurer");
	tc.Add("Sir Edmund Hillary (mountaineer/explorer/philanthropist)");
	tc.Add("Bear Grylls (adventurer/TV presenter/writer)");
	tc.Add("Steve Irwin (wildlife expert/TV personality/conservationist)");
	tc.Add("Philippe Petit (tightrope walker/high-wire artist)");
	tc.Add("Felix Baumgartner (austrian skydiver/base jumper)");}

	{auto& tc = list.Add("The Lifestyle Guru");
	tc.Add("Tony Robbins (motivational speaker/author/entrepreneur)");
	tc.Add("Tim Ferriss (entrepreneur/author/podcast host)");
	tc.Add("Deepak Chopra (spiritual teacher/author)");
	tc.Add("Dave Asprey (entrepreneur/author/health advocate)");
	tc.Add("Gary Vaynerchuk (entrepreneur/investor/author)");}

	{auto& tc = list.Add("Non-Niche Niche");
	tc.Add("Shia LaBeouf (actor/artist/filmmaker)");
	tc.Add("James Franco (actor/director/producer)");
	tc.Add("Jaden Smith (actor/musician/entrepreneur)");
	tc.Add("Jared Leto (actor/musician/director)");
	tc.Add("Kanye West (rapper/producer/fashion designer)");}

	{auto& tc = list.Add("The Activator");
	tc.Add("Steve Jobs (entrepreneur/CEO/founder of Apple Inc.)");
	tc.Add("Jeff Bezos (entrepreneur/CEO/founder of Amazon)");
	tc.Add("Richard Branson (entrepreneur/CEO/founder of Virgin Group)");
	tc.Add("Mark Zuckerberg (entrepreneur/CEO/co-founder of Facebook)");
	tc.Add("Elon Musk (entrepreneur/inventor/CEO)");}

	{auto& tc = list.Add("The Connector");
	tc.Add("Tim Ferriss (entrepreneur/author/podcast host)");
	tc.Add("Kevin Bacon (actor/musical artist)");
	tc.Add("Malcolm Gladwell (author/journalist/public speaker)");
	tc.Add("Gary Vaynerchuk (entrepreneur/investor/author)");
	tc.Add("Oprah Winfrey (media executive/actress/talk show host)");}

	{auto& tc = list.Add("The Opinionated");
	tc.Add("Bill O'Reilly (political commentator/TV host/author)");
	tc.Add("Tucker Carlson (political commentator/TV host/editor)");
	tc.Add("Milo Yiannopoulos (journalist/author/political commentator)");
	tc.Add("Ben Shapiro (conservative political commentator/columnist/author)");
	tc.Add("Rush Limbaugh (conservative radio host/author)");}

	{auto& tc = list.Add("The Educator");
	tc.Add("Neil deGrasse Tyson (astrophysicist/science communicator/author)");
	tc.Add("Bill Nye (mechanical engineer/science educator/TV host)");
	tc.Add("Noam Chomsky (linguist/philosopher/political activist)");
	tc.Add("Carl Sagan (astronomer/astrophysicist/author)");
	tc.Add("Jordan Peterson (psychologist/author) ");}

	{auto& tc = list.Add("The Lifestyle Enthusiast");
	tc.Add("Jeffree Star (makeup artist/entrepreneur/YouTube personality)");
	tc.Add("David Beckham (former professional soccer player/entrepreneur/model)");
	tc.Add("Scott Disick (TV personality/entrepreneur/model)");
	tc.Add("Jason Momoa (actor/producer/director)");
	tc.Add("Dwayne Johnson (actor/producer/wrestler) ");}

	{auto& tc = list.Add("The Humanitarian");
	tc.Add("Malala Yousafzai (noble peace prize laureate/activist/author)");
	tc.Add("Mahatma Gandhi (political leader/activist)");
	tc.Add("Martin Luther King Jr. (activist/minister/leader of the Civil Rights Movement)");
	tc.Add("Mother Teresa (Catholic nun/missionary/humanitarian)");
	tc.Add("Elie Wiesel (Nobel Peace Prize laureate/activist/author) ");}

	{auto& tc = list.Add("The Newbie");
	tc.Add("Timothee Chalamet (actor)");
	tc.Add("Tom Holland (actor)");
	tc.Add("Noah Centineo (actor)");
	tc.Add("Ansel Elgort (actor)");
	tc.Add("Jacob Elordi (actor)");}
	
	ASSERT(GetCharacters().GetCount() == list.GetCount());
	return list;
}

VectorMap<String,Vector<String>>& GetCharacterUnsafeFemale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
		{auto& tc = list.Add("The Influencer");
	tc.Add("Cardi B (rapper/actress/entrepreneur)");
	tc.Add("Kim Kardashian (reality TV star/businesswoman)");
	tc.Add("Huda Kattan (beauty blogger/entrepreneur)");
	tc.Add("Brené Brown (researcher/author/podcaster)");
	tc.Add("Naomi Klein (activist/journalist/author)");}

	{auto& tc = list.Add("The Authority");
	tc.Add("Oprah Winfrey (media mogul/philanthropist/actress)");
	tc.Add("Sheryl Sandberg (business executive/author)");
	tc.Add("Malala Yousafzai (human rights activist/Nobel Prize laureate)");
	tc.Add("Jacinda Ardern (prime minister of New Zealand)");
	tc.Add("Angela Merkel (Chancellor of Germany) ");}

	{auto& tc = list.Add("The Critic");
	tc.Add("AOC (political activist/congresswoman)");
	tc.Add("Greta Thunberg (environmental activist)");
	tc.Add("Roxane Gay (author/activist)");
	tc.Add("Chrissy Teigen (model/author/TV personality/social media critic)");
	tc.Add("Amanda Seales (actress/comedian/activist)");}

	{auto& tc = list.Add("The Storyteller");
	tc.Add("Chimamanda Ngozi Adichie (author/feminist)");
	tc.Add("J.K. Rowling (author/screenwriter/philanthropist)");
	tc.Add("Mindy Kaling (actress/producer/writer)");
	tc.Add("Lizzo (singer/rapper)");
	tc.Add("Ava DuVernay (filmmaker/activist)");}

	{auto& tc = list.Add("The Trendsetter");
	tc.Add("Rihanna (singer/entrepreneur/fashion designer)");
	tc.Add("Kylie Jenner (businesswoman/entrepreneur/influencer)");
	tc.Add("Alexa Chung (model/TV presenter/fashion designer)");
	tc.Add("Halsey (singer/songwriter)");
	tc.Add("Anna Wintour (editor-in-chief of Vogue)");}

	{auto& tc = list.Add("The Jokester");
	tc.Add("Tiffany Haddish (actress/comedian)");
	tc.Add("Ali Wong (comedian/actress/writer)");
	tc.Add("Phoebe Robinson (comedian/actress)");
	tc.Add("Amy Schumer (comedian/actress/writer)");
	tc.Add("Aidy Bryant (comedian/actress)");}

	{auto& tc = list.Add("The Insider");
	tc.Add("Beyoncé (singer/songwriter/actress/entrepreneur)");
	tc.Add("Serena Williams (professional tennis player/entrepreneur)");
	tc.Add("Jennifer Lawrence (actress/philanthropist)");
	tc.Add("Meghan Markle (actress/royal family member)");
	tc.Add("Reese Witherspoon (actress/producer/entrepreneur)");}

	{auto& tc = list.Add("The Activist");
	tc.Add("Amandla Stenberg (actress/activist)");
	tc.Add("Jane Fonda (actress/activist)");
	tc.Add("Tarana Burke (activist/founder of the Me Too movement)");
	tc.Add("Diane Guerrero (actress/immigrant rights activist)");
	tc.Add("Gloria Steinem (feminist/social and political activist)");}

	{auto& tc = list.Add("The Creative");
	tc.Add("Solange Knowles (singer/songwriter/actress)");
	tc.Add("Ava DuVernay (filmmaker/activist)");
	tc.Add("Shonda Rhimes (TV producer/screenwriter)");
	tc.Add("Roxane Gay (author/activist)");
	tc.Add("Carrie Brownstein (musician/actress/writer/director)");}

	{auto& tc = list.Add("The Adventurer");
	tc.Add("Bear Grylls (adventurer/TV presenter)");
	tc.Add("Amelia Earhart (aviator/author)");
	tc.Add("Alex Honnold (free solo rock climber)");
	tc.Add("Cheryl Strayed (author/adventurer)");
	tc.Add("Samantha Brown (TV host/travel expert)");}

	{auto& tc = list.Add("The Lifestyle Guru");
	tc.Add("Marie Kondo (organizing consultant/author)");
	tc.Add("Gwyneth Paltrow (actress/entrepreneur/wellness guru)");
	tc.Add("Joanna Gaines (designer/TV personality/author)");
	tc.Add("Jessica Alba (actress/entrepreneur/CEO of The Honest Company)");
	tc.Add("Kris Carr (author/wellness coach/filmmaker)");}

	{auto& tc = list.Add("Non-Niche Niche");
	tc.Add("Tiffany Haddish (actress/comedian)");
	tc.Add("Chrissy Teigen (model/author/TV personality/social media influencer)");
	tc.Add("Mindy Kaling (actress/producer/writer)");
	tc.Add("Ali Wong (comedian/actress/writer)");
	tc.Add("Aidy Bryant (comedian/actress)");}

	{auto& tc = list.Add("The Activator");
	tc.Add("Alexandria Ocasio-Cortez (political activist/congresswoman)");
	tc.Add("Greta Thunberg (environmental activist)");
	tc.Add("Tarana Burke (activist/founder of the Me Too movement)");
	tc.Add("Jane Fonda (actress/activist)");
	tc.Add("Malala Yousafzai (human rights activist/Nobel Prize laureate)");}

	{auto& tc = list.Add("The Connector");
	tc.Add("Oprah Winfrey (media mogul/philanthropist/actress)");
	tc.Add("Sheryl Sandberg (business executive/author)");
	tc.Add("Arianna Huffington (media mogul/author/businesswoman)");
	tc.Add("Melinda Gates (philanthropist/businesswoman)");
	tc.Add("Sophia Amoruso (entrepreneur/author)");}

	{auto& tc = list.Add("The Opinionated");
	tc.Add("Roxane Gay (author/activist)");
	tc.Add("Naomi Klein (activist/journalist/author)");
	tc.Add("Jacinda Ardern (prime minister of New Zealand)");
	tc.Add("AOC (political activist/congresswoman)");
	tc.Add("Amanda Seales (actress/comedian/activist)");}

	{auto& tc = list.Add("The Educator");
	tc.Add("Brené Brown (researcher/author/podcaster)");
	tc.Add("Chimamanda Ngozi Adichie (author/feminist)");
	tc.Add("Malala Yousafzai (human rights activist/Nobel Prize laureate)");
	tc.Add("Jane Goodall (primatologist/anthropologist/conservationist)");}

	{auto& tc = list.Add("The Lifestyle Enthusiast");
	tc.Add("Jamie Oliver (celebrity chef/activist)");
	tc.Add("Goop (wellness and lifestyle brand founded by Gwyneth Paltrow)");
	tc.Add("Ina Garten (author/TV host/entrepreneur)");
	tc.Add("Kourtney Kardashian (reality TV star/entrepreneur/wellness advocate)");
	tc.Add("Marie Forleo (author/life coach/entrepreneur)");}

	{auto& tc = list.Add("The Humanitarian");
	tc.Add("Angelina Jolie (actress/humanitarian)");
	tc.Add("Princess Diana (humanitarian/royal family member)");
	tc.Add("Melinda Gates (philanthropist/businesswoman)");
	tc.Add("Malala Yousafzai (human rights activist/Nobel Prize laureate)");
	tc.Add("Ellen Degeneres (comedian/TV host/philanthropist)");}

	{auto& tc = list.Add("The Newbie");
	tc.Add("Zendaya (actress/singer)");
	tc.Add("Yara Shahidi (actress/activist)");
	tc.Add("Maisie Williams (actress/activist)");
	tc.Add("Lili Reinhart (actress/activist)");
	tc.Add("Millie Bobby Brown (actress/activist)");}
	
	ASSERT(GetCharacters().GetCount() == list.GetCount());
	return list;
}

VectorMap<String,Vector<String>>& GetCharacterSafe(bool gender) {
	if (!gender)
		return GetCharacterSafeMale();
	else
		return GetCharacterSafeFemale();
}

VectorMap<String,Vector<String>>& GetCharacterUnsafe(bool gender) {
	if (!gender)
		return GetCharacterUnsafeMale();
	else
		return GetCharacterUnsafeFemale();
}

VectorMap<String,Vector<String>>& GetCharacterTropes(bool unsafe, bool gender) {
	if (!unsafe)
		return GetCharacterSafe(gender);
	else
		return GetCharacterUnsafe(gender);
}


















const Index<String>& GetStyles() {
	static Index<String> list;
	
	if (list.IsEmpty()) {
		list.Add("Intense");
		list.Add("Dark and brooding");
		list.Add("Whimsical and fantastical");
		list.Add("Sci-fi/Futuristic");
		list.Add("Film noir");
		list.Add("Comedic");
		list.Add("Romantic");
		list.Add("Family-friendly");
		list.Add("Adventure/Action");
		list.Add("Animated");
		list.Add("Period Piece");
		list.Add("Musical");
		list.Add("Dramatic");
		list.Add("Horror");
		list.Add("Mockumentary");
		list.Add("Documentary-style");
		list.Add("Experimental/Avant-garde");
		list.Add("Thriller/Suspense");
		list.Add("Mystery/Crime");
		list.Add("Western");
		list.Add("War/Epic");
		list.Add("Slice of Life");
		list.Add("Satire");
		list.Add("Surrealism");
	}
	return list;
}

const Vector<ContentType>& GetApproaches() {
	thread_local static Vector<ContentType> list;
	
	if (list.IsEmpty()) {
		list.Add().Set("Sequential", "Linear", "Non-linear", "Cyclical");
		list.Add().Set("Parallel", "Simultaneous", "Interweaving", "Parallel Universes");
		list.Add().Set("Split-screen", "Side by Side", "Overlapping", "Contrasting");
		list.Add().Set("Montage", "Montage of Happiness", "Montage of Tragedy", "Anti-Montage");
		list.Add().Set("Flashback/Flashforward", "Nostalgic Flashback", "Traumatic Flashback", "Hopeful Flashforward");
		list.Add().Set("First person POV", "Immersive", "Introspective", "Detached");
		list.Add().Set("Multiple POVs", "Collage of Perspectives", "Unreliable Narrators", "Contrasting POVs");
		list.Add().Set("Non-linear", "Chaotic", "Fragmented", "Achronological");
		list.Add().Set("Linear", "Clear Narrative", "Progressive", "Predictable");
		list.Add().Set("Real-time", "Tension-building", "Intense", "Slow-paced");
		list.Add().Set("Dream sequence", "Surreal", "Nightmarish", "Fantastical");
		list.Add().Set("Voiceover narration", "Informative", "Reflective", "Misleading");
		list.Add().Set("Metaphorical/Symbolic", "Allegorical", "Metaphysical", "Literal");
		list.Add().Set("Documentary-style", "Realistic", "Mockumentary", "Biographical");
		list.Add().Set("Experimental/Avant-garde", "Non-conventional", "Deconstructed", "Provocative");
		list.Add().Set("Animated", "Cartoonish", "Gritty", "Anime-inspired");
		list.Add().Set("Mash-up", "Unexpected Combination", "Satirical", "Remixed");
		list.Add().Set("Remix", "Reimagined", "Modernized", "Reinvented");
		list.Add().Set("Virtual Reality/Augmented Reality", "Immersive", "Interactive", "Escapist");
		list.Add().Set("Collage", "Fragmented", "Mixed Media", "Chaos");
		list.Add().Set("Split-screen", "Parallel Stories", "Contrasting Perspectives", "Altered Reality");
		list.Add().Set("Flipbook", "Playful", "Rhythmic", "Nostalgic");
		list.Add().Set("Motion graphics", "Graphic", "Animated", "Minimalistic");
		list.Add().Set("Minimalist", "Simplistic", "Abstract", "Meditative");
		list.Add().Set("Surreal/Fantasy", "Dreamlike", "Whimsical", "Dark");
		list.Add().Set("Minimalist/Fluid storytelling", "Efficient", "Streamlined", "Dynamic");
	}
	return list;
}

int GetApproachCount() {
	return GetNiches().GetCount();
}


VectorMap<String,Vector<String>>& GetStyleSafeMale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
	
	{auto& tc = list.Add("Intense");
	tc.Add("The opening sequence of the movie Saving Private Ryan, with fast-paced action and intense battle scenes.");
	tc.Add("A suspenseful horror movie, with dark and moody visuals and heart-pumping music.");
	tc.Add("An action-packed spy thriller, with high-speed chase scenes and dramatic fight sequences.");
	tc.Add("A dramatic war epic, with intense battle scenes and emotional dialogue.");
	tc.Add("A crime drama, with gritty and intense visuals and dialogue.");
	tc.Add("A high-stakes sports movie, with intense training and competition scenes.");}

	{auto& tc = list.Add("Dark and brooding");
	tc.Add("A psychological thriller, with dark and mysterious visuals and dialogue.");
	tc.Add("A neo-noir crime drama, with moody and atmospheric shots.");
	tc.Add("A gothic vampire film, with dark and dramatic scenes.");
	tc.Add("A dystopian sci-fi movie, with a bleak and oppressive atmosphere.");
	tc.Add("A revenge thriller, with brooding and intense imagery.");
	tc.Add("A character study drama, with somber and introspective scenes.");}

	{auto& tc = list.Add("Whimsical and fantastical");
	tc.Add("A fantasy adventure, with vibrant and colorful visuals and fantastical creatures.");
	tc.Add("A family-friendly animated film, with playful and imaginative visuals.");
	tc.Add("A romantic comedy, with light-hearted and whimsical dialogue.");
	tc.Add("A coming-of-age drama, with fantastical and dreamlike sequences.");
	tc.Add("A musical set in a magical world, with enchanting and playful musical numbers.");
	tc.Add("A fairytale retelling, with charming and whimsical storytelling.");}

	{auto& tc = list.Add("Sci-fi/Futuristic");
	tc.Add("An epic space opera, with futuristic technology and otherworldly landscapes.");
	tc.Add("A mind-bending sci-fi thriller, with high-tech visuals and complex concepts.");
	tc.Add("A cyberpunk adventure, with sleek and futuristic visuals and action sequences.");
	tc.Add("A time-travel adventure, with futuristic and otherworldly landscapes.");
	tc.Add("A post-apocalyptic dystopia, with advanced technology and a bleak and desolate atmosphere.");
	tc.Add("A thought-provoking sci-fi drama, with futuristic concepts and emotional depth.");}

	{auto& tc = list.Add("Film noir");
	tc.Add("A neo-noir crime thriller, with shadowy and atmospheric visuals and complex characters.");
	tc.Add("A detective mystery, with moody and brooding shots and a hard-boiled protagonist.");
	tc.Add("A classic film noir, with stylish visuals and a suspenseful plot.");
	tc.Add("A dark and gritty detective drama, with a noir-inspired aesthetic.");
	tc.Add("A neo-noir sci-fi movie, with a futuristic twist on the classic film noir genre.");
	tc.Add("A psychological thriller with noir elements, featuring a morally ambiguous protagonist and a twisty plot.");}

	{auto& tc = list.Add("Comedic");
	tc.Add("A buddy comedy, with hilarious hijinks and banter between friends.");
	tc.Add("A slapstick comedy, with over-the-top physical comedy and goofy characters.");
	tc.Add("A romantic comedy, with funny and heartwarming moments between the couple.");
	tc.Add("A satire, with clever and witty commentary on societal issues or pop culture.");
	tc.Add("A workplace comedy, with quirky and relatable characters navigating office dynamics.");
	tc.Add("A parody of a specific genre or movie, with exaggerated and humorous takes on clichés.");}

	{auto& tc = list.Add("Romantic");
	tc.Add("A sweeping romance, with stunning visuals and passionate love scenes.");
	tc.Add("A romantic comedy, with both humorous and heartfelt moments between the couple.");
	tc.Add("A period piece romance, with elegant and romantic settings and costumes.");
	tc.Add("A tragic love story, with emotional and heart-wrenching scenes.");
	tc.Add("A fantasy romance, with a unique and romantic premise.");
	tc.Add("A romantic drama, with a realistic and emotionally-charged portrayal of a relationship.");}

	{auto& tc = list.Add("Family-friendly");
	tc.Add("A heartwarming family adventure, with lovable characters and feel-good moments.");
	tc.Add("An animated film for all ages, with vibrant and imaginative visuals and positive messages.");
	tc.Add("A coming-of-age story for kids, with relatable and empowering themes.");
	tc.Add("A fantasy adventure, with magical and enchanting worlds and characters.");
	tc.Add("A sports movie about teamwork and determination, with an inspirational message.");
	tc.Add("A wholesome family comedy, with lovable and quirky characters and funny situations.");}

	{auto& tc = list.Add("Adventure/Action");
	tc.Add("A high-stakes heist movie, with thrilling action sequences and a clever plot.");
	tc.Add("An epic adventure with daring stunts and exotic locations.");
	tc.Add("A spy thriller, with intense action and suspenseful spy missions.");
	tc.Add("A survival movie, with adrenaline-pumping scenes and a fight for survival.");
	tc.Add("An epic fantasy journey, with epic battles and fantastical creatures.");
	tc.Add("A post-apocalyptic adventure, with intense action and a quest for survival.");}

	{auto& tc = list.Add("Animated");
	tc.Add("An animated musical with catchy songs and lovable characters.");
	tc.Add("A comedy adventure with hilarious and endearing animated animals.");
	tc.Add("A coming-of-age story about a young hero on a quest, with stunning animation and a heartwarming message.");
	tc.Add("A computer-animated comedy starring a group of lovable and quirky characters.");
	tc.Add("A family-friendly animated film with a heartwarming message and visually stunning scenes.");
	tc.Add("An action-packed animated movie with a diverse and dynamic group of heroes.");}

	{auto& tc = list.Add("Period Piece");
	tc.Add("A historical drama set in a specific time period, with striking costumes and sets.");
	tc.Add("A romantic period piece, with sweeping romance and beautiful cinematography.");
	tc.Add("A biopic about a famous historical figure, with stunning visuals and a compelling story.");
	tc.Add("A war drama set in a specific era, with realistic and emotional depictions of historical events.");
	tc.Add("A costume drama, featuring lavish costumes and grand settings.");
	tc.Add("A literary adaptation set in the past, with rich storytelling and immersive period details.");}

	{auto& tc = list.Add("Musical");
	tc.Add("A Broadway musical adaptation with lavish sets and show-stopping musical numbers.");
	tc.Add("A jukebox musical featuring popular songs, with energetic dance numbers and a heartwarming story.");
	tc.Add("A musical biopic, with powerful performances and iconic songs.");
	tc.Add("A modern musical with catchy and original songs, featuring a diverse and talented cast.");
	tc.Add("A family-friendly movie with musical numbers and a heartwarming message.");
	tc.Add("A romantic musical with sweeping love songs and dazzling choreography.");}

	{auto& tc = list.Add("Dramatic");
	tc.Add("A character-driven drama, with intense and emotional scenes between complex characters.");
	tc.Add("A family drama with realistic and relatable portrayals of relationships and conflicts.");
	tc.Add("A legal drama, with gripping courtroom scenes and thought-provoking themes.");
	tc.Add("A biographical drama about a real-life figure, with powerful performances and a moving story.");
	tc.Add("An ensemble drama, with interconnected storylines and compelling character development.");
	tc.Add("A war drama, with powerful and emotional depictions of human struggle and sacrifice.");}

	{auto& tc = list.Add("Horror");
	tc.Add("A psychological horror movie, with mind-bending scares and a twisted plot.");
	tc.Add("A supernatural horror film, with eerie and unsettling visuals and jump scares.");
	tc.Add("A monster movie, with terrifying creatures and frightening encounters.");
	tc.Add("A slasher film, with suspenseful chase scenes and gruesome kills.");
	tc.Add("A horror comedy, with a mix of humor and scares.");
	tc.Add("A found footage-style movie, with a sense of realism and found footage elements.");}

	{auto& tc = list.Add("Mockumentary");
	tc.Add("A comedy mockumentary about a quirky workplace or community.");
	tc.Add("A satire/mockumentary about a specific topic or industry, poking fun at its tropes and clichés.");
	tc.Add("A found footage-style movie, parodying and subverting the genre's conventions.");
	tc.Add("A mockumentary about a fictional event or historical figure, with witty and humorous commentary.");
	tc.Add("A musical mockumentary, featuring mock interviews and musical numbers.");
	tc.Add("A slapstick comedy mockumentary, following the antics of a bumbling protagonist.");}

	{auto& tc = list.Add("Documentary-style");
	tc.Add("A historical documentary, presenting real events and interviews with expert commentary.");
	tc.Add("A nature documentary, with stunning footage and educational narration.");
	tc.Add("A true crime documentary, with gripping interviews and investigative storytelling.");
	tc.Add("A sports documentary, exploring the struggles and triumphs of athletes and teams.");
	tc.Add("A music documentary, exploring the careers and legacies of famous musicians.");
	tc.Add("A social issue/documentary hybrid, shining a light on important topics and featuring real people and their stories.");}

	{auto& tc = list.Add("Experimental/Avant-garde");
	tc.Add("A surreal and visually striking experimental film with no traditional narrative structure.");
	tc.Add("An avant-garde film exploring complex and abstract concepts through unique visual storytelling.");
	tc.Add("A non-linear experimental film, disorienting the viewer and challenging traditional storytelling conventions.");
	tc.Add("A hybrid of live-action and animation, blurring the lines between reality and fantasy.");
	tc.Add("A character study film with bold and unconventional techniques, diving deep into the psyche of the protagonist.");
	tc.Add("A highly stylized and visually stunning experimental film, exploring themes of identity and perception.");}

	{auto& tc = list.Add("Thriller/Suspense");
	tc.Add("A kidnapping thriller, with tense and suspenseful scenes and a race against time.");
	tc.Add("A psychological thriller, with mind games and a twisty plot.");
	tc.Add("A political thriller, with high-stakes and dangerous conspiracy theories.");
	tc.Add("A home invasion thriller, with a terrifying cat-and-mouse game between the villain and victim(s).");
	tc.Add("A survival thriller, with intense and suspenseful scenarios and a fight for survival.");
	tc.Add("A legal thriller, with courtroom drama and clever plot twists.");}

	{auto& tc = list.Add("Mystery/Crime");
	tc.Add("A murder mystery, with a complex and twisty plot and a mysterious killer.");
	tc.Add("A heist/caper movie, with clever schemes and intricate plans.");
	tc.Add("A detective/crime procedural, with a dedicated investigator and a riveting case.");
	tc.Add("A spy thriller, with a web of secrets and double-crossing characters.");
	tc.Add("A neo-noir crime drama, with gritty and atmospheric visuals and complex characters.");
	tc.Add("A psychological mystery, with a clever and unpredictable plot and mind-bending twists.");}

	{auto& tc = list.Add("Western");
	tc.Add("A classic western, with sweeping landscapes and a clash between good and evil.");
	tc.Add("A revisionist western, subverting traditional western tropes and featuring complex characters.");
	tc.Add("A western with a diverse and inclusive cast, breaking the mold of traditional cowboy stories.");
	tc.Add("A neo-western, with modern elements and a gritty and realistic portrayal of the old west.");
	tc.Add("A western drama, exploring themes of revenge, redemption, and justice.");
	tc.Add("A western adventure, with rugged cowboys and thrilling shootouts.");}

	{auto& tc = list.Add("War/Epic");
	tc.Add("An epic war drama, with stunning battle scenes and emotional depictions of the effects of war.");
	tc.Add("A military action movie, with intense and explosive battle sequences.");
	tc.Add("A war biopic, telling the incredible true story of a soldier or group of soldiers.");
	tc.Add("A historical epic set during a major war or conflict, with grand and epic battles.");
	tc.Add("A war adventure, with soldiers on a dangerous mission behind enemy lines.");
	tc.Add("A war romance, with a love story set against the backdrop of war and its challenges.");}

	{auto& tc = list.Add("Slice of Life");
	tc.Add("A coming-of-age story about a teenager navigating the ups and downs of high school.");
	tc.Add("A relatable comedy about the everyday struggles of a group of friends or coworkers.");
	tc.Add("A heartwarming drama about family dynamics and relationships.");
	tc.Add("A character study about an ordinary person facing challenges and growth in their life.");
	tc.Add("A dramedy about a person going through a major life change or crisis.");
	tc.Add("A slice-of-life mockumentary about the everyday struggles of a group of quirky characters.");}

	{auto& tc = list.Add("Satire");
	tc.Add("A political satire, poking fun at politicians and their policies.");
	tc.Add("A social satire, critiquing societal norms and expectations.");
	tc.Add("A dark comedy satire, exploring taboo topics and pushing the boundaries of humor.");
	tc.Add("A comedic satire about the entertainment industry or media.");
	tc.Add("A satire of a specific genre or style, parodying its tropes and clichés.");
	tc.Add("A satire of a specific historical event or figure, using humor to make a statement about its impact.");}

	{auto& tc = list.Add("Surrealism");
	tc.Add("A surreal and dreamlike film, blurring the lines between reality and imagination.");
	tc.Add("A satirical surreal comedy, using strange and unconventional elements to make a point.");
	tc.Add("A dark and unsettling surreal horror movie.");
	tc.Add("A surreal drama exploring themes of identity and existence.");
	tc.Add("A hybrid of animation and live-action, creating a whimsical and surreal world.");
	tc.Add("A mind-bending thriller with surreal and thought-provoking twists.");}
	
	ASSERT(GetStyles().GetCount() == list.GetCount());
	return list;
}

VectorMap<String,Vector<String>>& GetStyleSafeFemale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
    
	{auto& tc = list.Add("Intense");
	tc.Add("A thriller movie, with high-stakes and intense action sequences.");
	tc.Add("A drama, with emotionally charged scenes and intense relationships.");
	tc.Add("An adventure film, with heart-pumping action and intense survival scenarios.");
	tc.Add("A heist/crime movie, with tense planning and high-risk situations.");
	tc.Add("A revenge story, with powerful emotions and intense confrontations.");}

	{auto& tc = list.Add("Dark and brooding");
	tc.Add("A psychological thriller, with a dark and haunting atmosphere.");
	tc.Add("A mystery, with complex and brooding characters.");
	tc.Add("A survival story, set in a dark and foreboding environment.");
	tc.Add("A drama, exploring dark and difficult themes.");
	tc.Add("A fantasy or supernatural story, with a brooding and ominous tone.");}

	{auto& tc = list.Add("Whimsical and fantastical");
	tc.Add("A fairy tale adventure, with magical creatures and whimsical landscapes.");
	tc.Add("A rom-com, with lighthearted humor and dreamy romance.");
	tc.Add("A musical, with vibrant and fantastical musical numbers.");
	tc.Add("An animated movie, with charming characters and a whimsical storyline.");
	tc.Add("A fantasy epic, with imaginative worlds and fantastical creatures.");}

	{auto& tc = list.Add("Sci-fi/Futuristic");
	tc.Add("A dystopian world, with advanced technology and a struggle for survival.");
	tc.Add("A space adventure, with futuristic spacecrafts and otherworldly creatures.");
	tc.Add("A futuristic society, exploring themes of technology and its impact on humanity.");
	tc.Add("A time-travel story, with futuristic gadgets and complex timelines.");
	tc.Add("A post-apocalyptic setting, with advanced technology used for survival.");}

	{auto& tc = list.Add("Film noir");
	tc.Add("A crime/mystery movie, with a dark and gritty portrayal of the seedy underbelly of society.");
	tc.Add("A drama, exploring themes of guilt, corruption, and moral ambiguity.");
	tc.Add("A thriller, with a bleak and moody atmosphere and suspenseful plot twists.");
	tc.Add("A period piece, set in the 1940s-1950s with classic film noir elements such as femme fatales and hard-boiled detectives.");
	tc.Add("A neo-noir film, modernizing the classic style with modern settings and characters.");}

	{auto& tc = list.Add("Comedic");
	tc.Add("A romantic comedy, with witty banter and hilarious situations.");
	tc.Add("A comedy of errors, with mistaken identities and outrageous mishaps.");
	tc.Add("An ensemble comedy, with a diverse group of characters and humorous interactions.");
	tc.Add("A parody or spoof, poking fun at popular movies or genres.");
	tc.Add("A coming-of-age comedy, with relatable and awkward teenage experiences.");}

	{auto& tc = list.Add("Romantic");
	tc.Add("A romantic drama, with heartwarming and emotional love stories.");
	tc.Add("A rom-com, with charming and humorous love interests and romantic escapades.");
	tc.Add("A historical romance, set in a specific time period with a passionate and sweeping love story.");
	tc.Add("A fantasy romance, with a mythical or supernatural element adding to the love story.");
	tc.Add("A romantic tragedy, with forbidden love and heart-wrenching sacrifices.");}

	{auto& tc = list.Add("Family-friendly");
	tc.Add("An animated adventure, suitable for all ages with positive messages and lovable characters.");
	tc.Add("A heartwarming family drama, exploring themes of love, forgiveness, and coming together.");
	tc.Add("A fantasy movie, suitable for the whole family with fantastical creatures and magical adventures.");
	tc.Add("A musical, with catchy songs and an uplifting story that appeals to all ages.");
	tc.Add("A comedy, with clean humor and relatable family dynamics.");}

	{auto& tc = list.Add("Adventure/Action");
	tc.Add("An epic adventure, with thrilling action sequences and a quest for treasure or a mythical artifact.");
	tc.Add("An action-thriller, with high-speed chases and intense fighting scenes.");
	tc.Add("A survival story, with characters pushed to their limits in extreme and dangerous conditions.");
	tc.Add("A period piece action movie, set in a specific time period and incorporating historical events into the action-packed plot.");
	tc.Add("A superhero movie, with a female protagonist and her heroic journey to save the world.");}

	{auto& tc = list.Add("Animated");
	tc.Add("A musical, with vibrant and colorful animation and catchy songs.");
	tc.Add("An adventure movie, with lovable animated characters and a grand journey.");
	tc.Add("A fantasy story, with imaginative and fantastical animation.");
	tc.Add("A comedy, with silly and zany animated characters and humorous situations.");
	tc.Add("A family-friendly movie, with heartwarming and relatable animated characters and positive messages.");}

	{auto& tc = list.Add("Period Piece");
	tc.Add("A historical drama, exploring a specific time period with realistic and accurate depictions.");
	tc.Add("A romance, set in a specific time period with alluring costumes and settings.");
	tc.Add("A biographical film, relaying the story of a real woman in history and her impact on the world.");
	tc.Add("A war movie, set in a particular historical era and showcasing the experiences of women during wartime.");
	tc.Add("A fantasy or fairy tale, incorporating elements of a specific time period and reimagining them through a female perspective.");}

	{auto& tc = list.Add("Musical");
	tc.Add("A romantic musical, with love ballads and grand musical numbers.");
	tc.Add("A drama, with emotional and powerful songs that drive the story forward.");
	tc.Add("A jukebox musical, incorporating popular songs into the storyline and showcasing the talent of the female lead.");
	tc.Add("A fantasy or animated musical, using music to enhance the magical or fantastical elements of the story.");
	tc.Add("A Broadway-inspired musical, with elaborate choreography and show-stopping performances.");}

	{auto& tc = list.Add("Dramatic");
	tc.Add("A character-driven drama, delving into the complex emotions and relationships of the main character.");
	tc.Add("A biographical film, showcasing the struggles and triumphs of a real woman in history.");
	tc.Add("A period piece, portraying the societal and personal challenges faced by women in a specific time period.");
	tc.Add("A political drama, tackling relevant political issues and how they impact women's lives.");
	tc.Add("A courtroom drama, following a female lawyer's journey to justice and navigating a male-dominated legal system.");}

	{auto& tc = list.Add("Horror");
	tc.Add("A supernatural horror, with ghostly apparitions and sinister spirits.");
	tc.Add("A psychological horror, delving into the dark and disturbed minds of the characters.");
	tc.Add("A survival horror, with a female lead fighting to survive against terrifying threats.");
	tc.Add("A slasher movie, with fast-paced action and thrilling suspense.");
	tc.Add("A horror-comedy, blending humor with horror for a fun and spooky viewing experience.");}

	{auto& tc = list.Add("Mockumentary");
	tc.Add("A mockumentary-style comedy, with a playful and satirical take on a specific topic or genre.");
	tc.Add("A mockumentary inspired by real-life events, exploring the absurdity and humor in everyday situations.");
	tc.Add("A political mockumentary, satirizing current events and societal issues through a female perspective.");
	tc.Add("A horror mockumentary, using a faux documentary style to add a sense of realism to the scary elements of the story.");
	tc.Add("A mockumentary following the behind-the-scenes antics of a group of female filmmakers, touching on the challenges and successes of being a woman in the industry.");}

	{auto& tc = list.Add("Documentary-style");
	tc.Add("A biographical documentary, showcasing the life and achievements of a real woman in history.");
	tc.Add("A feminist documentary, exploring important issues facing women today and highlighting the voices and experiences of women from diverse backgrounds.");
	tc.Add("A nature or wildlife documentary, showcasing the beauty and power of the natural world and its impact on women's lives.");
	tc.Add("A sports documentary, highlighting the accomplishments and challenges faced by female athletes.");
	tc.Add("A cultural documentary, shedding light on the unique experiences and perspectives of women from different cultures and backgrounds.");}

	{auto& tc = list.Add("Experimental/Avant-garde");
	tc.Add("An abstract art film, using unique and unconventional techniques to explore themes of femininity and identity.");
	tc.Add("An experimental narrative, pushing the boundaries of traditional storytelling with surreal and thought-provoking visuals.");
	tc.Add("An avant-garde film, showcasing unconventional and provocative themes and challenging societal norms and expectations.");
	tc.Add("A performance art piece, combining different mediums such as music, dance, and visuals to tell a thought-provoking story.");
	tc.Add("A virtual reality experience, immersing viewers in a visually stunning and thought-provoking journey through the female perspective.");}

	{auto& tc = list.Add("Thriller/Suspense");
	tc.Add("A psychological thriller, with mind-bending twists and turns that keep viewers on the edge of their seats.");
	tc.Add("A crime thriller, with high-stakes and intense investigations led by a female detective.");
	tc.Add("A domestic thriller, exploring the dark secrets and dangers hiding within seemingly perfect families.");
	tc.Add("A conspiracy thriller, following a female journalist's pursuit of the truth behind a dangerous cover-up.");
	tc.Add("A political thriller, tackling relevant and timely issues and the potentially deadly consequences faced by women trying to uncover the truth.");}

	{auto& tc = list.Add("Mystery/Crime");
	tc.Add("A detective movie, following a female investigator solving a challenging and complex case.");
	tc.Add("A whodunit mystery, with an intriguing and suspenseful plot that keeps viewers guessing until the very end.");
	tc.Add("A crime drama, delving into the lives and motivations of criminals and the women who are determined to take them down.");
	tc.Add("A noir-inspired mystery, filled with intrigue and deception in a dark and gritty setting.");
	tc.Add("A cozy mystery, featuring a strong and relatable female lead solving crimes in a charming small town.");}

	{auto& tc = list.Add("Western");
	tc.Add("A classic Western, with a strong and resilient female protagonist navigating the challenges of the Wild West.");
	tc.Add("A modern Western, exploring contemporary themes and social issues while still maintaining the spirit of the genre.");
	tc.Add("A Western with a feminist twist, highlighting the often overlooked roles and contributions of women in the Wild West.");
	tc.Add("A female-led revenge story, set in the harsh and unforgiving landscape of the Wild West.");
	tc.Add("A Western adventure, following a group of women on a daring journey through the rugged frontier.");}

	{auto& tc = list.Add("War/Epic");
	tc.Add("A historical war movie, portraying the experiences of female soldiers and civilians during a specific war or conflict.");
	tc.Add("A fantasy epic, with strong and capable female warriors fighting for survival in a treacherous and magical world.");
	tc.Add("A modern war movie, shedding light on the physical and emotional toll of war on women.");
	tc.Add("An epic journey, following a group of women on a perilous and heroic quest to save their homeland or their loved ones.");
	tc.Add("A war romance, showcasing the resilience and strength of love amidst the chaos and destruction of war.");}

	{auto& tc = list.Add("Slice of Life");
	tc.Add("A drama, delving into the everyday struggles and triumphs of a relatable female lead.");
	tc.Add("A romantic comedy, following the awkward and endearing moments of a woman navigating modern dating.");
	tc.Add("A coming-of-age story, exploring the challenges and joys of growing up as a young woman in today's society.");
	tc.Add("A family drama, touching on the complexities and dynamics of familial relationships.");
	tc.Add("A workplace comedy or drama, focusing on the experiences of women in a specific industry or job.");}

	{auto& tc = list.Add("Satire' :");
	tc.Add("A political satire, satirizing the absurdities and hypocrisies of gender roles and societal expectations.");
	tc.Add("A parody of a popular film or TV show, providing a humorous and witty commentary on gender stereotypes and tropes.");
	tc.Add("A comedy sketch or series, cleverly mocking the portrayal of women in media and entertainment.");
	tc.Add("A mockumentary, lampooning the stereotypes and misconceptions surrounding women in a certain field or industry.");
	tc.Add("A satirical examination of modern relationships and dating culture.");}

	{auto& tc = list.Add("Surrealism");
	tc.Add("An experimental short film, challenging traditional storytelling and exploring the complexities of female identity and experience.");
	tc.Add("A surreal fantasy, blurring the lines between dreams and reality and showcasing the inner world of a female protagonist.");
	tc.Add("A mind-bending thriller, following a woman on a journey through her own subconscious as she grapples with her desires and fears.");
	tc.Add("An avant-garde exploration of femininity and womanhood, using abstract imagery and symbolism.");
	tc.Add("A magical realism tale, uncovering the hidden struggles and triumphs of everyday women through fantastical elements. ");}

	ASSERT(GetStyles().GetCount() == list.GetCount());
	return list;
}

VectorMap<String,Vector<String>>& GetStyleUnsafeMale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
	
	{auto& tc = list.Add("Intense");
	tc.Add("A gritty and violent action-thriller, featuring a male protagonist on a mission for revenge.");
	tc.Add("A psychological drama, delving into the dark and intense inner thoughts and struggles of a male character.");
	tc.Add("A graphic crime thriller, showcasing brutal and intense scenes of violence.");
	tc.Add("A high-stakes heist story, filled with intense action and suspense as a group of male criminals attempt to pull off a daring robbery.");
	tc.Add("An intense and explicit drama, exploring the complexities of male relationships and the consequences of their actions. ");}

	{auto& tc = list.Add("Dark and brooding");
	tc.Add("A gritty and gritty noir-style detective story, following a male detective as he navigates the corrupt and dangerous underbelly of the city.");
	tc.Add("A psychological horror/thriller, delving into the troubled mind of a male character haunted by nightmares and delusions.");
	tc.Add("A supernatural thriller, centered around a male protagonist who discovers he has dark and dangerous powers.");
	tc.Add("A post-apocalyptic survival story, focused on a male character struggling to survive in a world overrun by violence and despair.");
	tc.Add("A disturbing and twisted portrayal of toxic masculinity, exploring the destructive consequences of male power and privilege.");}

	{auto& tc = list.Add("Whimsical and fantastical");
	tc.Add("A dark and twisted fairy tale, featuring a male hero on a magical quest to save a cursed princess.");
	tc.Add("A dark comedy following the misadventures of a group of male friends who accidentally unleash a powerful and mischievous spirit.");
	tc.Add("A surreal and trippy fantasy adventure, following a male protagonist as he journeys through a fantastical and bizarre world.");
	tc.Add("An R-rated parody of classic fantasy tropes, featuring a male character who must save the kingdom from an evil villain, with plenty of raunchy jokes along the way.");
	tc.Add("A twisted and dark retelling of a classic fairy tale, with unexpected twists and turns and a male anti-hero at the center.");}

	{auto& tc = list.Add("Sci-fi/Futuristic");
	tc.Add("A futuristic action-adventure, following a male hero as he fights against a tyrannical government in a world ruled by technology.");
	tc.Add("A mind-bending sci-fi thriller, following a male character who must unravel the truth about his own existence and the world he lives in.");
	tc.Add("A dark and gritty cyberpunk story, delving into the seedy underworld of a futuristic city and the male characters who inhabit it.");
	tc.Add("A sci-fi horror, featuring a male astronaut who must confront unimaginable horrors on a distant planet.");
	tc.Add("A sexy and provocative sci-fi drama, exploring themes of love, loss, and identity as two male characters find themselves in a world of advanced technology and complicated relationships. ");}

	{auto& tc = list.Add("Film noir");
	tc.Add("A classic neo-noir mystery, featuring a male private detective drawn into a web of deception and murder.");
	tc.Add("A gritty and atmospheric crime drama, following a group of male criminals as they plan and execute a dangerous heist.");
	tc.Add("A hard-boiled detective story, featuring a male detective navigating the corrupt and violent world of organized crime.");
	tc.Add("A dark and twisted tale of betrayal and vengeance, centered around a male protagonist seeking revenge for the death of his lover.");
	tc.Add("A neo-noir anthology following various male characters and their descent into darkness, each story connected by a common thread. ");}

	{auto& tc = list.Add("Comedic");
	tc.Add("A raunchy and irreverent comedy, following the misadventures of a group of male friends as they navigate love, life, and friendship.");
	tc.Add("A dark comedy about a male character who accidentally sells his soul to the devil and must find a way to get it back.");
	tc.Add("An outrageous and outrageous road trip comedy, featuring a group of male buddies on a wild and crazy journey.");
	tc.Add("A satire on toxic masculinity, poking fun at male stereotypes and societal expectations.");
	tc.Add("A black comedy about a dysfunctional family of male characters, featuring plenty of dark humor and twisted scenarios. ");}

	{auto& tc = list.Add("Romantic");
	tc.Add("A steamy and passionate romance, following the tumultuous love affair between two male characters.");
	tc.Add("A dramatic love triangle between three male characters, exploring themes of love, jealousy, and desire.");
	tc.Add("A forbidden romance between two male characters from rival gangs or families, facing challenges and dangers in their pursuit of love.");
	tc.Add("A coming-of-age story about a young male character discovering his sexuality and falling in love for the first time.");
	tc.Add("A sensual and erotic love story between two male characters, exploring the complexities of their relationship and the societal pressures they face. ");}

	{auto& tc = list.Add("Family-friendly");
	tc.Add("A heartwarming adventure story, featuring a male protagonist on a journey to reconnect with his estranged family.");
	tc.Add("A light-hearted comedy about a single father and his relationship with his young son.");
	tc.Add("An animated adventure about a group of male friends on a quest to save their hometown from an evil villain.");
	tc.Add("A heartwarming drama about a male character and his unconditional love for his adopted child.");
	tc.Add("A feel-good sports story about a male athlete and his close relationship with his father as they navigate the challenges of the game.");}

	{auto& tc = list.Add("Adventure/Action");
	tc.Add("An epic and adrenaline-fueled action movie, featuring a male hero on a mission to save the world.");
	tc.Add("A gritty and intense survival story, following a male character stranded on a remote island or in a hostile environment.");
	tc.Add("A high-stakes adventure, filled with daring stunts and dangerous obstacles as a group of male characters journey through uncharted territories.");
	tc.Add("A military action thriller, following a group of male soldiers on a dangerous mission in a war-torn country.");
	tc.Add("A fast-paced and exciting treasure hunt, featuring a male character and his team searching for a valuable and elusive artifact. ");}

	{auto& tc = list.Add("Animated");
	tc.Add("A dark and twisted animated film, following the journey of a male protagonist through a fantastical and surreal world.");
	tc.Add("A raunchy and irreverent adult animated comedy, featuring male characters and their outrageous antics.");
	tc.Add("An animated anthology series, featuring various male characters and their unique and bizarre stories.");
	tc.Add("A dark and provocative anime-style film, exploring themes of violence, sexuality, and identity through its male characters.");
	tc.Add("A coming-of-age animated movie about a young male character exploring his sexuality and relationships. ");}

	{auto& tc = list.Add("Period Piece");
	tc.Add("A lush and romantic drama, set in a past era and following the passionate and tumultuous relationships of male characters.");
	tc.Add("An epic historical adventure, featuring male characters in the midst of a war or uprising.");
	tc.Add("A biographical film about a male historical figure, delving into their personal struggles and accomplishments.");
	tc.Add("A dark and twisted retelling of a classic literary work, featuring male characters in a period setting.");
	tc.Add("A provocative and sensual exploration of the lives and desires of male characters in a past era. ");}

	{auto& tc = list.Add("Musical");
	tc.Add("A raunchy and irreverent musical comedy, featuring male characters and their outrageous musical numbers.");
	tc.Add("A dramatic and emotional journey through the music industry, following a male musician's rise to fame and the challenges they face.");
	tc.Add("A romantic musical about two male characters finding love and expressing their emotions through song and dance.");
	tc.Add("An animated musical about a group of male friends forming a band and striving for success.");
	tc.Add("A provocative and sensual musical exploring themes of love, lust, and betrayal through its male characters.");}

	{auto& tc = list.Add("Dramatic");
	tc.Add("A gripping and emotional drama, featuring a male protagonist facing personal challenges and obstacles.");
	tc.Add("A character study of a male character dealing with addiction, mental illness, or trauma.");
	tc.Add("A tense and intense legal drama, following a male lawyer fighting for justice and facing moral dilemmas.");
	tc.Add("A family drama about a male character struggling to balance his career and family life.");
	tc.Add("A dark and twisted exploration of the complexities and secrets of male relationships. ");}

	{auto& tc = list.Add("Horror");
	tc.Add("A chilling and atmospheric horror film, featuring a male character being stalked by a sinister presence.");
	tc.Add("A supernatural horror centered around a male character who uncovers a dark and dangerous secret about his family.");
	tc.Add("A psychological horror, following a male character as he becomes increasingly paranoid and delusional.");
	tc.Add("A slasher film with a male killer and his victims, featuring intense and graphic violence.");
	tc.Add("A horror anthology featuring various male characters encountering terrifying and otherworldly beings. ");}

	{auto& tc = list.Add("Mockumentary");
	tc.Add("A satirical mockumentary about the absurdities of male-dominated industries or societies.");
	tc.Add("A dark and twisted mockumentary about a male celebrity's fall from grace.");
	tc.Add("A hilarious and irreverent mockumentary following a group of male friends vying for a record deal in the music industry.");
	tc.Add("A dark comedy mockumentary about a group of male actors navigating the cutthroat world of Hollywood.");
	tc.Add("A controversial mockumentary delving into the private lives and scandals of male politicians or public figures.");}

	{auto& tc = list.Add("Documentary-style");
	tc.Add("A gritty and intense documentary-style film exploring the dangerous and illegal activities of male criminals.");
	tc.Add("A biographical documentary about a male historical figure, delving into their personal struggles and accomplishments.");
	tc.Add("A dark and provocative investigation into the world of male strip clubs and the lives of the dancers.");
	tc.Add("A controversial and thought-provoking documentary about the toxic effects of masculinity on society.");
	tc.Add("An exploration of the male psyche through the eyes of various real-life male subjects. ");}

	{auto& tc = list.Add("Experimental/Avant-garde");
	tc.Add("A surreal and visually stunning experiment in storytelling, featuring a male protagonist on a journey of self-discovery.");
	tc.Add("A thought-provoking and controversial avant-garde exploration of toxic masculinity and societal expectations of male behavior.");
	tc.Add("An abstract and dreamlike portrait of male identity and sexuality.");
	tc.Add("A dark and twisted avant-garde film about a male character's descent into madness.");
	tc.Add("A mind-bending and unconventional film about the blurred lines between reality and fantasy, featuring male characters at the center. ");}

	{auto& tc = list.Add("Thriller/Suspense");
	tc.Add("A gripping and intense suspense thriller, featuring a male protagonist trapped in a dangerous and terrifying situation.");
	tc.Add("A twisty and unpredictable psychological thriller, following a male character as he becomes a suspect in a murder investigation.");
	tc.Add("A cat-and-mouse game between a male detective and a cunning and dangerous criminal.");
	tc.Add("A political thriller, featuring male politicians and their manipulative games for power.");
	tc.Add("A dark and suspenseful thriller about a male hostage trying to escape the clutches of his captors. ");}

	{auto& tc = list.Add("Mystery/Crime");
	tc.Add("A twisty and suspenseful mystery, featuring a male detective trying to solve a complex and perplexing case.");
	tc.Add("A gritty and graphic crime drama, exploring the violent and corrupt world of organized crime through its male characters.");
	tc.Add("A chilling and disturbing mystery about a male character who believes he is being haunted by a ghost.");
	tc.Add("A noir-style murder mystery, following a male private investigator as he unravels a web of deceit and betrayal.");
	tc.Add("A tense and atmospheric mystery about a group of male friends on a camping trip, facing a series of deadly and mysterious events. ");}

	{auto& tc = list.Add("Western");
	tc.Add("An epic and action-packed western, featuring a male gunslinger on a quest for revenge.");
	tc.Add("A gritty and realistic depiction of the Wild West and the trials and tribulations of a group of male cowboys.");
	tc.Add("A revisionist western, exploring themes of masculinity and violence in the American West through its male characters.");
	tc.Add("A dark and brooding western, following a group of male outlaws on the run from the law.");
	tc.Add("A character-driven western about a male rancher and his struggles to protect his land and family from outside forces. ");}

	{auto& tc = list.Add("War/Epic");
	tc.Add("An epic war movie, following a group of male soldiers in the midst of a brutal and harrowing battle.");
	tc.Add("A gritty and realistic portrayal of the effects of war on male soldiers, both physically and mentally.");
	tc.Add("A political drama set during a war, featuring male generals and politicians making strategic decisions and facing moral dilemmas.");
	tc.Add("A dark and violent war story about a group of male soldiers on a dangerous mission behind enemy lines");
	tc.Add("An epic story of brotherhood and survival set during a war, following a group of male soldiers as they fight to make it home alive.");}

	{auto& tc = list.Add("Slice of Life");
	tc.Add("A raw and honest portrayal of the struggles and triumphs of a group of male friends living in a big city.");
	tc.Add("A coming-of-age story about a young male trying to navigate his relationships, career, and personal growth.");
	tc.Add("An intimate and sensual story about a male character discovering his own sexuality and desires.");
	tc.Add("A character-driven drama about a male character dealing with the aftermath of a life-changing event.");
	tc.Add("A gritty and realistic look at the daily struggles of blue-collar male workers in a small town.");}

	{auto& tc = list.Add("Satire");
	tc.Add("A satirical comedy about a group of male celebrities and their over-the-top antics and lifestyles.");
	tc.Add("A political satire about a male politician and the ridiculous and corrupt world of politics.");
	tc.Add("A darkly comedic exploration of toxic masculinity and the pressure of being a \"perfect\" man.");
	tc.Add("A satirical take on traditional fairy tales, featuring a male protagonist navigating a world of fairy tale characters and absurd situations.");
	tc.Add("An edgy and irreverent satire about a male writer trying to make it in the cutthroat world of literature.");}

	{auto& tc = list.Add("Surrealism");
	tc.Add("A trippy and mind-bending exploration of the subconscious through the eyes of a male protagonist.");
	tc.Add("A surreal and psychedelic journey through a male character's dream world.");
	tc.Add("An avant-garde and experimental film exploring themes of identity and masculinity through a male character's surreal experiences.");
	tc.Add("A dark and twisted horror story about a male character trapped in a bizarre and surreal alternate reality.");
	tc.Add("A surreal and absurdist comedy following a group of male roommates living in a strange and surreal apartment building.");}

	ASSERT(GetStyles().GetCount() == list.GetCount());
	return list;
}

VectorMap<String,Vector<String>>& GetStyleUnsafeFemale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
	
	{auto& tc = list.Add("Intense");
	tc.Add("A psychological thriller about a female protagonist who becomes obsessed with her mysterious and enigmatic neighbor.");
	tc.Add("A violent and shocking revenge story, following a fierce and determined female character seeking vengeance on those who have wronged her.");
	tc.Add("A female-led survival horror, following a group of women stranded on a remote island and fighting for their lives against a deadly creature.");
	tc.Add("A provocative and intense drama about a female politician navigating a corrupt and cutthroat world.");
	tc.Add("A gritty and adrenaline-fueled action flick, showcasing a skilled and resourceful female protagonist on a mission to take down a powerful drug lord.");}

	{auto& tc = list.Add("Dark and brooding");
	tc.Add("A Gothic romance filled with forbidden love, betrayal, and supernatural elements.");
	tc.Add("A horror tale about a group of female friends who stumble upon a cursed object, leading to terrifying and macabre consequences.");
	tc.Add("A noir-style crime drama, following a femme fatale who uses her charms and cunning to manipulate and outsmart the men in her life.");
	tc.Add("A slow-burning drama about a woman slowly unraveling as she becomes consumed by grief and loss.");
	tc.Add("A dark and twisted fantasy about a group of women with magical abilities who are caught in a power struggle with malevolent forces.");}

	{auto& tc = list.Add("Whimsical and fantastical");
	tc.Add("A whimsical fairytale about a young princess who must rescue her kingdom from an evil sorcerer with the help of her magical animal companions.");
	tc.Add("A magical coming-of-age story about a teenage girl who discovers she is a powerful witch and must learn to control her abilities while navigating the challenges of high school.");
	tc.Add("A colorful and delightful adventure about a group of female misfits on a quest to find a legendary treasure and save their village from destruction.");
	tc.Add("A quirky and surreal comedy about a woman who inherits a talking rabbit and must use its mystical powers to defeat an evil wizard.");
	tc.Add("A heartwarming fantasy about a girl who is transported to a land of mythical creatures and must go on a quest to find her way home.");}

	{auto& tc = list.Add("Sci-fi/Futuristic");
	tc.Add("An epic space opera following a badass female captain and her diverse and capable crew as they embark on dangerous missions across the galaxy.");
	tc.Add("A dystopian thriller about a group of women who must band together to overthrow a tyrannical government that seeks to control and oppress them.");
	tc.Add("A cyberpunk adventure following a skilled hacker and her team as they navigate a futuristic world filled with advanced technology and corrupt corporations.");
	tc.Add("A mind-bending sci-fi drama about a woman who discovers she has the ability to time travel, but must face the consequences of changing the past.");
	tc.Add("A post-apocalyptic survival story about a group of women who must rebuild society in a world devastated by a catastrophic event.");}

	{auto& tc = list.Add("Film noir");
	tc.Add("A sultry and seductive crime drama about a detective on the brink of retirement who gets caught up in a dangerous affair with a mysterious woman involved in a high-stakes heist.");
	tc.Add("A gritty and suspenseful thriller about a female journalist investigating a string of murders and facing off against a corrupt and powerful figure.");
	tc.Add("A stylish and twisted neo-noir about a femme fatale who manipulates her way into the inner circle of a wealthy and powerful family.");
	tc.Add("A moody and atmospheric detective story about a female private eye who must navigate the treacherous world of organized crime and corrupt officials.");
	tc.Add("A psychological thriller about a woman who becomes entangled in a dangerous love triangle with two men who may not be who they seem.");}

	{auto& tc = list.Add("Comedic");
	tc.Add("A raunchy and outrageous comedy about a group of girlfriends who go on a wild bachelorette weekend and end up getting into all sorts of hilarious shenanigans.");
	tc.Add("A buddy comedy about two women, one a no-nonsense detective and the other a quirky psychic, who team up to solve a case and become unlikely friends in the process.");
	tc.Add("A satirical workplace comedy about a group of ambitious and cutthroat women trying to climb the corporate ladder at a male-dominated company.");
	tc.Add("A silly and silly road trip comedy about two estranged sisters who must travel together to spread their grandmother's ashes and reconnect along the way.");
	tc.Add("A romantic comedy about two best friends who make a pact to get married if they are both still single by a certain age, and then unexpectedly find themselves falling for each other.");}

	{auto& tc = list.Add("Romantic");
	tc.Add("A sweeping historical romance about forbidden love between a noblewoman and a commoner in a time of war and political intrigue.");
	tc.Add("A modern day rom-com about a successful career woman who hires a male escort to pose as her boyfriend for a family wedding, only to find real feelings developing.");
	tc.Add("A steamy and passionate love story between two women who must navigate societal expectations and family disapproval in order to be together.");
	tc.Add("A romantic drama about a woman who falls in love with a mysterious and troubled stranger, only to discover he has a dangerous secret that threatens to tear them apart.");
	tc.Add("A charming and heartwarming tale about a woman who inherits a rundown cottage and hires a handsome carpenter to help repair it, leading to sparks and unexpected romance.");}

	{auto& tc = list.Add("Family-friendly");
	tc.Add("A heartwarming animated adventure about a young girl and her animal friends going on a quest to save their magical kingdom from an evil sorcerer.");
	tc.Add("A playful and wholesome comedy about a group of mothers who form a support group and go on fun and wacky outings together.");
	tc.Add("A heartwarming drama about a single mother trying to make a fresh start with her rebellious teenage daughter in a small town.");
	tc.Add("A fantasy adventure about a family of witches on a mission to stop an ancient curse from destroying their town and their magical way of life.");
	tc.Add("A heartwarming story about a girl who befriends a lonely mermaid and must help her return to her underwater kingdom before it's too late.");}

	{auto& tc = list.Add("Adventure/Action");
	tc.Add("A high-stakes spy thriller about a skilled and fearless female agent on a mission to stop a terrorist plot.");
	tc.Add("A swashbuckling pirate adventure about a clever and resourceful female captain and her crew as they navigate treacherous waters and rival pirates.");
	tc.Add("A survival drama about a group of women stranded in the wilderness after a plane crash and struggling to find a way back to civilization.");
	tc.Add("A post-apocalyptic action story about a tough and determined woman leading a band of survivors against ruthless and dangerous enemies.");
	tc.Add("A thrilling heist film about a group of female thieves planning and executing an elaborate and daring robbery.");}

	{auto& tc = list.Add("Animated");
	tc.Add("A magical and heartwarming tale about a girl who discovers she has the power to bring her drawings to life and embarks on a journey to save her family and friends from a dark force.");
	tc.Add("A sweeping fantasy epic about a young princess and her animal companions fighting against a tyrannical ruler to reclaim her rightful place as queen.");
	tc.Add("A musical adventure about a girl and her talking animal sidekick as they embark on a journey to find a legendary treasure and save their kingdom.");
	tc.Add("A high-energy and hilarious comedy about a group of female cartoon characters trying to escape their creator's outdated and sexist storylines.");
	tc.Add("A heartwarming coming-of-age story about a girl and her loyal dragon companion as they navigate through challenges and friendships in a magical world.");}

	{auto& tc = list.Add("Period Piece");
	tc.Add("A lavish and romantic drama set in the early 20th century about a wealthy woman torn between societal expectations and her own desires.");
	tc.Add("A historical adventure about a group of women who become highway robbers in order to survive during a time of war and famine.");
	tc.Add("A political thriller set in ancient Rome about a powerful and cunning woman using her charm and intelligence to rise to the top.");
	tc.Add("A drama about a rebellious young woman in Victorian England who finds herself at odds with society and her traditional family.");
	tc.Add("A sweeping saga about the lives and loves of a group of women from different backgrounds during the French Revolution.");}

	{auto& tc = list.Add("Musical");
	tc.Add("A modern day musical about a struggling singer who must navigate the cutthroat world of the music industry and find love along the way.");
	tc.Add("A romantic comedy about two rival a cappella groups and the unexpected romance that blossoms between their leaders.");
	tc.Add("A rock opera about a group of women in a punk band fighting against societal expectations and gender stereotypes in the 1970s.");
	tc.Add("A vibrant and colorful fantasy musical about a girl who discovers she is a princess and must use her magical powers to save her kingdom from an evil sorcerer.");
	tc.Add("A musical biopic about a legendary female musician and her rise to fame, highlighting the struggles and triumphs of her personal and professional life.");}

	{auto& tc = list.Add("Dramatic");
	tc.Add("A slow-burning and poignant drama about a woman battling drug addiction and trying to reconnect with her estranged family.");
	tc.Add("A powerful and emotional story about a female lawyer fighting for justice and facing backlash in a male-dominated legal system.");
	tc.Add("A heart-wrenching drama about a woman coming to terms with her terminal illness and making the most of her remaining time.");
	tc.Add("A tense and gripping thriller about a woman on the run from her abusive husband and fighting to protect her children.");
	tc.Add("A tragic and thought-provoking tale about a group of female soldiers dealing with the aftermath of war and the toll it takes on their relationships and mental health.");}

	{auto& tc = list.Add("Horror");
	tc.Add("A modern retelling of a classic tale about a woman cursed by a vengeful spirit and fighting to break the curse before it's too late.");
	tc.Add("A psychological horror about a woman who is haunted by her past and must confront her darkest secrets in order to survive.");
	tc.Add("A found footage style horror about a group of female friends who venture into an abandoned asylum and encounter terrifying supernatural forces.");
	tc.Add("A body horror about a woman undergoing a dangerous and experimental beauty treatment that leads to horrifying consequences.");
	tc.Add("A slasher film about a group of women on a camping trip who must fight for their lives against a masked killer in the woods.");}

	{auto& tc = list.Add("Mockumentary");
	tc.Add("A darkly comedic mockumentary about a group of feminist activists trying to infiltrate and expose a misogynistic cult.");
	tc.Add("A hilarious mockumentary about a group of women competing in a baking competition with high stakes and intense drama.");
	tc.Add("A satire of the fashion industry, following an eccentric and egotistical designer and his team of frenzied assistants as they prepare for a high-profile fashion show.");
	tc.Add("A mockumentary about a group of women trying to become the first all-female team to climb Mount Everest, with chaotic and humorous results.");
	tc.Add("A wacky and over-the-top mockumentary about a group of women running a dog grooming business and the hijinks that ensue.");}

	{auto& tc = list.Add("Documentary-style");
	tc.Add("A gripping and emotional documentary about a group of female activists fighting for women's rights in a repressive and conservative society.");
	tc.Add("A raw and honest documentary about the struggles and triumphs of a group of women trying to break into the male-dominated world of pro wrestling.");
	tc.Add("A true crime documentary about a notorious female serial killer and the events that led to her capture.");
	tc.Add("A poignant and thought-provoking documentary about the experiences of female soldiers in combat and the challenges they face when returning to civilian life.");
	tc.Add("A behind-the-scenes look at the making of an all-female action film, highlighting the hard work and dedication of the cast and crew.");}

	{auto& tc = list.Add("Experimental/Avant-garde");
	tc.Add("An abstract and surreal experimental film exploring the concept of female identity and societal expectations.");
	tc.Add("A performance art piece depicting the struggles and triumphs of a woman navigating the corporate world.");
	tc.Add("A visually stunning avant-garde film about a group of women stranded on a deserted island, grappling with isolation and survival.");
	tc.Add("A thought-provoking experimental film about a woman's journey through grief and loss, using metaphors and symbolism to convey her emotions.");
	tc.Add("A bold and daring exploration of female sexuality and desire through avant-garde dance and movement.");}

	{auto& tc = list.Add("Thriller/Suspense");
	tc.Add("A tense and gripping thriller about a woman who suspects her husband of cheating and sets out to uncover the truth.");
	tc.Add("A psychological suspense about a woman who starts receiving mysterious messages from her supposedly deceased twin sister.");
	tc.Add("A neo-noir thriller about a female detective investigating a string of murders linked to a powerful and corrupt pharmaceutical company.");
	tc.Add("A fast-paced and adrenaline-filled thriller about a woman who wakes up with no memory and must piece together the events of the previous night to clear her name.");
	tc.Add("An intense and claustrophobic thriller about a woman trapped on a malfunctioning elevator with a dangerous and unpredictable stranger.");}

	{auto& tc = list.Add("Mystery/Crime");
	tc.Add("A gripping mystery involving a group of female detectives trying to solve a string of brutal murders targeting women in their community.");
	tc.Add("A twisty and unpredictable crime drama about a woman who may or may not be guilty of a heinous crime, and the lawyer fighting to prove her innocence.");
	tc.Add("A neo-noir crime story about a female journalist investigating the disappearance of a wealthy socialite, leading her down a dark and dangerous path.");
	tc.Add("A thrilling and suspenseful murder mystery about a group of friends on a weekend getaway, discovering secrets and betrayals that could ultimately lead to murder.");
	tc.Add("A gritty and realistic crime drama about a female detective determined to take down a notorious drug lord, while facing sexism and corruption within the police force.");}


	{auto& tc = list.Add("Western");
	tc.Add("A female gunslinger seeking revenge on the men who killed her family and took her land in the Old West.");
	tc.Add("A sexy and daring love story between a female outlaw and a tough cowboy, set against the backdrop of the Wild West.");
	tc.Add("A feminist re-imagining of a classic Western, following a gang of female outlaws robbing banks and causing chaos in a male-dominated society.");
	tc.Add("A gritty and violent exploration of the hardships faced by women during the Gold Rush era, as a group of female pioneers fight for survival in a brutal and lawless wilderness.");
	tc.Add("A provocative and gritty tale of female cowboys, brothels, and shootouts in a small Western town run by powerful and corrupt men.");
	}

	{auto& tc = list.Add("War/Epic");
	tc.Add("A powerful and emotional war drama about a group of female nurses on the front lines during World War II.");
	tc.Add("A sprawling epic about a female warrior leading her people to victory against an oppressive and tyrannical empire.");
	tc.Add("A gritty and intense war film about a group of female soldiers fighting for their lives in the midst of a brutal and chaotic conflict.");
	tc.Add("A gripping war thriller about a female spy navigating dangerous territories and risking her life to gather crucial information for the Allied Forces.");
	tc.Add("An epic fantasy war film about a fierce and determined queen leading her army against a malevolent sorcerer threatening her kingdom.");}

	{auto& tc = list.Add("Slice of Life");
	tc.Add("A heartwarming and relatable slice of life film about a group of women navigating the challenges of motherhood and friendship.");
	tc.Add("A bittersweet and poignant story about a woman in her golden years, reflecting on her life, loves, and regrets.");
	tc.Add("A raw and honest portrayal of a woman struggling with addiction and trying to rebuild her life after hitting rock bottom.");
	tc.Add("A coming-of-age story about a group of young women in a small town, facing societal pressures and expectations as they try to find their place in the world.");
	tc.Add("A charming and light-hearted film about a group of female friends spending a summer at a beach house, reliving their carefree college days.");}

	{auto& tc = list.Add("Satire");
	tc.Add("A sharp and witty satire about a group of women in high society, who must confront their shallow and materialistic lifestyles.");
	tc.Add("A political satire following a female candidate's campaign for president, highlighting the absurdities and hypocrisy of the political system.");
	tc.Add("A subversive and darkly comedic take on the romantic comedy genre, featuring a female protagonist who actively tries to sabotage her own love life.");
	tc.Add("A satirical mockumentary about a group of women opening a feminist bookstore in a conservative town and dealing with backlash and resistance.");
	tc.Add("A humorous and biting satire about a group of women working in the tech industry, navigating sexism and discrimination in a male-dominated field.");}

	{auto& tc = list.Add("Surrealism");
	tc.Add("A visually stunning and dreamlike exploration of a woman's psyche and inner turmoil as she struggles with mental illness.");
	tc.Add("An emotionally charged and surreal journey of a woman trying to come to terms with her past trauma through her art.");
	tc.Add("A dark and twisted surrealist film following a group of women who become trapped in a never-ending cycle of their own nightmares.");
	tc.Add("A surrealist horror about a woman who starts to question if the strange and unsettling events happening around her are just figments of her imagination.");
	tc.Add("An experimental and surreal film about a woman who wakes up in a surreal and otherworldly landscape, trying to find her way back home.");}

	ASSERT(GetStyles().GetCount() == list.GetCount());
	return list;
}

VectorMap<String,Vector<String>>& GetStyleSafe(bool gender) {
	if (!gender)
		return GetStyleSafeMale();
	else
		return GetStyleSafeFemale();
}

VectorMap<String,Vector<String>>& GetStyleUnsafe(bool gender) {
	if (!gender)
		return GetStyleUnsafeMale();
	else
		return GetStyleUnsafeFemale();
}

VectorMap<String,Vector<String>>& GetStyleApproaches(bool unsafe, bool gender) {
	if (!unsafe)
		return GetStyleSafe(gender);
	else
		return GetStyleUnsafe(gender);
}














const Index<String>& GetPersuasiveElements() {
	static Index<String> list;
	if (list.IsEmpty()) {
		list.Add("Emotion");
		list.Add("Empathy");
		list.Add("Logos");
		list.Add("Ethos");
		list.Add("Narrative");
		list.Add("Anecdotes");
		list.Add("Storytelling");
		list.Add("Passion");
		list.Add("Imagery");
		list.Add("Urgency");
		list.Add("Authority");
		list.Add("Credibility");
		list.Add("Satisfaction");
		list.Add("Word Play");
		list.Add("Metaphors");
		list.Add("Humor");
		list.Add("Exaggeration");
		list.Add("(Maximization of) Benefits");
		list.Add("Facts");
		list.Add("Evidence");
		list.Add("Desire");
		list.Add("Contrast");
		list.Add("Experience");
		list.Add("Cause & Effect empathy");
	}
	return list;
}

const Vector<ContentType>& GetCallToActions() {
	thread_local static Vector<ContentType> list;
	
	if (list.IsEmpty()) {
		list.Add().Set("Invest", "We want you!", "Making your money go further", "The Finer Details of Investing", "Investment Made Simple");
		list.Add().Set("Grow", "Take your business to the next level", "Leadership", "Strategy", "Make your mark");
		list.Add().Set("Advertise", "Synthesizing Impactful Ideas", "Persuasion Millions Will See", "Expansive Your Traffic", "Corner Your Market");
		list.Add().Set("Share Feedback", "Say what you think, and share your opinions", "Casting Your Stones at the Throne", "Praise like you mean it", "Comment Rebirth");
		list.Add().Set("Discuss", "Let's talk!", "Heralding Your Opinion", "Not afraid to say what matters", "The True You");
		list.Add().Set("View Ads", "Products galore", "We Anything", "Rock the stuff", "Admirers Revolutionized");
		list.Add().Set("Refer/Share", "The more, the merrier", "Earn While Spreading", "Rewards for being a great friend", "Spread the Pereece love!");
		list.Add().Set("Get a Freebie", "Too Good to Pass Up", "Value is !== value paid.", "Freeness to Landing Page", "Relish the Freebie Venue");
		list.Add().Set("Remind Me!", "I Need a Nudge", "I'll Do Better Business", "We'll Call When A Better Time", "Bringing It Together");
		list.Add().Set("Book an appointment", "Enter Our Schedule", "Establish Meeting Grounds", "Hhornwinning Style & Billboard Actions", "3 Things We Want");
		list.Add().Set("Order Books", "Order More Tack", "Premier Quality Only", "Seeing You Success", "The Bookmarket Family");
		list.Add().Set("Download Content", "Re-Send to You KompiK", "Illustrious Download Vendors", "Most Popular Font", "Ideal Contentized");
		list.Add().Set("Unleash Your Creativity", "Let Your Fingers Do the Talking", "Writing And Advertising Deals", "Our #1 Abacabaca Village", "Crank Up Creative Mojo");
		list.Add().Set("Reserve Your Space", "World Extended", "Keeping It Your Way!", "Perormers Edge Bravo", "A Voice in the Audience Called");
		list.Add().Set("Sign Up Now!", "Right Now", "We Do Ensure Raw", "A Deaf Name", "The Quality Life for You");
		list.Add().Set("Subscribe", "Down, But We Will Be Heard", "Fates Join", "The Consummate Ideationist", "Subscribed For DoN Certification");
		list.Add().Set("Order Anything", "Great Deals On Anything", "We Build Repercussions", "Purchase Anything and Ill Goto Child Services", "Order Our Rental On PriPipes");
		list.Add().Set("Get Our Monthly Tips Email", "Pristine Tips", "Multiply Your Audience", "IFTTT Code", "Receive Your Ideas Bonus");
		list.Add().Set("Get a {{COMPANY}} Reading List", "What Books Many Should Read", "Without Quickreview", "Activate Ebook Deals", "Skip the Market To That Extend");
		list.Add().Set("Check Out The {{COMPANY}} Webstore", "Enjoy Our Great IT Deals", "Here to Stay", "The Price You Pay On Any Corner Netwarden", "Beauty Starts Here, Beauty Extended.");
	}
	return list;
}

int GetGetCallToActionCount() {
	return GetCallToActions().GetCount();
}


VectorMap<String,Vector<String>>& GetTriggerSafeMale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
	
	{auto& tc = list.Add("Invest");
    tc.Add("Warren Buffett");
    tc.Add("Elon Musk");
    tc.Add("Tony Robbins");
    tc.Add("Robert Kiyosaki");
    tc.Add("Tim Ferriss");}

    {auto& tc = list.Add("Grow");
    tc.Add("Richard Branson");
    tc.Add("Jeff Bezos");
    tc.Add("Steve Jobs");
    tc.Add("Mark Cuban");
    tc.Add("Bill Gates");}

    {auto& tc = list.Add("Advertise");
    tc.Add("Don Draper");
    tc.Add("David Ogilvy");
    tc.Add("Gary Vaynerchuk");
    tc.Add("Neil Patel");
    tc.Add("Seth Godin");}

    {auto& tc = list.Add("Share Feedback");
    tc.Add("Simon Sinek");
    tc.Add("Malcolm Gladwell");
    tc.Add("Daniel Pink");
    tc.Add("Guy Kawasaki");
    tc.Add("Tom Peters");}

    {auto& tc = list.Add("Discuss");
    tc.Add("Tim Draper");
    tc.Add("Tim Cook");
    tc.Add("Tim Berners-Lee");
    tc.Add("Tim Armstrong");
    tc.Add("Tim O'Reilly");}

    {auto& tc = list.Add("View Ads");
    tc.Add("David Beckham");
    tc.Add("Michael Jordan");
    tc.Add("Cristiano Ronaldo");
    tc.Add("Lionel Messi");
    tc.Add("LeBron James");}

    {auto& tc = list.Add("Refer/Share");
    tc.Add("Gary Keller");
    tc.Add("Grant Cardone");
    tc.Add("Brian Tracy");
    tc.Add("Jay Abraham");
    tc.Add("Zig Ziglar");}

    {auto& tc = list.Add("Get a Freebie");
    tc.Add("Oprah Winfrey");
    tc.Add("Ellen DeGeneres");
    tc.Add("David Copperfield");
    tc.Add("Jimmy Fallon");
    tc.Add("Tony Hawk");}

    {auto& tc = list.Add("Remind Me!");
    tc.Add("Tim Horton");
    tc.Add("Tim Gunn");
    tc.Add("Tim Conway");
    tc.Add("Tim Meadows");
    tc.Add("Tim Allen");}

    {auto& tc = list.Add("Book an appointment");
    tc.Add("Tony Hawk");
    tc.Add("Tim Ferriss");
    tc.Add("Tim Robbins");
    tc.Add("Tim Berners-Lee");
    tc.Add("Tim O'Reilly");}

    {auto& tc = list.Add("Order Books");
    tc.Add("Stephen King");
    tc.Add("James Patterson");
    tc.Add("J.K. Rowling");
    tc.Add("John Grisham");
    tc.Add("Dan Brown");}

    {auto& tc = list.Add("Download Content");
    tc.Add("Elon Musk");
    tc.Add("Tim Ferriss");
    tc.Add("Bill Gates");
    tc.Add("Jeff Bezos");
    tc.Add("Peter Thiel");}

    {auto& tc = list.Add("Unleash Your Creativity");
    tc.Add("Sir Ken Robinson");
    tc.Add("Austin Kleon");
    tc.Add("Elizabeth Gilbert");
    tc.Add("Seth Godin");
    tc.Add("Tom Kelley");}

    {auto& tc = list.Add("Reserve Your Space");
    tc.Add("Gary Vaynerchuk");
    tc.Add("Tony Robbins");
    tc.Add("Richard Branson");
    tc.Add("Tim Draper");
    tc.Add("Tim Armstrong");}

    {auto& tc = list.Add("Sign Up Now!");
    tc.Add("Robert Kiyosaki");
    tc.Add("T. Harv Eker");
    tc.Add("Brian Tracy");
    tc.Add("Darren Hardy");
    tc.Add("Jack Canfield");}

    {auto& tc = list.Add("Subscribe");
    tc.Add("Tim Ferriss");
    tc.Add("Gretchen Rubin");
    tc.Add("Lewis Howes");
    tc.Add("James Altucher");
    tc.Add("Chris Guillebeau");}

    {auto& tc = list.Add("Order Anything");
    tc.Add("Elon Musk");
    tc.Add("Jeff Bezos");
    tc.Add("Tony Robbins");
    tc.Add("Gary Vaynerchuk");
    tc.Add("Seth Godin");}

    {auto& tc = list.Add("Get Our Monthly Tips Email");
    tc.Add("Tim Ferriss");
    tc.Add("Ramit Sethi");
    tc.Add("Tim Harford");
    tc.Add("Malcolm Gladwell");
    tc.Add("Daniel Pink");}

    {auto& tc = list.Add("Get a {{COMPANY}} Reading List");
    tc.Add("Jack Welch");
    tc.Add("Michael Porter");
    tc.Add("Jim Collins");
    tc.Add("Thomas Friedman");
    tc.Add("Warren Buffett");}

    {auto& tc = list.Add("Check Out The {{COMPANY}} Webstore");
    tc.Add("Larry Page");
    tc.Add("Sergey Brin");
    tc.Add("Steve Jobs");
    tc.Add("Soichiro Honda");
    tc.Add("Phil Knight");}
	
	return list;
}

VectorMap<String,Vector<String>>& GetTriggerSafeFemale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
	
	{auto& tc = list.Add("Invest");
    tc.Add("Oprah Winfrey");
    tc.Add("Sheryl Sandberg");
    tc.Add("Arianna Huffington");
    tc.Add("Melinda Gates");
    tc.Add("Bethenny Frankel");}

    {auto& tc = list.Add("Grow");
    tc.Add("Indra Nooyi");
    tc.Add("Beyoncé");
    tc.Add("Michelle Obama");
    tc.Add("Marissa Mayer");
    tc.Add("Elena Cardone");}

    {auto& tc = list.Add("Advertise");
    tc.Add("Kim Kardashian");
    tc.Add("Ellen DeGeneres");
    tc.Add("Kylie Jenner");
    tc.Add("Sofia Vergara");
    tc.Add("Tyra Banks");}

    {auto& tc = list.Add("Share Feedback");
    tc.Add("Beyoncé");
    tc.Add("Jennifer Lopez");
    tc.Add("Taylor Swift");
    tc.Add("Angelina Jolie");
    tc.Add("Lady Gaga");}

    {auto& tc = list.Add("Discuss");
    tc.Add("Amy Schumer");
    tc.Add("Mindy Kaling");
    tc.Add("Chelsea Handler");
    tc.Add("Tiffany Haddish");
    tc.Add("Issa Rae");}

    {auto& tc = list.Add("View Ads");
    tc.Add("P!nk");
    tc.Add("Jennifer Aniston");
    tc.Add("Blake Lively");
    tc.Add("Jennifer Lawrence");
    tc.Add("Jennifer Garner");}

    {auto& tc = list.Add("Refer/Share");
    tc.Add("Karlie Kloss");
    tc.Add("Gigi Hadid");
    tc.Add("Chrissy Teigen");
    tc.Add("Kate Hudson");
    tc.Add("Reese Witherspoon");}

    {auto& tc = list.Add("Get a Freebie");
    tc.Add("Katy Perry");
    tc.Add("Rihanna");
    tc.Add("Selena Gomez");
    tc.Add("Demi Lovato");
    tc.Add("Hailey Bieber");}

    {auto& tc = list.Add("Remind Me!");
    tc.Add("Sofia Vergara");
    tc.Add("Salma Hayek");
    tc.Add("Eva Longoria");
    tc.Add("Rita Moreno");
    tc.Add("Jennifer Lopez");}

    {auto& tc = list.Add("Book an appointment");
    tc.Add("Sofia Vergara");
    tc.Add("Priyanka Chopra");
    tc.Add("Eva Mendes");
    tc.Add("Sofia Carson");
    tc.Add("Jennifer Lopez");}

    {auto& tc = list.Add("Order Books");
    tc.Add("Emma Watson");
    tc.Add("Meryl Streep");
    tc.Add("Oprah Winfrey");
    tc.Add("Reese Witherspoon");
    tc.Add("Ellen DeGeneres");}

    {auto& tc = list.Add("Download Content");
    tc.Add("Ariana Grande");
    tc.Add("Hailee Steinfeld");
    tc.Add("Anne Hathaway");
    tc.Add("Selena Gomez");
    tc.Add("Zendaya");}

    {auto& tc = list.Add("Unleash Your Creativity");
    tc.Add("Mindy Kaling");
    tc.Add("Tina Fey");
    tc.Add("Amy Poehler");
    tc.Add("Melissa McCarthy");
    tc.Add("Kristen Wiig");}

    {auto& tc = list.Add("Reserve Your Space");
    tc.Add("Beyoncé");
    tc.Add("Rihanna");
    tc.Add("Lady Gaga");
    tc.Add("Madonna");
    tc.Add("Demi Lovato");}

    {auto& tc = list.Add("Sign Up Now!");
    tc.Add("Jennifer Lawrence");
    tc.Add("Sandra Bullock");
    tc.Add("Kate Winslet");
    tc.Add("Nicole Kidman");
    tc.Add("Meryl Streep");}

    {auto& tc = list.Add("Subscribe");
    tc.Add("Ellen DeGeneres");
    tc.Add("Tyra Banks");
    tc.Add("Heidi Klum");
    tc.Add("Chrissy Teigen");
    tc.Add("Kendall Jenner");}

    {auto& tc = list.Add("Order Anything");
    tc.Add("Gwyneth Paltrow");
    tc.Add("Jessica Alba");
    tc.Add("Blake Lively");
    tc.Add("Kate Hudson");
    tc.Add("Drew Barrymore");}

    {auto& tc = list.Add("Get Our Monthly Tips Email");
    tc.Add("Gisele Bündchen");
    tc.Add("Serena Williams");
    tc.Add("Alex Morgan");
    tc.Add("Simone Biles");
    tc.Add("Megan Rapinoe");}

    {auto& tc = list.Add("Check Out The {{COMPANY}} Webstore");
    tc.Add("Kim Kardashian");
    tc.Add("Beyoncé");
    tc.Add("Kendall Jenner");
    tc.Add("Sofia Vergara");
    tc.Add("Rihanna");}
	
	return list;
}

VectorMap<String,Vector<String>>& GetTriggerUnsafeMale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
	
	{auto& tc = list.Add("Invest");
    tc.Add("Jordan Belfort");
    tc.Add("Bernard Madoff");
    tc.Add("Elon Musk");
    tc.Add("Kevin O'Leary (aka Mr. Wonderful)");
    tc.Add("Martin Shkreli");}

    {auto& tc = list.Add("Grow");
    tc.Add("Grant Cardone");
    tc.Add("Gary Vaynerchuk");
    tc.Add("Tony Robbins");
    tc.Add("Robert Kiyosaki");
    tc.Add("Tim Ferriss");}

    {auto& tc = list.Add("Advertise");
    tc.Add("Donald Trump");
    tc.Add("Tom Cruise (in his portrayal of Jerry Maguire)");
    tc.Add("Russell Brand");
    tc.Add("Billy Mays");
    tc.Add("Billy McFarland");}

    {auto& tc = list.Add("Share Feedback");
    tc.Add("Gordon Ramsay");
    tc.Add("Simon Cowell");
    tc.Add("Steve Jobs");
    tc.Add("Piers Morgan");
    tc.Add("Simon Sinek");}

    {auto& tc = list.Add("Discuss");
    tc.Add("Howard Stern");
    tc.Add("Joe Rogan");
    tc.Add("Tucker Carlson");
    tc.Add("Bill O'Reilly");
    tc.Add("Glenn Beck");}

    {auto& tc = list.Add("View Ads");
    tc.Add("Al Pacino (in his portrayal of Ricky Roma in \"Glengarry Glen Ross\")");
    tc.Add("Matthew McConaughey (in his portrayal of Mark Hanna in \"The Wolf of Wall Street\")");
    tc.Add("Christian Bale (in his portrayal of Patrick Bateman in \"American Psycho\")");
    tc.Add("Bradley Cooper (in his portrayal of Eddie Morra in \"Limitless\")");
    tc.Add("Ben Affleck (in his portrayal of Charles Redford in \"Boiler Room\")");}

    {auto& tc = list.Add("Refer/Share");
    tc.Add("Nathan Fielder");
    tc.Add("Steve Carell (in his role as Michael Scott in \"The Office\")");
    tc.Add("Andy Samberg (in his role as Jake Peralta in \"Brooklyn Nine-Nine\")");
    tc.Add("Sacha Baron Cohen (in his role as Borat)");
    tc.Add("Larry David (in his role as himself in \"Curb Your Enthusiasm\")");}

    {auto& tc = list.Add("Get a Freebie");
    tc.Add("Charlie Sheen");
    tc.Add("Shia LaBeouf");
    tc.Add("Johnny Depp");
    tc.Add("Kanye West");
    tc.Add("Justin Bieber");}

    {auto& tc = list.Add("Remind Me!");
    tc.Add("Jason Statham");
    tc.Add("Liam Neeson");
    tc.Add("Vin Diesel");
    tc.Add("Sylvester Stallone");
    tc.Add("Bruce Willis");}

    {auto& tc = list.Add("Book an appointment");
    tc.Add("Jeremy Piven (as Ari Gold in \"Entourage\")");
    tc.Add("Alec Baldwin (as Jack Donaghy in \"30 Rock\")");
    tc.Add("Jeffrey Tambor (as George Bluth Sr. in \"Arrested Development\")");
    tc.Add("Kevin Spacey (as Frank Underwood in \"House of Cards\")");
    tc.Add("Jason Bateman (as Michael Bluth in \"Arrested Development\")");}

    {auto& tc = list.Add("Order Books");
    tc.Add("Anthony Hopkins (as Hannibal Lecter in \"The Silence of the Lambs\")");
    tc.Add("Ian McKellen (as Magneto in the \"X-Men\" franchise)");
    tc.Add("Ben Kingsley (as Don Logan in \"Sexy Beast\")");
    tc.Add("Christoph Waltz (as Hans Landa in \"Inglourious Basterds\")");
    tc.Add("Ralph Fiennes (as Voldemort in the \"Harry Potter\" franchise)");}

    {auto& tc = list.Add("Download Content");
    tc.Add("Edward Norton (as Tyler Durden in \"Fight Club\")");
    tc.Add("Leonardo DiCaprio (as Jordan Belfort in \"The Wolf of Wall Street\")");
    tc.Add("Christian Bale (as Bruce Wayne in \"The Dark Knight\")");
    tc.Add("Tom Hardy (as Bane in \"The Dark Knight Rises\")");
    tc.Add("Robert Downey Jr. (as Tony Stark in the Marvel Cinematic Universe)");}

    {auto& tc = list.Add("Unleash Your Creativity");
    tc.Add("Jim Carrey");
    tc.Add("Will Ferrell");
    tc.Add("Robin Williams");
    tc.Add("Jerry Seinfeld");
    tc.Add("Chris Rock");}

    {auto& tc = list.Add("Reserve Your Space");
    tc.Add("Samuel L. Jackson (as Jules Winnfield in \"Pulp Fiction\")");
    tc.Add("Denzel Washington (as Frank Lucas in \"American Gangster\")");
    tc.Add("Al Pacino (as Tony Montana in \"Scarface\")");
    tc.Add("Ray Liotta (as Henry Hill in \"Goodfellas\")");
    tc.Add("Johnny Depp (as James \"Whitey\" Bulger in \"Black Mass\")");}

    {auto& tc = list.Add("Sign Up Now!");
    tc.Add("Will Smith (as Chris Gardner in \"The Pursuit of Happyness\")");
    tc.Add("Tom Hanks (as Chuck Noland in \"Cast Away\")");
    tc.Add("Jake Gyllenhaal (as Louis Bloom in \"Nightcrawler\")");
    tc.Add("Brad Pitt (as Billy Beane in \"Moneyball\")");
    tc.Add("Ben Affleck (as Nick Dunne in \"Gone Girl\")");}

    {auto& tc = list.Add("Subscribe");
    tc.Add("Hugh Jackman (as Wolverine in the \"X-Men\" franchise)");
    tc.Add("Al Pacino (as Michael Corleone in \"The Godfather\")");
    tc.Add("Heath Ledger (as the Joker in \"The Dark Knight\")");
    tc.Add("Javier Bardem (as Anton Chigurh in \"No Country for Old Men\")");
    tc.Add("Daniel Day-Lewis (as Daniel Plainview in \"There Will Be Blood\")");}

    {auto& tc = list.Add("Order Anything");
    tc.Add("Robert De Niro (as Travis Bickle in \"Taxi Driver\")");
    tc.Add("Heath Ledger (as Patrick Bateman in \"American Psycho\")");
    tc.Add("Edward Norton (as Derek Vinyard in \"American History X\")");
    tc.Add("Christian Bale (in his role as the Machinist)");
    tc.Add("Jake Gyllenhaal (as Lou Bloom in \"Nightcrawler\")");}

    {auto& tc = list.Add("Get Our Monthly Tips Email");
    tc.Add("Meryl Streep (in her role as Miranda Priestly in \"The Devil Wears Prada\")");
    tc.Add("Glenn Close (as Alex Forrest in \"Fatal Attraction\")");
    tc.Add("Sharon Stone (as Catherine Tramell in \"Basic Instinct\")");
    tc.Add("Angelina Jolie (as Maleficent in the film of the same name)");
    tc.Add("Viola Davis (in her role as Annalise Keating in \"How to Get Away with Murder\")");}

    {auto& tc = list.Add("Get a {{COMPANY}} Reading List");
    tc.Add("Jeremy Irons (as Simon Gruber in \"Die Hard with a Vengeance\")");
    tc.Add("Sean Connery (as James Bond in various films)");
    tc.Add("Gene Hackman (as Lex Luthor in the \"Superman\" franchise)");
    tc.Add("Kevin Spacey (as Verbal Kint in \"The Usual Suspects\")");
    tc.Add("Samuel L. Jackson (as Mace Windu in the \"Star Wars\" prequel trilogy)");}

    {auto& tc = list.Add("Check Out The {{COMPANY}} Webstore");
    tc.Add("Jack Nicholson (as Jack Torrance in \"The Shining\")");
    tc.Add("Heath Ledger (as the Joker in \"The Dark Knight\")");
    tc.Add("Christopher Walken (as Max Zorin in \"A View to a Kill\")");
    tc.Add("Christoph Waltz (as Blofeld in \"Spectre\")");
    tc.Add("Mark Hamill (as the Joker in various animated Batman films)");}
	
	return list;
}

VectorMap<String,Vector<String>>& GetTriggerUnsafeFemale() {
	thread_local static VectorMap<String,Vector<String>> list;
	
	if (!list.IsEmpty()) return list;
	
	{auto& tc = list.Add("Invest");
    tc.Add("Martha Stewart");
    tc.Add("Elizabeth Holmes");
    tc.Add("Farrah Abraham");
    tc.Add("Tyra Banks");
    tc.Add("Sophia Amoruso");}

    {auto& tc = list.Add("Grow");
    tc.Add("Arianna Huffington");
    tc.Add("Sheryl Sandberg");
    tc.Add("Kim Kardashian");
    tc.Add("Oprah Winfrey");
    tc.Add("Gwyneth Paltrow");}

    {auto& tc = list.Add("Advertise");
    tc.Add("Kylie Jenner");
    tc.Add("Paris Hilton");
    tc.Add("Joanna Gaines");
    tc.Add("Taylor Swift");
    tc.Add("Sophia Vergara");}

    {auto& tc = list.Add("Share Feedback");
    tc.Add("Wendy Williams");
    tc.Add("Ellen DeGeneres");
    tc.Add("Chelsea Handler");
    tc.Add("Joan Rivers");
    tc.Add("Rosie O'Donnell");}

    {auto& tc = list.Add("Discuss");
    tc.Add("Anne Coulter");
    tc.Add("Samantha Bee");
    tc.Add("Megyn Kelly");
    tc.Add("Alexandria Ocasio-Cortez");
    tc.Add("Sarah Palin");}

    {auto& tc = list.Add("View Ads");
    tc.Add("Victoria Beckham");
    tc.Add("Heidi Klum");
    tc.Add("Cindy Crawford");
    tc.Add("Kate Moss");
    tc.Add("Miranda Kerr");}

    {auto& tc = list.Add("Refer/Share");
    tc.Add("Kim Kardashian");
    tc.Add("Chrissy Teigen");
    tc.Add("Gisele Bundchen");
    tc.Add("Beyonce");
    tc.Add("Ellen DeGeneres");}

    {auto& tc = list.Add("Get a Freebie");
    tc.Add("Paris Hilton");
    tc.Add("Khloe Kardashian");
    tc.Add("Rihanna");
    tc.Add("Nicki Minaj");
    tc.Add("Miley Cyrus");}

    {auto& tc = list.Add("Remind Me!");
    tc.Add("Oprah Winfrey");
    tc.Add("Martha Stewart");
    tc.Add("Bethenny Frankel");
    tc.Add("Kris Jenner");
    tc.Add("Tyra Banks");}

    {auto& tc = list.Add("Book an appointment");
    tc.Add("Martha Stewart");
    tc.Add("Martha Stewart");
    tc.Add("Rachel Ray");
    tc.Add("Ina Garten");
    tc.Add("The Pioneer Woman");}

    {auto& tc = list.Add("Order Books");
    tc.Add("J.K. Rowling");
    tc.Add("E.L. James");
    tc.Add("Danielle Steel");
    tc.Add("Nora Roberts");
    tc.Add("Suzanne Collins");}

    {auto& tc = list.Add("Download Content");
    tc.Add("Sophia Amoruso");
    tc.Add("Michelle Obama");
    tc.Add("Rachel Hollis");
    tc.Add("Mindy Kaling");
    tc.Add("Lilly Singh");}

    {auto& tc = list.Add("Unleash Your Creativity");
    tc.Add("Madonna");
    tc.Add("Lady Gaga");
    tc.Add("Katy Perry");
    tc.Add("Lizzo");
    tc.Add("Rihanna");}

    {auto& tc = list.Add("Reserve Your Space");
    tc.Add("Martha Stewart");
    tc.Add("Beyonce");
    tc.Add("Kate Middleton");
    tc.Add("Angelina Jolie");
    tc.Add("Gwyneth Paltrow");}

    {auto& tc = list.Add("Sign Up Now!");
    tc.Add("Ivanka Trump");
    tc.Add("Caroline Kennedy");
    tc.Add("Hilaria Baldwin");
    tc.Add("Sarah Jessica Parker");
    tc.Add("Reese Witherspoon");}

    {auto& tc = list.Add("Subscribe");
    tc.Add("Jenna Kutcher");
    tc.Add("Gabby Bernstein");
    tc.Add("Maria Sharapova");
    tc.Add("Blake Lively");
    tc.Add("Amal Clooney");}

    {auto& tc = list.Add("Order Anything");
    tc.Add("Martha Stewart");
    tc.Add("Rachel Ray");
    tc.Add("Ina Garten");
    tc.Add("The Pioneer Woman");
    tc.Add("Julia Child");}

    {auto& tc = list.Add("Get Our Monthly Tips Email");
    tc.Add("Marie Forleo");
    tc.Add("Cheryl Strayed");
    tc.Add("Brene Brown");
    tc.Add("Mel Robbins");
    tc.Add("Caroline Myss");}

    {auto& tc = list.Add("Check Out The {{COMPANY}} Webstore");
    tc.Add("Kim Kardashian");
    tc.Add("Kylie Jenner");
    tc.Add("Rihanna");
    tc.Add("Jessica Alba");
    tc.Add("Jennifer Lopez");}

    {auto& tc = list.Add("Get a {{COMPANY}} Reading List");
    tc.Add("Oprah Winfrey");
    tc.Add("Reese Witherspoon");
    tc.Add("Emma Watson");
    tc.Add("Reese Witherspoon");
    tc.Add("Lupita Nyong'o");}
	
	return list;
}

VectorMap<String,Vector<String>>& GetTriggerSafe(bool gender) {
	if (!gender)
		return GetTriggerSafeMale();
	else
		return GetTriggerSafeFemale();
}

VectorMap<String,Vector<String>>& GetTriggerUnsafe(bool gender) {
	if (!gender)
		return GetTriggerUnsafeMale();
	else
		return GetTriggerUnsafeFemale();
}

VectorMap<String,Vector<String>>& GetPersuasiveTriggers(bool unsafe, bool gender) {
	if (!unsafe)
		return GetTriggerSafe(gender);
	else
		return GetTriggerUnsafe(gender);
}








const Index<String>& GetProgramGenres() {
	thread_local static Index<String> list;
	if (list.IsEmpty()) {
		list.Add("Action");
		list.Add("Adventure");
		list.Add("Role-Playing (RPG)");
		list.Add("Strategy");
		list.Add("Simulation");
		list.Add("Sports");
		list.Add("Puzzle");
		list.Add("Platformer");
		list.Add("Racing");
		list.Add("Fighting");
		list.Add("First-Person Shooter (FPS)");
		list.Add("Multiplayer Online Battle Arena (MOBA)");
		list.Add("Survival");
		list.Add("Horror");
		list.Add("Dance/Rhythm");
		list.Add("Educational");
		list.Add("Virtual Reality (VR)");
		list.Add("Massively Multiplayer Online (MMO)");
		list.Add("Productivity ");
		list.Add("Multimedia ");
		list.Add("Communication ");
		list.Add("Education ");
		list.Add("Finance ");
		list.Add("Photography ");
		list.Add("Social Media ");
		list.Add("Navigation ");
		list.Add("Health and Fitness ");
		list.Add("Weather ");
		list.Add("Shopping ");
		list.Add("Travel ");
		list.Add("News and Information ");
		list.Add("Utilities ");
		list.Add("Lifestyle ");
		list.Add("Reference ");
		list.Add("Gaming ");
		list.Add("Virtual/Augmented Reality (VR/AR)");
	}
	return list;
}

const Vector<ContentType>& GetProgrammingApproaches() {
	thread_local static Vector<ContentType> list;
	if (list.IsEmpty()) {
		list.Add().Set("Procedural programming", "procedures", "functions", "variables");
		list.Add().Set("Object-oriented programming (OOP)", "modularity", "reusability", "flexibility");
		list.Add().Set("Functional programming", "mathematical functions", "expressions", "immutable data structures");
		list.Add().Set("Event-driven programming", "events", " or system notifications", "response");
		list.Add().Set("Declarative programming", "result values", "result expressions", "result references");
		list.Add().Set("Imperative programming", "local commands", "global commands", "expression commands");
		list.Add().Set("Logical programming", "logical statements", "find a solution", "rules of logic");
		list.Add().Set("Aspect-oriented programming (AOP)", "reusable code", "multiple objects", "separation of concerns");
		list.Add().Set("Model-driven programming", "models", "the behavior of a system", "automatically generated");
		list.Add().Set("Domain-specific languages (DSLs)", "specific domains", "tasks", "abstraction");
		list.Add().Set("Low-level programming", "hardware", "assembly language", "machine code");
		list.Add().Set("High-level programming", "abstracted concepts", "syntax", "efficient");
		list.Add().Set("Event-driven concurrency", "events", "callbacks", "asynchronous execution");
		list.Add().Set("Multi-paradigm programming", "multiple programming paradigms", "take advantage of the strengths", "each approach");
		list.Add().Set("Test-driven development (TDD)", "automated tests", "write code", "pass those tests");
		list.Add().Set("Agile programming", "collaboration", "flexibility", "incremental development");
		list.Add().Set("Model-view-controller (MVC)", "model", "view", "controller");
		list.Add().Set("Rapid application development (RAD)", "building applications", "pre-made components", "rapid prototyping");
		list.Add().Set("Mobile app development", "creating applications", "different programming languages", "frameworks");
		list.Add().Set("Web development", "web applications", "websites", "web standards");
		list.Add().Set("Artificial intelligence (AI) programming", "human intelligence", "speech recognition", "decision-making");
		list.Add().Set("Machine learning", "algorithms", "learn and improve", "explicit programming");
	}
	return list;
}




END_UPP_NAMESPACE
