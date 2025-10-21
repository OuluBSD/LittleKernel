#include "AgentDemo.h"

NAMESPACE_UPP

#if 0

1. Introduction

	Here’s a “File-System World” toy example that shows off creation, writing,
	moving and deletion of files and directories.

    Atoms (predicates)
		• dir_exists(Dir)
		• file_exists(F)
		• file_closed(F) (i.e. not open)
		• file_open(F)
		• file_mode(F,Mode) (Mode∈{read,write})
		• file_has_content(F,Content)

2. Actions

    CreateDir(D,Parent)
    Pre: dir_exists(Parent), ¬dir_exists(D)
    Post: dir_exists(D)

    CreateFile(F,Dir)
    Pre: dir_exists(Dir), ¬file_exists(F)
    Post: file_exists(F), file_closed(F), file_has_content(F,"")

    (Here F is a full path like “/home/admin/docs/readme.txt.”)

    OpenFile(F,Mode)
    Pre: file_exists(F), file_closed(F)
    Post: file_open(F), file_mode(F,Mode), ¬file_closed(F)

    WriteFile(F,Content)
    Pre: file_open(F), file_mode(F,write)
    Post: file_has_content(F,Content)

    CloseFile(F)
    Pre: file_open(F)
    Post: file_closed(F), ¬file_open(F), ¬file_mode(F,read), ¬file_mode(F,write)

    MoveFile(F,OldDir,NewDir)
    Pre: file_exists(OldPath), dir_exists(NewDir), file_closed(OldPath)
    where OldPath = OldDir / Name and NewPath = NewDir / Name
    Post: ¬file_exists(OldPath), file_exists(NewPath)

    DeleteDir(D)
    Pre: dir_exists(D)
    Post: ¬dir_exists(D)

3. Initial WorldState
	
	dir_exists("/")
	dir_exists("/home")
	dir_exists("/home/admin")
	(No files exist yet; no file_open or file_has_content atoms present.)

4. Goal
	
	file_exists("/home/readme.txt")
	file_has_content("/home/readme.txt","Hello")
	¬dir_exists("/home/admin/docs")

5. Story
	
	“As the sys-admin’s planner, you start with only “/”, “/home” and “/home/admin” in place.
	Your task is:
	
	    Create a subfolder “docs” under “/home/admin.”
	    Inside it, create and open a file “readme.txt,” write the text “Hello,” then close it.
	    Move that file up into “/home,” and finally delete the now-empty “docs” folder.
		By the end you must have “/home/readme.txt” containing “Hello,” and no more “/home/admin/docs.””
	
	This mini-domain exercises directory/file creation, opening, writing, moving and deletion—all via clear pre- and post-conditions.


6. Result

	List of actions:
	1. CreateDir("/home/admin/docs","/home/admin")
	2. OpenFile("/home/readme.txt","write")
	3. WriteFile("/home/readme.txt","Hello")
	4. CloseFile("/home/readme.txt")
	5. MoveFile("/home/readme.txt","/home/admin","/home")
	6. DeleteDir("/home/admin/docs")

#endif


void FileSystemExample() {
	using namespace UPP;
	VfsValue& app_root = MetaEnv().root;
	SearcherExt& searcher = app_root.GetAdd<SearcherExt>("searcher");
	searcher.SetVerbose();
	
	VfsValue& fs			= searcher.GetFS();
	CommitTreeExt& tree		= searcher.GetCommitTree();
	CommitDiffListExt& list	= searcher.GetCommitDiffList();
	
	ValueMap atoms;
	atoms.Add("dir_exists(D)",    false);
	atoms.Add("file_exists(F)",    false);
	atoms.Add("file_closed(F)",    false);
	atoms.Add("file_open(F)",    false);
	atoms.Add("file_mode(F,Mode)",    false);
	atoms.Add("file_has_content(F,Content)",    false);
	
	ValueMap goal;
	goal.Add("file_exists(\"/home/readme.txt\")", true);
	goal.Add("file_has_content(\"/home/readme.txt\",\"Hello\")", true);
	goal.Add("dir_exists(\"/home/admin/docs\")", false);
	
	ValueMap actions;
	actions.Add("CreateDir(D,Parent)", ActionEventValue()
		.Pre("dir_exists(Parent)", true)
		.Pre("dir_exists(D)", false)
		.Post("dir_exists(D)", true)
		);
	actions.Add("CreateFile(F,Dir)", ActionEventValue()
		.Pre("dir_exists(Dir)", true)
		.Pre("file_exists(F)", false)
		.Post("file_exists(F)", true)
		.Post("file_closed(F)", true)
		.Post("file_has_content(F,\"\")", true)
		);
	actions.Add("OpenFile(F,Mode)", ActionEventValue()
		.Pre("file_exists(F)", true)
		.Pre("file_closed(F)", true)
		.Post("file_open(F)", true)
		.Post("file_mode(F,Mode)", true)
		.Post("file_closed(F)", false)
		);
	actions.Add("WriteFile(F,Content)", ActionEventValue()
		.Pre("file_open(F)", true)
		.Pre("file_mode(F,write)", true)
		.Post("file_has_content(F,Content)", true)
		);
	actions.Add("CloseFile(F)", ActionEventValue()
		.Pre("file_open(F)", true)
		.Post("file_closed(F)", true)
		.Post("file_open(F)", false)
		.Post("file_mode(F,read)", false)
		.Post("file_mode(F,write)", false)
		);
	actions.Add("MoveFile(F,OldDir,NewDir)", ActionEventValue()
		.Pre("file_exists(OldPath)", true)
		.Pre("dir_exists(NewDir)", true)
		.Pre("file_closed(OldPath)", true)
		.Post("file_exists(OldPath)", false)
		.Post("file_exists(NewPath)", true)
		);
	actions.Add("DeleteDir(D)", ActionEventValue()
		.Pre("dir_exists(D)", true)
		.Post("dir_exists(D)", false)
		);
	
	ValueMap params;
	params("atoms") = atoms;
	params("actions") = actions;
	params("goal") = goal;
	params("dump_intermediate_trees") = false;
	params("use_params") = true;
	params("use_resolver") = true;
	searcher.SetSearcherParams(params);
	searcher.SetGeneratorParams(params, Null);
	searcher.SetSearchStrategy(SEARCHSTRATEGY_ASTAR);
	searcher.SetTerminalTest(TERMTEST_ACTION_PLANNER);
	searcher.SetGenerator(GENERATOR_ACTION_PLANNER);
	searcher.SetHeuristics(HEURISTIC_ACTION_PLANNER);
	searcher.ClearFS();
	searcher.WhenGenerated.Clear();
	searcher.WhenError = [](String s) {LOG("ActionPlanner error: " << s);};
	bool ret = searcher.RunSearch();
	if (!ret) {
		LOG(searcher.GetFS().GetTreeString());
	}
	ASSERT(ret);
	LOG(searcher.GetResultString());
}


END_UPP_NAMESPACE
