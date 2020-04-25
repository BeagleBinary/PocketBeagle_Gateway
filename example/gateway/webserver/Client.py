import json
import random
import requests 
import stomper as stomper
import websocket
import threading
import sys
from enum import Enum
import signal


class UserLogin:
    def __init__(self, userName, password ):
        self.userName = userName
        self.password = password



class User:
    def __init__(self, userId, userName, firstName, lastName, emailAddress, phoneNumber, password):
        self.userId = userId
        self.userName = userName
        self.firstName = firstName
        self.lastName = lastName
        self.emailAddress = emailAddress
        self.phoneNumber = phoneNumber
        self.password = password

    def isValid(self):
        return len(self.userId) >= 0 and len(self.userName) >= 0 and len(self.firstName) >= 0 and len(self.lastName) >= 0 and len(self.emailAddress) >= 0  and len(self.phoneNumber) >= 0 and len(self.password) >= 0





class Sensor:
    def __init__(self, sensorId, userId):
        self.userId = userId
        self.sensorId = sensorId

    

class CloudConnector:


    

    def __init__(self):

        self.USER_URL = "https://fwsllpk77c.execute-api.us-east-1.amazonaws.com/dev/users"
        self.USER_LOGIN = self.USER_URL + "/login"
        self.SENSOR_URL = "https://fwsllpk77c.execute-api.us-east-1.amazonaws.com/dev/sensors"
        self.DATA_URL = "https://fwsllpk77c.execute-api.us-east-1.amazonaws.com/dev/data"

        self.timeInterval = 4

        self.user = None
        self.userId = None
        self.loginResult = False
        self.sensorId = None


    def loginUser(self, userName, password):



        loginJSON = {
            "userName": userName,
            "password": password
        }

        headers = {'Content-Type': 'application/json'}
        
        logIn = requests.post(url = self.USER_LOGIN,  json = loginJSON, headers=headers)
        
        cookie = logIn.json()


        if logIn.status_code != 200:
            print("Could not log in the specified user")
            return False
        
        if not cookie:
            print("Could not log in the specified user")
            return False

        print("\n \nSucessfully logged in. \n \n")
        self.userId = cookie['userId']

        if not self.userId:
            print("Could not find user")

            return False

        return True

        '''
        print("Retrieving user..............")
        user = requests.get(url = USER_URL + "/" + userId).json()


        print("Retrieved user.")
        if not user:
            return false


        user = User(userId, user['userName'], user['firstName'], user['lastName'], user['emailAddress'], user['phoneNumber'], user['password'])


        
        return user.isValid()
        '''
    

    def sendData(self):

        

        dataObject = {"sensorId": self.sensorId,"data":11}
        threading.Timer(self.timeInterval, self.sendData).start()
        websocket.enableTrace(True)
          # Connecting to websocket
        ws = websocket.create_connection("ws://websocketapi-env.eba-gxghbmt3.us-east-1.elasticbeanstalk.com//sensor-data")

          # Subscribing to topic
        client_id = str(random.randint(0, 1000))
        sub = stomper.subscribe("/topic/collect", client_id, ack='auto')
        ws.send(sub)

          # Sending some message
          #print(dataObject)
        ws.send(stomper.send("/app/API/data", dataObject))

        while True:
            print("Receiving data: ")
            d = ws.recv()
            print(d)
            return

    def sendDataWithData(self, data):

        

        dataObject = {"sensorId": self.sensorId,"data":data}
        threading.Timer(self.timeInterval, self.sendData).start()
        websocket.enableTrace(True)

          # Connecting to websocket
        ws = websocket.create_connection("ws://websocketapi-env.eba-gxghbmt3.us-east-1.elasticbeanstalk.com//sensor-data")

          # Subscribing to topic
        client_id = str(random.randint(0, 1000))
        sub = stomper.subscribe("/topic/collect", client_id, ack='auto')
        ws.send(sub)

          # Sending some message
          #print(dataObject)
        ws.send(stomper.send("/app/API/data", dataObject))

        while True:
            print("Receiving data: ")
            d = ws.recv()
            print(d)
            return

    def sendDataWithDataId(self,id, data):

        

        dataObject = {"sensorId": id,"data":data}
        threading.Timer(self.timeInterval, self.sendData).start()
        websocket.enableTrace(True)

          # Connecting to websocket
        ws = websocket.create_connection("ws://websocketapi-env.eba-gxghbmt3.us-east-1.elasticbeanstalk.com//sensor-data")

          # Subscribing to topic
        client_id = str(random.randint(0, 1000))
        sub = stomper.subscribe("/topic/collect", client_id, ack='auto')
        ws.send(sub)

          # Sending some message
          #print(dataObject)
        ws.send(stomper.send("/app/API/data", dataObject))

        while True:
            print("Receiving data: ")
            d = ws.recv()
            print(d)
            return

    def collectData(self):
        

        if not self.sensorId:
            print("This sensor is not registered. Please register before sending data.")
            return

        if self.loginResult:
            tempTime = (input("Enter an update interval. The deafault is 4: "))
            if not tempTime:
                self.timeInterval = 4
            else:
                self.timeInterval = int(tempTime)
            self.sendData()

        else:
            print("Not Logged In.")

    def collectData(self, timeInterval, data):
        

        if not self.sensorId:
            print("This sensor is not registered. Please register before sending data.")
            return

        self.timeInterval = timeInterval

        self.sendDataWithData(data)


    def registerUser(self,userJson):


       

        headers = {'Content-Type': 'application/json'}
        
        register = requests.post(url = self.USER_URL,  json = userJson, headers=headers)

        self.userId = register.json()['id']
        if register.status_code == 200:
            print("Sucessfully created user with a user id of (" + self.userId + ").")
        self.loginResult = True
        


    def registerSensor(self):


        headers = {'Content-Type': 'application/json'}
        if not self.userId:

            statusCode = 0
            while statusCode != 200:
                self.userId = input("Enter a user id for the sensor")

                headers = {'Content-Type': 'application/json'}
        
                getUser = requests.get(url = self.USER_URL + "/" + self.userId, headers=headers)
                statusCode = register.status_code
                print("Found user. Creating sensor...........")
        else:
            print("This user is logged in. Creating sensor............. \n")

        
        sensorJson = {
            "userId": self.userId,
            "externalAddress":""
        }   

        createSensor = requests.post(url = self.SENSOR_URL ,  json = sensorJson, headers=headers)

        if createSensor.status_code == 200:


            self.sensorId =  createSensor.json()['id']
            print("Sucessfully created sensor with id of (" + self.sensorId + ")! \n \n")

            return self.sensorId
        else:
            print(createSensor)


    def delete(self):
        headers = {'Content-Type': 'application/json'}

        if self.sensorId:

            deleteSensor = requests.delete(url = self.SENSOR_URL + "/" + self.sensorId , headers=headers)

            if deleteSensor.status_code == 200:
                print("Sucessfully Deleted Sensor")

            else:
                print("There was an issue deleting the sensor")
        else:
            print("This sensor does not have a sensor id")

    