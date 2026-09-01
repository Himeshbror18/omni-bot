"""High-level controller with a simple keyboard-friendly API."""

from .protocol import RobotClient


class Controller:
    COMMANDS = {
        "w": "f",
        "s": "b",
        "a": "l",
        "d": "r",
        "q": "ccw",
        "e": "cw",
    }

    def __init__(self, client: RobotClient | None = None, speed: int = 70):
        self.client = client or RobotClient()
        self.speed = speed

    def command(self, key: str) -> None:
        command = self.COMMANDS.get(key.lower())
        if command:
            self.client.move(command, self.speed)
        else:
            self.client.stop()
