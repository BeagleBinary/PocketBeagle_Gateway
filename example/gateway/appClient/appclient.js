/******************************************************************************
 @file appClient.js

 @brief TIMAC-2.0.0 Example Application - appClient Implementation

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/* *********************************************************************
 * Require Modules for connection with TIMAC Application Server
 * ********************************************************************/
var net = require("net");
var fs = require("fs");
var events = require("events");
var Client = require("./devices/Client.js")
var Device = require('./devices/device.js');
var NwkInfo = require('./nwkinfo/nwkinfo.js');
var ByteBuffer = require("bytebuffer");

/* *********************************************************************
 * Defines
 * ********************************************************************/
/* APP Server Port, this should match the port number defined in the
file: appsrv.c */
const APP_SERVER_PORT = 5000;
/* Timeout in ms to attempt to reconnect to app server */
const APP_CLIENT_RECONNECT_TIMEOUT = 5000;
const PKT_HEADER_SIZE = 4;
const PKT_HEADER_LEN_FIELD = 0;
const PKT_HEADER_SUBSYS_FIELD = 2;
const PKT_HEADER_CMDID_FIELD = 3;
const APPSRV_SYS_ID_RPC = 10;

/* Toggle debug print statements */
const PRINT_DEBUG = false;

/* network status */
const NTW_NOT_STARTED  = 1;

/* definitions */
var cmdIds = Object.freeze({
    DEVICE_JOINED_IND: 0,
    DEVICE_LEFT_IND: 1,
    NWK_INFO_IND : 2,
    GET_NWK_INFO_REQ : 3,
    GET_NWK_INFO_RSP : 4,
    GET_NWK_INFO_CNF : 5,
    GET_DEVICE_ARRAY_REQ : 6,
    GET_DEVICE_ARRAY_CNF : 7,
    DEVICE_NOTACTIVE_UPDATE_IND : 8,
    DEVICE_DATA_RX_IND : 9,
    COLLECTOR_STATE_CNG_IND : 10,
    SET_JOIN_PERMIT_REQ : 11,
    SET_JOIN_PERMIT_CNF : 12,
    TX_DATA_REQ : 13,
    TX_DATA_CNF : 14,
    RMV_DEVICE_REQ: 15,
    RMV_DEVICE_RSP: 16,
    DEV_MOVED_IND: 17
});
var Smsgs_dataFields = Object.freeze({
    tempSensor: 0x0001,
    lightSensor: 0x0002,
    humiditySensor: 0x0004,
    msgStats: 0x0008,
    configSettings: 0x0010,
    hallEffectSensor: 0x0020,
    accelSensor: 0x0040,
});

var smgsCmdIds = Object.freeze({
    CONFIG_REQ: 1,
    CONFIG_RSP: 2,
    TRACKING_REQ: 3,
    TRACKING_RSP: 4,
    SENSOR_DATA: 5,
    TOGGLE_REQ: 6,
    TOGGLE_RSP: 7,
});

/* address mode */
const ADDTYPE_SHORT = 2;
const ADDTYPE_EXT = 3;

/* *********************************************************************
 * Variables
 * ********************************************************************/
/* AppClient Instance */
var appClientInstance;


let client = new Client()

async function getUserId(){
    

    try{
        const userId = await client.loginUser();
        
        return userId

    }catch(error){
        console.log("error resolving" + error)
    }

    

}



async function getSensorId(userId){
    

    try{
        const sensorId = await client.createSensor(userId);
        
        return sensorId

    }catch(error){
        console.log("error resolving" + error)
    }


}



/* ********************************************************************
* Initialization Function
**********************************************************************/
/*!
 * @brief      Constructor for appClient
 *
 * @param      none
 *
 * @retun      none
 */
