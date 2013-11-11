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
// contains header file of the SMS settings.
// 
//

/**
 @file
*/


#if !defined (__SMSUSET_H__)
#define __SMSUSET_H__

#if !defined (__E32STD_H__)
#include "e32std.h"
#endif


class RReadStream;
class RWriteStream;


/**
 *  SMS settings.
 *  
 *  SMSPROT uses an instance of TSmsSettings to configure the sms stack timeouts.
 *  
 *  The SMSPROT calls TSmsSettings' default constructor during initialisation.
 *  It then parses smswap.sms.esk for customised settings and updates its instance of
 *  TSmsSettings accordingly.
 *  
 *  Customised settings in smswap.sms.esk should comply with the following syntax:
 *  
 *  [customTimeoutSettings]
 *  sendTryTimeout= value
 *  bootTimerTimeout = value
 *  
 *  where the identifier 'value' is to be substituted with an integer value.
 *  Integer values less than zero will cause the stack to leave.
 *  
 *  Only parameters that need to be customised are required in smswap.sms.esk
 *  
 *  @publishedAll
 *  @released
 */
class TSmsSettings
	{
public:
	IMPORT_C TSmsSettings();

	inline const TTimeIntervalMicroSeconds32& Timeout() const;
	inline void SetTimeout(const TTimeIntervalMicroSeconds32& aTimeout);
	inline TInt SendTryLimit() const;
	inline void SetSendTryLimit(TInt aLimit);
	inline TBool DeletePDUsFromSIM() const;
	inline void SetDeletePDUsFromSIM(TBool aDelete);
	inline TBool DeletePDUsFromPhoneStores() const;
	inline void SetDeletePDUsFromPhoneStores(TBool aDelete);
	inline TBool DeletePDUsFromCombinedStores() const;
	inline void SetDeletePDUsFromCombinedStores(TBool aDelete);
	inline const TTimeIntervalMinutes& ReassemblyLifetime() const;
	inline void SetReassemblyLifetime(const TTimeIntervalMinutes& aReassemblyLifetime);
	inline TInt KSegmentationLifetimeMultiplier() const;
	inline void SetKSegmentationLifetimeMultiplier(TInt aKSegmentationLifetimeMultiplier);
	inline const TTimeIntervalMicroSeconds32& ModemInitializationTimeout() const;
	inline void SetModemInitializationTimeout(const TTimeIntervalMicroSeconds32& aTimeout);
	inline const TTimeIntervalMicroSeconds32& SendTryTimeout() const;
 	inline void SetSendTryTimeout(const TTimeIntervalMicroSeconds32& aTimeout);
 	inline const TTimeIntervalMicroSeconds32& BootTimerTimeout();
 	inline void SetBootTimerTimeout(const TTimeIntervalMicroSeconds32& aTimeout);

	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

private:
	enum
		{
		KDefaultTimeoutMicroSeconds32=60000000 //<60 sec
		};
	enum
		{
		KDefaultSendTryLimit=4
		};
	enum
		{
		KDefaultReassemblyLifetime=2*1440 //< two days
		};
	enum
		{
		KDefaultKSegmentationLifetimeMultiplier=1200  //< 1.2
		};
	enum
		{
		KDefaultModemInitializationTimeoutMicroSeconds32=30000000 //<30sec
		};
	enum
		{
		KDefaultSendTryTimeoutMicroSeconds32=60000000 //< 60sec
		};
	enum
	    {
        KDefaultBootTimerMicroSeconds32=60000000 // 60sec
	    };
	enum TSmsSettingsFlags
		{
		ESmsFlagDeletePDUsFromSIM=0x01,
		ESmsFlagDeletePDUsFromPhoneStores=0x02,
		ESmsFlagDeletePDUsFromCombinedStores=0x04
		};
private:
	TTimeIntervalMicroSeconds32 iTimeoutMicroSeconds32;	//<TimeOut period in microseconds for sending a PDU
	/**
	 *  Number of attempts for sending a PDU.
	 *  	@deprecated 7.0	
	 */
	TInt iSendTryLimit;
	TInt iFlags;								//<Contains TSmsSettingsFlags
	TTimeIntervalMinutes iReassemblyLifetime;//< Maximum time PDUs making a concatenated message stay in the reassembly store before they have all arrived
	TInt iKSegmentationLifetimeMultiplier;  //<  Validity period is multiplied by this fraction, 1000=1:1
	TTimeIntervalMicroSeconds32 iModemInitializationTimeoutMicroSeconds32;//<TimeOut period in microseconds for initialising the modem
	TTimeIntervalMicroSeconds32 iSendTryTimeoutMicroSeconds32;//< TimeOut period in microseconds for the send operation
    TTimeIntervalMicroSeconds32 iBootTimerTimeout32; // TimeOut period in microseconds for the boot timer.
	};

#include "smsuset.inl"

#endif // !defined __SMSUSET_H__
