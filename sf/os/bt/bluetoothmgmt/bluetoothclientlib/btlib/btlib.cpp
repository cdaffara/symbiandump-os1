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

#include <bttypes.h>
#include <bt_sock.h>

/** 
Constructor
Start with no requests to update the required configuration.
*/
EXPORT_C TL2CapConfig::TL2CapConfig()
 : iSpecifiedMask(ENoConfigElementsSpecified)
 	{
 	}
 	
/** 
Request a new preferred Max Transmit Unit size.
	
The L2CAP channel configuration process will attempt to configure with this MTU
size. The resulting negotiated MTU will be at most this size and at least the
size given by the minimum acceptable MTU option (@see SetMinMTU).
Please note that if the minimum acceptable MTU is not specified along with this
parameter, it is taken to be equal to the preferred value, so a smaller MTU
value will not be accepted from the peer during the negotiation.

@param aSize the MTU size for use in configuration
@return error code, KErrArgument if MTU set too small, otherwise KErrNone
*/
EXPORT_C TInt TL2CapConfig::SetMaxTransmitUnitSize(TUint16 aSize)
	{
	TInt rerr = KErrArgument;
	if(aSize >= KL2MinMTU)
		{		
		iMTUSize = aSize;
		iSpecifiedMask |= EMTUSizeSpecifiedMask;
		rerr = KErrNone;
		}
	return rerr;
	}
	
/** 
Returns preferred MTU.
	
Also allows the user to know whether that value has been set, 
or whether it is a random value that will be ignored. 
This is done via the parameter 'aIsSpecified'. 
	
@param aIsSpecified Used to tell the caller whether the MTU value has been set. 
@return preferred MTU
*/
EXPORT_C TUint16 TL2CapConfig::MaxTransmitUnitSize(TBool& aIsSpecified) const
	{
	aIsSpecified = ((iSpecifiedMask & EMTUSizeSpecifiedMask) != 0);
	return iMTUSize;
	}
	
/** 
Request a new preferred Max Receive Unit size.

The L2CAP channel configuration process will attempt to configure with this MRU
size.  The resulting negotiated MRU will be at most this size and at least the
size given by the minimum acceptable MRU option (@see SetMinMRU).
Please note that if the minimum acceptable MRU is not specified, it is taken by
APIs using this class to be equal to the protocol minimum (48 bytes). 

@param aSize the MRU size for use in configuration
@return error code, KErrArgument if MRU set too small, otherwise KErrNone
*/
EXPORT_C TInt TL2CapConfig::SetMaxReceiveUnitSize(TUint16 aSize)
	{
	TInt rerr = KErrArgument;
	if(aSize >= KL2MinMTU)
		{		
		iMRUSize = aSize;
		iSpecifiedMask |= EMRUSizeSpecifiedMask;
		rerr = KErrNone;
		}
	return rerr;
	}
	
/** 
Returns preferred MRU.
	
Also allows the user to know whether that value has been set, 
or whether it is a random value that will be ignored. 
This is done via the parameter 'aIsSpecified'. 

@param aIsSpecified Used to tell the caller whether the MRU value has been set. 
@return preferred MRU
*/
EXPORT_C TUint16 TL2CapConfig::MaxReceiveUnitSize(TBool& aIsSpecified) const
	{
	aIsSpecified = ((iSpecifiedMask & EMRUSizeSpecifiedMask) != 0);
	return iMRUSize;
	}

/**
Set the minimum MTU acceptable during L2CAP channel configuration.

The configuration process will attempt to configure with the largest possible
MTU that's greater than or equal to this boundary and less than or equal to
the preferred value (@see SetMaxTransmitUnitSize) or the preferred stack
default.

A peer's proposal of any value below the minimum will be rejected and replied to
with a value equal to the acceptable minimum. The peer will then choose to
either close the connection or proceed with the acceptable minimum.

@param aSize the smallest acceptable MTU size
@return error code, KErrArgument if MTU set too small, otherwise KErrNone
*/
EXPORT_C TInt TL2CapConfig::SetMinMTU(TUint16 aSize)
	{
	TInt rerr = KErrArgument;
	if(aSize >= KL2MinMTU)
		{		
		iMinMTUSize = aSize;
		iSpecifiedMask |= EMinMTUSizeSpecifiedMask;
		rerr = KErrNone;
		}
	return rerr;
	}

