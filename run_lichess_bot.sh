#!/bin/bash

# Compile the engine
make
gcc src/sseh.c \
    src/init.c \
    src/bitboard.c \
    src/hashkey.c \
    src/board.c \
    src/data.c \
    src/attack.c \
    src/io.c \
    src/genmove.c \
    src/validate.c \
    src/makemove.c \
    src/perft.c \
    src/misc.c \
    src/search.c \
    src/pvtable.c \
    src/evaluate.c \
    src/uci.c \
    src/xboard.c \
    src/polybook.c \
    src/polykeys.c \
    -o ssehc \
    -O3 -Wall -Wextra -Wpedantic -Wno-unused-result -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-label -Wno-unused-label -Wno-unused-local-typedefs

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
