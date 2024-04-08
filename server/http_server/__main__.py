
from fastapi import FastAPI
from fastapi import Depends
from sqlalchemy.orm import sessionmaker
from fastapi.exceptions import HTTPException

from server.http_server import models, schemas
from server.http_server.models import Student, engine
from server.http_server.schemas import StudentCreate, StudentUpdate

from server.http_server import crud

app = FastAPI()

Session = sessionmaker(bind=engine)

def get_db():
    db = Session()
    try:
        yield db
    finally:
        db.close()

@app.get("/")
def root():
    return {"message": "Hello World"}

# Create student
@app.post("/students/", response_model=schemas.StudentCreate)
def create_student(student: schemas.StudentCreate, db: Session = Depends(get_db)):
    return crud.create_student(db=db, student=student)


# Read student by ID
@app.get("/students/{student_id}", response_model=schemas.Student)
def read_student(student_id: int, db: Session = Depends(get_db)):
    db_student = crud.get_student(db, student_id=student_id)
    if db_student is None:
        raise HTTPException(status_code=404, detail="Student not found")
    return db_student


# Update student by ID 
@app.put("/students/{student_id}", response_model=schemas.Student)
def update_student(student_id: int, student: schemas.StudentUpdate, db: Session = Depends(get_db)):
    db_student = crud.get_student(db, student_id=student_id)
    if db_student is None:
        raise HTTPException(status_code=404, detail="Student not found")
    return crud.update_student(db=db, student=student, student_id=student_id)

# Delete student by ID 
@app.delete("/students/{student_id}")
def delete_student(student_id: int, db: Session = Depends(get_db)):
    db_student = crud.get_student(db, student_id=student_id)
    if db_student is None:
        raise HTTPException(status_code=404, detail="Student not found")
    crud.delete_student(db=db, student_id=student_id)
    return {"message": "Student deleted successfully"}

# Delete all students 
@app.delete("/students/", response_model=int)
def delete_all_students(db: Session = Depends(get_db)):
    num_deleted = crud.delete_all_students(db=db)
    return {"message": f"{num_deleted} students deleted successfully"}