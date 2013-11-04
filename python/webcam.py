
#! /usr/bin/python
#
#  Process_Commands.py   Michael Ball Sept 2013
#  Manage command/response between Arduino Motor/Sensor controller, and Raspberry Pi intelligent controller.
#  Using MySQL to store commands and telemetry and serial via USB to communicate with Arduino

import os
import serial
from gps import *
from time import *
import time
import threading
import datetime
import Image
import ImageStat

from math import *

from config_settings import *

import numpy
import MySQLdb
import sys
from sets import Set
 
###############################################################################################################
# Global Variables

imgpath = '/home/pi/Dropbox/webcam.jpg'                                 # fix to use Global variable 'HOME'
thumbpath = '/home/pi/Dropbox/thumbnail.jpg'                            # fix to use Global variable 'HOME'
sql_get_gps_uid = "SELECT MAX(uid) FROM gps;"
sql_get_cmd_uid = "SELECT MAX(uid) FROM cmd;"
sql = "INSERT INTO webcam (gps_uid,cmd_uid,camimage,thumb) VALUES (%s, %s, %s, %s)"

try:
    running = True
 
    db = MySQLdb.connect(HOST, USER, PW, DBASE)
    cursor = db.cursor()

    cursor.execute(sql_get_gps_uid)          # Get current GPS_UID
    gps = cursor.fetchone()
    gps_uid = gps[0]

    cursor.execute(sql_get_cmd_uid)          # Get current CMD_UID
    cmd = cursor.fetchone()
    cmd_uid = cmd[0]


    os.system("fswebcam -r 640x480 -d /dev/video0 -F 1 -S 5 -s lights=on /home/pi/Dropbox/webcam.jpg 2>&1")
    os.system("convert /home/pi/Dropbox/webcam.jpg -thumbnail 80x60 /home/pi/Dropbox/thumbnail.jpg 2>&1")
    
    if os.path.exists(imgpath):
        with open(imgpath) as fh:
            jpeg = fh.read()
 
        with open(thumbpath) as th:
            thumb = th.read()

        cursor.execute(sql, (gps_uid, cmd_uid, jpeg, thumb,))
        db.commit()
 
 
except (KeyboardInterrupt, SystemExit): #when you press ctrl+c
    print "\nKilling Thread..."
    db.close()
print "Done.\nExiting."



