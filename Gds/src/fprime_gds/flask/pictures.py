import flask_restful

class Picture(flask_restful.Resource):

    def get(self):
        """
        Returns picture present.
        """
        try:
            with open(f'../data/picture.txt') as file: 
                lines = file.read().splitlines()
                return lines
        except:
            return [-1]
         
        