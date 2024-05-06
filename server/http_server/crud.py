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

def filter(db:Session, **kwargs):
    print(kwargs)
    query = db.query(models.Student)
    for key, value in kwargs.items():
        if key == "str":
            query = query.filter(getattr(models.Student, value[0]).startswith(value[1]))
        elif key == "int":
            comp_arg = kwargs.get("bool")
            if comp_arg:
                if comp_arg[0] == "and_less":
                    print("--wtf--0", kwargs.get("int")[0], "--wtf--1", kwargs.get("int")[1])
                    query = query.filter(getattr(models.Student, 
                                                 kwargs.get("int")[0]) <= int(kwargs.get("int")[1]))
                if comp_arg[0] == "and_greater":
                    print("--wtf--0", kwargs.get("int")[0], "--wtf--1", kwargs.get("int")[1])
                    query = query.filter(getattr(models.Student, 
                                                 kwargs.get("int")[0]) >= int(kwargs.get("int")[1]))
            else:
                query = query.filter(getattr(models.Student, value[0]) == value[1])
    #print(query.all())
    return [student.id for student in query.all()]