function Appclient() {

    /* There should be only one app client */
    if (typeof appClientInstance !== "undefined") {
        return appClientInstance;
    }

    /* set-up the instance */
    appClientInstance = this;
    /* set-up to emit events */
    events.EventEmitter.call(this);
    /* set-up to connect to app server */
    var appClient = net.Socket();

    /* Start the connection with app Server */
    appClient.connect(APP_SERVER_PORT, '127.0.0.1', function () {
        console.log("Connected to App Server");
        /* Request Network Information */
        appC_getNwkInfoFromAppServer();
    });
    /* set-up callback for incoming data from the app server */
    appClient.on('data', function (data) {
        /* Call the incoming data processing function */
        appC_processIncoming(data);
    });

    /* set-up to handle error event */
    appClient.on('error', function (data) {
		/* connection lost or unable to make connection with the
		appServer, need to get network and device info back again
		as those may have changed */
        console.log("ERROR: Rcvd Error on the socket connection with AppServer");
        if(PRINT_DEBUG) console.log(data);
        appClientReconnect();
    });
    /* Device list array */
    this.connectedDeviceList=[];
    self = this;
    /* Netowrk Information var */
    this.nwkInfo;

	/*
	 * @brief      This function is called to  attempt to reconnect with
	 * 			   the application server
 	 *
 	 * @param      none
 	 *
 	 * @retun      none
	*/
    function appClientReconnect() {
        if (typeof appClientInstance.clientReconnectTimer === 'undefined') {
            /*start a connection timer that tries to reconnect 5s */
            appClientInstance.clientReconnectTimer = setTimeout(function () {
                appClient.destroy();
                appClient.connect(APP_SERVER_PORT, '127.0.0.1', function () {
                    console.log("Connected to App Server");
					/* Request Network Information, we just
					reconnected get info again in case something may
					have changed */
                    appC_getNwkInfoFromAppServer();
                });
                clearTimeout(appClientInstance.clientReconnectTimer);
                delete appClientInstance.clientReconnectTimer;
            }, APP_CLIENT_RECONNECT_TIMEOUT);
        }
    }

	/* *****************************************************************
	* Process Incoming Messages from the App Server
	*******************************************************************/
	/*!
	* @brief        This function is called to handle incoming messages
	*				from the app server
	*
	* @param
	*
	* @return
	*/
    function appC_processIncoming(data) {
        var dataIdx = 0;
        while (dataIdx < data.length) {
            var rx_pkt_len = data[dataIdx + PKT_HEADER_LEN_FIELD] + (data[dataIdx + PKT_HEADER_LEN_FIELD + 1] << 8) + PKT_HEADER_SIZE;
            var rx_pkt_buf = new ByteBuffer(rx_pkt_len, ByteBuffer.LITTLE_ENDIAN);
            rx_pkt_buf.append(data.slice(dataIdx, dataIdx + rx_pkt_len), "hex", 0);
            dataIdx = dataIdx + rx_pkt_len;
            var rx_cmd_id = rx_pkt_buf.readUint8(PKT_HEADER_CMDID_FIELD);

            if(PRINT_DEBUG) console.log("AppClient RX_Pkt_Buffer: ", rx_pkt_buf);

            switch (rx_cmd_id) {
                case cmdIds.DEVICE_JOINED_IND:
                    if(PRINT_DEBUG) console.log('Device Joined Ind');
                    appC_processDeviceJoinedIndMsg(rx_pkt_buf);
                    break;
                case cmdIds.NWK_INFO_IND:
                    if(PRINT_DEBUG) console.log('Network Info Ind');
                    appC_processNetworkUpdateIndMsg(rx_pkt_buf);
                    break;
                case cmdIds.GET_NWK_INFO_CNF:
                    if(PRINT_DEBUG) console.log('Network Info Cnf');
                    appC_processGetNwkInfoCnf(rx_pkt_buf);
                    break;
                case cmdIds.GET_DEVICE_ARRAY_CNF:
                    if(PRINT_DEBUG) console.log('Device Array Cnf');
                    appC_processGetDevArrayCnf(rx_pkt_buf);
                    break;
                case cmdIds.DEVICE_NOTACTIVE_UPDATE_IND:
                    if(PRINT_DEBUG) console.log('Notactive Update Ind');
                    appC_processDeviceNotActiveIndMsg(rx_pkt_buf);
                    break;
                case cmdIds.DEVICE_DATA_RX_IND:
                    if(PRINT_DEBUG) console.log('Data Rx Ind');
                    appC_processDeviceDataRxIndMsg(rx_pkt_buf);
                    break;
                case cmdIds.COLLECTOR_STATE_CNG_IND:
                    if(PRINT_DEBUG) console.log('State Change Ind');
                    appC_processStateChangeUpdate(rx_pkt_buf);
                    break;
                case cmdIds.SET_JOIN_PERMIT_CNF:
                    if(PRINT_DEBUG) console.log('Join Permit Cnf');
                    appC_processSetJoinPermitCnf(rx_pkt_buf);
                    break;
                case cmdIds.TX_DATA_CNF:
                    if(PRINT_DEBUG) console.log('Tx Data Cnf');
                    break;
                default:
                    if(PRINT_DEBUG) console.log('ERROR: cmdId not processed');
            }
        }
    }

    /*!
    * @brief        This function is called to handle incoming network update
    * 				message from the application
    *
    * @param        Incoming msg data buffer
    *
    * @return       none
    */
      function appC_processGetNwkInfoCnf(data){
          data.mark(PKT_HEADER_SIZE);
          data.reset();
          var nInfo = {};
          nInfo.devInfo = {};

          nInfo.status = data.readUint8();
          nInfo.devInfo.panID = data.readUint16();
          nInfo.devInfo.shortAddress = data.readUint16();
          nInfo.devInfo.extAddress = data.readUint64();
          nInfo.channel = data.readUint8();
          nInfo.fh = data.readUint8();
          nInfo.securityEnabled = data.readUint8();
          nInfo.networkMode = data.readUint8();
          nInfo.state = data.readUint8();

          if (nInfo.status != NTW_NOT_STARTED) {
        /* Network not yet started, no nwk info returned
        by app server keep waiting until the server
        informs of the network info via network update indication
              */
              if (PRINT_DEBUG) console.log('Network not started; Waiting for updates.');
              if (PRINT_DEBUG) console.log(nInfo);
              return;
          }
          if (typeof self.nwkInfo === "undefined") {
              /* create a new network info element */
              self.nwkInfo = new NwkInfo(nInfo);
          }
          else {
              /* Update the network information */
              self.nwkInfo.updateNwkInfo(nInfo);
          }
          if(PRINT_DEBUG) console.log(self.nwkInfo);

          /* Send update to web-client */
          appClientInstance.emit('nwkInfo');
          /* Get device array from appsrv */
          appC_getDevArrayFromAppServer();
      }

    /*!
  	* @brief        This function is called to handle incoming device array
  	* 				cnf message from the application
  	*
  	* @param        data - Incoming msg data buffer
  	*
  	* @return       none
  	*/
      function appC_processGetDevArrayCnf(data) {
          /* Erase the exsisting infomration we will update
  		information with the incoming information */
          self.connectedDeviceList = [];
          data.mark(PKT_HEADER_SIZE);
          data.reset();
          var status = data.readUint8();
          var n = data.readUint16();

          var i;
          for(i = 0; i < n; i++){
              var panId = data.readUint16();
              var shortAddress = data.readUint16();
              var extendedAddress = data.readUint64();

              var capInfo = {};
              capInfo.panCoord = data.readUint8();
              capInfo.ffd = data.readUint8();
              capInfo.mainsPower = data.readUint8();
              capInfo.rxOnWhenIdle = data.readUint8();
              capInfo.security = data.readUint8();
              capInfo.allocAddr = data.readUint8();

              if(PRINT_DEBUG) console.log("in apps, calling Device()?");
              var newDev = new Device(shortAddress, extendedAddress, capInfo);
              self.connectedDeviceList.push(newDev);
          }
          if(PRINT_DEBUG) console.log("Emit: getdevArrayRsp");
          if(PRINT_DEBUG) console.log(self.connectedDeviceList);
          appClientInstance.emit('getdevArrayRsp', self.connectedDeviceList);
      }
    /*!
  	* @brief        This function is called to handle incoming network update
  	* 				ind message from the application
  	*
  	* @param        Incoming msg data buffer
  	*
  	* @return       none
  	*/
      function appC_processNetworkUpdateIndMsg(data) {
          data.mark(PKT_HEADER_SIZE);
          data.reset();
          /* TODO: Make this simpler by removing nested objects */
          var nInfo = {};
          nInfo.devInfo = {};

          nInfo.devInfo.panID = data.readUint16();
          nInfo.devInfo.shortAddress = data.readUint16();
          nInfo.devInfo.extAddress = data.readUint64();
          nInfo.channel = data.readUint8();
          nInfo.fh = data.readUint8();
          nInfo.securityEnabled = data.readUint8();
          nInfo.networkMode = data.readUint8(),
          nInfo.state = data.readUint8()

          if (typeof self.nwkInfo === "undefined") {
              /* Create a new network info element */
              self.nwkInfo = new NwkInfo(nInfo);
          }
          else {
              /* Update the network information */
              self.nwkInfo.updateNwkInfo(nInfo);
          }
          if(PRINT_DEBUG) console.log(self.nwkInfo);
          /* Send update to web-client */
          appClientInstance.emit('nwkInfo');
      }
    /*!
  	* @brief        This function is called to handle incoming device joined
  	* 				ind message informing of new device join from the
  	* 				application
  	*
  	* @param 		Incoming msg data buffer
  	*
  	* @return       none
  	*/
      function appC_processDeviceJoinedIndMsg(data) {
          data.mark(PKT_HEADER_SIZE);
          data.reset();
          var panID = data.readUint16();

          var shortAddress = data.readUint16();
          var extendedAddress = data.readUint64();

          var capInfo = {}
          capInfo.panCoord = data.readUint8();
          capInfo.ffd = data.readUint8();
          capInfo.mainsPower = data.readUint8();
          capInfo.rxOnWhenIdle = data.readUint8();
          capInfo.security = data.readUint8();
          capInfo.allocAddr = data.readUint8();

          /* Check if the device already exists */
          for (var i = 0; i < self.connectedDeviceList.length; i++) {

            var inarray = self.connectedDeviceList[i].extAddress.high.toString() + self.connectedDeviceList[i].extAddress.low.toString();
            var newone = extendedAddress.high.toString() + extendedAddress.low.toString();
            /* check if extended address match */
            if (inarray == newone) {
                console.log("the same one");
                 self.connectedDeviceList[i].devUpdateInfo(shortAddress, extendedAddress, capInfo);
                /* send update to web client */
                if(PRINT_DEBUG) console.log(self.connectedDeviceList[i]);
                appClientInstance.emit('connDevInfoUpdate', self.connectedDeviceList[i]);
                return;
            }              
          }
          // it is a new device  
          console.log("new one");
          var newDev = new Device(shortAddress, extendedAddress, capInfo);
          var devIndex = self.connectedDeviceList.push(newDev) - 1;

          if(PRINT_DEBUG) console.log(newDev);

          /* Send update to web-client */
          appClientInstance.emit('connDevInfoUpdate', self.connectedDeviceList[devIndex]);
      }

    /*!
  	* @brief        This function is called to handle incoming message informing that
  	* 				 a device is now inactive(?)
  	*
  	* @param 		data - Incoming msg data buffer
  	*
  	* @return       none
  	*/
      function appC_processDeviceNotActiveIndMsg(data) {
          data.mark(PKT_HEADER_SIZE);
          data.reset();
          var panID = data.readUint16();

          var inactivedeviceInfo = {};
          inactivedeviceInfo.shortAddress = data.readUint16();
          inactivedeviceInfo.extAddress = data.readUint64();
          inactivedeviceInfo.timeout = data.readUint8();

          var deviceIdx = findDeviceIndexFromAddr(inactivedeviceInfo.shortAddress);
          if (deviceIdx !== -1) {
            if(PRINT_DEBUG) console.log("in app, calling deviceNotActive()?");
              self.connectedDeviceList[deviceIdx].deviceNotActive(inactivedeviceInfo);
              /* Update the web-client */
              appClientInstance.emit('connDevInfoUpdate', self.connectedDeviceList[deviceIdx]);
          }
          else {
              if(PRINT_DEBUG) console.log("ERROR: rcvd inactive status info for non-existing device");
          }
      }

        /*!
    	* @brief        This function is called to handle incoming message informing of
    	* 				reception of sensor data message/device config resp
    	*				from a network device
    	*
    	* @param 		data - Incoming msg data buffer
    	*
    	* @return       none
    	*/

        function appC_processDeviceDataRxIndMsg(data){
            data.mark(PKT_HEADER_SIZE);
            data.reset();
            var deviceIdx = -1;
            var deviceData = {};
            deviceData.srcAddr = {};
            deviceData.srcAddr.addrMode = data.readUint8();
            if (deviceData.srcAddr.addrMode == ADDTYPE_EXT) {
                deviceData.srcAddr.extAddr = data.readUint64();
                var tempExtAddr = deviceData.srcAddr.extAddr.extAddress.high.toString() + deviceData.srcAddr.extAddr.low.toString();
                deviceIdx = findDeviceIndexFromAddr(tempExtAddr);
            }
            else if (deviceData.srcAddr.addrMode == ADDTYPE_SHORT) {
                deviceData.srcAddr.shortAddr = data.readUint16();
                deviceIdx = findDeviceIndexFromAddr(deviceData.srcAddr.shortAddr);
            }
            else {
                console.log("unknown addr mode: " + deviceData.srcAddr.addrMode);
                return;
            }
            /* Read signed rssi */
            deviceData.rssi = data.readInt8();

            deviceData.cmdId = data.readUint8();
            console.log("CMD ID: ", deviceData.cmdId);

            /* Sensor data msg received */
            if (deviceData.cmdId == smgsCmdIds.SENSOR_DATA){
                deviceData.extAddr = data.readUint64();
                deviceData.frameControl = data.readUint16();
                console.log("Frame Control: ", deviceData.frameControl);
                /* Temperature sensor data received */
                if(deviceData.frameControl & Smsgs_dataFields.tempSensor){
                    deviceData.tempSensor = {};
                    deviceData.tempSensor.ambienceTemp = data.readUint16();
                    deviceData.tempSensor.objectTemp = data.readUint16();



                    getUserId().then(function(result) {
                      this.userId = result
                      client.sendWebsockets(deviceData.tempSensor.ambienceTemp)
                      console.log("Sent temp data of " + deviceData.tempSensor.ambienceTemp);
                    });


                }
                /* Light sensor data received */
                if(deviceData.frameControl & Smsgs_dataFields.lightSensor){
                    deviceData.lightSensor = {};
                    deviceData.lightSensor.rawData = data.readUint16();


                    getUserId().then(function(result) {
                      this.userId = result
                      client.sendWebsockets(deviceData.lightSensor.rawData)
                      console.log("Sent light data of " + deviceData.lightSensor.rawData);
                    });
                }
                /* Humididty sensor data received */
                if(deviceData.frameControl & Smsgs_dataFields.humiditySensor){
                    deviceData.humiditySensor = {};
                    deviceData.humiditySensor.temp = data.readUint16();
                    deviceData.humiditySensor.humidity = data.readUint16();


                    getUserId().then(function(result) {
                      this.userId = result
                      client.sendWebsockets(deviceData.humiditySensor.humidity)
                      console.log("Sent humidity data of " + deviceData.humiditySensor.humidity);
                    });
                }
		          if(deviceData.frameControl & Smsgs_dataFields.hallEffectSensor)
		          {
                    deviceData.hallEffectSensor = {};
                    deviceData.hallEffectSensor.flux = data.readFloat();
	                 console.log(deviceData.hallEffectSensor.flux);

                    getUserId().then(function(result) {
                      this.userId = result
                      client.sendWebsockets(deviceData.hallEffectSensor.flux)
                      console.log("Sent hall effect data of " + deviceData.hallEffectSensor.flux);
                    });
		          }
                /* Msg Stats recieved */
                if(deviceData.frameControl & Smsgs_dataFields.msgStats){
                    deviceData.msgStats = {};
                    deviceData.msgStats.joinAttempts = data.readUint16();
                    deviceData.msgStats.joinFails = data.readUint16();
                    deviceData.msgStats.msgsAttempted = data.readUint16();
                    deviceData.msgStats.msgsSent = data.readUint16();
                    deviceData.msgStats.trackingRequests = data.readUint16();
                    deviceData.msgStats.trackingResponseAttempts = data.readUint16();
                    deviceData.msgStats.trackingResponseSent = data.readUint16();
                    deviceData.msgStats.configRequests = data.readUint16();
                    deviceData.msgStats.configResponseAttempts = data.readUint16();
                    deviceData.msgStats.configResponseSent = data.readUint16();
                    deviceData.msgStats.channelAccessFailures = data.readUint16();
                    deviceData.msgStats.macAckFailures = data.readUint16();
                    deviceData.msgStats.otherDataRequestFailures = data.readUint16();
                    deviceData.msgStats.syncLossications = data.readUint16();
                    deviceData.msgStats.rxDecryptFailures = data.readUint16();
                    deviceData.msgStats.txEncryptFailures = data.readUint16();
                    deviceData.msgStats.resetCount = data.readUint16();
                    deviceData.msgStats.lastResetReason = data.readUint16();
                    deviceData.msgStats.joinTime = data.readUint16();
                    deviceData.msgStats.interimDelay = data.readUint16();
                }
                /* Config Settings recieved */
                if(deviceData.frameControl & Smsgs_dataFields.configSettings){
                    deviceData.configSettings = {};
                    deviceData.configSettings.reportingInterval = data.readUint32();
                    deviceData.configSettings.pollingInterval = data.readUint32();
                }

                /* Check to see if device is known*/
                if (deviceIdx !== -1) {
                    self.connectedDeviceList[deviceIdx].rxSensorData(deviceData);
                   
                    if(PRINT_DEBUG) console.log("Emit: connDevInfoUpdate - rxData");
                    if(PRINT_DEBUG) console.log(self.connectedDeviceList[deviceIdx]);
                    /* Update the web-client */
                    appClientInstance.emit('connDevInfoUpdate', self.connectedDeviceList[deviceIdx]);
                }
                else {
                    if(PRINT_DEBUG) console.log('ERROR: Sensor data msg received from unknown device');
                    return;
                }
            }

            /* Sensor config msg received */
            if (deviceData.cmdId == smgsCmdIds.CONFIG_RSP){
                deviceData.status = data.readUint16();
                deviceData.frameControl = data.readUint16();
                deviceData.reportingInterval = data.readUint32();
                deviceData.pollingInterval = data.readUint32();

                /* Check to see if device is known*/
                if (deviceIdx !== -1) {
                    if(PRINT_DEBUG) console.log("in app, calling rxConfigRspInd()?");
                    self.connectedDeviceList[deviceIdx].rxConfigRspInd(deviceData);

                    /* Update the web-client */
                    if(PRINT_DEBUG) console.log(self.connectedDeviceList[deviceIdx]);
                    appClientInstance.emit('connDevInfoUpdate', self.connectedDeviceList[deviceIdx]);
                }
                else {
                    if(PRINT_DEBUG) console.log('ERROR: Sensor config response msg received from unknown device');
                }
            }
        }

    /*!
  	* @brief        This function is called to handle incoming message informing change
  	* 				in the state of the PAN-Coordiantor
  	*
  	* @param 		data - Incoming msg data buffer
  	*
  	* @return       none
  	*/
    function appC_processStateChangeUpdate(data) {
          var nState = {state: data.readUint8(4)};
          if(PRINT_DEBUG) console.log(nState);
          /* update state */
          self.nwkInfo.updateNwkState(nState);
          /* send info to web client */
          appClientInstance.emit('nwkInfo');
    }

    /*!
  	* @brief        This function is called to handle incoming confirm for
  	*				setjoinpermitreq
  	*
  	* @param 		data - Incoming msg data buffer
  	*
  	* @return       none
  	*/
    function appC_processSetJoinPermitCnf(data) {
          var cnfStatus = data.readUint32(PKT_HEADER_SIZE);
          if (PRINT_DEBUG) console.log('JoinPermit Success!');
          appClientInstance.emit('permitJoinCnf', { status: cnfStatus });
    }

	/************************************************************************
	 * Device list utility functions
	 * *********************************************************************/

    /*!
	* @brief        Find index of device in the list based on short/ext address
	*
	* @param 		srcAddr - short or ext address of the device
	*
	* @return      index of the device in the connected device list, if present
	*			   -1, if not present
	*
	*/
    function findDeviceIndexFromAddr(srcAddr) {
        /* find the device in the connected device list and update info */
        for (var i = 0; i < self.connectedDeviceList.length; i++) {
            var inarrayExtAdd = self.connectedDeviceList[i].extAddress.high.toString() + self.connectedDeviceList[i].extAddress.low.toString();
            if (self.connectedDeviceList[i].shortAddress == srcAddr
                || inarrayExtAdd == srcAddr) {
                console.log("already exists");
                return i;
            }
        }
        return -1;
    }


	/*****************************************************************
	Functions to send messages to the app server
	*****************************************************************/

        /*!
      	* @brief        Send Config req message to application server
      	*
      	* @param 		data - Contains device address to send req to
      	*
      	* @return       none
      	*/
        function appC_sendConfigReqToAppServer(data) {
              if (!data.dstAddr) {
                  if (PRINT_DEBUG) console.log("sendConfigReqToAppServer: invalid data.");
                  if (PRINT_DEBUG) console.log(data);
                  return;
              }
              var deviceIdx = findDeviceIndexFromAddr(data.dstAddr);
              if(deviceIdx != -1){
                  var len = 9;
                  var msg_buf = new ByteBuffer(PKT_HEADER_SIZE + len, ByteBuffer.LITTLE_ENDIAN);
                  msg_buf.writeShort(len, PKT_HEADER_LEN_FIELD);
                  msg_buf.writeUint8(APPSRV_SYS_ID_RPC, PKT_HEADER_SUBSYS_FIELD);
                  msg_buf.writeUint8(cmdIds.TX_DATA_REQ, PKT_HEADER_CMDID_FIELD);
                  msg_buf.mark(PKT_HEADER_SIZE);
                  msg_buf.reset();
                  msg_buf.writeUint8(smgsCmdIds.CONFIG_REQ);
                  msg_buf.writeUint16(self.connectedDeviceList[deviceIdx].shortAddress);
                  msg_buf.writeUint16(parseInt(data.pollingInterval));
                  msg_buf.writeUint16(parseInt(data.reportingInterval));
                  msg_buf.writeUint16(parseInt(data.framecontrol));

                  if(PRINT_DEBUG) console.log("Sent config request");

                  appClient.write(msg_buf.buffer);
              }

        }

        /*!
      	* @brief        Send Config req message to application server
      	*
      	* @param 		data - Contains device address to send req to
      	*
      	* @return       none
      	*/
          function appC_sendToggleLedMsgToAppServer(data) {
              //Find index of ext address
              if (!data.dstAddr) {
                  if (PRINT_DEBUG) console.log("appC_sendToggleLedMsgToAppServer: invalid data.");
                  if (PRINT_DEBUG) console.log(data);
                  return;
              }
              var deviceIdx = findDeviceIndexFromAddr(data.dstAddr);
              if(deviceIdx != -1){
                  var len = 5;
                  var msg_buf = new ByteBuffer(PKT_HEADER_SIZE + len, ByteBuffer.LITTLE_ENDIAN);
                  msg_buf.writeShort(len, PKT_HEADER_LEN_FIELD);
                  msg_buf.writeUint8(APPSRV_SYS_ID_RPC, PKT_HEADER_SUBSYS_FIELD);
                  msg_buf.writeUint8(cmdIds.TX_DATA_REQ, PKT_HEADER_CMDID_FIELD);
                  msg_buf.mark(PKT_HEADER_SIZE);
                  msg_buf.reset();
                  msg_buf.writeUint8(smgsCmdIds.TOGGLE_REQ);
                  msg_buf.writeUint16(self.connectedDeviceList[deviceIdx].shortAddress);

                  appClient.write(msg_buf.buffer);
                  if(PRINT_DEBUG) console.log("Sent toggle request");
              }
          }

    /*!
  	* @brief        Send get network Info Req to application server
  	*
  	* @param 		none
  	*
  	* @return       none
  	*/
      function appC_getNwkInfoFromAppServer() {
          var len = 0;
          var msg_buf = new ByteBuffer(PKT_HEADER_SIZE + len, ByteBuffer.LITTLE_ENDIAN);
          msg_buf.writeShort(len, PKT_HEADER_LEN_FIELD);
          msg_buf.writeUint8(APPSRV_SYS_ID_RPC, PKT_HEADER_SUBSYS_FIELD);
          msg_buf.writeUint8(cmdIds.GET_NWK_INFO_REQ, PKT_HEADER_CMDID_FIELD);
          appClient.write(msg_buf.buffer);
          if(PRINT_DEBUG) console.log("Get network info req sent to collector");
    }


    /*!
  	* @brief        Send get device array Req to application server
  	*
  	* @param 		none
  	*
  	* @return       none
  	*/
      function appC_getDevArrayFromAppServer() {
          var len = 0;
          var msg_buf = new ByteBuffer(PKT_HEADER_SIZE + len, ByteBuffer.LITTLE_ENDIAN);
          msg_buf.writeShort(len, PKT_HEADER_LEN_FIELD);
          msg_buf.writeUint8(APPSRV_SYS_ID_RPC, PKT_HEADER_SUBSYS_FIELD);
          msg_buf.writeUint8(cmdIds.GET_DEVICE_ARRAY_REQ, PKT_HEADER_CMDID_FIELD);
          appClient.write(msg_buf.buffer);
          if(PRINT_DEBUG) console.log("Sent get device array req");
      }

    /*!
  	* @brief        Send join permit Req to application server
  	*
  	* @param 		data - contains info about action required
  	*					"open" - open network for device joins
  	*				    "close"- close netwwork for device joins
  	*
  	* @return       none
  	*/
      function appC_setJoinPermitAtAppServer(data) {
          var duration = 0x0;
          if (data.action == "open") {
              /* Set always open value */
              duration = 0xFFFFFFFF;
          }
          else {
              /* Set always close value */
              duration = 0x0;
          }
          /* Create the message */
          var len = 4;
          var msg_buf = new ByteBuffer(PKT_HEADER_SIZE + len, ByteBuffer.LITTLE_ENDIAN);
          msg_buf.writeShort(len, PKT_HEADER_LEN_FIELD);
          msg_buf.writeUint8(APPSRV_SYS_ID_RPC, PKT_HEADER_SUBSYS_FIELD);
          msg_buf.writeUint8(cmdIds.SET_JOIN_PERMIT_REQ, PKT_HEADER_CMDID_FIELD);
          msg_buf.writeUint32(duration,PKT_HEADER_SIZE);
          /* Send the message */
          appClient.write(msg_buf.buffer);
          if(PRINT_DEBUG) console.log("Sending join permit");
      }

	/*!
	* @brief        Allows to request for network
	*				information
	*
	* @param 		none
	*
	* @return       network information
	*/
    Appclient.prototype.appC_getNwkInfo = function () {
        /* send the netwiork information */
        appClientInstance.emit('nwkInfo');
    };

	/*!
	* @brief        Allows to request for device array
	*				information
	*
	* @param 		none
	*
	* @return       connected device list
	*/
    Appclient.prototype.appC_getDeviceArray = function () {
          /* Update the web-client */
          appClientInstance.emit('getdevArrayRsp', self.connectedDeviceList);
    };

	/*!
	* @brief        Allows to modify permit join setting for the network
	*
	* @param 		none
	*
	* @return       data - containinfo about action required
	*					"open" - open network for device joins
	*				    "close"- close netwwork for device joins
	*/
    Appclient.prototype.appC_setPermitJoin = function (data) {
        appC_setJoinPermitAtAppServer(data);
    }

	/*!
	* @brief        Allows send toggle command to a network device
	*
	* @param 		none
	*
	* @return       none
	*/
    Appclient.prototype.appC_sendToggle = function (data) {
        appC_sendToggleLedMsgToAppServer(data);
    }


	/*!
	* @brief        Allows send config command to a network device
	*
	* @param 		none
	*
	* @return       none
	*/
    Appclient.prototype.appC_sendConfig = function (data) {
        appC_sendConfigReqToAppServer(data);
    }
}


Appclient.prototype.__proto__ = events.EventEmitter.prototype;

module.exports = Appclient;
