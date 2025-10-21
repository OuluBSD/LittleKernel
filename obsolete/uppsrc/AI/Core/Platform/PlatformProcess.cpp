#include "Platform.h"

NAMESPACE_UPP

void Platform::SetAttr(String name, bool value) {
	if (name.Left(4) == "has_") name = name.Mid(4);
	name = ToUpper(name);
	for(int i = 0; i < PLATFORM_ATTR_COUNT; i++) {
		const char* key = GetPlatformAttrEnum(i);
		if (name == key) {
			attrs[i] = value;
			return;
		}
	}
	Panic("Internal error: key not found: " + name);
}



int PlatformComment::GetTotalComments() const {
	int t = 1;
	for (const PlatformComment& pc : responses)
		t += pc.GetTotalComments();
	return t;
}

int PlatformThread::GetTotalComments() const {
	int t = 0;
	for (const PlatformComment& pc : comments)
		t += pc.GetTotalComments();
	return t;
}



PlatformAnalysis& PlatformManager::GetPlatform(int plat_i) {
	String key = GetPlatforms()[plat_i].name;
	return platforms.GetAdd(key);
}

SocietyRoleAnalysis& PlatformManager::GetAddRole(int role_i) {
	return roles.GetAdd(GetSocietyRoleEnum(role_i));
}

const SocietyRoleAnalysis* PlatformManager::FindRole(int role_i) const {
	String s = GetSocietyRoleEnum(role_i);
	int i = roles.Find(s);
	if (i < 0) return 0;
	return &roles[i];
}


// TODO to a ecs file
#define ENABLE(x) p.SetAttr(#x, true);

