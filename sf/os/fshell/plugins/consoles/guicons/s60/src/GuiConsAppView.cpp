// GuiConsAppView.cpp
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

#include <coemain.h>
#include <eikedwin.h>
#include <eiklabel.h>
#include <eikenv.h>
#include <barsread.h>
#include <coeaui.h>
#include <ImageConversion.h>
#include "GuiConsAppView.h"
#include "guicons.rsg"


class CEdwinThatDoesntConsumeEnter : public CEikEdwin
	{
	TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
	};

TKeyResponse CEdwinThatDoesntConsumeEnter::OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType)
	{
	if (aEvent.iCode == EKeyEnter)
		{
		return EKeyWasNotConsumed;
		}
	return CEikEdwin::OfferKeyEventL(aEvent, aType);
	}

_LIT(KGuiConsViewPanic, "GuiConsView");

void Panic(TGuiConsViewPanic aReason)
	{
	User::Panic(KGuiConsViewPanic, aReason);
	}

CGuiConsAppView* CGuiConsAppView::NewL(const TRect& aRect)
	{
	CGuiConsAppView* self = CGuiConsAppView::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

CGuiConsAppView* CGuiConsAppView::NewLC(const TRect& aRect)
	{
	CGuiConsAppView* self = new(ELeave)CGuiConsAppView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}
	
_LIT(KBgImage, "\\resource\\guiconsbg.png");

void CGuiConsAppView::ConstructL(const TRect& aRect)
	{
	CreateWindowL();

	iEditor = new(ELeave) CEdwinThatDoesntConsumeEnter;

	iEditor->ConstructL(
		CEikEdwin::EAvkonEditor | CEikEdwin::ENoWrap | CEikEdwin::ELineCursor | CEikEdwin::ENoLineOrParaBreaks,
		20, // width in chars
		0,
		1); // number of lines
/*    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC(reader, R_GUICONS_LINEEDITOR_EDWIN);

	iEditor->ConstructFromResourceL(reader);
	CleanupStack::PopAndDestroy();*/

	iEditor->SetBorder(TGulBorder::ESingleDotted);
	iEditor->SetContainerWindowL(*this);
	
	iLabel = new(ELeave)CEikLabel();
	iLabel->SetContainerWindowL(*this);
	iLabel->SetFont(CEikonEnv::Static()->DenseFont());
	iLabel->SetAlignment(EHCenterVCenter);
	iLabel->OverrideColorL(EColorLabelText, 0x808080);
	
	TRAP_IGNORE(iBgImage = CImageDecodeAo::NewL(*this, KBgImage));

	SetRect(aRect);
	}
	
void CGuiConsAppView::SetLargeControlL(CCoeControl* aControl)
	{
	__ASSERT_ALWAYS(!iLargeControl, Panic(EGuiConsControlAlreadyExists));
	if (!aControl->DrawableWindow())
		{
		aControl->SetContainerWindowL(*this);
		}
	iLargeControl = aControl;
	Layout();
	//DrawNow();
	DrawDeferred();
	}

void CGuiConsAppView::ClearLargeControl()
	{
	iLargeControl = NULL;
	//DrawNow();
	DrawDeferred();
	}
	
void CGuiConsAppView::ShowLabelL(const TDesC& aText)
	{
	iLabel->SetTextL(aText);
	iLabel->MakeVisible(ETrue);
	iEditor->MakeVisible(EFalse);
	DrawNow();
	}
	
void CGuiConsAppView::ShowEditor()
	{
	iLabel->MakeVisible(EFalse);
	iEditor->MakeVisible(ETrue);
	DrawNow();
	}
	
void CGuiConsAppView::FocusEditor(TBool aFocus)
	{
	iEditor->SetDimmed(!aFocus);
	iEditor->SetFocusing(aFocus);
	iEditor->SetFocus(aFocus);
	iEditor->SetReadOnly(!aFocus);
	}
	
void CGuiConsAppView::AddEditorToStackL(CCoeAppUi* aAppUi)
	{
	aAppUi->AddToStackL(iEditor);
	}
	
void CGuiConsAppView::RemoveEditorFromStack(CCoeAppUi* aAppUi)
	{
	aAppUi->RemoveFromStack(iEditor);
	}
	
HBufC* CGuiConsAppView::GetEditorTextL()
	{
	return iEditor->GetTextInHBufL();
	}
	
void CGuiConsAppView::SetEditorTextL(const TDesC* aText)
	{
	iEditor->SetTextL(aText);
	}

CGuiConsAppView::CGuiConsAppView()
	{
	}

CGuiConsAppView::~CGuiConsAppView()
	{
	delete iEditor;
	delete iLabel;
	delete iBgImage;
	}

void CGuiConsAppView::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
	gc.Clear( aRect );

	// JB - This doesn't seem to work reliably and it messes up scrolling.
/*	TGulBorder border = iEditor->Border();
	TRect editorRect = iEditor->Rect();
	border.Draw(gc, border.OuterRect(editorRect));
 */   
    if ((!iLargeControl) && (iBgImage) && (iBgImage->Bitmap()))
    	{
    	CFbsBitmap* bg = iBgImage->Bitmap();
    	TSize bgSize = bg->SizeInPixels();
    	TPoint myCentre = Rect().iTl + TPoint(Size().iWidth / 2, Size().iHeight/2);
    	TPoint imgTl = myCentre - TPoint(bgSize.iWidth / 2, bgSize.iHeight / 2);
    	iBgImage->Bitblt(gc, imgTl);
    	}
	}
	
