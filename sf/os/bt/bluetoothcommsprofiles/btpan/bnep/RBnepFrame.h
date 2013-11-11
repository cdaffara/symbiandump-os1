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

/**
 @file
 @internalComponent
*/

#ifndef __RBNEPFRAME_H
#define __RBNEPFRAME_H 

#include <e32def.h>
#include <nifmbuf.h>
#include "RBnepControl.h"
#include "MLinkManager.h"

class TBTDevAddr;
class RMBufChain;

/**
* KBnep is a flag that gets set by the BNEP packet driver to indicate
* to the PAN NAP IP hooks that the packet has come from a remote Bluetooth
* PAN device.
*/
const TUint KBnep = 0x00000100;

/**
* KBnepForwardingAllowed is also set by the BNEP packet driver to
* indicate to the PAN NAP IP hooks that the packet has come from a remote
* Bluetooth PAN device that has been granted access to the public network.
*/
const TUint KBnepForwardingAllowed = 0x00000200;

const TInt KGeneralEthernetHeaderLength 				= 13;
const TInt KCompressedEthernetSourceOnlyHeaderLength 	= 7;
const TInt KCompressedEthernetDestOnlyHeaderLength 		= 7;
const TInt KCompressedEthernetHeaderLength 				= 1;

const TInt KNetworkProtocolTypeFieldLength 				= 2;
const TInt KIEEE802_1QTagFieldLength 					= 2;


/**
   A BNEP frame.
   Built from one of an ethernet frame, a BNEP frame or a sequence of commands,
   this object encapsulates the data held by a BNEP frame.
*/
NONSHARABLE_CLASS(RBnepFrame) 
    {
public:
    enum TPacketContents
        {
        EDataFrame 			= 0x01,
        EControl 			= 0x02,
        EHas1Q 				= 0x04,
        EExtensionHeaders	= 0x08,
        };

    enum TDestAddrType
        {
        ELocal,
        EMulticast,
        ENormal
        };

    RBnepFrame();
    void BuildBnepFrameL(RMBufChain& aChain, 
                         const TBTDevAddr& aLocalAddr, 
                         const TBTDevAddr& aRemoteAddr,
						 TBool aFilterEthernetPayload);

    void BuildEthernetFrameL (RMBufPacket& aChain);
    const TBTDevAddr& DestAddr () const;
    TDestAddrType DestinationType () const;
    TBool Includes (TPacketContents aContent) const;
    void InsertControl (RBnepControl* aCommand);
    TBnepPacketType ParseBnepFrameL (RMBufChain& aInboundChain, const TBTDevAddr& aLocalAddr, const TBTDevAddr& aRemoteAddr);
    void ParseEthernetFrameL (RMBufChain& aFrame);
    void Reset ();
    const TBTDevAddr& SrcAddr () const;
    TBool IsEmpty ();
    void ExecuteControl (MLinkManager& aController);
    TUint16 Protocol () const;
	TInt PayloadLength() const;
	void SetUplinkAccessAllowedForBnepLink(TBool aUplinkAccessAllowedForBnepLink);
	
private:
    void BuildBnepControlFrameL (RMBufChain& aChain);
    void BuildBnepDataFrameL(RMBufChain& aChain, const TBTDevAddr& aLocalAddr, const TBTDevAddr& aRemoteAddr, TBool aFilterEthernetPayload);
    void SetContents (TPacketContents aContent);
    void TrimBTAddrL (RMBufChain& aChain, TBTDevAddr& aAddr);
    void TrimUint16L (RMBufChain& aChain, TUint16& aInt);
    void ParseL (RMBufChain& aInboundChain, TBnepPacketType aType, TBool aExt);
    TInt ParseExtensionL (RMBufChain& aInboundChain);
    void ExtractControlL (RMBufChain& aInboundChain, TInt aLength);

private: 
    // Data Members for Has Relationships
    TSglQue< RBnepControl  > iCommandQueue;
    TBTDevAddr iDestAddr;
    TBTDevAddr iSrcAddr;
    TUint16 iProtocol;
    TUint8 iPacketContents;
	TBool iUplinkAccessAllowedForBnepLink;
	
	// This member holds the contents of the frame excluding the BT
	// addresses and the protocol and any control headers.
    RMBufChain iFramePayload;
    };




// Class RBnepFrame 

inline void RBnepFrame::SetContents (TPacketContents aContent)
    {
    iPacketContents |= static_cast<TUint8>(aContent); 
    }

inline TBool RBnepFrame::IsEmpty ()
    {
    return iCommandQueue.IsEmpty();
    }

inline TUint16 RBnepFrame::Protocol () const
    {
    return iProtocol;
    }

#endif
