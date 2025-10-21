#include "SoundCtrl.h"


NAMESPACE_UPP


WaveformCtrl::WaveformCtrl() {
	background = White();
	foreground = Black();
}

void WaveformCtrl::Clear() {
	clip = 0;
}

void WaveformCtrl::Paint(Draw& d) {
	Size sz = GetSize();
	
	d.DrawRect(sz, background);
	
	if (clip) {
		int samplerate = clip->GetSampleRate();
		int begin_sample = (int)(samplerate * begin);
		int end_sample = (int)(samplerate * end);
		clip->GetValues(begin_sample, end_sample, values);
		
		if (values.GetCount() >= 2) {
			points.SetCount(values.GetCount());
			double* val = values.Begin();
			double step = sz.cx / (double)(values.GetCount()-2);
			double x = 0;
			int h = sz.cy;
			for (Point& pt : points) {
				pt.x = (int)(x + 0.5);
				pt.y = (int)(h * (1.0 - (*val++ + 1.0) * 0.5));
				x += step;
			}
			
			d.DrawPolyline(points, 1, foreground);
		}
		if (!clip->IsUpdating())
			tc.Kill();
	}
	else
		tc.Kill();
	
}

void WaveformCtrl::SetClip(const SoundClipBase& clip) {
	this->clip = clip.Clone();
	Update();
	end = duration;
	if (clip.IsUpdating())
		tc.Set(-100, [this]{end = this->clip ? this->clip->GetDuration() : end; Refresh();});
}

void WaveformCtrl::Update() {
	if (clip) {
		duration = clip->GetDuration();
		if (end == 0)
			end = duration;
	}
	else {
		duration = begin = end = 0;
	}
}

WaveformCtrl& WaveformCtrl::SetBackground(Color c) {
	background = c;
	return *this;
}

WaveformCtrl& WaveformCtrl::SetForeground(Color c) {
	foreground = c;
	return *this;
}


END_UPP_NAMESPACE
