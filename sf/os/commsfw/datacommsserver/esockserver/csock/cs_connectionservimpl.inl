// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @prototype
 @internalComponent
 @released since 399
*/

#ifndef CS_CONNECTION_SERV_IMPL_INL
#define CS_CONNECTION_SERV_IMPL_INL


/*inline*/ TInt ConnectionServ::CConnectionServImpl::Send(TInt aFunction,const TIpcArgs& aArgs) const
	{
	return iConnectionServ.Send(aFunction,aArgs);
	}

/*inline*/ void ConnectionServ::CConnectionServImpl::SendReceive(TInt aFunction,const TIpcArgs& aArgs,TRequestStatus& aStatus) const
	{
	iConnectionServ.SendReceive(aFunction,aArgs,aStatus);
	}

/*inline*/ TInt ConnectionServ::CConnectionServImpl::SendReceive(TInt aFunction,const TIpcArgs& aArgs) const
	{
	return iConnectionServ.SendReceive(aFunction,aArgs);
	}

/*inline*/ TInt ConnectionServ::CConnectionServImpl::Send(TInt aFunction) const
	{
	return iConnectionServ.Send(aFunction);
	}

/*inline*/ void ConnectionServ::CConnectionServImpl::SendReceive(TInt aFunction,TRequestStatus& aStatus) const
	{
	iConnectionServ.SendReceive(aFunction,aStatus);
	}

/*inline*/ TInt ConnectionServ::CConnectionServImpl::SendReceive(TInt aFunction) const
	{
	return iConnectionServ.SendReceive(aFunction);
	}


#endif // CS_CONNECTION_SERV_IMPL_INL

