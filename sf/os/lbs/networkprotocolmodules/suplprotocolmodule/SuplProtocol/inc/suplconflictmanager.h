// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file defines the class that manages configuration of
// Test Protocol Module operation.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef __SUPLCONFLICTMANAGER_H__
#define __SUPLCONFLICTMANAGER_H__

#include <e32base.h>
#include <lbs/lbsnetprotocolbase.h>

/** Configuration Manager

This class provides a method for the protocol manager to call in order
to generate a decision for handling a new operation, with due consideration
of any current active operation.

*/
NONSHARABLE_CLASS(CSuplConflictManager) : public CBase
	{
public:

	enum TConflictResult
		{
		EConflictNone,
		EConflictRejectNew,
		EConflictCancelCurrent,
		EConflictAdoptNewSessionId
		};

public:

	static CSuplConflictManager* NewL();
	~CSuplConflictManager();

	TConflictResult ConflictDecision(const MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask& aActiveServiceMask,
									 const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aNewOp,
									 const TLbsNetPosRequestOptionsBase& aNewOperationOptions) const;
private:

	CSuplConflictManager();
	void ConstructL();

private:

	RArray<TConflictResult> iDecisionTable;
	};

#endif // __SUPLCONFLICTMANAGER_H__
