#include "Media.h"
#ifdef flagGUI
#include <CtrlCore/CtrlCore.h>
#endif

NAMESPACE_UPP

// ImageDraw class requires CtrlCore
#ifdef flagGUI

AspectFixer::AspectFixer() {
	save_debug_images = true;
}

int AspectFixer::GetPhaseCount() const {
	return PHASE_COUNT;
}

void AspectFixer::DoPhase() {
	PNGEncoder enc;
	
	if (phase == PHASE_ANALYZE_PROMPT) {
		WhenIntermediate();
		
		Image img = src_image;
		Size sz = img.GetSize();
		int max_side = max(sz.cx, sz.cy);
		double ratio = 1024.0 / max_side;
		if (ratio < 1.0) {
			sz *= ratio;
			img = Rescale(img, sz);
		}
		
		JPGEncoder enc;
		String jpeg = enc.SaveString(img);
		
		VisionArgs args;
		SetWaiting(1);
		TaskMgr& m = AiTaskManager();
		m.GetVision(jpeg, args, [this](String res) {
			prompt = TrimBoth(res);
			
			int a = prompt.Find(". ");
			if (a >= 0)
				prompt = prompt.Left(a);
			
			NextPhase();
			SetWaiting(0);
			
			WhenIntermediate();
		});
	}
	else if (phase == PHASE_GET_SAFE_PROMPT) {
		String completion;
		completion = "Original prompt, which may violate dalle-2 content policy (e.g. can't use word \"intimate\"):\n"
			+ prompt + "\n\nImproved prompt, which doesn't violate the dalle-2 content policy:\n";
		

		SetWaiting(1);
		TaskMgr& m = AiTaskManager();
		m.RawCompletion(completion, [this](String res) {
			safe_prompt = TrimBoth(res);
			
			NextPhase();
			SetWaiting(0);
			
			WhenIntermediate();
		});
	}
	else if (phase == PHASE_GET_FILLERS) {
		if (batch == 0 && sub_batch == 0) {
			tasks.Clear();
			intermediate = src_image;
			
			Size sz = intermediate.GetSize();
			
			// Resize original image to maximum
			if (sz.cx >= sz.cy && sz.cy < 1024) {
				double resize_ratio = 1024.0 / sz.cy;
				Size new_sz = sz * resize_ratio;
				intermediate = Rescale(intermediate, new_sz);
				sz = new_sz;
			}
			else if (sz.cy >= sz.cx && sz.cx < 1024) {
				double resize_ratio = 1024.0 / sz.cx;
				Size new_sz = sz * resize_ratio;
				intermediate = Rescale(intermediate, new_sz);
				sz = new_sz;
			}
			
			src_intermediate = intermediate;
			
			double new_aspect = (double)w / (double)h;
			double cur_aspect = (double)sz.cx / (double)sz.cy;
			
			// Only to one side currently (left)
			if (w_extra > 0) {
				double aspect = new_aspect / cur_aspect;
				Task& t = tasks.Add();
				t.ratio_mul = w_extra * 0.01 + 1.0;
				t.side = LEFT;
				cur_aspect *= t.ratio_mul;
			}
			// Only to one side currently (top)
			if (h_extra > 0) {
				double aspect = new_aspect / cur_aspect;
				Task& t = tasks.Add();
				t.ratio_mul = 1 / (h_extra * 0.01 + 1.0);
				t.side = TOP;
				cur_aspect *= t.ratio_mul;
			}
			
			// Wider
			if (new_aspect > cur_aspect) {
				// Left half
				if (1) {
					double aspect = new_aspect / cur_aspect;
					Task& t = tasks.Add();
					t.ratio_mul = (aspect - 1.0) / 2.0 + 1.0;
					t.side = LEFT;
					cur_aspect *= t.ratio_mul;
				}
				// Right half
				if (1) {
					double aspect = new_aspect / cur_aspect;
					Task& t = tasks.Add();
					t.ratio_mul = aspect;
					t.side = RIGHT;
					cur_aspect *= t.ratio_mul;
				}
			}
			else if (new_aspect < cur_aspect) {
				// Top half
				if (1) {
					double aspect = new_aspect / cur_aspect;
					Task& t = tasks.Add();
					t.ratio_mul = (aspect - 1.0) / 2.0 + 1.0;
					t.side = TOP;
					cur_aspect *= t.ratio_mul;
				}
				// Bottom half
				if (1) {
					double aspect = new_aspect / cur_aspect;
					Task& t = tasks.Add();
					t.ratio_mul = aspect;
					t.side = BOTTOM;
					cur_aspect *= t.ratio_mul;
				}
			}
			
			
			intermediate = src_intermediate;
			WhenIntermediate();
		}
		
		
		if (batch >= tasks.GetCount()) {
			NextPhase();
			return;
		}
		Task& t = tasks[batch];
		
		// Image and mask
		Size sz = intermediate.GetSize();
		src_rect = Rect(0,0,0,0);
		dst_rect = Size(1024,1024);
		Image src_resized;
		if (t.side == LEFT || t.side == RIGHT) {
			double ratio = (double)dst_rect.Height() / sz.cy;
			sz *= ratio;
			src_resized = Rescale(intermediate, sz);
			src_rect = sz;
			int w_diff = (int)(sz.cx * (t.ratio_mul - 1.0));
			ASSERT(w_diff > 0);
			int w_src_visible = dst_rect.Width() - w_diff;
			int w_src_cut = max(0, w_src_visible - sz.cx);
			int w_src_visible_after_cut = w_src_visible - w_src_cut;
			if (t.side == LEFT) {
				src_rect &= RectC(0,0,w_src_visible_after_cut,sz.cy);
				dst_rect &= RectC(dst_rect.Width()-w_src_visible,0,w_src_visible_after_cut,sz.cy);
			}
			else {
				src_rect &= RectC(sz.cx-w_src_visible,0,w_src_visible_after_cut,sz.cy);
				dst_rect &= RectC(0,0,w_src_visible_after_cut,sz.cy);
			}
		}
		else if (t.side == TOP || t.side == BOTTOM) {
			double ratio = (double)dst_rect.Width() / sz.cx;
			sz *= ratio;
			src_resized = Rescale(intermediate, sz);
			src_rect = sz;
			double h_ratio = 1.0 / t.ratio_mul;
			int h_diff = (int)(sz.cy * (h_ratio - 1.0));
			ASSERT(h_diff > 0);
			int h_src_visible = dst_rect.Height() - h_diff;
			int h_src_cut = max(0, h_src_visible - sz.cy);
			int h_src_visible_after_cut = h_src_visible - h_src_cut;
			if (t.side == TOP) {
				src_rect &= RectC(0,0,sz.cx,h_src_visible_after_cut);
				dst_rect &= RectC(0,dst_rect.Height()-h_src_visible,sz.cx,h_src_visible_after_cut);
			}
			else {
				src_rect &= RectC(0,sz.cx-h_src_visible,sz.cx,h_src_visible_after_cut);
				dst_rect &= RectC(0,0,sz.cx,h_src_visible_after_cut);
			}
		}
		
		Image img, mask;
		
		#ifdef flagGUI
		// Create standard dalle image
		ImageDraw id(1024,1024);
		id.DrawRect(0,0,1024,1024,Black());
		id.DrawImage(dst_rect, src_resized, src_rect);
		img = id;
		
		// Create mask
		ImageDraw md(1024,1024);
		md.Alpha().DrawRect(0,0,1024,1024,Black());
		md.Alpha().DrawRect(dst_rect, White());
		mask = md;
		#else
		TODO
		#endif
		
		// Store debug image
		if (save_debug_images)
			enc.SaveFile(ConfigFile("debug-input-" + IntStr(batch) + ".png"), img);
		
		

		SetWaiting(1);
		TaskMgr& m = AiTaskManager();
		m.GetEditImage(img, MakeMask(mask), safe_prompt, 1, [this](Array<Image>& res) {
			PNGEncoder enc;
			
			if (res.GetCount()) {
				Image img = res[0];
				Size src_sz = img.GetSize();
				Size im_sz = intermediate.GetSize();
				
				if (save_debug_images)
					enc.SaveFile(ConfigFile("debug-result-" +IntStr(batch) + ".png"), img);
				
				// Make the new image
				const Task& t = tasks[batch];
				if (t.side == LEFT || t.side == RIGHT) {
					double ratio = (double)im_sz.cy / src_sz.cy;
					src_sz *= ratio;
					img = Rescale(img, src_sz);
					Size new_sz = Size((int)(im_sz.cx * t.ratio_mul), im_sz.cy);
					int w_diff = new_sz.cx - im_sz.cx;
					ASSERT(w_diff > 0);
					ImageDraw id(new_sz);
					if (t.side == LEFT) {
						id.DrawImage(0, 0, img);
						id.DrawImage(w_diff, 0, intermediate);
					}
					else {
						id.DrawImage(new_sz.cx-src_sz.cx, 0, img);
						id.DrawImage(0, 0, intermediate);
					}
					intermediate = id;
				}
				else if (t.side == TOP || t.side == BOTTOM) {
					double ratio = (double)im_sz.cx / src_sz.cx;
					src_sz *= ratio;
					img = Rescale(img, src_sz);
					double h_ratio = 1.0 / t.ratio_mul;
					Size new_sz = Size(im_sz.cx, (int)(im_sz.cy * h_ratio));
					int h_diff = new_sz.cy - im_sz.cy;
					ASSERT(h_diff > 0);
					ImageDraw id(new_sz);
					if (t.side == TOP) {
						id.DrawImage(0, 0, img);
						id.DrawImage(0, h_diff, intermediate);
					}
					else {
						id.DrawImage(0, new_sz.cy-src_sz.cy, img);
						id.DrawImage(0, 0, intermediate);
					}
					intermediate = id;
				}
				
				
				if (save_debug_images)
					enc.SaveFile(ConfigFile("debug-intermediate-" +IntStr(batch) + ".png"), intermediate);
				
				WhenIntermediate();
			}
			
			// Finally make the result image
			if (batch == tasks.GetCount() - 1 && !intermediate.IsEmpty()) {
				intermediate = Rescale(intermediate, Size(w,h));
				result = intermediate;
				
				if (save_debug_images)
					enc.SaveFile(ConfigFile("debug-result.png"), result);
				
				WhenIntermediate();
			}
			
			NextBatch();
			SetWaiting(0);
		});
	}
	
}

