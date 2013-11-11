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
 @internalTechnology
 
*/

#include "RRLP-Messages.h"
#include "rrlpassistancedataack.h"
#include "supldevloggermacros.h" 


/** 
Static factory constructor
*/
EXPORT_C CRrlpAssistanceDataAck* CRrlpAssistanceDataAck::NewL()
	{
	SUPLLOG(ELogP1, "CRrlpAssistanceDataAck::NewL() Begin\n");
	CRrlpAssistanceDataAck* self = new (ELeave) CRrlpAssistanceDataAck();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CRrlpAssistanceDataAck::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

		
/** 
Default constructor
*/
CRrlpAssistanceDataAck::CRrlpAssistanceDataAck()
 : CRrlpMessageBase(ERrlpAssistanceDataAck, ETrue)
	{
	}


/** 
Second stage constructor 
*/
void CRrlpAssistanceDataAck::ConstructL()
	{
	// call the base class constructor to initialise content container 
	// and control objects
	CRrlpMessageBase::ConstructL();

	iData->component.t = T_RRLP_Component_assistanceDataAck;
	}


/** 
Destructor 
*/
CRrlpAssistanceDataAck::~CRrlpAssistanceDataAck()
	{
	SUPLLOG(ELogP1, "CRrlpAssistanceDataAck::~CRrlpAssistanceDataAck() Begin\n");
	SUPLLOG(ELogP1, "CRrlpAssistanceDataAck::~CRrlpAssistanceDataAck() End\n");
	}
	
	
