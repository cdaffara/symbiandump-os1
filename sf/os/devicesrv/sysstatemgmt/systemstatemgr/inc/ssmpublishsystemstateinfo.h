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

#ifndef __SSMPUBLISHSYSTEMSTATEINFO_H__
#define __SSMPUBLISHSYSTEMSTATEINFO_H__

#include <e32def.h>
#include <ssm/ssmstate.h>
#include <ssm/ssmcmd.hrh>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <domainpolicy.h>
#else
/**
Defines the ways in which a domain hierarchy can be traversed.

@see RSsmStateManager::RequestStateTransition(TSsmStateTransition)
@publishedPartner
@released
*/
enum TSsmDmTraverseDirection
	{
	/**
	Transition parents first (i.e. before their children).
	*/
	ESsmTraverseParentsFirst,

	/**
	Transition children first (i.e. before their parents).
	*/
	ESsmTraverseChildrenFirst,

	/**
	Use the default direction specified in policy
	*/
	ESsmTraverseDefault,
	
	/**
	Maximum value for traverse mode
	*/
	ESsmTraverseMax = ESsmTraverseDefault
	};
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

/** 
Implementation of required information for a startup state.
@publishedPartner
@released
*/
NONSHARABLE_CLASS (TSsmPublishSystemStateInfo)
	{
public:
    IMPORT_C TSsmPublishSystemStateInfo();

    IMPORT_C TSsmState State() const;
	IMPORT_C TInt16 Retries() const;

 	IMPORT_C void Clear();
    IMPORT_C TSsmPublishSystemStateInfo& operator=(const TSsmPublishSystemStateInfo& aInfo);
    
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
	IMPORT_C TDmTraverseDirection Direction() const;
	IMPORT_C void Set(const TSsmState& aState, TDmTraverseDirection aDirection, TInt16 aRetries);
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
	IMPORT_C TSsmDmTraverseDirection Direction() const;
	IMPORT_C void Set(const TSsmState& aState, TSsmDmTraverseDirection aDirection, TInt16 aRetries);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS


private:
	TSsmState iState;
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS	
	TDmTraverseDirection iDirection;
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
	TSsmDmTraverseDirection iDirection;
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
	TInt16 iRetries;
	};

#endif // __SSMPUBLISHSYSTEMSTATEINFO_H__
