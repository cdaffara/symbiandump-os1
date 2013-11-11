// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef L2CAPPDU_H_
#define L2CAPPDU_H_

#include <e32std.h>
#include <es_sock.h>
#include <es_prot.h>

#include <es_mbuf.h>
#include <bttypes.h>

#include "L2types.h"
#include "l2util.h"

class HL2CapCommand;
class CServProviderBase;
class CL2CAPMux;
class CL2CapDataFlowController;

class HIFramePDU;
class HBFramePDU;
class HGFramePDU;
class HCFramePDU;
class HSFramePDU;
class HL2CapPDU;

class MOutgoingPDUHandler
	{
public:
	virtual TInt HandleOutgoingIFrame(HIFramePDU* aIFrame) = 0;
	virtual TInt HandleOutgoingBFrame(HBFramePDU* aBFrame) = 0;
	virtual TInt HandleOutgoingGFrame(HGFramePDU* aGFrame) = 0;
	virtual TInt HandleOutgoingCFrame(HCFramePDU* aCFrame) = 0;
	virtual TInt HandleOutgoingSFrame(HSFramePDU* aSFrame) = 0;
	};

// A PDU owner gets notified though this interface when a PDU send operation is complete.
// It's then up to the owner to delete the PDU or resend it, and to move it between
// various PDU lists - a PDU should not call iLink.Deque() nor destruct itself unless
// it has been orphaned.
// In practice CL2CAPMux owns C-, S- and B-Frames, while (E)RTM Data Controllers own
// I-Frames.
class MPduOwner
	{
public:
	virtual void HandlePduSendComplete(HL2CapPDU& aPdu) = 0;
	virtual void HandlePduSendError(HL2CapPDU& aPdu) = 0;
	};

enum TL2CapSAR
	{
	EUnsegmentedL2CapSDU	= 0,
	EStartOfL2CapSDU		= 1,
	EEndOfL2CapSDU			= 2,
	EContinuationOfL2CapSDU	= 3,
	};
	
enum TSupervisoryFunction
	{
	EReceiverReady		= 0,
	EReject				= 1,
	EReceiverNotReady	= 2,
	ESelectiveReject	= 3,
	};

class MDataPlaneElement
	{
public:
	virtual void DataElementSent(TUint16 aElementID = 0) = 0;
	virtual void DataElementFlushed(TUint16 aElementID = 0) = 0;

	virtual TBool IsPDUFlushed() const = 0;
	};
	
NONSHARABLE_CLASS(TDataPlaneElementHandle) : public TSockAddr
	{
public:
	struct SDataPlaneElementHandle
		{
		MDataPlaneElement* iDataElement;
		TUint16 iElementID;
		};
			
	TDataPlaneElementHandle(MDataPlaneElement* aDataElement, TUint16 aElementID = 0);

	MDataPlaneElement& DataPlaneElement() const;
	TUint16 ElementID() const;
	};


