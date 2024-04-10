from sqlalchemy import create_engine, Column, Integer, String, LargeBinary
from sqlalchemy.orm import declarative_base, sessionmaker
from sqlalchemy import select

Base = declarative_base()


class Student(Base):
    __tablename__ = 'students'

    id = Column(Integer, primary_key=True)
    last_name = Column(String)
    first_name = Column(String)
    patronymic = Column(String)
    year = Column(Integer)
    photo = Column(String)
    course = Column(Integer)
    group = Column(String)
    
    def __repr__(self):
        return f"<Student(id={self.id}, full_name='{self.full_name}', year={self.year}, " \
               f"photo='{self.photo}', course={self.course}, group='{self.group}')>"

engine = create_engine('postgresql://postgres@/postgres')

Base.metadata.create_all(engine)