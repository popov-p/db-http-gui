
from fastapi import FastAPI
from fastapi import Depends
from sqlalchemy.orm import sessionmaker
from server.http_server import models, schemas
from server.http_server.models import Student, engine
from server.http_server.schemas import StudentCreate,StudentUpdate

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

@app.post("/students/", response_model=schemas.StudentCreate)
def create_student(student: schemas.StudentCreate, db: Session = Depends(get_db)):
    db_student = models.Student(**student.__dict__)
    db.add(db_student)
    db.commit()
    db.refresh(db_student)
    return db_student