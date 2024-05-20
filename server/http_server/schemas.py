from pydantic import BaseModel
from typing import  List
from fastapi import Query

import base64 

class AuthModel(BaseModel):
    username: str
    password: str

    @classmethod
    def from_authorization_header(cls, authorization_header: str):
        auth_type, encoded_credentials = authorization_header.split(" ")
        if auth_type != "Basic":
            raise ValueError("Invalid authorization type")
        
        decoded_credentials = base64.b64decode(encoded_credentials).decode("utf-8")
        username, password = decoded_credentials.split(":")
        return cls(username=username, password=password)

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