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

#if !defined (__SYSTEMSTARTUPSTATEINFO_H__)
#define __SYSTEMSTARTUPSTATEINFO_H__

#include <e32base.h>
#include "StartupStateInfo.h"
 
/** Implementation of the MStartupStateInfo interface.
@internalComponent
@deprecated
@see MStartupStateInfo
*/
class CSystemStartupStateInfo : public CBase, public MStartupStateInfo
	{
public:
	static CSystemStartupStateInfo* NewL();
	static CSystemStartupStateInfo* NewLC();
	
	~CSystemStartupStateInfo();
	
public:
	void SetCommandList(CommandList aCommandList);
	void SetStateId(TUint8 aStateId);
	void SetName(const TDesC& aName);
	TInt AddCommand(MStartupCommand* aCommand);		
	void SetNoOfRetries(TNoOfRetries aNoOfRetries);
	void SetActionOnStateTransitionFailure(TActionOnStateTransitionFailure aActionOnStateTransitionFailure);
public:	// from MStartupStateInfo
    TUint8 StateId() const;
    TPtrC Name() const;
	const CommandList GetCommandList() const;
	TInt Count() const;
	MStartupCommand* GetCommand(TInt aIndex) const;
  	TNoOfRetries NoOfRetries() const;
  	TActionOnStateTransitionFailure ActionOnStateTransitionFailure() const;
private:
	CSystemStartupStateInfo();
	void ConstructL();

private:
	TNoOfRetries  iNoOfRetries;  
	TActionOnStateTransitionFailure iActionOnStateTransitionFailure;  
	TInt iStateId;
	HBufC* iName;
 
/**Encapsulates an array of system starter commands. Each startup state
owns one command list.
*/
	CommandList iCommandList;
	};

#endif // __SYSTEMSTARTUPSTATEINFO_H__
