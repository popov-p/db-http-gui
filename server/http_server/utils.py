from configparser import ConfigParser
from passlib.context import CryptContext
import os
import logging
from logging.handlers import RotatingFileHandler

def init_config(config_file_path):
    if os.path.exists(config_file_path):
        config = ConfigParser()
        config.read(config_file_path)
        while input("Authorize a new client? [y\\N]").lower() == 'y':
            login = input("Input login: ")
            password = input("Input password: ")
        
            pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")
            hashed_password = pwd_context.hash(password)

            if not config.has_section("auth"):
                config.add_section("auth")

            config.set("auth", "username", login)
            config.set("auth", "password", hashed_password)
            break

        with open(config_file_path, 'w') as configfile:
           config.write(configfile)
    else:
        config = ConfigParser()
        # [logging]
        config.add_section("logging")
        log_dir = input("Input log directory: ")
        config.set("logging", "dir", os.path.abspath(log_dir))
        
        log_severity = ''
        while log_severity.upper() not in ["ERROR", "INFO"]:       
            log_severity = input("Input log severity: [ERROR\\INFO] ").upper()
            if log_severity == "ERROR":
                config.set("logging", "severity", str(logging.ERROR))
            else:
                config.set("logging", "severity", str(logging.INFO))
        # [auth]
        config.add_section("auth")
        login = input("Input login: ")
        password = input("Input password: ")
        
        pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")
        hashed_password = pwd_context.hash(password)
        config.set("auth", "username", login)
        config.set("auth", "password", hashed_password)

        with open(config_file_path, 'w') as configfile:
            config.write(configfile)


def setup_logger(name, log_dir, level=logging.INFO, console=False, file=True):
    logger = logging.getLogger(name)
    logger.setLevel(level)

    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    
    if console:
        console_handler = logging.StreamHandler()
        console_handler.setLevel(level)
        console_handler.setFormatter(formatter)
        logger.addHandler(console_handler)

    if file:
        os.makedirs(log_dir, exist_ok=True)
        file_handler = RotatingFileHandler(
            os.path.join(log_dir, 'backend.log'), maxBytes=10*1024*1024, backupCount=5
        )
        file_handler.setLevel(level)
        file_handler.setFormatter(formatter)
        logger.addHandler(file_handler)

    return logger