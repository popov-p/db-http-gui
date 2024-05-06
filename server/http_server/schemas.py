from pydantic import BaseModel
from typing import Optional, List


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

class Filter(BaseModel):
    last_name: Optional[str]
    first_name: Optional[str]
    patronymic: Optional[str]
    group: Optional[str]
    year: Optional[int]
    course: Optional[int]
    and_less: Optional[bool]
    and_greater: Optional[bool]