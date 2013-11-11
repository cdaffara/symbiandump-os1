// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <s32file.h>
#include <fbs.h>
#include <bitmap.h>
#include <graphics/bitmapuid.h>
#include "fbsbitmapasyncstreamer.h"

CFbsBitmapAsyncStreamer::CFbsBitmapAsyncStreamer(TMode aMode):
	CBase(),
	iFbs(RFbsSession::GetSession()),
	iMode(aMode)
	{}

EXPORT_C CFbsBitmapAsyncStreamer::~CFbsBitmapAsyncStreamer()
	{
	iReadStream.Close();
	iWriteStream.Close();
	delete iStore;
	if(iMode==ELoad) delete iBitmap;
	}

EXPORT_C CFbsBitmapAsyncStreamer* CFbsBitmapAsyncStreamer::NewL(TMode aMode)
	{
	CFbsBitmapAsyncStreamer* thisptr=new(ELeave) CFbsBitmapAsyncStreamer(aMode);
	CleanupStack::PushL(thisptr);
	thisptr->ConstructL();
	CleanupStack::Pop();
	return(thisptr);
	}

void CFbsBitmapAsyncStreamer::ConstructL()
	{
	if(iMode==ELoad) iBitmap=new(ELeave) CFbsBitmap;
	}

EXPORT_C TInt CFbsBitmapAsyncStreamer::Load(const TDesC& aFilename,TInt32 aId,TInt& aScanLines)
	{
	if(iMode!=ELoad) return(KErrGeneral);

	TRAPD(ret,DoLoadL(aFilename,aId));
	if(ret!=KErrNone) return(ret);

	iDispMode = CBitwiseBitmap::DisplayMode(iHeader.iBitsPerPixel,iHeader.iColor);

	ret=iBitmap->Create(iHeader.iSizeInPixels,iDispMode);
	iBitmap->BeginDataAccess();
	iBitmap->Address()->iHeader=iHeader;
	if(ret!=KErrNone) return(ret);

	aScanLines=iHeader.iSizeInPixels.iHeight;
	iScanLineBase=(TUint32*)iBitmap->DataAddress();
	return(KErrNone);
	}

void CFbsBitmapAsyncStreamer::DoLoadL(const TDesC& aFilename,TInt32 aId)
	{
	iStore=CDirectFileStore::OpenL(iFbs->FileServer(),aFilename,EFileShareReadersOnly);
	if (iStore->Type()[1]!=KMultiBitmapFileImageUid)
		User::LeaveIfError(KErrGeneral);
	TStreamId streamid=iStore->Root();
	RStoreReadStream readstream;
	readstream.OpenL(*iStore,streamid);
	TInt numbitmaps=readstream.ReadInt32L();
	if(numbitmaps<aId+1)
		User::Leave(KErrUnknown);
	TStreamId bmpstreamid;
	bmpstreamid.InternalizeL(readstream);
	for(TInt count=0;count<aId;count++)
		bmpstreamid.InternalizeL(readstream);
	readstream.Close();
	iReadStream.OpenL(*iStore,bmpstreamid);
	CBitwiseBitmap::InternalizeHeaderL(iReadStream,iHeader);
	}

EXPORT_C TBool CFbsBitmapAsyncStreamer::LoadScanLinesL(TInt aNumberOfScanLines,CFbsBitmap*& aBitmap)
	{
	if(iMode!=ELoad || iBitmap==NULL) User::Leave(KErrGeneral);
	if(iHeader.iCompression!=ENoBitmapCompression)
		{
		TRAP_IGNORE(iBitmap->Address()->DoInternalizeL(iReadStream,iHeader.iBitmapSize-iHeader.iStructSize,iBitmap->DataAddress()));
		iBitmap->EndDataAccess(EFalse);
		aBitmap=iBitmap;
		iBitmap=NULL;
		return(ETrue);
		}
	aNumberOfScanLines=Abs(aNumberOfScanLines);
	if(aNumberOfScanLines+iCurrentScanLine>iHeader.iSizeInPixels.iHeight)
		aNumberOfScanLines=iHeader.iSizeInPixels.iHeight-iCurrentScanLine;
	TInt slbytes = CBitwiseBitmap::ByteWidth(iHeader.iSizeInPixels.iWidth,iDispMode);
	TUint8* sladd=(TUint8*)iBitmap->Address()->ScanLineAddress(iScanLineBase,iCurrentScanLine);
	iReadStream.ReadL(sladd,aNumberOfScanLines*slbytes);
	iCurrentScanLine+=aNumberOfScanLines;
	if(iCurrentScanLine==iHeader.iSizeInPixels.iHeight)
		{
		iBitmap->EndDataAccess(EFalse);
		aBitmap=iBitmap;
		iBitmap=NULL;
		return(ETrue);
		}
	return(EFalse);
	}

