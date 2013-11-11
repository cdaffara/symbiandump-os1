// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SERVER.H"


NONSHARABLE_CLASS(CFbsBackgroundCompressionQueueElement): public CBase
/**
@internalComponent
*/
	{
	friend class CFbsBackgroundCompression;
private:
	CFbsBackgroundCompressionQueueElement(CBitmapObject* aSrcObj, TBitmapfileCompressionScheme aScheme);
	~CFbsBackgroundCompressionQueueElement();
	void CompleteOutstandingRequestAndDestroy(TInt aReason);
	void CompleteOutstandingRequest(const CSession2* aSession);

private:
	TDblQueLink iLink;
	CBitmapObject* iSourceObject;
	TBitmapfileCompressionScheme iCompressionScheme;
	RMessage2 iMessage;
	};

NONSHARABLE_CLASS(CFbsBackgroundCompression): public CActive
/**
@internalComponent
*/
	{
public:
	static CFbsBackgroundCompression* NewL(CFbTop& aFbTop);
	~CFbsBackgroundCompression();
	TInt AddToCompressionQueue(CBitmapObject* aSrcObj, TBitmapfileCompressionScheme aScheme, const RMessage2* aMessage);
	void RemoveFromCompressionQueue(CBitmapObject* aSrcObj);
	void CompleteOutstandingRequests(const CSession2* aSession);
	void CompressAll();

private:
	CFbsBackgroundCompression(CFbTop& aFbTop);
	void ConstructL();
	TInt PrepareCompression();
	void FinalizeCompression(const TRequestStatus& aStatus);
	void RunL();
	void DoCancel();
	static TInt ThreadFunction(CFbsBackgroundCompression* aSelf);

private:
	CFbTop& iFbTop;
	RThread iThread;
	TBool iThreadCreated;
	RSemaphore iThreadGo;
	RMutex iThreadMutex;
	TDblQue<CFbsBackgroundCompressionQueueElement> iCompressionQueue;
	CBitmapObject* volatile iBitmapObject;
	volatile TBitmapfileCompressionScheme iCompressionScheme;
	};
