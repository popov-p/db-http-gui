
from fastapi import FastAPI, Query, Depends, Request, Response, HTTPException
from pydantic import ValidationError
from fastapi.responses import JSONResponse
from fastapi.exceptions import RequestValidationError

from sqlalchemy.orm import sessionmaker
from sqlalchemy import inspect
from fastapi.security import HTTPBasic, HTTPBasicCredentials

from typing import List, Optional
from server.http_server import models, schemas, utils
from server.http_server.models import Student, engine
from server.http_server.schemas import FieldsResponse

from server.http_server import crud
import os
from configparser import ConfigParser
from passlib.context import CryptContext

import logging
config_file_path = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                'cfg.ini')


utils.init_config(config_file_path)

config = ConfigParser()

config.read(config_file_path)
correct_username = config.get('auth', 'username')
correct_hashed_password = config.get('auth', 'password')

severity = int(config.get('logging', 'severity'))

log_dir = config.get('logging', 'dir')
os.makedirs(log_dir, exist_ok=True)

logging.basicConfig(filename=os.path.join(log_dir, 'backend.log'), level=severity, format='%(asctime)s - %(levelname)s - %(message)s')
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
        logging.error("POST /auth: failed, status_code = 403")
        raise HTTPException(status_code=403, detail="Unauthorized")
    logging.info("POST /auth: success, status_code = 200")
    return Response(status_code=200, content="Ok")

@app.get("/fields", response_model=FieldsResponse, dependencies=[Depends(auth)])
def get_fields(db: Session = Depends(get_db)):
    logging.info("GET /fields call")
    columns = inspect(Student).columns
    total = [col_name for col_name, column in columns.items()]
    alphabetic = [col_name for col_name, column in columns.items() if (column.type.python_type == str) and col_name != "photo"]
    comparable = [col_name for col_name, column in columns.items() if (column.type.python_type == int) and col_name != "id"]
    return FieldsResponse(total=total, alphabetic=alphabetic, comparable=comparable)


# Create student
@app.post("/add", response_model=schemas.StudentCreate, dependencies=[Depends(auth)])
def create_student(student: schemas.StudentCreate, db: Session = Depends(get_db)):
    logging.info("POST /add call")
    return crud.create_student(db=db, student=student)

@app.get("/students", response_model=List[schemas.Student], dependencies=[Depends(auth)])
def get_all_students(db: Session = Depends(get_db)):
    logging.info("GET /students call")
    return crud.get_all_students(db)

# Delete selected students ID 
@app.delete("/delete-selected", dependencies=[Depends(auth)])
def delete_student(data: List[int], db: Session = Depends(get_db)):
    logging.info("DELETE /delete-selected call")
    if data is not None:
        for delete_id in data:
                crud.delete_student(db=db, student_id=delete_id)
    else:
        logging.error("DELETE /delete-selected, input deletion data is empty, status_code = 404")
        raise HTTPException(status_code=404, detail="Empty delete data")

# Delete all students 
@app.delete("/delete-all", response_model=int, dependencies=[Depends(auth)])
def delete_all_students(db: Session = Depends(get_db)):
    logging.info("DELETE /delete-all call")
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
    logging.info("GET /filter call")
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

@app.post("/test-id", response_model=List[int])
def test_get_id_by_data(student:schemas.StudentCreate, db: Session = Depends(get_db)):
    #testing function, no logging provided
    return crud.id_by_data(db=db, student=student)

@app.exception_handler(RequestValidationError)
def validation_exception_handler(request: Request, exc: RequestValidationError):
    logging.error(f"Validation error occurred: {exc}")
    return Response(status_code=422, content={"detail": "Validation error", "errors": exc.errors()})