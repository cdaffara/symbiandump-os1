// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef RFCOMMFRAME_H
#define RFCOMMFRAME_H

#include <e32base.h>
#include "rfcommsap.h"
#include "rfcommconsts.h"
#include "btsockettimer.h"

/**
   Abstract base class for the different Rfcomm frame types

   All RFCOMM (TS07.10) frames have the following format:
   @verbatim
   Field  | Address  |  Control | Length       | Data        | FCS     |
   Length |   1 byte |  1 byte  | 1 or 2 bytes | 0..* bytes  | 1 byte  |
   @endverbatim

   Classes derived from this one provide a typeId so that the
   appropriate matching can be done when working out which command a
   response is for, since this depends on the type of the frame.

   All frames have the notion of whether they need a response, and
   also have an associated response timer if necessary.
   
**/

NONSHARABLE_CLASS(CRfcommFrame) : public CBase
	{
friend class CRfcommMuxer;
public:
	CRfcommFrame(CRfcommMuxer& aMux, CRfcommSAP* aSAP);
	CRfcommFrame(CRfcommMuxer& aMux);
	virtual ~CRfcommFrame();
	void SetAddress(TUint8 aChan);
	TUint8 Address() const;
	void SetControl(TUint8 aCtrl);
	TUint8 Ctrl() const;
	virtual TUint8 Credit() const;
	virtual void SetCredit(TUint8 aCredit);
	virtual TUint16 DataLength() const;
	CRfcommSAP* SAP() const;
	
	virtual const TDesC8& Data() =0;
	virtual TInt Type() const =0;
	virtual TBool ResponseNeeded() =0;
	virtual void QueResponseTimer() =0;
	virtual void DequeTimer() =0;
	virtual TBool Priority() const =0;

protected:
	TUint8 iAddr;
	TUint8 iCtrl;
	CRfcommMuxer& iMuxer;	//< The muxer this frame belongs to
	CRfcommSAP*  iSAP;		//< The sap this frame belongs to, if any - could be NULL
	TDblQueLink iLink;
	};

/**
   Represents the non-data frames that exist in RFCOMM

   These types are as follows:
   
   SABM :  Creates a channel
   UA   :  Acknowledgement
   DM   :  Disconnected mode - sent for a channel that is not on
   DISC :  Disconnects a channel
   

   FCS is calculated using a polynomial CRC.

   All these frames have a length of 0 and no user data.

   SABM and DISC frames require a response, so objects of this class
   have associated timer entries
**/

NONSHARABLE_CLASS(CRfcommCtrlFrame) : public CRfcommFrame
	{
public:
	CRfcommCtrlFrame(CRfcommMuxer& aMux, CRfcommSAP* aSAP);
	CRfcommCtrlFrame(CRfcommMuxer& aMux);
	~CRfcommCtrlFrame();
	void SetResponseNeeded(TBool aNeed);
	
	virtual const TDesC8& Data();
	virtual TInt Type() const;
	virtual TBool ResponseNeeded();
	virtual void QueResponseTimer();
	virtual void DequeTimer();
	virtual TBool Priority() const;

private:
	static TInt TimerExpired(TAny* aFrame);
	TBool						iResponseNeeded;
	TDeltaTimerEntry			iTimerEntry;
	TBool						iTimerQueued;
	TBuf8<KRfcommCtrlFrameSize> iData;
	};


/**
   Represents a data frame in RFCOMM

   UIH frames carry both user data and mux commands between the
   endpoints.  Thus this class is the base for mux ctrl frames.

   A simple, stream-like store API is defined so that subsequent
   writes occur further into the packet than earlier ones.
   
//   Note that with this implementation only single byte lengths are
//   supported, and a fixed sized buffer is used to contain the data.
// Mel no longer true but we have to  reorganise this to remove the dependency
// of Ctrl frames on data frames
**/
NONSHARABLE_CLASS(CRfcommUIHFrame) : public CRfcommFrame
	{
public:

	static CRfcommUIHFrame* NewL(TInt aInformationLength, CRfcommMuxer& aMux, CRfcommSAP* aSAP=0);

	virtual ~CRfcommUIHFrame();
	
	virtual void PutByte(TUint8 aByte);
	virtual void PutLittleEndian16(TUint16 aShort);
	virtual void PutLittleEndian32(TUint32 aLong);
	virtual void PutData(const TDesC8& aDes);
	
	virtual const TDesC8& Data();
	virtual TInt Type() const;
	virtual TBool ResponseNeeded();
	virtual void QueResponseTimer();
	virtual void DequeTimer();
	virtual TBool Priority() const;
	virtual TUint16 DataLength() const;
	
protected:
	
	CRfcommUIHFrame(CRfcommMuxer& aMux, CRfcommSAP* aSAP);
	
	//	Called as part of NewL
	virtual void ConstructDataBufferL(TInt aInformationLength);

	virtual void PrepareDataFrame();
	void PutByteAt(TInt aOffset, TUint8 aByte);

protected:

	HBufC8*	iData;
	TPtrC8	iReturnedFrame;
	TBool iFramePrepared;
	};