void CGuiConsAppView::BgImageReady()
	{
	if (!iLargeControl) DrawNow();
	}

void CGuiConsAppView::SizeChanged()
	{  
	Layout();
	DrawNow();
	}
	
TInt CGuiConsAppView::CountComponentControls() const
	{
	TInt count = 2;
	if (iLargeControl) count++;
	return count;
	}
	
CCoeControl* CGuiConsAppView::ComponentControl(TInt aIndex) const
	{
	switch (aIndex)
		{
	case 0:
		return iLabel;
	case 1:
		return iEditor;
	case 2:
		if (iLargeControl)
			{
			return iLargeControl;
			}
		// fallthrough
	default:
		Panic(EGuiConsBadControlIndex);
		return NULL; // keep compiler quiet
		}
	}
	
void CGuiConsAppView::Layout()
	{
	TRect myRect = Rect();
	TSize largeSize(myRect.Size());	


	TSize editorSize(myRect.Width(), iEditor->MinimumSize().iHeight);
	TRect editorRect(myRect.iBr - editorSize.AsPoint(), editorSize);
	iEditor->SetRect(editorRect);
	iLabel->SetRect(editorRect);
	
	
	largeSize.iHeight -= editorSize.iHeight;		
	
	if (iLargeControl)
		{
		TRect largeRect(myRect.iTl, largeSize);
		iLargeControl->SetRect(largeRect);
		}
	}

//______________________________________________________________________________
//						CImageDecodeAo
CImageDecodeAo* CImageDecodeAo::NewL(CGuiConsAppView& aOwner, const TDesC& aFileName)
	{
	CImageDecodeAo* self = new(ELeave)CImageDecodeAo(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName);
	CleanupStack::Pop(self);
	return self;
	}
	
CImageDecodeAo::~CImageDecodeAo()
	{
	Cancel();
	delete iDecoder;
	delete iBitmap;
	delete iMask;
	iFs.Close();
	}
	
CFbsBitmap* CImageDecodeAo::Bitmap()
	{
	return iBitmapReady ? iBitmap : NULL;
	}
	
void CImageDecodeAo::Bitblt(CWindowGc& aGc, TPoint aPosition)
	{
	if (!Bitmap()) return;
   	aGc.BitBltMasked(aPosition, iBitmap, iBitmap->SizeInPixels(), iMask, EFalse);
	}

CImageDecodeAo::CImageDecodeAo(CGuiConsAppView& aOwner)
	: CActive(CActive::EPriorityLow), iOwner(aOwner) 
	{
	CActiveScheduler::Add(this);
	}
	
void CImageDecodeAo::ConstructL(const TDesC& aFileName)
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareAuto());
	TFindFile ff(iFs);
	User::LeaveIfError(ff.FindByDir(aFileName, KNullDesC));
	
	iDecoder = CImageDecoder::FileNewL(iFs, ff.File());
	iBitmap = new(ELeave)CFbsBitmap;
	iMask = new(ELeave)CFbsBitmap;
	User::LeaveIfError(iBitmap->Create(iDecoder->FrameInfo().iOverallSizeInPixels, iDecoder->FrameInfo().iFrameDisplayMode));
	User::LeaveIfError(iMask->Create(iDecoder->FrameInfo().iOverallSizeInPixels, EGray2));
	iDecoder->Convert(&iStatus, *iBitmap, *iMask);
	SetActive();
	}

void CImageDecodeAo::RunL()
	{
	if (iStatus.Int()==0)
		{
		iBitmapReady = ETrue;
		iOwner.BgImageReady();
		}
	}
	
void CImageDecodeAo::DoCancel()
	{
	iDecoder->Cancel();
	}
