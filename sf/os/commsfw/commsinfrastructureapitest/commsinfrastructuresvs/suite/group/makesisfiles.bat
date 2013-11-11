@rem
@rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem

copy ..\release\armv5\urel\t_rconnection.exe .
copy ..\release\armv5\urel\t_rsocket.exe .
copy ..\release\armv5\urel\t_rsocketserv.exe .
copy ..\release\armv5\urel\t_rsubconnection.exe .

call makesis t_rconnection.pkg t_rconnection.sis
call makesis t_rsocket.pkg t_rsocket.sis
call makesis t_rsocketserv.pkg t_rsocketserv.sis
call makesis t_rsubconnection.pkg t_rsubconnection.sis

call signsis -s t_rconnection.sis t_rconnection.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_rsocket.sis t_rsocket.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_rsocketserv.sis t_rsocketserv.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_rsubconnection.sis t_rsubconnection.sis zeus.cer SymbianACS.key caforstat
