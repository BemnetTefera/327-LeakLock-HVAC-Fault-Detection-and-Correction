# ========== Tornado SERVER for Multi-ESP32 HVAC ==========
import os
import tornado.ioloop
import tornado.web
import tornado.websocket
import tornado.httpserver
import json

clients_ui = set()
clients_esp = set()
client_id_map = {}  # Map device_id -> ESP32 socket

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("templates/index.html")

class UIWebSocketHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        print("Web client connected")
        clients_ui.add(self)

    def on_close(self):
        print("Web client disconnected")
        clients_ui.discard(self)

    def on_message(self, message):
        print(f"UI sent: {message}")
        try:
            msg = json.loads(message)
            target_id = msg.get("device_id")
            if target_id and target_id in client_id_map:
                client_id_map[target_id].write_message(message)
            else:
                print("Device not found or not connected")
        except Exception as e:
            print(f"Error forwarding UI message: {e}")

    def check_origin(self, origin):
        return True

class ESPWebSocketHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        print("ESP32 connected")
        self.device_id = None
        clients_esp.add(self)

    def on_close(self):
        print("ESP32 disconnected")
        clients_esp.discard(self)
        if self.device_id and self.device_id in client_id_map:
            del client_id_map[self.device_id]

    def on_message(self, message):
        print(f"ESP sent: {message}")
        try:
            data = json.loads(message)
            if "device_id" in data:
                if len(data) == 1:
                    # Initial handshake
                    self.device_id = data["device_id"]
                    client_id_map[self.device_id] = self
                    print(f"ESP registered as {self.device_id}")
                    return
                # Forward data to UI
                for client in clients_ui:
                    client.write_message(message)
        except Exception as e:
            print(f"Error processing ESP message: {e}")

    def check_origin(self, origin):
        return True


def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/ws", UIWebSocketHandler),
        (r"/ws_esp32", ESPWebSocketHandler),
        (r"/static/(.*)", tornado.web.StaticFileHandler, {"path": os.path.join(os.path.dirname(__file__), "templates/static")}),
    ], debug=True)

if __name__ == "__main__":
    app = make_app()
    server = tornado.httpserver.HTTPServer(app)
    server.listen(8888)
    print("Tornado server running at http://localhost:8888")
    tornado.ioloop.IOLoop.current().start()
