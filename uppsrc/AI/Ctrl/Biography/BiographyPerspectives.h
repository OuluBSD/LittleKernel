#ifndef _AI_Ctrl_BiographyPerspectives_h_
#define _AI_Ctrl_BiographyPerspectives_h_

NAMESPACE_UPP


class ConceptualFrameworkNavigator : public AiComponentCtrl {
	Splitter cfsplit, vsplit, tsplit, bsplit;
	ArrayCtrl cfs;
	ArrayCtrl stories;
	WithConceptualFramework<Ctrl> cf;
	DocEdit story_struct, story_improved;
	int story_sort_column = 0;
	
public:
	WithConceptualFrameworkStory<Ctrl> story;
	
public:
	typedef ConceptualFrameworkNavigator CLASSNAME;
	ConceptualFrameworkNavigator();
	
	void Data() override;
	void ToolMenu(Bar& bar) override;
	void DataAll(bool forced);
	void DataFramework();
	void DataStory();
	void OnValueChange();
	void Do(int fn);
	void MainLayout();
	void SideLayout();
	void LockForm();
	void MoveSortColumn(int fn);
	
	
	void GetElements(ConceptualFrameworkArgs& args);
	int64 GetBeliefUniq() const;
	
};

class BiographyPerspectiveCtrl : public ConceptualFrameworkNavigator {
	
public:
	typedef BiographyPerspectiveCtrl CLASSNAME;
	BiographyPerspectiveCtrl();
	
	void ToolMenu(Bar& bar) override;
	
	
};

INITIALIZE(BiographyPerspectiveCtrl)


END_UPP_NAMESPACE

#endif
 