const Vector<Platform>& GetPlatforms() {
	static Vector<Platform> a;
	if (!a.IsEmpty())
		return a;
	a.SetCount(PLATFORM_COUNT);
	{
		Platform& p = a[PLATFORM_EMAIL];
		p.profile_type = PLATFORM_PROFILE_REAL_PERSON;
		p.name = "E-Mail";
		p.group = "Direct";
		p.description = "A platform for sending and receiving electronic mail messages.";
		ENABLE(has_title);
		ENABLE(has_message);
		ENABLE(has_link_promotion);
		p	<< "send an text email to someone"
			<< "send an email with images to someone"
			<< "send an email with links to someone"
			<< "reply to an email"
			;
	}
	{
		Platform& p = a[PLATFORM_TWITTER];
		p.name = "Twitter";
		p.group = "Short message site";
		p.description = "A social media platform for microblogging and sharing short messages or \"tweets.\"";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_hashtags);
		ENABLE(has_video);
		ENABLE(has_reel);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "send a short public message about your thoughts"
			<< "send a short public message about your opinion"
			<< "send a short public message about your news"
			<< "send a short public message about your updates"
			<< "reply to public messages with your own thoughts"
			<< "reply to public messages with your own opinions"
			<< "like other users' public messages"
			<< "follow other users to see their public messages and updates on your timeline"
			<< "create and join Twitter chats to engage in discussions on a specific topic"
			<< "use direct messages to have private conversations with other users"
			<< "share photos, videos, and GIFs in your public messages to make them more engaging"
			<< "customize your profile and bio to reflect your personal or brand identity"
			<< "search for specific topics using the search bar"
			<< "search for specific events using the search bar"
			<< "search for specific people using the search bar"
			<< "use hashtags to search for public messages related to a specific topic or event"
			;
	}
	{
		Platform& p = a[PLATFORM_THREADS];
		p.name = "Threads";
		p.group = "Short message site";
		p.description = "The Facebook owner Meta's clone of Twitter.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_hashtags);
		ENABLE(has_video);
		ENABLE(has_reel);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "send a short public message about your thoughts"
			<< "send a short public message about your opinion"
			<< "send a short public message about your news"
			<< "send a short public message about your updates"
			<< "reply to public messages with your own thoughts"
			<< "reply to public messages with your own opinions"
			<< "like other users' public messages"
			<< "follow other users to see their public messages and updates on your timeline"
			<< "create and join chats to engage in discussions on a specific topic"
			<< "use direct messages to have private conversations with other users"
			<< "share photos, videos, and GIFs in your public messages to make them more engaging"
			<< "customize your profile and bio to reflect your personal or brand identity"
			<< "search for specific topics using the search bar"
			<< "search for specific events using the search bar"
			<< "search for specific people using the search bar"
			<< "use hashtags to search for public messages related to a specific topic or event"
			;
	}
	{
		Platform& p = a[PLATFORM_FACEBOOK];
		p.name = "Facebook";
		p.group = "Full profile site";
		p.description = "A social networking platform for connecting with friends and sharing updates, photos, and other content.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_hashtags);
		ENABLE(has_video);
		ENABLE(has_reel);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "add a friend to your network and connect with people you know"
			<< "post a status update to share your thoughts and experiences"
			<< "post a photo to share your thoughts and experiences"
			<< "post a video to share your thoughts and experiences"
			<< "like a post from other user to engage with their content"
			<< "comment a post from other user to engage with their content"
			<< "share a post from other user to engage with their content"
			<< "join a group to connect with people who share the same interests as you"
			<< "use Facebook Live to stream videos in real-time and engage with your audience"
			<< "use hashtags to search for posts related to a specific topic or event"
			<< "use Messenger to have private conversations with other an user"
			<< "participate in an event"
			<< "invite a friend to join or atten an events"
			<< "use Explore to discover a new group or page based on your interests"
			<< "use Marketplace to buy an item in your local area"
			<< "use Marketplace to sell an item in your local area"
			;
	}
	{
		Platform& p = a[PLATFORM_INSTAGRAM];
		p.name = "Instagram";
		p.group = "Image site";
		p.description = "A visual-based social networking platform for sharing photos and videos.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_hashtags);
		ENABLE(has_video);
		ENABLE(has_reel);
		ENABLE(has_image);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "post a photo to share with followers"
			<< "post a video to share with followers"
			<< "comment on a post from other user"
			<< "like a post from othe user"
			<< "follow other user to see their posts on your feed"
			<< "use hashtags to search for posts related to a specific topic or event"
			<< "send a private message to other user through direct messaging"
			<< "share a post to your story"
			<< "share a post to a friend through direct messaging"
			<< "customize your profile to reflect your personal or brand identity"
			<< "discover and follow a new account through the Explore page"
			;
	}
	{
		Platform& p = a[PLATFORM_TIKTOK];
		p.name = "TikTok";
		p.group = "Video site";
		p.description = "A video-sharing platform for creating short and creative videos.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_hashtags);
		ENABLE(has_reel);
		p	<< "create and post a short video (up to 60 seconds) with music, sound effects, filters, and visual effects"
			<< "follow other user and see their content on your \"For You\" page"
			<< "like a video from other user"
			<< "comment a video from other user"
			<< "share a video from other user"
			<< "participate in challenge or trend by creating and sharing videos with specific hashtags"
			<< "use hashtags to search for videos related to a specific topic or challenge"
			<< "follow a hashtag to see content from other users who have used the same hashtag"
			<< "use the \"Duet\" feature to create a split-screen video with other user"
			<< "send a direct message to other user to engage in private conversations"
			<< "use the \"Discover\" feature to find new content and users to follow based on your interests"
			<< "play a game with other users"
			<< "participate in live streaming with other users"
			;
	}
	{
		Platform& p = a[PLATFORM_GETTR];
		p.name = "Gettr";
		p.group = "Short message";
		p.description = "A newer social media platform that promotes free speech and conservative viewpoints.";
		p.profile_type = PLATFORM_PROFILE_REAL_PERSON;
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_hashtags);
		ENABLE(has_reel);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "add a friend to your network and connect with people you know"
			<< "post a status update to share your thoughts and experiences"
			<< "post a photo to share your thoughts and experiences"
			<< "post a video to share your thoughts and experiences"
			<< "like a post from other user to engage with their content"
			<< "comment a post from other user to engage with their content"
			<< "share a post from other user to engage with their content"
			<< "use hashtags to search for posts related to a specific topic or event"
			<< "use private messaging to have private conversations with other user"
			<< "create and participate in groups to connect with users who share similar interests or viewpoints"
			;
	}
	{
		Platform& p = a[PLATFORM_LINKEDIN];
		p.name = "LinkedIn";
		p.group = "Professional site";
		p.description = "A professional networking platform for connecting with colleagues and building a professional profile.";
		p.profile_type = PLATFORM_PROFILE_REAL_PERSON;
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_hashtags);
		ENABLE(has_video);
		ENABLE(has_image);
		p	<< "connect with a colleagues"
			<< "connect with a peer"
			<< "connect with a industry professional"
			<< "connect with a potential client"
			<< "join and participate in a LinkedIn group to network and engage in a discussion with like-minded professionals"
			<< "share an update related to your industry or profession"
			<< "share an article related to your industry or profession"
			<< "share an insight related to your industry or profession"
			<< "endorse your skills on your profile"
			<< "ask for a recommendation from a colleague or client"
			<< "give a recommendation to a colleague or client"
			<< "search for a job opportunity and apply directly through the platform"
			<< "follow a company to stay updated on their news and job openings"
			<< "engage with a company"
			<< "participate in a LinkedIn learning course to enhance professional skills"
			<< "use the \"Jobs\" feature to search for job openings"
			<< "use LinkedIn messaging to establish a professional connection"
			<< "participate in a virtual event, webinar, or conference to network and gain industry insights"
			;
	}
	{
		Platform& p = a[PLATFORM_SOUNDCLOUD];
		p.name = "Soundcloud";
		p.group = "Music artist site";
		p.description = "An audio-sharing platform for musicians and creators to share their work.";
		p.profile_type = PLATFORM_PROFILE_MUSIC_ARTIST;
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_title);
		ENABLE(has_hashtags);
		ENABLE(has_audio);
		ENABLE(has_music);
		ENABLE(has_music_cover);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "upload and share original music or audio content"
			<< "listen to music or podcasts from other users"
			<< "like a track to show support"
			<< "repost a track to show support and to share with your followers"
			<< "comment on a track to engage with other user and give feedback"
			<< "follow other user to see their new releases and updates on your stream"
			<< "create and share a playlist of your favorite tracks"
			<< "join a groups or community based on your interests to discover new music and connect with others"
			<< "participate in a challenge or collaboration with other Soundcloud users"
			;
	}
	{
		Platform& p = a[PLATFORM_MUSIC_DISTRIBUTOR];
		p.name = "Music Distributor";
		p.group = "Professional music site";
		p.description = "A platform that helps musicians distribute their music to various streaming services and online stores.";
		ENABLE(has_title);
		ENABLE(has_music);
		ENABLE(has_image);
		ENABLE(has_music_cover);
		p	<< "upload and distribute music to various streaming platforms and online stores"
			<< "customize your artist profile on streaming platforms and online stores"
			<< "collaborate with other artists and producers through the platform's networking and collaboration features"
			;
	}
	{
		Platform& p = a[PLATFORM_YOUTUBE];
		p.name = "YouTube";
		p.group = "Video site";
		p.description = "A video-sharing platform for hosting and sharing user-generated content.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_title);
		ENABLE(has_hashtags);
		ENABLE(has_video);
		ENABLE(has_reel);
		ENABLE(has_image);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "create and upload a video to share with a wide audience"
			<< "like a video from other user to engage and interact with their content"
			<< "comment a video from other user to engage and interact with their content"
			<< "subscribe to other user's channel to see their new video uploads on your homepage"
			<< "join a YouTube community to engage in discussions with other users"
			<< "use hashtags to search for videos on a specific topic"
			<< "use the search bar to find videos and channels related to a specific topic or interest"
			<< "customize your channel by adding a profile picture, banner, and about section to reflect your personal or brand identity"
			;
	}
	{
		Platform& p = a[PLATFORM_VK];
		p.name = "VK";
		p.group = "Full profile site";
		p.description = "A social networking platform popular in Russia and other Eastern European countries.";
		p.profile_type = PLATFORM_PROFILE_REAL_PERSON;
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_hashtags);
		ENABLE(has_video);
		ENABLE(has_reel);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "post a update to share with their network of friends"
			<< "post a photo to share with their network of friends"
			<< "post a video to share with their network of friends"
			<< "post a link to share with their network of friends"
			<< "join a group based on shared interests, hobbies, or communities"
			<< "follow other user to see their updates, posts, and photos on your News Feed"
			<< "comment and like a post from other user to engage in conversations"
			<< "use hashtags (#) to search for posts related to a specific topic or event"
			<< "use the search bar to find specific users, groups, or posts"
			<< "use the \"Discover\" feature to explore new content from different categories"
			<< "use the messaging feature to have private conversation with a friend"
			;
	}
	{
		Platform& p = a[PLATFORM_REDDIT];
		p.name = "Reddit";
		p.group = "Forum";
		p.description = "A platform for discussion and sharing links and content on a variety of topics.";
		p.profile_type = PLATFORM_PROFILE_REAL_PERSON;
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_title);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		p	<< "join a subreddit communitt to discuss and share content on specific topics"
			<< "post a text to share content"
			<< "post a link to share content "
			<< "post a photo to share content"
			<< "post a video to share content"
			<< "start a discussion within a subreddit"
			<< "comment on a post to engage in discussion and provide additional information"
			<< "message other user privately to discuss or collaborate on specific topics"
			<< "customize your profile to reflect your interests and personality"
			<< "participate in AMA (Ask Me Anything) session with a public figure, expert, or celebrity."
			;
	}
	{
		Platform& p = a[PLATFORM_FORUM];
		p.name = "Forum";
		p.group = "Forum";
		p.description = "An online discussion platform where users can share and exchange ideas and information on a specific topic.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_title);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "create a post to share thoughts, opinions, or questions on a specific topic"
			<< "reply to a post from other users to engage in discussions or provide information"
			<< "search for posts related to a specific topic or interest"
			<< "use private messaging to have private conversations with other user"
			<< "customize your profile and bio to reflect your personal or brand identity"
			;
	}
	{
		Platform& p = a[PLATFORM_BLOGGER];
		p.name = "Blogger";
		p.group = "Personal";
		p.description = "A platform for creating and hosting personal or professional blogs.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_title);
		ENABLE(has_hashtags);
		ENABLE(has_music_cover);
		ENABLE(has_audio);
		ENABLE(has_music);
		ENABLE(has_video);
		ENABLE(has_reel);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "create and publish a blog post to share thoughts"
			<< "create and publish a blog post to share opinions"
			<< "create and publish a blog post to share stories"
			<< "create and publish a blog post to share information"
			<< "customize the design and layout of their blog to reflect their personal or brand identity"
			<< "follow and read other blogger's posts to stay updated and engage with their content"
			<< "join a blogging community or participate in guest blogging to expand their audience and network"
			;
	}
	{
		Platform& p = a[PLATFORM_WEBSITE];
		p.name = "Website";
		p.group = "Personal";
		p.description = "An online platform used to share information, promote products or services, or express personal opinions or viewpoints.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_message);
		ENABLE(has_title);
		ENABLE(has_hashtags);
		ENABLE(has_audio);
		ENABLE(has_music);
		ENABLE(has_music_cover);
		ENABLE(has_video);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "create and publish a blog post to share thoughts"
			<< "create and publish a blog post to share ideas"
			<< "create and publish a blog post to share stories"
			<< "share a photo to enhance their posts"
			<< "share a video to enhance their posts"
			<< "share other multimedia content to enhance their posts"
			<< "offer products or services for sale through their website"
			;
	}
	{
		Platform& p = a[PLATFORM_TWITCH];
		p.name = "Twitch";
		p.group = "Live site";
		p.description = "A live streaming platform popular among gamers but also used for other forms of live streaming entertainment.";
		p.profile_type = PLATFORM_PROFILE_REAL_PERSON;
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_video);
		p	<< "stream live content, such as gameplay, creative activities, or talk shows, to engage with viewers in real-time"
			<< "interact with viewers through live chat, responding to comments and questions"
			<< "follow other streamers to see their content and support their channels"
			<< "create and join communities based on shared interests or content themes"
			<< "participate in virtual events, such as tournaments or charity streams, with other streamers and viewers"
			<< "subscribe to channels to support and access exclusive content from streamers"
			<< "participate in group streams or collaborations with other streamers"
			;
	}
	{
		Platform& p = a[PLATFORM_STUMBLE];
		p.name = "Stumble";
		p.group = "Link sharing site";
		p.description = "A platform for discovering and sharing web content with a focus on personalized recommendations.";
		ENABLE(has_link_promotion);
	}
	{
		Platform& p = a[PLATFORM_GITHUB];
		p.name = "GitHub";
		p.group = "Professional programming site";
		p.description = "A platform for hosting and collaborating on software development projects.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
	}
	{
		Platform& p = a[PLATFORM_MYSPACE];
		p.name = "MySpace";
		p.group = "Music artist site";
		p.description = "A social networking platform that was once popular for music sharing and networking but has now declined in popularity.";
		p.profile_type = PLATFORM_PROFILE_MUSIC_ARTIST;
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_title);
		ENABLE(has_message);
		ENABLE(has_hashtags);
		ENABLE(has_music);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "add friends and connect with other users on the platform"
			<< "customize the layout and design of your profile"
			<< "share a blog post on your profile"
			<< "share a photo on your profile"
			<< "share a video on your profile"
			<< "share a musi on your profile"
			<< "join and participate in a group based on shared interests or communities"
			<< "send and receive a private messages with other user"
			<< "upload and manage music playlists to share with others"
			<< "write and receive comments on your profile and posts from friends"
			<< "use the search function to find and connect with other users or content on the platform"
			<< "participate in forums and discussions on various topics"
			;
	}
	{
		Platform& p = a[PLATFORM_MIKSERINET];
		p.name = "Mikseri.net";
		p.group = "Music artist site";
		p.description = "A Finnish social networking platform for musicians and music fans.";
		p.profile_type = PLATFORM_PROFILE_MUSIC_ARTIST;
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_title);
		ENABLE(has_message);
		ENABLE(has_hashtags);
		ENABLE(has_music);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "customize the layout and design of your profile"
			<< "share a blog post on your profile"
			<< "share a photo on your profile"
			<< "share a video on your profile"
			<< "share a musi on your profile"
			<< "send and receive a private messages with other user"
			<< "write and receive comments on your profile and posts from friends"
			<< "use the search function to find and connect with other users or content on the platform"
			<< "participate in forums and discussions on various topics"
			;
	}
	{
		Platform& p = a[PLATFORM_IRCGALLERIA];
		p.name = "Irc-Galleria.net";
		p.group = "Image site";
		p.description = "A Finnish image and chat-based social networking platform.";
		p.profile_type = PLATFORM_PROFILE_REAL_PERSON;
		ENABLE(has_description);
		ENABLE(has_profile_image);
		ENABLE(has_comments);
		ENABLE(has_hashtags);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_music_cover);
		p	<< "post and share photos in your profile"
			<< "post and share art in your profile"
			<< "post and share self promotion content in your profile"
			<< "use hashtags (#) to search for content related to a specific topic or interest"
			<< "send a public message to other user"
			<< "comment other user's image"
			;
	}
	{
		Platform& p = a[PLATFORM_DISCORD];
		p.name = "Discord";
		p.group = "Live site";
		p.description = "A platform for voice, video, and text communication primarily geared towards gamers but also used for other forms of online communities.";
		p.profile_type = PLATFORM_PROFILE_REAL_PERSON;
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_hashtags);
		ENABLE(has_audio);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		p	<< "join a community (server) to connect with other users who share similar interests or hobbies"
			<< "join a voice chat within servers to have real-time conversations with other users"
			<< "join a video chat within servers to have real-time conversations with other users"
			<< "send a public message to a channel to have a public conversation"
			<< "send a direct message to other user to have a private conversation"
			<< "create a channel within a server to discuss specific topics or share media"
			<< "customize your profile and bio to reflect your personal or server identity"
			<< "participate in voice or video discussions using the screen share feature"
			<< "share a file within a server"
			<< "share a photo within a server"
			<< "share a video within a server"
			<< "share a link within a server"
			;
	}
	{
		Platform& p = a[PLATFORM_MUKKEN];
		p.name = "Mukken";
		p.group = "Professional music site";
		p.description = "An European alternative for BandMix, which brings musicians, bands, music teachers, and producers together";
		p.profile_type = PLATFORM_PROFILE_REAL_PERSON;
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_title);
		ENABLE(has_message);
		ENABLE(has_hashtags);
		ENABLE(has_music);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		p	<< "make your profile to showcase your skills, experience, and musical interests"
			<< "search and connect with other musicians, bands, music teachers, and producers"
			<< "join music groups or communities with similar interests"
			<< "post and share your original music or covers to showcase your talent"
			<< "collaborate with other musicians, bands, or producers on projects"
			<< "search and apply for music gigs or job opportunities"
			<< "network and engage with other users through direct messages or by commenting on their profiles or posts"
			<< "provide feedback on other user's music on the platform"
			<< "use the platform's messaging features to communicate and collaborate with other users"
			;
	}
	{
		Platform& p = a[PLATFORM_BANDCAMP];
		p.name = "Bandcamp";
		p.group = "Professional music site";
		p.description = "A platform for musicians to sell and stream their music online.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_title);
		ENABLE(has_message);
		ENABLE(has_hashtags);
		ENABLE(has_music);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		p	<< "sell digital and physical music albums and singles to fans worldwide"
			<< "offer exclusive content or merchandise to fans"
			<< "connect with and collaborate with other musicians on the platform"
			<< "create and customize a profile to showcase music and engage with fans"
			<< "offer discounts or free downloads to incentivize fans to purchase music"
			<< "stream music to reach a wider audience and potentially gain new fans"
			<< "use tags and genres to make music more discoverable on the platform"
			;
	}
	{
		Platform& p = a[PLATFORM_REMOTEMORE];
		p.name = "RemoteMore";
		p.group = "Professional programming site";
		p.description = "A hiring marketplace for employers and employees that specializes in remote work.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		p	<< "create a profile to showcase your skills, experience, and work preferences"
			<< "search for and apply to remote job listings that match your skills and qualifications"
			<< "participate in and host virtual interviews and meetings"
			<< "use portfolio and project management tools to showcase your work and manage projects remotely"
			;
	}
	{
		Platform& p = a[PLATFORM_KUVAKENET];
		p.name = "Kuvake.net";
		p.group = "Image site";
		p.description = "A Finnish photo-sharing and social networking platform";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_hashtags);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		p	<< "upload and share a photo"
			<< "like and comment on a photo from other user to show appreciation or engage in conversations"
			<< "follow other user to see their photos and updates on your feed"
			<< "use captions and hashtags to describe your photos and make them more discoverable"
			<< "use direct messages to have private conversations with other user"
			<< "customize your profile and bio to reflect your personal style or brand"
			;
	}
	{
		Platform& p = a[PLATFORM_REVERBNATION];
		p.name = "ReverbNation";
		p.group = "Professional music site";
		p.description = "A platform for musicians to promote and distribute their music.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_title);
		ENABLE(has_hashtags);
		ENABLE(has_audio);
		ENABLE(has_music);
		ENABLE(has_image);
		ENABLE(has_comment_self_promotion_hack);
		p	<< "upload and share your music and music videos with your followers"
			<< "connect with other musicians, industry professionals, and fans"
			<< "participate in contests and music competitions to gain exposure and recognition"
			<< "sell your music and merchandise through the platform"
			<< "network and collaborate with other musicians on the platform"
			<< "promote your upcoming events and shows to your followers"
			<< "join and create music communities to connect with like-minded individuals"
			;
	}
	{
		Platform& p = a[PLATFORM_SONICBIDS];
		p.name = "SonicBids";
		p.group = "Professional music site";
		p.description = "A platform for musicians to find and apply for gigs and other opportunities.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_title);
		ENABLE(has_hashtags);
		ENABLE(has_audio);
		ENABLE(has_music);
		ENABLE(has_image);
		ENABLE(has_comment_self_promotion_hack);
		p	<< "search and apply for gigs and other opportunities posted by event organizers, venues, or brands"
			<< "promote your music by uploading songs, videos, and photos"
			<< "connect with other musicians, promoters, and industry professionals through the platform"
			<< "participate in online competitions and challenges to gain exposure and attract potential gigs"
			<< "use the messaging feature to communicate with event organizers and other users"
			;
	}
	{
		Platform& p = a[PLATFORM_MUSICGATEWAY];
		p.name = "MusicGateway";
		p.group = "Professional music site";
		p.description = "A platform for musicians and music professionals to network and collaborate.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_title);
		ENABLE(has_hashtags);
		ENABLE(has_audio);
		ENABLE(has_music);
		ENABLE(has_image);
		ENABLE(has_comment_self_promotion_hack);
		p	<< "search for other musicians and music professionals to connect and collaborate with"
			<< "send messages to other users to discuss project opportunities or collaborations"
			<< "use the marketplace to buy and sell services such as music production, songwriting, and mixing"
			<< "participate in challenges or competitions to showcase their talent and gain recognition"
			<< "give feedback on projects or collaborations"
			<< "attend virtual networking events and workshops to connect with other music professionals and learn from industry experts"
			<< "use the licensing platform to license your music for use in films, TV shows, and other media projects"
			<< "share and promote your music projects and collaborations on social media platforms through integration with other sites."
			;
	}
	{
		Platform& p = a[PLATFORM_INDIEONTHEMOVE];
		p.name = "Indie On The Move";
		p.group = "Professional music site";
		p.description = "A platform for musicians to find and book tour dates and accommodations.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_title);
		ENABLE(has_hashtags);
		ENABLE(has_audio);
		ENABLE(has_music);
		ENABLE(has_image);
		ENABLE(has_comment_self_promotion_hack);
		p	<< "search for and apply for tour opportunities and gigs"
			<< "upload music for fans and industry professionals to discover"
			<< "network with other musicians and industry professionals"
			<< "track tour dates and manage bookings"
			<< "stay updated on music industry news and tips for touring"
			<< "promote tour dates and updates to fans through social media integration"
			<< "customize artist profile and bio to reflect personal brand and music style"
			;
	}
	{
		Platform& p = a[PLATFORM_VOWAVE];
		p.name = "VoWave";
		p.group = "Professional music site";
		p.description = "A streaming and social networking platform focused on electronic and dance music.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_title);
		ENABLE(has_music);
		ENABLE(has_image);
		p	<< "follow and interact with other users, such as DJs, producers, and music enthusiasts"
			<< "stream and share own music or mixes"
			<< "discover new music and artists by listening to live streaming shows or recorded sets"
			<< "join and participate in virtual music events and live streams"
			<< "use hashtags to search for music related to a specific genre or event"
			<< "like, comment, and repost music from other users to show support or share with their followers"
			<< "use private messaging to connect with other users and collaborate on music projects"
			;
	}
	{
		Platform& p = a[PLATFORM_AUDIUS];
		p.name = "Audius";
		p.group = "Professional music site";
		p.description = "A website for music artists where you can share your own music, receive donations and network with other music artists.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_title);
		ENABLE(has_music);
		ENABLE(has_image);
		p	<< "upload and share your own music tracks or albums"
			<< "connect with other music artists and collaborate on projects"
			<< "discover and listen to music from other artists"
			<< "follow and support your favorite artists by listening to their music and leaving comments"
			<< "use hashtags to search for music related to a specific genre or mood"
			<< "join communities or groups to connect with other artists and share resources or advice"
			<< "customize your profile to reflect your brand or music style"
			;
	}
	{
		Platform& p = a[PLATFORM_SONGTRADR];
		p.name = "SongTrader";
		p.group = "Professional music site";
		p.description = "A global music licensing platform for artists and filmmakers to connect and collaborate.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_title);
		ENABLE(has_music);
		ENABLE(has_image);
		p	<< "browse and search for potential collaborations with filmmakers looking for music for their projects"
			<< "upload your music for licensing and potential use in film, TV, commercials, and other media projects"
			<< "connect and communicate with filmmakers through messaging and collaboration tools"
			<< "network and connect with other artists on the platform to collaborate and share resources"
			<< "explore and discover new music from other artists on the platform for potential collaborations"
			;
	}
	{
		Platform& p = a[PLATFORM_GROOVER];
		p.name = "Groover";
		p.group = "Professional music site";
		p.description = "A platform for artists to promote their music and connect with music professionals.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_title);
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_music);
		p	<< "upload and share your music, lyrics, and album artwork with other users"
			<< "discover and connect with music professionals such as labels, promoters, and bloggers"
			<< "submit their music to be reviewed by music professionals for potential promotion or collaborations"
			<< "join and participate in music industry-related groups and discussions"
			<< "connect with other artists and collaborate on music projects"
			<< "promote their music through targeted campaigns and collaborations with music professionals and brands"
			<< "search for opportunities such as festivals, competitions, and gigs to showcase their music"
			;
	}
	{
		Platform& p = a[PLATFORM_AIRPLAYDIRECT];
		p.name = "AirplayDirect";
		p.group = "Professional music site";
		p.description = "A digital music delivery system for radio stations and DJs.";
		ENABLE(has_title);
		ENABLE(has_link_promotion);
		ENABLE(has_music);
		ENABLE(has_music_cover);
		p	<< "upload and send new music releases to radio stations and DJs"
			<< "create and manage press releases and promotional materials for music releases"
			<< "network and connect with radio stations and DJs to promote music releases"
			<< "curate and maintain a personalized media library for music releases"
			<< "send updates and notifications about music releases and industry news"
			;
	}
	{
		Platform& p = a[PLATFORM_N1M];
		p.name = "N1M";
		p.group = "Professional music site";
		p.description = "A platform for independent musicians to promote and distribute their music.";
		ENABLE(has_description);
		ENABLE(has_profile_image);
		ENABLE(has_music_cover);
		ENABLE(has_music);
		ENABLE(has_title);
		ENABLE(has_message);
		ENABLE(has_link_promotion);
		p	<< "upload original music to promote to a wider audience"
			<< "follow other musicians and engage with their music by listening, liking, and leaving comments or feedback"
			<< "discover new music by browsing through different genres and playlists on the platform"
			<< "connect with fans and build a following by engaging with them through comments and direct messages"
			<< "participate in music challenges and contests to gain exposure and potentially win prizes"
			<< "collaborate with other musicians on the platform by featuring on each other's songs or creating joint releases"
			<< "share music on social media platforms to reach a larger audience"
			<< "sell music and merchandise through the platform's store feature"
			;
	}
	{
		Platform& p = a[PLATFORM_SOUNDBETTER];
		p.name = "SoundBetter";
		p.group = "Professional music site";
		p.description = "A platform for musicians and audio professionals to offer their services and collaborate.";
		ENABLE(has_description);
		ENABLE(has_profile_image);
		ENABLE(has_comments);
		ENABLE(has_Q_and_A);
		ENABLE(has_music);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		p	<< "showcase your skills, services, and experience as a musician or audio professional"
			<< "search for other professionals on the platform to find potential collaborators or clients"
			<< "offer your services to clients in need of musicians or audio professionals"
			<< "bid on projects posted by clients to work on music or audio-related tasks"
			<< "communicate with clients and collaborators through direct messages or in-app messaging"
			<< "share samples of your work or portfolio to showcase your talents"
			<< "collaborate with other professionals on projects by sending and receiving files through the platform"
			<< "participate in community forums and discussions to network with other professionals and share knowledge and resources"
			<< "give and receive feedback and ratings from clients or collaborators to build credibility"
			<< "update your profile with new projects and achievements to attract more clients"
			;
	}
	{
		Platform& p = a[PLATFORM_ABOUTME];
		p.name = "About.me";
		p.group = "Personal";
		p.description = "A platform for creating a personal landing page and showcasing professional skills and projects.";
		ENABLE(has_description);
		ENABLE(has_profile_image);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_link_promotion);
		p	<< "customize your page with a bio, profile picture, and background image"
			<< "add links to your social media profiles, websites, and other online content"
			;
	}
	{
		Platform& p = a[PLATFORM_FIVERR];
		p.name = "Fiverr";
		p.group = "Professional site";
		p.description = "A platform for freelancers to offer their services and connect with clients.";
		p.profile_type = PLATFORM_PROFILE_REAL_PERSON;
		ENABLE(has_description);
		ENABLE(has_profile_image);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_comments);
		p	<< "edit your profile to showcase skills, experience, and services offered"
			<< "search for and apply to freelance jobs posted by clients"
			<< "communicate with clients through direct messages to discuss project details and negotiate rates"
			<< "use Fiverr's built-in messaging and file sharing tools to collaborate with clients and deliver work"
			<< "offer customized services through Gig Extras, which allow for additional services to be added to a basic Gig"
			<< "utilize Fiverr's promotion tools to increase visibility and attract more clients"
			<< "participate in the Fiverr community by joining forums, discussions, and Q&A sessions"
			<< "leave and receive reviews to build a positive reputation on the platform"
			<< "use Fiverr's mobile app to manage gigs and communicate with clients on-the-go"
			;
	}
	{
		Platform& p = a[PLATFORM_THEDOTS];
		p.name = "The Dots";
		p.group = "Professional site";
		p.description = "A platform for creative professionals to showcase their work and connect with other creatives.";
		p.profile_type = PLATFORM_PROFILE_REAL_PERSON;
		ENABLE(has_description);
		ENABLE(has_profile_image);
		ENABLE(has_Q_and_A);
		ENABLE(has_link_promotion);
		p	<< "edit your profile to showcase your work and professional experience"
			<< "share projects, achievements, and accomplishments on your profile"
			<< "connect with other creatives in similar fields or industries"
			<< "follow other users to see their updates and work on your homepage"
			<< "participate in discussions and forums related to specific industries or topics"
			<< "post and search for job opportunities in the creative industry"
			<< "join and attend events and workshops to network and gain industry insights"
			<< "collaborate with other creatives on projects or events"
			<< "join or create communities of like-minded professionals to share knowledge and resources"
			<< "customize your profile to reflect your personal brand and style"
			;
	}
	{
		Platform& p = a[PLATFORM_CONSTANTCONTACT];
		p.name = "Constant Contact";
		p.group = "Direct";
		p.description = "A platform for email marketing and online surveys.";
		ENABLE(has_title);
		ENABLE(has_message);
		ENABLE(has_testimonial_hack);
		ENABLE(has_link_promotion);
		ENABLE(has_image);
		p	<< "design email marketing campaigns to reach and engage with potential or current customers"
			;
	}
	{
		Platform& p = a[PLATFORM_MUUSIKOIDEN_NET];
		p.name = "Muusikoiden.net";
		p.group = "Forum";
		p.description = "A Finnish platform for musicians and music fans to connect and share content and information.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_comments);
		ENABLE(has_message);
		ENABLE(has_title);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "edit your profile and showcase your music, band, or art"
			<< "post and share videos, audio recordings, photos, or other media related to music"
			<< "connect and collaborate with other musicians, artists, and bands"
			<< "join or create discussion forums and threads to engage in conversations and share information"
			<< "advertise and promote your music or art to a targeted audience of music fans and industry professionals"
			<< "use the classifieds section to buy, sell, or trade music equipment or instruments"
			<< "stay updated on the latest news and events in the Finnish music scene"
			<< "search for and discover new music and artists"
			<< "participate in online competitions and contests to showcase your talent"
			<< "engage in discussions and debates on various music-related topics"
			<< "attend or organize offline events and meetups to network with other musicians and music lovers"
			;
	}
	{
		Platform& p = a[PLATFORM_PODCAST];
		p.name = "Podcast";
		p.group = "Radio";
		p.description = "A platform for hosting and distributing talking shows for listeners to download or stream.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_message);
		ENABLE(has_title);
		ENABLE(has_link_promotion);
		ENABLE(has_comment_self_promotion_hack);
		ENABLE(has_Q_and_A_hack);
		ENABLE(has_testimonial_hack);
		p	<< "share audio recordings related to music"
			<< "connect and collaborate with other musicians, artists, and bands"
			<< "engage in discussions and debates on various music-related topics"
			;
	}
	{
		Platform& p = a[PLATFORM_TINDER];
		p.name = "Tinder";
		p.group = "Dating Site";
		p.description = "A popular dating app that allows users to swipe through potential matches and message each other if they both swipe right on each other's profiles.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_message);
		p	<< "edit profile with basic information, photos, and optional biographical information"
			<< "swipe left (reject) or right (like) on potential matches based on their profile photos and information"
			<< "match with other users who have also swiped right on your profile"
			<< "send messages to your matches to start a conversation and get to know each other better"
			<< "use the Super Like feature to show extra interest in a potential match"
			<< "create a list of users you have liked and view their profiles again later"
			<< "participate in Tinder Boost to increase your profile's visibility for a certain period of time"
			<< "adjust your profile settings and opt-out of certain features, such as being shown to friends or work colleagues"
			;
	}
	{
		Platform& p = a[PLATFORM_PATREON];
		p.name = "Patreon";
		p.group = "Exclusive social media";
		p.description = "A membership platform that allows creators to receive financial support from their fans or \"patrons\" in exchange for exclusive content and perks.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_message);
		ENABLE(has_title);
		ENABLE(has_audio);
		ENABLE(has_music);
		ENABLE(has_video);
		ENABLE(has_comments);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_Q_and_A_hack);
		p	<< "showcase your work and attract potential patrons"
			<< "engage with your patrons through private messaging, polls, and exclusive live streams"
			<< "post exclusive content for your patrons (e.g. behind-the-scenes updates, early access to videos or articles, bonus content, etc.)"
			<< "promote your Patreon page through social media and other platforms to attract new patrons"
			<< "collaborate with other creators on Patreon to reach new audiences and cross-promote each other's work"
			<< "provide updates and progress reports to your patrons to keep them informed and engaged"
			<< "express gratitude and appreciation to your patrons for their support and contributions"
			;
	}
	{
		Platform& p = a[PLATFORM_LOCALS];
		p.name = "Locals";
		p.group = "Exclusive social media";
		p.description = "A social media platform designed for creators to interact with their audience and monetize their content.";
		ENABLE(has_profile_image);
		ENABLE(has_description);
		ENABLE(has_message);
		ENABLE(has_title);
		ENABLE(has_audio);
		ENABLE(has_music);
		ENABLE(has_video);
		ENABLE(has_comments);
		ENABLE(has_image);
		ENABLE(has_link_promotion);
		ENABLE(has_Q_and_A_hack);
		p	<< "manage a paid subscription for your fans to access exclusive content"
			<< "engage in discussions and direct conversations with your paying subscribers"
			<< "share updates, announcements, and previews of your content with subscribers"
			<< "create and participate in Q&A sessions and live events for your subscribers"
			<< "offer exclusive discounts and promotions for your subscribers"
			<< "collaborate and connect with other creators on the platform"
			<< "customize your profile and bio to reflect your brand or niche"
			<< "use tags and categories to make your content more discoverable for potential subscribers"
			<< "interact with fans and followers through comments, likes, and private messages"
			<< "participate in community features, such as polls and discussions, to engage with your audience"
			<< "use the platform as a source for feedback and suggestions from your subscribers"
			<< "diversify your content by incorporating various formats, such as videos, photos, written posts, etc."
			<< "promote your Locals account through other social media platforms to attract new subscribers"
			;
	}
	
	
	
	return a;
}

