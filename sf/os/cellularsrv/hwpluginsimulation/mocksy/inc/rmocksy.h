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

#ifndef RMOCKSY_H
#define RMOCKSY_H

#include <e32def.h>
#include <e32base.h>

/**
Base class for a Mock SY session
Must be sub-class for specific types of SY and implement specific Expect and Complete
public methods 

*/
class RMockSY : public RSessionBase
	{
public:
	enum TMessage
		{
		KExpect =1,
		KComplete,
		KNotifyTerminated,
		KGetNextLogLine,
		KGetStatus,
		KPause,
		KResume
		};	

	IMPORT_C TInt Connect();	
	IMPORT_C void NotifyTerminated(TRequestStatus& aStatus);	
	IMPORT_C void GetNextLogLine(TDes& aLine);
	IMPORT_C void GetStatus(TBool& aHasWaitingExpect, TBool& aHasPendingComplete,TBool& aHasError);
	IMPORT_C TInt PauseCompletion();
	IMPORT_C TInt ResumeCompletion();

protected:
	virtual const TDesC& ServerName() =0;

protected:
	IMPORT_C void DoExpect(TInt aCmdId, TInt aErrorCode, TBool aLeave, const TDesC8& aData);
	IMPORT_C void DoComplete(TInt aCmdId, TInt aErrorCode, TInt aDelay, const TDesC8& aData);

	};
#endif
