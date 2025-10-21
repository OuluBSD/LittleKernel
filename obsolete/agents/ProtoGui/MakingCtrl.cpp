#include "ProtoGui.h"


struct PageClasses {
	
	// Splits area horizontally in 2
	Splitter hsplit;
	
	// Splits left area vertically on 2-3
	Splitter lsplit; // left-split
	
	// Right area has a placeholder for dynamic content
	Ctrl rplaceholder;
	
	
	
	void Attach(Ctrl& c) {
		// Main splitter takes whole area
		c.Add(hsplit.SizePos());
		
		hsplit.Horz() << lsplit << rplaceholder;
		
		lsplit.Vert();
		
		
	}
};