/** 
Returns the minimum acceptable negotiated MTU.
	
Also allows the user to know whether that value has been set, 
or whether it is a random value that will be ignored. 
This is done via the parameter 'aIsSpecified'. 

@param aIsSpecified Used to tell the caller whether the smallest acceptable MTU
	value has been set. 
@return Requested smallest acceptable MTU.
*/
EXPORT_C TUint16 TL2CapConfig::MinMTU(TBool& aIsSpecified) const
	{
	aIsSpecified = ((iSpecifiedMask & EMinMTUSizeSpecifiedMask) != 0);
	return iMinMTUSize;
	}

/**
Set the minimum MRU acceptable during L2CAP channel configuration.

The configuration process will attempt to configure with the largest possible
MRU that's greater than or equal to this boundary and less than or equal to
the preferred value (@see SetMaxReceiveUnitSize) or the preferred stack
default.

If the peer proposes an MRU below the minimum the L2CAP channel will be
disconnected, so this option should not be specified unless the application/profile
requires a guarantee of MRU of certain size and can not work with a smaller one.

@param aSize the smallest acceptable MRU size
@return error code, KErrArgument if MRU set too small, otherwise KErrNone
*/
EXPORT_C TInt TL2CapConfig::SetMinMRU(TUint16 aSize)
	{
	TInt rerr = KErrArgument;
	if(aSize >= KL2MinMTU)
		{		
		iMinMRUSize = aSize;
		iSpecifiedMask |= EMinMRUSizeSpecifiedMask;
		rerr = KErrNone;
		}
	return rerr;
	}
	
/** 
Returns the minimum acceptable negotiated MRU.
	
Also allows the user to know whether that value has been set, 
or whether it is a random value that will be ignored. 
This is done via the parameter 'aIsSpecified'. 

@param aIsSpecified Used to tell the caller whether the smallest acceptable MRU
	value has been set. 
@return Requested smallest acceptable MRU.
*/
EXPORT_C TUint16 TL2CapConfig::MinMRU(TBool& aIsSpecified) const
	{
	aIsSpecified = ((iSpecifiedMask & EMinMRUSizeSpecifiedMask) != 0);
	return iMinMRUSize;
	}

/** 
Request a reliable channel.

A reliable channel relies on the retransmission by L2Cap of unacknowledged 
L2Cap packets. 
Retransmissions are continued for a specified length of time. 
If this time is exceeded the L2Cap channel is disconnected.

The resulting behaviour depends on the setting of the LegacyModesDisallowed
option (@see SetLegacyModesDisallowed). If that option is disabled (which is the
default setting), then the channel mode attempted in L2CAP configuration process
will be Enhanced Retransmission Mode if the peer supports it, else
Retransmission Mode if the peer supports it, else Basic Mode.

If the LegacyModesDisallowed option is enabled, then the connection will only
be made if the peer supports and accepts the Enhanced Retransmission Mode.
If it doesn't then the connection will not be made.

Note that due to the nature of the negotiation process, it is not guranteed that
a mode will be negotiated even if it's supported by a peer.

The KL2CAPNegotiatedChannelMode socket option can be used to obtain the
negotiated channel mode after a socket has been connected.

@param aRetransmissionTimer The length of time allowed for l2Cap transmissions.
Note that the value of this parameter does not directly drive the L2CAP
retransmission timer. It is instead translated into a corresponding value for
the maximum number of transmissions of a single packet. If that number is
exceeded, then the connection is closed.
@return error code, currently KErrNone
*/
EXPORT_C TInt TL2CapConfig::ConfigureReliableChannel(TUint16 aRetransmissionTimer)
	{
	iChannelReliability = EReliableChannel;
	iChannelReliabilityTimer = aRetransmissionTimer;
	iSpecifiedMask |= EReliabilitySpecifiedMask;
	return KErrNone;
	}
	
	
