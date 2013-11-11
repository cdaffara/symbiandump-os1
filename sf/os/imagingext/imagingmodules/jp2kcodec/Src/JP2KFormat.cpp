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
* Description:  Collection of structs used to store and represent
*                the metadata in the JP2 file format.
*
*/


// INCLUDE FILES
#include "JP2KFormat.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// Destructor
TPalette::~TPalette()
    {
    iBList.Close();
    TInt entries = iC2DArray.Count();
    for (TInt index = 0; index < entries; ++index)
        {
        iC2DArray[index]->Close();
        }
    iC2DArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// TComponentMap::TComponentMap
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TComponentMap::TComponentMap( TUint16 aCmp, TUint8 aMtyp, TUint8 aPcol ) : 
    iCmp( aCmp ), 
    iMtyp( aMtyp ), 
    iPcol( aPcol )
    {
    }

// -----------------------------------------------------------------------------
// TChannelDefinition::TChannelDefinition
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TChannelDefinition::TChannelDefinition( TUint16 aCn, TUint16 aTyp, TUint16 aAsoc ): 
    iCn( aCn ), 
    iTyp( aTyp ), 
    iAsoc( aAsoc )
    {
    }

// -----------------------------------------------------------------------------
// TJ2kInfo::TJ2kInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TJ2kInfo::TJ2kInfo() : 
    iImageSize( 1, 1 ),
    iNC( 1 ),
    iBPC( 8 ),
    iOption( 0 ),
    iCSBoxLength( 0 ),
    iEnumCS( 0 ),
    iCSOffset( 0 ),
    iImageSizeInTwips( 0, 0 ),
    iBPCList( 1 ),
    iICCProfile( 0 ),
    iCMPList( 1 ),
    iCNList( 1 ),
    iPalette(),
    iIPRList( 1 ),
    iXMLList( 1 ),
    iUUIDList( 1 ),
    iUUIDInfoListList( 1 ),
    iUUIDInfoUrlList( 1 )
    {
    }

// Destructor
TJ2kInfo::~TJ2kInfo()
    {
    iBPCList.Close();
    
    delete iICCProfile;
    iICCProfile = 0;

    iCMPList.Close();
    iCNList.Close();

    iIPRList.ResetAndDestroy();
    iXMLList.ResetAndDestroy();
    iUUIDList.ResetAndDestroy();
    iUUIDInfoListList.ResetAndDestroy();
    iUUIDInfoUrlList.ResetAndDestroy();
    }
