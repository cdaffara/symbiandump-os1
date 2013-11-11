@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



mkdir ..\supl_old
mkdir ..\rrlp_old

xcopy /s /y ..\supl ..\supl_old
xcopy /s /y ..\rrlp ..\rrlp_old

mkdir ..\supl\inc
mkdir ..\supl\src
mkdir ..\rrlp\inc
mkdir ..\rrlp\src

call asn1c supl_asn1.txt -noInit -noOpenExt -compact -nodatestamp -cpp -per -symbian dll -o ..\supl\src -oh ..\supl\inc
call asn1c rrlp_asn1_RJS.txt -noInit -noOpenExt -compact -nodatestamp -cpp -per -symbian dll -o ..\rrlp\src -oh ..\rrlp\inc