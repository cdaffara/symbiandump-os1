// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mmfvideosurfacecustomcommands.h"

/**
@internalTechnology
@prototype

Class used when sending custom commands from the client API
to the video surface controller to get or set the video configuration.
*/
class TMMFVideoSurfaceConfig
	{
public:
	/**
	The surface ID for the display.
	*/
	TSurfaceId					iSurfaceId;
	/**
	The crop region currently applied to the image.
	*/
	TRect						iCropRectangle;
	/**
	The video picture pixel aspect ratio.
	*/
	TVideoAspectRatio			iPixelAspectRatio;
	};
	
/**
@internalComponent
*/
enum TMMFVideoPlaySurfaceSupportMessages
	{
	EMMFVideoPlaySurfaceSupportUseSurfaces,
	EMMFVideoPlaySurfaceSupportGetSurfaceParameters,
	EMMFVideoPlaySurfaceSupportSurfaceRemoved
	};

EXPORT_C RMMFVideoPlaySurfaceSupportCustomCommands::RMMFVideoPlaySurfaceSupportCustomCommands(RMMFController& aController) :
	RMMFCustomCommandsBase(aController, KUidInterfaceMMFVideoPlaySurfaceSupport)
	{
	}

EXPORT_C TInt RMMFVideoPlaySurfaceSupportCustomCommands::UseSurfaces() const
	{
	return iController.CustomCommandSync(iDestinationPckg,
										EMMFVideoPlaySurfaceSupportUseSurfaces,
										KNullDesC8,
										KNullDesC8);
	}

EXPORT_C TInt RMMFVideoPlaySurfaceSupportCustomCommands::GetSurfaceParameters(TSurfaceId& aSurfaceId,
													TRect& aCropRect, TVideoAspectRatio& aPixelAspectRatio) const
	{
	TPckgBuf<TMMFVideoSurfaceConfig> configPackage;
	
	TInt err = iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoPlaySurfaceSupportGetSurfaceParameters, 
										 KNullDesC8,
										 KNullDesC8,
										 configPackage);

	if (!err)
		{
		aSurfaceId = configPackage().iSurfaceId;
		aCropRect = configPackage().iCropRectangle;
		aPixelAspectRatio = configPackage().iPixelAspectRatio;
		}
	return err;
	}

EXPORT_C TInt RMMFVideoPlaySurfaceSupportCustomCommands::SurfaceRemoved(TSurfaceId& aSurfaceId) const
	{
	TPckgBuf<TMMFVideoSurfaceConfig> configPackage;
	
	configPackage().iSurfaceId = aSurfaceId;

	return iController.CustomCommandSync(iDestinationPckg, 
										 EMMFVideoPlaySurfaceSupportSurfaceRemoved, 
										 configPackage,
										 KNullDesC8);
	}

EXPORT_C CMMFVideoPlaySurfaceSupportCustomCommandParser* CMMFVideoPlaySurfaceSupportCustomCommandParser::NewL(
													MMMFVideoPlaySurfaceSupportCustomCommandImplementor& aImplementor)
	{
	return new(ELeave) CMMFVideoPlaySurfaceSupportCustomCommandParser(aImplementor);
	}

EXPORT_C CMMFVideoPlaySurfaceSupportCustomCommandParser::~CMMFVideoPlaySurfaceSupportCustomCommandParser()
	{
	}

CMMFVideoPlaySurfaceSupportCustomCommandParser::CMMFVideoPlaySurfaceSupportCustomCommandParser(
										MMMFVideoPlaySurfaceSupportCustomCommandImplementor& aImplementor) :
	CMMFCustomCommandParserBase(KUidInterfaceMMFVideoPlaySurfaceSupport),
	iImplementor(aImplementor)
	{
	}

void CMMFVideoPlaySurfaceSupportCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == KUidInterfaceMMFVideoPlaySurfaceSupport)
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error != KErrNone)
			{
			aMessage.Complete(error);
			}
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}

void CMMFVideoPlaySurfaceSupportCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	// Keep the same style of the implemetation for future maintainance and flexibility in
	// case of the asynchronous event.	
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case EMMFVideoPlaySurfaceSupportUseSurfaces:
		complete = DoUseSurfacesL(aMessage);
		break;
	case EMMFVideoPlaySurfaceSupportGetSurfaceParameters:
		complete = DoGetSurfaceParametersL(aMessage);
		break;
	case EMMFVideoPlaySurfaceSupportSurfaceRemoved:
		complete = DoSurfaceRemovedL(aMessage);
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}
	if (complete)
		{
		aMessage.Complete(KErrNone);
		}
	}

TBool CMMFVideoPlaySurfaceSupportCustomCommandParser::DoUseSurfacesL(TMMFMessage& /*aMessage*/)
	{
	iImplementor.MvpssUseSurfacesL();
	return ETrue;
	}

TBool CMMFVideoPlaySurfaceSupportCustomCommandParser::DoGetSurfaceParametersL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoSurfaceConfig> pckg;
	iImplementor.MvpssGetSurfaceParametersL(pckg().iSurfaceId, pckg().iCropRectangle, pckg().iPixelAspectRatio );
	aMessage.WriteDataToClientL(pckg);
	return ETrue;
	}

TBool CMMFVideoPlaySurfaceSupportCustomCommandParser::DoSurfaceRemovedL(TMMFMessage& aMessage)
	{
	TPckgBuf<TMMFVideoSurfaceConfig> pckg;

	aMessage.ReadData1FromClientL(pckg);
	iImplementor.MvpssSurfaceRemovedL(pckg().iSurfaceId);
	return ETrue;
	}


