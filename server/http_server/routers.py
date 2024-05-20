from fastapi import Query, Depends, Response, HTTPException, APIRouter, Header


from sqlalchemy import inspect
from fastapi.security import HTTPBasic

from typing import List, Optional
from server.http_server import schemas
from server.http_server.models import Student
from server.http_server.schemas import FieldsResponse, AuthModel

from server.http_server import crud
import os

from passlib.context import CryptContext

from .utils import setup_logger 
from .database import get_db, Session
from .config import app_config

logger = setup_logger('backend-logger',
                            log_dir=app_config.log_dir,
                            level=app_config.severity)

security = HTTPBasic()
router = APIRouter()

@router.post("/auth")
def auth(authorization: str = Header(...)):
    auth_model = AuthModel.from_authorization_header(authorization)
    pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")
    if auth_model.username != app_config.username or not pwd_context.verify(auth_model.password, app_config.hashed_password):
        logger.error("POST /auth: failed, status_code = 403")
        raise HTTPException(status_code=403, detail="Unauthorized")
    logger.info("POST /auth: success, status_code = 200")
    return Response(status_code=200, content="Ok")

@router.get("/fields", response_model=FieldsResponse, dependencies=[Depends(auth)])
def get_fields(db: Session = Depends(get_db)):
    logger.info("GET /fields call")
    columns = inspect(Student).columns
    total = [col_name for col_name, column in columns.items()]
    alphabetic = [col_name for col_name, column in columns.items() if (column.type.python_type == str) and col_name != "photo"]
    comparable = [col_name for col_name, column in columns.items() if (column.type.python_type == int) and col_name != "id"]
    return FieldsResponse(total=total, alphabetic=alphabetic, comparable=comparable)


# Create student
@router.post("/add", response_model=schemas.Student, dependencies=[Depends(auth)])
def create_student(student: schemas.StudentCreate, db: Session = Depends(get_db)):
    logger.info("POST /add call")
    return crud.create_student(db=db, student=student)
#get student by id
@router.get("/student", response_model=schemas.Student)
def testfunc_get_student(student_id: int, db: Session = Depends(get_db)):
    return crud.get_student(db=db,student_id=student_id)


@router.get("/students", response_model=List[schemas.Student])
def get_all_students(db: Session = Depends(get_db)): #############################
    logger.info("GET /students call")
    return crud.get_all_students(db)

# Delete selected students ID 
@router.delete("/delete-selected", dependencies=[Depends(auth)])
def delete_student(data: List[int], db: Session = Depends(get_db)):
    logger.info("DELETE /delete-selected call")
    if data is not None:
        for delete_id in data:
                crud.delete_student(db=db, student_id=delete_id)
    else:
        logger.error("DELETE /delete-selected, input deletion data is empty, status_code = 404")
        raise HTTPException(status_code=404, detail="Empty delete data")

# Delete all students 
@router.delete("/delete-all", response_model=int, dependencies=[Depends(auth)])
def delete_all_students(db: Session = Depends(get_db)):
    logger.info("DELETE /delete-all call")
    return crud.delete_all_students(db=db)

@router.get("/filter", response_model=List[int], dependencies=[Depends(auth)])
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
    logger.info("GET /filter call")
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

@router.post("/test-id", response_model=List[int])
def test_get_id_by_data(student:schemas.StudentCreate, db: Session = Depends(get_db)):
    #testing function, no logging provided
    return crud.id_by_data(db=db, student=student)
