/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This file contains the implementation of Effect Data Queue Item.
*
*/



// INCLUDE FILES

#include "EffectDataQueItem.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEffectDataQueItem::CEffectDataQueItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEffectDataQueItem::CEffectDataQueItem()
	:	iData(NULL)
	{
	}

// -----------------------------------------------------------------------------
// CEffectDataQueItem::~CEffectDataQueItem
// Destructor
// -----------------------------------------------------------------------------
//
CEffectDataQueItem::~CEffectDataQueItem()
	{
	delete iData;
	}

// -----------------------------------------------------------------------------
// CEffectDataQueItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEffectDataQueItem::ConstructL(
	TDesC8* aDataBuffer )
	{
	iData = (HBufC8*)aDataBuffer;
	}

// -----------------------------------------------------------------------------
// CEffectDataQueItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEffectDataQueItem* CEffectDataQueItem::NewL(
	TDesC8* aDataBuffer )
	{
	CEffectDataQueItem* self = new (ELeave) CEffectDataQueItem();
	CleanupStack::PushL(self);
	self->ConstructL(aDataBuffer);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CEffectDataQueItem::EffectData
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
TDesC8& CEffectDataQueItem::EffectData()
	{
	return *iData;
	}

