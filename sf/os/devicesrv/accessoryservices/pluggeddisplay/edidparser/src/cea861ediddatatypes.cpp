/*
 * Copyright (c) 2008,2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 * Implementation of classes specified in cea861ediddatatypes.h.
 *
 */

#include "cea861ediddatatypes.h"
#include "trace.h"

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


//=============================================================================
//CCea861ExtEdidInformation
//=============================================================================
//


// ---------------------------------------------------------------------------
// CCea861ExtEdidInformation::CCea861ExtEdidInformation
// ---------------------------------------------------------------------------
//
CCea861ExtEdidInformation::CCea861ExtEdidInformation() :
    iShortVideoDescriptors( NULL ),
    iShortAudioDescriptors( NULL ),
    iVendorSpecificData( NULL ),
    iVideoCapabilityDataBlock( NULL ),
    iDescriptorBlocks( NULL )
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
// CCea861ExtEdidInformation::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCea861ExtEdidInformation* CCea861ExtEdidInformation::NewL()
    {
    FUNC_LOG;

    CCea861ExtEdidInformation* self = CCea861ExtEdidInformation::NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CCea861ExtEdidInformation::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CCea861ExtEdidInformation* CCea861ExtEdidInformation::NewLC()
    {
    FUNC_LOG;

    CCea861ExtEdidInformation* self =
        new ( ELeave ) CCea861ExtEdidInformation();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CCea861ExtEdidInformation::ConstructL
// ---------------------------------------------------------------------------
//
void CCea861ExtEdidInformation::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CCea861ExtEdidInformation::~CCea861ExtEdidInformation
// ---------------------------------------------------------------------------
//
CCea861ExtEdidInformation::~CCea861ExtEdidInformation()
    {
    FUNC_LOG;

    // delete all video descriptors
    while( iShortVideoDescriptors )
        {
        // iterate to the end and delete the latest link
        TCEA861VideoDataBlock* last = iShortVideoDescriptors;
        TCEA861VideoDataBlock* prev = iShortVideoDescriptors;
        while( last->iNext )
            {
            prev = last;
            last = last->iNext;
            }
        // now last is pointing to the last link and prev is pointing to the one before the last
        if( prev == last )
            { // if there is only one
            iShortVideoDescriptors = NULL;
            prev = NULL;
            delete last;
            }
        else
            {
            prev->iNext = NULL;
            delete last;
            }
        }

    // delete all audio descriptors
    while( iShortAudioDescriptors )
        {
        // iterate to the end and delete the latest link
        TCEA861AudioDataBlock* last = iShortAudioDescriptors;
        TCEA861AudioDataBlock* prev = iShortAudioDescriptors;
        while( last->iNext )
            {
            prev = last;
            last = last->iNext;
            }
        // now last is pointing to the last link and prev is pointing to the one before the last
        if( prev == last )
            {
            iShortAudioDescriptors = NULL;
            prev = NULL;
            delete last;
            }
        else
            {
            prev->iNext = NULL;
            delete last;
            }
        }

    // delete vendor specific data
    while( iVendorSpecificData )
        {
        // iterate to the end and delete the latest link
        TCEA861VendorSpecificDataBlockPayload* last =
            iVendorSpecificData->iVendorSpecificPayloadStart;

        TCEA861VendorSpecificDataBlockPayload* prev =
            iVendorSpecificData->iVendorSpecificPayloadStart;

        while( last->iNext )
            {
            prev = last;
            last = last->iNext;
            }
        // now last is pointing to the last link and prev is pointing to the one before the last
        if( prev == last ) // if there is only one link left
            {
            prev = NULL;
            iVendorSpecificData->iVendorSpecificPayloadStart = NULL;
            delete last;
            // now we can delete the root as well
            delete iVendorSpecificData;
            iVendorSpecificData = NULL;
            }
        else
            {
            prev->iNext = 0;
            delete last;
            }
        }

    // delete all Video Capability Data Blockss
    while( iVideoCapabilityDataBlock )
        {
        // iterate to the end and delete the latest link
        TCEA861VideoCapabilityDataBlock* last = iVideoCapabilityDataBlock;

        TCEA861VideoCapabilityDataBlock* prev = iVideoCapabilityDataBlock;

        while( last->iNext )
            {
            prev = last;
            last = last->iNext;
            }
        // now last is pointing to the last link and prev is pointing to the one before the last
        if( prev == last )
            {
            iVideoCapabilityDataBlock = NULL;
            prev = NULL;
            delete last;
            }
        else
            {
            prev->iNext = NULL;
            delete last;
            }
        }

    // delete all descriptor blocks
    while( iDescriptorBlocks )
        {
        TCEA861TEdidDescriptorBlockList* node;
        
        node = iDescriptorBlocks;
        iDescriptorBlocks = iDescriptorBlocks->iNext;
        
        delete node;
        }
    iDescriptorBlocks = NULL;
    
    }

// End of file
