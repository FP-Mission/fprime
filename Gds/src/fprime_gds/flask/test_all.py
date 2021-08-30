import flask_restful
import json
from os import listdir
from os.path import isfile, join
from fprime_gds.flask.app import db, TestModel

class TestAll(flask_restful.Resource):

    def get(self):
        """
        Returns test scripts.
        """
        try:
            res = [q.to_json() for q in TestModel.query.order_by(TestModel.date.desc()).all()]
            return res
        except Exception as e:
            print(e)
            return [-1]
         
        