//
// Base class for all PDU types.
//
NONSHARABLE_CLASS(HL2CapPDU) : public MDataPlaneElement
	{
public:
	// Base or shared length definitions.	
	const static TUint8 KPDUHeaderLength 			= 4;
	const static TUint8 KControlFieldLength			= 2;		
	const static TUint8 KSDULengthFieldLength		= 2;
	const static TUint8 KMaxPDUHeaderLength			= KPDUHeaderLength + KControlFieldLength + KSDULengthFieldLength;		
	const static TUint8 KFCSFieldLength				= 2;
	const static TUint8 KSFrameLength				= 8;

	const static TUint8 KNumberOfFragmentsUnknown 	= 0;
		
	virtual ~HL2CapPDU();
	
	// Deliver Interface.
	virtual void DeliverOutgoingPDU(MOutgoingPDUHandler& aPDUHandler);

	// Method for reconstructing SDU data.
	virtual void AppendPayloadToBuffer(RMBufChain& aSDUData);
	 
	// Message Accessors.
	// Static
	static TUint16 PDUPayloadLength(const RMBufChain& aPDU);
	static TUint16 PDUCID(const RMBufChain& aPDU);
    static TInt CheckDecode(const RMBufChain& aPDU);

	// This method appends a fragment to the aPDU parameter and then
	// checks if the PDU is complete.
	static TBool AddFragment(RMBufChain& aPDU, RMBufChain& aPDUFragment);

	// Member
	inline TUint16 PDUPayloadLength() const;

	inline TUint16 PDUCID() const;
	void SetPDUCID(TUint16 aCID);

	// Calculate the PDU length, and place it in the PDU header.
	void WritePDUPayloadLength();  

	// For basic mode return the fragment overhead otherwise return the
	// L2CAP PDU overhead in bytes.
	static TInt GetPDUOrFragmentOverhead(TBool aBasicMode);
	
	// Return the optimal PDU size based on the MTU and controller buffer size.
	static TInt GetPDUOrFragmentSize(TInt aMTU, TInt aMaxMps, TInt aBufSize, TBool aBasicMode);
	
	inline TInt OptimalFragmentSize() const;
	
	// Inline accessor methods.
	inline RMBufChain& PDUBuffer();
	inline void SetPDUFlushed();
	inline TBool IsPDUFlushed() const;
	inline TDataPlaneElementHandle& ElementHandle();

	void PDUSendPending(TUint16 aTotalNumberOfFragments);
	void PDUSendComplete();
	
	inline void SetPduOwner(MPduOwner* aPduOwner);
	inline TBool HasOwner() const;
	inline TBool IsOwner(const MPduOwner* aPduOwner) const;
	inline void DeregisterPduOwner();

	inline TBool IsAwaitingHciCompletion() const;

	inline TBool IsQueuedForSend() const;
	inline void SetQueuedForSend(TBool aQueuedForSend);

	// MDataPlaneElement Interface
	void DataElementSent(TUint16 aElementID);
	void DataElementFlushed(TUint16 aElementID);
		
protected:
	// An optimal fragment size of 0 indicates that we do not expect fragmentation to take place
	HL2CapPDU(RMBufChain& aPDUData, TInt aOptimalFragmentSize);
	HL2CapPDU(TInt aOptimalFragmentSize = 0);

	void SetPDUPayloadLength(TUint16 aLength);

	virtual void DeliverOutgoingPDUDoubleDispatch(MOutgoingPDUHandler& aPDUHandler) = 0;
	virtual void SendComplete(); // for frame type-specific actions on send complete

	inline TUint16 CalcCRC();	
	static TUint16 CalcCRC(const RMBufChain& aPDU);	

public:
	// General link though which the PDU is "owned" by different entities during its lifetime.
	// An I-Frame PDU starts as a member of the PDU list in its SDU, then is pulled by a data
	// controller and depending on the data controller, proceeds through its unacknowledged I-Frame
	// lists (RTM and ERTM). B-Frames start linked to their SDUs but then are put on link muxer's
	// PDU list, because Basic mode doesn't do retransmissions, so the data controller doesn't care.
	// S-Frames are created inside data controllers and are put on the link muxer's list as well -
	// they are fire'n'forget too.
	// Note that the owning relationship is formalized with the MPduOwner interface.
	TDblQueLink iLink;
	// This is an additional link that can be used by a data controller to manage internal PDU
	// queues. (currently used in ERTM for Outgoing Q)
	TSglQueLink iDataControllerInternalQLink;
protected:
	// Base or shared field positions. 
	const static TUint8 KLengthByteOffset 			= 0;
	const static TUint8 KCIDByteOffset 				= 2;
	const static TUint8 KControlFieldByteOffset 	= 4;
	const static TUint8 KSDULengthFieldByteOffset 	= 6;

	// Control field bit masks.	
	const static TUint16 KCtrlFrameTypeMask 	= 0x0001;
	const static TUint16 KCtrlTxSeqMask			= 0x007e;
	const static TUint16 KCtrlReTxDisableMask	= 0x0080;
	const static TUint16 KCtrlFinalBitMask		= 0x0080;
	const static TUint16 KCtrlPollBitMask		= 0x0010;
	const static TUint16 KCtrlReqSeqMask		= 0x3f00;
	const static TUint16 KCtrlSARMask			= 0xc000;
	const static TUint16 KCtrlSupervisoryMask	= 0x000c;

	// Control field bit shifters.
	const static TUint16 KCtrlTxSeqShift		= 1;
	const static TUint16 KCtrlReTxDisableShift	= 7;
	const static TUint16 KCtrlReqSeqShift		= 8;
	const static TUint16 KCtrlSARShift			= 14;
				
	// All message information is held in this buffer.	
	RMBufChain iPDUData;

	TBool iIsFlushed;
	TBool iSendingError;

	// True when it's on the OutgoingQ in ERTM waiting for GetPdu to pull it.
	// Used for protecting the consistency of the OutgoingQ list - we mustn't try to
	// queue the same frame twice. Also useful for detecting misbehaving remotes, if
	// they try to request a retransmission of the same I-Frame more than once (per
	// one loss).
	TBool iQueuedForSend;

	// This indicates whether an initiated transmission has been completed by HCI or not,
	// so that a retransmission of the PDU can proceed or be stalled until this becomes false.
	// A PDU is supposed to start its life with this condition = False so that the initial
	// transmission can proceed, and then it should become True once the PDU is scheduled
	// for transmission. This is important for PDUs that can are acknowledgable or can be
	// rejected by the peer (read I-Frames). In case the acknowledgement/reject comes through
	// before the HCI completion event (which happens with the USB HCTL on Windows) we want
	// to wait until an I-Frame is transmitted until we can delete it/start re-transmitting it.
	// Note: an I-Frame may be queued for a retransmission before being completed by HCI
	// (because it may happen that we receive the (S)REJ before the HCI packet completion
	// event). That's why there are separate boolean flags instead of a single PDU state
	// enum.
	TBool iAwaitingHciCompletion;

	TDataPlaneElementHandle iElementHandle;
	MPduOwner* iPduOwner;	// Non-owned pointer.
	
	// Optimal Fragment size.
	// Only set for basic mode as we expect segmentation only in all other cases.
	TInt iOptimalFragmentSize;
	
public:	
	TUint16 iTotalNumberOfFragments;
	TUint16 iFragmentAcksReceived;
	};

