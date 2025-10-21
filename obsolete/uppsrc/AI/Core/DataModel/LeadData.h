#ifndef _AI_Core_DataModel_LeadData_h_
#define _AI_Core_DataModel_LeadData_h_



struct LeadOpportunity : Moveable<LeadOpportunity> {
	struct Genre : Moveable<Genre> {
		int id;
		String name;
		bool primary;
		void Visit(Vis& v);
		String ToString() const;
	};
	
	String id;
	Time first_seen, last_seen;
	String name;
	Vector<String> links;
	Vector<Genre> genres;
	Vector<Genre> promoter_group_genres;
	String band_opportunity_type;
	String obj_class;
	bool request_entry_fee = 0;
	bool request_featured = 0;
	bool request_exclusive = 0;
	bool request_curated = 0;
	bool request_contest = 0;
	String request_comments;
	String request_first_name;
	String request_last_name;
	String request_email;
	String request_phone;
	String request_description;
	String request_opportunity_description;
	String request_band_description;
	String request_selection_description;
	int vanity_url_id = 0;
	String vanity_url_name;
	String status_text;
	String description;
	String band_opportunity_type_text;
	String local_event_end_datetime;
	bool is_accepting_entries = true;
	bool deleted = 0;
	String address_str;
	String public_image_url;
	String logo_image_url;
	String promoter_group_name;
	String promoter_group_main_image_url;
	String promoter_group_facebook_url;
	String promoter_group_twitter_url;
	String promoter_group_youtube_url;
	String promoter_group_instagram_url;
	String promoter_group_talent_description;
	String promoter_group_short_description;
	String promoter_group_talent_roster;
	int promoter_group_opportunity_frequency_count = 0;
	String promoter_group_opportunity_frequency;
	bool compensated = 0;
	int min_compensation = 0;
	int max_compensation = 0;
	bool pay_to_apply = 0;
	bool free_to_apply = 0;
	int entry_count = 0;
	int min_entry_price_cents = 0;
	String entry_end_datetime;
	String date_created;
	
	double opp_score = -1;
	double money_score = -1;
	int money_score_rank = -1;
	int opp_score_rank = -1;
	double weighted_rank = 0;
	double chance_of_acceptance = 0;
	double average_payout_estimation = 0;
	Vector<String> chance_list;
	Vector<int> typeclasses;
	Vector<int> contents;
	Vector<String> lyrics_ideas;
	Vector<String> music_styles;
	
	Vector<int> analyzed_booleans;
	Vector<String> analyzed_string;
	Vector<Vector<String>> analyzed_lists;
	
	int GetCount() const;
	Value operator[](int i) const;
	const char* GetKey(int i) const;
	void Visit(Vis& v);
};

// TODO: rename to GigList or something
struct LeadData : Component {
	Vector<LeadOpportunity> opportunities;
	
	COMPONENT_CONSTRUCTOR(LeadData);
	void Load();
	void Store();
	LeadOpportunity& GetAddOpportunity(int leadsite, String id);
	void Visit(Vis& v) override;
	
	
};

INITIALIZE(LeadData)



#endif
