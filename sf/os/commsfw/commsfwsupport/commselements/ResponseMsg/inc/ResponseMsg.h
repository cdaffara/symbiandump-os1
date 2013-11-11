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
//

/**
 @file
 @internalTechnology
*/


#if !defined(RESPONSEMSG_H)
#define RESPONSEMSG_H

#include <e32base.h>
#include <elements/metavirtctor.h>
#include <elements/sd_apiextensionclient.h>

namespace Elements
{

/**
	The RResponseMsg class provides a controlled wrapper around an IPC
	client-provided RMessage2.

	@publishedPartner
	@released
*/
class RResponseMsg : protected RMessage2
	{
	friend class TResponseMsgAccessor;

public:
	explicit RResponseMsg() : iInterfaceId(0), iRequestMsgParam(-1), iResponseMsgParam(-1) {}
	IMPORT_C explicit RResponseMsg(const RMessage2& aMessage, TInt aInterfaceId, TInt aRequestMsgParam = -1, TInt aResponseMsgParam = -1);
	IMPORT_C explicit RResponseMsg(const RMessage2& aMessage, const Den::TApiExtIdentification& aInterfaceId, TInt aRequestMsgParam = -1, TInt aResponseMsgParam = -1);
	IMPORT_C TInt InterfaceId() const;
	IMPORT_C TInt ScopeId() const;

	IMPORT_C Meta::SMetaDataNetCtor* ReadClientReqMsg(TDes8& aDstBuff);

	IMPORT_C void Complete(TInt aError);
	IMPORT_C void Complete(Meta::SMetaDataECom& aMsg);
	IMPORT_C void Complete(Meta::SMetaDataECom& aMsg, TDes8& aDstBuff);
	IMPORT_C void Complete(RHandleBase& aHandle); 	       
	IMPORT_C void Panic(const TDesC& aCategory, TInt aReason) const;

	inline TInt GetDesLength(TInt aParam) const
		{
		return RMessage2::GetDesLength(aParam);
		}
	
	inline TInt GetDesLengthL(TInt aParam) const
		{
		return RMessage2::GetDesLengthL(aParam);
		}
		
	inline TInt GetDesMaxLength(TInt aParam) const
		{
		return RMessage2::GetDesMaxLength(aParam);
		}
	
	inline TInt GetDesMaxLengthL(TInt aParam) const
		{
		return RMessage2::GetDesMaxLengthL(aParam);
		}
	
	inline void ReadL(TInt aReadParam, TDes8& aBuf, TInt aOffset=0) const
		{
		RMessage2::ReadL(aReadParam, aBuf, aOffset);
		}

	inline void ReadL(TInt aReadParam, TDes16& aBuf, TInt aOffset=0) const
		{
		RMessage2::ReadL(aReadParam, aBuf, aOffset);
		}

	inline TInt Read(TInt aReadParam, TDes8& aBuf, TInt aOffset=0) const
		{
		return RMessage2::Read(aReadParam, aBuf, aOffset);
		}

	inline TInt Read(TInt aReadParam, TDes16& aBuf, TInt aOffset=0) const
		{
		return RMessage2::Read(aReadParam, aBuf, aOffset);
		}


	inline void WriteL(TInt aWriteParam, const TDesC8& aBuf, TInt aOffset=0) const
		{
		RMessage2::WriteL(aWriteParam, aBuf, aOffset);
		}

	inline void WriteL(TInt aWriteParam, const TDesC16& aBuf, TInt aOffset=0) const
		{
		RMessage2::WriteL(aWriteParam, aBuf, aOffset);
		}

	inline TInt Write(TInt aWriteParam, const TDesC8& aBuf, TInt aOffset=0) const
		{
		return RMessage2::Write(aWriteParam, aBuf, aOffset);
		}

	inline TInt Write(TInt aWriteParam, const TDesC16& aBuf, TInt aOffset=0) const
		{
		return RMessage2::Write(aWriteParam, aBuf, aOffset);
		}

	inline TInt Function() const
		{
		return RMessage2::Function();
		}
	
	inline TInt Int0() const
		{
		return RMessage2::Int0();
		}
	
	inline TInt Int1() const
		{
		return RMessage2::Int1();
		}
	
	inline TInt Int2() const
		{
		return RMessage2::Int2();
		}
	
	inline TInt Int3() const
		{
		return RMessage2::Int3();
		}
	
	inline const TAny* Ptr0() const
		{
		return RMessage2::Ptr0();
		}
	
	inline const TAny* Ptr1() const
		{
		return RMessage2::Ptr1();
		}
	
	inline const TAny* Ptr2() const
		{
		return RMessage2::Ptr2();
		}
	
	inline const TAny* Ptr3() const
		{
		return RMessage2::Ptr3();
		}

	inline TBool IsNull() const
		{
		return RMessage2::IsNull();
		}

	inline TBool HasCapability(TCapability aCapability, const char* aDiagnostic=0) const
		{
		return RMessage2::HasCapability(aCapability, aDiagnostic);
		}
	
private:
	Den::TApiExtIdentification iInterfaceId;
	TInt8 iRequestMsgParam;
	TInt8 iResponseMsgParam;
	};

} // namespace Elements

#endif	// RESPONSEMSG_H

