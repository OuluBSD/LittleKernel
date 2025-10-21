
template <class T>
FormEdit<T>::~FormEdit()
{
	if (embedded && !_Saved && PromptYesNo(t_("Do you want to save from before exit?"))) {
		EmbeddedSave();
	}
}

template <class T>
FormEdit<T>::FormEdit()
{
	this->SetRect(this->GetWorkArea().CenterRect(1000, 700));

	_ToolSize = Size(this->HorzLayoutZoom(240), this->VertLayoutZoom(18));
}


template <class T>
void FormEdit<T>::Construct(bool std_font_zoom) {
	if (std_font_zoom)
		StdFontZoom();

	_ViewMode = VIEW_MODE_AS_IS;

	this->AddFrame(_MenuBar);
	this->AddFrame(TopSeparatorFrame());
	this->AddFrame(_ToolBar);

	_View.Transparent();

	_Container.Set(_View, _View.GetPageRect().GetSize());

	_ItemProperties.WhenChildZ = THISBACK(UpdateChildZ);

	_View.SetContainer(_Container);
	_View.WhenUpdate = THISBACK(UpdateTools);
	_View.WhenObjectProperties = THISBACK(OpenObjectProperties);
	_View.WhenChildSelected = [this](const Vector<int>& indexes) {this->UpdateChildProperties(indexes);};
	_View.WhenUpdateLayouts = THISBACK(UpdateLayoutList);
	_View.WhenChildAllPos = THISBACK(UpdateChildAllPos);
	_View.WhenChildCount = THISBACK(UpdateChildCount);
	_View.WhenUpdateTabs = THISBACK(UpdateTabCtrls);
	_View.WhenChildPos = THISBACK(UpdateChildPos);
	_View.WhenMenuBar = THISBACK(CreateToolBar);
	_View.WhenChildZ = THISBACK(UpdateChildZ);

	SetViewMode(VIEW_MODE_INFO);
	UpdateTools();

	_LayoutList.Appending().Removing().Editing().SetFrame(NullFrame());
	_LayoutList.AddColumn(t_("Name")).Edit(_LayoutNameField);
	_LayoutList.HideRow(0);
	_LayoutList.SetInfoOffset(0);
	_LayoutList.SetInfo(t_("No lays"));
	_LayoutList.NotUseKeys();
	_LayoutList.WhenNewRow = THISBACK(OnAddLayout);
	_LayoutList.WhenChangeRow = THISBACK(OnSelectLayout);
	_LayoutList.WhenUpdateRow = THISBACK(OnUpdateLayout);
	_LayoutList.WhenRemoveRow = THISBACK(OnRemoveLayout);

	_ItemList.Editing().EditMode(1).MultiSelect().SetFrame(NullFrame());
	_ItemList.AddColumn(t_("Type")).Edit(_TypeList);
	_ItemList.AddColumn(t_("Variable")).Edit(_ObjectNameField);
	_ItemList.SetInfo(t_("No items"));
	_ItemList.WhenMenuBar = THISBACK(CreateObjectMenu);
	_ItemList.WhenChangeRow = THISBACK(SelectItem);
	_ItemList.WhenLeftClick = THISBACK(SelectItem);
	_ItemList.NotUseKeys();
	_TypeList.Add("Button");
	_TypeList.Add("Label");
	_TypeList.Add("EditField");
	_TypeList.Add("EditInt");
	_TypeList.Add("DropDate");
	_TypeList.Add("TabCtrl");
	_TypeList.Add("GridCtrl");
	_TypeList.Add("ProgressBar");
	_TypeList.Add("Form");
	_TypeList.WhenAction = THISBACK(UpdateObjectType);
	_ItemList.WhenUpdateRow = THISBACK(UpdateObjectName);
	_ObjectNameField.WhenAction = THISBACK(UpdateTempObjectName);
#ifdef PLATFORM_WIN32
	_ItemList.Chameleon();
#endif

	_ItemProperties.SetFrame(NullFrame());

	NewFile();
}

template <class T>
void FormEdit<T>::CreateMenuBar(Bar& bar)
{
	bar.Add(t_("File"), THISBACK(FileBar));
	bar.Add(t_("Item"), THISBACK(ItemBar));
	bar.Add(t_("Form"), THISBACK(FormBar));
	bar.Add(t_("View"), THISBACK(ViewBar));
}

template <class T>
void FormEdit<T>::FileBar(Bar& bar)
{
	if (embedded) {
		bar.Add(t_("Save changes"), FormEditImg::Save(), THISBACK(EmbeddedSave))
			.Enable(!IsProjectSaved() && _View.IsLayout());
		bar.Separator();
		bar.Add(t_("Import form"), FormEditImg::SaveAs(), THISBACK(EmbeddedExportFile))
			.Tip(t_("Import form"));
		bar.Add(t_("Export form"), FormEditImg::Open(), THISBACK(EmbeddedImportFile))
			.Tip(t_("Export form"));
	}
	else {
		bar.Add(t_("Create new form..."), FormEditImg::New(), THISBACK(NewFile))
			.Tip(t_("Create new form..."));
		bar.Add(t_("Open form..."), FormEditImg::Open(), THISBACK(OpenFile))
			.Tip(t_("Open form..."));
		bar.Separator();
		bar.Add(t_("Save changes to file..."), FormEditImg::Save(), THISBACK(SaveFile))
			.Enable(!IsProjectSaved() && _View.IsLayout())
			.Tip(t_("Save changes to file..."));
		bar.Add(t_("Save form to another file..."), FormEditImg::SaveAs(), THISBACK(SaveAsFile))
			.Enable(_View.IsLayout())
			.Tip(t_("Save form to another file..."));
		bar.Separator();
		bar.Add(t_("Quit"), THISBACK(Quit));
	}
}

