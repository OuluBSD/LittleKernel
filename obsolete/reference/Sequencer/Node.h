#ifndef _Sequencer_Node_h_
#define _Sequencer_Node_h_

NAMESPACE_UPP

class SessionEditor;
class TrackCtrl;
class TrackListCtrl;

class NodeParentCtrl : public ParentCtrl {
	
	
public:
	NodeParentCtrl();
	
	SessionEditor& GetEditor();
	TrackListCtrl& GetTrackList();
	
};

template <class T>
class LeftDownHook : public T {
	
public:
	virtual void LeftDown(Point p, dword keyflags) {
		WhenLeftDown();
	}
	Callback WhenLeftDown;
};


END_UPP_NAMESPACE

#endif