/** 
Request a reliable channel.

This is a deprecated version of the ConfigureReliableChannel method and its
behaviour is exactly the same.

@param aRetransmissionTimer The length of time allowed for l2Cap transmissions
@return error code, currently KErrNone
@deprecated
Use ConfigureReliableChannel(TUint16 aRetransmissionTimer) instead
*/
EXPORT_C TInt TL2CapConfig::SetupReliableChannel(TUint16 aRetransmissionTimer)
	{
	iChannelReliability = EReliableChannel;
	iChannelReliabilityTimer = aRetransmissionTimer;
	iSpecifiedMask |= EReliabilitySpecifiedMask;
	return KErrNone;
	}

/** 
Request an unreliable channel.

An unreliable channel allows L2Cap packets to be dropped by the baseband.
It helps to maintain low latency at the cost of reliability.

The resulting behaviour depends on the setting of the LegacyModesDisallowed
option (@see SetLegacyModesDisallowed). If that option is disabled (which is the
default setting), then the channel mode attempted in L2CAP configuration process
will be Streaming Mode if the peer supports it, else Flow Control Mode if the
peer supports it, else Basic Mode.

If the LegacyModesDisallowed option is enabled, then the connection will only
be made if the peer supports and accepts Streaming Mode. If it doesn't then the
connection will not be made.

Note that due to the nature of the negotiation process, it is not guranteed that
a mode will be negotiated even if it's supported by a peer.

The KL2CAPNegotiatedChannelMode socket option can be used to obtain the
negotiated channel modes after a socket has been connected.

@param aObsolescenceTimer The time after which a packet may be dropped 
or "flushed" by the baseband. Note that outgoing packet flushing may not be
implemented on all Symbian OS platforms.
@return error code, KErrArgument if the obsolescence time is made shorter than is 
physically possible, otherwise KErrNone

*/
EXPORT_C TInt TL2CapConfig::ConfigureUnreliableChannel(TUint16 aObsolescenceTimer)
	{
	TInt rerr = KErrNone;

	// Only flush timers above a certain physical limit are permitted.
	if(aObsolescenceTimer < EMinDataObsolescenceTimeout)
		{
		rerr = KErrArgument;
		}
	else
		{
		iChannelReliability = EUnreliableChannel;
		iChannelReliabilityTimer = aObsolescenceTimer;
		iSpecifiedMask |= EReliabilitySpecifiedMask;
		}
	return rerr;
	}

/** 
Request an unreliable channel.

This is a deprecated version of the ConfigureReliableChannel method and its
behaviour is exactly the same.

@param aObsolescenceTimer The time after which a packet may be dropped 
or "flushed" by the baseband.
@return error code, KErrArgument if the obsolescence time is made shorter than is 
physically possible, otherwise KErrNone
@deprecated
Use ConfigureUnreliableChannel(TUint16 aObsolescenceTimer) instead
*/
EXPORT_C TInt TL2CapConfig::SetupUnreliableChannel(TUint16 aObsolescenceTimer)
	{
	TInt rerr = KErrNone;

	// Only flush timers above a certain physical limit are permitted.
	if(aObsolescenceTimer < EMinDataObsolescenceTimeout)
		{
		rerr = KErrArgument;
		}
	else
		{
		iChannelReliability = EUnreliableChannel;
		iChannelReliabilityTimer = aObsolescenceTimer;
		iSpecifiedMask |= EReliabilitySpecifiedMask;
		}
	return rerr;
	}
	
/** 
Request an unreliable channel, but allow any channel mode to be negotiated.

This will configure the socket to prefer unreliable modes during L2CAP channel
configuration, but accept any mode proposed by the peer.
The purpose of this interface is to be used with listening sockets which need
to accept both reliable and unreliable mode connections on a single PSM.
The ConfigureReliableChannel and ConfigureUnreliableChannel methods should be
used in preference to this one in normal situations.

The LegacyModesDisallowed option does not influence the behavior of this method.

The KL2CAPNegotiatedChannelMode socket option can be used to obtain the
negotiated channel modes after a socket has been connected.

@param aObsolescenceTimer The time after which a packet may be dropped 
or "flushed" by the baseband if an Unreliable channel is negotiated.
Note that outgoing packet flushing may not be implemented on all Symbian OS
platforms.

@param aRetransmissionTimer The length of time allowed for l2Cap transmissions
if a Reliable channel is negotiated.
Note that the value of this parameter does not directly drive the L2CAP
retransmission timer. It is instead translated into a corresponding value for
the maximum number of transmissions of a single packet. If that number is
exceeded, then the connection is closed.
@return error code, KErrArgument if the obsolescence time is made shorter than is 
physically possible, otherwise KErrNone
*/
EXPORT_C TInt TL2CapConfig::ConfigureUnreliableDesiredChannel(TUint16 aObsolescenceTimer, TUint16 aRetransmissionTimer)
	{
	TInt rerr = KErrNone;

	// Only flush timers above a certain physical limit are permitted.
	if(aObsolescenceTimer < EMinDataObsolescenceTimeout)
		{
		rerr = KErrArgument;
		}
	else
		{
		iChannelReliability = EUnreliableDesiredChannel;
		iChannelReliabilityTimer = aObsolescenceTimer;
		iAdditionalChannelReliabilityTimer = aRetransmissionTimer;
		iSpecifiedMask |= EReliabilitySpecifiedMask;
		}
	return rerr;
	}

