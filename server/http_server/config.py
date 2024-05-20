import os
from configparser import ConfigParser
from .utils import init_config

class AppConfig:
    def __init__(self, config_file_path: str):
        self.config = ConfigParser()
        self.config.read(config_file_path)
        self.username = self.config.get('auth', 'username')
        self.hashed_password = self.config.get('auth', 'password')
        self.log_dir = self.config.get('logging', 'dir')
        self.severity = int(self.config.get('logging', 'severity'))

config_file_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'cfg.ini')
init_config(config_file_path)
app_config = AppConfig(config_file_path)