// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BREDRCONTROLLERCONFIGURATION_H
#define BREDRCONTROLLERCONFIGURATION_H

#include <e32base.h>
#include <bttypespartner.h>
#include <bluetooth/hcicommandqueueclient.h>

class MHCICommandQueue;
class CLinkMgrProtocol;

/**
	Handles page scan parameters update requests. Maps KPageScanParametersInfo enums to actual page scan parameters
	and sends appropriate HCI commands.
	Before the first update the original settings are stored. If fetching of them fails the default values
	from the Bluetooth specification are used instead of them.
*/
NONSHARABLE_CLASS(CBrEdrControllerConfiguration) : public CBase, public MHCICommandQueueClient
	{
public:
	static CBrEdrControllerConfiguration* NewL(MHCICommandQueue& aCommandQueue, CLinkMgrProtocol& aLinkMgrProtocol);
	~CBrEdrControllerConfiguration();

	// from MHCICommandQueueClient
	void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);

	void SetPageScanParameters(TPageScanParameterSettings aPageScanSettings);

private:
	CBrEdrControllerConfiguration(MHCICommandQueue& aCommandQueue, CLinkMgrProtocol& aLinkMgrProtocol);

private:
	void SetPageScanParametersL(TPageScanParameterSettings aPageScanSettings);

private:
// owned
	TUint16		iOriginalPageScanInterval;
	TUint16		iOriginalPageScanWindow;
	TUint8		iOriginalPageScanType;

	TInt		iPageScanParameters;

// non-owned resources
	MHCICommandQueue&	iCommandQueue;
	CLinkMgrProtocol&	iLinkMgrProtocol;
	};

#endif
