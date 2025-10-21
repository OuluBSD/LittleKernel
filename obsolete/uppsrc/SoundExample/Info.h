#ifndef _portaudioTest_Info_h_
#define _portaudioTest_Info_h_

inline String ToSimpleString(String s) {
	WString ws = s.ToWString();
	String out;
	for(int i = 0; i < ws.GetCount(); i++) {
		int chr = ws[i];
		if (chr >= 0 && chr < 128)
			out.Cat(chr);
	}
	return out;
}

class AudioInfo:public WithInfoLayout<ParentCtrl>{
	typedef AudioInfo CLASSNAME;
public:
	AudioInfo(){
		CtrlLayout(*this);
		PopulateTree();
		tree.NoRoot().OpenDeep(0);
		tree.WhenSel=THISBACK(OnSel);
		OnSel();
	}
	void PopulateTree(){
		const SoundSystem& s=SoundSys();
		for(int i=0;i<s.GetAPICount();i++){
			SoundAPI a=s.GetAPI(i);
			tree.Add(0,SoundImg::API(),a.index,a.name,true);
		}
		for(int i=0;i<s.GetCount();i++){
			SoundDevice d=s[i];
			tree.Add(tree.Find(d.API),SoundImg::Device(),d.index,d.name,false);
		}
	}
	void OnSel(){
		if(tree.GetSel().GetCount()==0){
			info<<="[1 Select one of the items from the tree on the left...]";
			return;
		}
		int n=tree.GetSel()[0];
		String s;
		if(tree.GetParent(n)==0){
			SoundAPI a(tree.Get(n));
			s<<"[ [ "<< DeQtf(ToSimpleString(a.name)) <<"&][1 &][ {{1:1@NFNGN^ "
			   "[ [1 Default input:]]:: [ [1 "<<DeQtf(ToSimpleString(SoundDevice(a.defaultInputDevice).name))<<"]]:: "
			   "[ [1 Default output:]]:: [ [1 "<<DeQtf(ToSimpleString(SoundDevice(a.defaultOutputDevice).name))<<"]]:: "
			   "[ [1 Number of devices:]]:: [ [1 "<<a.deviceCount <<"]]"
			   "}}]";
		}else{
			SoundDevice d(tree.Get(n));
			s<<"[ [ "<<DeQtf(ToSimpleString(d.name))<<"&][1 &][ {{1:1@NFNGN^ "
			   "[ [1 API:]]:: [ [1 "<< DeQtf(ToSimpleString(SoundAPI(d.API).name))<<"]]:: "
			   "[ [1 Input Channels:]]:: [ [1 "<<d.InputChannels<<"]]:: "
			   "[ [1 Output Channels:]]:: [ [1 "<<d.OutputChannels<<"]]:: "
			   "[ [1 Default sample rate:]]:: [ [1 "<< d.SampleRate<<"]]:: "
			   "[ [1 Default input latency:]]:: [ [1 Low: "<<d.LowInputLatency<<" s&High: "<<d.HighInputLatency<<" s ]]:: "
			   "[ [1 Default output latency:]]:: [ [1 Low: "<<d.LowOutputLatency<<" s&High: "<<d.HighOutputLatency<<" s ]]"
			   "}}]";
		}
		info<<=s;
		LOG(s);
	}
};

#endif
