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
// This file defines the class that manages configuration of
// Test Protocol Module operation.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CCONFIGMANAGER_H__
#define __CCONFIGMANAGER_H__

#include <e32base.h>

// Table index values
#define KMoLr				0
#define KMtLr				1
#define KEmergency			2
#define KX3pLow				3
#define KX3pMid				4
#define KX3pHi				5
#define KMaxOperationTypes	6

/** Central repository UID
This identifies the external data source for decision data
*/
#define KMyRepositoryUid	0x10281D6F

/** Default decision 
Used if a decision value is not provided by external data, and also 
if a caller specifies an invalid type of current or new operation.
*/
#define KDefaultDecision EConflictRejectNew


/** Configuration Manager
This class manages the configuration of Test Protocol Module operation in
relation to how to handle conflicting services.

The class provides a method for the protocol manager to call in order
to generate a decision for handling a new operation, with due consideration
of any current active operation.

Conflict decisions are taken from a Decision Table which is initialised from an
external data source during construction. The table is represented as a matrix 
of decisions organised in rows and columns, representing current operation
and new operation, respectively. The UID (unique identifiers) for the external
data items are shown in this representation of the table.

                         New
                        Operation -->
    
                        MOLR MTLR EMRG X3Pl X3Pm X3Ph
                        -----------------------------
   Current        MOLR |  1 |  2 |  3 |  4 |  5 |  6 |
   Operation           |----|----|----|----|----|----|
      |           MTLR |  7 |  8 |  9 | 10 | 11 | 12 |
      |                |----|----|----|----|----|----|
      v           EMRG | 13 | 14 | 15 | 16 | 17 | 18 |
                       |----|----|----|----|----|----|
                  X3Pl | 19 | 20 | 21 | 22 | 23 | 24 |
                       |----|----|----|----|----|----|
                  X3Pm | 25 | 26 | 27 | 28 | 29 | 30 |
                       |----|----|----|----|----|----|
                  X3Ph | 31 | 32 | 33 | 34 | 35 | 36 |
                        -----------------------------

X3Pl = low priority X3P
X3Pm = medium priority X3P
X3Ph = high priority X3P

To retrieve a decision this class performs a table look-up according to the 
current operation and new operation specified by the caller. The index is
calculated using row and column values according to the operation types.
Note: the table index equates to (UID value - 1)

*/
NONSHARABLE_CLASS(CConfigManager) : public CBase
	{
public:

	enum TConflictResult
		{
		EConflictNone,
		EConflictRejectNew,
		EConflictQueueNew,
		EConflictCancelCurrent,
		EConflictQueueCurrent
		};

	enum TConflictOp
		{
		EOpNone,
		EOpMoLr, // MO-LR
		EOpNbLr, // Net Based Loc
		EOpMtLr, // MT-LR
		EOpNiLr, // Net Induced
		EOpX3p   // X3P
		};

	enum TConflictPriority
		{
		EPriorityNone,
		EPriorityLow,
		EPriorityMedium,
		EPriorityHigh,
		EPriorityEmergency
		};

	class TConflictOperation
		{
		public:
		TConflictOp iOperation;
		TConflictPriority iPriority;
		};

public:

	static CConfigManager* NewL();
	~CConfigManager();

	TConflictResult ConflictDecision(const TConflictOperation& aCurrentOp, 
										const TConflictOperation& aNewOp) const;

private:

	CConfigManager();
	void ConstructL();
	void LoadDecisionDataL();

	TInt DecisionTableIndex(const TConflictOperation& aOperation) const;

private:

	RArray<TConflictResult> iDecisionTable;

	};

#endif // __CCONFIGMANAGER_H__
