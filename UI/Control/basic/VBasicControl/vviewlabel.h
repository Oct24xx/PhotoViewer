/*
 * VViewLabel.hpp
 *	@description  : The View Label
*/

#pragma once

#include "vabstractbutton.hpp"
#include "vanimation.hpp"
#include "vuiobject.hpp"

VLIB_BEGIN_NAMESPACE

enum class VScrollButtonRadiusMode {
	WidthMode, HeightMode
};

/*
 * VScrollButton class:
 *	@description  : The Push Button in VScrollBar
*/
class VScrollButton : public VAbstractButton {
private:
	VScrollButtonTheme* Theme;

	VColorInterpolator BackgroundColor;
	VColorInterpolator LineColor;
	VScrollButtonRadiusMode
		               RadiusMode = VScrollButtonRadiusMode::HeightMode;

public:
	VSignal<> ButtonOnHover;
	VSignal<> ButtonDishover;
	VSignal<int, int> ButttonClickedDraged;
	VSignal<> ButtonDragedStart;
	VSignal<> ButtonDragedEnd;

	bool      InDrag       = false;
	bool      InMouseFocus = false;

	int       MouseDragedX = 0;
	int       MouseDragedY = 0;

public:
	/*
	 * Build up Functional
	*/

	VScrollButton(VUIObject* Parent, std::wstring PlaneString = L"") : VAbstractButton(Parent),
		BackgroundColor(0.1, VInterpolatorType::AccelerateInterpolator),
		LineColor(0.1, VInterpolatorType::AccelerateInterpolator) {
		Theme = new VScrollButtonTheme(*(static_cast<VScrollButtonTheme*>(SearchThemeFromParent(VSCROLLBUTTON_THEME))));
		Theme->PlaneString = PlaneString;

		if (VUnlikely(Theme == nullptr)) {
			VAssertBox(L"Error! Can't Get the Native Theme Of VScrollButton");
		}
	}

	/*
	 * OnPaint override Functional
	*/
	void OnPaint(VCanvas* Canvas) override {
		VPen        BorderPen(Theme->CurrentLineColor);
		VSolidBrush FillBrush(Theme->CurrentBackgroundColor);

		VPainterDevice Device(Canvas);

		if (RadiusMode == VScrollButtonRadiusMode::WidthMode) {
			Device.FillRoundedRectangle(&BorderPen, &FillBrush, GetSourceRect(), { GetWidth(), GetWidth() });
		}
		else {
			Device.FillRoundedRectangle(&BorderPen, &FillBrush, GetSourceRect(), { GetHeight(), GetHeight() });
		}
	}

	void LeftClickedDown() override {
		if (InDrag == false) {
			BackgroundColor.Start(Theme->CurrentBackgroundColor, Theme->OnClickedBackgroundColor);
			LineColor.Start(Theme->CurrentLineColor, Theme->OnClickedLineColor);

			InDrag = true;

			ButtonDragedStart.Emit();

			LockGlobalFocusID(Kernel()->GlobalID);
		}
	}
	void LeftClickedUp() override {
		InDrag = false;
		InMouseFocus = false;

		BackgroundColor.Start(Theme->CurrentBackgroundColor, Theme->BackgroundColor);
		LineColor.Start(Theme->CurrentLineColor, Theme->LineColor);

		ButtonDragedEnd.Emit();

		UnlockGlobalFocusID();
	}
	void GotMouseFocus() override {
		if (InDrag == false) {
			BackgroundColor.Start(Theme->CurrentBackgroundColor, Theme->OnHoverBackgroundColor);
			LineColor.Start(Theme->CurrentLineColor, Theme->OnHoverLineColor);

			ButtonOnHover.Emit();

			InMouseFocus = true;
		}
	}
	void LosedMouseFocus() override {
		if (InDrag == false) {
			ButtonDishover.Emit();
		}

		InMouseFocus = false;
	}

