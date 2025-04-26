#!/bin/bash

# Exit immediately if any command fails
set -e
pyenv install 3.10.16
pyenv global 3.10.16
eval "$(pyenv init -)"
pip install opencv-python
pip install vessl