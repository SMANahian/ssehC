#!/bin/bash

# Compile the engine
make

# Put the engine in the engines folder
cp ssehc lichess-bot/engines/

# Copy the default config file
cp config.yml.default lichess-bot/config.yml

# Change directory to the bot
cd lichess-bot

# Create a virtual environment and install the requirements
python3 -m venv venv
virtualenv venv -p python3
source ./venv/bin/activate
python3 -m pip install -r requirements.txt

# Inject LICHESS_TOKEN into config.yml
sed -i "s|\${LICHESS_TOKEN}|$LICHESS_TOKEN|g" config.yml

python3 lichess-bot.py -u
