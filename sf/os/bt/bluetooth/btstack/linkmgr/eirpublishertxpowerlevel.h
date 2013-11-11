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


#ifndef EIRPUBLISHERTXPOWERLEVEL_H
#define EIRPUBLISHERTXPOWERLEVEL_H

#include <e32base.h>
#include "eirmansession.h"

class CEirManServer;

#define KSizeOfTxPowerLevelData 1

//**********************************
// CEirPublisherTxPowerLevel
//**********************************
/**
Provides functionality to publish TxPowerLevel to EIR.
**/
NONSHARABLE_CLASS(CEirPublisherTxPowerLevel) : public CBase, public MEirInternalSessionNotifier
	{
public:
	static CEirPublisherTxPowerLevel* NewL(CEirManServer& aServer);
	~CEirPublisherTxPowerLevel();
	void UpdateTxPowerLevel(TInt8 aTxPowerLevel);

private:
	CEirPublisherTxPowerLevel();
	void ConstructL(CEirManServer& aServer);
	
	// From MEirInternalSessionNotifier
	virtual void MeisnSpaceAvailable(TUint aBytesAvailable);
	virtual void MeisnRegisterComplete(TInt aResult);
	virtual void MeisnSetDataError(TInt aError);
	
private:
	TInt8 iTxPowerLevel;
	TBuf8<KSizeOfTxPowerLevelData> iPublishBuf;
	CEirManInternalSession* iSession;
	TBool iTxPowerLevelPublished;
	TBool		iTagRegistered;
	};
	
#endif	//EIRPUBLISHERTXPOWERLEVEL_H
