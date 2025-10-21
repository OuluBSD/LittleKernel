#include "LLDB.h"


LLDBDebuggerApp::LLDBDebuggerApp() {
	Title("LLDB Debugger App");
	Sizeable().MaximizeBox().MinimizeBox();
	Maximize();
	
	Add(hsplit.SizePos());
	
	hsplit.Horz() << lsplit << csplit << rsplit;
	hsplit.SetPos(1500,0);
	hsplit.SetPos(10000-3000,1);
	
	lsplit.Vert() << files;
	
	csplit.Vert() << ftabs << btabs;
	csplit.SetPos(10000-1800);
	
	CtrlLayout(console);
	btabs.Add(console.SizePos(), "Console");
	btabs.Add(normal.SizePos(), "Normal");
	btabs.Add(error.SizePos(), "Error");
	
	CtrlLayout(threads);
	rsplit.Vert() << threads << stack << loctabs << breaktabs << dbg_stream;
	
	stack.AddColumn("Function");
	stack.AddColumn("File");
	stack.AddColumn("Line");
	stack.AddIndex("IDX");
	stack.ColumnWidths("6 2 1");
	
	loctabs.Add(locals.SizePos(), "Locals");
	
	loctabs.Add(regs.SizePos(), "Registers");
	regs.AddColumn("Name");
	regs.AddColumn("Type");
	regs.AddColumn("Value");
	regs.AddIndex("IDX");
	regs.ColumnWidths("6 2 1");
	
	breaktabs.Add(breaks.SizePos(), "Breaks");
	breaks.AddColumn("File");
	breaks.AddColumn("Line");
	breaks.AddIndex("IDX");
	breaks.ColumnWidths("6 2");
	
	breaktabs.Add(watchs.SizePos(), "Waches");
	watchs.AddColumn("Variable");
	watchs.AddColumn("Value");
	watchs.AddIndex("IDX");
	watchs.ColumnWidths("6 2");
	
}

void LLDBDebuggerApp::Data() {
	
	// Locals
	{
		if (process.has_value() && process_is_stopped(*process)) {
            lldb::SBThread viewed_thread = process->GetThreadAtIndex(this->viewed_thread_index);
            lldb::SBFrame frame = viewed_thread.GetFrameAtIndex(this->viewed_frame_index);
            lldb::SBValueList locals = frame.GetVariables(true, true, true, true);

            // TODO: select entire row like in stack trace
            locals.Clear();
            for (int i = 0; i < locals.GetSize(); i++) {
                DataLocalRecursive(0, locals.GetValueAtIndex(i));
            }
        }
	}
	
	// Registers
	{
		if (process.has_value() && process_is_stopped(*process)) {
            lldb::SBThread viewed_thread = process->GetThreadAtIndex(this->viewed_thread_index);
            lldb::SBFrame frame = viewed_thread.GetFrameAtIndex(this->viewed_frame_index);
            if (viewed_thread.IsValid() && frame.IsValid()) {
                lldb::SBValueList register_collections = frame.GetRegisters();

                for (int i = 0; i < register_collections.GetSize(); i++) {
                    lldb::SBValue regcol = register_collections.GetValueAtIndex(i);

                    String collection_name = regcol.GetName();

                    if (!~collection_name) {
                        LOG("warning: Skipping over invalid/un-named register collection");
                        continue;
                    }

                    String reg_coll_name = Format("%s##RegisterCollection", collection_name);
                    if (ImGui::TreeNode(~reg_coll_name)) {
                        for (int i = 0; i < regcol.GetNumChildren(); i++) {
                            lldb::SBValue reg = regcol.GetChildAtIndex(i);
                            const char* reg_name = reg.GetName();
                            const char* reg_value = reg.GetValue();

                            if (!reg_name || !reg_value) {
                                LOG("warning: skipping invalid register");
                                continue;
                            }

                            ImGui::Text("%s = %s", reg_name, reg_value);
                        }

                        ImGui::TreePop();
                    }
                }
            }
        }
	}
	
}

void LLDBDebuggerApp::DataLocalRecursive(int parnode, lldb::SBValue local) {
	String local_type = local.GetDisplayTypeName();
    String local_name = local.GetName();
    String local_value = local.GetValue();

    if (!~local_type || !~local_name) {
        return;
    }

    String children_node_label = Format("%s - Children_%d", local_name, (int)local.GetID());

    if (local.MightHaveChildren()) {
        String str = children_node_label + ": " + local_type + " ...";
        int n = locals.Add(parnode, CtrlImg::Dir(), str);

        // TODO: figure out best way to handle very long children list
        for (int i = 0; i < local.GetNumChildren(100); i++) {
            DataLocalRecursive(n, local.GetChildAtIndex(i));
        }
    }
    else {
        String str = children_node_label + ": " + local_type + ": ";
        if (local_value.Is()) {
            children_node_label += local_value;
        }
        else {
            children_node_label += "unknown";
        }
        int n = locals.Add(parnode, CtrlImg::Dir(), str);
    }
}
