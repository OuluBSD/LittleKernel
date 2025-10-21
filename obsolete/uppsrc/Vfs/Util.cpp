#include "Vfs.h"

NAMESPACE_UPP





void PathIdentifier::Clear() {
	memset(this, 0, sizeof(PathIdentifier));
}

String PathIdentifier::ToString() const {
	if (!begin || begin == end)
		return String();
	String s;
	const Token* iter = begin;
	const bool* is_meta = &this->is_meta[0];
	while (iter != end) {
		if (iter->type == TK_ID) {
			if (*is_meta)
				s.Cat('$');
			is_meta++;
			s.Cat(iter->GetTextValue());
		}
		else if (iter->type == '$')
			; // pass
		else
			s.Cat(iter->GetTextValue());
		iter++;
	}
	return s;
}

bool PathIdentifier::IsEmpty() const {
	return part_count == 0;
}

bool PathIdentifier::HasMeta() const {
	for(int i = 0; i < part_count; i++)
		if (is_meta[i])
			return true;
	return false;
}

bool PathIdentifier::HasPartialMeta() const {
	for(int i = 0; i < part_count; i++)
		if (is_meta[i])
			return i > 0; // if first is meta, then it's full, not partial
	return false;
}




String GetCodeCursorString(CodeCursor t) {
	switch (t) {
		case Cursor_Op_INC: return "increase";
		case Cursor_Op_DEC: return "decrease";
		case Cursor_Op_POSTINC: return "post-increase";
		case Cursor_Op_POSTDEC: return "post-decrease";
		case Cursor_Op_NEGATIVE: return "negative";
		case Cursor_Op_POSITIVE: return "positive";
		case Cursor_Op_NOT: return "not";
		case Cursor_Op_NEGATE: return "negate";
		case Cursor_Op_ADD: return "add";
		case Cursor_Op_SUB: return "subtract";
		case Cursor_Op_MUL: return "multiply";
		case Cursor_Op_DIV: return "divide";
		case Cursor_Op_MOD: return "modulus";
		case Cursor_Op_LSH: return "left-shift";
		case Cursor_Op_RSH: return "right-shift";
		case Cursor_Op_GREQ: return "greater-or-equal";
		case Cursor_Op_LSEQ: return "less-or-equal";
		case Cursor_Op_GREATER: return "greater";
		case Cursor_Op_LESS: return "less";
		case Cursor_Op_EQ: return "equal";
		case Cursor_Op_INEQ: return "inequal";
		case Cursor_Op_BWAND: return "bitwise-and";
		case Cursor_Op_BWXOR: return "bitwise-xor";
		case Cursor_Op_BWOR: return "bitwise-or";
		case Cursor_Op_AND: return "and";
		case Cursor_Op_OR: return "op";
		case Cursor_Op_COND: return "conditional";
		case Cursor_Op_ASSIGN: return "assign";
		case Cursor_Op_ADDASS: return "add-and-assign";
		case Cursor_Op_SUBASS: return "subtract-and-assign";
		case Cursor_Op_MULASS: return "multiply-and-assign";
		case Cursor_Op_DIVASS: return "divide-and-assign";
		case Cursor_Op_MODASS: return "modulus-and-assign";
		case Cursor_Op_CALL: return "call";
		case Cursor_Op_SUBSCRIPT: return "subscript";
		default: break;
	}
	
	switch (t) {
		case Cursor_Literal_BOOL:	return "bool";
		case Cursor_Literal_INT32:	return "int32";
		case Cursor_Literal_INT64:	return "int64";
		case Cursor_Literal_DOUBLE:	return "double";
		case Cursor_Literal_STRING:	return "string";
		default: break;
	}
	switch (t) {
		#define CURSOR(a,b,c) case Cursor_##a: return #a;
		CURSOR_LIST
		#undef CURSOR
		default: return "<error>";
	}
}


String GetOpCodeString(CodeCursor t) {
	switch (t) {
		case Cursor_Op_INC: return "++";
		case Cursor_Op_DEC: return "--";
		case Cursor_Op_POSTINC: return "++";
		case Cursor_Op_POSTDEC: return "--";
		case Cursor_Op_NEGATIVE: return "-";
		case Cursor_Op_POSITIVE: return "+";
		case Cursor_Op_NOT: return "!";
		case Cursor_Op_NEGATE: return "~";
		case Cursor_Op_ADD: return "+";
		case Cursor_Op_SUB: return "-";
		case Cursor_Op_MUL: return "*";
		case Cursor_Op_DIV: return "/";
		case Cursor_Op_MOD: return "%";
		case Cursor_Op_LSH: return "<<";
		case Cursor_Op_RSH: return ">>";
		case Cursor_Op_GREQ: return ">=";
		case Cursor_Op_LSEQ: return "<=";
		case Cursor_Op_GREATER: return ">";
		case Cursor_Op_LESS: return "<";
		case Cursor_Op_EQ: return "==";
		case Cursor_Op_INEQ: return "!=";
		case Cursor_Op_BWAND: return "&";
		case Cursor_Op_BWXOR: return "^";
		case Cursor_Op_BWOR: return "|";
		case Cursor_Op_AND: return "&&";
		case Cursor_Op_OR: return "||";
		case Cursor_Op_COND: return "?:";
		case Cursor_Op_ASSIGN: return "=";
		case Cursor_Op_ADDASS: return "+=";
		case Cursor_Op_SUBASS: return "-=";
		case Cursor_Op_MULASS: return "*=";
		case Cursor_Op_DIVASS: return "/=";
		case Cursor_Op_MODASS: return "%=";
		default: return "<invalid>";
	}
}


bool IsTypedNode(CodeCursor src) {
	return IsPartially(src, Cursor_TypeDecl);
}

bool IsMetaTypedNode(CodeCursor src) {
	return IsPartially(src, Cursor_MetaTypeDecl);
}

bool IsRvalReturn(CodeCursor src) {
	return IsPartially(src, Cursor_WithRvalReturn);
}


END_UPP_NAMESPACE
