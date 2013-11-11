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

#include <bluetooth/logger.h>
#include <es_sock.h>
#include "rfcommframe.h"
#include "rfcommmuxer.h"
#include "rfcommconsts.h"
#include "rfcommutil.h"
#include "rfcommfcs.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_RFCOMM);
#endif

CRfcommFrame::~CRfcommFrame()
	{
	iLink.Deque();
	}

/*
  CRfcommCtrlFrame is used for SABM, UA, DISC & DM frames
*/

CRfcommCtrlFrame::CRfcommCtrlFrame(CRfcommMuxer& aMux, CRfcommSAP* aSAP)
	: CRfcommFrame(aMux, aSAP),
	  iData(KRfcommCtrlFrameSize)
	{
	LOG3(_L("RFCOMM: Ctrl Frame 0x%08x created, sap 0x%08x, len %d"),
				  this, aSAP, iData.Length());
	}

CRfcommCtrlFrame::CRfcommCtrlFrame(CRfcommMuxer& aMux)
	: CRfcommFrame(aMux),
	  iData(KRfcommCtrlFrameSize)
	{
	}

CRfcommCtrlFrame::~CRfcommCtrlFrame()
	{
	DequeTimer();
	}

TBool CRfcommCtrlFrame::ResponseNeeded()
	{
	return iResponseNeeded;
	}

void CRfcommCtrlFrame::QueResponseTimer()
	{
	if(iResponseNeeded && !iTimerQueued)
		{
		// Enque this timer
		TCallBack cb(TimerExpired, this);
		iTimerEntry.Set(cb);
		//	Timer length dependent on whether this is a SABM for a DLCI other than 0
		BTSocketTimer::Queue(
			( ((iCtrl&~KPollFinalBitmask)==KSABMCtrlField) && ((iAddr>>2)!=KMuxDLCI) 
			?KFrameTimerT1SABM:KFrameTimerT1), iTimerEntry);
		iTimerQueued=ETrue;
		}
	}

void CRfcommCtrlFrame::DequeTimer()
	{
	if(iTimerQueued)
		{
		BTSocketTimer::Remove(iTimerEntry);
		iTimerQueued=EFalse;
		}
	}

TBool CRfcommCtrlFrame::Priority() const
	/**
	Check whether this RfcommCtrlFrame should 
	a) jump past the data frames in the queue and 
	b) be blocked by FCON/FCOFF flow control.

	If the function returns true, the frame will jump and
	not be affected by flow control.
	**/
	{
	//FC ?
	switch(iCtrl)
		{
		case KUIHCBFCCtrlField:
			if(!DataLength())
				return ETrue;
		default:
			break;
		}
	switch(iCtrl&~KPollFinalBitmask)
		{
		//
		case KUACtrlField:
		//	Deliberate fall through...
		case KDMCtrlField:
		//	High priority for UAs and DMs
			return ETrue;
		default:
		//	Others (i.e. SABMs and DISCs) obey flow control.
			return EFalse;
		}
	//
	}

TInt CRfcommCtrlFrame::TimerExpired(TAny* aFrame)
	/**
	   The T1 timer on a ctrl frame has expired

	   This is probably fatal to the mux, and certainly fatal
	   for any SAP associated with frame.
	**/
	{
	CRfcommCtrlFrame* frm=static_cast<CRfcommCtrlFrame*>(aFrame);
	// Notify the mux that this frame has timed out
	LOG2(_L("RFCOMM: Ctrl frame timeout, frame %08x, ctrl %d"),
				  frm, frm->Ctrl());
		
	frm->iTimerQueued=EFalse;
	frm->iMuxer.FrameResponseTimeout(frm);
	return EFalse;
	}


void CRfcommCtrlFrame::SetResponseNeeded(TBool aNeed)
	{
	iResponseNeeded=aNeed;
	}

