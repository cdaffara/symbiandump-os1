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

#ifndef __SSMSTATETRANSITION_H__
#define __SSMSTATETRANSITION_H__

#include <ssm/ssmstate.h>

/**
 Encapsulates a system state transition.
 
 @publishedPartner
 @released
 */
NONSHARABLE_CLASS(TSsmStateTransition)
	{
public:
	IMPORT_C TSsmStateTransition();
	IMPORT_C TSsmStateTransition(TSsmState aState, TInt aReason);
	IMPORT_C TSsmStateTransition(TUint16 aMainState, TUint16 aSubState, TInt aReason);
	IMPORT_C TSsmStateTransition(const TSsmStateTransition& aRequest);
	
	IMPORT_C TSsmState State() const;
	IMPORT_C TInt Reason() const;
				
	IMPORT_C TSsmStateTransition& operator=(const TSsmStateTransition& aRequest);
	IMPORT_C TBool operator==(const TSsmStateTransition& aRequest) const;
	IMPORT_C TBool operator!=(const TSsmStateTransition& aRequest) const;
	
private:
	TSsmState iState;
	TInt iReason;
	};

#endif
