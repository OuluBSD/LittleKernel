#ifndef _SoundCtrl_SoundCtrl_h_
#define _SoundCtrl_SoundCtrl_h_

#include <CtrlLib/CtrlLib.h>
#include <Sound/Sound.h>


NAMESPACE_UPP


class WaveformCtrl : public Ctrl {
	Color background, foreground;
	One<SoundClipBase> clip;
	Vector<double> values;
	Vector<Point> points;
	double duration = 0;
	double begin = 0, end = 0;
	TimeCallback tc;
	
public:
	typedef WaveformCtrl CLASSNAME;
	WaveformCtrl();
	
	void Clear();
	void SetClip(const SoundClipBase& clip);
	void Update();
	void Paint(Draw& d) override;
	WaveformCtrl& SetBackground(Color c);
	WaveformCtrl& SetForeground(Color c);
};


END_UPP_NAMESPACE


#endif
