import os
from histo import histogram

if __name__ == '__main__':
    ROOT_DIR = os.path.abspath("")
    MODEL_PATH = os.path.join(ROOT_DIR)
    histogram(MODEL_PATH)