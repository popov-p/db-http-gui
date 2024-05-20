from fastapi import FastAPI
from server.http_server.routers import router

app = FastAPI()
app.include_router(router)
