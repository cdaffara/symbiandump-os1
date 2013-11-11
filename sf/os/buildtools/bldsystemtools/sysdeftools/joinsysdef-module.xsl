<?xml version="1.0"?>
 <xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
 	xmlns:exslt="http://exslt.org/common" exclude-result-prefixes="exslt">
<!--Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
	All rights reserved.
	This component and the accompanying materials are made available
	under the terms of "Eclipse Public License v1.0"
	which accompanies this distribution, and is available
	at the URL "http://www.eclipse.org/legal/epl-v10.html".

	Initial Contributors:
	Nokia Corporation - initial contribution.
	Contributors:
	Description:
	XSLT module which contains the logic to join a system definition file
-->
	<!-- save SF namespace as a constant to avoid the risk of typos-->
 <xsl:variable name="defaultns">http://www.symbian.org/system-definition</xsl:variable>
 
<!-- create a stand-alone sysdef from a linked set of fragments -->

<xsl:template match="/*" mode="join">
	<xsl:param name="filename"/>
	<xsl:message terminate="yes">ERROR: Cannot process this document<xsl:if test="$filename !=''"> (<xsl:value-of select="$filename"/>)</xsl:if>
		<xsl:choose>
			<xsl:when test="self::SystemDefinition/@schema">. Unrecognised syntax schema="<xsl:value-of select="@schema"/>"</xsl:when>
			<xsl:when test="self::SystemDefinition">. Missing schema</xsl:when>
			<xsl:otherwise>. Invalid file type: <xsl:value-of select="name()"/></xsl:otherwise>
		</xsl:choose>
	</xsl:message>
</xsl:template>

<!-- anything in schemas 3.0.x won't add new functional attributes that need processing here, just blindly copy them-->

