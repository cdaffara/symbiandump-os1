/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* INCLUDES
* None
* CONSTANTS
* None
* MACROS
* None
* DATA TYPES
* None
* FUNCTION PROTOTYPES
* None
* FORWARD DECLARATIONS
* None
* CHWRMVibraCommonData::MaximumOnTime
*
*/



inline TTimeIntervalMicroSeconds32 CHWRMVibraCommonData::MaximumOnTime() const
    {
    return iMaximumOnTime;
    }
    
// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::FeedbackDefaultIntensity
// -----------------------------------------------------------------------------
//
inline TInt CHWRMVibraCommonData::FeedbackDefaultIntensity() const
    {
    return iFeedbackIntensity;
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::FeedbackDefaultDuration
// -----------------------------------------------------------------------------
//
inline TInt CHWRMVibraCommonData::FeedbackDefaultDuration() const
    {
    return iFeedbackDuration;
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::FeedbackPulseSupported
// -----------------------------------------------------------------------------
//
inline TBool CHWRMVibraCommonData::FeedbackPulseSupported() const
    {
    return iPulseSupported;
    }

// End of File