	void DealyMessage(VMessage* Message) override {
		if (InDrag == false) {
			return;
		}

		switch (Message->GetType()) {
		case VMessageType::MouseClickedMessage: {
			VMouseClickedMessage* MouseClickedMessage = static_cast<VMouseClickedMessage*>(Message);

			if (MouseClickedMessage->ClickedKey == VMouseKeyFlag::Left &&
				MouseClickedMessage->ClickedMethod == VMouseClickedFlag::Up &&
				InMouseFocus == false) {
				InDrag = false;

				BackgroundColor.Start(Theme->CurrentBackgroundColor, Theme->BackgroundColor);
				LineColor.Start(Theme->CurrentLineColor, Theme->LineColor);

				ButtonDragedEnd.Emit();

				UnlockGlobalFocusID();
			}

			break;
		}
		case VMessageType::MouseMoveMessage: {
			VMouseMoveMessage* MoveMessage = static_cast<VMouseMoveMessage*>(Message);

			MouseDragedX = MoveMessage->MousePosition.x;
			MouseDragedY = MoveMessage->MousePosition.y;

			ButttonClickedDraged.Emit(MouseDragedX, MouseDragedY);

			break;
		}
		}
	}
	void SetRadiusMode(VScrollButtonRadiusMode Mode) {
		RadiusMode = Mode;
	}

public:
	void CheckFrame() override {
		if (BackgroundColor.IsAnimationEnd() == false ||
			LineColor.IsAnimationEnd() == false) {
			Theme->CurrentBackgroundColor = BackgroundColor.GetOneFrame();
			Theme->CurrentLineColor = LineColor.GetOneFrame();

			Update();
		}

		if (InDrag == true) {
			SetGlobalFocusID(Kernel()->GlobalID);
		}
	}
};

/*
 * VScrollBar class:
 *	@description  : The Scroll Bar (Height)
*/
class VScrollBarHeight : public VUIObject {
private:
	VScrollButton*   ViewButton;

	VScrollBarTheme* Theme;

	int              ViewHeight;

	bool             InDragChanging = false;

	VGeomteryAnimation* ScrollGeomteryAnimation;
	VPositionAnimation* ScrollPositionAnimaiton;

	VColorInterpolator BackgroundColor;
	VColorInterpolator LineColor;

private:
	int MouseX = 0;
	int MouseY = 0;

private:
	void StartDragAnimation() {
		ScrollGeomteryAnimation->Start({ 11     , ViewButton->GetHeight() });
		ScrollPositionAnimaiton->Start({ GetX(), ViewButton->GetY()      });
	}
	void EndDragAnimaiton() {
		ScrollGeomteryAnimation->Start({ 4     , ViewButton->GetHeight()                                         });
		ScrollPositionAnimaiton->Start({ GetX() + (11 / 2 - 4 / 2), ViewButton->GetY()                           });
	}

	void BarButtonDragStarted() {
		BackgroundColor.Start(Theme->BackgroundColor, Theme->OnDragBackgroundColor);
		LineColor.Start(Theme->LineColor, Theme->OnDragLineColor);
	}
	void BarButtonDragStopped() {
		EndDragAnimaiton();
		BackgroundColor.Start(Theme->OnDragBackgroundColor, Theme->BackgroundColor);
		LineColor.Start(Theme->OnDragLineColor, Theme->LineColor);
	}
	void BarButtonClickDraged(int MouseX, int MouseY) {
		if (MouseY >= GetY() &&
			MouseY <= GetY() + GetHeight()) {
			if (MouseY + ViewButton->GetHeight() >= GetY() + GetHeight()) {
				ViewButton->Move(GetX(), GetY() + GetHeight() - ViewButton->GetHeight());
			}
			else {
				ViewButton->Move(GetX(), MouseY);
			}

			BarDarged.Emit(GetPercent());
		}

	}

	void DealyMessage(VMessage* Message) override {
		switch (Message->GetType()) {
		case VMessageType::MouseMoveMessage: {
			VMouseMoveMessage* MouseMoveMessage = static_cast<VMouseMoveMessage*>(Message);

			if (InDragChanging == true) {
				if (MouseMoveMessage->MousePosition.y >= GetY() &&
					MouseMoveMessage->MousePosition.y <= GetY() + GetHeight()) {
					if (MouseMoveMessage->MousePosition.y + ViewButton->GetHeight() >= GetY() + GetHeight()) {
						ViewButton->Move(GetX(), GetY() + GetHeight() - ViewButton->GetHeight());
					}
					else {
						ViewButton->Move(GetX(), MouseMoveMessage->MousePosition.y);
					}
				}
			}
			else {
				MouseX = MouseMoveMessage->MousePosition.x;
				MouseY = MouseMoveMessage->MousePosition.y;
			}

			break;
		}
		}
	}

