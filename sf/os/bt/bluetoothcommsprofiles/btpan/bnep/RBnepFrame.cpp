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

#include <bluetooth/logger.h>
#include <e32std.h>
#include <bttypes.h>
#include <es_mbuf.h>

#include "RBnepFrame.h"
#include "bneputils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

const TUint8  KEBit = 0x80;
const TUint16 KQTag = 0x8100;

// 'minTypeValue' from section 4.2.7.1 of the 802.3-2002 specification
// Subtract one, as the constant in the spec is for the first non-length value.
const TUint16 K802_3MaxLength = 1536 - 1;

void doResetControl(TAny* aControl)
    {
    /**
       Reset and destroy an RBnepControl object.
    */
    LOG_STATIC_FUNC
    RBnepControl* control = static_cast<RBnepControl*>(aControl);
    control->Free();
    delete control;
    }

RBnepFrame::RBnepFrame()
        : iCommandQueue(_FOFF(RBnepControl, iLink))
    {
    /**
       Construct and initialise (reset) a BNEP frame.
       @internalComponent
    */
    LOG_FUNC
    Reset(); 
    }

void RBnepFrame::BuildBnepControlFrameL (RMBufChain& aChain)
    {
    /**
       Build a BNEP control frame from the encapsulated data.
       @param aChain The target to build the frame into.
       @internalComponent
    */
    LOG_FUNC
    TUint8 frameType = EBnepControl;
    // There have to be items in the command queue 
    if(iCommandQueue.IsEmpty())
        {
        User::Leave(KErrUnderflow);
        }
    else
        {
        // If the last item is also the first one, there must be
        // one and only one item in the queue.
        // If there is more than one, then the ebit must be set.
        // Note: there is no checking done here that the queue items are
        // small enough to fit into an extension header.
        TUint8 ebit = static_cast<TUint8>(((iCommandQueue.IsLast(iCommandQueue.First())) ? 0 : KEBit));
        frameType |= ebit;

        RBnepControl* buffer = iCommandQueue.First();
        buffer->CopyL(aChain);
        iCommandQueue.Remove(*buffer);
        buffer->Free();
        delete buffer;

        aChain.PrependL(sizeof(TUint8));
        TPckgBuf<TUint8> pkgFrameType(frameType);
        aChain.CopyIn(pkgFrameType,RBnepControl::KControlTypeOffset);
        if(ebit) // We have at least one extension
            {
            TSglQueIter<RBnepControl> iter(iCommandQueue);
            iter.SetToFirst();
            RBnepControl* element;
            while ((element = iter++), element)
                {
                TPckgBuf<TInt8> aLength(static_cast<TInt8>(element->Length()));
                element->PrependL(sizeof(TInt8));
                element->CopyIn(aLength);

                element->PrependL(sizeof(TUint8));
                if(iCommandQueue.IsLast(element))
                    {
                    TPckgBuf<TUint8> extFrameType(static_cast<TUint8>(EBnepExtensionControl));
                    element->CopyIn(extFrameType);
                    }
                else
                    {
                    // The extension bit must be set.
                    TPckgBuf<TUint8> extExtFrameType(static_cast<TUint8>(EBnepExtensionControl | KEBit));
                    element->CopyIn(extExtFrameType);
                    }

                aChain.Append(*element);
                }
            }
        LOG1(_L8("Final Chain length = %d"),aChain.Length());
        Reset();
        }
    }