void ProfileData::Visit(Vis& v) {
	v.Ver(1)
	(1)	.VisitVector("platforms", platforms)
		;
}

void ProfileData::Load() {
	TODO
	#if 0
	String dir = AppendFileName(MetaDatabase::GetDirectory(), "share");
	String fname = IntStr64(hash) + ".json";
	String path = AppendFileName(dir, fname);
	
	LoadFromJsonFileStandard(*this, path);
	#endif
}

void ProfileData::Store() {
	TODO
	#if 0
	String dir = AppendFileName(MetaDatabase::GetDirectory(), "share");
	String fname = IntStr64(hash) + ".json";
	String path = AppendFileName(dir, fname);
	
	RealizeDirectory(dir);
	StoreAsJsonFileStandard(*this, path);
	#endif
}

Array<ProfileData>& ProfileData::GetAll() {
	static Array<ProfileData> a;
	return a;
}

ProfileData& ProfileData::Get(Profile& p) {
	Array<ProfileData>& a = GetAll();
	CombineHash ch;
	ch.Do(p.val.id);
	ch.Do(p.name);
	hash_t h = ch;
	for (ProfileData& pd : a) {
		if (pd.hash == h) {
			ASSERT(pd.profile == &p);
			pd.platforms.SetCount(PLATFORM_COUNT);
			return pd;
		}
	}
	ProfileData& pd = a.Add();
	pd.hash = h;
	pd.Load();
	pd.platforms.SetCount(PLATFORM_COUNT);
	pd.profile = &p;
	return pd;
}

