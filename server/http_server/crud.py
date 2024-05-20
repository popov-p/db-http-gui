from server.http_server import models, schemas
from sqlalchemy.orm import Session
from sqlalchemy.exc import SQLAlchemyError, NoResultFound
import logging
from fastapi import HTTPException

def create_student(db: Session, student: schemas.Student):
    try:
        with db.begin():
            db_student = models.Student(**student.dict())
            db.add(db_student)
            db.flush()
            #print("here is ---:", db_student)
            return db_student
    except SQLAlchemyError as e:
        logging.error(f"Database error: {e}")
        raise HTTPException(status_code=500, detail="Database error")
    except Exception as e:
        logging.error(f"Error creating student: {e}")
        raise HTTPException(status_code=500, detail="Internal server error")

def get_student(db: Session, student_id: int):
        student = db.query(models.Student).filter(models.Student.id == student_id).first()
        if not student:
            logging.error(f"Student with id {student_id} not found")
            raise HTTPException(status_code=404, detail="Student not found")
        return student

def get_all_students(db: Session):
    try:
        return db.query(models.Student)
    except Exception as e:
        logging.error(f"Error getting all students: {e}")
        raise HTTPException(status_code=500, detail="Internal server error")

def delete_student(db: Session, student_id: int):
    try:
        with db.begin():
            db_student = db.query(models.Student).filter(models.Student.id == student_id).first()
            if not db_student:
                raise HTTPException(status_code=404, detail="Student not found")
            db.delete(db_student)
    except NoResultFound:
        logging.error(f"Student with id {student_id} not found")
        raise HTTPException(status_code=404, detail="Student not found")
    except SQLAlchemyError as e:
        logging.error(f"Database error: {e}")
        raise HTTPException(status_code=500, detail="Database error")
    except Exception as e:
        logging.error(f"Error deleting student: {e}")
        raise HTTPException(status_code=500, detail="Internal server error")

def delete_all_students(db: Session):
    try:
        with db.begin():
            num_deleted = db.query(models.Student).delete()
            return num_deleted
    except SQLAlchemyError as e:
        logging.error(f"Database error: {e}")
        raise HTTPException(status_code=500, detail="Database error")
    except Exception as e:
        logging.error(f"Error deleting all students: {e}")
        raise HTTPException(status_code=500, detail="Internal server error")

def filter(db:Session, **kwargs):
    print(kwargs)
    query = db.query(models.Student)
    for key, value in kwargs.items():
        if key == "str":
            query = query.filter(getattr(models.Student, value[0]).ilike(f"{value[1]}%"))
        elif key == "int":
            comp_arg = kwargs.get("bool")
            if comp_arg:
                comp_field = kwargs.get("int")
                if comp_arg[0] == "and_less":
                    query = query.filter(getattr(models.Student, 
                                                 comp_field[0]) <= comp_field[1])
                if comp_arg[0] == "and_greater":
                    query = query.filter(getattr(models.Student, 
                                                 comp_field[0]) >= comp_field[1])
            else:
                query = query.filter(getattr(models.Student, value[0]) == value[1])
    return [student.id for student in query.all()]

def id_by_data(db: Session, student: schemas.StudentCreate):
    db_students_query = db.query(models.Student).filter_by(
        last_name=student.last_name,
        first_name=student.first_name,
        patronymic=student.patronymic,
        group=student.group,
        year=student.year,
        course=student.course,
        photo=student.photo
    )

    if db_students_query.first():
        return [db_student.id for db_student in db_students_query]
    else:
        return [-1]