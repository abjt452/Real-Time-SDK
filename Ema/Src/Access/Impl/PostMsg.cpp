/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2015. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#include "PostMsg.h"
#include "PostMsgDecoder.h"
#include "PostMsgEncoder.h"
#include "Utilities.h"
#include "GlobalPool.h"
#include "RdmUtilities.h"

using namespace thomsonreuters::ema::access;
using namespace thomsonreuters::ema::rdm;

extern const EmaString& getDTypeAsString( DataType::DataTypeEnum dType );

const EmaString CreateName( "Create" );
const EmaString DeleteName( " | Delete" );
const EmaString ModifyName( " | ModifyPermission" );

PostMsg::PostMsg() :
 Msg(),
 _toString(),
 _postUserRightsString()
{
}

PostMsg::~PostMsg()
{
	if ( _pEncoder )
		g_pool._postMsgEncoderPool.returnItem( static_cast<PostMsgEncoder*>( _pEncoder ) );

	if ( _pDecoder )
		g_pool._postMsgDecoderPool.returnItem( static_cast<PostMsgDecoder*>(_pDecoder) );
}

PostMsg& PostMsg::clear()
{
	if ( _pEncoder )
		_pEncoder->clear();

	return *this;
}

const EmaString& PostMsg::getPostUserRightsAsString() const
{
	UInt16 postUserRights = getPostUserRights();

	_postUserRightsString.set( "PostUserRights are: " );

	if ( postUserRights & CreateEnum )
		_postUserRightsString += CreateName;

	if ( postUserRights & DeleteEnum )
		_postUserRightsString += DeleteName;

	if ( postUserRights & ModifyPermissionEnum )
		_postUserRightsString += ModifyName;

	return _postUserRightsString;
}

Data::DataCode PostMsg::getCode() const
{
	return Data::NoCodeEnum;
}

DataType::DataTypeEnum PostMsg::getDataType() const
{
	return DataType::PostMsgEnum;
}

const EmaString& PostMsg::toString() const
{
	return toString( 0 );
}