void ProfileData::StoreAll() {
	for (ProfileData& pd : GetAll())
		pd.Store();
}



int PlatformAnalysis::GetRoleScoreSum(const PlatformManager& plat, int score_i) const {
	ASSERT(score_i >= 0 && score_i < SOCIETYROLE_SCORE_COUNT);
	int sum = 0;
	for(int i = 0; i < roles.GetCount(); i++) {
		int role_i = roles[i];
		int weight = 5 + (roles.GetCount() - i);
		const SocietyRoleAnalysis* sra = plat.FindRole(role_i);
		if (!sra)
			continue;
		sum += weight * sra->scores[score_i];
	}
	return sum;
}

double PlatformAnalysis::GetRoleScoreSumWeighted(const PlatformManager& plat, int score_i) const {
	ASSERT(score_i >= 0 && score_i < SOCIETYROLE_SCORE_COUNT);
	int sum = 0;
	int weight_sum = 0;
	for(int i = 0; i < roles.GetCount(); i++) {
		int role_i = roles[i];
		int weight = 5 + (roles.GetCount() - i);
		const SocietyRoleAnalysis* sra = plat.FindRole(role_i);
		if (!sra)
			continue;
		sum += weight * sra->scores[score_i];
		weight_sum += weight;
	}
	return (double)sum / (double)weight_sum;
}