const TDesC8& CRfcommCtrlFrame::Data()
	/**
	   Returns a reference to the data.

	   This function first builds up the data buffer, then sets the length
	   and the FCS before returning the reference.
	**/
	{
	__ASSERT_DEBUG(iCtrl != KUIHCtrlField, Panic(ERfcommBadFrameCtrlField));
	__ASSERT_DEBUG(iData.Length()>=4, Panic(ERfcommBadFrameCtrlField));
	LOG1(_L("CRfcommCtrlFrame::Data() - formatting data, length %d"), iData.Length());
	iData[0]=iAddr;
	iData[1]=iCtrl;
	iData[2]=KRfcommCtrlFrameLengthByte;
	iData[3]=CalcFCS(iData.Ptr(), 3);  // Calc over all three bytes
	return iData;
	}

TInt CRfcommCtrlFrame::Type() const //Shehla - added for ease of Blogging
	/**
	   Returns the typeid for this class
	**/
	{
	return KCtrlFrameType;
	}

/*
  CRfcommUIHFrame is used for all UIH data frames flowing over the link
*/

CRfcommUIHFrame* CRfcommUIHFrame::NewL(TInt aInformationLength, CRfcommMuxer& aMux, CRfcommSAP* aSAP)
	{
	CRfcommUIHFrame* self=new(ELeave) CRfcommUIHFrame( aMux, aSAP);
	CleanupStack::PushL(self);
	self->ConstructDataBufferL(aInformationLength);
	CleanupStack::Pop();	//	self
	LOG2(_L("RFCOMM: Data Frame 0x%08x created, sap 0x%08x"), self, aSAP);
	return self;
	}

CRfcommUIHFrame::CRfcommUIHFrame(CRfcommMuxer& aMux, CRfcommSAP* aSAP)
	: CRfcommFrame(aMux, aSAP)
	{
	}

CRfcommUIHFrame::~CRfcommUIHFrame()
	{
	delete iData;
	}

void CRfcommUIHFrame::ConstructDataBufferL(TInt aInformationLength)
	/**
		Allocate the data buffer to hold the information that will be placed in this frame.

		An allowance is made for the KMaxFrameHeaderLength, ie. addr, ctrl , 2*len = 4 octets
	**/
	{
	iData=HBufC8::NewL(aInformationLength+KMaxFrameOverhead);
	//	Reserve max possible space for addr, control, length, and fcs at end
	for(TUint8 i=0;i<KMaxFrameOverhead-1;++i)
		PutByte(0x00);	
	}

TInt CRfcommUIHFrame::Type() const
	{
	return KDataFrameType;
	}

TBool CRfcommUIHFrame::ResponseNeeded()
	{
	return EFalse;
	}

void CRfcommUIHFrame::QueResponseTimer()
	/**
	  Never a response for data frames, so do nothing
	**/
	{
	// Nothing
	}

void CRfcommUIHFrame::DequeTimer()
	{
	// Do nothing
	}

TBool CRfcommUIHFrame::Priority() const
	{
	return EFalse;
	}

TUint16 CRfcommUIHFrame::DataLength() const
	{
	TInt overhead = iFramePrepared?KMaxFrameOverhead:KMaxFrameHeaderLength;
	TUint16 length=static_cast<TUint16>((*iData).Length()-overhead);
	return length;
	}

void CRfcommUIHFrame::PutByte(TUint8 aByte)
	{
	iData->Des().Append(aByte);
	}

void CRfcommUIHFrame::PutLittleEndian16(TUint16 aVal)
	{
	TUint8* ptr=const_cast<TUint8*>((*iData).Ptr());
	ptr+=(*iData).Length();
	iData->Des().SetLength((*iData).Length()+2);
	LittleEndian::Put16(ptr, aVal);
	}

void CRfcommUIHFrame::PutLittleEndian32(TUint32 aVal)
	{
	TUint8* ptr=const_cast<TUint8*>((*iData).Ptr());
	ptr+=(*iData).Length();
	iData->Des().SetLength((*iData).Length()+4);
	LittleEndian::Put32(ptr, aVal);
	}

