from Client import CloudConnector
import signal
import sys
import signal
import atexit
import json

connector = CloudConnector()
userName = "TestUser"
password = "42069"
timeInterval = 4

connector.loginResult = connector.loginUser(userName, password)

sensorData = sys.argv[1]
sensorJson = json.loads(sensorData)
print("Sensor data is " + str(sensorData))
if connector.loginResult:
	
	connector.sendDataWithDataId(sensorJson['sensorId'], sensorJson['data'])




