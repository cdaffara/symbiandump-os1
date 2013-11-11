/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   CustomInterface Builder
*
*/


#include <CustomInterfaceBuilder.h>
#include <sounddevice.h>
#include <MessageHandlerFactory.h>
#include "AdaptationCustomInterfaceBuilder.h"
#include "CustomInterfaceBuilderTypes.h"

#include "RestrictedAudioOutputMessageHandler.h"
#include "AudioOutputMessageTypes.h"

const TUid KUidCustomInterfaceBuilderImpl = {0x10207A8E};

EXPORT_C CCustomInterfaceBuilder* CCustomInterfaceBuilder::NewL(CMMFObjectContainer& aObjectContainer,
                                                                CMMFDevSound&        aDevSound)
	{
	CCustomInterfaceBuilder* self = new(ELeave) CCustomInterfaceBuilder(aObjectContainer,
	                                                                    aDevSound);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCustomInterfaceBuilder* CCustomInterfaceBuilder::NewL(CMMFObjectContainer& aObjectContainer,
                                                                MCustomInterface& aCustomInterface)
	{
	CCustomInterfaceBuilder* self = new(ELeave) CCustomInterfaceBuilder(aObjectContainer,
	                                                                    aCustomInterface);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CCustomInterfaceBuilder::CCustomInterfaceBuilder(CMMFObjectContainer& aObjectContainer,
                                                 CMMFDevSound&        aDevSound) :
    CMMFObject(KUidCustomInterfaceBuilderImpl),
    iDevSound(&aDevSound),
	iObjectContainer(aObjectContainer),
	iSecureCustomInterfaces(EFalse)
	{
	}

CCustomInterfaceBuilder::CCustomInterfaceBuilder(CMMFObjectContainer& aObjectContainer,
                                                 MCustomInterface& aCustomInterface) :
    CMMFObject(KUidCustomInterfaceBuilderImpl),
	iCustomInterface(&aCustomInterface),
	iObjectContainer(aObjectContainer),
	iSecureCustomInterfaces(EFalse)
	{
	}

void CCustomInterfaceBuilder::ConstructL()
	{
	}

CCustomInterfaceBuilder::~CCustomInterfaceBuilder()
	{
	}

void CCustomInterfaceBuilder::HandleRequest(TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidCustomInterfaceBuilderImpl);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

void CCustomInterfaceBuilder::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch( aMessage.Function() )
		{
		case ECibBuild:
			DoBuildL(aMessage);
			break;
		case ECibRemove:
			DoRemoveL(aMessage);
			break;

		default:
			User::Leave(KErrNotSupported);

		}

	}

void CCustomInterfaceBuilder::DoBuildL(TMMFMessage& aMessage)
	{
	TPckgBuf<TUid> uidPckg;
	aMessage.ReadData1FromClientL(uidPckg);
	TUid interfaceId = uidPckg();
	//only KUidRestrictedAudioOutput is blocked here. 
    //KUidAudioOutput will be blocked in messageHandler for specific commands.
	if( iSecureCustomInterfaces && interfaceId==KUidRestrictedAudioOutput )
	    {
	    User::Leave( KErrNotSupported );
	    }

	TMMFMessageDestination handle;
	
	TAny* customInterface = GetCustomInterfaceL(interfaceId);
	if(customInterface)
		{
		CMMFObject* messageHandler = CMessageHandlerFactory::Create(interfaceId, customInterface,iObjectContainer);
		if(messageHandler)
			{
			iObjectContainer.AddMMFObject(*messageHandler);
			handle = messageHandler->Handle();
			}
		}
	TMMFMessageDestinationPckg handlePckg(handle);
	aMessage.WriteDataToClient(handlePckg);
	aMessage.Complete(KErrNone);
	}
	
void CCustomInterfaceBuilder::DoRemoveL(TMMFMessage& aMessage)
	{
	TMMFMessageDestinationPckg handlePckg;
	aMessage.ReadData1FromClientL(handlePckg);

	CMMFObject* messageHandler = NULL;

	if ( iObjectContainer.FindMMFObject(handlePckg(), messageHandler) == KErrNone )
		{
		iObjectContainer.RemoveAndDestroyMMFObject(*messageHandler);
		}
	aMessage.Complete(KErrNone);
		

	}

TAny* CCustomInterfaceBuilder::GetCustomInterfaceL(TUid aInterfaceId)
	{
	TAny* customInterface = NULL;
    if (iDevSound)
        {
        customInterface = iDevSound->CustomInterface(aInterfaceId);
        }
    else if (iCustomInterface)
        {
        customInterface = iCustomInterface->CustomInterface(aInterfaceId);
        }
	return customInterface;
	}

EXPORT_C void CCustomInterfaceBuilder::SetSecureCustomInterfaces( const TBool aSecure )
    {
    iSecureCustomInterfaces = aSecure;
    }
