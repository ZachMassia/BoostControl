from sqlalchemy import Table, Column, Integer, String, Float, DateTime, ForeignKey
from sqlalchemy.orm import relationship
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()


class Session(Base):
    __tablename__ = 'session'
    session_id    = Column(Integer, primary_key=True)
    name          = Column(String)
    desc          = Column(String)
    creation_time = Column(DateTime)

    logs = relationship('Log', order_by=Log.log_id, back_populates='session')


class Log(Base):
    __tablename__ = 'log'
    log_id        = Column(Integer, primary_key=True)
    timestamp     = Column(Integer)
    setpoint      = Column(Float)
    map_reading   = Column(Float)
    duty_cycle    = Column(Integer)
    kp            = Column(Float)
    ki            = Column(Float)
    kd            = Column(Float)

    session_id = Column(Integer, ForeignKey('session.session_id'))
    session    = relationship('Session', back_populates='log')