	void MouseLeftClicked(VMouseClickedFlag ClickedFlag) override {
		if (ClickedFlag == VMouseClickedFlag::Down) {
			BackgroundColor.Start(Theme->BackgroundColor, Theme->OnDragBackgroundColor);
			LineColor.Start(Theme->LineColor, Theme->OnDragLineColor);

			if (MouseY + ViewButton->GetHeight() >= GetY() + GetHeight()) {
				ViewButton->Move(GetX() + (11.f / 2 - double(ViewButton->GetWidth()) / 2), GetY() + GetHeight() - ViewButton->GetHeight());
			}
			else {
				ViewButton->Move(GetX() + (11.f / 2 - double(ViewButton->GetWidth()) / 2), MouseY);
			}
		}
		else {
			BackgroundColor.Start(Theme->OnDragBackgroundColor, Theme->BackgroundColor);
			LineColor.Start(Theme->OnDragLineColor, Theme->LineColor);
		}
	}

public:
	VSignal<double> BarDarged;

public:
	/*
	 * Build up Functional
	*/

	VScrollBarHeight(VUIObject* Parent, int RealH, int ViewH) : 
		VUIObject(Parent),
		BackgroundColor(0.1, VInterpolatorType::AccelerateInterpolator),
		LineColor(0.1, VInterpolatorType::AccelerateInterpolator) {

		ViewButton = new VScrollButton(Parent);

		ScrollGeomteryAnimation = new VGeomteryAnimation(ViewButton, 100, VInterpolatorType::AccelerateInterpolator);
		ScrollPositionAnimaiton = new VPositionAnimation(ViewButton, 100, VInterpolatorType::AccelerateInterpolator);

		ViewButton->ButtonOnHover.Connect(this, &VScrollBarHeight::StartDragAnimation);
		ViewButton->ButtonDragedStart.Connect(this, &VScrollBarHeight::BarButtonDragStarted);
		ViewButton->ButttonClickedDraged.Connect(this, &VScrollBarHeight::BarButtonClickDraged);
		ViewButton->ButtonDishover.Connect(this, &VScrollBarHeight::EndDragAnimaiton);
		ViewButton->ButtonDragedEnd.Connect(this, &VScrollBarHeight::BarButtonDragStopped);
		ViewButton->SetRadiusMode(VScrollButtonRadiusMode::WidthMode);
		
		Resize(11, RealH);

		ViewButton->Resize(4, RealH * RealH / double(ViewH));
		ViewButton->Move(GetX() + (11.f / 2 - double(ViewButton->GetWidth()) / 2), GetY());

		Theme = new VScrollBarTheme(*(static_cast<VScrollBarTheme*>(SearchThemeFromParent(VSCROLLBAR_THEME))));

		ViewHeight = ViewH;
	}

	void OnPaint(VCanvas* Canvas) override {
		VPainterDevice Painter(Canvas);
		
		VPen        Pen(Theme->CurrentLineColor);
		VSolidBrush Brush(Theme->CurrentBackgroundColor);

		Painter.FillRoundedRectangle(&Pen, &Brush, { 0, 0, GetWidth(), GetHeight() }, { GetWidth(), GetWidth() });
	}

	void Move(int X, int Y)  override {
		int Dx = ViewButton->GetX() - GetX();
		int Dy = ViewButton->GetY() - GetY();

		VUIObject::Move(X, Y);
		ViewButton->Move(X + Dx, Y + Dy);
	}

	void Resize(int Width, int Height) override {
		VUIObject::Resize(11, Height);

		ViewButton->Resize(4, Height * Height / double(ViewHeight));
		ViewButton->Move(GetX() + (11.f / 2 - double(ViewButton->GetWidth()) / 2), GetY());
	}

	double GetPercent() {
		return double(ViewButton->GetY() - GetY()) / (GetHeight() - ViewButton->GetHeight());
	}

	void SetViewHeight(int Height) {
		ViewHeight = Height;

		ViewButton->Resize(4, GetWidth() * Height / double(ViewHeight));
		ViewButton->Move(GetX() + (11.f / 2 - double(ViewButton->GetWidth()) / 2), GetY());
	}

public:
	void CheckFrame() override {
		if (BackgroundColor.IsAnimationEnd() == false ||
			LineColor.IsAnimationEnd() == false) {
			Theme->CurrentBackgroundColor = BackgroundColor.GetOneFrame();
			Theme->CurrentLineColor       = LineColor.GetOneFrame();

			Update();
		}
	}
};
class VScrollBarWidth : public VUIObject {
private:
	VScrollButton* ViewButton;

	VScrollBarTheme* Theme;