EXPORT_C TInt CFbsBitmapAsyncStreamer::Save(const TDesC& aFilename,CFbsBitmap* aBitmap,TInt32& aId,TInt& aScanLines)
	{
	if(iMode!=ESave || aBitmap==NULL || !aBitmap->iHandle) return(KErrGeneral);
	RFile file;
	TInt ret=file.Replace(iFbs->FileServer(),aFilename,EFileWrite);
	if(ret!=KErrNone) return(ret);
	iHeader=aBitmap->Header();

	iDispMode = CBitwiseBitmap::DisplayMode(iHeader.iBitsPerPixel,iHeader.iColor);

	TRAP(ret,DoSaveL(file));
	if(ret!=KErrNone) return(ret);
	aId=0;
	iBitmap=aBitmap;
	iBitmap->BeginDataAccess();
	aScanLines=iHeader.iSizeInPixels.iHeight;
	iScanLineBase=(TUint32*)aBitmap->DataAddress();
	return(KErrNone);
	}

void CFbsBitmapAsyncStreamer::DoSaveL(RFile& aFile)
	{
	iStore=CDirectFileStore::NewL(aFile);
	TUidType uidtype(KDirectFileStoreLayoutUid,KMultiBitmapFileImageUid);
	iStore->SetTypeL(uidtype);
	iId=iWriteStream.CreateL(*iStore);
	iWriteStream.WriteInt32L(iHeader.iBitmapSize);
	iWriteStream.WriteInt32L(iHeader.iStructSize);
	iWriteStream.WriteInt32L(iHeader.iSizeInPixels.iWidth);
	iWriteStream.WriteInt32L(iHeader.iSizeInPixels.iHeight);
	iWriteStream.WriteInt32L(iHeader.iSizeInTwips.iWidth);
	iWriteStream.WriteInt32L(iHeader.iSizeInTwips.iHeight);
	iWriteStream.WriteInt32L(iHeader.iBitsPerPixel);
	iWriteStream.WriteUint32L(iHeader.iColor);
	iWriteStream.WriteInt32L(0);
	iWriteStream.WriteUint32L(iHeader.iCompression);
	}

EXPORT_C TBool CFbsBitmapAsyncStreamer::SaveScanLinesL(TInt aNumberOfScanLines)
	{
	if(iMode!=ESave || iBitmap==NULL) User::Leave(KErrGeneral);
	aNumberOfScanLines=Abs(aNumberOfScanLines);
	if(aNumberOfScanLines+iCurrentScanLine>iHeader.iSizeInPixels.iHeight)
		aNumberOfScanLines=iHeader.iSizeInPixels.iHeight-iCurrentScanLine;
	TInt slbytes = CBitwiseBitmap::ByteWidth(iHeader.iSizeInPixels.iWidth,iDispMode);
	TUint8* sladd=(TUint8*)iBitmap->Address()->ScanLineAddress(iScanLineBase,iCurrentScanLine);
	iWriteStream.WriteL(sladd,aNumberOfScanLines*slbytes);
	iCurrentScanLine+=aNumberOfScanLines;
	if(iCurrentScanLine==iHeader.iSizeInPixels.iHeight)
		{
		iBitmap->EndDataAccess(ETrue);
		iWriteStream.Close();
		TStreamId headstreamid=iWriteStream.CreateL(*iStore);
		iWriteStream.WriteInt32L(1); // number of bitmaps
		iId.ExternalizeL(iWriteStream); // stream id of bitmap
		iWriteStream.Close();
		iStore->SetRootL(headstreamid);
		delete iStore;
		iStore=NULL;
		return(ETrue);
		}
	return(EFalse);
	}