void RBnepFrame::BuildBnepDataFrameL(RMBufChain& aChain, 
                                     const TBTDevAddr& aLocalAddr, 
                                     const TBTDevAddr& aRemoteAddr,
                                     TBool aFilterEthernetPayload)
    {
    /**
       Build a BNEP data frame from the encapsulated data.
       The method deals with compressing off any unnecessary address data based on
       the parameters supplied.
       @param aChain The target to build the frame into.
       @param aLocalAddr The address of the bridge (local side of BNEP link).
       @param aRemoteAddr The address of the remote end of the BNEP link.
    */
    LOG_FUNC
    // If the command queue has items in it, we need to build extension headers.
    TUint8 frameType;

    TBool sendDest = (aRemoteAddr != iDestAddr);
    TBool sendSrc  = (aLocalAddr  != iSrcAddr);
    TInt headerLength;
    if (sendDest && sendSrc)
        {
        frameType = EBnepGeneralEthernet;
        headerLength = KGeneralEthernetHeaderLength;
        LOG(_L8("frameType = EBnepGeneralEthernet;"));
        LOG1(_L8("headerLength = %d"),headerLength);
        }
    else 
        {
        if (sendSrc)
            {
            frameType = EBnepCompressedEthernetSourceOnly;
            headerLength = KCompressedEthernetSourceOnlyHeaderLength;
            LOG(_L8("frameType = EBnepCompressedEthernetSourceOnly;"));
            LOG1(_L8("headerLength = %d"),headerLength);
            }
        else 
            {
            if (sendDest)
                {
                frameType = EBnepCompressedEthernetDestOnly;
                headerLength = KCompressedEthernetDestOnlyHeaderLength;
                LOG(_L8("frameType = EBnepCompressedEthernetDestOnly;"));
                LOG1(_L8("headerLength = %d"),headerLength);
                }
            else
                {
                frameType = EBnepCompressedEthernet;
                headerLength = KCompressedEthernetHeaderLength;
                LOG(_L8("frameType = EBnepCompressedEthernet"));
                LOG1(_L8("headerLength = %d"),headerLength);
                }
            }
        }
    aChain.AllocL(headerLength);

	if(Includes(EExtensionHeaders))
		{
		frameType |= KEBit;
		}
		
    LOG1(_L8("Setting type to %02x"), frameType);
    TInt offset = 0;
    TPckgBuf<TUint8> pkgFrameType(frameType);
    aChain.CopyIn(pkgFrameType, offset++);
    if (sendDest)
        {
        LOG_BT(_L8("Inserting destination address %S"), iDestAddr);
        aChain.CopyIn(iDestAddr.Des(), offset);
        offset += KBTDevAddrSize;
        }
    if (sendSrc)
        {
        LOG_BT(_L8("Inserting source address %S"), iSrcAddr);
        aChain.CopyIn(iSrcAddr.Des(), offset);
        offset += KBTDevAddrSize;
        }

	// Set-up the protocol ID to use for this packet.
	TUint16 protocol = (aFilterEthernetPayload ? 0x0000 : iProtocol);
	
    aChain.AppendL(KNetworkProtocolTypeFieldLength);
    TBuf8<KNetworkProtocolTypeFieldLength> protocolBuf;
    protocolBuf.SetMax();

    if (Includes(EHas1Q))
        {
        BigEndian::Put16(&protocolBuf[0], static_cast<TUint16>(KQTag));
        }     
    else
        {
        BigEndian::Put16(&protocolBuf[0], static_cast<TUint16>(protocol));
        }
    aChain.CopyIn(protocolBuf, offset);
    offset += KNetworkProtocolTypeFieldLength;

    // Cannot just append the ethernet payload mbufs, because if this is a 
    // multicast frame, this will corrupt it for others
    RMBufChain finalPacket;
    CleanupStack::PushL(finalPacket); // This will call Free() as its cleanup
    iFramePayload.CopyL(finalPacket);

	// Filter out the ethernet payload if required.
	if(aFilterEthernetPayload)
		{
		TInt payloadOffset = 0;
		// Find the start of the ethernet payload.
	    TBuf8<KExtensionHeaderHeaderBytes> buf;
		buf.SetMax();

		TUint8 ebit = ETrue;
		while(ebit)
			{
			// The extension must be long enough for the header.
	        if((payloadOffset + KExtensionHeaderHeaderBytes) > finalPacket.Length())
	            {	
	            User::Leave(KErrCorrupt);
	            }

			// Move the offset to the next extension header.
	    	finalPacket.CopyOut(buf, payloadOffset);
			ebit = buf[0] & KEBit;
			payloadOffset += (buf[1] + KExtensionHeaderHeaderBytes);
			}

		// If the packet includes IEEE 802.1Q fields then these should remain.
		// The network protocol field should be set to zero. 
	    if(Includes(EHas1Q))
	        {
			if((payloadOffset + KIEEE802_1QTagFieldLength + KNetworkProtocolTypeFieldLength) > finalPacket.Length())
				{
				User::Leave(KErrCorrupt);
				}

			protocolBuf[0] = 0;
			protocolBuf[1] = 0;
        	finalPacket.CopyIn(protocolBuf, payloadOffset + KIEEE802_1QTagFieldLength);
			payloadOffset += (KIEEE802_1QTagFieldLength + KNetworkProtocolTypeFieldLength);
	        }
			
		// The extension headers are finished.  Remove any remaining data.
		finalPacket.TrimEnd(payloadOffset);
		}
			
	// Append will take ownership of finalPacket
	CleanupStack::Pop(); // finalPacket
	// Add any payload onto the chain.
    aChain.Append(finalPacket);
    
    LOG1(_L8("RBnepFrame::BuildBnepDataFrameL final Chain length = %d"),aChain.Length());
    }

