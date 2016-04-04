import sys
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


class Parser:

    def __init__(self):
        # Define the serial protocol.
        self.msg_start = '!'
        self.msg_sep = ';'
        self.msg_end = '|'


def test():
    """Quick and dirty test method for reading from Arduino."""

    class Listener:
        def on_msg_received(self, msg):
            print msg

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