template <class T>
void FormEdit<T>::UpdateTools()
{
	_ToolBar.Set(THISBACK(CreateToolBar));
	_MenuBar.Set(THISBACK(CreateMenuBar));
	_View.Refresh();
}

template <class T>
void FormEdit<T>::CreateBar(Bar& bar)
{
	_View.AddObjectMenu(bar, Point(_View.GetGridSize().cx * 5, _View.GetGridSize().cy * 5));
}

template <class T>
void FormEdit<T>::AlignBar(Bar& bar)
{
	_View.AlignObjectMenu(bar);
}

template <class T>
void FormEdit<T>::ItemBar(Bar& bar)
{
	Vector<int> sel = _View.GetSelected();
	int count = _View.IsLayout() ? _View.GetObjects()->GetCount() : 0;
	dword first;
	bool align = true;
	bool valign = true;

	if (sel.GetCount() == 0)
	{
		count = 0;
	}
	else if (sel.GetCount() == 1)
	{
		_View.SetHAlign( (*_View.GetObjects())[ sel[0] ].GetHAlign() );
		_View.SetVAlign( (*_View.GetObjects())[ sel[0] ].GetVAlign() );
	}
	else
	{
		for (int i = 0; i < sel.GetCount(); ++i)
		{
			if (i == 0) first = (*_View.GetObjects())[ sel[i] ].GetHAlign();
			if (first != (*_View.GetObjects())[ sel[i] ].GetHAlign()) { align = false; break; }
		}

		for (int i = 0; i < sel.GetCount(); ++i)
		{
			if (i == 0) first = (*_View.GetObjects())[ sel[i] ].GetVAlign();
			if (first != (*_View.GetObjects())[ sel[i] ].GetVAlign()) { valign = false; break; }
		}
	}

	bar.Add(t_("Add object..."), THISBACK(CreateBar));
	bar.Separator();

	if (sel.GetCount() == 1)
		_View.CreateObjectMenu(bar, sel[0]);

	if (sel.GetCount())
	{
		bar.Add(t_("Align selected"), THISBACK(AlignBar));
		bar.Separator();
	}

	bar.Add(t_("Spring left side"), FormViewImg::SpringLeft(), THISBACK2(SetSprings, Ctrl::LEFT, -1))
		.Check(_View.GetHAlign() == Ctrl::LEFT && align).Enable(count)
		.Tip(t_("Spring left side"));

	bar.Add(t_("Spring right side"), FormViewImg::SpringRight(), THISBACK2(SetSprings, Ctrl::RIGHT, -1))
		.Check(_View.GetHAlign() == Ctrl::RIGHT && align).Enable(count)
		.Tip(t_("Spring right side"));

	bar.Add(t_("Spring both horizontal sides"), FormViewImg::SpringHSize(), THISBACK2(SetSprings, Ctrl::SIZE, -1))
		.Check(_View.GetHAlign() == Ctrl::SIZE && align).Enable(count)
		.Tip(t_("Spring both horizontal sides"));

	bar.Add(t_("Horizontal center"), FormViewImg::SpringHCenter(), THISBACK2(SetSprings, Ctrl::CENTER, -1))
		.Check(_View.GetHAlign() == Ctrl::CENTER && align).Enable(count)
		.Tip(t_("Horizontal center"));

	bar.Separator();

	bar.Add(t_("Spring top side"), FormViewImg::SpringTop(), THISBACK2(SetSprings, -1, Ctrl::TOP))
		.Check(_View.GetVAlign() == Ctrl::TOP && valign).Enable(count)
		.Tip(t_("Spring top side"));

	bar.Add(t_("Spring bottom side"), FormViewImg::SpringBottom(), THISBACK2(SetSprings, -1, Ctrl::BOTTOM))
		.Check(_View.GetVAlign() == Ctrl::BOTTOM && valign).Enable(count)
		.Tip(t_("Spring bottom side"));

	bar.Add(t_("Spring both vertical sides"), FormViewImg::SpringVSize(), THISBACK2(SetSprings, -1, Ctrl::SIZE))
		.Check(_View.GetVAlign() == Ctrl::SIZE && valign).Enable(count)
		.Tip(t_("Spring both vertical sides"));

	bar.Add(t_("Vertical center"), FormViewImg::SpringVCenter(), THISBACK2(SetSprings, -1, Ctrl::CENTER))
		.Check(_View.GetVAlign() == Ctrl::CENTER && valign).Enable(count)
		.Tip(t_("Vertical center"));

	bar.Separator();

	bar.Add(t_("Spring left-top corner"), FormViewImg::SpringTopLeft(), THISBACK2(SetSprings, Ctrl::LEFT, Ctrl::TOP))
		.Enable(count)
		.Check(_View.GetHAlign() == Ctrl::LEFT && _View.GetVAlign() == Ctrl::TOP && align && valign)
		.Tip(t_("Spring left-top corner"));

	bar.Add(t_("Spring left-bottom corner"), FormViewImg::SpringBottomLeft(), THISBACK2(SetSprings, Ctrl::LEFT, Ctrl::BOTTOM))
		.Enable(count)
		.Check(_View.GetHAlign() == Ctrl::LEFT && _View.GetVAlign() == Ctrl::BOTTOM && align && valign)
		.Tip(t_("Spring left-bottom corner"));

	bar.Add(t_("Spring right-bottom corner"), FormViewImg::SpringBottomRight(), THISBACK2(SetSprings, Ctrl::RIGHT, Ctrl::BOTTOM))
		.Enable(count)
		.Check(_View.GetHAlign() == Ctrl::RIGHT
			&& _View.GetVAlign() == Ctrl::BOTTOM && align && valign)
		.Tip(t_("Spring right-bottom corner"));

	bar.Add(t_("Spring right-top corner"), FormViewImg::SpringTopRight(), THISBACK2(SetSprings, Ctrl::RIGHT, Ctrl::TOP))
		.Enable(count)
		.Check(_View.GetHAlign() == Ctrl::RIGHT
			&& _View.GetVAlign() == Ctrl::TOP && align && valign)
		.Tip(t_("Spring right-top corner"));

	bar.Separator();

	bar.Add(t_("Spring all sides"), FormViewImg::SpringAuto(), THISBACK2(SetSprings, Ctrl::SIZE, Ctrl::SIZE))
		.Enable(count)
		.Check(_View.GetHAlign() == Ctrl::SIZE && _View.GetVAlign() == Ctrl::SIZE && align && valign)
		.Tip(t_("Spring all sides"));

	bar.Add(t_("Center object"), FormViewImg::SpringSize(), THISBACK2(SetSprings, Ctrl::CENTER, Ctrl::CENTER))
		.Enable(count)
		.Check(_View.GetHAlign() == Ctrl::CENTER && _View.GetVAlign() == Ctrl::CENTER && align && valign)
		.Tip(t_("Center object"));
}