//
// Class used to fragment and send any type of PDU.
// An instance of this class is currently owned by the Mux
//
NONSHARABLE_CLASS(HFragmentedPDUSender)
	{
public:
	enum TFragmentSenderStatus
		{
		EFragmentOK				= 0,
		EFragmentationComplete 	= 1,
		EFlowControlledOff 		= 2,
		};
		
	HFragmentedPDUSender(CL2CAPMux& aMuxer);
	~HFragmentedPDUSender();
		
	TInt FragmentPDU(HL2CapPDU& aPDU);
	TFragmentSenderStatus WriteNextFragment(CServProviderBase& aSender, TInt aACLMTU);
	inline TBool IsPDUBeingSent() const; 

	void Reset();
	void CheckForFlushed();
	void PDUSenderFailed();

private:
	HL2CapPDU* iPDU;			// Non-owned
	HBufC8* iPDUBuffer;			// Buffer for the outgoing PDU.
	CL2CAPMux& iMuxer;			// Reference to the sending object.
	TInt iCurrentWriteIndex;	// Current position in the PDU buffer.
	TUint16 iCurrentFragmentID;	// Current [next] fragment ID to be sent (first fragment has ID = 0)
	TInt iPDULength;			// Length of the PDU being sent.
	TInt iPDUFragmentSize;		// Optimal fragment size.
	};

