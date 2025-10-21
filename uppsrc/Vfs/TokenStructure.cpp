#include "Vfs.h"

NAMESPACE_UPP

TokenNode::TokenNode(VfsValue& v) :
	VfsValueExt(v)
{
	
}

void TokenNode::Clear() {
	val.sub.Clear();
}

TokenNode& TokenNode::Add() {
	TokenNode& s = val.Add<TokenNode>();
	ASSERT(s.val.owner);
	return s;
}

String TokenNode::GetTreeString(int indent) const {
	String s;
	s.Cat('\t', indent);
	s << ToString() << "\n";
	for (const TokenNode& n : val.Sub<TokenNode>()) {
		s << n.GetTreeString(indent+1);
	}
	return s;
}

String TokenNode::GetCodeString(const CodeArgs2& args) const {
	TODO return String();
}

String TokenNode::ToString() const {
	if (begin) {
		String s;
		int i = 0;
		const Token* iter = begin;
		while (iter != end) {
			if (i++ > 0) s.Cat(' ');
			s.Cat(iter->GetTextValue());
			iter++;
		}
		return s;
	}
	else return "<no token>";
}











TokenStructure::TokenStructure(VfsValue& v) :
	VfsValueExt(v),
	ErrorSource("TokenStructure")
{
	
}

TokenNode& TokenStructure::GetRoot() const {
	return const_cast<VfsValue&>(val).GetAdd<TokenNode>("root");
}

bool TokenStructure::ProcessEon(const Tokenizer& t) {
	auto& root = GetRoot();
	root.val.sub.Clear();
	
	const Vector<Token>& tokens = t.GetTokens();
	
	iter = tokens.Begin();
	end = tokens.End();
	
	return ParseBlock(root);
}

bool TokenStructure::ParseBlock(TokenNode& n) {
	while (!IsEnd()) {
		if (Current().IsType(TK_DEDENT))
			break;
		
		if (Current().IsType(TK_INDENT)) {
			if (!PassType(TK_INDENT))
				return false;
			
			ParseBlock(n);
			
			if (!PassType(TK_DEDENT))
				return false;
		}
		else {
			TokenNode& s = n.Add();
			if (!ParseStatement(s, false))
				return false;
		}
	}
	return true;
}

bool TokenStructure::ParseStatement(TokenNode& n, bool break_comma) {
	ASSERT(!IsEnd());
	n.begin = &Current();
	ASSERT(!n.begin->IsType(':'));
	ASSERT(!n.begin->IsType(TK_INDENT));
	bool has_block = false;
	
	while (!IsEnd()) {
		const Token& t = Current();
		n.end = &t;
		
		if (t.IsType(':')) {
			Next();
			has_block = true;
			break;
		}
		else if (t.IsType(TK_END_STMT)) {
			Next();
			break;
		}
		else if (t.IsType(TK_EOF)) {
			Next();
			break;
		}
		else if (t.IsType(TK_DEDENT))
			break;
		
		Next();
		
		if (break_comma && Current().IsType(','))
			break;
	}
	
	if (has_block) {
		if (Current().IsType(TK_INDENT)) {
			if (!PassType(TK_INDENT))
				return false;
			
			ParseBlock(n);
			
			if (!PassType(TK_DEDENT))
				return false;
		}
		else {
			TokenNode& s = n.Add();
			if (!ParseStatement(s, true))
				return false;
			
			while (Current().IsType(',')) {
				Next();
				if (!ParseStatement(s, true))
					return false;
			}
			
		}
	}
	
	return true;
}

bool TokenStructure::PassType(int tk) {
	const Token& t = Current();
	if (t.IsType(tk)) {
		Next();
		return true;
	}
	AddError(t.loc, "expected '" + Token::GetTypeStringStatic(tk) + "', but got '" + t.GetTypeString() + "'");
	return false;
}

String TokenStructure::GetTreeString(int indent) const {
	auto& root = GetRoot();
	return root.GetTreeString();
}

String TokenStructure::GetCodeString(const CodeArgs2& args) const {
	auto& root = GetRoot();
	return root.GetCodeString(args);
}

String TokenStructure::ToString() const {
	auto& root = GetRoot();
	return root.ToString();
}



END_UPP_NAMESPACE
