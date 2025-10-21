#include "CtrlLib.h"

namespace Upp {

ChatCtrl::Message& ChatCtrl::Message::SetUser(String s) {
	if (item) item->Text(s);
	return *this;
}

ChatCtrl::Message& ChatCtrl::Message::SetText(String qtf) {
	this->textctrl.SetQTF(qtf);
	return *this;
}

ChatCtrl::Message& ChatCtrl::Message::SetImage(Image img) {
	if (item) item->SetImage(img);
	return *this;
}

ChatCtrl::ChatCtrl() {
	
}

void ChatCtrl::Clear() {
	for (auto& it : messages)
		RemoveChild(&it.textctrl);
	PageCtrl::Clear();
	messages.Clear();
}

int ChatCtrl::GetMessageCount() const {
	return messages.GetCount();
}

ChatCtrl::Message& ChatCtrl::GetMessage(int i) {
	return messages[i];
}

ChatCtrl::Message& ChatCtrl::AddMessage() {
	Message& msg = messages.Add();
	auto& item = PageCtrl::Add(msg.textctrl, "");
	msg.item = &item;
	return msg;
}

ChatCtrl::Message& ChatCtrl::AddMessage(String user, String qtf) {
	Message& msg = messages.Add();
	msg.textctrl.SetQTF(qtf);
	auto& item = PageCtrl::Add(msg.textctrl, user);
	msg.item = &item;
	PostCallback([this,&msg,&item]{
		int w = GetTabWidth();
		int h = msg.textctrl.Get().GetHeight(w);
		item.Height(h);
		Layout();
	});
	return msg;
}

ChatCtrl::Message& ChatCtrl::AddMessage(String user, Image icon, String qtf) {
	Message& msg = messages.Add();
	msg.textctrl.SetQTF(qtf);
	auto& item = PageCtrl::Add(msg.textctrl, icon, user);
	msg.item = &item;
	PostCallback([this,&msg,&item]{
		int w = GetTabWidth();
		int h = msg.textctrl.Get().GetHeight(w);
		item.Height(h);
		Layout();
	});
	return msg;
}

}
