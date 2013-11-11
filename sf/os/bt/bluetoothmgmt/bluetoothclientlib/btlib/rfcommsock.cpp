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

#include <bt_sock.h>

#define FLOG(a) (a)

// Remote Port Params class
EXPORT_C void TRfcommRemotePortParams::UpdateFlowCtrlBit(TUint8 aFlowCtrl, 
												TRPNFlowCtrlMask aFCMask)
/**	Constructor
Set the flow control bit specified by aFCMask to 
the same value it has in aFlowCtrl
@param aFlowCtrl Flow control specifier.
@param aFCMask Flow control mask.
*/
	{
	// Set flow control to all zeros if it's not valid already
	if(!(IsValid() & EVMFlowCtrl))
		{
		const TUint8 KDefaultFlowControl = 0;
		SetFlowCtrl(KDefaultFlowControl);
		}

	// bitMask is guaranteed to have one and only one bit set as long as
	// the enums in TRPNFlowCtrlMask only have one bit set each
	TUint8 bitMask = TUint8(aFCMask);
	if(aFlowCtrl & bitMask) 
		{
		// Bit is set in aFlowCtrl so set it here too
		SetFlowCtrl(iFlowCtrl |= bitMask);
		}
	else
		{
		// Bit is clear in aFlowCtrl so clear it here too
		SetFlowCtrl(iFlowCtrl &= ~bitMask);	
		}
	}	

EXPORT_C void TRfcommRemotePortParams::UpdateWholeFlowCtrl(TUint16 aParamMask, 
												  TUint8 aFlowCtrl)
/** Used to update flow control with relevant bits from aFlowCtrl 
(as specified by aParamsMask)
@param aParamMask The parameter mask specifing what to change.
@param aFlowCtrl Flow control information specifing new values.
*/
	{
	if(aParamMask & EPMXOnOffInput) 
		{
		// XON/off on input
		UpdateFlowCtrlBit(aFlowCtrl, EFCXOnOffInput);
		}
		
	if(aParamMask & EPMXOnOffOutput)
		{
		// XON/off on output
		UpdateFlowCtrlBit(aFlowCtrl, EFCXOnOffOutput);
		}

	if(aParamMask & EPMRTRInput)
		{
		// RTR on input
		UpdateFlowCtrlBit(aFlowCtrl, EFCRTRInput);
		}
	
	if(aParamMask & EPMRTROutput)
		{
		// RTR on output
		UpdateFlowCtrlBit(aFlowCtrl, EFCRTROutput);
		}

	if(aParamMask & EPMRTCInput)
		{
		// RTC on input
		UpdateFlowCtrlBit(aFlowCtrl, EFCRTCInput);
		}
			
	if(aParamMask & EPMRTCOutput)
		{
		// RTC on output
		UpdateFlowCtrlBit(aFlowCtrl, EFCRTCOutput);
		}
	}

EXPORT_C void TRfcommRemotePortParams::UpdateFromRPNTransaction(
							const TRfcommRPNTransaction& aRPNTransaction)
/**	Update using a TRfcommRPNTransaction class.
Update all parameters according to parameters and mask in aRPNTransaction
@param aRPNTransaction The internal values are updated according to the values held in this attribute.
@see TRfcommRPNTransaction
*/
	{
	TUint16 paramMask					= aRPNTransaction.iParamMask;
	TRfcommRemotePortParams portParams	= aRPNTransaction.iPortParams;

	// Copy everything that is marked in the parameter mask if it is valid in the
	// source 'remote port params' structure
	if(paramMask & EPMBitRate)
		{
		TBps bitRate = EBps9600; // To stop compiler warnings
		if(portParams.GetBitRate(bitRate)) // Guarantees 'bitRate' set if returns True
			SetBitRate(bitRate);
		}
	
	if(paramMask & EPMDataBits)
		{
		TDataBits dataBits = EData5; // To stop compiler warnings
		if(portParams.GetDataBits(dataBits)) // Guarantees 'dataBits' set if returns True
			SetDataBits(dataBits);
		}

	if(paramMask & EPMStopBit)
		{
		TStopBits stopBit = EStop1; // To stop compiler warnings
		if(portParams.GetStopBit(stopBit)) // Guarantees 'stopBit' set if returns True
			SetStopBit(stopBit);
		}

	// Both indicate a change in the same thing in TRfcommRemotePortparams
	if(paramMask & EPMParity || paramMask & EPMParityType)
		{
		TParity parity = EParityNone; // To stop compiler warnings
		if(portParams.GetParity(parity)) // Guarantees 'parity' set if returns True
			SetParity(parity);
		}
	
	if(paramMask & EPMXOnChar)
		{
		TUint8 xOnChar = 0; // To stop compiler warnings
		if(portParams.GetXOnChar(xOnChar)) // Guarantees 'xOnChar' set if returns True
			SetXOnChar(xOnChar);
		}

	if(paramMask & EPMXOffChar)
		{
		TUint8 xOffChar = 0; // To stop compiler warnings
		if(portParams.GetXOffChar(xOffChar)) // Guarantees 'xOffChar' set if returns True
			SetXOffChar(xOffChar);
		}

	// Flow control is compound - do it separately
	TUint8 flowCtrl = 0; // To stop compiler warnings
	if(portParams.GetFlowCtrl(flowCtrl))
		UpdateWholeFlowCtrl(paramMask, flowCtrl);
	}

