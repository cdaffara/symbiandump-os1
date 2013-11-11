// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: The main purpose of RTFXEffect implementation is to forward 
// RWsSession/RWindowBase calls to RegisterEffect and OverrideEffect to server side. 
// Please see documantion of RTFXEffect::RegisterTFXEffect() for more details.
//

#include "rtfxeffect.h"
#include "w32comm.h"
#include "client.h"

/**
Constructor which takes handle and buffer paratemter and initilaizes 
its member variables.

@param aHandle Client side handle of the class derived from MWsClientClass
@param aBuffer Pointer to the wserv client side buffer of the above class
*/
RTFXEffect::RTFXEffect(TInt aHandle, RWsBuffer* aBuffer) 
	: MWsClientClass(aBuffer), iDirPathSizePaded(0), iFileName1SizePaded(0),
	  iFileName2SizePaded(0), iCombSizePaded(0) 
	{
	iWsHandle = aHandle;
	}

/**
Function checks the sent parmaters and pancis client if any of its length 
is greater than KMaxFileName. Calculates the padded lengths of sent parameters
and stores them in member variables

@param aResourceDir directory name of animation description file
@param aFilenameOutgoing File name of Outgoing phase of TFX
@param aFilenameIncoming File name of Incoming phase of TFX
*/
void RTFXEffect::CheckFileNameAndSetSizes(const TFileName& aResourceDir, 
		const TFileName& aFilenameOutgoing, const TFileName& aFilenameIncoming)
	{
	__ASSERT_ALWAYS(aResourceDir.Length() <= KMaxFileName, Panic(EW32PanicStringTooLong));
	__ASSERT_ALWAYS(aFilenameOutgoing.Length() <= KMaxFileName, Panic(EW32PanicStringTooLong));
	__ASSERT_ALWAYS(aFilenameIncoming.Length() <= KMaxFileName, Panic(EW32PanicStringTooLong));
	
	iDirPathSizePaded = PaddedValue(aResourceDir.Size());
	iFileName1SizePaded = PaddedValue(aFilenameOutgoing.Size());
	iFileName2SizePaded = PaddedValue(aFilenameIncoming.Size());
	
	iCombSizePaded = iDirPathSizePaded + iFileName1SizePaded + iFileName2SizePaded;
	}

/**
Appends folder and file names to wserv client's buffer using AppendData() of MWsClientClass.
AppendData adds data directly to buffer. So before calling AppendData we must make sure that
current command is added to buffer. Please see description of MWsClientClass::AppendData()
for more details.

@param aResourceDir directory name of animation description file
@param aFilenameOutgoing File name of Outgoing phase of TFX
@param aFilenameIncoming File name of Incoming phase of TFX
*/
void RTFXEffect::AppendFileNameData(const TFileName& aResourceDir, const TFileName& aFilenameOutgoing, const TFileName& aFilenameIncoming)
	{
	AppendData(aResourceDir.Ptr(), aResourceDir.Size(), EFalse);
    AppendData(aFilenameOutgoing.Ptr(), aFilenameOutgoing.Size(), EFalse);
    AppendData(aFilenameIncoming.Ptr(), aFilenameIncoming.Size(), ETrue);
	}

/**
Writes file names using IPC args along with data related to TWsClCmdRegisterEffect
Out going phase animation file name is sent in second slot of IPC to server
In coming phase animation file name is sent in third slot of IPC to server
Data related to TWsClCmdRegisterEffect and folder name are sent in the normal wserv buffer

@param aForRegister an object of TWsClCmdRegisterEffect filled with data related to RegisterTFXEffect
					If non Empty then this function is called for Register effect
@param aForOverride an object of TWsClCmdOverrideEffect filled with data related to OverrideTFXEffect
					If non Empty then this function is called for Overide effect					 
@param aResourceDir directory name of animation description file
@param aFilenameOutgoing File name of Outgoing phase of TFX
@param aFilenameIncoming File name of Incoming phase of TFX
@param aCalledFrom value from TFXEffect enum reprseting whether called from RWsSession or RWindowbase 	
*/
void RTFXEffect::WriteDataUsingIPC(TWsClCmdRegisterEffect* aForRegister, TWsClCmdOverrideEffect* aForOverride, 
		const TFileName& aResourceDir, const TFileName& aFilenameOutgoing, const TFileName& aFilenameIncoming, TFXEffect aCalledFrom)
	{
	TIpcArgs ipcArgsDesc;
	ipcArgsDesc.Set(1, &aFilenameOutgoing);
	ipcArgsDesc.Set(2, &aFilenameIncoming);
	// If called for RegisterTFXEffect
	if (aForRegister)
		{
		Write(aForRegister, sizeof(*aForRegister), aResourceDir.Ptr(), aResourceDir.Size(), 
				EWsClOpRegisterTFXEffectIPC, &ipcArgsDesc);
		}
	else	// Else called for OverrideTFXEffect
		{
		Write(aForOverride, sizeof(*aForOverride), aResourceDir.Ptr(), aResourceDir.Size(), 
				(aCalledFrom == ETFXSession ? EWsClOpOverrideEffectIPC : EWsWinOpOverrideEffectIPC), &ipcArgsDesc);
		}
	}

