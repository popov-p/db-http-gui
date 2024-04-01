from . import database, models
from fastapi import FastAPI


app = FastAPI()


@app.get("/")
def root():
    return {"message": "Hello World"}
