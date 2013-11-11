// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Record the infomation related to hangup commands

#ifndef ATHANGUPCOMMANDRECORDS_H_
#define ATHANGUPCOMMANDRECORDS_H_

//system include
#include <e32base.h>
#include <e32cmn.h> 

/**
 * Hand up command information
 * 
 */
class TATHanupCommandInfo
	{
public:
	TATHanupCommandInfo();
	TInt iCallId;
	TInt iHangupCause;
	};

/**
 * Hang up command information records
 */
class CATHangupCommandRecords : public CBase
	{
public:
	
	/**
	 * Factory fucntion
	 */
	static CATHangupCommandRecords* NewL();
	
	/**
	 * Factory function
	 * 
	 */
	static CATHangupCommandRecords* NewLC();
	
	/**
	 * Destructor
	 * 
	 */
	virtual ~CATHangupCommandRecords();
	
	/**
	 * Add hanup information
	 */
	TInt AddHangupCommandInfo(TInt aCallId, TInt aHangupCase);
	
	/**
	 * KErrNone get command info
	 * KErrNotFound not get command info
	 * and then remove this info from the array
	 */
	TInt GetFirstHangupCommandInfoAndRemoveIt(TATHanupCommandInfo& aInfo);
	
	/**
	 * Mark there is one hanup active command
	 */
	void SetHangupActiveFlag(TBool aIsActive);
	
	/**
	 * Retrieve the flag of current hangup command
	 *
	 * @return The ETrue if there is one acitve hangup command active, EFalase otherwise
	 */
	TBool GetHangupActiveFlag() const;
	
	/**
	 * Check if there are any hangup command in this store
	 */
	TBool IsHaveRecords() const;
	
protected:
	
	/**
	 * Constructor
	 */
	CATHangupCommandRecords();
	
	/**
	 * 2nd structor
	 */
	void ConstructL();
	
private:
	/**
	 * Flag of active hangup command 
	 */
	TBool iHangupActive;
	
	/**
	 * Store the hangup command info
	 */
	RArray<TATHanupCommandInfo> iHangupInfoArray;
	};

#endif /*ATHANGUPCOMMANDRECORDS_H_*/
