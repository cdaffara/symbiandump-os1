// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(MOBILITYMESSAGESECOM_H_INCLUDED)
#define MOBILITYMESSAGESECOM_H_INCLUDED

#include <e32std.h>
#include <comms-infras/metadata.h>
#include <comms-infras/api_ext_msg.h>
#include <comms-infras/mobilitymessages.h>

class MMobilityProtocolResp;

namespace ESock
{

const TInt KMobilityMessagesImplementationUid = 0x102732e5;

/** Types of the mobility message interface represented by CMessage.
@publishedPartner
@released
*/
enum TMobilityMsgImplementations
    {
    EMobilityMessageAny                 = Meta::KNetMetaTypeAny, //Any mobility message type
    EPreferredCarrierAvailable          = 0x00000001,
    ENewCarrierActive                   = 0x00000002,
    };

/**
	Mobility message factory.

	@internalComponent
	@released since v9.2
*/
class CMobilityMsgFactory : public CBase
   {
public:
   static Meta::SMetaDataECom* NewMessageL(TAny* aParams);
   };

class CPreferredCarrierAvailable : public CCommsMobilityApiExtRespMsg
/**
Preferred carrier available message.

@publishedPartner
@released
*/
   {
   friend class CMobilityMsgFactory;

public:
	/**
	 Dispatches the message.
	
	 @param aProtocol The protocol used for dispatching the response message.
	 */
	void DispatchL(MMobilityProtocolResp& aProtocol);

	/**
	 Creates a new CNewCarrierActive response message
	
	 @return A pointer to a message instance if successful, otherwise leaves with error code.
     */
	inline static CPreferredCarrierAvailable* NewL();
	
	inline TAccessPointInfo OldAPInfo() const;
	inline void SetOldAPInfo(TAccessPointInfo aAPInfo);
		  
	/**
	 Returns the new access point information.
	
	 @return The value of iNewAP.	
	 */
	inline TAccessPointInfo NewAPInfo() const;
	/**
	 Sets the new access point information.
	
	 @param aAPInfo The "new access point" information - iNewAP will be set to that value.
	 */
	inline void SetNewAPInfo(TAccessPointInfo aAPInfo);
	
	inline TBool IsUpgrade() const;
	inline void SetIsUpgrade(TBool aIsUpgrade);
   
	/**
	 Returns the value of the seamless flag.
	
	 @return The value of iIsSeamless.	
	 */
	inline TBool IsSeamless() const;
	
	/**
	 Sets the new access point information.
	
	 @param aAPInfo The "new access point" information - iNewAP will be set to that value.
	 */
	inline void SetIsSeamless(TBool aIsSeamless);

	DATA_VTABLE

protected:
	CPreferredCarrierAvailable();

protected:
	TAccessPointInfo iOldAP;
	TAccessPointInfo iNewAP;
	TBool iIsUpgrade;
	/** Flag indicating whether this is an seamless message */
	TBool iIsSeamless;
   };

class CNewCarrierActive : public CCommsMobilityApiExtRespMsg
/**
New carrier active message.

@publishedPartner
@released
*/
   {
   friend class CMobilityMsgFactory;

public:
	/**
	 Dispatches the message.
	
	 @param aProtocol The protocol used for dispatching the response message.
     */
	void DispatchL(MMobilityProtocolResp& aProtocol);
   
	/**
	 Creates a new CNewCarrierActive response message
	
	 @return A pointer to a message instance if successful, otherwise leaves with error code.
     */
	inline static CNewCarrierActive* NewL();
   
	/**
	 Returns the new access point information.
	
	 @return The value of iNewAP.	
	 */	
	inline TAccessPointInfo NewAPInfo() const;
   
	/**
	 Sets the new access point information.
	
	 @param aAPInfo The "new access point" information - iNewAP will be set to that value.
	 */
	inline void SetNewAPInfo(TAccessPointInfo aAPInfo);
   	
	/**
	 Returns the value of the seamless flag.
	
	 @return The value of iIsSeamless.	
	 */
   	inline TBool IsSeamless() const;
	
   	/**
	 Sets the new access point information.
	
	 @param aAPInfo The "new access point" information - iNewAP will be set to that value.
	 */
	inline void SetIsSeamless(TBool aIsSeamless);

   	DATA_VTABLE

protected:
   	CNewCarrierActive();

protected:
   	TAccessPointInfo iNewAP;
	/** Flag indicating whether this is an seamless message */
   	TBool iIsSeamless;
   };

#include <comms-infras/mobilitymessagesecom.inl>

} //namespace ESock

#endif // MOBILITYMESSAGESECOM_H_INCLUDED

