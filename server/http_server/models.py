from sqlalchemy import create_engine, Column, Integer, String
from sqlalchemy.orm import declarative_base, sessionmaker
from sqlalchemy import select

Base = declarative_base()


class Student(Base):
    __tablename__ = 'students'

    id = Column(Integer, primary_key=True)
    full_name = Column(String)
    year = Column(Integer)
    photo = Column(String)
    course = Column(Integer)
    group = Column(String)
    
    def __repr__(self):
        return f"<Student(id={self.id}, full_name='{self.full_name}', year={self.year}, " \
               f"photo='{self.photo}', course={self.course}, group='{self.group}')>"

engine = create_engine('postgresql://postgres@/postgres')

Base.metadata.create_all(engine)

Session = sessionmaker(bind=engine)
session = Session()
query = select(Student)
results = session.execute(query)

for row in results:
   print(row)

#session.commit()
session.close()



#student1 = Student(full_name='Иванов Иван Иванович', year=2000, photo='/path/to/photo.jpg', course=2, group='A1')
#student2 = Student(full_name='Петров Петр Петрович', year=2001, photo='/path/to/photo.jpg', course=1, group='B2')
#student3 = Student(full_name='Сидоров Сидор Сидорович', year=1999, photo='/path/to/photo.jpg', course=3, group='C3')

#session.add_all([student1, student2, student3])
#session.commit()


#delete_query = delete(Student)

#session.execute(delete_query)
#session.commit()
#session.close()