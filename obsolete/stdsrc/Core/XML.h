// Minimal XML node structure and serializer/parser (subset of XML)

struct XmlAttr { String key; String value; };

struct XmlNS { String prefix; String uri; };

struct XmlNode {
    String tag;
    String nsPrefix;
    String nsUri;
    Vector<XmlAttr> attrs;
    Vector<XmlNode> children;
    String text; // text content if any
    Vector<XmlNS> nsdecl; // namespace declarations effective on this node

    XmlNode() {}
    XmlNode(const String& t) : tag(t) {}

    XmlNode& Add(const String& t) { children.Add(XmlNode(t)); return children.Top(); }
    void Set(const String& t) { text = t; }
    void Attr(const String& k, const String& v) { XmlAttr a; a.key = k; a.value = v; attrs.Add(a); }
    void SetNamespace(const String& prefix, const String& uri) { nsPrefix = prefix; nsUri = uri; XmlNS d; d.prefix = prefix; d.uri = uri; nsdecl.Add(d); }

    static void Escape(String& out, const String& s) {
        for(int i=0;i<s.GetLength();++i){ char c = s[i];
            switch(c){ case '&': out.Cat("&amp;"); break; case '<': out.Cat("&lt;"); break; case '>': out.Cat("&gt;"); break; case '"': out.Cat("&quot;"); break; case '\'': out.Cat("&apos;"); break; default: out.Cat((int)c); }
        }
    }
    static String Escape(const String& s) { String out; Escape(out, s); return out; }
    static void Indent(String& out, int n) { for(int i=0;i<n;i++) out.Cat(' '); }

    void Dump(String& out, int indent = 0) const {
        Indent(out, indent);
        out.Cat('<'); if(!nsPrefix.IsEmpty()){ out.Cat(nsPrefix); out.Cat(':'); } out.Cat(tag);
        for(int i=0;i<nsdecl.GetCount();++i){ out.Cat(' '); out.Cat("xmlns"); if(!nsdecl[i].prefix.IsEmpty()){ out.Cat(':'); out.Cat(nsdecl[i].prefix); } out.Cat('='); out.Cat('"'); Escape(out, nsdecl[i].uri); out.Cat('"'); }
        for(int i=0;i<attrs.GetCount();++i){ out.Cat(' '); out.Cat(attrs[i].key); out.Cat('='); out.Cat('"'); Escape(out, attrs[i].value); out.Cat('"'); }
        if(children.IsEmpty() && text.IsEmpty()) { out.Cat("/>"); return; }
        out.Cat('>');
        if(!text.IsEmpty()) Escape(out, text);
        if(!children.IsEmpty()) { out.Cat('\n'); for(int i=0;i<children.GetCount();++i){ children[i].Dump(out, indent+2); out.Cat('\n'); } Indent(out, indent); }
        out.Cat("</"); out.Cat(tag); out.Cat('>');
    }
    String ToString() const { String s; Dump(s, 0); return s; }

    // Very simple parser for well-formed subset
    static void SkipWs(const char*& p) { while(*p && (unsigned char)*p <= ' ') ++p; }
    static String ReadName(const char*& p) { String n; while(*p && (std::isalnum((unsigned char)*p) || *p=='_'||*p=='-'||*p==':'||*p=='.')) n.Cat(*p++); return n; }
    static String ReadQuoted(const char*& p) { if(*p!='"'&&*p!='\'') return String(); char q=*p++; String v; while(*p && *p!=q){ if(*p=='&'){ // minimal entity decode
                if(std::strncmp(p, "&lt;", 4)==0){ v.Cat('<'); p+=4; }
                else if(std::strncmp(p, "&gt;", 4)==0){ v.Cat('>'); p+=4; }
                else if(std::strncmp(p, "&amp;", 5)==0){ v.Cat('&'); p+=5; }
                else if(std::strncmp(p, "&quot;", 6)==0){ v.Cat('"'); p+=6; }
                else if(std::strncmp(p, "&apos;", 6)==0){ v.Cat('\''); p+=6; }
                else { v.Cat(*p++); }
            } else v.Cat(*p++); }
        if(*p==q) ++p; return v; }
    static String ReadText(const char*& p) { String t; while(*p){ if(*p=='<' && p[1]=='!' && std::strncmp(p, "<![CDATA[", 9)==0){ p+=9; while(*p && !(p[0]==']'&&p[1]==']'&&p[2]=='>')) t.Cat(*p++); if(*p) p+=3; continue; } if(*p=='<') break; t.Cat(*p++); } return t; }

    static bool ParseNode(XmlNode& out, const char*& p) {
        SkipWs(p);
        if(*p != '<') return false; ++p;
        if(*p == '?'){ // skip prolog
            while(*p && !(*p=='?' && p[1]=='>')) ++p; if(*p) p+=2; SkipWs(p); if(*p!='<') return false; ++p;
        }
        if(*p == '!'){ // comments or doctype; skip
            if(std::strncmp(p, "!--", 3)==0){ p+=3; while(*p && !(p[0]=='-'&&p[1]=='-'&&p[2]=='>')) ++p; if(*p) p+=3; return ParseNode(out, p); }
            while(*p && *p!='>') ++p; if(*p) ++p; return ParseNode(out, p);
        }
        out.tag = ReadName(p);
        // handle prefix in tag
        int cp = out.tag.Find(':'); if(cp >= 0){ out.nsPrefix = out.tag.Left(cp); out.tag = out.tag.Mid(cp+1); }
        // attributes and xmlns declarations
        SkipWs(p);
        while(*p && *p!='>' && *p!='/') {
            String k = ReadName(p); SkipWs(p); if(*p!='=') return false; ++p; SkipWs(p); String v = ReadQuoted(p);
            if(k == "xmlns") { XmlNS d; d.prefix = String(); d.uri = v; out.nsdecl.Add(d); }
            else if(k.GetLength()>6 && k.Left(6) == "xmlns:") { XmlNS d; d.prefix = k.Mid(6); d.uri = v; out.nsdecl.Add(d); }
            else out.Attr(k, v);
            SkipWs(p);
        }
        if(*p == '/' && p[1] == '>') { p += 2; return true; }
        if(*p != '>') return false; ++p;
        // content
        SkipWs(p);
        if(*p == '<' && p[1] == '/') { p += 2; String end = ReadName(p); if(end != out.tag) return false; if(*p!='>') return false; ++p; return true; }
        // children or text
        if(*p == '<') {
            while(*p == '<' && p[1] != '/') { XmlNode ch; if(!ParseNode(ch, p)) return false; out.children.Add(ch); SkipWs(p); }
        } else {
            out.text = ReadText(p);
        }
        if(*p != '<' || p[1] != '/') return false; p += 2; String end = ReadName(p); if(end != out.tag) return false; if(*p!='>') return false; ++p; return true;
    }

    static XmlNode Parse(const String& s) { const char* p = s.Begin(); XmlNode root; ParseNode(root, p); return root; }
};
