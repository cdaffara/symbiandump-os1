// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef _EIRMANSESSION_H
#define _EIRMANSESSION_H

#include <e32base.h>
#include <e32std.h>
#include <bluetooth/eirmanshared.h>
#include "eirmanager.h"

class CEirManServer;
class CEirManInternalSession;

NONSHARABLE_CLASS(MEirSessionNotifier)
	{
public:
	virtual void MesnRegisterComplete(TInt aResult) = 0;
	virtual void MesnSpaceAvailable(TUint aSpaceForTag) = 0;
	};

NONSHARABLE_CLASS(MEirInternalSessionNotifier)
	{
public:
	virtual void MeisnRegisterComplete(TInt aResult) = 0;
	virtual void MeisnSpaceAvailable(TUint aSpaceForTag) = 0;
	virtual void MeisnSetDataError(TInt aError) = 0;
	};

NONSHARABLE_CLASS(CEirManSession) : public CBase, public MEirManagerNotifiee
	{
public:
	static CEirManSession* NewL(CEirManServer& aServer, MEirSessionNotifier& aParent, TBool aInternalSession);
	~CEirManSession();
	void NotifyEirFeatureState(TInt aResult);
	
	void RegisterTag(TEirTag aTag);
	TInt SetData(const TDesC8& aData, TEirDataMode aMode);
	TInt NewData(TInt aRequiredLength);
	TEirTag EirTag() const;

private:
	CEirManSession(CEirManServer& aServer, MEirSessionNotifier& aParent, TBool aInternalSession);
	void ConstructL();
	void DeregisterTag();

private: // from MEirManagerNotifiee
	virtual void MemnEirBlockAvailable(TEirTag aTag, TUint aSpaceForTag);

public:
	TDblQueLink	iLink;
	
private:
	CEirManServer& 		iEirManServer; // unowned
	MEirSessionNotifier& iParent;
	TEirTag 			iEirTag;
	TEirTag				iPendingEirTag;
	TBool				iRegisterPending;
	TBool				iInternalSession;
	};

NONSHARABLE_CLASS(CEirManExternalSession): public CSession2, public MEirSessionNotifier
	{
public:
	static CEirManExternalSession* NewL(CEirManServer& aServer);
	~CEirManExternalSession();
	
private:
	void ConstructL(CEirManServer& aServer);
	CEirManExternalSession();
	void RegisterTagL(const RMessage2& aMessage);
	void RegisterSpaceAvailableListenerL(const RMessage2& aMessage);
	void CancelSpaceAvailableListenerL(const RMessage2& aMessage);
	void NewDataL(const RMessage2& aMessage);
	void SetDataL(const RMessage2& aMessage);

private:
	void CompleteSpaceAvailableRequest(TUint aBytesAvailable);

private: // CSession2 virtuals
	virtual void ServiceL(const RMessage2& aMessage);

private:
	virtual void MesnRegisterComplete(TInt aResult);
	virtual void MesnSpaceAvailable(TUint aSpaceForTag);
	
private:
	RMessage2			iRegisterMessage;
	RMessage2			iDataAvailableListenerMessage;
	TUint 				iLastSpaceOffered;

	CEirManSession* iSession;
	};

NONSHARABLE_CLASS(CEirManInternalSession) : public CBase, public MEirSessionNotifier
	{
public:
	static CEirManInternalSession* NewL(CEirManServer& aServer, MEirInternalSessionNotifier& aParent);
	~CEirManInternalSession();
	
	void RegisterTag(TEirTag aTag);
	void SetData(const TDesC8& aData, TEirDataMode aMode);
	TInt NewData(TInt aRequiredLength);
	
private:
	CEirManInternalSession(MEirInternalSessionNotifier& aParent);
	void ConstructL(CEirManServer& aServer);

	static TInt SetDataCb(TAny* aThis);
	void DoSetData();
	
private: // from MEirManagerNotifiee
	virtual void MesnRegisterComplete(TInt aResult);
	virtual void MesnSpaceAvailable(TUint aSpaceForTag);
	
private:
	CEirManSession* iSession;
	MEirInternalSessionNotifier& iParent;
	CAsyncCallBack* iSetDataCb;
	HBufC8* iPendingData;
	TEirDataMode iPendingMode;
	};

#endif // _EIRMANSESSION_H

