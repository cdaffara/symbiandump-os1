// genbmp.cpp
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

#include <fshell/ioutils.h>
#include <gdi.h>
#include <fbs.h>
#include <imageconversion.h>

using namespace IoUtils;

class CCmdGenbmp : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdGenbmp();
private:
	CCmdGenbmp();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private: // From CActive.
	virtual void RunL();
private:
	RFbsSession iFbsSession;
	CFbsBitmap* iBitmap;
	CFbsBitmapDevice* iBitmapDevice;
	CBitmapContext* iBitmapContext;
	CImageEncoder* iImageEncoder;
	RFile iFile;
	TFileName2 iFileName;
	RPointerArray<HBufC> iText;
	TSize iSize;
	TUint32 iForegroundColor;
	TUint32 iBackgroundColor;
	HBufC* iTypefaceName;
	TInt iMaxFontHeight;
	TInt iGap;
	};


CCommandBase* CCmdGenbmp::NewLC()
	{
	CCmdGenbmp* self = new(ELeave) CCmdGenbmp();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdGenbmp::~CCmdGenbmp()
	{
	delete iTypefaceName;
	delete iImageEncoder;
	delete iBitmapContext;
	delete iBitmapDevice;
	delete iBitmap;
	iFbsSession.Disconnect();
	iFile.Close();
	iText.ResetAndDestroy();
	REComSession::FinalClose();
	}

CCmdGenbmp::CCmdGenbmp()
	: iSize(150, 150), iForegroundColor(0xffffff), iBackgroundColor(0x000000), iMaxFontHeight(15), iGap(10)
	{
	}

const TDesC& CCmdGenbmp::Name() const
	{
	_LIT(KName, "genbmp");	
	return KName;
	}

void CCmdGenbmp::DoRunL()
	{
	LeaveIfErr(iFbsSession.Connect(FsL()), _L("Couldn't connect to the Font & Bitmap Server"));
	iBitmap = new(ELeave)CFbsBitmap;
	User::LeaveIfError(iBitmap->Create(iSize, EColor16));
	iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);
	LeaveIfErr(iBitmapDevice->CreateBitmapContext(iBitmapContext), _L("Couldn't create bitmap context"));
	iBitmapContext->SetPenColor(iForegroundColor);
	iBitmapContext->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmapContext->SetBrushColor(iBackgroundColor);
	iBitmapContext->SetBrushStyle(CGraphicsContext::ESolidBrush);

	TFontSpec fontSpec;
	fontSpec.iTypeface.iName = iTypefaceName ? *iTypefaceName : _L("Arial");
	fontSpec.iTypeface.SetIsSymbol(EFalse);
	CFont* font;
	LeaveIfErr(iBitmapDevice->GetNearestFontToMaxHeightInPixels(font, fontSpec, iMaxFontHeight), _L("Couldn't get suitable font"));
	iBitmapContext->DrawRect(iSize);
	iBitmapContext->UseFont(font);

	TPoint linePos(iGap, iGap + iMaxFontHeight);
	const TInt numLines = iText.Count();
	for (TInt i = 0; i < numLines; ++i)
		{
		iBitmapContext->DrawText(*(iText[i]), linePos);
		linePos.iY += (iGap + iMaxFontHeight);
		}

	iBitmapContext->DiscardFont();

	LeaveIfErr(iFile.Replace(FsL(), iFileName, EFileWrite), _L("Unable to open \"%S\" for writing"), &iFileName);
	iImageEncoder = CImageEncoder::FileNewL(iFile, CImageEncoder::EOptionNone, KImageTypeBMPUid);
	iImageEncoder->Convert(&iStatus, *iBitmap);
	SetActive();
	}

void CCmdGenbmp::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgFileName, "file_name");
	aArguments.AppendFileNameL(iFileName, KArgFileName);

	_LIT(KArgText, "text");
	aArguments.AppendStringL(iText, KArgText);
	}

void CCmdGenbmp::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptForegroundColor, "foreground-color");
	aOptions.AppendUintL((TUint&)iForegroundColor, KOptForegroundColor);

	_LIT(KOptBackgroundColor, "background-color");
	aOptions.AppendUintL((TUint&)iBackgroundColor, KOptBackgroundColor);

	_LIT(KOptWidth, "width");
	aOptions.AppendIntL(iSize.iWidth, KOptWidth);

	_LIT(KOptHeight, "height");
	aOptions.AppendIntL(iSize.iHeight, KOptHeight);

	_LIT(KOptTypefaceName, "typeface");
	aOptions.AppendStringL(iTypefaceName, KOptTypefaceName);

	_LIT(KOptMaxFontHeight, "max-font-height");
	aOptions.AppendIntL(iMaxFontHeight, KOptMaxFontHeight);

	_LIT(KOptGap, "gap");
	aOptions.AppendIntL(iGap, KOptGap);
	}
	
void CCmdGenbmp::RunL()
	{
	LeaveIfErr(iStatus.Int(), _L("Encode to BMP failed"));
	Complete(KErrNone);
	}


EXE_BOILER_PLATE(CCmdGenbmp)