/** 
Disallow usage of legacy L2CAP channel modes.

This option influences the behavior of ConfigureReliableChannel and
ConfigureUnreliableChannel. If it's enabled, then only the newest
reliable/unreliable modes will be allowed during L2CAP channel configuration.

The default value is to allow the usage of legacy modes.

@param aDisallowed Whether the usage of legacy modes is disallowed.
*/
EXPORT_C void TL2CapConfig::SetLegacyModesDisallowed(TBool aDisallowed)
	{
	if (aDisallowed)
		{
		iSpecifiedMask |= ELegacyModesDisallowedSpecifiedMask;
		}
	else
		{
		iSpecifiedMask &= ~ELegacyModesDisallowedSpecifiedMask;
		}
	}

/** 
Checks if the usage of legacy L2CAP channel modes is disallowed.
The default value is to allow the usage of legacy modes.

@param aDisallowed Whether the usage of legacy modes is disallowed.
*/
EXPORT_C TBool TL2CapConfig::LegacyModesDisallowed() const
	{
	return iSpecifiedMask & ELegacyModesDisallowedSpecifiedMask;
	}


/** 
Returns the channel reliability and the associated timer.

Also allows the user to know whether reliability has been set up, 
or whether it is a random value that will be ignored. 
This is done via the parameter 'aIsSpecified'. 
The associated timer is the obsolescence timer if the channel is unreliable 
and the retransmission timer if it is reliable.
This is returned via the parameter 'aAssociatedTimer'.
Note that if ConfigureUnreliableDesiredChannel was used then both timer values
have been set and this method will only return the obsolescence timeout.
The overload of this method which doesn't return a timer value together
with the timer value getters can be used instead of this one to uniformly handle
all cases.

@param aIsSpecified Used to tell the caller whether reliability has been set up. 
@param aAssociatedTimer This is a 'return' value. The associated timer is the obsolescence timer if the channel is unreliable 
and the retransmission timer if it is reliable. 
@return the reliability
*/
EXPORT_C TL2CapConfig::TChannelReliability TL2CapConfig::ChannelReliability(TBool& aIsSpecified, TUint16& aAssociatedTimer) const
	{
	aIsSpecified = ((iSpecifiedMask & EReliabilitySpecifiedMask) != 0);
	aAssociatedTimer = iChannelReliabilityTimer;
	return iChannelReliability;
	}

/** 
Returns the channel reliability.

Also allows the user to know whether reliability has been set up, 
or whether it is a random value that will be ignored. 
This is done via the parameter 'aIsSpecified'. 
The associated timer is the obsolescence timer if the channel is unreliable, 
the retransmission timer if it is reliable, and both timers if it's 'unreliable
desired'.
The associated timer values can be obtained with RetransmissionTimer() and
ObsolescenceTimer() methods.

@param aIsSpecified Used to tell the caller whether reliability has been set up. 
@return the reliability
*/
EXPORT_C TL2CapConfig::TChannelReliability TL2CapConfig::ChannelReliability(TBool& aIsSpecified) const
	{
	aIsSpecified = ((iSpecifiedMask & EReliabilitySpecifiedMask) != 0);
	return iChannelReliability;
	}

