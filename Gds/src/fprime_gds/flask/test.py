import flask_restful
import json
from os import listdir
from os.path import isfile, join
from fprime_gds.flask.app import db, TestModel
import time

class Test(flask_restful.Resource):

    def __init__(self):
        self.parser_post = flask_restful.reqparse.RequestParser()
        self.parser_post.add_argument("script", required=True)
        self.script_path = "../data/testScript/"


    def get(self):
        """
        Returns test scripts.
        """
        try:
            files = [f for f in listdir(self.script_path) if isfile(join(self.script_path, f))]
            return files
        except Exception as e:
            print(e)
            return [-1]

    def post(self):
        data = self.parser_post.parse_args()
        success = False
        try:
            exec(open(f"{self.script_path}{data['script']}").read())
            success = True
        except:
            success = False

        test = TestModel(date=time.time(), test=data['script'],success=success)
        db.session.add(test)
        db.session.commit()
         
        