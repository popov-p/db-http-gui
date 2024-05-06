from sqlalchemy import create_engine, Column, Integer, String, LargeBinary
from sqlalchemy.orm import declarative_base
from sqlalchemy import select

Base = declarative_base()


class Student(Base):
    __tablename__ = 'students-ubuntu'

    id = Column(Integer, primary_key=True)
    last_name = Column(String)
    first_name = Column(String)
    patronymic = Column(String)
    group = Column(String)
    year = Column(Integer)
    course = Column(Integer)
    photo = Column(String)
    
    def __repr__(self):
        return f"<Student(id={self.id}, last_name='{self.last_name}', first_name='{self.first_name}', patronymic='{self.patronymic}', " \
               f"group='{self.group}', year={self.year}, course={self.course}, photo='{self.photo}',)>"

engine = create_engine('postgresql://postgres@/postgres')

Base.metadata.create_all(engine)