template <class T>
void FormEdit<T>::FormBar(Bar& bar)
{
	bar.Add(t_("Change form settings..."), FormViewImg::LayoutProperties(), THISBACK(OpenLayoutProperties))
		.Enable(_View.IsLayout())
		.Tip(t_("Change form settings..."));
	bar.Add(t_("Preview"), FormViewImg::Layout(), THISBACK(TestLayout))
		.Enable(_View.IsLayout())
		.Tip(t_("Preview"));
}

template <class T>
void FormEdit<T>::ViewBar(Bar& bar)
{
	bar.Add(t_("Switch to as-is view..."), FormViewImg::ViewModeAsIs(), THISBACK1(SetViewMode, VIEW_MODE_AS_IS))
		.Check(_ViewMode == VIEW_MODE_AS_IS).Enable(_View.IsLayout())
		.Tip(t_("Switch to as-is view..."));

	bar.Add(t_("Switch to detailed view..."), FormViewImg::ViewModeInfo(), THISBACK1(SetViewMode, VIEW_MODE_INFO))
		.Check(_ViewMode == VIEW_MODE_INFO).Enable(_View.IsLayout())
		.Tip(t_("Switch to detailed view..."));
		
	bar.Add(t_("Switch to solid view..."), FormViewImg::ViewModeWireframe(), THISBACK1(SetViewMode, VIEW_MODE_WIREFRAME))
		.Check(_ViewMode == VIEW_MODE_WIREFRAME).Enable(_View.IsLayout())
		.Tip(t_("Switch to solid view..."));

	bar.Add(t_("Bind to grid..."), FormViewImg::GridBinding(), THISBACK2(ToggleFlag, "Grid.Binding", false))
		.Check(GetFlag("Grid.Binding", false)).Enable(_View.IsLayout())
		.Tip(t_("Bind to grid..."));

	bar.Add(t_("Coloring of the objects..."), FormEditImg::Coloring(), THISBACK(ToggleColoring))
		.Check(GetFlag("View.Coloring", true)).Enable(_View.IsLayout())
		.Tip(t_("Coloring of the objects..."));

	bar.Separator();

	bar.Add(t_("Use system font..."), FormEditImg::FontZoom(), THISBACK(StdFontZoom)).Check(IsStandardZoom())
		.Enable(_View.IsLayout())
		.Tip(t_("Use system font..."));
	bar.Add(t_("Use larger font..."), FormEditImg::FontZoomStd(), THISBACK(SmallFontZoom)).Check(IsSmallZoom())
		.Enable(_View.IsLayout())
		.Tip(t_("Use larger font..."));
	bar.Add(t_("Use largest font..."), FormEditImg::FontZoomLarge(), THISBACK(LargeFontZoom)).Check(IsLargeZoom())
		.Enable(_View.IsLayout())
		.Tip(t_("Use largest font..."));
}

