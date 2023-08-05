#! /usr/bin/env python3
from datetime import datetime
import sqlite3
from flask import Flask, render_template

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///your_database_file.db'

@app.route('/')
def stats():
    conn = get_db_connection()
    stats = conn.execute("SELECT * FROM stats WHERE probe = 'humidity' ORDER BY id DESC LIMIT 100").fetchall()
    humidities_metrics = []
    humidities_ts = []
    for row in stats:
        id, created, ts, probe, metric = row
        humidities_metrics.append(metric)
        humidities_ts.append(ts)
    humidities_metrics.reverse()
    humidities_ts.reverse()

    stats = conn.execute("SELECT * FROM stats WHERE probe = 'temperature' ORDER BY id DESC LIMIT 100").fetchall()
    temperature_metrics = []
    temperature_ts = []
    for row in stats:
        id, created, ts, probe, metric = row
        temperature_metrics.append(metric)
        temperature_ts.append(ts)
    temperature_metrics.reverse()
    temperature_ts.reverse()

    return render_template('index.html', 
                            data_y_humidity=humidities_metrics, 
                            data_x_humidity=humidities_ts,
                            data_y_temperature=temperature_metrics, 
                            data_x_temperature=temperature_ts)

def get_db_connection() -> sqlite3.Connection:
    conn = sqlite3.connect('/tmp/database.db')
    conn.row_factory = sqlite3.Row
    return conn