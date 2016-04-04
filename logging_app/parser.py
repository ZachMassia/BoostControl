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
            time.sleep(2)  # Allow the Arduino time to reboot.

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

    def verify_msg(self, msg):
        """Ensure the message is of a valid format.

        If the message contains the proper delimiters, it is deconstructed
        into a list and returned.

        Improperly formatted messages will raise a BadSerialMessageError.

        """
        if (not msg.startswith(self.msg_start) and
                not msg.endswith(self.msg_end)):
            raise BadSerialMessageError(
                'Message missing start char "{}" or end char "{}"'.format(
                    self.msg_start, self.msg_end))

        # Remove start and end chars.
        re_str = '[{}{}]'.format(self.msg_start, self.msg_end)
        msg = re.sub(re_str, '', msg.strip())

        # Split the message into a list using msg_sep.
        return msg.split(self.msg_sep)

    def register_format(self, msg):
        """Parse the format line from the Arduino.

        The first line sent from the Arduino after a reboot defines
        the order of the sensor values to be sent thereafter.

        """
        try:
            msg = self.verify_msg(msg)
            if msg[0] == 'log_format':
                self.msg_format = msg[1:]
            else:
                raise BadSerialMessageError(
                    'Attempt to register format with invalid format string.')
        except BadSerialMessageError as e:
            print 'Error parsing format message.'
            print 'The following message was received: "{}"'.format(msg)
            print e.strerror
            raise

    def parse_log_line(self, msg):
        """Parse a sensor log line from the Arduino.

        The sensor value is stored in a dict along with it's name as
        given in the format string.

        TODO: Verify creating this many dict objects is not slowing
              things down.

        """
        try:
            data = self.verify_msg(msg)
            data_dict = {}

            for index, value in enumerate(data):
                sensor = self.msg_format[index]
                data_dict[sensor] = value
            return data_dict
        except BadSerialMessageError as e:  # TODO Factor this repetition out
            print 'Error parsing log message.'
            print 'The following message was received: "{}"'.format(msg)
            print e.strerror
            raise
        except IndexError as e:
            print 'Bad data: "{}"'.format(msg)


def test():
    """Quick and dirty test method for reading from Arduino."""

    parser = Parser()

    class Listener:

        def __init__(self):
            self.format_received = False

        def on_msg_received(self, msg):
            if not self.format_received:
                parser.register_format(msg)
                self.format_received = True
            else:
                data = parser.parse_log_line(msg)
                print 'Raw: "{}" | Dict: "{}"'.format(msg, data)

    listener = Listener()
    baud = 500000
    run_time = 10  # seconds

    arduino = Arduino(baud)
    arduino.register_listener(listener)

    print 'Starting serial receive loop for {} seconds.'.format(run_time)
    arduino.start()
    time.sleep(run_time)

    print 'Calling stop()'
    arduino.stop()
    time.sleep(0.25)
    print 'All done.'


if __name__ == '__main__':
    test()