//
// Basic frame PDU class.
//
NONSHARABLE_CLASS(HBFramePDU) : public HL2CapPDU
	{
public:
	~HBFramePDU();

	static HBFramePDU* New(RMBufChain& aPayloadData, TInt aOptimalFragmentSize);

	// Deliver Interface.
	void DeliverOutgoingPDUDoubleDispatch(MOutgoingPDUHandler& aPDUHandler);

	static void RemoveHeaderBytes(RMBufChain& aPDU);
	void AppendPayloadToBuffer(RMBufChain& aSDUData);

	void PDUFragmentSent(TUint16 aFragmentIx);
	
	static TInt CheckPayloadDecode(const RMBufChain& aPDU);
		
private:
	HBFramePDU(RMBufChain& aPDUData, TInt aOptimalFragmentSize);

	virtual void SendComplete();
	};

	
//
// Information frame PDU class.
//
NONSHARABLE_CLASS(HIFramePDU) : public HL2CapPDU
	{
public:
	~HIFramePDU();
	
	static HIFramePDU* New(RMBufChain& aPayloadData, TL2CapSAR aPduSAR);

	// Deliver Interface.
	void DeliverOutgoingPDUDoubleDispatch(MOutgoingPDUHandler& aPDUHandler);

	// Message Accessors.
	// Static
	static TBool IFrameIdentifier(const RMBufChain& aPDU);
	static TBool CheckFCS(const RMBufChain& aPDU);
	static TInt CheckPayloadDecode(const RMBufChain& aPDU);
	static TInt CheckLengthWithinLimits(const RMBufChain& aPDU, TUint16 aMps);
	static TInt CheckStartSduLength(const RMBufChain& aPDU, TUint16 aMtu);
	static TL2CapSAR SAR(const RMBufChain& aPDU);
	static void RemoveHeaderAndFCSBytes(RMBufChain& aPDU);
	static TBool IsStartOfSDU(const RMBufChain& aPDU);
	static TUint16 SDUSize(const RMBufChain& aPDU);
	static TUint8 ReqSeqNumber(const RMBufChain& aPDU);
	static TUint8 TxSeqNumber(const RMBufChain& aPDU);
	static TBool FinalBit(const RMBufChain& aPDU);
	static TBool RetransmitDisable(const RMBufChain& aPDU);
	
	// Member
	void SetFinalBit(TBool aFinalBit);
	inline TBool FinalBit() const;
	
	inline TUint8 TxSeqNumber() const;
	void SetTxSeqNumber(TUint8 aTxSeqNum);
	
	void SetRetransmitDisable(TBool aReTxDisable);

	inline TUint8 ReqSeqNumber() const;
	void SetReqSeqNumber(TUint8 aReqSeqNum);
	
	inline TL2CapSAR SAR() const;
	void SetSAR(TL2CapSAR aSARValue);
	
	inline TUint16 SDUSize() const;
	void SetSDUSize(TUint16 aSDUSize);
	
	void CalculateAndSetFCS();
	TBool CheckFCS();
	
	void AppendPayloadToBuffer(RMBufChain& aSDUData);	

	inline TBool IsStartOfSDU() const;
	
	// Re-transmission Methods
	inline TBool CanTransmit(TUint8 aMaxNumberOfTransmissions) const;
	inline TUint8 TransmissionCount() const;
	
	inline TBool Acked() const { return iAcked; }
	inline void SetAcked(TBool aAcked) { iAcked = aAcked; }

private:
	const static TUint16 KIFrameControlDefault	= 0x0000;			

	HIFramePDU(RMBufChain& aPDUData);

	void SetIFrameControlDefault();
	virtual void SendComplete();

	TUint8 iTransmissionCount;
	TBool iAcked;					// has been acknowledged by the remote
	};

