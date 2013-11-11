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
* Description: CustomInterfaceUtility Proxy implementation
*
*/



// INCLUDE FILES
#include <ProxyCustomInterfaceUtility.h>
#include "CustomInterfaceBuilderTypes.h"
#include "CustomInterfaceProxyFactory.h"


EXPORT_C CProxyCustomInterfaceUtility* CProxyCustomInterfaceUtility::NewL(CCustomCommandUtility* aCustomCommandUtility)
	{
	CProxyCustomInterfaceUtility* self = new(ELeave) CProxyCustomInterfaceUtility(aCustomCommandUtility);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CProxyCustomInterfaceUtility* CProxyCustomInterfaceUtility::NewL(MCustomCommand& aMCustomCommand)
	{
	CProxyCustomInterfaceUtility* self = new(ELeave) CProxyCustomInterfaceUtility(aMCustomCommand);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CProxyCustomInterfaceUtility::CProxyCustomInterfaceUtility(CCustomCommandUtility* aCustomCommandUtility) :
														CCustomInterfaceUtility(),
														iCustomCommandUtility(aCustomCommandUtility)
	{
	}

CProxyCustomInterfaceUtility::CProxyCustomInterfaceUtility(MCustomCommand& aMCustomCommand) :
														CCustomInterfaceUtility(),
														iMCustomCommand(&aMCustomCommand)
	{
	}


CProxyCustomInterfaceUtility::~CProxyCustomInterfaceUtility()
	{
	delete iCustomCommandUtility;
    // Do not delete iMCustomCommand. Unlike iCustomCommandUtility we don't own
    // iMCustomCommand.
	}

void CProxyCustomInterfaceUtility::ConstructL()
	{
	TMMFMessageDestination     destination(KUidCustomInterfaceBuilder);
	TMMFMessageDestinationPckg destinationPckg(destination);
	TMMFMessageDestination     builderHandle;
	TMMFMessageDestinationPckg builderHandlePckg(builderHandle);
	iBuilderHandle = builderHandlePckg;

    if (iCustomCommandUtility)
        {
	    iCustomCommandUtility->CustomCommandSync(destinationPckg, ECibGetBuilder, KNullDesC8, KNullDesC8, iBuilderHandle);
        }
    else if (iMCustomCommand)
        {
        iMCustomCommand->CustomCommandSync(destinationPckg, ECibGetBuilder, KNullDesC8, KNullDesC8, iBuilderHandle);
        }

	if(iBuilderHandle().InterfaceId() == KNullUid)
		{
		User::Leave(KErrNotFound);
		}
	}

EXPORT_C TAny* CProxyCustomInterfaceUtility::CustomInterface(TUid aInterfaceId)
	{
	// Get Handle
	TMMFMessageDestination     handle;
	TMMFMessageDestinationPckg handlePckg(handle);
	TPckgBuf<TUid> uidPckg;
	uidPckg() = aInterfaceId;

	if (iCustomCommandUtility)
        {
        iCustomCommandUtility->CustomCommandSync(iBuilderHandle, ECibBuild, uidPckg, KNullDesC8, handlePckg);
        }
    else if (iMCustomCommand)
        {
        iMCustomCommand->CustomCommandSync(iBuilderHandle, ECibBuild, uidPckg, KNullDesC8, handlePckg);
        }
	
	TAny* result = NULL;

	if(handlePckg().InterfaceId() == KNullUid)
		{
		result = NULL;
		}
	else
		{
	    if (iCustomCommandUtility)
            {
    		result = CCustomInterfaceProxyFactory::CreateProxy(aInterfaceId, handlePckg, *iCustomCommandUtility, this);
            }
        else if (iMCustomCommand)
            {
            result = CCustomInterfaceProxyFactory::CreateProxy(aInterfaceId, handlePckg, *iMCustomCommand, this);
            }
        
		}

	return result;
	}


EXPORT_C void CProxyCustomInterfaceUtility::RemoveCustomInterface(TMMFMessageDestinationPckg aMessageHandler)
	{
	// Get Handle
	TMMFMessageDestination     handle;
	TMMFMessageDestinationPckg handlePckg(handle);

	
	if (iCustomCommandUtility)
        {
        iCustomCommandUtility->CustomCommandSync(iBuilderHandle, ECibRemove, aMessageHandler, KNullDesC8);
        }
    else if (iMCustomCommand)
        {
        iMCustomCommand->CustomCommandSync(iBuilderHandle, ECibRemove, aMessageHandler, KNullDesC8);
        }
	}


// End of File
