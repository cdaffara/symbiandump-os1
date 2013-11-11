// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LBSX3P_H_
#define LBSX3P_H_

#include <e32def.h>
#include <lbs/lbsloccommon.h>

// Forward declarations
class CLbsTransmitPositionImpl;
class CLbsTransmitPositionServerImpl;
class TPositionInfo;


NONSHARABLE_CLASS(TLbsTransmitPositionOptions) : public TLbsLocClassTypeBase 
/**
The class used for storing transmit position to third party options
@publishedPartner
@released
 */
{
	
public:
	IMPORT_C TLbsTransmitPositionOptions(); 
	IMPORT_C TLbsTransmitPositionOptions(const TTimeIntervalMicroSeconds& aTimeOut); 
	
	IMPORT_C void SetTimeOut(const TTimeIntervalMicroSeconds& aTimeOut);
	IMPORT_C TTimeIntervalMicroSeconds TimeOut() const;

protected:
	/** Timeout */
	TTimeIntervalMicroSeconds iTimeOut;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[32];
	};


NONSHARABLE_CLASS(RLbsTransmitPositionServer)
/**
This is generally the first interface class used by all client applications.
It is used to make the primary connection to the Transmit position server. 
After the primary connection has been established, its handle is passed as a 
parameter of the Open methods of RLbsTransmitPosition to create a "sub-session".
This class is not intended for derivation. 
@publishedPartner
@released
 */
	{
public:
	IMPORT_C RLbsTransmitPositionServer();

	IMPORT_C TInt Connect();
	IMPORT_C void Close();

	IMPORT_C TVersion Version() const;
	/**
	Returns reference to implementation. Impl() is for internal use only.
	 */
	CLbsTransmitPositionServerImpl& Impl();

private:
	IMPORT_C virtual void ConstructL();
	IMPORT_C virtual void Destruct();

	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
private:
	/** A pointer to a container that holds the implementation */
	CLbsTransmitPositionServerImpl* iImpl;
	
	/** Unused variable for future expansion. */
	TAny* iReserved;
	};


NONSHARABLE_CLASS(RLbsTransmitPosition)
/**
This class is used to create a sub-session with the server for 
the purpose of transmitting the current position to a third party.
Before using the class, a primary connection must have already been
established with the server.
This class is not intended for derivation. 

@see RLbsTransmitPositionServer
@publishedPartner
@released
 */
	{

public:
	IMPORT_C RLbsTransmitPosition();

	IMPORT_C TInt Open(RLbsTransmitPositionServer& aServer);
	IMPORT_C void Close();

	IMPORT_C void TransmitPosition(const TDesC& aDestinationID,TUint aTransmitPriority, 
									TRequestStatus& aTransmittedPosStatus, TPositionInfo& aTransmittedPosInfo);
									
	IMPORT_C void TransmitPosition(const TDesC& aDestinationID,TUint aTransmitPriority, 
									TRequestStatus& aRefPosStatus, TPositionInfo& aRefPosInfo,
									TRequestStatus& aTransmittedPosStatus, TPositionInfo& aTransmittedPosInfo);	
																	
	IMPORT_C void CancelTransmitPosition();

	IMPORT_C TInt SetTransmitOptions(const TLbsTransmitPositionOptions& aTransmitOptions);
	IMPORT_C TInt GetTransmitOptions(TLbsTransmitPositionOptions& aTransmitOptions) const;

private:
 	IMPORT_C virtual void ConstructL(RLbsTransmitPositionServer& aServer);
	IMPORT_C virtual void Destruct();

	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
private:
	/** A pointer to a container that holds the implementation */
	CLbsTransmitPositionImpl* iImpl;
	/** Unused variable for future expansion. */
	TAny* iReserved;
	};



#endif // LBSX3P_H_