/**
   Represents a UIH frame in RFCOMM which is used for carrying information  
**/
NONSHARABLE_CLASS(CRfcommDataFrame) : public CRfcommUIHFrame
	{
public:

	static CRfcommDataFrame* NewL(TInt aInformationLength, CRfcommMuxer& aMux, CRfcommSAP* aSAP=0);

	virtual ~CRfcommDataFrame();
	
	
protected:
	
	CRfcommDataFrame(CRfcommMuxer& aMux, CRfcommSAP* aSAP);
	
	//	Called as part of NewL
	virtual void ConstructDataBufferL(TInt aInformationLength);

	virtual void PrepareDataFrame();

	};

/**
   Represents a data frame in RFCOMM which also carries a credit  
**/
NONSHARABLE_CLASS(CRfcommCreditDataFrame) : public CRfcommDataFrame
	{
public:

	static CRfcommCreditDataFrame* NewL(TInt aInformationLength, CRfcommMuxer& aMux, CRfcommSAP* aSAP=0);

	virtual ~CRfcommCreditDataFrame();
	
	virtual TInt Type() const;
	virtual TUint16 DataLength() const;
	virtual void SetCredit(TUint8 aCredit);
	virtual TUint8 Credit() const;

protected:
	
	CRfcommCreditDataFrame(CRfcommMuxer& aMux, CRfcommSAP* aSAP);
	
	//	Called as part of NewL
	virtual void ConstructDataBufferL(TInt aInformationLength);

	virtual void PrepareDataFrame();

private:

	TUint8 iCredit;	// what we are doling out this frame
	};

/**
   Represents mux control channel commands and responses

   These are carried in UIH frames, but as commands require responses
   the infrastructure for these has been added.
 **/
NONSHARABLE_CLASS(CRfcommMuxCtrlFrame) : public CRfcommUIHFrame
	{
friend class CRfcommMuxer;
public:
	
	static CRfcommMuxCtrlFrame* NewL(TUint8 aCommandValuesLength, CRfcommMuxer& aMux, CRfcommSAP* aSAP=0);

	~CRfcommMuxCtrlFrame();
	void SetCommandType(TUint8 aType, TBool aCommand);
	TUint8 CommandType() const;
	void SetResponseNeeded(TBool aNeed);

	virtual TInt Type() const;
	virtual TBool ResponseNeeded();
	virtual void QueResponseTimer();
	virtual void DequeTimer();
	virtual TBool Priority() const;
	
	//	NB. MuxCtrlFrames are always sent on DLCI 0 (i.e. the Mux Control Channel).
	//	The SetDLCI / DLCI members refer to the actual channel which is being addressed,
	//	rather than the channel on which this frame is being sent.
	//
	void SetDLCI(TUint8 aDLCI);
	TUint8 DLCI() const;

protected:

	//	Called as part of NewL
	virtual void ConstructDataBufferL(TInt aCommandValuesLength);
	virtual void PrepareDataFrame();

private:
	CRfcommMuxCtrlFrame(TUint8 aCommandValuesLength, CRfcommMuxer& aMux, CRfcommSAP* aSAP);

	static TInt TimerExpired(TAny* aFrame);
	TBool iResponseNeeded;
	TUint8 iCommandType;
	TUint8 iCommandLength;
	TDeltaTimerEntry iTimerEntry;
	TBool iTimerQueued;
	TUint8 iDLCI;
	};

#include "rfcommframe.inl"
#endif