INITIALIZER_COMPONENT(PlatformManager, "ecs.public.platform.manager", "Ecs|Public")















PlatformProcess::PlatformProcess() {
	
}

int PlatformProcess::GetPhaseCount() const {
	return PHASE_COUNT;
}

int PlatformProcess::GetBatchCount(int phase) const {
	switch (phase) {
		case PHASE_ANALYZE_ROLE_SCORES:						return SOCIETYROLE_COUNT;
		case PHASE_ANALYZE_PLATFORM_ROLES:					return PLATFORM_COUNT;
		case PHASE_ANALYZE_PLATFORM_EPK_TEXT_FIELDS:		return PLATFORM_COUNT;
		case PHASE_ANALYZE_PLATFORM_EPK_PHOTO_TYPES:		return PLATFORM_COUNT;
		case PHASE_ANALYZE_PLATFORM_EPK_PHOTO_AI_PROMPTS:	return PLATFORM_COUNT;
		default: return 1;
	}
}

int PlatformProcess::GetSubBatchCount(int phase, int batch) const {
	if (phase == PHASE_ANALYZE_PLATFORM_EPK_PHOTO_AI_PROMPTS) {
		const Platform& plat = GetPlatforms()[batch];
		const PlatformAnalysis& pa = p.platmgr->GetPlatform(batch);
		return pa.epk_photos.GetCount();
	}
	return 1;
}