/** 
Returns the retransmission timer.

@param aIsSpecified Used to tell the caller whether the timer value has been set.
@return The value of the timer.
*/
EXPORT_C TUint16 TL2CapConfig::RetransmissionTimer(TBool& aIsSpecified) const
	{
	TUint16 timer = 0;
	aIsSpecified = EFalse;
	TBool reliabilitySpecified = ((iSpecifiedMask & EReliabilitySpecifiedMask) != 0);
	if (reliabilitySpecified)
		{
		if (iChannelReliability == EReliableChannel)
			{
			aIsSpecified = ETrue;
			// It's the only timer specified, so it's in iChannelReliabilityTimer.
			timer = iChannelReliabilityTimer;
			}
		else if (iChannelReliability == EUnreliableDesiredChannel)
			{
			aIsSpecified = ETrue;
			// We've got 2 timers specified, Retransmission goes in the additional field
			// (see the note accompanying field declarations).
			timer = iAdditionalChannelReliabilityTimer;
			}
		}
	return timer;
	}

/** 
Returns the obsolescence timer.

@param aIsSpecified Used to tell the caller whether the timer value has been set.
@return The value of the timer.
*/
EXPORT_C TUint16 TL2CapConfig::ObsolescenceTimer(TBool& aIsSpecified) const
	{
	TUint16 timer = 0;
	aIsSpecified = EFalse;

	if ((iSpecifiedMask & EReliabilitySpecifiedMask) != 0)
		{
		if (iChannelReliability == EUnreliableChannel || iChannelReliability == EUnreliableDesiredChannel)
			{
			aIsSpecified = ETrue;
			// Flush timeout always goes in iChannelReliabilityTimer
			// (see the note accompanying field declarations).
			timer = iChannelReliabilityTimer;
			}
		}
	return timer;
	}

/** 
Request a new channel priority.

L2Cap channels have three priorities, low, medium, and high. Data on channels with 
higher priority may be sent before data on channels with lower priority.

@param aPriority the priority to be used for this L2Cap channel
@return error code, currently KErrNone
*/
EXPORT_C TInt TL2CapConfig::ConfigureChannelPriority(TL2CapConfig::TChannelPriority aPriority)
	{
	iChannelPriority = aPriority;
	iSpecifiedMask |= EPrioritySpecifiedMask;
	return KErrNone;
	}

/** 
Request a new channel priority.

L2Cap channels have three priorities, low, medium, and high. Data on channels with 
higher priority may be sent before data on channels with lower priority.

@param aPriority the priority to be used for this L2Cap channel
@return error code, currently KErrNone
@deprecated
Use ConfigureChannelPriority(TChannelPriority aPriority) instead
*/
EXPORT_C TInt TL2CapConfig::SetChannelPriority(TL2CapConfig::TChannelPriority aPriority)
	{
	iChannelPriority = aPriority;
	iSpecifiedMask |= EPrioritySpecifiedMask;
	return KErrNone;
	}
	
/** 
Returns requested channel priority.

Also allows the user to know whether that priority has been set, 
or whether it is a random value that will be ignored. 
This is done via the parameter 'aIsSpecified'. 

@param aIsSpecified Used to tell the caller whether the priority has been set. 
@return requested channel priority.
*/
EXPORT_C TL2CapConfig::TChannelPriority TL2CapConfig::ChannelPriority(TBool& aIsSpecified) const
	{
	aIsSpecified = ((iSpecifiedMask & EPrioritySpecifiedMask) != 0);
	return iChannelPriority;
	}

/**
Comparison operator.

Compare the pincode length first, if equal, then compare each individual bits

@param aPINCodeV10 Object to compare to this.
@return ETrue if aPINCodeV10 is the same as this, EFalse if not.
*/
EXPORT_C TBool TPINCodeV10::operator==(const TPINCodeV10& aPINCodeV10) const
	{
	TBool equal = EFalse;
	
	if (iLength == aPINCodeV10.iLength)
		{
		equal = ETrue; 
		
		//check for individual bits, if not equal, then set equal variable to EFalse
		for (TInt i = 0; i < iLength; i++)
			{
			if (iPIN[i] != aPINCodeV10.iPIN[i])
				{
				equal = EFalse;
				break;
				}
			}
		}
	
	return equal;
	}
