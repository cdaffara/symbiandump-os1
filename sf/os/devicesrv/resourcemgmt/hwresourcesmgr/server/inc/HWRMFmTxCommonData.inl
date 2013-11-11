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
* CHWRMFmTxCommonData::Status
*
*/



inline TFmTxState CHWRMFmTxCommonData::Status() const
    {
    return iFmTxState;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::IsRdsInitialised
// -----------------------------------------------------------------------------
//
inline TBool CHWRMFmTxCommonData::IsRdsInitialised() const
    {
    return iRdsInitialised;
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::IsFmTxHardwareOn
// -----------------------------------------------------------------------------
//
inline TBool CHWRMFmTxCommonData::IsFmTxHardwareOn() const
    {
    return ( iFmTxState == EFmTxStateActive || iFmTxState == EFmTxStateInactive || iFmTxState == EFmTxStateScanning ) ? ETrue : EFalse;
    }
        
// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::IsFmTxHardwareOn
// -----------------------------------------------------------------------------
//
inline TBool CHWRMFmTxCommonData::IsFmTxHardwareOn( TFmTxState aState ) const
    {
    return ( aState == EFmTxStateActive || aState == EFmTxStateInactive || aState == EFmTxStateScanning ) ? ETrue : EFalse;
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::Frequency
// -----------------------------------------------------------------------------
//
inline TInt CHWRMFmTxCommonData::Frequency() const
    {
    return iFmTxFrequency;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::FrequencyRangeUpdated
// -----------------------------------------------------------------------------
//
inline TBool CHWRMFmTxCommonData::FrequencyRangeUpdated() const
    {
    return iFrequencyRangeUpdated;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::GetRdsTextConverter
// -----------------------------------------------------------------------------
//
inline CHWRMFmTxRdsTextConverter* CHWRMFmTxCommonData::GetRdsTextConverter()
    {
    return iRdsTextConverter;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::RdsData
// -----------------------------------------------------------------------------
//
inline TFmTxRdsData CHWRMFmTxCommonData::RdsData() const
    {
    return iRdsData;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::RdsPs
// -----------------------------------------------------------------------------
//
inline const TDesC8& CHWRMFmTxCommonData::ConvertedRdsPs() const
    {
    return iRdsConvertedPs;
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::RdsPtyn
// -----------------------------------------------------------------------------
//
inline const TDesC8& CHWRMFmTxCommonData::ConvertedRdsPtyn() const
    {
    return iRdsConvertedPtyn;
    }    

// -----------------------------------------------------------------------------
// CHWRMFmTxCommonData::PowerSaveTimeout
// -----------------------------------------------------------------------------
//
inline TUint32 CHWRMFmTxCommonData::PowerSaveTimeout() const
    {
    return iFmTxPowerSaveTimeout;
    }    

// End of File
