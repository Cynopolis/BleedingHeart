import serial
from time import sleep

class Comm:
    current_pressure = -1
    target_pressure = -1
    motor_speed = -1
    def __init__(self, port, baud_rate):
        # Setup serial
        self.comm = serial.Serial(port, baud_rate)
        #read a line from the serial bus to sync arduino with python script
        for x in range(0,5):
            self.comm.readline()
        self.getPID()
        return

    def getPID(self):
        message = self.comm.readline().decode().strip()
        #message = 'Current Pressure: 80, Target Pressure: 15, Motor Speed: 255'
        #print(message)
        if message[0:7] == 'Current':
            startIndex = message.find(':')
            endIndex = message.find(',')
            self.current_pressure = float(message[startIndex+1:endIndex])
            startIndex = message.find(':', endIndex)
            endIndex = message.find(',', endIndex+1)
            self.target_pressure = float(message[startIndex+1:endIndex])
            startIndex = message.find(':', endIndex)
            self.motor_speed = float(message[startIndex+1:len(message)])
            #print("1", self.current_pressure, "2", self.target_pressure, "3", self.motor_speed)
            return (self.current_pressure, self.target_pressure, self.motor_speed)
        else:
            return (self.current_pressure, self.target_pressure, self.motor_speed)

    def setPressure(self, pressure):
        pressure = float(pressure)
        msg = 'PRSR ' + str(pressure) + '\r\n'
        while self.getPID()[1] != pressure:
            self.comm.write(msg.encode())
        return

    def setHeartRate(self, BPM):
        msg = 'BEAT ' + str(BPM) + '\r\n'
        response = 'Heart Rate set to ' + str(BPM) + ' BPM.'
        ans = ''
        while ans != response:
            self.comm.write(msg.encode())
            ans = self.comm.readline().decode().strip()
        return

    def fill(self, time):
        msg = 'FILL ' + str(time) + '\r\n'
        response = 'Filling for ' + str(time) + ' seconds.'
        ans = ''
        while ans != response:
            self.comm.write(msg.encode())
            ans = self.comm.readline().decode().strip()
        return

#comm = Comm('COM11', 115200)