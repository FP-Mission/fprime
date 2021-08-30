import flask_restful
import json
from os import listdir
from os.path import isfile, join
from fprime_gds.flask.app import db, TestModel
from sqlalchemy import func

class TestHistory(flask_restful.Resource):

    def get(self):
        """
        Returns test scripts.
        """
        try:
            res = [q[0].to_json() for q in TestModel.query.with_entities(TestModel, func.max(TestModel.date)).group_by(TestModel.test).all()]
            return res
        except Exception as e:
            print(e)
            return [-1]
         
        