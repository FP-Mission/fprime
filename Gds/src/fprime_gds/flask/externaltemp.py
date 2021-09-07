import flask_restful
import json

class ExternalTemp(flask_restful.Resource):

    def get(self):
        """
        Returns temperature present.
        """
        try:
            with open('../data/externalTemp.txt') as json_file:
                json_data = json.load(json_file)
                return json_data
        except Exception as e:
            #print(e)
            return [-1]
         
        