void PlatformProcess::DoPhase() {
	switch (phase) {
		case PHASE_ANALYZE_ROLE_SCORES:						ProcessAnalyzeRoleScores(); return;
		case PHASE_ANALYZE_PLATFORM_ROLES:					ProcessAnalyzePlatformRoles(); return;
		case PHASE_ANALYZE_PLATFORM_EPK_TEXT_FIELDS:		ProcessAnalyzePlatformEpkTextFields(); return;
		case PHASE_ANALYZE_PLATFORM_EPK_PHOTO_TYPES:		ProcessAnalyzePlatformEpkPhotoTypes(); return;
		case PHASE_ANALYZE_PLATFORM_EPK_PHOTO_AI_PROMPTS:	ProcessAnalyzePlatformEpkPhotoAiPrompts(); return;
		default: return;
	}
}

PlatformProcess& PlatformProcess::Get(DatasetPtrs p) {
	static ArrayMap<String, PlatformProcess> arr;
	
	String key = p.platmgr->val.GetPath();
	PlatformProcess& ts = arr.GetAdd(key);
	ts.p = p;
	return ts;
}

void PlatformProcess::ProcessAnalyzeRoleScores() {
	if (batch >= SOCIETYROLE_COUNT) {
		NextPhase();
		return;
	}
	PlatformManager& plat = *p.platmgr;
	const SocietyRoleAnalysis& sra = plat.GetAddRole(batch);
	
	if (skip_ready && sra.GetScoreSum() > 0) {
		NextBatch();
		return;
	}
	
	SocialArgs args;
	args.fn = 13;
	args.text = GetSocietyRoleKey(batch);
	args.description = GetSocietyRoleDescription(batch);
	
	SetWaiting(1);
	TaskMgr& m = AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessAnalyzeRoleScores));
}

