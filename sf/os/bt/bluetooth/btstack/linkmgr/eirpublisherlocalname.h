// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef EIRPUBLISHERLOCALNAME_H
#define EIRPUBLISHERLOCALNAME_H

#include <e32base.h>
#include <bluetooth/hci/hciconsts.h>
#include "eirmansession.h"

class CEirManServer;

//**********************************
// CEirPublisherLocalName
//**********************************
/**
Provides functionality to publish 16 bit UUIDs to EIR.
**/
NONSHARABLE_CLASS(CEirPublisherLocalName): public CBase, public MEirInternalSessionNotifier
	{
public:
	static CEirPublisherLocalName* NewL(CEirManServer& aServer);
	~CEirPublisherLocalName();
	void UpdateName(const TDesC8& aName);

private:
	CEirPublisherLocalName();
	void ConstructL(CEirManServer& aServer);

	// From MEirInternalSessionNotifier
	virtual void MeisnSpaceAvailable(TUint aBytesAvailable);
	virtual void MeisnRegisterComplete(TInt aResult);
	virtual void MeisnSetDataError(TInt aError);

private:
	TBuf8<KHCILocalDeviceNameMaxLength> iLocalName;
	HBufC8* iPublishBuf;
	CEirManInternalSession* iSession;
	TBool		iTagRegistered;
	};
	
#endif	// EIRPUBLISHERLOCALNAME_H
