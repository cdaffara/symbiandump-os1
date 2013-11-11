<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html>
    
<head>
	<script type="text/javascript">

	function urlEncode (str) { 
	    len = str.length; 
	    res = new String(); 
	    charOrd = new Number(); 
	     
	    for (i = 0; i < len; i++) { 
	        charOrd = str.charCodeAt(i); 
	        if ((charOrd >= 65 && charOrd <= 90) || (charOrd >= 97 && charOrd <= 122) || (charOrd >= 48 && charOrd <= 57) || (charOrd == 33) || (charOrd == 36) || (charOrd == 95)) { 
	            // this is alphanumeric or $-_.+!*'(), which according to RFC1738 we don't escape 
	            res += str.charAt(i); 

	        } 
	        else { 
	            res += '%'; 
	            if (charOrd > 255) res += 'u'; 
	            hexValStr = charOrd.toString(16); 
	            if ((hexValStr.length) % 2 == 1) hexValStr = '0' + hexValStr; 
	            res += hexValStr; 
	        } 
	    } 

	    return res; 
	}
	

</script>

<title>S60 Cross Referencer: mcl_201032_hw79</title>
<meta http-equiv="X-UA-Compatible" content="IE=7">
<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
<link rel="icon" href="/matrix.ico" type="image/x-icon">
<link rel="stylesheet" type="text/css" href="/css/global.css" media="all" />
<link href="/css/lxr.css" rel="STYLESHEET" type="text/css">
<link rel="alternate" type="application/rss+xml" title="Indexed builds feed" href="/rss/all.xml" />
</head>

<body>
<center>
	<div id="header">
		<div id="container">
			<div id="rounded-top" class="lu-round"><div class="ru-round">&nbsp</div><div class="center">&nbsp;</div></div>
		    <div id="h">
				<div id="logo">
					<a href="http://s60lxr.europe.nokia.com/"><img src="/img/LXR_logo2.gif" /></a>
				</div>
				<div id="toolmenu"><a href="http://s60dashboard.europe.nokia.com/">LiveStream</a><br><a href="http://diamonds.europe.nokia.com/diamonds/">Diamonds</a><br><a href="http://gaia.europe.nokia.com/">Gaia</a><br></div>
				<div id="toolmenu2">
					<div id="userinfo"> | <a href="mailto:I_EXT_LXR_ALL@mgd.nokia.com">Feedback</a></div>
					<div id="search-div">
			    		<form method="get" action="/search">
			      			<input type="text" name="string" id="searchinput" class="txt" value="" />
			        		<img class="img" onclick="window.location = '/search?filestring=&string=' + urlEncode(getElementById('searchinput').value);" src="/img/icon_search.gif" style="border-width:0px;" />
			      		</form>
			    	</div>
					<div id="webtools" style="" >by <a href="http://s60dashboard.europe.nokia.com/webtools/">webtools</a></div>
				</div>
			</div>
			<div id="rounded-bottom" class="lb-round"><div class="rb-round">&nbsp</div><div class="center-b">&nbsp;</div></div>
		</div>
	</div>
</center>
	<div id="classification"><p>Company confidential</p></div>
<div id="content">




