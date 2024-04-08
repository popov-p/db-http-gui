from pydantic import BaseModel
from typing import Optional


class Student(BaseModel):
    id: int
    full_name: str
    year: int
    photo: str
    course: int
    group: str


class StudentCreate(BaseModel):
    full_name: str
    year: int
    photo: str
    course: int
    group: str


class StudentUpdate(BaseModel):
    full_name: Optional[str] = None
    year: Optional[int] = None
    photo: Optional[str] = None
    course: Optional[int] = None
    group: Optional[str] = None