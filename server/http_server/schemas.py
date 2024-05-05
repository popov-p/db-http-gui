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

class FieldsRequest(BaseModel):
    total:List[str]
    alphabetic: List[str]
    comparable: List[str]