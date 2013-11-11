/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JP2KCodeBlock class used to collect the codeblock
*                related information such as compressed data and data length.
*
*/


// INCLUDE FILES
#include "JP2KCodeBlock.h"
#include "JP2KUtils.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::CJ2kCodeBlock
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CJ2kCodeBlock::CJ2kCodeBlock() : 
    iLastPass(-1), 
    iLengthBits(3)
    {
    }

// Destructor
CJ2kCodeBlock::~CJ2kCodeBlock()
    {
    delete iDataSize;
    iDataSize = 0;

    delete iPassesPerSegment;
    iPassesPerSegment = 0;

    delete iNumSegment;
    iNumSegment = 0;

    delete iData;
    iData = 0;
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::SetCodeBlockCanvas
// Set the canvas of the code block.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kCodeBlock::SetCodeBlockCanvas( TInt32 aX, TInt32 aY, TInt32 aWidth, TInt32 aHeight )
    {
    iCodeBlockCanvas.iTl = TPoint( aX, aY );
    iCodeBlockCanvas.SetWidth( aWidth );
    iCodeBlockCanvas.SetHeight( aHeight );
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::InitializeL
// Initialize the number of segment.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kCodeBlock::InitializeL( TUint16 aLayer )
    {
    iNumSegment = HBufC16::NewMaxL( aLayer );
    iNumSegment->Des().FillZ();
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::DataL
// Get the data buffer descriptor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CJ2kCodeBlock::DataL()
    {
    if ( iData )
        {
        if ( iData->Des().MaxLength() < (TInt)iDataLength )
            {
            iData = iData->ReAllocL( iDataLength );
            }
        }
    else
        {
        iData = HBufC8::NewL( iDataLength );
        }

    return iData;
    }

// -----------------------------------------------------------------------------
// CJ2kCodeBlock::Data
// Get the pointer to the data portion.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TUint8* CJ2kCodeBlock::Data() const
    {
    if ( iData )
        {
        return iData->Ptr();
        }

    return 0;
    }