void CRfcommUIHFrame::PutData(const TDesC8& aDes)
	{
	iData->Des().Append(aDes);
	}

const TDesC8& CRfcommUIHFrame::Data()
	/**
	  Build the data frame, then return it
	  
	**/
	{
	if(!iFramePrepared)
		PrepareDataFrame();
	return iReturnedFrame;
	}

void CRfcommUIHFrame::PrepareDataFrame()
	/**
	   Build the data frame and return it
	   
	   Fill in the address, ctrl and length fields, then add the FCS
	   Don't do any asserts on the basic parts since this can be used
	   by derived classes.

	   An additional complication here is that the length field may
	   be either 1 or 2 octets, depending on the length of the data
	   field.
	**/
	{
	//TUint length=static_cast<TUint16>((*iData).Length()-KMaxFrameHeaderLength);
	TUint length = DataLength();
	TPtr8 rwData=iData->Des();
	TInt offset=0;
	if(length<=127)
		{
		//	Note the length dependent offset for the Address and Control Octets
		offset=1;
		//	length will be just the 4th octet in the buffer,
		//	of which we will return everything from the 
		//	second octet onwards...
		rwData[3]=static_cast<TUint8>(length<<1 | 1);  // Set the E/A bit to 1
		}
	else
		{
		//	length will be in both the 3rd and 4th octet in the buffer.
		//	length is stored as follows;
		//
		//			Bit 1	Bit 2	Bit 3	Bit 4	Bit 5	Bit 6	Bit 7	Bit 8
		//	Octet 1  E/A	 L1		 L2		 L3		 L4		 L5		 L6		 L7
		//	Octet 2	 L8		 L9		 L10	 L11	 L12	 L13	 L14	 L15
		offset=0;
		rwData[2]=static_cast<TUint8>((length & 0x7F)<<1);
		rwData[3]=static_cast<TUint8>((length>>7) & 0xFF);
		}

	rwData[offset]=iAddr;
	rwData[1+offset]=KUIHCtrlField;
	rwData.Append(CalcFCS((*iData).Ptr()+offset, 2));  // Calc over two bytes	
	iReturnedFrame.Set((*iData).Right((*iData).Length()-offset));
	iFramePrepared=ETrue;
	}

void CRfcommUIHFrame::PutByteAt(TInt aOffset, TUint8 aByte)
	/**
	   Insert this byte in the data part of the packet at the
	   position specified

	   @param aOffset The index into the data part of the packet
	   (after the length field)
	**/
	{
	(iData->Des())[KMaxFrameHeaderLength+aOffset]=aByte;
	}






/*
  CRfcommDataFrame is used for UIH data frames designed to carry information
*/

CRfcommDataFrame* CRfcommDataFrame::NewL(TInt aInformationLength, CRfcommMuxer& aMux, CRfcommSAP* aSAP)
	{
	CRfcommDataFrame* self=new(ELeave) CRfcommDataFrame( aMux, aSAP);
	CleanupStack::PushL(self);
	self->ConstructDataBufferL(aInformationLength);
	CleanupStack::Pop();	//	self
	LOG2(_L("RFCOMM: Data Frame 0x%08x created, sap 0x%08x"), self, aSAP);
	return self;
	}

CRfcommDataFrame::CRfcommDataFrame(CRfcommMuxer& aMux, CRfcommSAP* aSAP)
	: CRfcommUIHFrame(aMux, aSAP)
	{
	}

CRfcommDataFrame::~CRfcommDataFrame()
	{
	}


void CRfcommDataFrame::ConstructDataBufferL(TInt aInformationLength)
	/**
		Allocate the data buffer to hold the information that will be placed in this frame.

		An allowance is made for the KMaxFrameHeaderLength, ie. addr, ctrl , 2*len = 4 octets
	**/
	{
	//	Reserve max possible space for addr, cont, length
	iData=HBufC8::NewL(aInformationLength+KMaxFrameOverhead);
	for(TUint8 i=0;i<KMaxFrameOverhead-1;++i)
		PutByte(0x00);	
	}

