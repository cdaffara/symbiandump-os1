// GuiConsAppView.h
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __GUICONSAPPVIEW_H__
#define __GUICONSAPPVIEW_H__

#include <coecntrl.h>
class CEikEdwin;
class CEikLabel;
class CImageDecoder;
class CImageDecodeAo;

enum TGuiConsViewPanic
	{
	EGuiConsControlAlreadyExists = 0,
	EGuiConsBadControlIndex,
	};

class CGuiConsAppView : public CCoeControl
	{
public:
	static CGuiConsAppView* NewL(const TRect& aRect);
	static CGuiConsAppView* NewLC(const TRect& aRect);
	virtual ~CGuiConsAppView();
	
	void SetLargeControlL(CCoeControl* aControl);
	void ClearLargeControl();
	void ShowLabelL(const TDesC& aText);
	void ShowEditor();
	void FocusEditor(TBool aFocus);
	void AddEditorToStackL(CCoeAppUi* aAppUi);
	void RemoveEditorFromStack(CCoeAppUi* aAppUi);
	HBufC* GetEditorTextL();
	void SetEditorTextL(const TDesC* aText);
	
	void Draw(const TRect& aRect) const;
	virtual void SizeChanged();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;

	void Layout();

	void BgImageReady();
private:
	void ConstructL(const TRect& aRect);
	CGuiConsAppView();
	
private:
	CCoeControl* iLargeControl;
	CEikEdwin* iEditor;
	CEikLabel* iLabel;
	CImageDecodeAo* iBgImage;
	};
	
class CImageDecodeAo : public CActive
	{
public:
	static CImageDecodeAo* NewL(CGuiConsAppView& aOwner, const TDesC& aFileName);
	~CImageDecodeAo();
	CFbsBitmap* Bitmap();
	void Bitblt(CWindowGc& aGc, TPoint aPosition);
	
private:
	CImageDecodeAo(CGuiConsAppView& aOwner);
	void ConstructL(const TDesC& aFileName);
	virtual void RunL();
	virtual void DoCancel();
private:
	CGuiConsAppView& iOwner;
	RFs iFs;
	CImageDecoder* iDecoder;
	TBool iBitmapReady;
	CFbsBitmap* iBitmap;
	CFbsBitmap* iMask;	
	};
	


#endif // __GUICONSAPPVIEW_H__
