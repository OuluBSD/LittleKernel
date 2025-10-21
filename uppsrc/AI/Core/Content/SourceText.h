#ifndef _AI_Core_Content_SourceText_h_
#define _AI_Core_Content_SourceText_h_



class MergeProcess : public SolverBase {
	
	void LoadForAppending();
	void TransferContext();
	void TransferAmbiguous();
	void TransferScripts();
	void TransferOrphanedScripts();
	void TransferPhraseParts();
	void TransferWordnets();
	void TransferActionPhrases();
	void TransferActionTransitions();
	void CountValues();
	void TransferScript(const ScriptStruct& ss0, ScriptStruct& ss1);
	void Write();
	
	int TransferElement(int el_i0);
	int TransferTypeclass(int tc_i0);
	int TransferTokenText(int tt_i0);
	int TransferToken(int tk_i0);
	int TransferWord(int wrd_i0);
	ContentIdx TransferContent(ContentIdx con_i);
	int TransferVirtualPhrase(int vp_i0);
	int TransferVirtualPhraseStruct(int vps_i0);
	int TransferWordClass(int wc_i0);
	int TransferStructType(int st_i0);
	int TransferVirtualPhrasePart(int vpp_i0);
	int TransferStructPartType(int spt_i0);
	int TransferPhrasePart(int pp_i0, const TokenText* tt0);
	int TransferAttribute(int attr_i0);
	int TransferAction(int act_i0);
	int TransferSimpleAttr(int sa_i0);
	
	VectorMap<int,int> el_transfer, tt_transfer, token_transfer, vps_transfer, wordclass_transfer;
	VectorMap<int,int> vp_transfer, word_transfer, structtype_transfer, vpp_transfer;
	VectorMap<int,int> spt_transfer, pp_transfer, attr_transfer, sa_transfer, act_transfer;
	String batch_err;
	byte current_language = 0xFF;
	ContextType current_ctx;
	bool skip_typeclass_content = false;
	
	void SetBatchError(String s) {batch_err = s;}
public:
	enum {
		PHASE_RESET,
		PHASE_LOAD,
		PHASE_TRANSFER_CONTEXT,
		PHASE_TRANSFER_AMBIGUOUS_WORDS,
		PHASE_TRANSFER_SCRIPTS,
		PHASE_TRANSFER_OPRHANED_SCRIPTS,
		PHASE_TRANSFER_PHRASE_PARTS,
		PHASE_TRANSFER_WORDNETS,
		PHASE_TRANSFER_ACTION_PHRASES,
		PHASE_TRANSFER_ACTION_TRANSITION,
		PHASE_TRANSFER_COUNT,
		PHASE_WRITE,
		
		PHASE_COUNT
	};
	typedef MergeProcess CLASSNAME;
	MergeProcess();
	
	int GetPhaseCount() const override;
	int GetBatchCount(int phase) const override;
	int GetSubBatchCount(int phase, int batch) const override;
	void DoPhase() override;
	
	static MergeProcess& Get(DatasetPtrs p, String path, String language, String ctx, bool append);
	
	One<SrcTextData> target;
	String language_str;
	String context_str;
	String path_str;
	bool append = false;
	
};



#endif