void CRfcommDataFrame::PrepareDataFrame()
	/**
	   Build the data frame and return it
	   
	   Fill in the address, ctrl and length fields, then add the FCS
	   Don't do any asserts on the basic parts since this can be used
	   by derived classes.

	   An additional complication here is that the length field may
	   be either 1 or 2 octets, depending on the length of the data
	   field.
	**/
	{
	//TUint length=static_cast<TUint16>((*iData).Length()-KMaxFrameHeaderLength);
	TUint length = DataLength();
	TPtr8 rwData=iData->Des();
	TInt offset=0;
	if(length<=127)
		{
		//	Note the length dependent offset for the Address and Control Octets
		offset=1;
		//	length will be just the 4th octet in the buffer,
		//	of which we will return everything from the 
		//	second octet onwards...
		rwData[3]=static_cast<TUint8>(length<<1 | 1);  // Set the E/A bit to 1
		}
	else
		{
		//	length will be in both the 3rd and 4th octet in the buffer.
		//	length is stored as follows;
		//
		//			Bit 1	Bit 2	Bit 3	Bit 4	Bit 5	Bit 6	Bit 7	Bit 8
		//	Octet 1  E/A	 L1		 L2		 L3		 L4		 L5		 L6		 L7
		//	Octet 2	 L8		 L9		 L10	 L11	 L12	 L13	 L14	 L15
		offset=0;
		rwData[2]=static_cast<TUint8>((length & 0x7F)<<1);
		rwData[3]=static_cast<TUint8>((length>>7) & 0xFF);
		}

	rwData[offset]=iAddr;
	rwData[1+offset]=KUIHCtrlField;
	rwData.Append(CalcFCS((*iData).Ptr()+offset, 2));  // Calc over two bytes	
	iReturnedFrame.Set((*iData).Right((*iData).Length()-offset));
	iFramePrepared=ETrue;
	}



	
/*
  CRfcommCreditDataFrame is used for UIH data frames designed to carry information and supply credits
*/

CRfcommCreditDataFrame* CRfcommCreditDataFrame::NewL(TInt aInformationLength, CRfcommMuxer& aMux, CRfcommSAP* aSAP)
	{
	CRfcommCreditDataFrame* self=new(ELeave) CRfcommCreditDataFrame( aMux, aSAP);
	CleanupStack::PushL(self);
	self->ConstructDataBufferL(aInformationLength);
	CleanupStack::Pop();	//	self
	LOG2(_L("RFCOMM: Data Frame 0x%08x created, sap 0x%08x"), self, aSAP);
	return self;
	}

CRfcommCreditDataFrame::CRfcommCreditDataFrame(CRfcommMuxer& aMux, CRfcommSAP* aSAP)
	: CRfcommDataFrame(aMux, aSAP)
	{
	}

CRfcommCreditDataFrame::~CRfcommCreditDataFrame()
	{
	}

TInt CRfcommCreditDataFrame::Type() const
	{
	return KCreditDataFrameType;
	}

void CRfcommCreditDataFrame::SetCredit(TUint8 aCredit)
	/**
	   Add the credit to an outgoing UIH frame
	**/
	{
	iCredit = aCredit;
	}

TUint8 CRfcommCreditDataFrame::Credit() const
	{
	return iCredit;
	}


TUint16 CRfcommCreditDataFrame::DataLength() const
	{
	TInt overhead = iFramePrepared?KMaxFrameCreditOverhead:KMaxCreditFrameHeaderLength;
	TUint16 length=static_cast<TUint16>((*iData).Length()-overhead);
	return length;
	}

