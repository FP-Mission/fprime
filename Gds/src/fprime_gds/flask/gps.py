import flask_restful
import json

class Gps(flask_restful.Resource):

    def get(self):
        """
        Returns pressure present.
        """
        try:
            with open('../data/gps.txt') as json_file:
                json_data = json.load(json_file)
                return json_data
        except Exception as e:
            return [-1]
         
        