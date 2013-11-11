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
//

/**
 @file
 @InternalComponent
*/

#include "psylogging.h"
#include "cpositionerq.h"
#include "psypanic.h"
#include "tpositionercall.h"

/*
* CPositionerQ::NewL
* Two-phased constructor.
* @param aEnv pointer to the CPositionerEnvironment
*/
CPositionerQ* CPositionerQ::NewL()
    {
    CPositionerQ* self = new( ELeave ) CPositionerQ();
    
    return self;
    }

/*
* CPositionerQ::CPositionerQ
* C++ default constructor can NOT contain any code, that
* might leave.
*/
CPositionerQ::CPositionerQ()
    {
    }

/*
* CPositionerQ::~CPositionerQ
* Destructor.
*/
CPositionerQ::~CPositionerQ()
    {
    __ASSERT_DEBUG(iPsyArray.Count() == 0, User::Panic(KAdaptationPanicCategory, EPanicQNotEmpty));
    
    iPsyArray.Close();
    }

/**
* Registers the PSY instance to the request handler
* @param aPSY pointer to the MPositioner
*/
void CPositionerQ::RegisterPSYL(MPositioner* aPSY)
    {
    TRACESTRING("CPositionerQ::RegisterPSYL start...")
    
    //Register the psy
    User::LeaveIfError(iPsyArray.Append(aPSY));
    
    TRACESTRING("CPositionerQ::RegisterPSYL end")
    }

/**
* Unregisters the MPositioner instance
* @param aPSY pointer to the MPositioner
*/
void CPositionerQ::UnregisterPSY(MPositioner* aPSY)
    {
    TRACESTRING("CPositionerQ::UnregisterPSY start...")

    TInt index = KErrNotFound;
    index = Index(*aPSY);
    
    if(index!=KErrNotFound)
        {
        iPsyArray.Remove(index);
        }
        
    TRACESTRING("CPositionerQ::UnregisterPSY end")
    }

/**
* Calles aCall::Call for all MPositioners in the queue
* @param TPositionerCall* pointer to the TPositionerCall derived class
*/
void CPositionerQ::PositionerIterator(TPositionerCall& aCall)
	{
	for(TInt i=0; i < iPsyArray.Count(); ++i)
		{
		aCall.Call(Positioner(i));
		}
	}

/**
* CPositionerQ::IterETrue
* Calls aCall::Call for all MPositioners in the queue
* @param aCall reference to the TPositionerCall derived function to be called
* @return TBool	returns True if the call returns true for any positioner in the queue
*/
TBool CPositionerQ::IterETrue(TRetBoolPositionerCall& aCall)
	{
    TInt count = iPsyArray.Count();
    for(TInt i=0; i<count; i++)
        {
        if(aCall.Call(Positioner(i)))
            {
            return ETrue;
            }
        }
    return EFalse;
	}

/**
* CPositionerQ::IterGreatest
* 
* Calls aCall::Call for all MPositioners in the queue
* @param aCall reference to the TPositionerCall derived function to be called
* @return TBool	returns the greatest of all the integer values returned from each MPositioner method
*/
TUint CPositionerQ::IterGreatest(TRetTUintPositionerCall& aCall)
	{
    TInt count = iPsyArray.Count();
    TUint greatest = 0;
    TUint result = 0;
    
    for(TInt i=0; i<count; i++)
        {
        result = aCall.Call(Positioner(i));
        if(result > greatest)
            {
            greatest = result;
            }
        }
    return greatest;
	}

/*
* CPositionerQ::Count
*
* @return count of MPositioner objects in the queue
*/
TUint CPositionerQ::Count()
	{
	return iPsyArray.Count();
	}
	
TInt CPositionerQ::Index(MPositioner& aPSY)
    {
    TInt index = KErrNotFound;
    TInt count = iPsyArray.Count();

    for(TInt i=0; i<count; i++)
        {
        if(iPsyArray[i] == &aPSY)
            {
            index = i;
            }
        }
	return index;
	}    
/*
* CPositionerQ::Positioner
*
* @return MPositioner pointer for requested index
*/
MPositioner& CPositionerQ::Positioner(TUint aIndex)
	{
	return *(iPsyArray[aIndex]);
	}
	
//  End of File

