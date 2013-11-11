/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Provides the interface between SUPL Framework and POS
*				 Message plugin. 
*
*/



#include <badesca.h>

#include "epos_comasuplpossessionbase.h"
#include "epos_tomasuplallowedcapabilities.h"

// ============================ MEMBER FUNCTIONS ===============================

//
// ---------------------------------------------------------
// Interface's (abstract base class's) constuctor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplPosSessionBase::COMASuplPosSessionBase(MOMASuplObserver *aSuplObserver) 
					   : iSuplObserver(aSuplObserver)
    {
    }
    

//
// ---------------------------------------------------------
// Interface's (abstract base class's) destructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C COMASuplPosSessionBase::~COMASuplPosSessionBase() 
    {
    }

//
// ---------------------------------------------------------
// Interface's (abstract base class's) GetSuplState
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void COMASuplPosSessionBase::GetSuplState(TOMASuplMsg& aSuplMsg,TOMASuplMsgState& aSuplMsgState) const
{
	aSuplMsg = iSuplMsg;
	aSuplMsgState = iSuplMsgState;
}

//
// ---------------------------------------------------------
// Interface's (abstract base class's) SetSuplState
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void COMASuplPosSessionBase::SetSuplState(TOMASuplMsg aSuplMsg,TOMASuplMsgState aSuplMsgState)
{
	iSuplMsg = aSuplMsg;
	iSuplMsgState = aSuplMsgState;
}


// ---------------------------------------------------------
// Interface's (abstract base class's) Default constructor
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
COMASuplPosSessionBase::COMASuplPosSessionBase()
{
	// Do Nothing
}


//
// ---------------------------------------------------------
// Interface's (abstract base class's) SuplObserver
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C MOMASuplObserver* COMASuplPosSessionBase::SuplObserver()
{
	return iSuplObserver;
}


//
// ---------------------------------------------------------
// COMASuplPosSessionBase::SetPosMethodAndAllowedCapabilities()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void COMASuplPosSessionBase::SetPosMethodAndAllowedCapabilities(TOMASuplAllowedCapabilities /*aAllowedCapabilities*/, TOMASuplPositioningMethod /*aPosMethod*/)
{
	//Do nothing

}
//
// ---------------------------------------------------------
// COMASuplPosSessionBase::SetSLPAddressUsed()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void COMASuplPosSessionBase::SetSLPAddressUsed(const TDesC& /*aServerAddress*/)
{
	//Do nothing

}
