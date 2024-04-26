
from fastapi import FastAPI
from fastapi import Depends
from sqlalchemy.orm import sessionmaker
from fastapi.exceptions import HTTPException
from fastapi.security import HTTPBasic, HTTPBasicCredentials

from typing import List
from server.http_server import models, schemas, utils
from server.http_server.models import Student, engine
from server.http_server.schemas import StudentUpdate, FieldsRequest
from server.http_server import crud
import os
from configparser import ConfigParser

config_file_path = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                'cfg.ini')
#utils.init_config(config_file_path)

config = ConfigParser()

#login = 'pavel'
#password = 'popov'

#pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")
#hashed_password = pwd_context.hash(password)

#config.add_section('auth')
#config.set('auth', 'username', login)
#config.set('auth', 'password', hashed_password)

#with open(config_file_path, 'w') as configfile:
#    config.write(configfile)


config.read(config_file_path)
username = config.get('auth', 'username')
user_password = config.get('auth', 'password')
security = HTTPBasic()


app = FastAPI()

Session = sessionmaker(bind=engine)

def get_db():
    db = Session()
    try:
        yield db
    finally:
        db.close()

@app.get("/")
def root():
    return {"message": "Hello World"}

@app.get("/fields", response_model=FieldsRequest)
def get_fields(db: Session = Depends(get_db)):
    print(db.query(Student).first().__dict__.keys())
    total = Student.__table__.columns.keys()
    alphabetic = ["last_name, first_name", "patronymic"]
    comparable = ["year","course", "group"]
    return FieldsRequest(total=total, alphabetic=alphabetic, comparable=comparable)

@app.post("/auth")
def auth(credentials: HTTPBasicCredentials = Depends(security)):
    correct_username = username
    correct_password = user_password
    if credentials.username != correct_username or credentials.password != correct_password:
        raise HTTPException(status_code=403 , detail="Incorrect username or password")
    return {"message": "Welcome, authorized user!"}

# Create student
@app.post("/create-student/", response_model=schemas.Student)
def create_student(student: schemas.Student, db: Session = Depends(get_db)):
    return crud.create_student(db=db, student=student)
@app.get("/students/")


# Read student by ID
@app.get("/students/{student_id}", response_model=schemas.Student)
def read_student(student_id: int, db: Session = Depends(get_db)):
    db_student = crud.get_student(db, student_id=student_id)
    if db_student is None:
        raise HTTPException(status_code=404, detail="Student not found")
    return db_student

@app.get("/students", response_model=List[schemas.Student])
def get_all_students(db: Session = Depends(get_db)):
    return crud.get_all_students(db)

# Update student by ID 
@app.put("/students/{student_id}", response_model=schemas.Student)
def update_student(student_id: int, student: schemas.StudentUpdate, db: Session = Depends(get_db)):
    db_student = crud.get_student(db, student_id=student_id)
    if db_student is None:
        raise HTTPException(status_code=404, detail="Student not found")
    return crud.update_student(db=db, student=student, student_id=student_id)

# Delete student by ID 
@app.delete("/students/{student_id}")
def delete_student(student_id: int, db: Session = Depends(get_db)):
    db_student = crud.get_student(db, student_id=student_id)
    if db_student is None:
        raise HTTPException(status_code=404, detail="Student not found")
    crud.delete_student(db=db, student_id=student_id)
    return {"message": "Student deleted successfully"}

# Delete all students 
@app.delete("/students/", response_model=int)
def delete_all_students(db: Session = Depends(get_db)):
    num_deleted = crud.delete_all_students(db=db)
    return {"message": f"{num_deleted} students deleted successfully"}