/**
Checks if the sum of iCombSizePaded, size of TWsCmdHeader and sent size is less than
the current buffer size.

@param aSize size to be compared with current buffer size
@return ETrue if the combined size if less then or equal to current buffer size
		EFalse if the combined size is greater then current buffer size
 */
TBool RTFXEffect::CheckCombinedSizeWithCurrentBuffer(TInt aSize) const
	{
	return (iCombSizePaded + aSize + sizeof(TWsCmdHeader) <= iBuffer->BufferSize());
	}

/**
Checks the length of sent variables and does as explained below   

Main logic involved in both RegisterTFXEffect() and OverrideTFXEffect() is as follows
First check the sum of all strings 
If it is less then max wserv buffer
 Send unpadded sizes in TWsClCmdRegisterEffect/TWsClCmdOverrideEffect but when we append 
 the data we make sure that we pad it
 Then at server side get the buffer for total length(inlcuding pading)and unpad it and 
 send it to renderstage's RegisterEffect/OverrideEffect function
If it is greater then max wserv buffer 
 Send one string in the wserv buffer as done before ie. pading and unpading
 Other two strings are sent using IPC args in 2 and 3 slot of IPC and do explicit flush
 And at server side get one string from buffer and other 2 from IPC

@param aAction Particular transition to register the animation for.
@param aPurpose The purpose of the window.
@param aResourceDir The name of the directory that contains the animation description files.
@param aFilenameOutgoing The file containing the description of the animation for the outgoing phase of the transition. 
						 Specify KNullDesC for no outgoing phase effect.
@param aFilenameIncoming The file containing the description of the animation for the incoming phase of the transition. 
						 Specify KNullDesC for no incoming phase effect.
@param aAppUid The Application UID this effect applies to. Set to zero to specify that all apps will use default effect.
@param aFlags Flag for the effect. Please see TTfxFlags for values this flag parameter can use.
*/
void RTFXEffect::RegisterTFXEffect(TInt aAction, TInt aPurpose, const TFileName& aResourceDir, 
		const TFileName& aFilenameOutgoing, const TFileName& aFilenameIncoming, TUint aAppUid, TBitFlags aFlags)
	{
	CheckFileNameAndSetSizes(aResourceDir, aFilenameOutgoing, aFilenameIncoming);
	if (CheckCombinedSizeWithCurrentBuffer(sizeof(TWsClCmdRegisterEffect)))
		{
		TWsClCmdRegisterEffect params(aAction, aPurpose, aResourceDir.Size(), aFilenameOutgoing.Size(), aFilenameIncoming.Size(), aAppUid, aFlags);
		// Here we just pass the length of combined strings so that it checks and does flush if needed.
		// Then AppendData actually adds the data to buffer at the end
		Write(&params, sizeof(params), iCombSizePaded, EWsClOpRegisterTFXEffectBuf);
		if (iCombSizePaded > 0)
			AppendFileNameData(aResourceDir, aFilenameOutgoing, aFilenameIncoming);
		}
	else
		{
		TWsClCmdRegisterEffect params(aAction, aPurpose, aResourceDir.Size(), 0, 0, aAppUid, aFlags);
		WriteDataUsingIPC(&params, NULL, aResourceDir, aFilenameOutgoing, aFilenameIncoming, ETFXSession);
		}
	}

/**
Checks the length of sent variables and does as explained in
RTFXEffect::RegisterTFXEffect() API description

@param aAction The particular transition to set the animation for.
@param aPurpose This override only effects the window/layers owned by the application that have the specified purpose.
@param aResourceDir The name of the directory that contains the animation description files.
@param aFilenameOutgoing The file containing the description of the animation for the outgoing phase of the transition. 
						 Specify KNullDesC for no outgoing phase effect.
@param aFilenameIncoming The file containing the description of the animation for the incoming phase of the transition. 
						 Specify KNullDesC for no incoming phase effect.
@param aFlags Flag for the effect. Please see TTfxFlags for values this flag parameter can use.
*/
void RTFXEffect::OverrideTFXEffect(TFXEffect aCalledFrom, TInt aAction, TInt aPurpose, const TFileName& aResourceDir, 
		const TFileName& aFilenameOutgoing, const TFileName& aFilenameIncoming, TBitFlags aFlags)
	{
	CheckFileNameAndSetSizes(aResourceDir, aFilenameOutgoing, aFilenameIncoming);
	if (CheckCombinedSizeWithCurrentBuffer(sizeof(TWsClCmdOverrideEffect)))
		{
		TWsClCmdOverrideEffect params(aAction, aPurpose, aResourceDir.Size(), aFilenameOutgoing.Size(), aFilenameIncoming.Size(), aFlags);
		Write(&params, sizeof(params), iCombSizePaded, (aCalledFrom == ETFXSession ? EWsClOpOverrideEffectBuf : EWsWinOpOverrideEffectBuf));
		if (iCombSizePaded > 0)
			AppendFileNameData(aResourceDir, aFilenameOutgoing, aFilenameIncoming);
		}
	else
		{
		TWsClCmdOverrideEffect params(aAction, aPurpose, aResourceDir.Size(), 0, 0, aFlags);
		WriteDataUsingIPC(NULL, &params, aResourceDir, aFilenameOutgoing, aFilenameIncoming, aCalledFrom);
		}
	}