//
// Group frame PDU class.
//
NONSHARABLE_CLASS(HGFramePDU) : public HL2CapPDU
	{
public:
	HGFramePDU(RMBufChain& aPDUData);
	~HGFramePDU();
	
	static HGFramePDU* New(RMBufChain& aPayloadData);

	// Deliver Interface.
	void DeliverOutgoingPDUDoubleDispatch(MOutgoingPDUHandler& aPDUHandler);

	TUint16 PSM() const;
	void SetPSM(TUint16 aPSM);
	
	void AppendPayloadToBuffer(RMBufChain& aSDUData);
	
private:
	const static TUint8 KGFramePSMLength 			= 2;
	};


//
// Control frame PDU class.
//
NONSHARABLE_CLASS(HCFramePDU) : public HL2CapPDU
	{
public:
	const static TUint8 KCFrameHeaderLength			= 4;
	const static TUint8 KCFrameCodeSize				= 1;
	const static TUint8 KCFrameIdentifierSize		= 1;
	const static TUint8 KCFrameLengthSize     		= 2;
		
	static HCFramePDU* New(TInt aOptimalFragmentSize);

	HCFramePDU(RMBufChain& aPDUData, TInt aOptimalFragmentSize);
	
	~HCFramePDU();

	// Deliver Interface.
	void DeliverOutgoingPDUDoubleDispatch(MOutgoingPDUHandler& aPDUHandler);

	HL2CapCommand* FirstCommand();
	HL2CapCommand* NextCommand();

	inline TBool HasCommands() const;
	
	TInt AddCommand(HL2CapCommand& aCommand, CL2CAPMux& aMuxer);
	TInt CheckDecode();
	
private:
	TInt CheckPayloadDecode();
	TInt CreateCommands();
	
	//Dbl link list of commands included within this pdu
	TDblQue<HL2CapCommand> iCommands;
	TDblQueIter<HL2CapCommand> iCommandIter;
	};


//
// Supervisory frame PDU class.
//
NONSHARABLE_CLASS(HSFramePDU) : public HL2CapPDU
	{
public:
	HSFramePDU(RMBufChain& aPDUData);
	~HSFramePDU();
	
	static HSFramePDU* New(TSupervisoryFunction aFunction);
	static HSFramePDU* NewL(TSupervisoryFunction aFunction);

	// Deliver Interface.
	void DeliverOutgoingPDUDoubleDispatch(MOutgoingPDUHandler& aPDUHandler);

	// Message Accessors.
	// Static
	static TInt CheckLengthField(const RMBufChain& aPDU);
	static TBool CheckFCS(RMBufChain& aPDU);
	static TInt CheckPayloadDecode(RMBufChain& aPDU);
	static TSupervisoryFunction SupervisoryFunction(const RMBufChain& aPDU);
	static TUint8 ReqSeqNumber(const RMBufChain& aPDU);
	static TBool RetransmitDisable(const RMBufChain& aPDU);
	static TBool FinalBit(const RMBufChain& aPDU);
	static TBool PollBit(const RMBufChain& aPDU);
	
	// Members
	void SetFinalBit(TBool aFinalBit);
	inline TBool FinalBit() const;

	void SetPollBit(TBool aPollBit);
	inline TBool PollBit() const;

	void SetSupervisoryFunction(TSupervisoryFunction aSupervisoryFunction);
	inline TSupervisoryFunction SupervisoryFunction() const;

	void SetRetransmitDisable(TBool aReTxDisable);
	void SetReqSeqNumber(TUint8 aReqSeqNum);
	inline TUint8 ReqSeqNumber() const;
	
	void CalculateAndSetFCS();
	TInt CheckFCS();

	void SetSFrameControlDefault();
	
	void PDUFragmentSent(TUint16 aFragmentIx);
	
private:
	const static TUint16 KSFrameControlDefault	= 0x0001;			
	};


//
// PDU Fragment sender inline methods.
//
inline TBool HFragmentedPDUSender::IsPDUBeingSent() const
	{
	return (iPDU != NULL);
	}


//
// Base class inline methods.
//
inline RMBufChain& HL2CapPDU::PDUBuffer()
	{
	return iPDUData;
	}

