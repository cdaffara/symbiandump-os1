// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "WapMsgUtils.h"
#include <wapmessage.h>
#include <wapmsgerr.h>
#include <es_wsms.h>
#include <f32file.h>

TInt CSWSWapMsgUtils::GetLocalAddress(HBufC8*& aLocalHost)
/**
Not supported
@internalComponent
@released
@since v8.0
*/
	{
	// Panic client if the buffer isn't null
	__ASSERT_ALWAYS(aLocalHost==NULL, User::Panic(Wap::KWapMsgPanicDescriptor, KErrArgument));

	RArray<Wap::TAddressInfo> myArray;

	TInt ret(0);
	TRAPD(err, ret=CWapMessageUtils::GetLocalAddressesL(myArray))
	if (err)
		{
		return err;
		}
	// GetLocalAddressesL can return KErrNotFound if there is no network interface up
	if (ret)
		{
		return ret;
		}

	TBuf16<256>tempBuf16;
	TBuf8<256>tempBuf8;				  
	TInetAddr* info = &myArray[0].iAddress;
	// Output the address to a descriptor
	info->Output(tempBuf16);
	// Need to convert the buffer to 8 bit;
	tempBuf8.Copy(tempBuf16);
	// Note: Ownership of the allocated buffer is passed to the client
	aLocalHost = HBufC8::New( tempBuf16.Length() );
	if (!aLocalHost)
		{
		return KErrNoMemory;
		}
	// Now copy the 8 bit version of the address
	*aLocalHost = tempBuf8;

	return KErrNone;
	}
			 
void CSWSWapMsgUtils::BuildAddrL(TSockAddr& aAddr, Wap::TBearer aBearer, const TDesC8& aHost, Wap::TPort aPort)
/**
Build a socket address according to the given parameters.
@internalComponent
@released
@since v8.0
@param aAddr the socket address which is built from given parameters
@param aBearer the underlie bearer type
@param aHost the host name
@param aHost the port number
*/
	{
    if (aBearer==Wap::EIP)
        {
        TInetAddr addr(aPort);
        HBufC16* addr16=HBufC16::NewL(aHost.Length());
        TPtr16 addr16Des=addr16->Des();
        addr16Des.Copy(aHost);
        // coverity[check_return]
        addr.Input(addr16Des);
        aAddr=addr;
        delete addr16;
        }
	else
		{
		TWapAddr addr;
		addr.SetWapPort(TWapPortNumber(aPort));
		addr.SetWapAddress(aHost);
		aAddr=addr;
		}
	}
void CSWSWapMsgUtils::AnalyseAddrL(TSockAddr& aAddr, Wap::TBearer aBearer, TDes8& aHost, Wap::TPort& aPort)
/**
Analyse a socket address, and generate host name and port
@internalComponent
@released
@since v8.0
@param aAddr the socket address to be analysed
@param aBearer the underlie bearer type
@param aHost the host name
@param aHost the port number
*/
	{
	aPort=static_cast<Wap::TPort>(aAddr.Port());
	if (aBearer==Wap::EIP)
		{
		TInetAddr addr(aAddr);
		// If the family is KAfInet6, the output buffer must be at least 
		// 39 characters. 
		// If less, the buffer is filled with '*' characters.
		HBufC16* addr16=HBufC16::NewL(40);
		TPtr16 addr16Des=addr16->Des();
		addr.Output(addr16Des);
		aHost.Copy(addr16Des);
		delete addr16;
		}
	else
		{
		TWapAddr& wapAddr = *(reinterpret_cast<TWapAddr*>(&aAddr));
		TPtrC8 addr=wapAddr.WapAddress();
		aHost.Copy(addr);
		}
	}

