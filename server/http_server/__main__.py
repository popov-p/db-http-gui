
from fastapi import FastAPI
from fastapi import Depends, Response
from sqlalchemy.orm import sessionmaker
from fastapi.exceptions import HTTPException
from fastapi.security import HTTPBasic, HTTPBasicCredentials

from typing import List
from server.http_server import models, schemas, utils
from server.http_server.models import Student, engine
from server.http_server.schemas import FieldsRequest, StudentCreate, Filter
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

@app.post("/auth")
def auth(credentials: HTTPBasicCredentials = Depends(security)):
    correct_username = username
    correct_password = user_password
    if credentials.username != correct_username or credentials.password != correct_password:
        raise HTTPException(status_code=403 , detail="Incorrect username or password")
    return {"message": "Welcome, authorized user!"}

@app.get("/fields", response_model=FieldsRequest, dependencies=[Depends(auth)])
def get_fields(db: Session = Depends(get_db)):
    total = [key for key in Student.__table__.columns.keys()]
    #total = Student.__table__.columns.keys()
    alphabetic = ["last_name", "first_name", "patronymic", "group"]
    comparable = ["year", "course"]

    return FieldsRequest(total=total, alphabetic=alphabetic, comparable=comparable)


# Create student
@app.post("/add", response_model=schemas.StudentCreate)
def create_student(student: schemas.StudentCreate, db: Session = Depends(get_db)):
    return crud.create_student(db=db, student=student)

@app.get("/students", response_model=List[schemas.Student])
def get_all_students(db: Session = Depends(get_db)):
    return crud.get_all_students(db)

# Delete selected students ID 
@app.delete("/delete-selected")
def delete_student(data: List[int], db: Session = Depends(get_db)):
    if data is not None:
        for delete_id in data:
            db_student = crud.get_student(db, student_id=delete_id)
            if db_student is None:
                raise HTTPException(status_code=404, detail="Student not found")
            crud.delete_student(db=db, student_id=delete_id)
    return Response(status_code=200)

# Delete all students 
@app.delete("/delete-all", response_model=int)
def delete_all_students(db: Session = Depends(get_db)):
    return crud.delete_all_students(db=db)

def filter_by(data: schemas.Filter, db: Session = Depends(get_db)):
    filter_dict = data.dict()
    filters = {key: value for key, value in filter_dict.items() if value is not None}
    return crud.filter(db=db, **filters)
   