void RBnepFrame::BuildBnepFrameL(RMBufChain& aChain, const TBTDevAddr& aLocalAddr, const TBTDevAddr& aRemoteAddr, TBool aFilterEthernetPayload)
    {
    /**
       Build a BNEP frame from encapsulated data.
       Processes the data carried by this frame to build a frame which can
       be sent to a remote PAN device.  This involves compressing off any
       addresses which are not required for this link and inserting any
       extension headers.
       @param aChain The target MBufChain to build into.
       @param aLocalAddr The address of the local PAN node.
       @param aRemoteAddr The address of the remote PAN node.
       @internalComponent
    */
    LOG_FUNC
    if (Includes(EControl))
        {
        BuildBnepControlFrameL(aChain);
        }
    else
        {
        BuildBnepDataFrameL(aChain, aLocalAddr, aRemoteAddr, aFilterEthernetPayload);
        }
    }

void RBnepFrame::BuildEthernetFrameL(RMBufPacket& aChain)
    {
    /**
       Builds an ethernet frame from the BNEP frame.
       This function ignores away any control data present and simply inserts all
       addresses, protocol and 802.1Q data, as well as the data payload.
       
       This function always builds an Ethernet II format frame.  This should be
       accepted by the ethernet NIF, even if it is configured to send in LLC/SNAP
       format.  This behaviour is unlikely to change as it would be valid for a
       remote wired station to send a mixture of packet formats.  As the packet
       driver has no way of knowing which format has been configured, our options
       here are limited.
       
       @param aChain The MBufPacket to build the packet into.
       @internalComponent
    */
    LOG_FUNC
    // WARNING! Potential for a memory leak follows. 
    // Remember to free this RMBufChain before you exit.
    RMBufChain tempFrame;
	CleanupStack::PushL(tempFrame);
	
    LOG(_L8("Attempting to copy payload"));
	// Check there is a valid payload 
    if(iFramePayload.IsEmpty())
        {
        LOG(_L8("No payload"));
        User::Leave(KErrUnderflow);
        }
	iFramePayload.CopyL(tempFrame);

    // Any extensions need to be removed.
    if(Includes(EExtensionHeaders))
        {
	    TBuf8<KExtensionHeaderHeaderBytes> buf;
		buf.SetMax();

		TUint8 ebit = ETrue;
		while(ebit)
			{
			// The extension must be long enough for the header.
	        if((KExtensionHeaderHeaderBytes) > tempFrame.Length())
	            {	
	            User::Leave(KErrCorrupt);
	            }

			// Move the offset to the next extension header.
	    	tempFrame.CopyOut(buf, 0);
			ebit = buf[0] & KEBit;
			tempFrame.TrimStart(buf[1] + KExtensionHeaderHeaderBytes);
			}
        }

	// Check there is some data to send.
	if(Includes(EHas1Q) && (tempFrame.Length() <= (KIEEE802_1QTagFieldLength + KNetworkProtocolTypeFieldLength)) ||
	   tempFrame.IsEmpty())
		{
		LOG(_L8("No data to send pass up."));
		User::Leave(KErrUnderflow);
		}
		
	// Add protocol.
	TUint16 protocol = (Includes(EHas1Q) ? KQTag : iProtocol);
	
    TBuf8<KNetworkProtocolTypeFieldLength> protocolBuf;
    protocolBuf.SetMax();                                                                                                      
	tempFrame.PrependL(KNetworkProtocolTypeFieldLength);
    BigEndian::Put16(&protocolBuf[0], static_cast<TUint16>(protocol));
    tempFrame.CopyIn(protocolBuf);

	// Add addresses
	tempFrame.PrependL(KBTDevAddrSize);
    LOG_BT(_L8("Inserting source address %S"),iSrcAddr);
    tempFrame.CopyIn(iSrcAddr.Des());

	tempFrame.PrependL(KBTDevAddrSize);
    LOG_BT(_L8("Inserting destination address %S"),iDestAddr);
    tempFrame.CopyIn(iDestAddr.Des());

	// This should be removed when defect DEF066321 is fixed.
	TInt alignLength = Min(tempFrame.Length(), KMBufSmallSize);
	if(tempFrame.Align(alignLength) != alignLength)
		{
		User::Leave(KErrCorrupt);
		}

	RMBufQ tempQueue;
	tempQueue.Assign(tempFrame);
	CleanupStack::Pop();		// Remove tempFrame from the cleanup stack
	CleanupStack::PushL(tempQueue);
	
	aChain.CreateL(tempQueue); 	
	CleanupStack::PopAndDestroy();		// Remove tempQueue from the cleanup stack
							// As RMBufQs provide support for normal cleanup stack 
							// usage, this will cause the Q to be Free()ed.

	// Create the flags field for the packet info header.
	// Mark the packet as being from BNEP
	TUint flags = KBnep;

	// Check if this packet can be forward to DHCP / NAPT
	if(iUplinkAccessAllowedForBnepLink)
		{
		flags |= KBnepForwardingAllowed;
		}

	aChain.Info()->iFlags = flags;
    aChain.Pack();
    }

