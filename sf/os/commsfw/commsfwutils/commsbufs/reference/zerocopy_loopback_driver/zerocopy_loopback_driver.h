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

#ifndef __ZEROCOPY_LOOPBACK_DRIVER_H__
#define __ZEROCOPY_LOOPBACK_DRIVER_H__

#include <e32cmn.h>
#include <e32ver.h>

#ifndef __KERNEL_MODE__
#include <e32shbuf.h>
#include <comms-infras/commsbuf.h>
#include <comms-infras/commsbufpond.h>
#endif

_LIT(KZeroCopyLoopbackLddFileName,"zerocopy_loopback_driver.ldd");
_LIT(KZeroCopyLoopbackPddFileName,"zerocopy_loopback_driver.pdd");

class RShPool;
class RCommsBuf;

class TShCommsBufBase;

/**
User interface for 'zerocopy loopback'
*/
class RZeroCopyLoopbackDriver : public RBusLogicalChannel
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

#ifndef __KERNEL_MODE__
	inline TInt LoadPond(RCommsBufPond& aCommsPond);
#endif
	inline TInt UnloadPond();

	inline void SendData(TRequestStatus &aStatus, const RCommsBuf& aData);
	inline void SendDataCancel();
	inline void ReceiveData(TRequestStatus &aStatus);
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
		ESetConfig,
		ELoadPond,
		EUnloadPond
		};

	/**
	Enumeration of Request messages.
	*/
	enum TRequest
		{
		ESendData,
		EReceiveData,
		ESendData_ByZeroCopy,
		EReceiveData_ByZeroCopy,
		ENumRequests,
		EAllRequests = (1<<ENumRequests)-1
		};

	// Kernel side LDD channel is a friend
	friend class DZeroCopyLoopbackChannel;
	};

/**
  The driver's name

  @return The name of the driver

  @internalComponent
*/
inline const TDesC& RZeroCopyLoopbackDriver::Name()
	{
	_LIT(KDriver1Name,"ZeroCopyLoopbackDriver");
	return KDriver1Name;
	}

/**
  The driver's version

  @return The version number of the driver

  @internalComponent
*/
inline TVersion RZeroCopyLoopbackDriver::VersionRequired()
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
inline TInt RZeroCopyLoopbackDriver::Open()
	{
	return DoCreate(Name(),VersionRequired(),KNullUnit,NULL,NULL,EOwnerThread);
	}

/**
  Get the current configuration settings.
  @param aConfig A structure which will be filled with the configuration settings.
  @return KErrNone
*/
inline TInt RZeroCopyLoopbackDriver::GetConfig(TConfigBuf& aConfig)
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
inline TInt RZeroCopyLoopbackDriver::SetConfig(const TConfigBuf& aConfig)
	{
	return DoControl(ESetConfig,(TAny*)&aConfig);
	}

/**
@purpose Send commspond to the driver. It will open the pools independently in order to make use of 
buffers sent during ::Send and to allocate buffers that comms can make use of on packet reception.
@param aCommsPond The comms buffer pond to transfer.
*/
inline TInt RZeroCopyLoopbackDriver::LoadPond(RCommsBufPond& aCommsPond)
	{
	TPondTransferBuf buf;
	buf.SetMax();
	TInt error = aCommsPond.Store(buf);
	if(error != KErrNone)
		{
		return error;
		}
	
	return DoControl(ELoadPond, (TAny*)(&buf));
	}

/**
@purpose Instructs driver to unload its pond.
*/
inline TInt RZeroCopyLoopbackDriver::UnloadPond()
	{
	return DoControl(EUnloadPond);
	}

/**
  Send data to the device.
  Only one send request may be pending at any time.

  @param aStatus The request to be signaled when the data has been sent.
		         The result value will be set to KErrNone on success;
		         or set to one of the system wide error codes when an error occurs.
  @param aData   A descriptor containing the data to send.
*/
inline void RZeroCopyLoopbackDriver::SendData(TRequestStatus &aStatus, const RCommsBuf& aData)
	{
	RCommsBufAccessor sendBuf(aData);
	DoRequest(ESendData_ByZeroCopy, aStatus, (TAny*)(sendBuf.Handle()));
	}


/**
  Cancel a previous SendData request.
*/
inline void RZeroCopyLoopbackDriver::SendDataCancel()
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
inline void RZeroCopyLoopbackDriver::ReceiveData(TRequestStatus &aStatus)
	{
	DoRequest(EReceiveData_ByZeroCopy, aStatus, NULL);
	}

/**
  Cancel a previous ReceiveData request.
*/
inline void RZeroCopyLoopbackDriver::ReceiveDataCancel()
	{
	DoCancel(1<<EReceiveData);
	}

#endif  // !__KERNEL_MODE__

#endif

