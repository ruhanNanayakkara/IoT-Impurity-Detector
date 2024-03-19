import threading
import serial
import pymysql
from flask import Flask, render_template

_distVertical = -1.0 # Initialise Vertical Distance
_distHorizontal = -1.0 # Initialise Horizontal Distance
_verticalBaseValue = -1.0 # Initialise Vertical Base Distance
_wallDistance = 25 # Initialise Distance To Wall
_obstruction = False # Initialise Obstruction Value To false
_wall = False # Initialise Wall Value to false

ser = serial.Serial('/dev/ttyS0',9600, timeout = 1) # Establish Serial Transmition

app = Flask(__name__) # Create Flask Object
@app.route('/') # Assign Base Route

# Retrives Data From Database And Renders In HTML Page
def index():
    global _wallDistance
    
    selectQuery = "SELECT obstruction, wall, moving FROM readings ORDER BY reading_id DESC LIMIT 1"
    
    dbConn = pymysql.connect("localhost", "pi", "", "assignment_2_db") or die("Could not connect.")
    cursor = dbConn.cursor()
    cursor.execute(selectQuery)
    
    for (obstruction, wall, moving) in cursor:
        global dbObs
        dbObs = obstruction
        global dbWall
        dbWall = wall
        global dbMoving
        dbMoving = moving
    cursor.close
    
    vals = {'obstruction' : dbObs,
            'wall' : dbWall,
            'moving' : dbMoving,
            'dist' : _wallDistance}
    
    return render_template('index.html', **vals)

# Sets Route For Next Page
@app.route("/dist/<value>")

# Changes Distance Value Depdening On Value Entered By User
def dist(value):
    global _wallDistance
    _wallDistance = int(value)
    
    # Redirect To Index Page
    return render_template('redir.html')

# Check If The Motor Should Run Based On The Values Provided In Readings
def shouldRun(readings):
    global _wallDistance

    for val in readings:
        print(val, end=" ")
        if (len(val) > 1):
            if (val[0] == "v"):
                global _distVertical
                _distVertical = float(val[1:])
            elif (val[0] == "h"):
                global _distHorizontal
                _distHorizontal = float(val[1:])
            elif (val[0] == "b"):
                global _verticalBaseValue
                _verticalBaseValue = float(val[1:])
                
    if (_distVertical != -1.0 and _distHorizontal != -1.0 and _verticalBaseValue != -1.0):
        # Check If An Impurity Is Detected
        if (abs(_distVertical - _verticalBaseValue) > 0.2 * _verticalBaseValue): # Impurity Detected
            global _obstruction
            _obstruction = True
            return False
        else: _obstruction = False
        
        # Check If Wall Is Detected
        if (_distHorizontal <= _wallDistance): # Wall Ahead
            global _wall
            _wall = True
            return False
        else: _wall = False
        
        return True
    
# Loop To Continuously Read Values From Arduino, Send Data To Database And Send Signals To Arduino
def serialLoop():
    while True:
        line = ser.readline().decode('utf-8').rstrip()
        readings = line.split(',', 2)
        
        decision = shouldRun(readings)
        
        # Connect To Database If Values Have Been Changed
        if (_distVertical != -1.0 and _distHorizontal != -1.0 and _verticalBaseValue != -1.0):
            
            dbConn = pymysql.connect("localhost", "pi", "", "assignment_2_db") or die("Could not connect.")
            cursor = dbConn.cursor()
            
            # INSERT Values Into Database
            insertQuery = "INSERT INTO readings (base_vertical_distance, vertical_distance, horizontal_distance, obstruction, wall, moving) VALUES (%s, %s, %s, %s, %s, %s)"
            values = (_verticalBaseValue, _distVertical, _distHorizontal, _obstruction, _wall, not(_obstruction or _wall))
            cursor.execute(insertQuery, values)
            dbConn.commit()
            cursor.close()
            dbConn.close()
            
        print(decision)
        finalDecision = str(decision) + "\n"
        
        # Send Data To Arduino To Perform Necessary Actions
        ser.write(finalDecision.encode('utf-8'))

# Main Code
if __name__ == "__main__":
    # Create New Thread So That Loop And Web Server Proccesses Can Run Concurrently
    t = threading.Thread(target=serialLoop)
    t.start()
    
    app.run(host='0.0.0.0', port=80, debug=True, use_reloader=False)