const TBTDevAddr& RBnepFrame::DestAddr () const
    {
    LOG_FUNC
    return iDestAddr; 
    }

RBnepFrame::TDestAddrType RBnepFrame::DestinationType () const
    {
    /**
       Return the type of destination for the frame (local, multicast address,
       or a specified machine).
       @internalComponent
    */
    LOG_FUNC
    if (TBTDevAddr(0) == iDestAddr) 
        {
        return ELocal;
        }
    if (BnepUtils::IsMulticast(iDestAddr)) 
        {
        return EMulticast;
        }
    return ENormal; 
    }

TBool RBnepFrame::Includes (TPacketContents aContent) const
    {
    /**
       Accessor function for contents of packet.
       @param aContent The type of content to check for.
       @internalComponent
    */
    LOG_FUNC
    return (iPacketContents & static_cast<TUint8>(aContent));
    }

void RBnepFrame::InsertControl (RBnepControl* aCommand)
    {
    /**
       Inserts and takes ownership of a BNEP command.
       @param aCommand The command to insert into the frame.  The method takes
       ownership of the object, which should be heap based.
       @internalComponent
    */
    LOG_FUNC
    //iCommandQueue.AddLast(*aCommand);
    iCommandQueue.AddLast(*aCommand);
    SetContents(EControl); 
    }

TBnepPacketType RBnepFrame::ParseBnepFrameL (RMBufChain& aInboundChain, const TBTDevAddr& aLocalAddr, const TBTDevAddr& aRemoteAddr)
    {
    /**
       Parse a supplied BNEP frame.
       Parses the provided BNEP frame, separating data and control messages and
       resolving any source and destination addresses.
       @param aInboundChain The frame received from the remote device
       @param aLocalAddr The address of the bridge (the local PAN node and possible
       destination of the packet)
       @param aRemoteAddr The address of the remote PAN node (the possible source
       of the packet)
       @internalComponent
    */
    LOG_FUNC
    if(aInboundChain.Length() < 1)
        {
        User::Leave(KErrUnderflow);
        }
    Reset();
    // Extract the packet type and the Extension bit.
    TBnepPacketType type = static_cast<TBnepPacketType>(aInboundChain.First()->Ptr()[0]);
    aInboundChain.TrimStart(sizeof(TUint8));
    // Test the extension bit
    TBool ebit = type & KEBit;
    // Discard the extension bit
    type = static_cast<TBnepPacketType>(type & (~KEBit));
    LOG1(_L8("Type 0x%02x"),type);
    LOG1(_L8("Ebit 0x%02x"),ebit);
    // Based on the packet type, determine whether to extract addresses
    TBool dest = EFalse;
    TBool src = EFalse;
    switch (type)
        {
        case EBnepGeneralEthernet:
            // General ethernet -- both source and destination present
            LOG(_L8("Type = EBnepGeneralEthernet"));
            SetContents(EDataFrame);
            dest = ETrue;
            src = ETrue;
            break;

        case EBnepControl:
            // Control -- no addresses present (or any data)
            LOG(_L8("Type = EBnepControl"));
            break;

        case EBnepCompressedEthernetDestOnly:
            // Comressed destination only -- only destination present
            LOG(_L8("Type = EBnepCompressedEthernetDestOnly"));
            SetContents(EDataFrame);
            dest = ETrue;
            break;

        case EBnepCompressedEthernetSourceOnly:
            // Compressed source only -- only source present
            LOG(_L8("Type = EBnepCompressedEthernetSourceOnly"));
            SetContents(EDataFrame);
            src = ETrue;
            break;

        case EBnepCompressedEthernet:
            // Compressed -- no addresses present
            LOG(_L8("Type = EBnepCompressedEthernet"));
            SetContents(EDataFrame);
            break;

        default:
            // Unknown packet type
            LOG(_L8("RBnepFrame[%x]: Type = Unknown"));
            User::Leave(KErrCorrupt);   
            break;
        }

    if (iPacketContents & EDataFrame) // Data present
        {
        if (dest)
            {
            TrimBTAddrL(aInboundChain, iDestAddr);	// Address in packet, extract
            LOG_BT(_L8("Extracted destination address %S"), iDestAddr);
            }
        else
            {
            iDestAddr = aLocalAddr;					// No address in packet, assume.
            LOG_BT(_L8("Defaulting destination to %S"), iDestAddr);
            }
        if (src)
            {
            TrimBTAddrL(aInboundChain, iSrcAddr);	// Address in packet, extract
            LOG_BT(_L8("Extracted source address %S"), iSrcAddr);
            }
        else
            {
            iSrcAddr = aRemoteAddr;					// No address in packet, assume.
            LOG_BT(_L8("Defaulting source address to %S"), iSrcAddr);
            }
        // Extract protocol
        TrimUint16L(aInboundChain, iProtocol);
		if(iProtocol == KQTag)
			{
			SetContents(EHas1Q);
			}
        LOG1(_L8("Extracted protocol %04x"), iProtocol);
        }

    ParseL(aInboundChain, type, ebit);

    if(EBnepControl != type)
        {
        // Extract data from the incoming frame that might (depending on any filters
        // need to be forwarded.
        iFramePayload.Assign(aInboundChain);
        LOG1(_L8("Extracted %d bytes of payload data"), iFramePayload.Length());
        }
    return type;
    }

