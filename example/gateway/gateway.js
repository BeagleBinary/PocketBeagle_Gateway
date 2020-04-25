/******************************************************************************

 @file gateway.js

 @brief local gateway

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
var AppClient = require("./appClient/appclient.js");
var Webserver = require("./webserver/webserver.js");

/* Toggle debug print statements */
const PRINT_DEBUG = false;


/*!
 * @brief      Constructor for local gateway
 *
 * @param      none
 *
 * @retun      none
 */
function Gateway() {
	var applclient = new AppClient();
	var webserver = new Webserver();

	/* rcvd send config req */
	webserver.on('sendConfig', function (data) {
		/* send config request */
		applclient.appC_sendConfig(data);
	});

	/* rcvd send toggle req */
	webserver.on('sendToggle', function (data) {
		/* send toggle request */
		applclient.appC_sendToggle(data);
	});

	/* rcvd getDevArray Req */
	webserver.on('getDevArrayReq', function (data) {
		/* process the request */
		applclient.appC_getDeviceArray();
	});

	/* rcvd getNwkInfoReq */
	webserver.on('getNwkInfoReq', function (data) {
		/* process the request */
		applclient.appC_getNwkInfo();
	});

	webserver.on('setJoinPermitReq', function (data) {
		/* process the request */
		applclient.appC_setPermitJoin(data);
	});

	/* send message to web-client */
	applclient.on('permitJoinCnf', function (data) {
		if(PRINT_DEBUG) console.log("gateway to ws: permitJoinCnf");
		webserver.webserverSendToClient('permitJoinCnf', data);
	});

	/* send connected device info update to web-client */
	applclient.on('connDevInfoUpdate', function (data) {
		if(PRINT_DEBUG) console.log("gateway to ws: connDevInfoUpdate");
		webserver.webserverSendToClient('connDevInfoUpdate', data);
	});

	/* Send Network Information Message to to web-client*/
	applclient.on('nwkInfo', function () {
		if(PRINT_DEBUG) console.log("gateway to ws: nwkInfo");
		webserver.webserverSendToClient('nwkInfo',applclient.nwkInfo);
	});

	/* send device array to web-client */
	applclient.on('getdevArrayRsp', function (data) {
		if(PRINT_DEBUG) console.log("gateway to ws: getdevArrayRsp");
		webserver.webserverSendToClient('getdevArrayRsp', data);
 });

}



/* create gateway */
var gateway = new Gateway();
