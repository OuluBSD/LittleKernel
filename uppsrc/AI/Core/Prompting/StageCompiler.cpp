#include "Prompting.h"

NAMESPACE_UPP

FarStageCompiler::FarStageCompiler() {
	
}

bool FarStageCompiler::Compile(Val& stage_node) {
	bool succ = true;
	msgs.Clear();
	this->stage.Clear();
	this->stage.Create();
	
	VfsFarStage* s = stage_node.FindExt<VfsFarStage>();
	if (!s)
		return false;
	String txt = s->code;
	txt.Replace("\r", "");
	
	
	struct Range : Moveable<Range> {
		enum {
			BODY,
			DEFINE,
			SYSTEM,
			MODEL,
		};
		int type = -1;
		Point pt;
		String header;
	};
	// Split ranges with separator '#define'
	Vector<String> lines = Split(txt, "\n");
	int line_i = -1;
	Vector<Range> ranges;
	Range* r = 0;
	if (TrimLeft(txt).Left(1) != "#") {
		r = &ranges.Add();
		r->type = Range::BODY;
		r->pt.x = 0;
	}
	for (String& line : lines) {
		line_i++;
		try {
			CParser p(line);
			if (p.Char('#')) {
				if (p.Id("define")) {
					if (r)
						r->pt.y = line_i;
					r = &ranges.Add();
					r->type = Range::DEFINE;
					r->pt.x = line_i+1;
					r->header = TrimBoth(p.GetPtr());
				}
				else if (p.Id("pragma")) {
					int type;
					if (p.Id("system"))      type = Range::SYSTEM;
					else if (p.Id("model"))  type = Range::MODEL;
					else if (p.Id("body"))   type = Range::BODY;
					else
						p.ThrowError("unexpected pragma");
					if (r)
						r->pt.y = line_i;
					r = &ranges.Add();
					r->type = type;
					r->pt.x = line_i+1;
					r->header = TrimBoth(p.GetPtr());
				}
				else p.ThrowError("unexpected section");
			}
		}
		catch (Exc e) {
			ProcMsg& m = msgs.Add();
			m.severity = PROCMSG_ERROR;
			m.msg = e;
			succ = false;
		}
	}
	r->pt.y = lines.GetCount();
	
	//DUMPM(ranges);
	
	for(int i = 0; i < ranges.GetCount(); i++) {
		auto& range = ranges[i];
		String body;
		for(int i = range.pt.x, j = 0; i < range.pt.y; i++, j++) {
			if (j)
				body += "\n";
			body += lines[i];
		}
		Value value;
		if (range.type == Range::BODY ||
			range.type == Range::DEFINE) {
			value = ParseJSON(body, false);
			if (value.IsError()) {
				LOG(body);
				ProcMsg& m = msgs.Add();
				m.severity = PROCMSG_ERROR;
				m.msg = "(line offset " + IntStr(range.pt.x) + "): " + GetErrorText(value);
				succ = false;
				continue;
			}
		}
		
		if (range.type == Range::BODY) {
			if (stage->body.GetCount()) {
				ProcMsg& m = msgs.Add();
				m.severity = PROCMSG_ERROR;
				m.msg = "(" + IntStr(range.pt.x) + "): duplicate body";
				succ = false;
			}
			stage->body = body;
			stage->value = value;
		}
		else if (range.type == Range::MODEL) {
			stage->model_name = range.header;
		}
		else if (range.type == Range::SYSTEM) {
			if (stage->system.GetCount()) {
				ProcMsg& m = msgs.Add();
				m.severity = PROCMSG_ERROR;
				m.msg = "Multiple system definitions";
				succ = false;
				continue;
			}
			stage->system = body;
		}
		else if (range.type == Range::DEFINE) {
			auto& fn = this->stage->funcs.Add();
			fn.body = body;
			fn.value = value;
			CParser p(range.header);
			try {
				fn.name = p.ReadId();
				p.PassChar('(');
				while (!p.Char(')')) {
					if (!fn.params.IsEmpty())
						p.PassChar(',');
					fn.params.Add(p.ReadString());
				}
				p.PassChar2('-','>');
				fn.ret = p.ReadString();
			}
			catch (Exc e) {
				ProcMsg& m = msgs.Add();
				m.severity = PROCMSG_ERROR;
				m.msg = e;
				succ = false;
			}
			
			// Create declaration for Esc script
			fn.esc_declaration = fn.name + "(";
			int param_i = 0;
			for (const auto& param : fn.params) {
				VfsPath p;
				p.SetPosixPath(param);
				if (p.IsEmpty()) {
					ProcMsg& m = msgs.Add();
					m.severity = PROCMSG_ERROR;
					m.msg = "invalid param in function " + fn.name;
					return false;
				}
				if (param_i)
					fn.esc_declaration.Cat(',');
				fn.esc_declaration += p.TopPart().ToString();
				param_i++;
			}
			fn.esc_declaration += ")";
			
			// Create hash by Visitor (TODO cleanup ugliness)
			fn.hash = 0;
			Visitor v((hash_t)0);
			v VISN(fn);
			fn.hash = v.hash;
		}
		else {
			ProcMsg& m = msgs.Add();
			m.severity = PROCMSG_ERROR;
			m.msg = "Internal error";
			succ = false;
			break;
		}
	}
	
	// Create hash by Visitor (TODO cleanup ugliness)
	stage->hash = 0;
	Visitor v((hash_t)0);
	v VISN(*stage);
	stage->hash = v.hash;
	
	return succ;
}

END_UPP_NAMESPACE
