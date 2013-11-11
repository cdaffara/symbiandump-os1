// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include    "epos_tpospsytestparameters.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TPosPSYTestParameters::TPosPSYTestParameters() : 
    iNrOfMemoryRequests(0),
    iNrOfLocationRequests(0),
    iNrOfPartialUpdateRequests(0),
    iNrOfStressTestInstances(0),
	iNrOfMultipleSessionTestInstances(0),
    iNrOfMultipleSessionCancelTestInstances(0),
    iHeapSizePercentageLevel(0),
    iTimeToFixThreshold(0),
	iHorizontalAccuracyThreshold(0),
    iVerticalAccuracyThreshold(0),
    iMaxAgeMaxTestTime(0),
    iMaxHorizontalAccuracy(0),
	iMaxVerticalAccuracy(0),
    iResonableHorizontalAccuracy(0),
    iResonableVertictalAccuracy(0),
    iMaxTimeToFirstFix(0)
    {
    }

// ---------------------------------------------------------
// TPosPSYTestParameters::SetNumberOfMemoryRequests
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetNumberOfMemoryRequests(
	TInt aNrOfMemoryRequests)
	{
	iNrOfMemoryRequests = aNrOfMemoryRequests;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetNumberOfLocationRequests
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetNumberOfLocationRequests(
	TInt aNrOfLocationRequests)
	{
	iNrOfLocationRequests = aNrOfLocationRequests;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetNumberOfPartialUpdateRequests
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetNumberOfPartialUpdateRequests(
	TInt aNrOfPartialUpdateRequests)
	{
	iNrOfPartialUpdateRequests = aNrOfPartialUpdateRequests;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetNumberOfStressTestInstances
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetNumberOfStressTestInstances(
	TInt aNrOfStressTestInstances)
	{
	iNrOfStressTestInstances = aNrOfStressTestInstances;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetNumberOfMultipleSessionTestInstances
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetNumberOfMultipleSessionTestInstances(
	TInt aNrOfMultipleSessionTestInstances)
	{
	iNrOfMultipleSessionTestInstances = aNrOfMultipleSessionTestInstances;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetNumberOfMultipleSessionCancelTestInstances
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetNumberOfMultipleSessionCancelTestInstances(
	TInt aNrOfMultipleSessionCancelTestInstances)
	{
	iNrOfMultipleSessionCancelTestInstances = 
		aNrOfMultipleSessionCancelTestInstances;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetHeapSizePercentageLevel
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetHeapSizePercentageLevel(
	TInt aHeapSizePercentageLevel)
	{
	iHeapSizePercentageLevel = aHeapSizePercentageLevel;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetTimeToFixThreshold
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetTimeToFixThreshold(
	TInt aTimeToFixThreshold)
	{
	iTimeToFixThreshold = aTimeToFixThreshold;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetHorizontalAccuracyThreshold
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetHorizontalAccuracyThreshold(
	TInt aHorizontalAccuracyThreshold)
	{
	iHorizontalAccuracyThreshold = aHorizontalAccuracyThreshold;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetVerticalAccuracyThreshold
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetVerticalAccuracyThreshold(
	TInt aVerticalAccuracyThreshold)
	{
	iVerticalAccuracyThreshold = aVerticalAccuracyThreshold;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetMaxAgeMaxTestTime
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetMaxAgeMaxTestTime(
	TInt aMaxAgeMaxTestTime)
	{
	iMaxAgeMaxTestTime = aMaxAgeMaxTestTime;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetMaxHorizontalAccuracy
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetMaxHorizontalAccuracy(
	TInt aMaxHorizontalAccuracy)
	{
	iMaxHorizontalAccuracy = aMaxHorizontalAccuracy;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetMaxVerticalAccuracy
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetMaxVerticalAccuracy(
	TInt aMaxVerticalAccuracy)
	{
	iMaxVerticalAccuracy = aMaxVerticalAccuracy;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetResonableHorizontalAccuracy
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetResonableHorizontalAccuracy(
	TInt aResonableHorizontalAccuracy)
	{
	iResonableHorizontalAccuracy = aResonableHorizontalAccuracy;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetResonableVertictalAccuracy
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetResonableVertictalAccuracy(
	TInt aResonableVertictalAccuracy)
	{
	iResonableVertictalAccuracy = aResonableVertictalAccuracy;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::SetMaxTimeToFirstFix
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYTestParameters::SetMaxTimeToFirstFix(
	TInt aMaxTimeToFirstFix)
	{
	iMaxTimeToFirstFix = aMaxTimeToFirstFix;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::NumberOfMemoryRequests
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::NumberOfMemoryRequests() const
	{
	return iNrOfMemoryRequests;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::NumberOfLocationRequests
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::NumberOfLocationRequests() const
	{
	return iNrOfLocationRequests;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::NumberOfPartialUpdateRequests
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::NumberOfPartialUpdateRequests() const
	{
	return iNrOfPartialUpdateRequests;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::NumberOfStressTestInstances
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::NumberOfStressTestInstances() const
	{
	return iNrOfStressTestInstances;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::NumberOfMultipleSessionTestInstances
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::NumberOfMultipleSessionTestInstances() const
	{
	return iNrOfMultipleSessionTestInstances;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::NumberOfMultipleSessionCancelTestInstances
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::NumberOfMultipleSessionCancelTestInstances() const
	{
	return iNrOfMultipleSessionCancelTestInstances;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::HeapSizePercentageLevel
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::HeapSizePercentageLevel() const
	{
	return iHeapSizePercentageLevel;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::TimeToFixThreshold
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::TimeToFixThreshold() const
	{
	return iTimeToFixThreshold;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::HorizontalAccuracyThreshold
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::HorizontalAccuracyThreshold() const
	{
	return iHorizontalAccuracyThreshold;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::VerticalAccuracyThreshold
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::VerticalAccuracyThreshold() const
	{
	return iVerticalAccuracyThreshold;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::MaxAgeMaxTestTime
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::MaxAgeMaxTestTime() const
	{
	return iMaxAgeMaxTestTime;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::MaxHorizontalAccuracy
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::MaxHorizontalAccuracy() const
	{
	return iMaxHorizontalAccuracy;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::MaxVerticalAccuracy
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::MaxVerticalAccuracy() const
	{
	return iMaxVerticalAccuracy;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::ResonableHorizontalAccuracy
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::ResonableHorizontalAccuracy() const
	{
	return iResonableHorizontalAccuracy;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::ResonableVertictalAccuracy
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::ResonableVertictalAccuracy() const
	{
	return iResonableVertictalAccuracy;
	}

// ---------------------------------------------------------
// TPosPSYTestParameters::MaxTimeToFirstFix
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYTestParameters::MaxTimeToFirstFix() const
	{
	return iMaxTimeToFirstFix;
	}

//  End of File
