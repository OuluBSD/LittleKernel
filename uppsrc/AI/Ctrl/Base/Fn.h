#ifndef _AI_TextCtrl_Fn_h_
#define _AI_TextCtrl_Fn_h_

NAMESPACE_UPP

void SetColoredListValue(ArrayCtrl& list, int row, int col, const String& txt, Color clr, bool blend=true);
String CacheImageFile(String dir, hash_t h);
String ThumbnailImageFile(String dir, hash_t h);
String FullImageFile(String dir, hash_t h);
Image RescaleToFit(const Image& img, int smaller_side_length);

struct AiComponentCtrl : ComponentCtrl {
	const Index<String>& GetTypeclasses() const;
	const Vector<ContentType>& GetContents() const;
	const Vector<String>& GetContentParts() const;
};

void SetCountForArray(ArrayCtrl& arr, int count);
void SetCountWithDefaultCursor(ArrayCtrl& arr, int count);
void SetCountWithDefaultCursor(ArrayCtrl& arr, int count, int sort_row, bool descending=false);

template <class T> bool LoadFromJsonFile_VisitorNodePrompt(T& o) {
	FileSelNative sel;
	sel.ActiveDir(GetHomeDirectory());
	sel.Type("JSON", "*.json");
	if (sel.ExecuteOpen("Select json file to import")) {
		String path = sel.Get();
		if (FileExists(path)) {
			try {
				String json = LoadFile(path);
				Value jv = ParseJSON(json);
				if(jv.IsError())
					return false;
				JsonIO jio(jv);
				Vis vis(jio);
				o.Visit(vis);
			}
			catch(ValueTypeError) {
				return false;
			}
			catch(JsonizeError) {
				return false;
			}
			return true;
		}
	}
	return false;
}

END_UPP_NAMESPACE

#endif