<xsl:template match="/SystemDefinition[starts-with(@schema,'3.0.') and count(*)=1]" mode="join">
	<xsl:param name="origin" select="/.."/>
	<xsl:param name="root"/>
	<xsl:param name="filename"/>
	<xsl:param name="namespaces"/>
	<xsl:param name="data" select="/.."/>
	<xsl:choose>
		<xsl:when test="$origin">	<!-- this sysdef fragment was linked from a parent sysdef -->
			<xsl:for-each select="*"> <!-- can be only one -->
				<xsl:variable name="upid"><xsl:apply-templates select="$origin/@id" mode="my-id"/></xsl:variable>		<!-- namespaceless ID of this in parent doc -->
				<xsl:variable name="id"><xsl:apply-templates select="@id" mode="my-id"/></xsl:variable>						<!-- namespaceless ID of this here -->
				<xsl:variable name="upns"><xsl:apply-templates select="$origin/@id" mode="my-namespace"/></xsl:variable>	<!-- ID's namespace in parent doc -->
				<xsl:variable name="ns"><xsl:apply-templates select="@id" mode="my-namespace"/></xsl:variable>	<!-- ID's namespace -->
				<xsl:if test="$id!=$upid or $ns!=$upns">
					<xsl:message terminate="yes">ERROR: Linked ID "<xsl:value-of select="$id"/>" (<xsl:value-of select="$ns"/>) must match linking document "<xsl:value-of select="$upid"/>" (<xsl:value-of select="$upns"/>)</xsl:message>
				</xsl:if>
				<!-- copy any attributes not already defined (parent doc overrides child doc)-->
				<xsl:for-each select="@*">
					<xsl:variable name="n" select="name()"/>
					<xsl:choose>
						<xsl:when test="$n='id'"/> <!-- never copy this, always set -->
						<xsl:when test="$origin/@*[name()=$n]"> <!-- don't copy if already set -->
							<xsl:message>Note: Cannot set "<xsl:value-of select="$n"/>", already set. Ignoring linked value</xsl:message>
						</xsl:when>
						<xsl:when test="$n='before' or $n='replace'">
							<!-- ensure ns is correct (if any future attribtues will ever use an ID, process it here too)-->
							<xsl:apply-templates select="." mode="join">
								<xsl:with-param name="namespaces" select="$namespaces"/>
							</xsl:apply-templates>
						</xsl:when> 
						<xsl:otherwise><xsl:copy-of select="."/></xsl:otherwise> <!-- just copy anything else -->
					</xsl:choose>
				</xsl:for-each>
				<xsl:copy-of select="../namespace::*[not(.=$namespaces)]"/> <!-- set any namespaces not already set (they should all alreayd be, but some XSLT processors are quirky) -->
			 	<xsl:apply-templates select="$data" mode="overlay-attributes">
					<xsl:with-param name="item" select="current()"/>
				</xsl:apply-templates>
				<xsl:variable name="content">									
					<xsl:apply-templates select="*|comment()" mode="join">
						<xsl:with-param name="root" select="$root"/>
						<xsl:with-param name="filename" select="$filename"/>
						<xsl:with-param name="data" select="$data"/>
						<xsl:with-param name="namespaces" select="$namespaces | ../namespace::*[not(.=$namespaces)]"/>
					</xsl:apply-templates>
				</xsl:variable>
				<xsl:apply-templates select="." mode="is-content-filtered"> <!-- optionally add filtered="yes" if some content has been removed -->
					<xsl:with-param name="content" select="$content"/>
				</xsl:apply-templates>
			 	<xsl:apply-templates select="$data" mode="overlay-meta">
					<xsl:with-param name="item" select="current()"/>
				</xsl:apply-templates>
				<xsl:copy-of select="$content"/>
			</xsl:for-each>
		</xsl:when>
		<xsl:when test="function-available('exslt:node-set')"> <!-- this is the root element of a root sysdef -->
			<!--try to put all namespaces in root element -->
			<xsl:variable name="nss">
				<!-- contains node set of namespaces to add to root element.
					May panic if there are too many single-letter namespaces and this can't create a new one -->
				<xsl:call-template name="needed-namespaces">
					<xsl:with-param name="foundns">
						<xsl:apply-templates select="//*[(self::component or self::collection or self::package or self::layer) and @href]" mode="scan-for-namespaces"/>
					</xsl:with-param>
				</xsl:call-template>
			</xsl:variable>
			<xsl:variable name="ns" select="@id-namespace | namespace::* | exslt:node-set($nss)/*"/>
			<xsl:copy><xsl:copy-of select="@*[name()!='schema']"/><xsl:call-template name="set-schema"/>
				<xsl:apply-templates select="self::*[not(@id-namespace)]" mode="add-id-ns"/>
				<xsl:for-each select="exslt:node-set($nss)/*"> <!-- add namespace definitions -->
					<xsl:attribute name="xmlns:{name()}">
						<xsl:value-of select="."/>
					</xsl:attribute>
				</xsl:for-each>
				<!-- no need to call is-content-filtered, it never will be from this element --> 
				<xsl:apply-templates select="*|comment()" mode="join">
					<xsl:with-param name="namespaces" select="$ns"/>
					<xsl:with-param name="root" select="$root"/>
					<xsl:with-param name="data" select="$data"/>
					<xsl:with-param name="filename" select="$filename"/>
				</xsl:apply-templates>
			</xsl:copy>
		</xsl:when>
		<xsl:otherwise> <!-- can't handle node-set() so put the namespaces in the document instead of the root element-->
			<xsl:variable name="ns" select="@id-namespace | namespace::*"/>
			<xsl:copy><xsl:copy-of select="@*[name()!='schema']"/><xsl:call-template name="set-schema"/>
				<!-- no need to call is-content-filtered, it never will be from this element -->
				<xsl:apply-templates select="*|comment()" mode="join">
					<xsl:with-param name="namespaces" select="$ns"/>
					<xsl:with-param name="root" select="$root"/>
					<xsl:with-param name="data" select="$data"/>
					<xsl:with-param name="filename" select="$filename"/>
				</xsl:apply-templates>
			</xsl:copy>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="*" mode="scan-for-namespaces"/> <!-- just in case of errors, consider replacing by terminate -->
<xsl:template match="*[@href and not(self::meta)]" mode="scan-for-namespaces">
	<!-- produce a list of namespace-prefix namespace pairs separated by newlines, in reverse order found in documents 
		reverse order so we can try to use the first namespace prefix defined if it's available-->
	<xsl:for-each select="document(@href,.)/*">
		<xsl:apply-templates select="//*[(self::component or self::collection or self::package or self::layer) and @href]" mode="scan-for-namespaces"/>
		<xsl:for-each select="//namespace::* | @id-namespace">
			<xsl:value-of select="concat(name(),' ',.,'&#xa;')"/>
		</xsl:for-each>
	</xsl:for-each>			