void RBnepFrame::ParseEthernetFrameL (RMBufChain& aFrame)
    {
    /**
       Parse an ethernet frame.
       @param aFrame The ethernet frame received from the higher
       networking layers.
       @internalComponent
    */
    LOG_FUNC
    Reset();
    // Extract the source and destination addresses.
    TrimBTAddrL(aFrame, iDestAddr);
    TrimBTAddrL(aFrame, iSrcAddr);
    LOG_BT(_L8("Extracted destination address %S"),iDestAddr);
    LOG_BT(_L8("Extracted source address %S"),iSrcAddr);
    
    // Pull out protocol, and, if present, 802.1Q data
    TrimUint16L(aFrame, iProtocol);
    LOG1(_L8("Extracted protocol (or type) %04x"), iProtocol);
    
    if (iProtocol <= K802_3MaxLength)
    	{
    	// 'Protocol' field is overloaded.  In 802.3, it actually contains the payload
    	// length.  We can therefore use this field to detect an 802.3 format packet,
    	// and behave accordingly.  Payload type numbers are guaranteed to be greater
    	// than available packet lengths (by design, in order to allow this distinction.)
    	
    	// 802.3 requires additional headers to carry the required data.  Most of this
    	// is essentially constant, especially as we only ever parse ethernet frames
    	// provided by the Symbian NIF.
    	//
    	// Expected values:
    	//  LLC
    	//  DSAP:    aa
    	//  SSAP:    aa
    	//  Control: 03
    	//
    	//  SNAP
    	//  OUI:     00 00 00
    	//  Type:    varies
    	//
    	// We can therefore trim the next six bytes of the supplied data, then extract
    	// the protocol type.  If we were really keen, we could try to verify that the
    	// trimmed bytes are as expected.
    	
    	aFrame.TrimStart(6);
		TrimUint16L(aFrame, iProtocol);
		}
    
    if (KQTag == iProtocol)
        {
        SetContents(EHas1Q);

	    TBuf8<KNetworkProtocolTypeFieldLength> protocolBuf;
		protocolBuf.SetMax();
		if(aFrame.Length() < KIEEE802_1QTagFieldLength + KNetworkProtocolTypeFieldLength)
			{
			aFrame.Free();
			User::Leave(KErrCorrupt);
			}
    	aFrame.CopyOut(protocolBuf, KIEEE802_1QTagFieldLength);
		iProtocol = static_cast<TUint16>((protocolBuf[0] << 8) + protocolBuf[1]);

        LOG1(_L8("Found 802.1Q header, real protocol %04x"), iProtocol);
        }
    
    // Extract frame payload
    SetContents(EDataFrame);
    iFramePayload.Assign(aFrame);
    LOG1(_L8("Extracted %d bytes of payload data"), iFramePayload.Length());
    aFrame.Free();
    }

