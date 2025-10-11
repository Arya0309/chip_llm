import json
from pathlib import Path
from typing import List, Dict


class Agent:
    def __init__(self, *args, **kwargs):
        self.memory = []

    def load_memory(self, json_path: str):
        self.questions = ...