</xsl:template>

<xsl:template name="needed-namespaces">
	<xsl:param name="foundns"/>
	<xsl:param name="usedpre"/>

	<xsl:if test="$foundns!=''">
		<xsl:variable name="line" select="substring-before($foundns,'&#xa;')"/> <!-- always has trailing newline -->
		<xsl:variable name="name" select="substring-after($line,' ')"/> <!-- namespace prefix -->
		<xsl:variable name="remainder" select="substring-after($foundns,'&#xa;')"/>
		<xsl:variable name="newprefix">
			<xsl:if test="not(contains(concat('&#xa;',$remainder),concat('&#xa;',$line,'&#xa;'))) and
				not(//namespace::*[.=$name] or @id-namespace[.=$name] or (not(@id-namespace) and $defaultns=$name))">
						<xsl:apply-templates select="." mode="ns-prefix">
							<xsl:with-param name="ns" select="$name"/>
							<xsl:with-param name="pre" select="substring-before($line,' ')"/>
							<xsl:with-param name="dontuse" select="$usedpre"/>
						</xsl:apply-templates>
			</xsl:if>
		</xsl:variable>
		<xsl:if test="$newprefix!=''">
			<!-- can treat this as if it were a namespace node -->
			<xsl:element name="{$newprefix}">
				<xsl:value-of select="$name"/>
			</xsl:element>
		</xsl:if>
		<xsl:if test="$remainder!=''">
			<xsl:call-template name="needed-namespaces">
				<xsl:with-param name="foundns" select="$remainder"/>
				<xsl:with-param name="usedpre" select="concat($usedpre,' ',$newprefix,' ')"/>
			</xsl:call-template>
		</xsl:if>
	</xsl:if>
</xsl:template>

<xsl:template match="/SystemDefinition" mode="ns-prefix">
	<!-- should be able to replace this with mechanism that uses the XSLT processor's own ability to generate namespaces -->
	<xsl:param name="ns"/>
	<xsl:param name="pre"/>
	<xsl:param name="dontuse"/>
	<xsl:param name="chars">ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz</xsl:param>
	<xsl:variable name="name" select="substring(substring-after($ns,'http://www.'),1,1)"/>
	<xsl:choose>
		<xsl:when test="$pre!='' and $pre!='id-namespace' and not(//namespace::*[name()=$pre]) and not(contains($dontuse,concat(' ',$pre,' ')))">
			<xsl:value-of select="$pre"/>
		</xsl:when>
		<xsl:when test="$ns='' and $chars=''">
			<xsl:message terminate="yes">ERROR: Cannot create namespace prefix for downstream default namespace in <xsl:value-of select="*/@id"/></xsl:message>
		</xsl:when>
		<xsl:when test="$name!='' and not(contains($dontuse,concat(' ',$name,' ')))"><xsl:value-of select="$name"/></xsl:when>
		<xsl:when test="namespace::*[name()=substring($chars,1,1)] or contains($dontuse,concat(' ',substring($chars,1,1),' '))">
			<xsl:apply-templates mode="ns-prefix">
				<xsl:with-param name="chars" select="substring($chars,2)"/>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="substring($chars,1,1)"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>


<!-- schema handling -->

<xsl:template name="set-schema">
<xsl:attribute name="schema">
	<xsl:apply-templates mode="my-schema" select="/SystemDefinition"/>
</xsl:attribute>
</xsl:template>

<xsl:template name="compare-versions"><xsl:param name="v1"/><xsl:param name="v2"/>
			<xsl:choose>
				<xsl:when test="$v1=$v2"><xsl:value-of select="$v1"/></xsl:when>
				<xsl:when test="substring-before($v1,'.') &gt; substring-before($v2,'.')"><xsl:value-of select="$v1"/></xsl:when>
				<xsl:when test="substring-before($v1,'.') &lt; substring-before($v2,'.')"><xsl:value-of select="$v2"/></xsl:when>
				<xsl:when test="substring-before(substring-after($v1,'.'),'.') &gt; substring-before(substring-after($v2,'.'),'.')"><xsl:value-of select="$v1"/></xsl:when>
				<xsl:when test="substring-before(substring-after($v1,'.'),'.') &lt; substring-before(substring-after($v2,'.'),'.')"><xsl:value-of select="$v2"/></xsl:when>
				<xsl:when test="substring-after(substring-after($v1,'.'),'.') &gt; substring-after(substring-after($v2,'.'),'.')"><xsl:value-of select="$v1"/></xsl:when>
				<xsl:when test="substring-after(substring-after($v1,'.'),'.') &lt; substring-after(substring-after($v2,'.'),'.')"><xsl:value-of select="$v2"/></xsl:when>
				<xsl:otherwise><xsl:value-of select="$v1"/></xsl:otherwise>
			</xsl:choose>
</xsl:template>

<xsl:template name="compare-version-list"><xsl:param name="list"/>
	<xsl:variable name="cur" select="substring-before($list,' ')"/>
	<xsl:variable name="remaining" select="substring-after($list,' ')"/>
	<xsl:choose>
		<xsl:when test="$remaining=''"><xsl:value-of select="$cur"/></xsl:when>
		<xsl:otherwise>
			<xsl:variable name="nextbig">
				<xsl:call-template name="compare-version-list">
					<xsl:with-param name="list" select="$remaining"/>
				</xsl:call-template>
			</xsl:variable>
			<xsl:call-template name="compare-versions">
				<xsl:with-param name="v1" select="$cur"/>
				<xsl:with-param name="v2" select="$nextbig"/>
			</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="/SystemDefinition" mode="my-schema"><xsl:param name="biggest" select="@schema"/>
	<xsl:variable name="linked" select="//*[(self::component or self::collection or self::package or self::layer) and @href]"/>
	<xsl:choose>
		<xsl:when test="not($linked)"> <!-- no need to go further -->
			<xsl:call-template name="compare-versions">
				<xsl:with-param name="v1" select="@schema"/>
				<xsl:with-param name="v2" select="$biggest"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
				<xsl:call-template name="compare-version-list">
					<xsl:with-param name="list">
						<xsl:for-each select="$linked">
						<xsl:call-template name="compare-versions">
							<xsl:with-param name="v1">
								<xsl:apply-templates mode="my-schema" select="document(@href,.)/*"/>
							</xsl:with-param>
							<xsl:with-param name="v2" select="$biggest"/>
						</xsl:call-template>
						<xsl:text> </xsl:text>
					</xsl:for-each>
				</xsl:with-param>
				</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>


<xsl:template match="unit" mode="join">	<xsl:param name="root"/><xsl:param name="filename"/><xsl:param name="data"/>
	 <xsl:variable name="display">
	 	<xsl:apply-templates select="$data" mode="filter">
			<xsl:with-param name="item" select="current()"/>
		</xsl:apply-templates>
	 </xsl:variable>
	 
	 <xsl:if test="$display != 'hide' "> <!-- if hide, remove completely from the output-->	 
		<xsl:element name="{name()}">
			<xsl:apply-templates select="@*" mode="join">
				<xsl:with-param name="root" select="$root"/>
				<xsl:with-param name="filename" select="$filename"/>
			</xsl:apply-templates>
		</xsl:element>
	</xsl:if>
</xsl:template>

<!-- override mode="meta" to translate metadata sections. By default, include -->
<xsl:template match="meta" priority="2" mode="join"><xsl:param name="data"/>
	 <xsl:variable name="display">
	 	<xsl:apply-templates select="$data" mode="filter">
			<xsl:with-param name="item" select="current()"/>
		</xsl:apply-templates>
	 </xsl:variable>
	 
	<xsl:if test="$display != 'hide' "> <!-- if hide, remove completely from the output-->	
		<xsl:apply-templates select="." mode="meta"> 
			<xsl:with-param name="display" select="$display"/>
			<xsl:with-param name="data" select="$data"/>
		</xsl:apply-templates>
	</xsl:if>
</xsl:template>

<xsl:template match="meta[@rel='link-mapping']" priority="3" mode="join"/> <!--these are only used in the joining process, so remove from output -->


<xsl:template match="*" mode="join">
	<xsl:param name="root"/><xsl:param name="filename"/><xsl:param name="namespaces"/><xsl:param name="data"/>
	<!-- get attribtues from overlay -->
	<!-- test for presence, if filtered out, just return -->
	<!-- test for children, if it has some, but they're filtered out, either return or leave as empty, dependening on filter rule
		if had items and now has none, options:
			still has meta: keep / delete
			still has comments: keep / delete 
	 -->
	 <xsl:variable name="display">
	 	<xsl:apply-templates select="$data" mode="filter">
			<xsl:with-param name="item" select="current()"/>
		</xsl:apply-templates>
	 </xsl:variable>
	 
	 <xsl:if test="$display != 'hide' "> <!-- if hide, remove completely from the output-->
		 <xsl:variable name="href">
		 	<xsl:apply-templates select="." mode="link">
				<xsl:with-param name="data" select="$data"/>
			</xsl:apply-templates>
		 </xsl:variable>
	 		 
		<xsl:element name="{name()}"> <!-- use this instead of <copy> so xalan doesn't add extra wrong namespaces -->
			<xsl:apply-templates select="@*" mode="join">
				<xsl:with-param name="namespaces" select="$namespaces"/>
			</xsl:apply-templates>
			<xsl:if test="$display != '' ">
				<!-- custom attribute to indicate how this is to be represented. Blank indicates normal, hide removes from the output (see above), anything else is put in the attribute --> 
				<xsl:attribute name="display"><xsl:value-of select="$display"/></xsl:attribute>
			</xsl:if>
		 	<xsl:apply-templates select="$data" mode="overlay-attributes">
				<xsl:with-param name="item" select="current()"/>
			</xsl:apply-templates>			
			<xsl:choose>
				<xsl:when test="$href !='' ">
					<xsl:variable name="prefixmap" select="ancestor::SystemDefinition/*/meta[@rel='link-mapping']/map-prefix[starts-with($href,@link)]"/>
					<xsl:variable name="origin" select="."/>
					<xsl:apply-templates select="document($href,.)/*" mode="join">
						<xsl:with-param name="origin" select="$origin"/>
						<xsl:with-param name="data" select="$data"/>
						<xsl:with-param name="namespaces" select="$namespaces"/>
						<xsl:with-param name="filename">
							<xsl:call-template name="joinpath">
								<xsl:with-param name="file" select="$filename"/>
								<xsl:with-param name="rel">
									<xsl:choose>
										<xsl:when test="$prefixmap">
											<xsl:value-of select="$prefixmap/@to"/>
											<xsl:value-of select="substring-after($href,$prefixmap/@link)"/>
										</xsl:when>
										<xsl:otherwise>
											<xsl:value-of select="$href"/>
										</xsl:otherwise>
									</xsl:choose>								
								 </xsl:with-param>
							</xsl:call-template>					
						</xsl:with-param>
						<xsl:with-param name="root">
							<xsl:value-of select="$root"/>/<xsl:call-template name="lastbefore">
								<xsl:with-param name="string" select="$href"/>
							</xsl:call-template>
						</xsl:with-param>
					</xsl:apply-templates> 
				</xsl:when>
				<xsl:otherwise>
					<xsl:variable name="content">
						<xsl:apply-templates select="*|comment()" mode="join">
							<xsl:with-param name="root" select="$root"/>
							<xsl:with-param name="filename" select="$filename"/>
							<xsl:with-param name="namespaces" select="$namespaces"/>
							<xsl:with-param name="data" select="$data"/>
						</xsl:apply-templates>
					</xsl:variable>
					<xsl:apply-templates select="." mode="is-content-filtered"> <!-- add filtered="yes" if some content has been removed -->
						<xsl:with-param name="content" select="$content"/>
					</xsl:apply-templates>
				 	<xsl:apply-templates select="$data" mode="overlay-meta">
						<xsl:with-param name="item" select="current()"/>
					</xsl:apply-templates>
					<xsl:copy-of select="$content"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:element>
	</xsl:if>
</xsl:template>

<!-- By default, do nothing. Can override template to add filtered="yes" if need to track what's a placeholder and what's been filtered 
	implement with param name="content"
-->
<xsl:template mode="is-content-filtered" match="*" priority="-2"/>


<xsl:template match="@mrp[starts-with(.,'/')] | @bldFile[starts-with(.,'/')] | @base[starts-with(.,'/')]" mode="join">
	<xsl:copy-of select="."/>
</xsl:template>


<xsl:template match="@mrp|@bldFile|@base" mode="join">	<xsl:param name="root"/><xsl:param name="filename"/>
	<xsl:attribute name="{name()}">
		<xsl:call-template name="joinpath">
			<xsl:with-param name="file" select="$filename"/>
			<xsl:with-param name="rel" select="."/>
		</xsl:call-template>	
	</xsl:attribute>	
</xsl:template>


<xsl:template match="@href" mode="join"/> <!--never copy this into the generated doc, that's the whole point of this module -->

<xsl:template match="@*" mode="my-namespace"> <!-- the namespace of an ID -->
	<xsl:choose>
		<xsl:when test="contains(.,':')">
			<xsl:value-of select="ancestor::*/namespace::*[name()=substring-before(current(),':')]"/>
		</xsl:when>
		<xsl:when test="/SystemDefinition/@id-namespace">
			<xsl:value-of select="/SystemDefinition/@id-namespace"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="$defaultns"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>


<xsl:template match="@*" mode="my-id"> <!-- the ID with namespace prefix removed -->
	<xsl:choose>
		<xsl:when test="contains(.,':')">
			<xsl:value-of select="substring-after(.,':')"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="."/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="*" mode="err-path">
<xsl:if test="../@id">
	<xsl:apply-templates select=".." mode="err-path"/>/</xsl:if>
<xsl:value-of select="@id"/>
</xsl:template>


<xsl:template match="@id|@before|@replace" mode="join">
	<xsl:param name="namespaces"/>
	<!-- this will change the namespace prefixes for all IDs to match the root document -->
	<xsl:variable name="ns">
		<xsl:apply-templates select="." mode="my-namespace"/>
	</xsl:variable>
	<xsl:if test="$ns=''">
		<xsl:message terminate="yes">ERROR: Could not find namespace for <xsl:value-of select="name()"/> "<xsl:value-of select="."/>" in <xsl:apply-templates select="../.." mode="err-path"/>
		<xsl:text>&#xa;</xsl:text>
		</xsl:message>
	</xsl:if>
	<xsl:variable name="prefix" select="name($namespaces[.=$ns])"/>
	<xsl:attribute name="{name()}">
	<xsl:choose>
		<xsl:when test="$prefix = 'id-namespace' or  (not($namespaces[name()='id-namespace']) and $ns=$defaultns)"/> <!-- it's the default namespace, no prefix -->
		<xsl:when test="$prefix='' and contains(.,':')">
			<!-- complex: copy id and copy namespace (namespace should be copied already)-->
			<xsl:value-of select="."/>
		</xsl:when>
		<xsl:when test="$prefix='' and $ns=$defaultns"/> <!-- no prefix and it's the default --> 
		<xsl:when test="$prefix!=''">			<!-- just change the prefix -->
			<xsl:value-of select="concat($prefix,':')"/>
		</xsl:when>
		<xsl:otherwise>
		<xsl:message terminate="yes">ERROR: Joining error in resolving namespace for <xsl:value-of select="name()"/> "<xsl:value-of select="."/>" in <xsl:apply-templates select="../.." mode="err-path"/>
		<xsl:text>&#xa;</xsl:text></xsl:message>
		</xsl:otherwise>
	</xsl:choose>
		<xsl:apply-templates select="." mode="my-id"/>
	</xsl:attribute>
</xsl:template>



<xsl:template match="@*|comment()" mode="join"><xsl:copy-of select="."/></xsl:template>


<!-- path handling follows -->

 <xsl:template name="lastbefore"><xsl:param name="string"/><xsl:param name="substr" select="'/'"/>
        <xsl:if test="contains($string,$substr)">
                <xsl:value-of select="substring-before($string,$substr)"/>
                <xsl:if test="contains(substring-after($string,$substr),$substr)">
	                <xsl:value-of select="$substr"/>
	              </xsl:if>
        <xsl:call-template name="lastbefore">
                <xsl:with-param name="string" select="substring-after($string,$substr)"/>
                <xsl:with-param name="substr" select="$substr"/>
        </xsl:call-template>
        </xsl:if>
</xsl:template>

 <xsl:template name="joinpath"><xsl:param name="file"/><xsl:param name="rel"/>
	<xsl:choose>
		<xsl:when test="(contains($rel,'://') and not(contains(substring-before($rel,'://'),'/'))) or starts-with($rel,'/')"> <!-- absolute URI or absolute path-->
			<xsl:value-of select="$rel"/>
		</xsl:when>
		<xsl:otherwise> <!-- relative link -->
			<xsl:call-template name="reducepath">
				<xsl:with-param name="file">
					<xsl:call-template name="lastbefore">
						<xsl:with-param name="string" select="$file"/>
					</xsl:call-template>
					<xsl:text>/</xsl:text>
					<xsl:value-of select="$rel"/>
				</xsl:with-param>
			</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>
 </xsl:template>

<xsl:template name="reducepath"><xsl:param name="file"/>
	<xsl:call-template name="reducedotdotpath">
    	<xsl:with-param name="file">
			<xsl:call-template name="reducedotpath">
		    	<xsl:with-param name="file" select="$file"/>
		    </xsl:call-template>
		</xsl:with-param>
	</xsl:call-template>
</xsl:template>

<xsl:template name="reducedotdotpath"><xsl:param name="file"/>
	<xsl:choose>
		<xsl:when test="starts-with($file,'../')">
			<xsl:text>../</xsl:text>
			<xsl:call-template name="reducedotdotpath">
        		<xsl:with-param name="file" select="substring($file,4)"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:when test="contains($file,'/../')">							
			<xsl:call-template name="reducepath">
        		<xsl:with-param name="file">
			        <xsl:call-template name="lastbefore">
			                <xsl:with-param name="string" select="substring-before($file,'/../')"/>
			        </xsl:call-template>
			        <xsl:text>/</xsl:text>
					<xsl:value-of select="substring-after($file,'/../')"/>
				</xsl:with-param>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise><xsl:value-of select="$file"/></xsl:otherwise>
	</xsl:choose>
 </xsl:template>

<xsl:template name="reducedotpath"><xsl:param name="file"/>
	<xsl:choose>	
		<xsl:when test="starts-with($file,'./')">
			<xsl:call-template name="reducedotpath">
        		<xsl:with-param name="file" select="substring($file,3)"/>
			</xsl:call-template>
		</xsl:when>

		<xsl:when test="contains($file,'/./')">
			<xsl:call-template name="reducepath">
        		<xsl:with-param name="file">
	                <xsl:value-of select="substring-before($file,'/./')"/>
			        <xsl:text>/</xsl:text>
					<xsl:value-of select="substring-after($file,'/./')"/>
				</xsl:with-param>
			</xsl:call-template>
		</xsl:when>
		<xsl:when test="substring($file,string-length($file) - 1) = '/.'">
           <xsl:value-of select="substring($file,1,string-length($file) - 2)"/>
		</xsl:when>
		<xsl:otherwise><xsl:value-of select="$file"/></xsl:otherwise>
	</xsl:choose>
 </xsl:template>

<!-- overridable templates follow -->


<xsl:template match="*" mode="filter" priority="-9"/> <!-- by default show --> 
<xsl:template match="*" mode="overlay-attributes" priority="-9"/> <!-- by default do nothing --> 
<xsl:template match="*" mode="overlay-meta" priority="-9"/> <!-- by default do nothing --> 
<xsl:template match="/SystemDefinition" mode="add-id-ns" priority="-9"/> <!-- some tools may have an easier job if this were always present, but, by default, assume it can just stay implied -->

<xsl:template match="*" mode="link" priority="-1"> <!-- can be overriden to allow custom changes to href values --> 
<xsl:value-of select="@href"/>
</xsl:template>


<xsl:template match="*" mode="meta" priority="-9"><xsl:param name="data"/><xsl:param name="display"/>
	<xsl:element name="{name()}">
		<xsl:copy-of select="@*[name()!='href']"/> <!-- copy all attributes as is, always drop href -->
		<xsl:choose>
			<xsl:when test="$display='local' and @href and contains(@href,':') and not(starts-with(@href,'file:'))">
				<!-- non-local URL: only want local URLs, so keep href as is-->
				<xsl:copy-of select="@href"/> 
			</xsl:when>
			<xsl:when test="@href">
				<xsl:copy-of select="document(@href,.)/*"/> 
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy-of select="*|comment()"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:element>
</xsl:template>


</xsl:stylesheet>
