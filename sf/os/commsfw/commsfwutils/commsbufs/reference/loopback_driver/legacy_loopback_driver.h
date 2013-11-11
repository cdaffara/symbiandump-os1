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
//

#ifndef _LEGACY_LOOPBACK_DRIVER_H_
#define _LEGACY_LOOPBACK_DRIVER_H_

#include <e32cmn.h>
#include <e32ver.h>
#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif

_LIT(KLegacyLoopbackLddFileName,"legacy_loopback.ldd");
_LIT(KLegacyLoopbackPddFileName,"legacy_loopback.pdd");


/**
User interface for 'ESockLoopbackDriver'
*/
class RLegacyLoopbackDriver : public RBusLogicalChannel
	{
public:
	enum { KLoopbackMTU = 1600 };
	
	/**
	Structure for holding driver capabilities information
	(Just a version number in this example.)
	*/
	class TCaps
		{
	public:
		TVersion iVersion;
		};

	/**
	Structure for holding driver configuration data
	*/
	class TConfig
		{
	public:
		TInt iSpeed;				/**< Data transfer speed in microseconds/byte */
		TInt iPddBufferSize;		/**< Size of the PDD's data buffer (not modifiable) */
		TInt iMaxSendDataSize;		/**< Maximum size of data which can be sent in one go (not modifiable) */
		TInt iMaxReceiveDataSize;	/**< Maximum size of data which can be received in one go (not modifiable) */
		};
	typedef TPckgBuf<TConfig> TConfigBuf;

public:
	inline TInt Open();
	inline TInt GetConfig(TConfigBuf& aConfig);
	inline TInt SetConfig(const TConfigBuf& aConfig);
	inline void SendData(TRequestStatus &aStatus,const TDesC8& aData);
	inline void SendDataCancel();
	inline void ReceiveData(TRequestStatus &aStatus,TDes8& aBuffer);
	inline void ReceiveDataCancel();
	inline static const TDesC& Name();
	inline static TVersion VersionRequired();
private:
	/**
	Enumeration of Control messages.
	*/
	enum TControl
		{
		EGetConfig,
		ESetConfig
		};

	/**
	Enumeration of Request messages.
	*/
	enum TRequest
		{
		ESendData,
		EReceiveData,
		ENumRequests,
		EAllRequests = (1<<ENumRequests)-1
		};

	// Kernel side LDD channel is a friend
	friend class DESockLoopbackChannel;
	};

/**
  The driver's name

  @return The name of the driver

  @internalComponent
*/
inline const TDesC& RLegacyLoopbackDriver::Name()
	{
	_LIT(KDriver1Name,"legacy_loopback");
	return KDriver1Name;
	}

/**
  The driver's version

  @return The version number of the driver

  @internalComponent
*/
inline TVersion RLegacyLoopbackDriver::VersionRequired()
	{
	const TInt KMajorVersionNumber=1;
	const TInt KMinorVersionNumber=0;
	const TInt KBuildVersionNumber=KE32BuildVersionNumber;
	return TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
	}

/*
  NOTE: The following methods would normally be exported from a seperate client DLL
  but are included inline in this header file for convenience.
*/

#ifndef __KERNEL_MODE__

/**
  Open a logical channel to the driver

  @return One of the system wide error codes.
*/
inline TInt RLegacyLoopbackDriver::Open()
	{
	return DoCreate(Name(),VersionRequired(),KNullUnit,NULL,NULL,EOwnerThread);
	}

/**
  Get the current configuration settings.

  @param aConfig A structure which will be filled with the configuration settings.

  @return KErrNone
*/
inline TInt RLegacyLoopbackDriver::GetConfig(TConfigBuf& aConfig)
	{
	return DoControl(EGetConfig,(TAny*)&aConfig);
	}

/**
  Set the current configuration settings.

  @param aConfig The new configuration settings to be used.

  @return KErrInUse if there are outstanding data transfer requests.
          KErrArgument if any configuration values are invalid.
		  KErrNone otherwise
*/
inline TInt RLegacyLoopbackDriver::SetConfig(const TConfigBuf& aConfig)
	{
	return DoControl(ESetConfig,(TAny*)&aConfig);
	}

/**
  Send data to the device.
  Only one send request may be pending at any time.

  @param aStatus The request to be signaled when the data has been sent.
		         The result value will be set to KErrNone on success;
		         or set to one of the system wide error codes when an error occurs.
  @param aData   A descriptor containing the data to send.
*/
inline void RLegacyLoopbackDriver::SendData(TRequestStatus &aStatus,const TDesC8& aData)
	{
	DoRequest(ESendData,aStatus,(TAny*)&aData);
	}

/**
  Cancel a previous SendData request.
*/
inline void RLegacyLoopbackDriver::SendDataCancel()
	{
	DoCancel(1<<ESendData);
	}

/**
  Receive data from the device.
  Only one receive request may be pending at any time.

  @param aStatus The request to be signaled when the data has been received.
		         The result value will be set to KErrNone on success;
		         or set to one of the system wide error codes when an error occurs.
  @param aData	A descriptor to which the received data will be written.
*/
inline void RLegacyLoopbackDriver::ReceiveData(TRequestStatus &aStatus,TDes8& aBuffer)
	{
	DoRequest(EReceiveData,aStatus,(TAny*)&aBuffer);
	}

/**
  Cancel a previous ReceiveData request.
*/
inline void RLegacyLoopbackDriver::ReceiveDataCancel()
	{
	DoCancel(1<<EReceiveData);
	}

#endif  // !__KERNEL_MODE__

#endif