void RBnepFrame::Reset ()
    {
    /**
       Reset a BNEP frame.
       The object can be allowed to go out of scope only after a call to this
       function.  Note also that some data fields will still contain data after
       this function returns, so the object should either be destroyed or
       re-initialised based on another data frame.
       @internalComponent
    */
    LOG_FUNC
    // Reset contents flag
    iPacketContents = 0;
    // Free any payload data
    iFramePayload.Free();
    // Remove, free and delete all commands
    while(!iCommandQueue.IsEmpty())
        {    
        RBnepControl* element = iCommandQueue.First();
        iCommandQueue.Remove(*element);
        element->Free();
        delete element;
        }

	// By default remote devices are not permitted to use the uplink
	iUplinkAccessAllowedForBnepLink = EFalse;
    }

const TBTDevAddr& RBnepFrame::SrcAddr () const
    {
    LOG_FUNC
    return iSrcAddr; 
    }

void RBnepFrame::TrimBTAddrL (RMBufChain& aChain, TBTDevAddr& aAddr)
    {
    /**
       Populate a Bluetooth address from the front of an MBufChain.
       Extract the bluetooth address and remove it from the MBufChain.
       @param aChain The MBufChain starting with a Bluetooth address.
       @param aAddr The Bluetooth address to populate.
       @internalComponent
    */
    LOG_FUNC
    TPtr8 ptr = aAddr.Des();
    if (aChain.Length() < KBTDevAddrSize)
        {
        User::Leave(KErrCorrupt);
        }
    aChain.CopyOut(ptr);
    aChain.TrimStart(KBTDevAddrSize);
    }

void RBnepFrame::TrimUint16L (RMBufChain& aChain, TUint16& aInt)
    {
    /**
       Read a TUint16 from an MBufChain.
       Reads and removes a TUint16 from the front of an MBufChain.
       @param aChain The MBufChain starting with a TUint16.
       @param aInt The TUint16 to populate.
       @internalComponent
    */
    LOG_FUNC
    if (aChain.Length() < static_cast<TInt>(sizeof(TUint16)))
        {
        User::Leave(KErrCorrupt);
        }
    TBuf8<sizeof(TUint16)> buf;
	buf.SetLength(sizeof(TUint16));
	aChain.CopyOut(buf);
    // To handle endian issues, copy int out bytewise
    aInt = BigEndian::Get16(buf.Ptr());
    aChain.TrimStart(sizeof(TUint16));
    }