void PlatformProcess::OnProcessAnalyzeRoleScores(String res) {
	PlatformManager& plat = *p.platmgr;
	SocietyRoleAnalysis& sra = plat.GetAddRole(batch);
	sra.Zero();
	
	if (res.Left(2) != "1.")
		res = "1. " + res;
	
	Vector<String> lines = Split(res, "\n");
	for (String& l : lines) {
		Vector<String> parts;
		if (l.Find(":") >= 0)
			parts = Split(l, ":");
		else
			parts = Split(l, ".");
		
		if (parts.GetCount() < 2)
			continue;
		for (String& s : parts) s = TrimBoth(s);
		
		int score_i = ScanInt(parts[0]) - 1;
		int score = max(0, min(10, ScanInt(parts[1])));
		
		if (score_i >= 0 && score_i < SOCIETYROLE_SCORE_COUNT)
			sra.scores[score_i] = score;
	}
	
	SetWaiting(0);
	NextBatch();
}

void PlatformProcess::ProcessAnalyzePlatformRoles() {
	if (batch >= PLATFORM_COUNT) {
		NextPhase();
		return;
	}
	
	const Platform& plat = GetPlatforms()[batch];
	const PlatformAnalysis& pa = p.platmgr->GetPlatform(batch);
	
	if (skip_ready && pa.roles.GetCount()) {
		NextBatch();
		return;
	}
	
	SocialArgs args;
	args.fn = 11;
	for(int i = 0; i < SOCIETYROLE_COUNT; i++)
		args.parts.Add(GetSocietyRoleKey(i));
	args.text = plat.name;
	args.description = plat.description;
	
	SetWaiting(1);
	TaskMgr& m = AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessAnalyzePlatformRoles));
}

void PlatformProcess::OnProcessAnalyzePlatformRoles(String res) {
	const Platform& plat = GetPlatforms()[batch];
	PlatformAnalysis& pa = p.platmgr->GetPlatform(batch);
	
	res = "1. #" + res;
	
	RemoveEmptyLines2(res);
	
	pa.roles.Clear();
	Vector<String> lines = Split(res, "\n");
	for (String& l : lines) {
		int a = l.Find("#");
		if (a >= 0) {
			a++;
			int role_i = ScanInt(l.Mid(a)); // 0 is first
			if (role_i >= 0 && role_i < SOCIETYROLE_COUNT)
				pa.roles << role_i;
		}
	}
	
	SetWaiting(0);
	NextBatch();
}

void PlatformProcess::ProcessAnalyzePlatformEpkTextFields() {
	
	if (batch >= PLATFORM_COUNT) {
		NextPhase();
		return;
	}
	
	const Platform& plat = GetPlatforms()[batch];
	const PlatformAnalysis& pa = p.platmgr->GetPlatform(batch);
	
	if (skip_ready && pa.epk_text_fields.GetCount()) {
		NextBatch();
		return;
	}
	
	SocialArgs args;
	args.fn = 12;
	args.text = plat.name;
	args.description = plat.description;
	for(int i = 0; i < pa.roles.GetCount(); i++)
		args.parts.Add(GetSocietyRoleKey(pa.roles[i]));
	
	if (plat.profile_type == PLATFORM_PROFILE_ANY)
		args.profile = "any type of real person or artist profile";
	else if (plat.profile_type == PLATFORM_PROFILE_MUSIC_ARTIST)
		args.profile = "an music artist";
	else if (plat.profile_type == PLATFORM_PROFILE_VISUAL_ARTIST)
		args.profile = "an visual artist";
	else if (plat.profile_type == PLATFORM_PROFILE_PHOTOGRAPHER)
		args.profile = "a photographer";
	else if (plat.profile_type == PLATFORM_PROFILE_REAL_PERSON)
		args.profile = "a real person";
	else
		args.profile = GetPlatformProfileKey(plat.profile_type);
	
	SetWaiting(1);
	TaskMgr& m = AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessAnalyzePlatformEpkTextFields));
}

