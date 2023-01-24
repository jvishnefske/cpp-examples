import socket
import sqlite3
import http.server
import json


class MyHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        self.wfile.write(json.dumps({"ip": socket.gethostbyname(socket.gethostname())}).encode())


# database layer
class Database:
    def __init__(self, db_name):
        self.conn = sqlite3.connect(db_name)
        self.cursor = self.conn.cursor()

    def __del__(self):
        self.conn.close()

    def get_all_data(self):
        self.cursor.execute("SELECT * FROM data")
        return self.cursor.fetchall()

    def get_data(self, id):
        self.cursor.execute("SELECT * FROM data WHERE id=?", (id,))
        return self.cursor.fetchone()

    def add_data(self, data):
        self.cursor.execute("INSERT INTO data(data) VALUES (?)", (data,))
        self.conn.commit()
        return self.cursor.lastrowid

    def update_data(self, id, data):
        self.cursor.execute("UPDATE data SET data=? WHERE id=?", (data, id))
        self.conn.commit()

    def delete_data(self, id):
        self.cursor.execute("DELETE FROM data WHERE id=?", (id,))
        self.conn.commit()


# business logic layer
class BusinessLogic:
    def __init__(self, database):
        self.db = database

    def get_all_data(self):
        return self.db.get_all_data()

    def get_data(self, id):
        return self.db.get_data(id)

    def add_data(self, data):
        return self.db.add_data(data)

    def update_data(self, id, data):
        self.db.update_data(id, data)

    def delete_data(self, id):
        self.db.delete_data(id)


# data transfer object
class DataTransferObject:
    def __init__(self):
        pass

    def get_all_data_dto(self, data):
        dto = []
        for row in data:
            dto.append({"id": row[0], "data": row[1]})
        return dto

    def get_data_dto(self, data):
        return {"id": data[0], "data": data[1]}


# controller layer
class Controller:
    def __init__(self, business_logic):
        self.business_logic = business_logic
        self.dto = DataTransferObject()

    def get_all_data(self):
        data = self.business_logic.get_all_data()
        return self.dto.get_all_data_dto(data)

    def get_data(self, id):
        data = self.business_logic.get_data(id)
        return self.dto.get_data_dto(data)

    def add_data(self, data):
        id = self.business_logic.add_data(data)
        return self.get_data(id)

    def update_data(self, id, data):
        self.business_logic.update_data(id, data)
        return self.get_data(id)

    def delete_data(self, id):
        self.business_logic.delete_data(id)


# interface layer
class InterfaceLayer:
    def __init__(self, controller):
        self.controller = controller

    def get_all_data(self):
        return self.controller.get_all_data()

    def get_data(self, id):
        return self.controller.get_data(id)

    def add_data(self, data):
        return self.controller.add_data(data)

    def update_data(self, id, data):
        return self.controller.update_data(id, data)

    def delete_data(self, id):
        self.controller.delete_data(id)


# client
class Client:
    def __init__(self, interface_layer):
        self.interface_layer = interface_layer

    def get_all_data(self):
        return self.interface_layer.get_all_data()

    def get_data(self, id):
        return self.interface_layer.get_data(id)

    def add_data(self, data):
        return self.interface_layer.add_data(data)

    def update_data(self, id, data):
        return self.interface_layer.update_data(id, data)

    def delete_data(self, id):
        self.interface_layer.delete_data(id)


# run
def instance(database_name="database.db"):
    database = Database(database_name)
    business_logic = BusinessLogic(database)
    controller = Controller(business_logic)
    interface_layer = InterfaceLayer(controller)
    client = Client(interface_layer)

    # create table
    database.cursor.execute("""CREATE TABLE IF NOT EXISTS data (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        data TEXT
    )""")

    # add data
    client.add_data("data1")
    client.add_data("data2")
    client.add_data("data3")

    # get data
    print(client.get_data(1))
    print(client.get_data(2))
    print(client.get_data(3))

    # update data
    client.update_data(1, "data1 updated")
    print(client.get_data(1))

    # delete data
    client.delete_data(1)
    print(client.get_all_data())

    # run server
    httpd = http.server.HTTPServer(('', 8000), MyHandler)
    return httpd


def test_server():
    assert isinstance(instance(":memory:"), http.server.HTTPServer)


if __name__ == "__main__":
    instance().serve_forever()