void RBnepFrame::ParseL(RMBufChain& aInboundChain, TBnepPacketType aType, TBool aExt)
    {
    /**
       This method does all the donkey work sorting out the controls and possible extensions on the packet.
       @param aInboundChain the rest of the packet
       @param aType the calling method needs to know the packet type when it calls this method
       @param aExt the calling method needs to know about the existence of extensions when it calls this method.
       @internalComponent
    */
    LOG_FUNC
    TInt offset = 0;
    TInt length =0; 	

    if(aType == EBnepControl)
        {
        LOG(_L8("A control"));
        if(aExt)
            {
            LOG(_L8("With extensions"));
            // This is a control packet with extensions.
            // We're going to have to apply a little bit of intelligence to this one,
            // because the spec is a bit unhelpful here (like - how long is the 
            // control part of the message supposed to be BEFORE we start 
            // pulling out the extension headers?)
            TUint8 uuidSize;
            TUint16 maListLength;
            TUint16 ntListLength;
            TBnepControlType type = static_cast<TBnepControlType>(aInboundChain.First()->Ptr()[0]);
            LOG1(_L8("TBnepControlType = 0x%02x"), type);	
            switch (type)
                {
                case EBnepControlCommandNotUnderstood:
                    LOG(_L8("EBnepControlCommandNotUnderstood"));
                    length = 2*sizeof(TUint8); // Control type byte + 1 byte response message
                    ExtractControlL(aInboundChain, length);
                    break;

                case EBnepSetupConnectionRequestMessage: 
                    LOG(_L8("EBnepSetupConnectionRequestMessage"));
                    uuidSize = aInboundChain.First()->Ptr()[1];
                    length = 2*sizeof(TUint8) + 2*uuidSize; // Control type byte + UUIDsize byte + 2 UUIDs
                    // Note, this NOT where we check that UUIDSize is within bounds. That is done later.
                    ExtractControlL(aInboundChain, length);
                    break;

                case EBnepSetupConnectionResponseMessage:
                    LOG(_L8("EBnepSetupConnectionResponseMessage"));
                    length = sizeof(TUint8) + sizeof(TUint16); // Control type byte + 2 byte response message
                    ExtractControlL(aInboundChain, length);
                    break;

                case EBnepFilterNetTypeSetMsg:
                    LOG(_L8("EBnepFilterNetTypeSetMsg"));
                    ntListLength = BigEndian::Get16(aInboundChain.First()->Ptr() + 1);
                    length = sizeof(TUint8) + sizeof(TUint16) + ntListLength; // control type byte + length word + list of filters
                    ExtractControlL(aInboundChain, length);
                    break;

                case EBnepFilterNetTypeResponse:
                    LOG(_L8("EBnepFilterNetTypeResponse"));
                    length = sizeof(TUint8) + sizeof(TUint16); // Control type byte + 2 byte response message
                    ExtractControlL(aInboundChain, length);
                    break;

                case EBnepFilterMultiAddrSetMsg:
                    LOG(_L8("EBnepFilterMultiAddrSetMsg"));
                    maListLength = BigEndian::Get16(aInboundChain.First()->Ptr() + 1);
                    length = sizeof(TUint8) + sizeof(TUint16) + maListLength; // control type byte + length word + list of filters
                    ExtractControlL(aInboundChain, length);
                    break;

                case EBnepFilterMultiAddrResponseMsg:
                    LOG(_L8("EBnepFilterMultiAddrResponseMsg"));
                    length = sizeof(TUint8) + sizeof(TUint16); // Control type byte + 2 byte response message
                    ExtractControlL(aInboundChain, length);
                    break;

                default: // We don't really want to execute this bit of code ever.
                    LOG(_L8("Unknown control type - leaving!"));
                    User::Leave(KErrCorrupt);
                }
            }
        else
            {
            // This one's quite simple because we know that the packet contains a 
            // control and only a control. So the format is pretty simple to stash 
            // as a command for future processing.
            LOG(_L8("Without extensions"));
            length = aInboundChain.Length();
            ExtractControlL(aInboundChain, length);
            }
        }

    if(aExt)
        {
		// Extension headers are present.
        LOG(_L8("Extensions embedded"));

        offset = ParseExtensionL(aInboundChain);
        }
        
	// If the packet includes IEEE 802.1Q info then the real protocol field must be
	// extracted and stored for filtering purposes.
    if(Includes(EHas1Q))
    	{
		offset+=KIEEE802_1QTagFieldLength;
    	// Store the extra protcol information.
		if((offset + KNetworkProtocolTypeFieldLength) > aInboundChain.Length())
			{
			User::Leave(KErrCorrupt);
			}
			
	    TBuf8<KNetworkProtocolTypeFieldLength> protocolBuf;
		protocolBuf.SetMax();

    	aInboundChain.CopyOut(protocolBuf, offset);
    	
		iProtocol = static_cast<TUint16>((protocolBuf[0] << 8) + protocolBuf[1]);
    	}	
    }

