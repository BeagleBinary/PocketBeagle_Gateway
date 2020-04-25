
var store = require('store')
class Client{

    

    constructor()
    {
        this.fileSystem = require('fs');
        this.https = require('https')
        this.jsonData = { "sensorId":"db603444-adb7-4d16-b4b9-5b4027558fd0", "data":42069666};
        this.data = JSON.stringify(this.jsonData)
        this.userId = null;
        this.sensorId = null;
    }




    createSensor(userId) {
        
        const request = require('request');
        
        var sensorJson = {
            userId: "" + userId,
            externalAddress:""
        }
        

        const sensorData = JSON.stringify(sensorJson);

        const SENSOR_URL = "https://fwsllpk77c.execute-api.us-east-1.amazonaws.com/dev/sensors"
        const headers = {'Content-Type': 'application/json'};
        

        return new Promise((resolve, reject) => {
        request.post(
            {
                headers: headers, 
                url: SENSOR_URL , 
                body: sensorData
            }, 
                (error, response, body) => {
            if (error) reject(error);
            if (response && response.statusCode != 200) {
                reject('Invalid status code <' + response.statusCode + '>');
            }

            const bodyJson = JSON.parse(body)
            this.sensorId = bodyJson['id'];

            store.set("userId", { Id: userId })
            store.set(userId, { sensor: this.sensorId })

            resolve(bodyJson['id']);
        });
        })
    }

    loginUser(){
        
        const request = require('request');
        var settingsJson = JSON.parse(this.fileSystem.readFileSync('Settings.json', 'utf8'));

        const LOGIN_URL = "https://fwsllpk77c.execute-api.us-east-1.amazonaws.com/dev/users/login"
        const headers = {'Content-Type': 'application/json'}

         var loginJson  = {
            userName: settingsJson['userName'],
            password: settingsJson['password']
        }
       
        const loginData = JSON.stringify(loginJson)


        return new Promise((resolve, reject) => {
        request.post(
            {
                headers: headers, 
                url: LOGIN_URL , 
                body: loginData
            }, 
                (error, response, body) => {
            if (error) reject(error);
            if (response && response.statusCode != 200) {
                reject('Invalid status code <' + response.statusCode + '>');
            }
            	const bodyJson = JSON.parse(body)
            	this.userId = bodyJson['userId'];
            	resolve(bodyJson['userId']);
            });

        });

    }

    sendWebsockets(data){
        let {PythonShell} = require('python-shell')
        let userId = store.get('userId').Id
        let sensorId = store.get(userId).sensor

        console.log("User id offf " + userId)
        console.log("sensor id offff " + sensorId)

        const dataJson = {
            sensorId: sensorId,
            data: data
        }

        const dataString = JSON.stringify(dataJson)
        

        let options = {
            mode: 'text',
            pythonPath: '/usr/bin/python3',
            pythonOptions: ['-u'], // get print results in real-time
            scriptPath: '',
            args: [dataString]
        };


        PythonShell.run('Collector.py', options, function (err, results) {
            if (err) throw err;
            // results is an array consisting of messages collected during execution
            console.log('results: %j', results);
        });

    }

    /*
    sendWebsockets(sensorId, data)
    {

        let {PythonShell} = require('python-shell')

        const dataJson = {
            sensorId: sensorId,
            data: data
        }

        const dataString = JSON.stringify(dataJson)
        

        let options = {
            mode: 'text',
            pythonPath: '/usr/bin/python3',
            pythonOptions: ['-u'], // get print results in real-time
            scriptPath: '',
            args: [dataString]
        };


        PythonShell.run('Collector.py', options, function (err, results) {
            if (err) throw err;
            // results is an array consisting of messages collected during execution
            console.log('results: %j', results);
        });
    }
    */

}

module.exports = Client













/*
let {PythonShell} = require('python-shell')

let options = {
    mode: 'text',
    pythonPath: '/Library/Frameworks/Python.framework/Versions/3.7/bin/python3',
    pythonOptions: ['-u'], // get print results in real-time
    scriptPath: '',
    args: [data]
};


PythonShell.run('test.py', options, function (err, results) {
    if (err) throw err;
    // results is an array consisting of messages collected during execution
    console.log('results: %j', results);
});

*/


