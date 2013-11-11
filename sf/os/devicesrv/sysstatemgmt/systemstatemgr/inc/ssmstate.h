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

#ifndef __SSMSTATE_H__
#define __SSMSTATE_H__

#include <e32std.h>
#include <ssm/ssmstates.hrh>

/**
 The maximum length of a System State Name
 
 @see TSsmState
 
 @publishedPartner
 @released
 */
const TInt KMaxSsmStateName = 11;

/**
 A buffer for a System State Name
 
 @see TSsmState
  
 @publishedPartner
@released
 */
typedef TBuf<KMaxSsmStateName> TSsmStateName;

/**
 Encapsulates the System State.
 
 @publishedPartner
@released
 */
NONSHARABLE_CLASS(TSsmState)
	{
public:
	IMPORT_C TSsmState();
	IMPORT_C TSsmState(TSsmMainSystemStates aMainState, TUint16 aSubState);
	IMPORT_C TSsmState(TUint16 aMainState, TUint16 aSubState);
	IMPORT_C TSsmState(const TSsmState& aState);
	IMPORT_C TUint16 MainState() const;
	IMPORT_C TUint16 SubState() const;
	IMPORT_C void Set(TUint16 aMainState, TUint16 aSubState);
	IMPORT_C TUint32 Int() const;
	IMPORT_C void SetFromInt(TUint32 aValue);
	IMPORT_C TSsmStateName Name() const;
	IMPORT_C TSsmState& operator=(const TSsmState& aState);
	IMPORT_C TBool operator==(const TSsmState& aState) const;
	IMPORT_C TBool operator!=(const TSsmState& aState) const;
private:
	TUint16 iMainState;
	TUint16 iSubState;
	};

#endif