TInt RBnepFrame::ParseExtensionL(RMBufChain& aInboundChain)
    {
    /**
       Helper method to extract control extension headers from a packet.
       Non-Control extension headers will remain in the inbound chain
       @param aInboundChain the rest of the packet
       @internalComponent
    */
    LOG_FUNC
    TUint16 offset = 0;
    TUint16 lastExtensionHeaderOffset = 0;
    TBool ebit = ETrue;
	TBool removeExtensions = EFalse;
	
    TBuf8<KExtensionHeaderHeaderBytes> buf;
	buf.SetMax();
    
    // Used to verify that the extensions are in assending order.
    TBnepExtensionType lastExtensionType = EBnepExtensionControl;
    
    while(ebit)
        {
        // Extension headers are 2 bytes long.
        if((offset + KExtensionHeaderHeaderBytes) > aInboundChain.Length())
            {	
            User::Leave(KErrCorrupt);
            }

    	aInboundChain.CopyOut(buf, offset);
        
        // Extract the extension header type and e-bit.
        TBnepExtensionType type = static_cast<TBnepExtensionType>(buf[0]);
		ebit = type & KEBit;
        type = static_cast<TBnepExtensionType>(type & (~KEBit));

		if(type < lastExtensionType)
			{
			// Extensions are not in assending order.  Remove all subsequent
			// extensions from the packet.
			if(!removeExtensions)
				{
				// The previous extension header bit must be set to zero
				removeExtensions = ETrue;
				TBuf8<KSizeOfBnepExtensionType> exTypeBuf;
				exTypeBuf.SetMax();
				aInboundChain.CopyOut(exTypeBuf, lastExtensionHeaderOffset);
				exTypeBuf[0] &= ~KEBit;
				aInboundChain.CopyIn(exTypeBuf, lastExtensionHeaderOffset);
				}
			}
		
		lastExtensionType = type;	

        // Extract the length of this extension header
        TUint8 length = buf[1];

        LOG4(_L8("Found extension header, type %02x, ebit %02x, length %d, offset %d"),
    			      type, ebit, length, offset);

        if((length + KExtensionHeaderHeaderBytes) > (aInboundChain.Length() - offset))
            {	
            User::Leave(KErrCorrupt);
            }

		if(type == EBnepExtensionControl && !removeExtensions)
			{
			// Remove the BNEP extension control header (i.e., [ebit]0x00, Length). 
			aInboundChain.TrimStart(KExtensionHeaderHeaderBytes);
			
			// Control extention headers need to be extracted.
			ExtractControlL(aInboundChain, length);
			}
		else
			{
			if(!removeExtensions)
				{
				// If the extension has not been removed adjust the offset.
				lastExtensionHeaderOffset = offset;
				offset += (length + KExtensionHeaderHeaderBytes);
				
				// Note that an extension header is present in the payload.
				SetContents(EExtensionHeaders);
				}
			else
				{
				// Remove the extension.
				RMBufChain tempBuf;
				// Split the buffer so that aInboundChain includes everything before
				// the extension to be removed and tempBuf includes everything after
				// including the extension to be removed.
				aInboundChain.SplitL(offset, tempBuf);
				
				// Remove the extension from tempBuf.
				tempBuf.TrimStart(length + KExtensionHeaderHeaderBytes);
				
				// Re-join the two buffers
				aInboundChain.Append(tempBuf);
				}
			}
        }

	return offset;
    }

void RBnepFrame::ExtractControlL(RMBufChain& aInboundChain, TInt aLength)
    {
    /**
       Helper method to extract the control portion of a packet.
       @param aInboundChain the rest of the packet.
       @param aLength the size of the control portion to be extracted.
       @internalComponent
    */
    LOG_FUNC
    LOG1(_L8("Extraction length = %d bytes"),aLength);
    if(0 == aLength)	
        {
        LOG(_L8("Zero length buffer - leaving!"));
        User::Leave(KErrArgument);
        }
    if(aLength > aInboundChain.Length())
        {
        LOG(_L8("Corrupted buffer - leaving!"));
        User::Leave(KErrCorrupt);
        }
    RBnepControl* command = new(ELeave) RBnepControl();
    CleanupStack::PushL(TCleanupItem(doResetControl, command));
    // MBuf SplitL is a bit awkward here, as it splits off the end of the chain
    // into the supplied parameter.  So move the MBuf data to all be with the
    // command MBufChain, then split back into the original MBufChain.
    command->Assign(aInboundChain);
    command->SplitL(aLength,aInboundChain);
    CleanupStack::Pop(command);
    LOG1(_L8("Storing new command 0x%08x in queue"),command);
    iCommandQueue.AddLast(*command);
    SetContents(EControl);
    }

void RBnepFrame::ExecuteControl (MLinkManager& aController)
    {
    /**
       Helper method that pushes the controls in the queue up to the controller for handling 
       and response.
       @param aController The controller object to pass the control messages to.
       @internalComponent
    */
    LOG_FUNC
    while(!iCommandQueue.IsEmpty() &&
          !aController.AwaitingResponse())
        {    
        RBnepControl* element = iCommandQueue.First();
        iCommandQueue.Remove(*element);
        LOG(_L8("Passing command up to controller"));
        // The controller will now own the control and will become responsible for its memory management
        aController.Execute(element); 
        }

    // If we reached the end, then show that the packet no longer contains control extensions.
    if (iCommandQueue.IsEmpty())
    	{
    	iPacketContents &= ~EControl;
    	}
    }

    
TInt RBnepFrame::PayloadLength() const
	{
	return iFramePayload.Length();
	}

void RBnepFrame::SetUplinkAccessAllowedForBnepLink(TBool aUplinkAccessAllowedForBnepLink)
	{
	iUplinkAccessAllowedForBnepLink = aUplinkAccessAllowedForBnepLink;
	}