template <class T>
void FormEdit<T>::CreateToolBar(Bar& bar)
{
	Vector<int> sel = _View.GetSelected();
	int count = _View.IsLayout() ? _View.GetObjects()->GetCount() : 0;
	dword first;
	bool align = true;
	bool valign = true;

	if (sel.GetCount() == 0)
	{
		count = 0;
	}
	else if (sel.GetCount() == 1)
	{
		_View.SetHAlign( (*_View.GetObjects())[ sel[0] ].GetHAlign() );
		_View.SetVAlign( (*_View.GetObjects())[ sel[0] ].GetVAlign() );
	}
	else
	{
		for (int i = 0; i < sel.GetCount(); ++i)
		{
			if (i == 0) first = (*_View.GetObjects())[ sel[i] ].GetHAlign();
			if (first != (*_View.GetObjects())[ sel[i] ].GetHAlign()) { align = false; break; }
		}

		for (int i = 0; i < sel.GetCount(); ++i)
		{
			if (i == 0) first = (*_View.GetObjects())[ sel[i] ].GetVAlign();
			if (first != (*_View.GetObjects())[ sel[i] ].GetVAlign()) { valign = false; break; }
		}
	}

	if (embedded) {
		bar.Add(t_("Save changes"), FormEditImg::Save(), THISBACK(EmbeddedSave))
			.Enable(!IsProjectSaved() && _View.IsLayout());
		bar.Separator();
		bar.Add(t_("Import form"), FormEditImg::SaveAs(), THISBACK(EmbeddedExportFile))
			.Tip(t_("Import form"));
		bar.Add(t_("Export form"), FormEditImg::Open(), THISBACK(EmbeddedImportFile))
			.Tip(t_("Export form"));
	}
	else {
		bar.Add(FormEditImg::New(), THISBACK(NewFile)).Tip(t_("Create new form..."));
		bar.Add(FormEditImg::Open(), THISBACK(OpenFile)).Tip(t_("Open form..."));
		bar.Add(FormEditImg::Save(), THISBACK(SaveFile)).Enable(!IsProjectSaved() && _View.IsLayout())
			.Tip(t_("Save changes to file..."));
		bar.Add(FormEditImg::SaveAs(), THISBACK(SaveAsFile)).Enable(_View.IsLayout())
			.Tip(t_("Save form to another file..."));
	}

	bar.Separator();

	bar.Add(FormViewImg::ViewModeAsIs(), THISBACK1(SetViewMode, VIEW_MODE_AS_IS))
		.Check(_ViewMode == VIEW_MODE_AS_IS).Enable(_View.IsLayout())
		.Tip(t_("Switch to as-is view..."));

	bar.Add(FormViewImg::ViewModeInfo(), THISBACK1(SetViewMode, VIEW_MODE_INFO))
		.Check(_ViewMode == VIEW_MODE_INFO).Enable(_View.IsLayout())
		.Tip(t_("Switch to detailed view..."));
		
	bar.Add(FormViewImg::ViewModeWireframe(), THISBACK1(SetViewMode, VIEW_MODE_WIREFRAME))
		.Check(_ViewMode == VIEW_MODE_WIREFRAME).Enable(_View.IsLayout())
		.Tip(t_("Switch to solid view..."));

	bar.Add(FormViewImg::GridBinding(), THISBACK2(ToggleFlag, "Grid.Binding", false))
		.Check(GetFlag("Grid.Binding", false)).Enable(_View.IsLayout())
		.Tip(t_("Bind to grid..."));

	bar.Add(FormEditImg::Coloring(), THISBACK(ToggleColoring))
		.Check(GetFlag("View.Coloring", true)).Enable(_View.IsLayout())
		.Tip(t_("Coloring of the objects..."));

	bar.Separator();

	bar.Add(FormViewImg::LayoutProperties(), THISBACK(OpenLayoutProperties))
		.Enable(_View.IsLayout())
		.Tip(t_("Change form settings..."));
	bar.Add(FormViewImg::Layout(), THISBACK(TestLayout))
		.Enable(_View.IsLayout())
		.Tip(t_("Preview"));

	bar.Separator();

	bar.Add(FormEditImg::FontZoom(), THISBACK(StdFontZoom)).Check(IsStandardZoom())
		.Enable(_View.IsLayout())
		.Tip(t_("Use system font..."));
	bar.Add(FormEditImg::FontZoomStd(), THISBACK(SmallFontZoom)).Check(IsSmallZoom())
		.Enable(_View.IsLayout())
		.Tip(t_("Use larger font..."));
	bar.Add(FormEditImg::FontZoomLarge(), THISBACK(LargeFontZoom)).Check(IsLargeZoom())
		.Enable(_View.IsLayout())
		.Tip(t_("Use largest font..."));

	if (_GUILayouts.GetCount() >= 2) {
		bar.Separator();
		bar.AddTool(_GUILayouts, _ToolSize);
		_GUILayouts.Enable(_GUILayouts.GetCount());
		_GUILayouts.GetButton(2).Enable(_GUILayouts.GetCount() > 1
			&& (~_GUILayouts).ToString() != t_(" Default"));
		_GUILayouts.GetButton(3).Enable(_GUILayouts.GetCount());
	}

	bar.ToolBreak();

	bar.Add(FormViewImg::SpringLeft(), THISBACK2(SetSprings, Ctrl::LEFT, -1))
		.Check(_View.GetHAlign() == Ctrl::LEFT && align).Enable(count)
		.Tip(t_("Spring left side"));

	bar.Add(FormViewImg::SpringRight(), THISBACK2(SetSprings, Ctrl::RIGHT, -1))
		.Check(_View.GetHAlign() == Ctrl::RIGHT && align).Enable(count)
		.Tip(t_("Spring right side"));

	bar.Add(FormViewImg::SpringHSize(), THISBACK2(SetSprings, Ctrl::SIZE, -1))
		.Check(_View.GetHAlign() == Ctrl::SIZE && align).Enable(count)
		.Tip(t_("Spring both horizontal sides"));

	bar.Add(FormViewImg::SpringHCenter(), THISBACK2(SetSprings, Ctrl::CENTER, -1))
		.Check(_View.GetHAlign() == Ctrl::CENTER && align).Enable(count)
		.Tip(t_("Horizontal center"));

	bar.Separator();

	bar.Add(FormViewImg::SpringTop(), THISBACK2(SetSprings, -1, Ctrl::TOP))
		.Check(_View.GetVAlign() == Ctrl::TOP && valign).Enable(count)
		.Tip(t_("Spring top side"));

	bar.Add(FormViewImg::SpringBottom(), THISBACK2(SetSprings, -1, Ctrl::BOTTOM))
		.Check(_View.GetVAlign() == Ctrl::BOTTOM && valign).Enable(count)
		.Tip(t_("Spring bottom side"));

	bar.Add(FormViewImg::SpringVSize(), THISBACK2(SetSprings, -1, Ctrl::SIZE))
		.Check(_View.GetVAlign() == Ctrl::SIZE && valign).Enable(count)
		.Tip(t_("Spring both vertical sides"));

	bar.Add(FormViewImg::SpringVCenter(), THISBACK2(SetSprings, -1, Ctrl::CENTER))
		.Check(_View.GetVAlign() == Ctrl::CENTER && valign).Enable(count)
		.Tip(t_("Vertical center"));

	bar.Separator();

	bar.Add(FormViewImg::SpringTopLeft(), THISBACK2(SetSprings, Ctrl::LEFT, Ctrl::TOP))
		.Enable(count)
		.Check(_View.GetHAlign() == Ctrl::LEFT && _View.GetVAlign() == Ctrl::TOP && align && valign)
		.Tip(t_("Spring left-top corner"));

	bar.Add(FormViewImg::SpringBottomLeft(), THISBACK2(SetSprings, Ctrl::LEFT, Ctrl::BOTTOM))
		.Enable(count)
		.Check(_View.GetHAlign() == Ctrl::LEFT && _View.GetVAlign() == Ctrl::BOTTOM && align && valign)
		.Tip(t_("Spring left-bottom corner"));

	bar.Add(FormViewImg::SpringBottomRight(), THISBACK2(SetSprings, Ctrl::RIGHT, Ctrl::BOTTOM))
		.Enable(count)
		.Check(_View.GetHAlign() == Ctrl::RIGHT
			&& _View.GetVAlign() == Ctrl::BOTTOM && align && valign)
		.Tip(t_("Spring right-bottom corner"));

	bar.Add(FormViewImg::SpringTopRight(), THISBACK2(SetSprings, Ctrl::RIGHT, Ctrl::TOP))
		.Enable(count)
		.Check(_View.GetHAlign() == Ctrl::RIGHT
			&& _View.GetVAlign() == Ctrl::TOP && align && valign)
		.Tip(t_("Spring right-top corner"));

	bar.Separator();

	bar.Add(FormViewImg::SpringAuto(), THISBACK2(SetSprings, Ctrl::SIZE, Ctrl::SIZE))
		.Enable(count)
		.Check(_View.GetHAlign() == Ctrl::SIZE && _View.GetVAlign() == Ctrl::SIZE && align && valign)
		.Tip(t_("Spring all sides"));

	bar.Add(FormViewImg::SpringSize(), THISBACK2(SetSprings, Ctrl::CENTER, Ctrl::CENTER))
		.Enable(count)
		.Check(_View.GetHAlign() == Ctrl::CENTER && _View.GetVAlign() == Ctrl::CENTER && align && valign)
		.Tip(t_("Center object"));
}

