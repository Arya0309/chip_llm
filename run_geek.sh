#!/bin/bash

# Navigate to the directory
cd ./systemc_generation/dataset/geek/output/ || { echo "Directory not found!"; exit 1; }

# Execute the Python script
python3 check.py
