// grabscreen.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <w32std.h>
#include <fbs.h>
#include <bitdev.h>
#include <imageconversion.h>

using namespace IoUtils;


class CCmdGrabscreen : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdGrabscreen();
private:
	enum TFormat
		{
		EJpeg,
		EGif,
		EBmp,
		EPng,
		ERaw
		};
private:
	CCmdGrabscreen();
	void WriteEncodedL(TUid aEncodingUid);
	void WriteRawL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	
	virtual void RunL();
private:
	TFileName2 iFileName;
	CImageEncoder* iImageEncoder;
	HBufC8* iImageData;
	CFrameImageData* iFrameImageData;
	RFile iFile;
	TFormat iFormat;
	RArray<TInt> iRect;
	TInt iScreen;
	CFbsBitmap* iBitMap;
	CWsScreenDevice* iDev;
	RWsSession iWsSession;
	TInt iQuality;
	TBool iGreyscale;
	};


CCommandBase* CCmdGrabscreen::NewLC()
	{
	CCmdGrabscreen* self = new(ELeave) CCmdGrabscreen();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdGrabscreen::~CCmdGrabscreen()
	{
	delete iImageEncoder;
	delete iFrameImageData;
	delete iDev;
	delete iBitMap;
	delete iImageData;
	iFile.Close();
	iRect.Close();
	iWsSession.Close();
	REComSession::FinalClose();
	}

CCmdGrabscreen::CCmdGrabscreen() 
	: CCommandBase(EManualComplete), iQuality(100)
	{
	}

const TDesC& CCmdGrabscreen::Name() const
	{
	_LIT(KName, "grabscreen");	
	return KName;
	}

void CCmdGrabscreen::DoRunL()
	{
	if ((iFormat == ERaw) && (!iArguments.IsPresent(0)))
		{
		PrintError(KErrArgument, _L("STDOUT writing is not supported for raw format; please specify a filename"));
		DisplayHelp();
		User::Leave(KErrArgument);
		}
	
	LeaveIfErr(iWsSession.Connect(), _L("Couldn't connect to windowserver"));
	iDev = new(ELeave) CWsScreenDevice(iWsSession);
#ifdef SYMBIAN_WSERV_AND_CONE_MULTIPLE_SCREENS
	const TInt numScreens = iWsSession.NumberOfScreens();
	if (iScreen >= numScreens)
		{
		if (numScreens == 1)
			{
			LeaveIfErr(KErrArgument, _L("Invalid screen number - there is only 1 screen"));
			}
		else
			{
			LeaveIfErr(KErrArgument, _L("Invalid screen number - there are only %d screens"));
			}
		}
#endif
	LeaveIfErr(iDev->Construct(iScreen), _L("Couldn't construct CWsScreenDevice for screen %d"), iScreen);
	iBitMap = new(ELeave) CFbsBitmap();

	TRect rect;
	if (iOptions.IsPresent(&iRect))
		{
		if (iRect.Count() != 4)
			{
			LeaveIfErr(KErrArgument, _L("Invalid --rect option - four integers must be specifed"));
			}
		rect.SetRect(iRect[0], iRect[1], iRect[2], iRect[3]);
		}
	else
		{
		rect.SetRect(TPoint(0, 0), iDev->SizeInPixels());
		}
	
	LeaveIfErr(iBitMap->Create(rect.Size(), iDev->DisplayMode()), _L("Couldn't create fbsbitmap of size (%d,%d) for display mode %d"), rect.Width(), rect.Height(), iDev->DisplayMode());
	User::LeaveIfError(iDev->CopyScreenToBitmap(iBitMap, rect));

	switch (iFormat)
		{
		case EJpeg:
			WriteEncodedL(KImageTypeJPGUid);
			break;
		case EGif:
			WriteEncodedL(KImageTypeGIFUid);
			break;
		case EBmp:
			WriteEncodedL(KImageTypeBMPUid);
			break;
		case EPng:
			WriteEncodedL(KImageTypePNGUid);
			break;
		case ERaw:
			WriteRawL();
			break;
		}
	}

void CCmdGrabscreen::WriteEncodedL(TUid aEncodingUid)
	{
	if (aEncodingUid == KImageTypeJPGUid)
		{
		TJpegImageData* imageData = new(ELeave) TJpegImageData();
		CleanupStack::PushL(imageData);
		imageData->iSampleScheme = iGreyscale ? TJpegImageData::EMonochrome : TJpegImageData::EColor444;
		imageData->iQualityFactor = iQuality;
		TRAPL(iFrameImageData = CFrameImageData::NewL(), _L("Couldn't create CFrameImageData"));
		LeaveIfErr(iFrameImageData->AppendImageData(imageData), _L("Failed to write image data to TJpegImageData"));
		CleanupStack::Pop(imageData);
		}
	else if (aEncodingUid == KImageTypeGIFUid)
		{
		if (iGreyscale)
			{
			PrintWarning(_L("Greyscale not supported for GIF images."));
			}
		if (iOptions.IsPresent(&iQuality))
			{
			PrintWarning(_L("Quality control not supported for GIF images."));
			}
		}
	else if (aEncodingUid == KImageTypeBMPUid)
		{
		if (iGreyscale)
			{
			PrintWarning(_L("Greyscale not supported for BMP images."));
			}
		if (iOptions.IsPresent(&iQuality))
			{
			PrintWarning(_L("Quality control not supported for BMP images."));
			}
		}
	else if (aEncodingUid == KImageTypePNGUid)
		{
		if (iOptions.IsPresent(&iQuality))
			{
			PrintWarning(_L("Quality control not supported for PNG images."));
			}

		TPngEncodeData* imageData = new(ELeave) TPngEncodeData();
		CleanupStack::PushL(imageData);
		imageData->iBitsPerPixel = iGreyscale ? 4 : 8;
		imageData->iColor = !iGreyscale;
		imageData->iLevel = TPngEncodeData::EBestCompression;
		imageData->iPaletted = EFalse;
		iFrameImageData = CFrameImageData::NewL();
		User::LeaveIfError(iFrameImageData->AppendFrameData(imageData));
		CleanupStack::Pop(imageData);
		}
		
	if (iArguments.IsPresent(0))
		{
		LeaveIfErr(iFile.Replace(FsL(), iFileName, EFileWrite), _L("Unable to open \"%S\" for writing"), &iFileName);
		iImageEncoder = CImageEncoder::FileNewL(iFile, CImageEncoder::EOptionNone, aEncodingUid);
		}
	else
		{
		iImageEncoder = CImageEncoder::DataNewL(iImageData, CImageEncoder::EOptionNone, aEncodingUid);
		}
	iImageEncoder->Convert(&iStatus, *iBitMap, iFrameImageData);
	SetActive();
	}
	
void CCmdGrabscreen::RunL()
	{
	LeaveIfErr(iStatus.Int(), _L("Error returned from CImageEncoder::Convert"));
	if (!iArguments.IsPresent(0))
		{
		User::LeaveIfNull(iImageData);
		
		Stdout().SetModeL(RIoReadWriteHandle::EBinary);
		
		// we need to expand the 8 bit data when writing to stdout
		static const TInt KBufferSize = 0x100;
		TBuf<KBufferSize> buf;
		TPtr8 imageData(iImageData->Des());
		while (imageData.Length())
			{
			TInt thisCopyLen = Min(KBufferSize, imageData.Length());
			buf.Copy(imageData.Left(thisCopyLen));
			imageData.Set(imageData.MidTPtr(thisCopyLen));
			Stdout().WriteL(buf);
			}
		}
	Complete(KErrNone);
	}

void CCmdGrabscreen::WriteRawL()
	{
	if (iGreyscale)
		{
		PrintWarning(_L("Greyscale not supported for raw images."));
		}
	if (iOptions.IsPresent(&iQuality))
		{
		PrintWarning(_L("Quality control not supported for raw images."));
		}
	LeaveIfErr(iBitMap->Save(iFileName), _L("Unable to write \"%S\""), &iFileName);
	Complete(KErrNone);
	}

void CCmdGrabscreen::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArg, "file_name");
	aArguments.AppendFileNameL(iFileName, KArg);
	}

void CCmdGrabscreen::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptFormat, "format");
	aOptions.AppendEnumL((TInt&)iFormat, KOptFormat);

	_LIT(KOptRect, "rect");
	aOptions.AppendIntL(iRect, KOptRect);

	_LIT(KOptScreen, "screen");
	aOptions.AppendIntL(iScreen, KOptScreen);

	_LIT(KOptQuality, "quality");
	aOptions.AppendIntL(iQuality, KOptQuality);

	_LIT(KOptGreyscale, "greyscale");
	aOptions.AppendBoolL(iGreyscale, KOptGreyscale);
	}


EXE_BOILER_PLATE(CCmdGrabscreen)

