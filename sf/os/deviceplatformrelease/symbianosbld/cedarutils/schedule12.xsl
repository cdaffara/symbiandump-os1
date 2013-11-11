<?xml version="1.0"?>
<xsl:stylesheet version = "1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html"/>

<xsl:variable name="version" select="Schedule12/@OS_version"/>
<xsl:variable name="reltype" select="Schedule12/@Release_type"/>
<xsl:variable name="docversion" select="Schedule12/@DocVersion"/>
<xsl:variable name="status" select="Schedule12/@Status"/>
<xsl:template match = "Schedule12">
	<html>
	<head>
	<title>
		Schedule 12 for Symbian OS v<xsl:value-of select="$version"/>
	</title>
	<link href="schedule12.css" type="text/css" rel="stylesheet"/>
	</head>
	<body>
	<I>DocVersion - <xsl:value-of select="$docversion"/></I>
	<p/>
	
	<H1>
	SCHEDULE 12: Symbian OS Component Categorisation
	(i.e. for <xsl:value-of select="$reltype"/> Release <xsl:value-of select="$version"/>)
	<xsl:if test="@Status!='ISSUED'">
	   - Draft for discussion
	</xsl:if>
	</H1>
	
	The table below sometimes refers to alternative component names taken from 
	the Symbian System Model. For your convenience, a diagram of the 
	System Model is included in the release notes to this 
	<xsl:value-of select="$reltype"/> Release. 
	Please note that Schedule 12 forms part of your CKL/NRA/as applicable 
	and the System Model diagram and nomenclature are provided 
	for reference only. 
	In cases of a difference in description between the two, 
	Schedule 12 shall prevail.
	
	<H1>
		Common Symbian Components
	</H1>
	The Common Symbian components for Symbian OS 
	(i.e. Symbian OS v<xsl:value-of select="$version"/>) are:
	
	<table border="1" width="100%">
	<tr>
	<th align="left" width="30%">Components</th>
	<th align="left">Description</th>
	</tr>
	<xsl:apply-templates select="CS"/>
	</table>

	<H1>
		Common Replaceable Components
	</H1>
	The Common Replaceable components for Symbian OS 
	(i.e. Symbian OS v<xsl:value-of select="$version"/>) are:

	<table border="1" width="100%">
	<tr>
	<th align="left" width="30%">Components</th>
	<th align="left">Description</th>
	</tr>
	<xsl:apply-templates select="CR"/>
	</table>

	<H1>
		Optional Symbian Components
	</H1>
	The Optional Symbian components for Symbian OS 
	(i.e. Symbian OS v<xsl:value-of select="$version"/>) are:
	
	<table border="1" width="100%">
	<tr>
	<th align="left" width="30%">Components</th>
	<th align="left">Description</th>
	</tr>
	<xsl:apply-templates select="OS"/>
	</table>

	<H1>
		Optional Replaceable Components
	</H1>
	The Optional Replaceable components for Symbian OS 
	(i.e. Symbian OS v<xsl:value-of select="$version"/>) are:
	
	<table border="1" width="100%">
	<tr>
	<th align="left" width="30%">Components</th>
	<th align="left">Description</th>
	</tr>
	<xsl:apply-templates select="OR"/>
	</table>

	<H1>
		Reference/Test Components
	</H1>
	The Reference/Test components for Symbian OS 
	(i.e. Symbian OS v<xsl:value-of select="$version"/>)
	have been split into the following two sections:

	<p/>
	<p/>
	<B>Reference</B> - Code which is not intended to be included in Licensee Software 
	and Products.  
	If Licensee does include Reference code in Licensee Software or Products then the 
	terms and conditions of Clause 20.2 (Warranty) and Clause 21 (Product Liability) 
	regarding Reference/Test shall apply.
	
	<p/>
	<p/>
	Reference Code for Symbian OS v<xsl:value-of select="$version"/> is as follows:	
	<UL>
	<xsl:apply-templates select="REF"/>
	</UL>

	<p/>
	<p/>
	<B>Test/Tools</B> - Code which cannot be included in Licensee Software and Products.  
	In order to ensure that Licensee can easily identify Test/Tools code, this code has 
	been given the Classification Type T.  

	<p/>
	<p/>
	Test/Tools Code for Symbian OS v<xsl:value-of select="$version"/> is as follows:
	<UL>
	<xsl:apply-templates select="TEST"/>
	</UL>
	
	<p>
	The components listed above under Common Symbian, Common Replaceable, Optional Symbian 
	and Optional Replaceable may also contain code marked Reference/Test which is intended 
	for reference or test purposes only (as appropriate) and Licensee shall ensure that 
	all code marked Reference/Test is handled as set out in this Paragraph 5
	</p>
	
	<xsl:apply-templates select="footnote"/>

	</body>
	</html>
</xsl:template>

<!-- Display the name and the text as a row in a table -->
<xsl:template match="CS|CR|OS|OR">
	<tr>
	<td valign="top">
	<xsl:value-of select="@name"/>
	<xsl:if test="count(system_model)=0">
	   <i> (empty)</i>
	</xsl:if>
	</td>
	<xsl:choose>
		<xsl:when test="@draft='Y'">
		  <td bgcolor="Red">
		  <b>DRAFT DESCRIPTION: </b>
		  <xsl:copy-of select="node()"/>
		  </td>
		</xsl:when>
		<xsl:otherwise>
		  <td>
		  <xsl:copy-of select="node()"/>
		  </td>
		</xsl:otherwise>
	</xsl:choose>
	</tr>
</xsl:template>

<!-- Display just the name as a item in a list -->
<xsl:template match="REF">
	<LI>
	<xsl:value-of select="@name"/> <xsl:value-of select="@more"/>
	<xsl:if test="count(system_model)=0">
	   <i> (empty)</i>
	</xsl:if>
	</LI>
</xsl:template>
<xsl:template match="TEST">
	<LI>
	<xsl:value-of select="@name"/> <xsl:value-of select="@more"/>
	<xsl:if test="count(system_model)=0">
	   <i> (empty)</i>
	</xsl:if>
	</LI>
</xsl:template>

<xsl:template match="footnote">
	<HR/>
	<xsl:copy-of select="node()"/>
	<br/>
</xsl:template>

</xsl:stylesheet>

