from time import sleep
from datetime   import datetime

from sqlalchemy     import create_engine, func
from sqlalchemy.orm import sessionmaker, scoped_session

import models
from parser import ArduinoController


class LogSessionManager:

    def __init__(self, Session):
        self.logging_session = None
        self.current_log_id = None
        self.Session = Session

    def _start_logging_session(self, name=None, desc=None):
        """Create a new session entry in the database. Internal use only."""
        self.logging_session = models.Session(
            creation_time=datetime.now(),
            name=name,
            desc=desc
        )
        self.Session.add(self.logging_session)
        self.Session.commit()

        # Store the current log session_id. This way the log session can be
        # retrieved when closing the session, as it is not called from the
        # serial message handling thread.
        self.current_log_id = self.logging_session.session_id

        # As a convenience, return the logging session object.
        return self.logging_session

    def end_logging_session(self):
        """Close the logging session."""
        if not self.logging_session:
            return

        entry_count = self.Session.query(models.Log).\
            filter(models.Log.session_id == self.current_log_id).\
            count()
        print 'Closing log session {}, {} entries were logged.'.\
            format(self.current_log_id, entry_count)
        self.logging_session = None

    def on_log_received(self, log_data):
        """Event handler for received logs."""
        try:
            log = models.Log(**log_data)
            s = self.logging_session or self._start_logging_session()
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
    logs.end_logging_session()


if __name__ == '__main__':
    main()