void CRfcommCreditDataFrame::ConstructDataBufferL(TInt aInformationLength)
	/**
		Allocate the data buffer to hold the information that will be placed in this frame.

		An allowance is made for the KMaxFrameHeaderLength, ie. addr, ctrl, 2*len, credit = 5 octets
	**/
	{
	//	Reserve max possible space for addr, cont, length
	iData=HBufC8::NewL(aInformationLength+KMaxFrameCreditOverhead);
	for(TUint8 i=0;i<KMaxFrameCreditOverhead-1;++i)
		PutByte(0x00);	
	}

void CRfcommCreditDataFrame::PrepareDataFrame()
	/**
	   Build the data frame and return it
	   
	   Fill in the address, ctrl and length fields, then add the FCS
	   Don't do any asserts on the basic parts since this can be used
	   by derived classes.

	   An additional complication here is that the length field may
	   be either 1 or 2 octets, depending on the length of the data
	   field.
	**/
	{
	//TUint length=static_cast<TUint16>((*iData).Length()-KMaxCreditFrameHeaderLength);
	TUint length = DataLength();
	TPtr8 rwData=iData->Des();
	TInt offset=0;
	rwData[4]=iCredit;	// we're going backwards, credit is after length
	if(length<=127)
		{
		//	Note the length dependent offset for the Address and Control Octets
		offset=1;
		//	length will be just the 4th octet in the buffer,
		//	of which we will return everything from the 
		//	second octet onwards...
		rwData[3]=static_cast<TUint8>(length<<1 | 1);  // Set the E/A bit to 1
		}
	else
		{
		//	length will be in both the 3rd and 4th octet in the buffer.
		//	length is stored as follows;
		//
		//			Bit 1	Bit 2	Bit 3	Bit 4	Bit 5	Bit 6	Bit 7	Bit 8
		//	Octet 1  E/A	 L1		 L2		 L3		 L4		 L5		 L6		 L7
		//	Octet 2	 L8		 L9		 L10	 L11	 L12	 L13	 L14	 L15
		offset=0;
		rwData[2]=static_cast<TUint8>((length & 0x7F)<<1);
		rwData[3]=static_cast<TUint8>((length>>7) & 0xFF);
		}

	rwData[offset]=iAddr;
	rwData[1+offset]=KUIHCBFCCtrlField;
	rwData.Append(CalcFCS((*iData).Ptr()+offset, 2));  // Calc over two bytes	
	iReturnedFrame.Set((*iData).Right((*iData).Length()-offset));
	iFramePrepared=ETrue;
	}


	
/*
  CRfcommMuxCtrlFrame is used for frames carrying mux control
  commands on the mux control channel (DLCI 0)
*/

CRfcommMuxCtrlFrame* CRfcommMuxCtrlFrame::NewL(TUint8 aCommandValuesLength, CRfcommMuxer& aMux, CRfcommSAP* aSAP)
	{
	CRfcommMuxCtrlFrame* self=new(ELeave) CRfcommMuxCtrlFrame(aCommandValuesLength, aMux, aSAP);
	CleanupStack::PushL(self);
	self->ConstructDataBufferL(aCommandValuesLength);
	CleanupStack::Pop();	//	self
	LOG2(_L("RFCOMM: Mux Ctrl Frame 0x%08x created, sap 0x%08x"), self, aSAP);
		
	return self;
	}

void CRfcommMuxCtrlFrame::ConstructDataBufferL(TInt aCommandValuesLength)
	/**
	   Allocate the data buffer to hold the command that will be
	   placed in this frame.

		This function takes the number of value octets required by the
		command and adds on the appropriate number of octets to allow
		for the type and length fields
	**/
	{	
	CRfcommUIHFrame::ConstructDataBufferL(aCommandValuesLength+KShortMuxCommandHeaderLength);
	// Reserve space for command type and command length
	for(TUint8 i=0;i<KShortMuxCommandHeaderLength;++i)
		PutByte(0x00);	
	}


