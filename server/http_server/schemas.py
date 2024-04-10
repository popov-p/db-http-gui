from pydantic import BaseModel
from typing import Optional


class Student(BaseModel):
    id: int
    last_name: str
    first_name: str
    patronymic: str
    year: int
    photo: str
    course: int
    group: str


class StudentCreate(BaseModel):
    last_name: str
    first_name: str
    patronymic: str
    year: int
    photo: str
    course: int
    group: str


class StudentUpdate(BaseModel):
    last_name: Optional[str] = None
    first_name: Optional[str] = None
    patronymic: Optional[str] = None
    year: Optional[int] = None
    photo: Optional[str] = None
    course: Optional[int] = None
    group: Optional[str] = None