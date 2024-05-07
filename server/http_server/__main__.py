
from fastapi import FastAPI, Query
from fastapi import Depends, Response
from sqlalchemy.orm import sessionmaker
from fastapi.exceptions import HTTPException
from fastapi.security import HTTPBasic, HTTPBasicCredentials

from typing import List, Optional
from server.http_server import models, schemas, utils
from server.http_server.models import Student, engine
from server.http_server.schemas import FieldsRequest, StudentCreate
from server.http_server import crud
import os
from configparser import ConfigParser
from passlib.context import CryptContext

config_file_path = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                'cfg.ini')


#utils.init_config(config_file_path)

config = ConfigParser()

config.read(config_file_path)
correct_username = config.get('auth', 'username')
correct_hashed_password = config.get('auth', 'password')
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
    pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")
    if credentials.username != correct_username or not pwd_context.verify(credentials.password, correct_hashed_password):
        return Response(status_code=403, content="Unauthorized")
    return Response(status_code=200, content="Ok")

@app.get("/fields", response_model=FieldsRequest, dependencies=[Depends(auth)])
def get_fields(db: Session = Depends(get_db)):
    total = [key for key in Student.__table__.columns.keys()]
    alphabetic = ["last_name", "first_name", "patronymic", "group"]
    comparable = ["year", "course"]
    return FieldsRequest(total=total, alphabetic=alphabetic, comparable=comparable)


# Create student
@app.post("/add", response_model=schemas.StudentCreate, dependencies=[Depends(auth)])
def create_student(student: schemas.StudentCreate, db: Session = Depends(get_db)):
    return crud.create_student(db=db, student=student)

@app.get("/students", response_model=List[schemas.Student], dependencies=[Depends(auth)])
def get_all_students(db: Session = Depends(get_db)):
    return crud.get_all_students(db)

# Delete selected students ID 
@app.delete("/delete-selected", dependencies=[Depends(auth)])
def delete_student(data: List[int], db: Session = Depends(get_db)):
    if data is not None:
        for delete_id in data:
            db_student = crud.get_student(db, student_id=delete_id)
            if db_student is None:
                raise HTTPException(status_code=404, detail="Student not found")
            crud.delete_student(db=db, student_id=delete_id)
    return Response(status_code=200)

# Delete all students 
@app.delete("/delete-all", response_model=int, dependencies=[Depends(auth)])
def delete_all_students(db: Session = Depends(get_db)):
    return crud.delete_all_students(db=db)

@app.get("/filter", response_model=List[int], dependencies=[Depends(auth)])
def filter_by(
    last_name: Optional[str] = Query(None, description="First letter of last_name"),
    first_name: Optional[str] = Query(None, description="First letter of first_name"),
    patronymic: Optional[str] = Query(None, description="First letter of patronymic"),
    group: Optional[str] = Query(None, description="First letter of group"),
    year: Optional[int] = Query(None, description="Student`s birth year"),
    course: Optional[int] = Query(None, description="Student`s course"),
    and_less:Optional[bool] = Query(None, description="Show less than"),
    and_greater:Optional[bool] = Query(None, description="Show greater than"),
    db: Session = Depends(get_db)
):
    params = locals()
    params.pop("db")

    filtering_params = {}
    for key, value in params.items():
        if value is not None:
            if(type(value) == str):
                filtering_params["str"] = (key, value)
            elif(type(value) == int):
                filtering_params["int"] = (key, value)
            elif(type(value) == bool):
                if value:
                    filtering_params["bool"] = (key, value)
    return crud.filter(db=db, **filtering_params)