CRfcommMuxCtrlFrame::CRfcommMuxCtrlFrame(TUint8 aCommandValuesLength, CRfcommMuxer& aMux, CRfcommSAP* aSAP)
	: CRfcommUIHFrame(aMux, aSAP),
	  iCommandLength(aCommandValuesLength)
	{
	}

CRfcommMuxCtrlFrame::~CRfcommMuxCtrlFrame()
	{
	if(iTimerQueued)
		BTSocketTimer::Remove(iTimerEntry);
	}

TInt CRfcommMuxCtrlFrame::Type() const
	{
	return KMuxCtrlFrameType;
	}

void CRfcommMuxCtrlFrame::SetResponseNeeded(TBool aNeed)
	{
	iResponseNeeded=aNeed;
	}

TBool CRfcommMuxCtrlFrame::ResponseNeeded()
	{
	return iResponseNeeded;
	}

void CRfcommMuxCtrlFrame::QueResponseTimer()
	/*
	  Que a timer if necessary
	*/
	{
	if(iResponseNeeded && !iTimerQueued)
		{
		// Enque this timer
		TCallBack cb(TimerExpired, this);
		iTimerEntry.Set(cb);
		BTSocketTimer::Queue(KFrameTimerT2, iTimerEntry);
		iTimerQueued=ETrue;
		}
	}

void CRfcommMuxCtrlFrame::DequeTimer()
	{
	if(iTimerQueued)
		{
		BTSocketTimer::Remove(iTimerEntry);
		iTimerQueued=EFalse;
		}
	}

TBool CRfcommMuxCtrlFrame::Priority() const
	{
	return ETrue;
	}

TInt CRfcommMuxCtrlFrame::TimerExpired(TAny* aFrame)
	{
	CRfcommMuxCtrlFrame* frm=static_cast<CRfcommMuxCtrlFrame*>(aFrame);
	// Notify the mux that this frame has timed out
	LOG2(_L("RFCOMM: MuxCtrl frame timeout, frame %08x, command %d"),
				  frm, frm->CommandType());
	/*
	BLOG: Timeout on command:
	TUint8 command = frm->CommandType();
	switch(command)
		{
		case KTestType
			//KBlogTestCommand
			break;
		case KPNType
			//KBlogPN
			break;
		case KRPNType
			//KBlogRPN
			break;
		case KFConType
			//KBlogFcOn
			break;
		case KFCoffType
			//KBlogFcOff
			break;
		case KMSCType
			//KBlogMSC
			break;
		case KNSCType
			//KBlogNSC
			break;
		case KRLSType
			//KBlogRLS
			break;		  
		};

	//BLOG: MuxCtrl frame timeout, 
	*/
	
	frm->iTimerQueued=EFalse;
	frm->iMuxer.FrameResponseTimeout(frm);
	return EFalse;
	}


#pragma warning(disable : 4244) //  conversion from int to unsigned char

void CRfcommMuxCtrlFrame::SetCommandType(TUint8 aType, TBool aCommand)
	{
	iCommandType=(aType << 1) | (aCommand ? 1 : 0);
	}

TUint8 CRfcommMuxCtrlFrame::CommandType() const
	{
	return iCommandType >> 1;
	}

void CRfcommMuxCtrlFrame::PrepareDataFrame()
	/**
       Prepare the Mux ctrl frame to be transmitted.

	   We add the command type, command length before using the normal
	   UIH frame PrepareDataFrame to add the normal headers+FCS
	**/
	{
	__ASSERT_DEBUG((iAddr & 0xfc) == 0, Panic(ERfcommBadFrameAddrField));
	__ASSERT_DEBUG(iCtrl == KUIHCtrlField, Panic(ERfcommBadFrameCtrlField));
	TUint8 byte;
	byte= (iCommandType << 1)  | 0x01;  // add EA bit
	PutByteAt(0, byte);
	byte=(iCommandLength << 1) | 0x01;  // add EA bit
	PutByteAt(1, byte);
	CRfcommUIHFrame::PrepareDataFrame();
	}
	
#pragma warning(default : 4244)