<table width='100%' border='0' cellpadding='0' cellspacing='0'>
  <tr>
    <td valign='top'>
      <!-- put local logo or links here -->
    </td>
    <td>
      <table width='100%' border='0' cellpadding='0' cellspacing='0'>
        <tr>
          <td align="center"><span class="banner"><a class='banner' href="/source/?v=mcl_201032_hw79">mcl_201032_hw79</a>/ <a class='banner' href="/source/sf/?v=mcl_201032_hw79">sf</a>/ <a class='banner' href="/source/sf/os/?v=mcl_201032_hw79">os</a>/ <a class='banner' href="/source/sf/os/buildtools/?v=mcl_201032_hw79">buildtools</a>/ <a class='banner' href="/source/sf/os/buildtools/toolsandutils/?v=mcl_201032_hw79">toolsandutils</a>/ <a class='banner' href="/source/sf/os/buildtools/toolsandutils/buildsystem/?v=mcl_201032_hw79">buildsystem</a>/ <a class='banner' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/?v=mcl_201032_hw79">extension</a>/ <a class='banner' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/?v=mcl_201032_hw79">converged-comms</a>/ <a class='banner' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79">createcommdbs.mk</a></span></td>
        </tr>
      </table>
    </td>
    <td align='right'>
      
      [&nbsp;<span class='modes-sel'>source navigation</span>&nbsp;]<br>
      [&nbsp;<a class='modes' href="/diff/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79">diff markup</a>&nbsp;]<br>
      [&nbsp;<a class='modes' href="/ident?v=mcl_201032_hw79">identifier search</a>&nbsp;]<br>
      [&nbsp;<a class="modes" href="/search?v=mcl_201032_hw79">general search</a>&nbsp;]<br>
    </td>
  </tr>
  <tr><td colspan='3'>&nbsp;</td></tr>
  <tr>
   <td colspan='3'>
     <table width="100%" border="0" cellpadding='0' cellspacing='3' class="vartable">
       
       <tr valign="top" align="left">
       <th>&nbsp;DFS7x.92 <a href="/rss/DFS7x.92.xml"><img src="/img/feed-icon-14x14.png" class="rsslink" align="right"></a></th><th>&nbsp;MCL - DFS7x.01 <a href="/rss/DFS7x.01.xml"><img src="/img/feed-icon-14x14.png" class="rsslink" align="right"></a></th><th>&nbsp;S60_3_23 <a href="/rss/S60_3_23.xml"><img src="/img/feed-icon-14x14.png" class="rsslink" align="right"></a></th><th>&nbsp;pf_5250 <a href="/rss/pf_5250.xml"><img src="/img/feed-icon-14x14.png" class="rsslink" align="right"></a></th><th>&nbsp;requested <a href="/rss/requested.xml"><img src="/img/feed-icon-14x14.png" class="rsslink" align="right"></a></th></tr>
       <tr valign="top">
         
         <td align="left" style="border-left: thin solid; border-color: lightgray">
           
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=92_201033_hw79">92_201033_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=92ps1_201033_hw79">92ps1_201033_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=92att_201033_hw79">92att_201033_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=92_201032_hw79">92_201032_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=92ps1_201032_hw79">92ps1_201032_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=92att_201032_hw79">92att_201032_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=92_201030_hw79">92_201030_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=92ps1_201030_hw79">92ps1_201030_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=92_201022_hw79">92_201022_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=92_201012_hw79">92_201012_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201002_hw79">mcl_201002_hw79</a>&nbsp;]<br>
           </td>
         <td align="left" style="border-left: thin solid; border-color: lightgray">
           
           &nbsp;[&nbsp;<span class="var-sel">mcl_201032_hw79</span>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201030_hw79">mcl_201030_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201028_hw79">mcl_201028_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201026_hw79">mcl_201026_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201020_hw79">mcl_201020_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201012_hw79">mcl_201012_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201004_hw79">mcl_201004_hw79</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl.01_201002_hw79">mcl.01_201002_hw79</a>&nbsp;]<br>
           </td>
         <td align="left" style="border-left: thin solid; border-color: lightgray">
           
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=S60_3_23_201024_RnD_gadget">S60_3_23_201024_RnD_gadget</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=S60_3_23_201022_RnD_gadget">S60_3_23_201022_RnD_gadget</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=S60_3_23_201010_RnD_gadget">S60_3_23_201010_RnD_gadget</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=S60_3_23_201002_RnD_gadget">S60_3_23_201002_RnD_gadget</a>&nbsp;]<br>
           </td>
         <td align="left" style="border-left: thin solid; border-color: lightgray">
           
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=pf_5250_prd_30.0.029">pf_5250_prd_30.0.029</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=pf_5250_prd_30.0.028">pf_5250_prd_30.0.028</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=pf_5250_prd_30.0.004">pf_5250_prd_30.0.004</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=pf_5250_201012">pf_5250_201012</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=pf_5250_201002">pf_5250_201002</a>&nbsp;]<br>
           </td>
         <td align="left" style="border-left: thin solid; border-color: lightgray">
           
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=NCP79_92_10w33_RC1">NCP79_92_10w33_RC1</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=NCP79_92ps1_10w33_RC1">NCP79_92ps1_10w33_RC1</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=NCP79_92att_10w33_RC1">NCP79_92att_10w33_RC1</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=NCP_TB10.1.10w32.013">NCP_TB10.1.10w32.013</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=NCP79_92_10w32_RC1">NCP79_92_10w32_RC1</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=NCP79_92ps1_10w32_RC1">NCP79_92ps1_10w32_RC1</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=NCP79_92att_10w32_RC1">NCP79_92att_10w32_RC1</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=NCP110_10w32_RC">NCP110_10w32_RC</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=NCP110_10w30_RC2">NCP110_10w30_RC2</a>&nbsp;]<br>
           &nbsp;[&nbsp;<a class='varlink' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=dfs110_11001_201028_hw110">dfs110_11001_201028_hw110</a>&nbsp;]<br>
           </td>
       </tr>
     </table>
   </td>
  </tr>
</table>

<hr>
<pre class="file">
<a name=001 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#001">001</a> <span class="comment"># Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).</span>
<a name=002 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#002">002</a> <span class="comment"></span><span class="comment"># All rights reserved.</span>
<a name=003 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#003">003</a> <span class="comment"></span><span class="comment"># This component and the accompanying materials are made available</span>
<a name=004 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#004">004</a> <span class="comment"></span><span class="comment"># under the terms of "Eclipse Public License v1.0"</span>
<a name=005 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#005">005</a> <span class="comment"></span><span class="comment"># which accompanies this distribution, and is available</span>
<a name=006 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#006">006</a> <span class="comment"></span><span class="comment"># at the URL "<a class='offshore' href="http://www.eclipse.org/legal/epl-v10.html"">http://www.eclipse.org/legal/epl-v10.html"</a>.</span>
<a name=007 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#007">007</a> <span class="comment"></span><span class="comment">#</span>
<a name=008 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#008">008</a> <span class="comment"></span><span class="comment"># Initial Contributors:</span>
<a name=009 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#009">009</a> <span class="comment"></span><span class="comment"># Nokia Corporation - initial contribution.</span>
<a name=010 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#010">010</a> <span class="comment"></span><span class="comment">#</span>
<a name=011 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#011">011</a> <span class="comment"></span><span class="comment"># Contributors:</span>
<a name=012 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#012">012</a> <span class="comment"></span><span class="comment">#</span>
<a name=013 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#013">013</a> <span class="comment"></span><span class="comment"># Description:</span>
<a name=014 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#014">014</a> <span class="comment"></span><span class="comment"># Envoke CED to create correct CommDB</span>
<a name=015 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#015">015</a> <span class="comment"></span><span class="comment"># </span>
<a name=016 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#016">016</a> <span class="comment"></span><span class="comment">#</span>
<a name=017 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#017">017</a> <span class="comment"></span>
<a name=018 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#018">018</a> <a class='fid' href="/ident?v=mcl_201032_hw79;i=do_nothing">do_nothing</a> :
<a name=019 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#019">019</a> 
<a name=020 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#020">020</a> 
<a name=021 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#021">021</a> <span class="comment">#</span>
<a name=022 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#022">022</a> <span class="comment"></span><span class="comment"># The targets invoked by abld </span>
<a name=023 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#023">023</a> <span class="comment"></span><span class="comment">#</span>
<a name=024 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#024">024</a> <span class="comment"></span>
<a name=025 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#025">025</a> <a class='fid' href="/ident?v=mcl_201032_hw79;i=MAKMAKE">MAKMAKE</a> : <a class='fid' href="/ident?v=mcl_201032_hw79;i=do_nothing">do_nothing</a>
<a name=026 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#026">026</a> 
<a name=027 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#027">027</a> <a class='fid' href="/ident?v=mcl_201032_hw79;i=RESOURCE">RESOURCE</a> : <a class='fid' href="/ident?v=mcl_201032_hw79;i=do_nothing">do_nothing</a>
<a name=028 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#028">028</a> 
<a name=029 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#029">029</a> <a class='fid' href="/ident?v=mcl_201032_hw79;i=SAVESPACE">SAVESPACE</a> : <a class='fid' href="/ident?v=mcl_201032_hw79;i=BLD">BLD</a>
<a name=030 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#030">030</a> 
<a name=031 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#031">031</a> <a class='fid' href="/ident?v=mcl_201032_hw79;i=BLD">BLD</a> : <a class='fid' href="/ident?v=mcl_201032_hw79;i=do_nothing">do_nothing</a>
<a name=032 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#032">032</a> 
<a name=033 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#033">033</a> <a class='fid' href="/ident?v=mcl_201032_hw79;i=FREEZE">FREEZE</a> : <a class='fid' href="/ident?v=mcl_201032_hw79;i=do_nothing">do_nothing</a>
<a name=034 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#034">034</a> 
<a name=035 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#035">035</a> <a class='fid' href="/ident?v=mcl_201032_hw79;i=LIB">LIB</a> : <a class='fid' href="/ident?v=mcl_201032_hw79;i=do_nothing">do_nothing</a>
<a name=036 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#036">036</a> 
<a name=037 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#037">037</a> <a class='fid' href="/ident?v=mcl_201032_hw79;i=CLEANLIB">CLEANLIB</a> : <a class='fid' href="/ident?v=mcl_201032_hw79;i=do_nothing">do_nothing</a>
<a name=038 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#038">038</a> 
<a name=039 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#039">039</a> <a class='fid' href="/ident?v=mcl_201032_hw79;i=FINAL">FINAL</a> : 
<a name=040 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#040">040</a>         <a class='fid' href="/ident?v=mcl_201032_hw79;i=perl">perl</a> $(<a class='fid' href="/ident?v=mcl_201032_hw79;i=EXTENSION_ROOT">EXTENSION_ROOT</a>)/<a class='fid' href="/ident?v=mcl_201032_hw79;i=createcommdbs">createcommdbs</a>.<a class='fid' href="/ident?v=mcl_201032_hw79;i=pl">pl</a> --<a class='fid' href="/ident?v=mcl_201032_hw79;i=command">command</a>=<a class='fid' href="/ident?v=mcl_201032_hw79;i=build">build</a> --<a class='fid' href="/ident?v=mcl_201032_hw79;i=platform">platform</a>=$(<a class='fid' href="/ident?v=mcl_201032_hw79;i=PLATFORM">PLATFORM</a>) --<a class='fid' href="/ident?v=mcl_201032_hw79;i=variant">variant</a>=$(<a class='fid' href="/ident?v=mcl_201032_hw79;i=CFG">CFG</a>) --<a class='fid' href="/ident?v=mcl_201032_hw79;i=sourceDir">sourceDir</a>=$(<a class='fid' href="/ident?v=mcl_201032_hw79;i=EXTENSION_ROOT">EXTENSION_ROOT</a>)/$(<a class='fid' href="/ident?v=mcl_201032_hw79;i=SRCDIR">SRCDIR</a>) --<a class='fid' href="/ident?v=mcl_201032_hw79;i=platsec">platsec</a>
<a name=041 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#041">041</a> 
<a name=042 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#042">042</a> <a class='fid' href="/ident?v=mcl_201032_hw79;i=CLEAN">CLEAN</a> : 
<a name=043 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#043">043</a>         <a class='fid' href="/ident?v=mcl_201032_hw79;i=perl">perl</a> $(<a class='fid' href="/ident?v=mcl_201032_hw79;i=EXTENSION_ROOT">EXTENSION_ROOT</a>)/<a class='fid' href="/ident?v=mcl_201032_hw79;i=createcommdbs">createcommdbs</a>.<a class='fid' href="/ident?v=mcl_201032_hw79;i=pl">pl</a> --<a class='fid' href="/ident?v=mcl_201032_hw79;i=command">command</a>=<a class='fid' href="/ident?v=mcl_201032_hw79;i=clean">clean</a> --<a class='fid' href="/ident?v=mcl_201032_hw79;i=platform">platform</a>=$(<a class='fid' href="/ident?v=mcl_201032_hw79;i=PLATFORM">PLATFORM</a>) --<a class='fid' href="/ident?v=mcl_201032_hw79;i=variant">variant</a>=$(<a class='fid' href="/ident?v=mcl_201032_hw79;i=CFG">CFG</a>) --<a class='fid' href="/ident?v=mcl_201032_hw79;i=sourceDir">sourceDir</a>=$(<a class='fid' href="/ident?v=mcl_201032_hw79;i=EXTENSION_ROOT">EXTENSION_ROOT</a>)/$(<a class='fid' href="/ident?v=mcl_201032_hw79;i=SRCDIR">SRCDIR</a>) --<a class='fid' href="/ident?v=mcl_201032_hw79;i=platsec">platsec</a>
<a name=044 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#044">044</a> 
<a name=045 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#045">045</a> <a class='fid' href="/ident?v=mcl_201032_hw79;i=RELEASABLES">RELEASABLES</a> : 
<a name=046 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#046">046</a>         @<a class='fid' href="/ident?v=mcl_201032_hw79;i=perl">perl</a> $(<a class='fid' href="/ident?v=mcl_201032_hw79;i=EXTENSION_ROOT">EXTENSION_ROOT</a>)/<a class='fid' href="/ident?v=mcl_201032_hw79;i=createcommdbs">createcommdbs</a>.<a class='fid' href="/ident?v=mcl_201032_hw79;i=pl">pl</a> --<a class='fid' href="/ident?v=mcl_201032_hw79;i=command">command</a>=<a class='fid' href="/ident?v=mcl_201032_hw79;i=releasables">releasables</a> --<a class='fid' href="/ident?v=mcl_201032_hw79;i=platform">platform</a>=$(<a class='fid' href="/ident?v=mcl_201032_hw79;i=PLATFORM">PLATFORM</a>) --<a class='fid' href="/ident?v=mcl_201032_hw79;i=variant">variant</a>=$(<a class='fid' href="/ident?v=mcl_201032_hw79;i=CFG">CFG</a>) --<a class='fid' href="/ident?v=mcl_201032_hw79;i=sourceDir">sourceDir</a>=$(<a class='fid' href="/ident?v=mcl_201032_hw79;i=EXTENSION_ROOT">EXTENSION_ROOT</a>)/$(<a class='fid' href="/ident?v=mcl_201032_hw79;i=SRCDIR">SRCDIR</a>) --<a class='fid' href="/ident?v=mcl_201032_hw79;i=platsec">platsec</a>
<a name=047 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#047">047</a>         
<a name=048 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#048">048</a> 
<a name=049 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#049">049</a> 
<a name=050 class='fline' href="/source/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79#050">050</a> </pre>
<hr>
<table width="100%" cellpadding="0" border="0">
  <tr valign="middle">
    
    <td align="center" nowrap="nowrap">
      [&nbsp;<span class='modes-sel'>source navigation</span>&nbsp;]</td>
    <td align="center" nowrap="nowrap">
      [&nbsp;<a class='modes' href="/diff/sf/os/buildtools/toolsandutils/buildsystem/extension/converged-comms/createcommdbs.mk?v=mcl_201032_hw79">diff markup</a>&nbsp;]</td>
    <td align="center" nowrap="nowrap">
      [&nbsp;<a class='modes' href="/ident?v=mcl_201032_hw79">identifier search</a>&nbsp;]</td>
    <td align="center" nowrap="nowrap">
      [&nbsp;<a class="modes" href="/search?v=mcl_201032_hw79">general search</a>&nbsp;]</td>
  </tr>
</table>
<hr>
<table width="100%" cellpadding="0" border="0">
  <tr>
    <td align="left">
      This page was automatically generated by the 
      <a href="http://lxr.sf.net/">LXR engine</a>.
      <br>And brought to you by the 
      <a href="http://wikis.in.nokia.com/AMPIwiki/DeliveryTools">Delivery Tools</a>.
    </td>
  </tr>
</table>
</div>
</body>
</html>
Current status:  ready