const EmaString& PostMsg::toString( UInt64 indent ) const
{
	const PostMsgDecoder* pTempDecoder = static_cast<const PostMsgDecoder*>( _pDecoder );

	addIndent( _toString.clear(), indent++ ).append( "PostMsg" );
	addIndent( _toString, indent, true ).append( "streamId=\"" ).append( pTempDecoder->getStreamId() ).append( "\"" );
	addIndent( _toString, indent, true ).append( "domain=\"" ).append( rdmDomainToString( getDomainType() ) ).append( "\"" );

	if ( pTempDecoder->getComplete() ) addIndent( _toString, indent, true ).append( "Complete" );

	if ( pTempDecoder->getSolicitAck() ) addIndent( _toString, indent, true ).append( "Ack Requested" );

	if ( pTempDecoder->hasSeqNum() )
		addIndent( _toString, indent, true ).append( "seqNum=\"" ).append( pTempDecoder->getSeqNum() ).append( "\"" );

	if ( pTempDecoder->hasPartNum() )
		addIndent( _toString, indent, true ).append( "partNum=\"" ).append( pTempDecoder->getPartNum() ).append( "\"" );

	if ( pTempDecoder->hasPostId() )
		addIndent( _toString, indent, true ).append( "postId=\"" ).append( pTempDecoder->getPostId() ).append( "\"" );

	if ( pTempDecoder->hasPostUserRights() )
		addIndent( _toString, indent, true ).append( "postUserRights=\"" ).append( pTempDecoder->getPostUserRights() ).append( "\"" );

	if ( pTempDecoder->hasPermissionData() )
	{
		EmaString temp;
		hexToString( temp, pTempDecoder->getPermissionData() );
		addIndent( _toString, indent, true ).append( "permissionData=\"" ).append( temp ).append( "\"" );
	}

	addIndent( _toString, indent, true ).append( "publisherIdUserId=\"" ).append( pTempDecoder->getPublisherIdUserId() ).append( "\"" );
	addIndent( _toString, indent, true ).append( "publisherIdUserAddress=\"" ).append( pTempDecoder->getPublisherIdUserAddress() ).append( "\"" );
	
	indent--;
	if ( pTempDecoder->hasMsgKey() )
	{
		indent++;
		if ( pTempDecoder->hasName() )
			addIndent( _toString, indent, true ).append( "name=\"" ).append( pTempDecoder->getName() ).append( "\"" );

		if ( pTempDecoder->hasNameType() )
			addIndent( _toString, indent, true ).append( "nameType=\"" ).append( pTempDecoder->getNameType() ).append( "\"" );

		if ( pTempDecoder->hasServiceId() )
			addIndent( _toString, indent, true ).append( "serviceId=\"" ).append( pTempDecoder->getServiceId() ).append( "\"" );

		if ( pTempDecoder->hasServiceName() )
			addIndent( _toString, indent, true ).append( "serviceName=\"" ).append( pTempDecoder->getServiceName() ).append( "\"" );

		if ( pTempDecoder->hasFilter() )
			addIndent( _toString, indent, true ).append( "filter=\"" ).append( pTempDecoder->getFilter() ).append( "\"" );

		if ( pTempDecoder->hasId() )
			addIndent( _toString, indent, true ).append( "id=\"" ).append( pTempDecoder->getId() ).append( "\"" );
			
		indent--;
		if ( pTempDecoder->hasAttrib() )
		{
			indent++;
			addIndent( _toString, indent, true ).append( "Attrib dataType=\"" ).append( getDTypeAsString( pTempDecoder->getAttribData().getDataType() ) ).append( "\"\n" );

			indent++;
			_toString.append( pTempDecoder->getAttribData().toString( indent ) );
			indent--;

			addIndent( _toString, indent, true ).append( "AttribEnd" );
			indent--;
		}
	}

	if ( pTempDecoder->hasExtendedHeader() )
	{
		indent++;
		addIndent( _toString, indent, true ).append( "ExtendedHeader\n" );

		indent++;

		addIndent( _toString, indent );
		hexToString( _toString, pTempDecoder->getExtendedHeader() );

		indent--;

		addIndent( _toString, indent, true ).append( "ExtendedHeaderEnd" );
		indent--;
	}

	if ( pTempDecoder->hasPayload() )
	{
		indent++;
		addIndent( _toString, indent, true ).append( "Payload dataType=\"" ).append( getDTypeAsString( pTempDecoder->getPayloadData().getDataType() ) ).append( "\"\n" );

		indent++;
		_toString.append( pTempDecoder->getPayloadData().toString( indent ) );
		indent--;

		addIndent( _toString, indent, true ).append( "PayloadEnd" );
		indent--;
	}
			
	addIndent( _toString, indent, true ).append( "PostMsgEnd\n" );

	return _toString;
}

const EmaBuffer& PostMsg::getAsHex() const
{
	return static_cast<const PostMsgDecoder*>( _pDecoder )->getHexBuffer();
}

bool PostMsg::hasSeqNum() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->hasSeqNum();
}

bool PostMsg::hasPostId() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->hasPostId();
}

bool PostMsg::hasPartNum() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->hasPartNum();
}

bool PostMsg::hasPostUserRights() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->hasPostUserRights();
}

bool PostMsg::hasPermissionData() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->hasPermissionData();
}

bool PostMsg::hasServiceName() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->hasServiceName();
}

UInt32 PostMsg::getSeqNum() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->getSeqNum();
}

UInt32 PostMsg::getPostId() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->getPostId();
}

UInt16 PostMsg::getPartNum() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->getPartNum();
}

UInt16 PostMsg::getPostUserRights() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->getPostUserRights();
}

const EmaBuffer& PostMsg::getPermissionData() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->getPermissionData();
}

