import re
import time
import serial
import threading

from serial.tools import list_ports


# A list of possible serial port names the Arduino may use.
PORT_NAMES = [
    # Mac OS X
    'tty.usbmodem',
    'cu.usbmodem',
    'tty.usbserial',
    'cu.usbserial',
    # Linux
    '/dev/usbdev',
    '/dev/tty',
    '/dev/serial',
    # Windows
    'COM3'
]


class Arduino:

    def __init__(self, baud, port=None):
        # Configure the serial port.
        self.ser = serial.Serial()
        self.ser.baudrate = baud
        self.ser.timeout = 2
        self.ser.port = port or self.find_port()

        self.msg_received_listeners = []
        self.run_msg_loop = False

    def find_port(self):
        """Looks through all serial devices for a connected Arduino"""
        ports = list_ports.comports()
        for port in ports:
            for possible_port in PORT_NAMES:
                if possible_port in port.device:
                    return port.device
        return ''

    def reset_arduino(self):
        """Toggle DTR which causes the Arduino to reset."""
        self.ser.setDTR(False)
        time.sleep(1)
        self.ser.flushInput()
        self.ser.setDTR(True)

    def register_listener(self, listener):
        """Register a listener to the message received event."""
        # Make sure the listener has the on_msg_received method defined.
        m = getattr(listener, 'on_msg_received', None)
        if callable(m):
            self.msg_received_listeners.append(listener)

    def notify_listeners(self, msg):
        """Notify all listeners of the message received event."""
        for listener in self.msg_received_listeners:
            listener.on_msg_received(msg)

    def msg_loop(self):
        """The message reading loop."""
        # Make sure the port is open.
        # TODO: Add error handling.
        if not self.ser.isOpen():
            self.ser.open()
            self.reset_arduino()

        self.run_msg_loop = True

        while self.run_msg_loop:
            msg = self.ser.readline()
            if msg:
                self.notify_listeners(msg)

        # Someone called stop(), so close the serial connection.
        self.ser.close()

    def start(self):
        """Start the message handling thread."""

        self.msg_thread = threading.Thread(target=self.msg_loop)
        self.msg_thread.start()

    def stop(self):
        """Stop the message handling thread."""
        self.run_msg_loop = False


class BadSerialMessageError(ValueError):
    """Raised when an improperly formatted message is parsed."""


class Parser:

    def __init__(self):
        # Define the serial protocol delimiters.
        self.msg_start = '!'
        self.msg_sep = ';'
        self.msg_end = '|'
        self.msg_format = []

    def parse_msg(self, msg):
        """Ensure the message is of a valid format.

        If the message contains the proper delimiters, it is returned in
        a map with the header and data.

        Improperly formatted messages will raise a BadSerialMessageError.

        """
        if (not msg.startswith(self.msg_start) and
                not msg.endswith(self.msg_end)):
            raise BadSerialMessageError(
                'Message missing start char "{}" or end char "{}": {}'.format(
                    self.msg_start, self.msg_end, msg))

        # Remove start and end chars.
        re_str = '[{}{}]'.format(self.msg_start, self.msg_end)
        msg = re.sub(re_str, '', msg.strip())

        # Split the messaged based on msg_sep.
        msg = msg.split(self.msg_sep)
        header = msg[0]
        data = msg[1:]

        # Make sure at least one data item was received.
        if not len(data):
            raise BadSerialMessageError(
                'No data was sent with msg header "{}"'.format(header))

        return {'header': header, 'data': data}

    def register_log_format(self, data):
        """Register the log format message from the Arduino.

        The first line sent from the Arduino after a reboot defines
        the order of the sensor values to be sent thereafter.

        """
        self.msg_format = data

    def parse_sensor_readings(self, data):
        """Parse a sensor readings message from the Arduino.

        The sensor value is stored in a dict along with it's name as
        given in the format string.

        TODO: Verify creating this many dict objects is not slowing
              things down.

        """
        try:
            readings = {}
            for sensor_id, value in enumerate(data):
                sensor = self.msg_format[sensor_id]
                readings[sensor] = value
            return readings
        except IndexError:
            raise BadSerialMessageError(
                'Received more sensor data than actual sensors: "{}"'.format(
                    data))


class ArduinoController:

    BAUD_RATE = 115200

    def __init__(self):
        self.parser = Parser()

        self.arduino = Arduino(ArduinoController.BAUD_RATE)
        self.arduino.register_listener(self)

        self.msg_handlers = {
            'log_format': self.parser.register_log_format,
            'sensor_readings': self.handle_sensor_readings
        }

    def on_msg_received(self, msg):
        """Attempt to parse & handle the received serial message."""
        parsed_msg = self.parser.parse_msg(msg)
        try:
            self.msg_handlers[parsed_msg['header']](parsed_msg['data'])
        except KeyError:
            raise BadSerialMessageError(
                'Unknown command: {}'.format(parsed_msg['header']))

    def start(self):
        self.arduino.start()  # For now simply delegate to the Arduino class.

    def stop(self):
        self.arduino.stop()  # For now simply delegate to the Arduino class.

    def handle_sensor_readings(self, data):
        """Store the sensor readings in the database."""
        # TODO: Implement database storage.
        print self.parser.parse_sensor_readings(data)


def test():
    """Quick and dirty test method for reading from Arduino."""

    arduino = ArduinoController()
    run_time = 10

    print 'Starting serial receive loop for {} seconds.'.format(run_time)

    arduino.start()
    time.sleep(run_time)

    print 'Calling stop()'
    arduino.stop()
    time.sleep(0.25)
    print 'All done.'


if __name__ == '__main__':
    test()
