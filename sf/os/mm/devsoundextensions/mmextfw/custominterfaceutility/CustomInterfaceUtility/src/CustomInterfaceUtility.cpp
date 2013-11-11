/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CustomInterface Utility Proxy implementation
*
*/



// INCLUDE FILES
#include "CustomInterfaceUtility.h"
#include <ProxyCustomInterfaceUtility.h>
#include "CustomCommandUtility.h"
#include <sounddevice.h>
#include <mdaaudioinputstream.h>
#include <DrmAudioSamplePlayer.h>
#include <videoplayer.h>
#include <videorecorder.h>

EXPORT_C CCustomInterfaceUtility* CCustomInterfaceUtility::NewL(CMdaAudioConvertUtility& aUtility)
	{
	CCustomCommandUtility* customCommandUtility = CCustomCommandUtility::NewL(aUtility);
	CProxyCustomInterfaceUtility* util = CProxyCustomInterfaceUtility::NewL(customCommandUtility);

	return util;
	}

EXPORT_C CCustomInterfaceUtility* CCustomInterfaceUtility::NewL(CMdaAudioPlayerUtility& aUtility)
	{
	CCustomCommandUtility* customCommandUtility = CCustomCommandUtility::NewL(aUtility);
	CProxyCustomInterfaceUtility* util = CProxyCustomInterfaceUtility::NewL(customCommandUtility);

	return util;
	}

EXPORT_C CCustomInterfaceUtility* CCustomInterfaceUtility::NewL(CMdaAudioRecorderUtility& aUtility,
                                                                TBool aRecordStream)
	{
	CCustomCommandUtility* customCommandUtility = CCustomCommandUtility::NewL(aUtility,aRecordStream);
	CProxyCustomInterfaceUtility* util = CProxyCustomInterfaceUtility::NewL(customCommandUtility);

	return util;
	}

EXPORT_C CCustomInterfaceUtility* CCustomInterfaceUtility::NewL(CCustomCommandUtility* aCustomCommandUtility)
	{
	//CCustomInterfaceUtility* self = new(ELeave) CCustomInterfaceUtility();
	//CleanupStack::PushL(self);
	//self->ConstructL(aCustomCommandUtility);
	//CleanupStack::Pop(self);
	CProxyCustomInterfaceUtility* util = CProxyCustomInterfaceUtility::NewL(aCustomCommandUtility);
	return util;
	}
EXPORT_C CCustomInterfaceUtility* CCustomInterfaceUtility::NewL(CMMFDevSound& /*aDevSound*/)
	{
	User::Leave(KErrNotSupported);
	CCustomInterfaceUtility* self = NULL; 
	return self;
/*****
	CCustomInterfaceUtility* self = new(ELeave) CCustomInterfaceUtility();
	CleanupStack::PushL(self);
	self->ConstructL(aDevSound);
	CleanupStack::Pop(self);
	return self;
*****/
	}

EXPORT_C CCustomInterfaceUtility* CCustomInterfaceUtility::NewL(CMidiClientUtility& aUtility)
	{
	CCustomCommandUtility* customCommandUtility = CCustomCommandUtility::NewL(aUtility);
	CProxyCustomInterfaceUtility* util = CProxyCustomInterfaceUtility::NewL(customCommandUtility);

	return util;
	}

EXPORT_C CCustomInterfaceUtility* CCustomInterfaceUtility::NewL(MCustomCommand& aUtility)
	{
	CProxyCustomInterfaceUtility* util = CProxyCustomInterfaceUtility::NewL(aUtility);

	return util;
	}

EXPORT_C CCustomInterfaceUtility* CCustomInterfaceUtility::NewL(CDrmPlayerUtility& aUtility)
	{
	CCustomCommandUtility* customCommandUtility = CCustomCommandUtility::NewL(aUtility);
	CProxyCustomInterfaceUtility* util = CProxyCustomInterfaceUtility::NewL(customCommandUtility);

	return util;
	}

EXPORT_C CCustomInterfaceUtility* CCustomInterfaceUtility::NewL(CVideoPlayerUtility& aUtility)
	{
	CCustomCommandUtility* customCommandUtility = CCustomCommandUtility::NewL(aUtility);
	CProxyCustomInterfaceUtility* util = CProxyCustomInterfaceUtility::NewL(customCommandUtility);

	return util;
	}

EXPORT_C CCustomInterfaceUtility* CCustomInterfaceUtility::NewL(CVideoRecorderUtility& aUtility)
	{
	CCustomCommandUtility* customCommandUtility = CCustomCommandUtility::NewL(aUtility);
	CProxyCustomInterfaceUtility* util = CProxyCustomInterfaceUtility::NewL(customCommandUtility);

	return util;
	}
		
EXPORT_C CCustomInterfaceUtility::CCustomInterfaceUtility()
	{
	}

void CCustomInterfaceUtility::ConstructL(CCustomCommandUtility* aCustomCommandUtility)
	{
	iProxyFactory = CProxyCustomInterfaceUtility::NewL(aCustomCommandUtility);
	}

void CCustomInterfaceUtility::ConstructL(CMMFDevSound& aDevSound)
	{
	iDevSound = &aDevSound;
	}
EXPORT_C CCustomInterfaceUtility::~CCustomInterfaceUtility()
	{
	delete iProxyFactory;
	}

TAny* CCustomInterfaceUtility::CustomInterface(TUid aInterfaceId)
	{
	TAny* result;

	if (iDevSound)
		{
		result = iDevSound->CustomInterface(aInterfaceId);
//		delete this; //protect from memory leaks in clients
		}
	else
		{
	result = iProxyFactory->CustomInterface(aInterfaceId);
		}
	return result;
	}

void CCustomInterfaceUtility::RemoveCustomInterface(TMMFMessageDestinationPckg aMessageHandler)
	{

	iProxyFactory->RemoveCustomInterface(aMessageHandler);

	}


// End of File
