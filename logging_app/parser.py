import re
import time

from arduino import Arduino


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
        msg = msg.strip()

        if not (msg.startswith(self.msg_start) and
                msg.endswith(self.msg_end)):
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
        for sensor in data:
            data_type, sensor_name = sensor.split(':')
            if data_type == 'f':
                data_type = float
            elif data_type == 'i':
                data_type = int
            else:
                raise BadSerialMessageError(
                    'Unknown data type "{}" for sensor "{}"'.format(
                        data_type, sensor_name))

            self.msg_format.append({'type': data_type, 'name': sensor_name})
        print 'Using log format: {}'.format(data)

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
                sensor_info = self.msg_format[sensor_id]
                readings[sensor_info['name']] = sensor_info['type'](value)
            return readings
        except IndexError:
            raise BadSerialMessageError(
                'Received more sensor data than actual sensors: "{}"'.format(
                    data))
        except ValueError:
            raise BadSerialMessageError(
                'Could not convert {}: "{}" to {}'.format(
                    sensor_info['name'], value, sensor_info['type']))


class ArduinoController:

    BAUD_RATE = 115200

    def __init__(self):
        self.parser = Parser()

        self.arduino = Arduino(ArduinoController.BAUD_RATE)
        self.arduino.register_listener(self)

        self.msg_handlers = {
            'log_format': self.parser.register_log_format,
            'sensor_readings': self.notify_listeners
        }

        self.log_received_listeners = []

    def register_listener(self, listener):
        """Register a listener to the log received event."""
        # Make sure the listener has the on_log_received method defined.
        m = getattr(listener, 'on_log_received', None)
        if callable(m):
            self.log_received_listeners.append(listener)

    def on_msg_received(self, msg):
        """Attempt to parse & handle the received serial message."""
        try:
            parsed_msg = self.parser.parse_msg(msg)
            handler = self.msg_handlers[parsed_msg['header']]
            handler(parsed_msg['data'])
        except KeyError:
            raise BadSerialMessageError(
                'Unknown command: {}'.format(parsed_msg['header']))
        except BadSerialMessageError as e:
            print 'Ignoring bad msg: {}'.format(e)


    def start(self):
        self.arduino.start()  # For now simply delegate to the Arduino class.

    def stop(self):
        self.arduino.stop()  # For now simply delegate to the Arduino class.

    def notify_listeners(self, data):
        """Notify all listeners of the log received event."""
        try:
            readings = self.parser.parse_sensor_readings(data)
            for listener in self.log_received_listeners:
                listener.on_log_received(readings)
        except BadSerialMessageError as e:
            print "Ignoring bad log: {}".format(e)
