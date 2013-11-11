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
// MPdpFsmInterface.h
// The interface to the PDP Context Finite State Machine.
// 
//

/**
 @file
 @internalComponent
*/


#ifndef MPDPFSMINTERFACE_H
#define MPDPFSMINTERFACE_H


#include <etelmm.h>
#include <etelqos.h>
#include <etelpckt.h>

#include <networking/umtsnifcontrolif.h>

/** Maximum number of PDP contexts supported by SPUD 
 This is derived from the number of bits available for NSAPIs
 Each PDP context corresponds with a NSAPI (and each NSAPI at most maps to one primary or secondary PDP Context)
 There are 4 bits allocated for NSAPIs with values 0 - 4 being reserved, leaving 11 values for PDP contexts
 @see GSM24.008
*/
const TContextId KMaxPdpContexts = 11; 

/** Internal pseudo-context ID meaning all contexts */
const TContextId KAllContexts = -1;

/** ID of the primary PDP context 
Primary context is the first context created, and it can be
created only once during Spud lifetime. 
There is a subltly here. There is always a default context.
This just happens to be the primary until it is deleted, at
which point one of the secondries will become the default.
This can happen in a network handover situation.
This is documented in the design spec. */
const TContextId KPrimaryContextId = 0; 

/* Internal constant */
const TInt KMbmsContextCreated=13;

#endif
// MPDPFSMINTERFACE_H
