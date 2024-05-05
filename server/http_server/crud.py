from server.http_server import models, schemas
from sqlalchemy.orm import Session

def create_student(db: Session, student: schemas.Student):
    db_student = models.Student(**student.dict())
    db.add(db_student)
    db.commit()
    db.refresh(db_student)
    return db_student

def get_student(db: Session, student_id: int):
    return db.query(models.Student).filter(models.Student.id == student_id).first()

def get_all_students(db: Session):
    return db.query(models.Student)

def delete_student(db: Session, student_id: int):
    db_student = db.query(models.Student).filter(models.Student.id == student_id).first()
    db.delete(db_student)
    db.commit()

def delete_all_students(db: Session):
    num_deleted = db.query(models.Student).delete()
    db.commit()
    return num_deleted

def filter(db: Session, keyfields: schemas.Filter):
    query = db.query(models.Student)

    filters = [
        getattr(models.Student, field) == value
        for field, value in keyfields.dict().items()
        if value is not None
    ]

    if filters:
        query = query.filter(and_(*filters))

    student_ids = [result[0] for result in results]
    
    return student_ids