template <class T>
void FormEdit<T>::TestLayout()
{
	String preview = (_File.IsEmpty())
		? ConfigFile("FormEditor.view")
		: GetFileDirectory(_File) + "\\" + "FormEditor.view";

	_View.SaveAll(preview, false);

	FormWindow form;

	form.Load(preview);
	form.Layout(_View.GetCurrentLayout()->Get("Form.Name"), _View.GetFont());
	form.ExecuteForm();

	DeleteFile(preview);
}

template <class T>
void FormEdit<T>::UpdateChildPos(const Vector<int>& indexes)
{
	if (!_View.IsLayout())
		return;

	if (_ViewMode == VIEW_MODE_WIREFRAME)
		return;

	for (int i = 0; i < indexes.GetCount(); ++i)
	{
		_Ctrls[ indexes[i] ].SetRect(
			_View.Zoom(_View.Offseted((*_View.GetObjects())[indexes[i]].GetRect()).Offseted(1, 1))
		);
	}
	ProjectSaved(false);
}

template <class T>
void FormEdit<T>::UpdateChildAllPos()
{
	if (!_View.IsLayout())
		return;

	if (_ViewMode == VIEW_MODE_WIREFRAME)
		return;

	if (_View.GetObjectCount() != _Ctrls.GetCount())
		return UpdateChildZ();

	for (int i = 0; i < _View.GetObjectCount(); ++i)
		_Ctrls[i].SetRect( _View.Zoom(_View.Offseted((*_View.GetObjects())[i].GetRect()).Offseted(1, 1)) );

	ProjectSaved(false);
}