Image AspectFixer::MakeMask(const Image& image) {
	Color brush_clr = White();
	Size sz = image.GetSize();
	RGBA light, dark;
	light.r = brush_clr.GetR();
	light.g = brush_clr.GetG();
	light.b = brush_clr.GetB();
	light.a = 255;
	dark.r = 0;
	dark.g = 0;
	dark.b = 0;
	dark.a = 0;
	
	ImageBuffer ib(sz);
	const RGBA* src = image.Begin();
	RGBA* it = ib.Begin();
	RGBA* end = ib.End();
	while (it != end) {
		if (src->a == 0) {
			*it = light;
		}
		else {
			*it = dark;
		}
		it++;
		src++;
	}
	return ib;
}

AspectFixer& AspectFixer::Get(const Image& src_image, int w, int h, int w_extra, int h_extra) {
	static ArrayMap<int64, AspectFixer> map;
	hash_t hash = src_image.GetHashValue();
	String name = IntStr64(hash) + "-" + Format("%d-%d-%d-%d", w, h, w_extra, h_extra);
	hash_t name_hash = name.GetHashValue();
	int i = map.Find(name_hash);
	if (i >= 0)
		return map[i];
	AspectFixer& af = map.Add(name_hash);
	af.src_image = src_image;
	af.hash = hash;
	af.w = w;
	af.h = h;
	af.w_extra = w_extra;
	af.h_extra = h_extra;
	return af;
}

#endif

INITIALIZER_COMPONENT(AspectFixerLayer, "photo.layer.fixer.aspect", "Image|Photo");

END_UPP_NAMESPACE
