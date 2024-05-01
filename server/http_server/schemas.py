from pydantic import BaseModel
from typing import Optional, List, Union


class Student(BaseModel):
    id: int
    last_name: str
    first_name: str
    patronymic: str
    group: str
    year: int
    course: int
    photo: str

class StudentCreate(BaseModel):
    last_name: str
    first_name: str
    patronymic: str
    group: str
    year: int
    course: int
    photo: str


class StudentUpdate(BaseModel):
    last_name: Optional[str] = None
    first_name: Optional[str] = None
    patronymic: Optional[str] = None
    year: Optional[int] = None
    photo: Optional[str] = None
    course: Optional[int] = None
    group: Optional[str] = None

class FieldsRequest(BaseModel):
    total:List[str]
    alphabetic: List[str]
    comparable: List[str]
    years: List[int]
    courses: List[int]