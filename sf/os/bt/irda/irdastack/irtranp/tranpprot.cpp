// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "debug.h"
#include <e32std.h>
#include <e32base.h>
#include <e32svr.h>
#include "tranp.h"
#include "tranpprot.h"
#include "BFTP.H"
#include "GLOBAL.H"

CTranpProtocol::CTranpProtocol(MTranpNotification& aNotifier) : iNotifier(aNotifier)
	{
	}

CTranpProtocol::~CTranpProtocol()
	{
	delete iBFTP;
	}

CTranpProtocol* CTranpProtocol::NewL(MTranpNotification& aNotifier)
	{
	DEBUG_OPEN();
	SESSIONPRINT(_L("CTranpProtocol::NewL()\n"));
	CTranpProtocol* self = new(ELeave) CTranpProtocol(aNotifier);
	CleanupStack::PushL(self);
	self->ConstructL(aNotifier);
	CleanupStack::Pop();
	return self;
	}

void CTranpProtocol::ConstructL(const MTranpNotification& aNotifier)
	{
	iBFTP = BFTP::NewL(this, &aNotifier);
	TUint8 MaxPDU = '4'; // 4096 bytes PDU

	iBFTP->SetPMID(KDefaultPMID);
	iBFTP->SetSMID(KNullPMID);
	iBFTP->SetPDU(MaxPDU);

	}

void CTranpProtocol::Connect()
	{
	}

void CTranpProtocol::Disconnect()
	{
	}

void CTranpProtocol::Abort()
	{
	iBFTP->Abort();
	}
    
    
void CTranpProtocol::Query(CTranpSession::TTranP /*aWhat*/)
	{
	}
    
void CTranpProtocol::Get(TTranpPicture& aPicture)
	{
	iTranpPicture = &aPicture;
	iBFTP->Pump(iTranpPicture->iLatticeSize); //Start the ball rolling...
	}
    
void CTranpProtocol::Put(const TTranpPicture& /*aPicture*/)
	{
	}
	
void CTranpProtocol::Config(const TTranpConfig& /*aConfig*/)
	{
	}

TTranpConfig CTranpProtocol::Config(const TTranpConfig& aConfig) const
	{
	return aConfig;
	}

void CTranpProtocol::SetPicture(TDesC8& /*aPicture*/, TDesC8& aFilename, TDesC8& aUPFPicture, TDesC8& aLongFilename)
// This function needs to take account of exact location of image data so iPicture needs
// to be set from image address location specified in UPF header
   	{
 	// Full UPF image information
 	iTranpPicture->iUPFPicture = &aUPFPicture;
 	
 	// Thumbnail image information is stored in UPF Entry 1
 	// UPF file bytes 240-243 contain address of thumbnail
 	TUint UPFThumbnailAddress = IrTranpUtil::LExtract(iTranpPicture->iUPFPicture->Mid(KTranpUPFEntryArea1,4) , 0); //address is 4 bytes long
 	// UPF file bytes 244-247 contain size of thumbnail
 	TInt UPFThumbnailSize = IrTranpUtil::LExtract(iTranpPicture->iUPFPicture->Mid(KTranpUPFEntryArea1+4,4) , 0); //size is 4 bytes long + offset by 4

	if (UPFThumbnailAddress != KNullImageAddress) // Thumbnail Image exists
		iTranpPicture->iThumbNail.Set(iTranpPicture->iUPFPicture->Mid(KTranpUPFHeader + UPFThumbnailAddress,UPFThumbnailSize));
 
 	// Full image information is stored in UPF Entry Area 2
 	// UPF file bytes 276-279 contain address of image
 	TUint UPFImageAddress = IrTranpUtil::LExtract(iTranpPicture->iUPFPicture->Mid(KTranpUPFEntryArea2,4) , 0); //address is 4 bytes long
 	// UPF file bytes 279-282 contain size of image
 	TInt UPFImageSize = IrTranpUtil::LExtract(iTranpPicture->iUPFPicture->Mid(KTranpUPFEntryArea2+4,4) , 0); //address is 4 bytes long  + offset by 4
 
	if (UPFImageAddress != KNullImageAddress) // Image exists
 		iTranpPicture->iPicture.Set(iTranpPicture->iUPFPicture->Mid(KTranpUPFHeader + UPFImageAddress,UPFImageSize));

   	iTranpPicture->iFilename = aFilename;
   	iTranpPicture->iLongFilename = aLongFilename;

	// Set picture creation date
	TPtr8 timePtr((unsigned char*) aUPFPicture.Ptr() + KTranpDateOffset, 8, 8);
	
	iTranpPicture->iTime.Format(_L8("%04d%02d%02d%02d%02d%02d"),IrTranpUtil::DExtract(timePtr, KTranpYearOffset),
														 timePtr[KTranpMonthOffset],timePtr[KTranpDayOffset],
														 timePtr[KTranpHourOffset],timePtr[KTranpMinuteOffset],
														 timePtr[KTranpSecondOffset]);
	
	// Set Picture Size
	TPtr8 pictureSizePtr((unsigned char*) aUPFPicture.Ptr() + KTranpPictureInfOffset, 4, 4);
	
	iTranpPicture->iWidth = IrTranpUtil::DExtract(pictureSizePtr, KTranpPictureXSizeOffset);
	iTranpPicture->iHeight = IrTranpUtil::DExtract(pictureSizePtr, KTranpPictureYSizeOffset);
	
	// QVGA:=320x240, VGA:=640x480, SVGA:=800x600, XGA:=1024x768, SXGA:=1280x960, FREE:=m x n

	switch (iTranpPicture->iWidth)
		{
	case 320:
	case 240:
		iTranpPicture->iLatticeSize = EQVGA;
		break;
	case 640:
	case 480:
		iTranpPicture->iLatticeSize = EVGA;
		break;
	case 800:
	case 600:
		iTranpPicture->iLatticeSize = ESVGA;
		break;
	case 1024:
	case 768:
		iTranpPicture->iLatticeSize = EXGA;
		break;
	case 1280:
	case 960:
		iTranpPicture->iLatticeSize = ESXGA;
		break;
	default:
		iTranpPicture->iLatticeSize = EFREE;
		break;
		}
	}
