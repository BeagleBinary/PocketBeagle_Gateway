/******************************************************************************

 @file webserver.js

 @brief webserver implementation

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

var express = require('express');
var events = require("events");
var socket = require("socket.io");
var http = require("http");


/* Webserver Instance */
var webserverInstance;

/*!
 * @brief      Constructor for web-server
 *
 * @param      none
 *
 * @retun      none
 */
function Webserver() {

    /* There should be only one app client */
	if (typeof webserverInstance !== "undefined") {
		return webserverInstance;
	}

	/* Set up to emit events */
	events.EventEmitter.call(this);
	webserverInstance = this;

	/* Set up webserver */
	var	app = express();
	var	server = http.createServer(app);
	webserverInstance.io = socket.listen(server);
	server.listen( 1310, '0.0.0.0');
	var path = require('path');
	app.use(express.static(path.join(__dirname, '..'+'/public')));
	app.get('/', function(req, res){
		res.sendFile(__dirname + '/collectorApp.html');
	});

    /* Handle socket events */
    webserverInstance.io.sockets.on('connection', function (socket) {

        socket.on('setJoinPermit', function (data) {
            webserverInstance.emit('setJoinPermitReq', data);
        });

        socket.on('getDevArrayReq', function (data) {
            webserverInstance.emit('getDevArrayReq', data);
        });

        socket.on('getNwkInfoReq', function (data) {
            webserverInstance.emit('getNwkInfoReq', data);
        });

        socket.on('sendConfig', function (data) {
            webserverInstance.emit('sendConfig', data);
        });

        socket.on('sendToggle', function (data) {
            webserverInstance.emit('sendToggle', data);
        });

    });

	/**********************************************************************
	 Public method to send Update Messages to the client
    ***********************************************************************/
	webserverInstance.webserverSendToClient = function(msgType, data){
		webserverInstance.io.sockets.emit(msgType, data);
		};

    }

Webserver.prototype.__proto__ = events.EventEmitter.prototype;

module.exports = Webserver;