template <class T>
void FormEdit<T>::UpdateChildCount(int count)
{
	for (int i = _Ctrls.GetCount()-1; i >= 0; i--)
		_CtrlContainer.RemoveChild(&_Ctrls[i]);

	_Ctrls.Clear();
	_ItemList.Clear();
	_Temporaries.Clear();

	if (!_View.IsLayout())
	{
		UpdateItemList();
		return;
	}

//	if (_ViewMode == VIEW_MODE_WIREFRAME)
//		return;

	for (int i = 0; i < count; ++i)
	{
		if (!_View.GetObject(i))
			continue;

		String type = (*_View.GetObjects())[i].Get("Type");
		Font font = _View.GetFont();
		int h = _View.ZoomY((*_View.GetObjects())[i].GetNumber("Font.Height"));
		if (h != 0) font.Height(h);
		if (font.GetHeight() == 0) font.Height(StdFont().GetHeight());

		_ItemList.AddRow(type, (*_View.GetObjects())[i].Get("Variable"));

		if ((*_View.GetObjects())[i].GetBool("OutlineDraw", false)
			&& _ViewMode != VIEW_MODE_AS_IS)
		{
			if (_ViewMode != VIEW_MODE_WIREFRAME)
				_CtrlContainer.Add( _Ctrls.Create<StaticRect>() );
		}
		else if (type == "Button")
		{
			Button* b = &_Ctrls.Create<Button>();
			b->SetFont(font);
			if (_ViewMode == VIEW_MODE_AS_IS)
				b->SetLabel((*_View.GetObjects())[i].Get("Label"));
			if (_ViewMode != VIEW_MODE_WIREFRAME)
				_CtrlContainer.Add( b->NoWantFocus() );
		}
		else if (type == "DropDate")
		{
			DropDate *b = &_Ctrls.Create<DropDate>();
			b->SetFont(font);
			if (_ViewMode != VIEW_MODE_WIREFRAME)
				_CtrlContainer.Add( b->NoWantFocus() );
		}
		else if (type == "GridCtrl")
		{
			GridCtrl *b = &_Ctrls.Create<GridCtrl>();
			// b->SetFont(font); TODO

#ifdef PLATFORM_WIN32
			b->Chameleon();
#endif

			String src = (*_View.GetObjects())[i].Get("Grid.Columns");
			ReplaceString(src, ";", "\r\n");
			StringStream s;
			s.Open(src);
			IniFile f;
			f.Load(s);

			Vector<String> names = f.EnumNames("Columns");

			for (int j = 0; j < names.GetCount(); ++j)
			{
				int n = ScanInt(names[j]);

				Vector<String> values = f.GetArray("Columns", names[j]);
				if (values.GetCount() != 3)
					continue;

				if (values[1] == "Left") b->AddColumn(values[0]).HeaderAlignCenterLeft();
				else if (values[1] == "Right") b->AddColumn(values[0]).HeaderAlignCenterRight();
				else b->AddColumn(values[0]).HeaderAlignCenter();
			}

			if (_ViewMode != VIEW_MODE_WIREFRAME)
				_CtrlContainer.Add( b->NoWantFocus() );
		}
		else if (type == "EditField")
		{
			EditField *b = &_Ctrls.Create<EditField>();
			b->SetFont(font);
			if (_ViewMode == VIEW_MODE_AS_IS)
				b->SetText((*_View.GetObjects())[i].Get("DefaultData"));
			if (_ViewMode != VIEW_MODE_WIREFRAME)
				_CtrlContainer.Add( b->NoWantFocus() );
		}
		else if (type == "EditInt")
		{
			EditInt *b = &_Ctrls.Create<EditInt>();
			b->SetFont(font);
			if (_ViewMode == VIEW_MODE_AS_IS)
				b->SetText((*_View.GetObjects())[i].Get("DefaultData"));
			if (_ViewMode != VIEW_MODE_WIREFRAME)
				_CtrlContainer.Add( b->NoWantFocus() );
		}
		else if (type == "ProgressBar")
		{
			ProgressIndicator *b = &_Ctrls.Create<ProgressIndicator>();
			b->Set(0, 100);
			if (_ViewMode != VIEW_MODE_WIREFRAME)
				_CtrlContainer.Add( b->NoWantFocus() );
		}
		else if (type == "TabCtrl")
		{
			TabCtrl* e = &_Ctrls.Create<TabCtrl>();
			TabCtrl::Style& style = e->StyleDefault().Write();
			style.font = font;
			style.tabheight = font.GetHeight() + this->VertLayoutZoom(10);
			e->SetStyle(style);

			String src = (*_View.GetObjects())[i].Get("Tab.Content");
			ReplaceString(src, ";", "\r\n");
			StringStream s;
			s.Open(src);
			IniFile f;
			f.Load(s);

			Vector<String> names = f.EnumNames("Tabs");
			VectorMap<int, Vector<String> > cache;

			int tabCount = 0;
			for (int j = 0; j < names.GetCount(); ++j)
			{
				int n = ScanInt(names[j]);

				if (AsString(n) != names[j])
					continue;

				Vector<String> values = f.GetArray("Tabs", names[j]);
				if (values.GetCount() != 3)
					continue;

				Container *cont = &_Temporaries.Create<Container>();
				Form *f = &_Temporaries.Create<Form>();

					if (values[0] != t_("Current form"))
					{
						if (!f->Load(GetFileDirectory(_File) + "\\" + values[0]))
							continue;
					}
					else
					{
						int lay = _View.HasLayout(values[1]);
						if (lay < 0)
							continue;

						f->GetLayouts().Add() <<= _View.GetLayouts()[lay];
					}

				if (!f->Layout(values[1], font))
					continue;

				cont->Set(*f, f->GetSize());
				cont->SizePos();
				e->Add(*cont, values[2]);

				tabCount++;
			}

			int activeTab = -1;

			if (tabCount)
			{
				activeTab = (*_View.GetObjects())[i].GetNumber("Tab.Active", 0, 0);
				if (activeTab >= tabCount)
				{
					activeTab = tabCount - 1;
					e->Set(activeTab);
				}
				e->Set(activeTab);
			}

			(*_View.GetObjects())[i].SetNumber("Tab.Active", activeTab);
			(*_View.GetObjects())[i].SetNumber("Tab.Count", tabCount);

			if (_ViewMode != VIEW_MODE_WIREFRAME)
				_CtrlContainer.Add( e->NoWantFocus() );
		}
		else if (type == "Form")
		{
			Form f;
			String path = (*_View.GetObjects())[i].Get("Form.Path");
			(*_View.GetObjects())[i].Get("Form.PathType") == "Relative"
				? f.Load(::GetFileDirectory(_File) + "\\" + path)
				: f.Load(path);
			f.Layout((*_View.GetObjects())[i].Get("Form.Layout"), font);
			ImageDraw w(f.GetSize());
			f.DrawCtrl(w);
			ImageBuffer buf(w);
			StaticImage *s = &_Ctrls.Create<StaticImage>();
			s->SetImage(buf);

			if (_ViewMode != VIEW_MODE_WIREFRAME)
				_CtrlContainer.AddChild( s );
		}
		else
		{
			Label *b = &_Ctrls.Create<Label>();
			b->SetFont(font);
			Color fontColor = Black();
			LoadFromString(fontColor, Decode64((*_View.GetObjects())[i].Get("Font.Color",
				StoreAsString(fontColor))));
			b->SetInk(fontColor);
			String align = (*_View.GetObjects())[i].Get("Text.Align");
			if (align == "Center") b->SetAlign(ALIGN_CENTER);
			if (align == "Right") b->SetAlign(ALIGN_RIGHT);
			if (align == "Left") b->SetAlign(ALIGN_LEFT);
			if (_ViewMode == VIEW_MODE_AS_IS)
				b->SetLabel((*_View.GetObjects())[i].Get("Label"));
			if (_ViewMode != VIEW_MODE_WIREFRAME)
				_CtrlContainer.Add( b->NoWantFocus() );
		}

		String frame = (*_View.GetObjects())[i].Get("Frame");
		Ctrl* c = NULL;
		if (_Ctrls.GetCount())
			c = &_Ctrls[_Ctrls.GetCount() - 1];

		if (c)
		{
			if (frame == "Null frame")             c->SetFrame(NullFrame());
			if (frame == "Field frame")            c->SetFrame(FieldFrame());
			if (frame == "Inset frame")            c->SetFrame(InsetFrame());
			if (frame == "Outset frame")           c->SetFrame(OutsetFrame());
			if (frame == "Thin inset frame")       c->SetFrame(ThinInsetFrame());
			if (frame == "Thin outset frame")      c->SetFrame(ThinOutsetFrame());
			if (frame == "Black frame")            c->SetFrame(BlackFrame());
			if (frame == "Button frame")           c->SetFrame(ButtonFrame());
			if (frame == "Top separator frame")    c->SetFrame(TopSeparatorFrame());
			if (frame == "Left separator frame")   c->SetFrame(LeftSeparatorFrame());
			if (frame == "Right separator frame")  c->SetFrame(RightSeparatorFrame());
			if (frame == "Bottom separator frame") c->SetFrame(BottomSeparatorFrame());
		}
	}

	UpdateItemList();
	UpdateChildAllPos();
}