inline void HL2CapPDU::SetPDUFlushed()
	{
	iIsFlushed = ETrue;
	}
	
inline TBool HL2CapPDU::IsPDUFlushed() const
	{
	return iIsFlushed;
	}

inline TDataPlaneElementHandle& HL2CapPDU::ElementHandle()
	{
	return iElementHandle;
	}

inline TUint16 HL2CapPDU::PDUCID() const
	{
	return PDUCID(iPDUData);
	}

inline TUint16 HL2CapPDU::PDUPayloadLength() const
	{
	return PDUPayloadLength(iPDUData);
	}

inline TUint16 HL2CapPDU::CalcCRC()	
	{
	return CalcCRC(iPDUData);
	}
	
inline TInt HL2CapPDU::OptimalFragmentSize() const
	{
	return iOptimalFragmentSize;
	}

inline void HL2CapPDU::SetPduOwner(MPduOwner* aPduOwner)
	{
	__ASSERT_ALWAYS(aPduOwner != NULL, Panic(EL2CAPNullOwnerSupplied));
	__ASSERT_ALWAYS(iPduOwner == NULL || iPduOwner == aPduOwner, Panic(EL2CAPTryingToStealOwnedPdu));
	iPduOwner = aPduOwner;
	}

inline TBool HL2CapPDU::HasOwner() const
	{
	return iPduOwner != NULL;
	}

inline TBool HL2CapPDU::IsOwner(const MPduOwner* aPduOwner) const
	{
	__ASSERT_ALWAYS(aPduOwner != NULL, Panic(EL2CAPNullOwnerSupplied));
	return iPduOwner == aPduOwner;
	}

inline void HL2CapPDU::DeregisterPduOwner()
	{
	iPduOwner = NULL;
	}

inline TBool HL2CapPDU::IsAwaitingHciCompletion() const
	{
	return iAwaitingHciCompletion;
	}

inline TBool HL2CapPDU::IsQueuedForSend() const
	{
	return iQueuedForSend;
	}

inline void HL2CapPDU::SetQueuedForSend(TBool aQueuedForSend)
	{
	iQueuedForSend = aQueuedForSend;
	}

//
// Information frame inline methods.
//
inline TBool HIFramePDU::CanTransmit(TUint8 aMaxNumberOfTransmissions) const
	{
	return (iTransmissionCount < aMaxNumberOfTransmissions);
	}

inline TUint8 HIFramePDU::TransmissionCount() const
	{
	return iTransmissionCount;
	}


inline TL2CapSAR HIFramePDU::SAR() const
	{
	return SAR(iPDUData);
	}
	
inline TBool HIFramePDU::IsStartOfSDU() const
	{
	return IsStartOfSDU(iPDUData);
	}

inline TUint16 HIFramePDU::SDUSize() const
	{
	return SDUSize(iPDUData);
	}

inline TUint8 HIFramePDU::ReqSeqNumber() const
	{
	return ReqSeqNumber(iPDUData);
	}

inline TUint8 HIFramePDU::TxSeqNumber() const
	{
	return TxSeqNumber(iPDUData);
	}

inline TBool HIFramePDU::FinalBit() const
	{
	return FinalBit(iPDUData);
	}

//
// Control frame inline methods.
//
inline TBool HCFramePDU::HasCommands() const
	{
	return !iCommands.IsEmpty();
	}

//
// Supervisory frame inline methods.
//
inline TBool HSFramePDU::FinalBit() const
	{
	return FinalBit(iPDUData); 
	}

inline TBool HSFramePDU::PollBit() const
	{
	return PollBit(iPDUData);
	}

inline TSupervisoryFunction HSFramePDU::SupervisoryFunction() const
	{
	return SupervisoryFunction(iPDUData);
	}

inline TUint8 HSFramePDU::ReqSeqNumber() const
	{
	return ReqSeqNumber(iPDUData);
	}

#endif