EXPORT_C TRfcommRemotePortParams::TRfcommRemotePortParams()
	: iFlowCtrl(0),
	  iXOnChar(0),
	  iXOffChar(0),
  	  iValidMask(0)	  
	{
	}

EXPORT_C TUint8 TRfcommRemotePortParams::IsValid() const
	{
	return iValidMask;
	}

EXPORT_C TBool TRfcommRemotePortParams::GetBitRate(TBps& aBitRate) const
	{
	if(IsValid() & EVMBitRate)
		{
		aBitRate = iBitRate;
		return ETrue;
		}
	else
		return EFalse;
	}

EXPORT_C TInt TRfcommRemotePortParams::SetBitRate(TBps aBitRate)
	{
	switch(aBitRate)
		{
	case EBps2400:
	case EBps4800:
	case EBps7200:
	case EBps9600:
	case EBps19200:
	case EBps38400:
	case EBps57600:
	case EBps115200:
	case EBps230400:
		iValidMask|=EVMBitRate;
		iBitRate=aBitRate;
		return KErrNone;
		//break;
	default:
		return KErrArgument;
		//break;
		}
	}

EXPORT_C TBool TRfcommRemotePortParams::GetDataBits(TDataBits& aDataBits) const
// If Data Bits are valid, sets reference and returns True, 
// otherwise, returns False
	{
	if(IsValid() & EVMDataBits)	
		{
		aDataBits = iDataBits;
		return ETrue;
		}
	else
		return EFalse;
	}

EXPORT_C TInt TRfcommRemotePortParams::SetDataBits(TDataBits aDataBits)
	{
	iValidMask|=EVMDataBits;
	iDataBits=aDataBits;
	return KErrNone;
	}

EXPORT_C TBool TRfcommRemotePortParams::GetStopBit(TStopBits& aStopBit) const
// If Stop Bit is valid, sets reference and returns True, 
// otherwise, returns False
	{
	if(IsValid() & EVMStopBit)
		{
		aStopBit = iStopBit;
		return ETrue;
		}
	else
		return EFalse;
	}

EXPORT_C TInt TRfcommRemotePortParams::SetStopBit(TStopBits aStopBit)
	{
	iValidMask|=EVMStopBit;
	iStopBit=aStopBit;
	return KErrNone;
	}

EXPORT_C TBool TRfcommRemotePortParams::GetParity(TParity& aParity) const
// If Parity is valid, sets reference and returns True, 
// otherwise, returns False
	{
	if(IsValid() & EVMParity)
		{
		aParity = iParity;
		return ETrue;
		}
	else
		return EFalse;
	}

EXPORT_C TInt TRfcommRemotePortParams::SetParity(TParity aParity)
	{
	iValidMask|=EVMParity;
	iParity=aParity;
	return KErrNone;
	}

EXPORT_C TBool TRfcommRemotePortParams::GetFlowCtrl(TUint8& aFlowCtrl) const
// If Flow Control is valid, sets reference and returns True, 
// otherwise, returns False
	{
	if(IsValid() & EVMFlowCtrl)
		{
		aFlowCtrl = iFlowCtrl;
		return ETrue;
		}
	else
		return EFalse;
	}

EXPORT_C TInt TRfcommRemotePortParams::SetFlowCtrl(TUint8 aFlowCtrl)
	{
	iValidMask|=EVMFlowCtrl;
	iFlowCtrl=aFlowCtrl;
	return KErrNone;
	}

EXPORT_C TBool TRfcommRemotePortParams::GetXOnChar(TUint8& aXOnChar) const
// If XOn Char is valid, sets reference and returns True, 
// otherwise, returns False
	{
	if(IsValid() & EVMXOnChar)
		{
		aXOnChar = iXOnChar;
		return ETrue;
		}
	else
		return EFalse;
	}

EXPORT_C TInt TRfcommRemotePortParams::SetXOnChar(TUint8 aXOnChar)
	{
	iValidMask|=EVMXOnChar;
	iXOnChar=aXOnChar;
	return KErrNone;
	}

EXPORT_C TBool TRfcommRemotePortParams::GetXOffChar(TUint8& aXOffChar) const
// If XOff Char is valid, sets reference and returns True, 
// otherwise, returns False
	{
	if(IsValid() & EVMXOffChar)
		{
		aXOffChar = iXOffChar;
		return ETrue;
		}
	else
		return EFalse;
	}

EXPORT_C TInt TRfcommRemotePortParams::SetXOffChar(TUint8 aXOffChar)
	{
	iValidMask|=EVMXOffChar;
	iXOffChar=aXOffChar;
	return KErrNone;
	}

// RPN Transaction class

EXPORT_C TRfcommRPNTransaction::TRfcommRPNTransaction():iParamMask(0)
	{
	}

// Socket Address class

EXPORT_C TRfcommSockAddr::TRfcommSockAddr()
/** Constructor
*/
	{
	}

EXPORT_C TRfcommSockAddr::TRfcommSockAddr(const TSockAddr &aAddr)
	: TBTSockAddr(aAddr)
/** Constructor that takes a BT socket address.
@param aAddr The BT socket address from which this instance is initialised. 
*/
	{
	}

EXPORT_C TRfcommSockAddr& TRfcommSockAddr::Cast(const TSockAddr &aAddr)
	{
	return *((TRfcommSockAddr *)&aAddr);
	}
	
// EOF