UInt32 PostMsg::getPublisherIdUserId() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->getPublisherIdUserId();
}

UInt32 PostMsg::getPublisherIdUserAddress() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->getPublisherIdUserAddress();
}

bool PostMsg::getSolicitAck() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->getSolicitAck();
}

bool PostMsg::getComplete() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->getComplete();
}

const EmaString& PostMsg::getServiceName() const
{
	return static_cast<const PostMsgDecoder*>(_pDecoder)->getServiceName();
}

Decoder& PostMsg::getDecoder()
{
	if ( !_pDecoder )
		setDecoder( g_pool._postMsgDecoderPool.getItem() );

	return *_pDecoder;
}

PostMsg& PostMsg::streamId( Int32 id )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	_pEncoder->streamId( id );
	return *this;
}

PostMsg& PostMsg::domainType( UInt16 domainType )
{
	if ( domainType > 255 )
	{
		EmaString temp( "Passed in DomainType is out of range." );
		throwDtuException( domainType, temp );
		return *this;
	}

	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	_pEncoder->domainType( (UInt8)domainType );
	return *this;
}

PostMsg& PostMsg::name( const EmaString& name )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	_pEncoder->name( name );
	return *this;
}

PostMsg& PostMsg::nameType( UInt8 nameType )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	_pEncoder->nameType( nameType );
	return *this;
}

PostMsg& PostMsg::serviceName( const EmaString& name )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	_pEncoder->serviceName( name );
	return *this;
}

PostMsg& PostMsg::serviceId( UInt32 serviceId )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	_pEncoder->serviceId( serviceId );
	return *this;
}

PostMsg& PostMsg::id( Int32 id )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	_pEncoder->identifier( id );
	return *this;
}

PostMsg& PostMsg::filter( UInt32 filter )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	_pEncoder->filter( filter );
	return *this;
}

PostMsg& PostMsg::seqNum( UInt32 seqNum )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	static_cast<PostMsgEncoder*>(_pEncoder)->seqNum( seqNum );
	return *this;
}

PostMsg& PostMsg::postId( UInt32 postId )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	static_cast<PostMsgEncoder*>(_pEncoder)->postId( postId );
	return *this;
}

PostMsg& PostMsg::partNum( UInt16 partNum )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	static_cast<PostMsgEncoder*>(_pEncoder)->partNum( partNum );
	return *this;
}

PostMsg& PostMsg::postUserRights( UInt16 postUserRights )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	static_cast<PostMsgEncoder*>(_pEncoder)->postUserRights( postUserRights );
	return *this;
}

PostMsg& PostMsg::permissionData( const EmaBuffer& permissionData )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	static_cast<PostMsgEncoder*>(_pEncoder)->permissionData( permissionData );
	return *this;
}

PostMsg& PostMsg::publisherId( UInt32 UserId, UInt32 UserAddress )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	static_cast<PostMsgEncoder*>(_pEncoder)->publisherId( UserId, UserAddress );
	return *this;
}

PostMsg& PostMsg::attrib( const ComplexType& data )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	_pEncoder->attrib( data );
	return *this;
}

PostMsg& PostMsg::payload( const ComplexType& data )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	_pEncoder->payload( data );
	return *this;
}

PostMsg& PostMsg::extendedHeader( const EmaBuffer& Buffer )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	static_cast<PostMsgEncoder*>(_pEncoder)->extendedHeader( Buffer );
	return *this;
}

PostMsg& PostMsg::solicitAck( bool ack )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	static_cast<PostMsgEncoder*>(_pEncoder)->solicitAck( ack );
	return *this;
}

PostMsg& PostMsg::complete( bool complete )
{
	if ( !_pEncoder )
		_pEncoder = g_pool._postMsgEncoderPool.getItem();

	static_cast<PostMsgEncoder*>(_pEncoder)->complete( complete );
	return *this;
}
