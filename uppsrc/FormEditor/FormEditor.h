#ifndef _FormEditor_FormEditor_h_
#define _FormEditor_FormEditor_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

#include <GridCtrl/GridCtrl.h>
#include <Docking/Docking.h>
#include <Form/Form.hpp>
#include <Form/IniConfig.hpp>

#define IMAGECLASS FormEditImg
#define IMAGEFILE <FormEditor/FormEdit.iml>
#include <Draw/iml_header.h>

#define TFILE <FormEditor/FormEdit.t>
#include <Core/t.h>

#define LAYOUTFILE <FormEditor/FormEdit.lay>
#include <CtrlCore/lay.h>

NAMESPACE_UPP

#include "ScrollContainer.hpp"
#include "FormView.hpp"
#include "ExGridCtrl.hpp"
#include "FormProperties.hpp"
#include "StaticImage.hpp"
#include "EditTabs.hpp"
#include "EditColumns.hpp"
#include "FormEdit.h"
#include "FormEdit.hpp"

END_UPP_NAMESPACE

#endif
