from time import sleep
from datetime   import datetime

from sqlalchemy     import create_engine
from sqlalchemy.orm import sessionmaker, scoped_session

import models
from parser import ArduinoController


class LogSessionManager:

    def __init__(self, Session):
        self.logging_session = None
        self.Session = Session

    def get_current_logging_session(self):
        """Returns the current logging session. Creating it if required."""
        if not self.logging_session:
            self.logging_session = models.Session(creation_time=datetime.now())
            self.Session.add(self.logging_session)
            self.Session.commit()

        return self.logging_session

    def end_current_logging_session(self):
        """Commit any changes to the db and close the logging session."""
        self.logging_session = None

    def on_log_received(self, log_data):
        """Event handler for received logs."""
        # TODO: Verify log_data is valid before inserting.
        try:
            log = models.Log(**log_data)
            s = self.get_current_logging_session()
            s.logs.append(log)
            self.Session.commit()
        except:
            print 'Exception with log data: {}'.format(log_data)


def main():
    # Setup database objects.
    sqlite_engine = create_engine('sqlite:///logs.db')
    models.Base.metadata.create_all(sqlite_engine)
    session_factory = sessionmaker(bind=sqlite_engine)

    logs = LogSessionManager(scoped_session(session_factory))

    arduino = ArduinoController()
    arduino.register_listener(logs)

    # Temp commands to test logging to sqlite database.
    arduino.start()
    sleep(15)
    arduino.stop()
    logs.end_current_logging_session()


if __name__ == '__main__':
    main()
