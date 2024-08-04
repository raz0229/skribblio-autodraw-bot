# screen_capture.py

from pynput import mouse

class ScreenCapture:
    def __init__(self):
        self.start_x = None
        self.start_y = None
        self.end_x = None
        self.end_y = None
        self.listener = mouse.Listener(on_click=self.on_click)
        self.listener.start()

    def on_click(self, x, y, button, pressed):
        if pressed:
            self.start_x = x
            self.start_y = y
            self.listener.stop()
            self.capture_area()

    def capture_area(self):
        if self.start_x is not None:
            print(f"Start Area: Start({self.start_x}, {self.start_y})")
