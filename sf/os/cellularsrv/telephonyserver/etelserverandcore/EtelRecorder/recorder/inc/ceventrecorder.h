// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the CEventRecorder class
// 
//

/**
 @file
 @internalAll
 @released
*/
#ifndef CEVENTRECORDER_H
#define CEVENTRECORDER_H

#include "et_record.h"

#ifdef ETEL_RECORDER

#include <e32base.h>
#include <comms-infras/commsdebugutility.h>


NONSHARABLE_CLASS(CEventRecorder) : public CBase
	{
public:
	static CEventRecorder* NewL(const TDesC8& aSubsystem);	
	~CEventRecorder();
	
	void Dump(const TDesC8& aToDump);
	void DumpLogHeader();
	
	__FLOG_DECLARATION_MEMBER;	
protected:
	void ConstructL(const TDesC8& aSubsystem);
	
	};
#endif // ETEL_RECORDER	
#endif //CEVENTRECORDER_H