	int              ViewWidth;

	bool             InDragChanging = false;

	VGeomteryAnimation* ScrollGeomteryAnimation;
	VPositionAnimation* ScrollPositionAnimaiton;

	VColorInterpolator BackgroundColor;
	VColorInterpolator LineColor;

private:
	int MouseX = 0;
	int MouseY = 0;

private:
	void StartDragAnimation() {
		ScrollGeomteryAnimation->Start({ ViewButton->GetWidth(), 11 });
		ScrollPositionAnimaiton->Start({ ViewButton->GetX(), GetY() });
	}
	void EndDragAnimaiton() {
		ScrollGeomteryAnimation->Start({ ViewButton->GetWidth(), 4});
		ScrollPositionAnimaiton->Start({ ViewButton->GetX(), GetY() + 3 });
	}

	void BarButtonDragStarted() {
		BackgroundColor.Start(Theme->BackgroundColor, Theme->OnDragBackgroundColor);
		LineColor.Start(Theme->LineColor, Theme->OnDragLineColor);
	}
	void BarButtonDragStopped() {
		EndDragAnimaiton();
		BackgroundColor.Start(Theme->OnDragBackgroundColor, Theme->BackgroundColor);
		LineColor.Start(Theme->OnDragLineColor, Theme->LineColor);
	}
	void BarButtonClickDraged(int MouseX, int MouseY) {
		if (MouseX >= GetX() &&
			MouseX <= GetX() + GetWidth()) {
			if (MouseX + ViewButton->GetWidth() >= GetX() + GetWidth()) {
				ViewButton->Move(GetX() + GetWidth() - ViewButton->GetWidth(), ViewButton->GetY());
			}
			else {
				ViewButton->Move(MouseX, ViewButton->GetY());
			}
		}
	}

	void DealyMessage(VMessage* Message) override {
		switch (Message->GetType()) {
		case VMessageType::MouseMoveMessage: {
			VMouseMoveMessage* MouseMoveMessage = static_cast<VMouseMoveMessage*>(Message);

			if (InDragChanging == true) {
				if (MouseMoveMessage->MousePosition.y >= GetY() &&
					MouseMoveMessage->MousePosition.y <= GetY() + GetHeight()) {
					if (MouseMoveMessage->MousePosition.y + ViewButton->GetHeight() >= GetY() + GetHeight()) {
						ViewButton->Move(GetX(), GetY() + GetHeight() - ViewButton->GetHeight());
					}
					else {
						ViewButton->Move(GetX(), MouseMoveMessage->MousePosition.y);
					}
				}
			}
			else {
				MouseX = MouseMoveMessage->MousePosition.x;
				MouseY = MouseMoveMessage->MousePosition.y;
			}

			break;
		}
		}
	}

	void MouseLeftClicked(VMouseClickedFlag ClickedFlag) override {
		if (ClickedFlag == VMouseClickedFlag::Down) {
			BackgroundColor.Start(Theme->BackgroundColor, Theme->OnDragBackgroundColor);
			LineColor.Start(Theme->LineColor, Theme->OnDragLineColor);

			if (MouseX + ViewButton->GetWidth() >= GetX() + GetWidth()) {
				ViewButton->Move(GetX() + GetWidth() - ViewButton->GetWidth(), ViewButton->GetY());
			}
			else {
				ViewButton->Move(MouseX, ViewButton->GetY());
			}

			BarDarged.Emit(GetPercent());
		}
		else {
			BackgroundColor.Start(Theme->OnDragBackgroundColor, Theme->BackgroundColor);
			LineColor.Start(Theme->OnDragLineColor, Theme->LineColor);
		}
	}

	double GetPercent() {
		return double(ViewButton->GetX() - GetX()) / (GetWidth() - ViewButton->GetWidth());
	}

	void SetViewWidth(int Width) {
		ViewWidth = Width;

		ViewButton->Resize(GetWidth() * GetWidth() / double(ViewWidth), 4);
		ViewButton->Move(GetX(), GetY() + 3);
	}

public:
	VSignal<double> BarDarged;

public:
	/*
	 * Build up Functional
	*/

