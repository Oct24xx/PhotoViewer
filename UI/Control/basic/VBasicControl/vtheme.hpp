﻿/*
 * VTheme.hpp
 *	@description : Control's Theme
 *	@birth		 : 2022/6.10
*/

#pragma once

#include "../../../basic/vbasic/vbase.hpp"
#include "../../../render/vrender/vpainter.hpp"

VLIB_BEGIN_NAMESPACE

#define UNKNOW_THEME_TYPE 0

/*
 * VTheme class:
 *	@description  : THe Theme of Control
*/
class VTheme {
public:
	/*
	 * Build up Functional
	*/

	VTheme() {

	}

public:
	/*
	 * GetThemeType virtual Functional:
	 *	@description  : Get the The Theme Type
	*/
	virtual short GetThemeType() {
		return UNKNOW_THEME_TYPE;
	}
};

/*
 * The Native Theme
*/

#define VPUSHBUTTON_THEME    1
#define VTEXTLABEL_THEME     2
#define VWIDGET_THEME        3
#define VBLURLABEL_THEME     4
#define VIMAGELABEL_THEME    5

class VWidgetTheme : public VTheme {
public:
	VColor BackgroundColor;

	VColor BlurMixColor;
	bool   EnableBlurEffect;

public:
	VWidgetTheme() {
		BackgroundColor  = VColor(41, 42, 43);
		BlurMixColor     = VColor(56, 56, 56);

		EnableBlurEffect = false;
	}

	short GetThemeType() override {
		return VWIDGET_THEME;
	}
};

class VBlurLabelTheme : public VTheme {
public:
	VColor MixedColor;
	VPoint Radius;
	int    BlurRadius;

	VBlurLabelTheme() {
		MixedColor = VColor(56, 56, 56, 200);
		Radius     = { 0, 0 };

		BlurRadius = 200;
	}

public:
	short GetThemeType() override {
		return VBLURLABEL_THEME;
	}
};

class VNativeLabelTheme : public VTheme {
public:
	VColor BackgroundColor;
	VColor LineColor;
	VColor TextColor;
	
	VImage* BackgroundImage;

	VPoint Radius;
	VFontFamily*
		   FontFamily = nullptr;
	
	int    FontSize   = 0;

	int    BorderThinkness;

	std::wstring 
		   PlaneString;

public:
	VNativeLabelTheme() {
		BackgroundImage = nullptr;

		BorderThinkness = 2;
	}
};

class VTextLabelTheme : public VNativeLabelTheme {
public:
	VFontFormat* FontFormat;

public:
	VTextLabelTheme() {
		TextColor       = VColor(255, 255, 255, 119);
		BackgroundColor = VColor(  0,   0,   0,   0);
		LineColor       = VColor(  0,   0,   0,   0);

		Radius          = { 0, 0 };

		FontFamily      = new VFontFamily(L"微软雅黑");
		FontSize        = 16;

		FontFormat      = new VFontFormat;

		FontFormat->SetAlignment(VStringAlignment::AlignmentNear);
		FontFormat->SetLineAlignment(VStringAlignment::AlignmentNear);
	}

	short GetThemeType() override {
		return VTEXTLABEL_THEME;
	}
};

class VPushButtonTheme : public VNativeLabelTheme {
public:
	VColor OnHoverBackgroundColor;
	VColor OnHoverLineColor;
	VColor OnHoverTextColor;

	VColor OnClickedBackgroundColor;
	VColor OnClickedLineColor;
	VColor OnClickedTextColor;

	VColor CurrentLineColor;
	VColor CurrentBackgroundColor;
	VColor CurrentTextColor;

public:
	VPushButtonTheme() {
		BackgroundColor          = VColor(56 ,  56,  56,   0);
		LineColor                = VColor(53 ,  53,  53, 255);
		TextColor                = VColor(126, 126, 126, 255);

		OnHoverBackgroundColor   = VColor(56 ,  56,  56, 255);
		OnHoverLineColor         = VColor(56 ,  56,  56, 255);
		OnHoverTextColor         = VColor(126, 126, 126, 255);

		OnClickedBackgroundColor = VColor(98 , 100, 167, 255);
		OnClickedLineColor       = VColor(98 , 100, 167, 255);
		OnClickedTextColor       = VColor(255, 255, 255, 255);

		Radius                   = { 5, 5 };

		CurrentLineColor         = LineColor;
		CurrentBackgroundColor   = BackgroundColor;
		CurrentTextColor         = TextColor;

		FontFamily               = new VFontFamily(L"微软雅黑");
		FontSize                 = 16;
		IconImage                = nullptr;
	}

public:
	short GetThemeType() override {
		return VPUSHBUTTON_THEME;
	}

public:
	VImage* IconImage;
};

class VImageLabelTheme : public VTheme {
public:
	VImage* Image;
	VPoint  Radius;

public:
	VImageLabelTheme() {
		Image  = nullptr;
		Radius = { 0, 0 };
	}

	short GetThemeType() override {
		return VIMAGELABEL_THEME;
	}
};

VLIB_END_NAMESPACE