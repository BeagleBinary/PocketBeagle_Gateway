/******************************************************************************

 @file device.js

 @brief device specific functions

 Group: WCS LPC
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/

/********************************************************************
 * Variables
 * *****************************************************************/
var fs = require("fs");

var Client = require("./Client.js")

/********************************************************************
 * Defines
 * *****************************************************************/
const Smsgs_dataFields = Object.freeze({
    tempSensor: 0x0001,
    lightSensor: 0x0002,
    humiditySensor: 0x0004,
    msgStats: 0x0008,
    configSettings: 0x0010,
    hallEffectSensor: 0x0020,
    accelSensor: 0x0040,
});

const STATUS_SUCCESS = 0;

/* Toggle debug print statements */
const PRINT_DEBUG = false;

/*!
 * @brief      Constructor for device objects
 *
 * @param      shortAddress - 16 bit address of the device
 * 			   extAddress - 64 bit IEEE address of the device
 * 			   capabilityInfo - device capability information
 *
 * @retun      device object
 */



let client = new Client()
let userId
let sensorCount = 0
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









function Device(shortAddress, extAddress, capabilityInfo) {
    if(PRINT_DEBUG) console.log("In device, Device()");
    var devInfo = this;
    devInfo.shortAddress = shortAddress;
    devInfo.extAddress = extAddress;
    devInfo.capabilityInfo = capabilityInfo;
    devInfo.active = 'true';




    getUserId().then(function(result) {
    this.userId = result

        getSensorId(this.userId).then(function(secondResult) {
            this.sensorId = secondResult
            //client.sendWebsockets(this.sensorId, 0)
            console.log("Sensor id of : " + secondResult)
        
        });

    console.log("User id of : " + result)
    

    });


    if(PRINT_DEBUG) console.log(devInfo);
    return devInfo;
}

/* Prototype Functions */
Device.prototype.rxSensorData = function (sensorData) {
    /* recieved message from the device, set as active */
    this.active = 'true';
	/* Check the support sensor Types and
	add information elements for those */
    console.log("SENSOR DATA IS" )
    console.log(sensorData)
    
    if (sensorData.frameControl & Smsgs_dataFields.tempSensor){
        /* update the sensor values */
        this.temperaturesensor = {
            ambienceTemp: sensorData.tempSensor.ambienceTemp,
            objectTemp: sensorData.tempSensor.objectTemp
        };


        if (!this.userId){
        console.log("Not logged in.")
        getUserId().then(function(result) {
            this.userId = result

            /*
            getSensorId(this.userId).then(function(secondResult) {
                this.sensorId = secondResult

                console.log("Sensor id of : " + secondResult)
                client.sendWebsockets(this.sensorId, sensorData.tempSensor.ambienceTemp)
            });
            */

            client.sendWebsockets(sensorData.tempSensor.ambienceTemp)

            console.log("User id of : " + result)
    

            });
        }else{
            console.log("Logged in.")
            client.sendWebsockets(this.sensorId, sensorData.tempSensor.ambienceTemp)
        }
    }
    if (sensorData.frameControl & Smsgs_dataFields.lightSensor) {
        /* update the sensor values */
        this.lightsensor = {
            rawData: sensorData.lightSensor.rawData
        };


        if (!this.userId){
        console.log("Not logged in.")
        getUserId().then(function(result) {
            this.userId = result

            /*
            getSensorId(this.userId).then(function(secondResult) {
                this.sensorId = secondResult

                console.log("Sensor id of : " + secondResult)
                client.sendWebsockets(this.sensorId, sensorData.lightSensor.rawData)
            });
            */

            client.sendWebsockets(sensorData.lightSensor.rawData)
            console.log("User id of : " + result)
    

            });
        }else{
            console.log("Logged in.")
            client.sendWebsockets(this.sensorId, sensorData.lightSensor.rawData)
        }
    }
    if (sensorData.frameControl & Smsgs_dataFields.humiditySensor) {
        /* update the sensor values */
        this.humiditysensor = {
            temp: sensorData.humiditySensor.temp,
            humidity: sensorData.humiditySensor.humidity
        };




        if (!this.userId){
        console.log("Not logged in.")
        getUserId().then(function(result) {
            this.userId = result

            /*
            getSensorId(this.userId).then(function(secondResult) {
                this.sensorId = secondResult

                console.log("Sensor id of : " + secondResult)
                client.sendWebsockets(this.sensorId, sensorData.humiditySensor.humidity)
            });
            */

            client.sendWebsockets(sensorData.humiditySensor.humidity)

            console.log("User id of : " + result)
    

            });
        }else{
            console.log("Logged in.")
            client.sendWebsockets(this.sensorId, sensorData.humiditySensor.humidity)
        }
    }
    if (sensorData.frameControl & Smsgs_dataFields.hallEffectSensor) {
        /* update the sensor values */
        this.halleffectsensor = {
	    flux: sensorData.hallEffectSensor.flux
        };


	console.log(sensorData.hallEffectSensor.flux)


        if (!this.userId){
        console.log("Not logged in.")
        getUserId().then(function(result) {
            this.userId = result

            /*
            getSensorId(this.userId).then(function(secondResult) {
                this.sensorId = secondResult

                console.log("Sensor id of : " + secondResult)
                client.sendWebsockets(this.sensorId, sensorData.humiditySensor.humidity)
            });
            */

            client.sendWebsockets(sensorData.sensorData.hallEffectSensor.flux)

            console.log("User id of : " + result)
    

            });
        }else{
            console.log("Logged in.")
            client.sendWebsockets(this.sensorId, sensorData.hallEffectSensor.flux)
        }
    }
    /* update rssi information */
    this.rssi = sensorData.rssi;
}

Device.prototype.rxConfigRspInd = function (devConfigData) {
    var device = this;
    if (devConfigData.status == STATUS_SUCCESS) {
        device.active = 'true';
		/* Check the support sensor Types and add
		information elements for those */
        if (devConfigData.frameControl & Smsgs_dataFields.tempSensor) {
            /* initialize sensor information element */
            device.temperaturesensor = {
                ambienceTemp: 0,
                objectTemp: 0
            };
        }
        if (devConfigData.frameControl & Smsgs_dataFields.lightSensor) {
            /* initialize sensor information element */
            device.lightsensor = {
                rawData: 0
            };
        }
        if (devConfigData.frameControl & Smsgs_dataFields.humiditySensor) {
            /* initialize sensor information element */
            device.humiditysensor = {
                temp: 0,
                humidity: 0
            };
        }
        if (devConfigData.frameControl & Smsgs_dataFields.hallEffectSensor) {
	    device.halleffectsensor = {
		flux: 0.0
	    };
	}
        device.reportingInterval = devConfigData.reportingInterval;
        if (device.capabilityInfo.rxOnWhenIdle == 1) {
            device.pollingInterval = devConfigData.pollingInterval;
        }
        else {
            device.pollingInterval = "always on device";
        }
    }
}

Device.prototype.deviceNotActive = function (inactiveDevInfo) {
    this.active = 'false';
}

Device.prototype.devUpdateInfo = function (shortAddr, capInfo) {
    this.shortAddress = shortAddr;
    this.capabilityInfo = capInfo;
    this.active = 'true';
}

module.exports = Device;
