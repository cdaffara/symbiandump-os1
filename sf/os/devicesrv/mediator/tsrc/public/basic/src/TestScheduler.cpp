/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This module contains the implementation of CTestScheduler that is used for 
*				 creating pauses.
*
*/


// INCLUDE FILES
#include "TestScheduler.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================


// ================= MEMBER FUNCTIONS =========================================

// -----------------------------------------------------------------------------
// CTestScheduler::CTestScheduler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CTestScheduler::CTestScheduler()
{
	
}

// Destructor
CTestScheduler::~CTestScheduler()
{
	delete iTimeout;
}

// -----------------------------------------------------------------------------
// CTestScheduler::NewL
// Two-phased constructor.
// 
// -----------------------------------------------------------------------------
CTestScheduler* CTestScheduler::NewL()
{

	CTestScheduler* self = new( ELeave )CTestScheduler();
	CleanupStack::PushL( self );

	if (CActiveScheduler::Current() == NULL)
	{
		CActiveScheduler::Install(self);
	}

	self->ConstructL();
	CleanupStack::Pop( self );
	
	return self;
	
}

// -----------------------------------------------------------------------------
// CTestScheduler::ConstructL
// Symbian 2nd phase constructor can leave.
// 
// -----------------------------------------------------------------------------
void CTestScheduler::ConstructL()
{
	iTimeout = new(ELeave) CTimeout;
}

// -----------------------------------------------------------------------------
// CTestScheduler::WaitForAnyRequest
// Waits for user request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CTestScheduler::WaitForAnyRequest()
{
	User::WaitForAnyRequest();
}


// -----------------------------------------------------------------------------
// CTestScheduler::Error
// Handles error cases.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CTestScheduler::Error(TInt /*anError*/) const
{
	Stop();
}


// -----------------------------------------------------------------------------
// CTestScheduler::StartAndStopL
// Starts and stops the timer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CTestScheduler::StartAndStopL
		(
		TInt aMillisecs //vibra running time
		)
{
	//initialize timer and start scheduler
	CTestScheduler* self = CTestScheduler::NewL();

	TBool value( ETrue );
	self->iTimeout->Start(aMillisecs);

	CActiveScheduler::Start();

	// if not timed, then timer must be cancelled
	if (!self->iTimeout->Timed())
	{
		self->iTimeout->Cancel();
		value = ETrue;
	}

	delete self;
	self = NULL;

	return value;
}

//CTIMEOUT

// -----------------------------------------------------------------------------
// CTimeout::CTimeout
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CTimeout::CTimeout()
:CActive(0)
{
	iTimer.CreateLocal();

	CActiveScheduler::Add(this);			
}


// Destructor
CTimeout::~CTimeout()
{
	Cancel();
	iTimer.Close();
}

// -----------------------------------------------------------------------------
// CTimeout::Start
// Starts the timer with given time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CTimeout::Start(TInt aTimeoutInMilliSecs)
{
	iTimer.After(iStatus, 1000 * aTimeoutInMilliSecs);
	iTimed = FALSE;
	SetActive();
}

// -----------------------------------------------------------------------------
// CTimeout::DoCancel
// Cancels the timer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CTimeout::DoCancel()
{
	iTimer.Cancel();
}

// -----------------------------------------------------------------------------
// CTimeout::RunL
// 
// 
// -----------------------------------------------------------------------------
void CTimeout::RunL()
{
	iTimed = TRUE;
	CActiveScheduler::Stop();
}