template <class T>
void FormEdit<T>::UpdateChildZ()
{
	UpdateChildCount( _View.GetObjectCount() );
}

template <class T>
void FormEdit<T>::UpdateTabCtrls()
{
	if (_Ctrls.GetCount() != _View.GetObjectCount())
		return;

	UpdateChildZ();
}

template <class T>
void FormEdit<T>::Clear()
{
	UpdateChildZ();
	_ItemProperties.Clear();

	_LayoutList.Clear();
	_ItemList.Clear();
	_Ctrls.Clear();

	_View.New();
	_File.Clear();
	_Saved = true;
}

template <class T>
void FormEdit<T>::OpenLayoutProperties()
{
	if (!_View.IsLayout())
		return;

	UpdateChildZ();

	StdFontZoomShort();

	WithFormProperties<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, t_("Form properties"));

#ifndef PLATFORM_WIN32
	dlg.ToolOption.Disable();
#else
	dlg.ToolWindow();
#endif

	FormLayout* l = _View.GetCurrentLayout();
	if (!l) { LastFontZoom(); return; }

	dlg.Width  <<= l->GetFormSize().cx;
	dlg.Height <<= l->GetFormSize().cy;
	dlg.MaximizeOption <<= l->GetBool("Form.MaximizeBox", false);
	dlg.MinimizeOption <<= l->GetBool("Form.MinimizeBox", false);
	dlg.SizeableOption <<= l->GetBool("Form.Sizeable", false);
	dlg.ToolOption <<= l->GetBool("Form.ToolWindow", false);
	dlg.TitleEdit <<= l->Get("Form.Title");

	if (dlg.Execute() != IDOK)
	{
		LastFontZoom();
		return;
	}

	_View.SetFormSize(Size(~dlg.Width, ~dlg.Height));
	l->SetBool("Form.MaximizeBox", dlg.MaximizeOption.Get());
	l->SetBool("Form.MinimizeBox", dlg.MinimizeOption.Get());
	l->SetBool("Form.ToolWindow", dlg.ToolOption.Get());
	l->SetBool("Form.Sizeable", dlg.SizeableOption.Get());
	l->Set("Form.Title", AsString(~dlg.TitleEdit));

	_Container.Set(_View, _View.GetPageRect().GetSize());
	LastFontZoom();
}