void PlatformProcess::OnProcessAnalyzePlatformEpkTextFields(String res) {
	const Platform& plat = GetPlatforms()[batch];
	PlatformAnalysis& pa = p.platmgr->GetPlatform(batch);
	
	if (res.Left(2) != "1.")
		res = "1. " + res;
	
	RemoveEmptyLines2(res);
	
	res.Replace("Name: ", "");
	res.Replace("name: ", "");
	res.Replace("\nDescription: ", ":");
	res.Replace("\ndescription: ", ":");
	res.Replace("Description: ", ":");
	res.Replace("description: ", ":");
	res.Replace("\n\n","\n");
	//LOG(res);
	
	
	pa.epk_text_fields.Clear();
	Vector<String> lines = Split(res, "\n");
	for (String& l : lines) {
		l = TrimBoth(l);
		if (l.IsEmpty()) continue;
		String key, value;
		int a = l.Find(":");
		if (a == -1)
			a = l.Find("-");
		if (a >= 0) {
			key = TrimBoth(l.Left(a));
			value = TrimBoth(l.Mid(a+1));
			if (value.Left(1) == ":") value = TrimBoth(value.Mid(1));
			value = Capitalize(value);
		}
		else {
			key = l;
		}
		pa.epk_text_fields.Add(key, value);
	}
	
	SetWaiting(0);
	NextBatch();
}

void PlatformProcess::ProcessAnalyzePlatformEpkPhotoTypes() {
	
	if (batch >= PLATFORM_COUNT) {
		NextPhase();
		return;
	}
	
	const Platform& plat = GetPlatforms()[batch];
	const PlatformAnalysis& pa = p.platmgr->GetPlatform(batch);
	
	if (skip_ready && pa.epk_photos.GetCount()) {
		NextBatch();
		return;
	}
	
	SocialArgs args;
	args.fn = 14;
	args.text = plat.name;
	args.description = plat.description;
	for(int i = 0; i < pa.roles.GetCount(); i++)
		args.parts.Add(GetSocietyRoleKey(pa.roles[i]));
	
	if (plat.profile_type == PLATFORM_PROFILE_ANY)
		args.profile = "any type of real person or artist profile";
	else if (plat.profile_type == PLATFORM_PROFILE_MUSIC_ARTIST)
		args.profile = "an music artist";
	else if (plat.profile_type == PLATFORM_PROFILE_VISUAL_ARTIST)
		args.profile = "an visual artist";
	else if (plat.profile_type == PLATFORM_PROFILE_PHOTOGRAPHER)
		args.profile = "a photographer";
	else if (plat.profile_type == PLATFORM_PROFILE_REAL_PERSON)
		args.profile = "a real person";
	else
		args.profile = GetPlatformProfileKey(plat.profile_type);
	
	SetWaiting(1);
	TaskMgr& m = AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessAnalyzePlatformEpkPhotoTypes));
}

void PlatformProcess::OnProcessAnalyzePlatformEpkPhotoTypes(String res) {
	const Platform& plat = GetPlatforms()[batch];
	PlatformAnalysis& pa = p.platmgr->GetPlatform(batch);
	
	if (res.Left(2) != "1.")
		res = "1. " + res;
	
	RemoveEmptyLines2(res);
	
	res.Replace("Name: ", "");
	res.Replace("name: ", "");
	res.Replace("\nDescription: ", ":");
	res.Replace("\ndescription: ", ":");
	res.Replace("Description: ", ":");
	res.Replace("description: ", ":");
	res.Replace("\n\n","\n");
	//LOG(res);
	
	
	pa.epk_photos.Clear();
	Vector<String> lines = Split(res, "\n");
	for (String& l : lines) {
		l = TrimBoth(l);
		if (l.IsEmpty()) continue;
		String key, value;
		int a = l.Find(":");
		if (a == -1)
			a = l.Find("-");
		if (a >= 0) {
			key = TrimBoth(l.Left(a));
			value = TrimBoth(l.Mid(a+1));
			if (value.Left(1) == ":") value = TrimBoth(value.Mid(1));
			value = Capitalize(value);
		}
		else {
			key = l;
		}
		RemoveQuotes(key);
		pa.epk_photos.Add(key).description = value;
	}
	
	SetWaiting(0);
	NextBatch();
}

void PlatformProcess::ProcessAnalyzePlatformEpkPhotoAiPrompts() {
	
	if (batch >= PLATFORM_COUNT) {
		NextPhase();
		return;
	}
	
	const Platform& plat = GetPlatforms()[batch];
	const PlatformAnalysis& pa = p.platmgr->GetPlatform(batch);
	if (sub_batch >= pa.epk_photos.GetCount()) {
		NextBatch();
		return;
	}
	
	const PlatformAnalysisPhoto& pap = pa.epk_photos[sub_batch];
		if (skip_ready && pap.prompts.GetCount()) {
		NextBatch();
		return;
	}
	
	SocialArgs args;
	args.fn = 15;
	args.text = plat.name;
	args.description = plat.description;
	for(int i = 0; i < pa.roles.GetCount(); i++)
		args.parts.Add(GetSocietyRoleKey(pa.roles[i]));
	
	if (plat.profile_type == PLATFORM_PROFILE_ANY)
		args.profile = "any type of real person or artist profile";
	else if (plat.profile_type == PLATFORM_PROFILE_MUSIC_ARTIST)
		args.profile = "an music artist";
	else if (plat.profile_type == PLATFORM_PROFILE_VISUAL_ARTIST)
		args.profile = "an visual artist";
	else if (plat.profile_type == PLATFORM_PROFILE_PHOTOGRAPHER)
		args.profile = "a photographer";
	else if (plat.profile_type == PLATFORM_PROFILE_REAL_PERSON)
		args.profile = "a real person";
	else
		args.profile = GetPlatformProfileKey(plat.profile_type);
	
	args.photo_description = pa.epk_photos.GetKey(sub_batch) + ": " + pap.description;
	
	SetWaiting(1);
	TaskMgr& m = AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessAnalyzePlatformEpkPhotoAiPrompts));
}

void PlatformProcess::OnProcessAnalyzePlatformEpkPhotoAiPrompts(String res) {
	const Platform& plat = GetPlatforms()[batch];
	PlatformAnalysis& pa = p.platmgr->GetPlatform(batch);
	PlatformAnalysisPhoto& pap = pa.epk_photos[sub_batch];
	
	RemoveEmptyLines3(res);
	
	pap.prompts.Clear();
	Vector<String> lines = Split(res, "\n");
	for (String& l : lines) {
		l = TrimBoth(l);
		RemoveQuotes(l);
		pap.prompts.Add().prompt = l;
	}
	
	SetWaiting(0);
	NextSubBatch();
}

void PlatformComment::ClearMerged() {
	text_merged_status.Clear();
	for (auto& o : responses)
		o.ClearMerged();
}

END_UPP_NAMESPACE
