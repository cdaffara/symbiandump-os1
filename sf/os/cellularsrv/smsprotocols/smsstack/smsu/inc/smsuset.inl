// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// contains the implementation of the inline function the TSmsSettings class
// used to get and set the SMS settings
// 
//

/**
 @file
 
 Gets the time-out value.
 
 @return Time-out value 
 
*/
inline const TTimeIntervalMicroSeconds32& TSmsSettings::Timeout() const
    {
    return iTimeoutMicroSeconds32;
    }

/**
 *  Sets the time-out value.
 *  
 *  @param aTimeout Time-out value 
 */
inline void TSmsSettings::SetTimeout(const TTimeIntervalMicroSeconds32& aTimeout)
    {
    iTimeoutMicroSeconds32=aTimeout;
    }

/**
 *  Gets the maximum number of attempts to send an SMS.
 *  
 *  @return Maximum number of attempts to send a SMS 
 */
inline TInt TSmsSettings::SendTryLimit() const
    {
    return iSendTryLimit;
    }

/**
 *  Sets the maximum number of attempts to send an SMS.
 *  
 *  @param aLimit Maximum number of attempts to send a SMS 
 */
inline void TSmsSettings::SetSendTryLimit(TInt aLimit)
    {
    iSendTryLimit=aLimit;
    }

/**
 *  Gets whether the PDU can be deleted from the SIM.
 *  
 *  @return True if the PDU can be deleted from the SIM 
 */
inline TBool TSmsSettings::DeletePDUsFromSIM() const
    {
    return iFlags&ESmsFlagDeletePDUsFromSIM;
    }

/**
 *  Sets whether the PDU can be deleted from the SIM.
 *  
 *  @param aDelete True if the PDU can be deleted from the SIM 
 */
inline void TSmsSettings::SetDeletePDUsFromSIM(TBool aDelete)
    {
    iFlags=aDelete? iFlags|ESmsFlagDeletePDUsFromSIM: iFlags&(~ESmsFlagDeletePDUsFromSIM);
    }

/**
 *  Gets whether the PDU can be deleted from the phone store.
 *  
 *  @return True if the PDU can be deleted from the phone store 
 */
inline TBool TSmsSettings::DeletePDUsFromPhoneStores() const
    {
    return iFlags&ESmsFlagDeletePDUsFromPhoneStores;
    }

/**
 *  Sets whether the PDU can be deleted from the phone store.
 *  
 *  @param aDelete True if the PDU can be deleted from phone store 
 */
inline void TSmsSettings::SetDeletePDUsFromPhoneStores(TBool aDelete)
    {
    iFlags=aDelete? iFlags|ESmsFlagDeletePDUsFromPhoneStores: iFlags&(~ESmsFlagDeletePDUsFromPhoneStores);
    }

/**
 *  Gets whether the PDU can be deleted from the combined store.
 *  
 *  @return True if the PDU can be deleted from the combined store 
 */
inline TBool TSmsSettings::DeletePDUsFromCombinedStores() const
    {
    return iFlags&ESmsFlagDeletePDUsFromCombinedStores;
    }

/**
 *  Sets whether the PDU can be deleted from the combined store.
 *  
 *  @param aDelete True if the PDU can be deleted from the combined store 
 */
inline void TSmsSettings::SetDeletePDUsFromCombinedStores(TBool aDelete)
    {
    iFlags=aDelete? iFlags|ESmsFlagDeletePDUsFromCombinedStores: iFlags&(~ESmsFlagDeletePDUsFromCombinedStores);
    }

/**
 *  Gets the value of the Reassembly store life time.
 *  
 *  @return Reassembly store life time 
 */
inline const TTimeIntervalMinutes& TSmsSettings::ReassemblyLifetime() const
    {
    return iReassemblyLifetime;
    }

/**
 *  Sets the value of the Reassembly store life time.
 *  
 *  @param aReassemblyLifetime Reassembly store life time 
 */
inline void TSmsSettings::SetReassemblyLifetime(const TTimeIntervalMinutes& aReassemblyLifetime)
    {
    iReassemblyLifetime=aReassemblyLifetime;
    }

/**
 *  Gets the value of the segmentation store multiplier.
 *  
 *  @return Segmentation store multiplier 
 */
inline TInt TSmsSettings::KSegmentationLifetimeMultiplier() const
    {
    return iKSegmentationLifetimeMultiplier;
    }

/**
 *  Sets the value of the segmentation store multiplier.
 *  
 *  @param aKSegmentationLifetimeMultiplier Segmentation store multiplier 
 */
inline void TSmsSettings::SetKSegmentationLifetimeMultiplier(TInt aKSegmentationLifetimeMultiplier)
    {
    iKSegmentationLifetimeMultiplier=aKSegmentationLifetimeMultiplier;
    }

/**
 *  Gets the value of the modem initialisation timeout duration.
 *  
 *  @return Modem initialisation timeout duration 
 */
inline const TTimeIntervalMicroSeconds32& TSmsSettings::ModemInitializationTimeout() const
    {
    return iModemInitializationTimeoutMicroSeconds32;
    }

/**
 *  Sets the value of the modem initialisation timeout duration.
 *  
 *  @param aTimeout Modem initialisation timeout duration 
 */
inline void TSmsSettings::SetModemInitializationTimeout(const TTimeIntervalMicroSeconds32& aTimeout)
    {
    iModemInitializationTimeoutMicroSeconds32=aTimeout;
    }

/**
 *  Gets the Send Try Timeout value
 *  
 *  @return Time-out value
 */
inline const TTimeIntervalMicroSeconds32& TSmsSettings::SendTryTimeout() const
    {
    return iSendTryTimeoutMicroSeconds32;
    }

/**
 *  Sets the Send Try Timeout value.
 *  
 *  @param aTimeout Time-out value
 */
inline void TSmsSettings::SetSendTryTimeout(const TTimeIntervalMicroSeconds32& aTimeout)
    {
    iSendTryTimeoutMicroSeconds32=aTimeout;
    }

/**
 *  Gets the Boot Timer Timeout value
 *  
 *  @return Boot Timer Time-out value
 */
inline const TTimeIntervalMicroSeconds32& TSmsSettings::BootTimerTimeout()
    {
    return iBootTimerTimeout32;
    }

/**
 *  Sets the Boot Timer Timeout value
 *  
 *  @param aTimeout Time-out value 
 */
inline void TSmsSettings::SetBootTimerTimeout(const TTimeIntervalMicroSeconds32& aTimeout)
    {
    iBootTimerTimeout32 = aTimeout;
    }
