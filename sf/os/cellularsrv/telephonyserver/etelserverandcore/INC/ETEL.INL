// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/
inline TVersion RTelServer::Version() const
/** Gets the version number of the ETel server.
	
@return The server version and build numbers.
@publishedPartner
@released
*/
	{
	return(TVersion(KEtelMajorVersionNumber,
					KEtelMinorVersionNumber,
					KEtelBuildVersionNumber));
	}

//
// RTelSubSessionBase
//
inline RSessionBase& RTelSubSessionBase::SessionHandle() const
/**
@publishedPartner
@released
*/
	{ 
	return *iTelSession;
	}

inline void RTelSubSessionBase::SetSessionHandle(RSessionBase& aTelSession)
/**
@publishedPartner
@released
*/
	{ 
	iTelSession=&aTelSession;
	}

inline void RTelSubSessionBase::ResetSessionHandle()
/**
@publishedPartner
@released
*/
	{ 
	iTelSession=NULL;
	}

inline TInt RTelSubSessionBase::SubSessionHandle()
/**
@publishedPartner
@released
*/
	{
	return(RSubSessionBase::SubSessionHandle());
	}