template <class T>
void FormEdit<T>::NewFile()
{
	Clear();
	this->WhenTitle(t_("Form Editor") + String(" - ") + String(t_("New file")));
	ProjectSaved(false);
	_View.New();
	_Container.Set(_View, _View.GetPageRect().GetSize());
	this->Refresh();
}

template <class T>
void FormEdit<T>::OpenFile()
{
	UpdateChildZ();

	FileSelNative fs;
	fs.Type(t_("Form files"), "*.form");
	fs.Type(t_("Form archives"), "*.fz");
	fs.AllFilesType();

	if (!fs.ExecuteOpen(t_("Open form...")))
		return;

	Clear();
	_File = ~fs;

	bool compression = false;
	if (Upp::GetFileName(_File).Find(".fz") >= 0)
		compression = true;

	_View.LoadAll(_File, compression);
	UpdateLayoutList();
	UpdateChildZ();

	this->WhenTitle((t_("Form Editor")) + String(" - ") + ::GetFileName(_File));

	_Container.Set(_View, _View.GetPageRect().GetSize());
	UpdateTools();

	ProjectSaved(true);
}

template <class T>
void FormEdit<T>::OpenXml(const String& xml, bool compression) {
	UpdateChildZ();

	Clear();
	_File = "";
	_View.LoadAllString(xml, compression);
	UpdateLayoutList();
	UpdateChildZ();

	this->WhenTitle(t_("Form Editor"));

	_Container.Set(_View, _View.GetPageRect().GetSize());
	UpdateTools();

	ProjectSaved(true);
}

template <class T>
void FormEdit<T>::SaveFile()
{
	if (!_View.IsLayout())
		return;

	UpdateChildZ();

	if (_File.IsEmpty())
		SaveAsFile();
	else
	{
		bool compression = false;
		if (Upp::GetFileName(_File).Find(".fz") >= 0)
			compression = true;
		_View.SaveAll(_File, compression);
		ProjectSaved(true);
	}
}

template <class T>
void FormEdit<T>::SaveAsFile()
{
	if (!_View.IsLayout())
		return;

	UpdateChildZ();

	FileSelector fs;
	fs.Type(t_("Form files (*.form)"), "*.form");
	fs.Type(t_("Form archives (*.fz)"), "*.fz");
	fs.AllFilesType();

	if (!fs.ExecuteSaveAs(t_("Save form...")))
		return;

	_File = ~fs;

	if (Upp::GetFileName(_File).Find('.') < 0)
		_File += ".form";

	bool compression = false;
	if (Upp::GetFileName(_File).Find(".fz") >= 0)
		compression = true;

	_View.SaveAll(_File, compression);
	this->WhenTitle((t_("Form Editor")) + String(" - ") + ::GetFileName(_File));
	ProjectSaved(true);
}

template <class T>
void FormEdit<T>::SaveXml(String& xml, bool compression) {
	if (!_View.IsLayout())
		return;

	UpdateChildZ();

	_File = "";

	_View.SaveAllString(xml, compression);
	this->WhenTitle(t_("Form Editor"));
	ProjectSaved(true);
}

template <class T>
void FormEdit<T>::EmbeddedSave() {
	WhenEmbeddedSave();
}

template <class T>
void FormEdit<T>::EmbeddedExportFile() {
	if (!_View.IsLayout())
		return;

	UpdateChildZ();

	FileSelector fs;
	fs.Type(t_("Form files (*.form)"), "*.form");
	fs.Type(t_("Form archives (*.fz)"), "*.fz");
	fs.AllFilesType();

	if (!fs.ExecuteSaveAs(t_("Export form...")))
		return;

	String filepath = ~fs;

	if (Upp::GetFileName(filepath).Find('.') < 0)
		filepath += ".form";

	bool compression = false;
	if (Upp::GetFileName(filepath).Find(".fz") >= 0)
		compression = true;

	_View.SaveAll(filepath, compression);
}

template <class T>
void FormEdit<T>::EmbeddedImportFile() {
	UpdateChildZ();

	FileSelector fs;
	fs.Type(t_("Form files"), "*.form");
	fs.Type(t_("Form archives"), "*.fz");
	fs.AllFilesType();

	if (!fs.ExecuteOpen(t_("Import form...")))
		return;

	Clear();
	String filepath = ~fs;

	bool compression = false;
	if (Upp::GetFileName(filepath).Find(".fz") >= 0)
		compression = true;

	_View.LoadAll(filepath, compression);
	UpdateLayoutList();
	UpdateChildZ();

	_Container.Set(_View, _View.GetPageRect().GetSize());
	UpdateTools();
}

template <class T>
void FormEdit<T>::OpenObjectProperties(const Vector<int>& indexes)
{
	if (!_View.IsLayout())
		return;

	String temp = _TempObjectName;
	_TempObjectName.Clear();
	_ItemList.EndEdit(false, false, false);
	int row = _ItemList.GetCurrentRow();
	if (row >= 0 && !temp.IsEmpty())
	{
		_View.GetCurrentLayout()->GetObjects()[row].Set("Variable", temp);
		_ItemList.Set(row, 1, temp);
	}
	_LayoutList.EndEdit();

	if (indexes.GetCount() == 1)
	{
		FormObject* pI = _View.GetObject(indexes[0]);
		if (!pI) return;

		_ItemProperties._Options.EndEdit();
		_ItemProperties.Generate(pI, indexes[0]);
	}

	if (indexes.GetCount() == 0)
	{
		_ItemProperties._Options.EndEdit();
		_ItemProperties._Headers.Clear();
		_ItemProperties._Options.Clear();
	}

	UpdateItemList();
}
