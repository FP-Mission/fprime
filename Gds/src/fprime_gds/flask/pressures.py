import flask_restful
import json

class Pressure(flask_restful.Resource):

    def get(self):
        """
        Returns pressure present.
        """
        try:
            with open('/mnt/c/dev/HE-ARC/github/pressure.txt') as json_file:
                json_data = json.load(json_file)
                return json_data
        except Exception as e:
            print(e)
            return [-1]
         
        