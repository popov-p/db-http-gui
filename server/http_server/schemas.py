from pydantic import BaseModel
from typing import Optional, List
from fastapi import Query


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

class FieldsResponse(BaseModel):
    total:List[str]
    alphabetic: List[str]
    comparable: List[str]