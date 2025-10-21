#ifndef _AI_Core_Content_LyricStructSolver_h_
#define _AI_Core_Content_LyricStructSolver_h_



class TextComparison {
	WString a, b;
	Vector<int> sflags, aflags;
	Vector<WString> aw, bw;
	Vector<bool> a_in_use, b_in_use;
	VectorMap<int, double> link_weights;
	Vector<int> map_ab, unlinked_a, unlinked_b;
	Vector<double> map_ab_weights;

	double SCALING_FACTOR = 0.1;

	static void ReplaceWithWhite(WString& a);

public:
	typedef TextComparison CLASSNAME;
	TextComparison();
	double Process(const String& a, const String& b);

	double JaroWinklerDistance(const WString& s, const WString& a);
};

struct SoftMatchString : Moveable<SoftMatchString> {
	Vector<String> lines;
	SoftMatchString() {}
	SoftMatchString(const Vector<String>& s) { lines <<= s; }
	SoftMatchString(const SoftMatchString& s) { lines <<= s.lines; }
	SoftMatchString(SoftMatchString&& s) { lines <<= s.lines; }
	void operator=(const SoftMatchString& s) { lines <<= s.lines; }
	void operator=(const Vector<String>& s) { lines <<= s; }
	bool operator==(const SoftMatchString& s) const;
	hash_t GetHashValue() const;
};

struct TextDescriptor : Moveable<TextDescriptor> {
	struct Item : Moveable<Item> {
		uint16 from, to;
		uint16 count;

		bool operator()(const Item& a, const Item& b) const
		{
			if(a.count != b.count)
				return a.count > b.count;
			if(a.from != b.from)
				return a.from < b.from;
			return a.to < b.to;
		}
		void Zero()
		{
			from = 0;
			to = 0;
			count = 0;
		}
	};

	static constexpr int ITEM_COUNT = 50;

	Item items[ITEM_COUNT];

	void Zero() { memset(items, 0, sizeof(items)); }
	hash_t GetHash(int c) const;
};

class ScriptStructureSolverBase {

public:
	struct MetaSection : Moveable<MetaSection> {
		Vector<int> sections;
		TextPartType type = TXT_INVALID;
		int num = -1;
		int count = 0;
	};
	struct UniqueLine : Moveable<UniqueLine> {
		String txt;
		Vector<int> lines;
		Vector<int> possible_sections;
		int count = 0;
		bool operator()(const UniqueLine& a, const UniqueLine& b) const
		{
			return a.count > b.count;
		}
		String ToString() const { return IntStr(count) + ": " + txt; }
	};

	String input;
	Vector<MetaSection> meta_sections;
	String debug_out;

	// Params
	int section_cmp_header_len = 3;
	double cr_limit = 0.3;
	bool force_limit = false;
	double forced_limit_value = 0;
	double repeatability_range = 0.2;

	// Temp
	double iter_r_limit = 0;

public:
	typedef ScriptStructureSolverBase CLASSNAME;
	ScriptStructureSolverBase();

	void Process(String s);
	void SetForcedLimit(double d)
	{
		forced_limit_value = d;
		force_limit = true;
	}

	String GetDebugOut() const { return debug_out; }

protected:
	virtual void DefaultProcess() = 0;
	void SingleIteration();

	virtual void MakeLines() = 0;
	virtual void MakeUniqueLines() = 0;
	virtual void MakeSections() = 0;
	virtual void MakeRepeatingSectionLines() = 0;
	virtual void MakeSingleLineSections() = 0;
	virtual void MakeMetaSections() = 0;

public:
	virtual String GetDebugLines() const = 0;
	virtual String GetDebugSections() const = 0;
	virtual String GetResult() const = 0;
	virtual String GetDebugHashes() const = 0;
	virtual String FindLine(hash_t h) const = 0;
};

class TryStrDistSectionSolverBase : public ScriptStructureSolverBase {

public:
	struct Line : Moveable<Line> {
		TextDescriptor descriptor;
		String txt;
		int section = -1;
		double repeatability = 0;
		double circumstacial_repeatability = 0;
		double GetRepeatabilitySum() const
		{
			return repeatability + circumstacial_repeatability;
		}
	};
	struct Section : Moveable<Section> {
		Vector<String> lines;
		int meta_section = -1;
		int orig_count = 0;
		int count = 0;
		int orig_weight = 0;
		double repeat = 0;
		int first_line = -1;
		bool flag_repeating = false;
		bool operator()(const Section& a, const Section& b) const
		{
			return a.lines.GetCount() > b.lines.GetCount();
		}
	};
	struct HashRangeLink : Moveable<HashRangeLink> {
		SoftMatchString k;
		Vector<int> v;
	};

	Vector<Line> lines;
	Vector<Section> sections;
	Vector<UniqueLine> uniq_lines;
	double unique_dist_limit = 0.95;
	Vector<HashRangeLink> hash_ranges;

	UniqueLine& GetAddUniqueLine(const String& s);
	Vector<int>& GetAddHashRange(const SoftMatchString& s);

	void DefaultProcess() override;
	void MakeLines() override;
	void MakeUniqueLines() override;
	void MakeSingleLineSections() override;
	void MakeMetaSections() override;
	String GetDebugLines() const override;
	String GetDebugSections() const override;
	String GetResult() const override;
	String GetDebugHashes() const override;
	String FindLine(hash_t h) const override;
	int GetMetaSectionLen(int ms_i) const;
};

// TODO rename
class TryNo5tStructureSolver : public TryStrDistSectionSolverBase {

public:
	void MakeSections() override;
	void MakeRepeatingSectionLines() override;
};



#endif
