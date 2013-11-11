// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SS_PROTBINDER.H
// 
//

/**
 @file 
 @publishedPartner 
*/

#if !defined(SS_PROTBINDER_H_INCLUDED_)
#define SS_PROTBINDER_H_INCLUDED_

#include <es_sock.h>
#include <in_sock.h>

struct TBinderInfo 
	{
	TUint iFeatures;      /** Feature flags */
	TInt  iMtu;           /** Maximum transmission unit. */
	TInt  iRMtu;          /** Maximum transmission unit for receiving. */
	TInt  iSpeedMetric;	  /** approximation of the interface speed in Kbps. */
	private:
	TInt  iReserved1;
	TInt  iReserved2;
	};
	
class TBinderConfig : public Meta::SMetaData
	{
public:
	TUint iFamily;			/** KAfInet / KAfInet6 - selects TBinderConfig4/6 */
	
	template <class CONFIG>
  static CONFIG* Cast(TBinderConfig& aConfig)
		{
		if(aConfig.GetTypeId().iType == CONFIG::ETypeId && aConfig.GetTypeId().iUid == TUid::Uid(CONFIG::EUid))
				{
				CONFIG* config = &static_cast<CONFIG&>(aConfig);
				return config;
				}
		else
				{
				return NULL;
				}
			}
	};

class TBinderConfig4 : public TBinderConfig
	{
public:
	TBinderInfo iInfo;
	TInetAddr iAddress;		/** Interface IP address. */
	TInetAddr iNetMask;		/** IP netmask. */
	TInetAddr iBrdAddr;		/** IP broadcast address. */
	TInetAddr iDefGate;		/** IP default gateway or peer address (if known). */
	TInetAddr iNameSer1;	/** IP primary name server (if any). */
	TInetAddr iNameSer2;	/** IP secondary name server (if any). */
	
	enum 
		{
		EUid = 0x101F7482,
		ETypeId = 1,
		};
	
public:
	EXPORT_DATA_VTABLE_AND_FN
	};
		
class TBinderConfig6 : public TBinderConfig
	{
public:	
	TBinderInfo iInfo;
	TSockAddr iLocalId;
	TSockAddr iRemoteId;
	TInetAddr iNameSer1;
	TInetAddr iNameSer2;
	
	
	enum 
		{
		EUid = 0x101F7482,
		ETypeId = 2,
		};
	
public:
	EXPORT_DATA_VTABLE_AND_FN
	};

#endif	// SS_PROTBINDER_H_INCLUDED_

