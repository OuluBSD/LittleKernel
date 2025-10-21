#include <iostream>
#include <Core/Core.h>
using namespace Upp;

static int fails = 0;
static void Check(const char* name, bool ok) { if(!ok){ std::cout << "FAIL: " << name << "\n"; ++fails; } }

int main(){
    // JSON
    String src = "{\"a\":1,\"b\":[true,false,null,\"x\"]}";
    Json j = Json::Parse(src);
    Check("json parse", !j.IsNull() && j.IsObject());
    const Json* pc = j.Ptr("b[2]"); Check("json ptr", pc && pc->IsNull());
    bool setok = j.PointerSet("/b/1", Json(true)); Check("json pointer set", setok);
    bool remok = j.PointerRemove("/b/2"); Check("json pointer remove", remok);

    // XML
    XmlNode x("root"); x.Attr("k","v"); x.Add("c").Set("t"); String xml = x.ToString();
    XmlNode y = XmlNode::Parse(xml);
    String xml2 = y.ToString();
    Check("xml roundtrip", xml2.GetLength()>0);

    if(fails==0) std::cout << "OK\n";
    return fails ? 1 : 0;
}