	VScrollBarWidth(VUIObject* Parent, int RealW, int ViewW) :
		VUIObject(Parent),
		BackgroundColor(0.1, VInterpolatorType::AccelerateInterpolator),
		LineColor(0.1, VInterpolatorType::AccelerateInterpolator) {

		ViewButton = new VScrollButton(Parent);

		ScrollGeomteryAnimation = new VGeomteryAnimation(ViewButton, 100, VInterpolatorType::AccelerateInterpolator);
		ScrollPositionAnimaiton = new VPositionAnimation(ViewButton, 100, VInterpolatorType::AccelerateInterpolator);

		ViewButton->ButtonOnHover.Connect(this, &VScrollBarWidth::StartDragAnimation);
		ViewButton->ButtonDragedStart.Connect(this, &VScrollBarWidth::BarButtonDragStarted);
		ViewButton->ButttonClickedDraged.Connect(this, &VScrollBarWidth::BarButtonClickDraged);
		ViewButton->ButtonDishover.Connect(this, &VScrollBarWidth::EndDragAnimaiton);
		ViewButton->ButtonDragedEnd.Connect(this, &VScrollBarWidth::BarButtonDragStopped);

		ViewButton->SetRadiusMode(VScrollButtonRadiusMode::HeightMode);

		Resize(RealW, 11);

		ViewButton->Resize(RealW * RealW / double(ViewW), 4);
		ViewButton->Move(GetX(), GetY() + 3);

		Theme = new VScrollBarTheme(*(static_cast<VScrollBarTheme*>(SearchThemeFromParent(VSCROLLBAR_THEME))));

		ViewWidth = ViewW;
	}

	void OnPaint(VCanvas* Canvas) override {
		VPainterDevice Painter(Canvas);

		VPen        Pen(Theme->CurrentLineColor);
		VSolidBrush Brush(Theme->CurrentBackgroundColor);

		Painter.FillRoundedRectangle(&Pen, &Brush, { 0, 0, GetWidth(), GetHeight() }, { GetHeight(), GetHeight() });
	}

	void Move(int X, int Y)  override {
		int Dx = ViewButton->GetX() - GetX();
		int Dy = ViewButton->GetY() - GetY();

		VUIObject::Move(X, Y);
		ViewButton->Move(X + Dx, Y + Dy);
	}

	void Resize(int Width, int Height) override {
		VUIObject::Resize(Width, 11);

		ViewButton->Resize(Width * Width / double(ViewWidth), 4);
		ViewButton->Move(GetX(), GetY() + 3);
	}

public:
	void CheckFrame() override {
		if (BackgroundColor.IsAnimationEnd() == false ||
			LineColor.IsAnimationEnd() == false) {
			Theme->CurrentBackgroundColor = BackgroundColor.GetOneFrame();
			Theme->CurrentLineColor = LineColor.GetOneFrame();

			Update();
		}
	}
};


/*
 * VViewLabel class:
 *	@description  : The View Label
*/
class VViewLabel : public VUIObject {
private:
	/*
	 * The User Surface View Rect
	*/
	VRect ViewRect;
	VRect ClipRect;

	VScrollBarHeight* HeightBar;
	VScrollBarWidth*  WidthBar;

private:
	/*
	 * EditCanvas
	*/
	void EditCanvas(VCanvas* Canvas) override {
		VCanvas NewCanvas(ViewRect.GetWidth(), ViewRect.GetHeight());

		VPainterDevice Device(&NewCanvas);
		Device.DrawImage(Canvas, ViewRect);

		*Canvas = NewCanvas;
	}

	void HeightDraged(double Percent) {
		ViewRect.Move(ViewRect.left, (ViewRect.GetHeight() - GetHeight()) * Percent);
	}
	void WidthDraged(double Percent) {
		ViewRect.Move((ViewRect.GetWidth() - GetWidth()) * Percent, ViewRect.top);
	}

public:
	/*
	 * Build up Functional
	*/

	VViewLabel(VUIObject* Parent, VSize SurfaceSize, VSize ViewSize) : VUIObject(Parent) {
		HeightBar = new VScrollBarHeight(Parent, ViewSize.y, SurfaceSize.y);
		WidthBar  = new VScrollBarWidth (Parent, ViewSize.x, SurfaceSize.x);

		HeightBar->BarDarged.Connect(this, &VViewLabel::HeightDraged);
		WidthBar->BarDarged.Connect(this, &VViewLabel::WidthDraged);

		Resize(SurfaceSize);

		ViewRect = { 0, 0, SurfaceSize.x, SurfaceSize.y };
	}

public:
	/*
	 * SetViewRect Functional:
	*/
	void SetViewRect(VRect Rect) {
		ViewRect = Rect;
	}
};

VLIB_END_NAMESPACE