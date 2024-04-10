from configparser import ConfigParser
from passlib.context import CryptContext
import os
def init_config(config_file_path):
    config = ConfigParser()

    if not config.has_section("logging"):
        config.add_section("logging")

    log_dir = input("Input log directory: ")
    print(f"--------------------------------------logdir: qwe {log_dir}")
    config.set("logging", "dir", os.path.abspath(log_dir))

    log_severity = ''
    while log_severity.upper() not in ["ERROR", "INFO"]:       
        log_severity = input("Input log severity: [ERROR\\INFO] ").upper()
    config.set("logging", "severity", log_severity)

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