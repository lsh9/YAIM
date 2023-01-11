import flask
import requests
import os

app = flask.Flask(__name__)
PATH = "file/"


@app.route('/upload file', method=['POST'])
def upload_file():
    file = requests.files['file']
    filename = file.filename
    file.save(os.path.join(PATH, filename))
    return {"url": filename}


@app.route('/download file/<filename>', method=['GET'])
def download_file(filename):
    with open(os.path.join(PATH, filename), 'rb') as f:
        file = f.read()
    return flask.send_file(file)


if __name__ == '__main__':
    app.run()
