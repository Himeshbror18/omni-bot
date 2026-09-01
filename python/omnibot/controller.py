"""High-level keyboard-friendly controller for Omni-Bot."""

from .protocol import RobotClient


class Controller:
    """Translate human-friendly keys into ESP32 movement commands."""

    COMMANDS = {
        "w": "f",
        "s": "b",
        "a": "l",
        "d": "r",
        "q": "ccw",
        "e": "cw",
        "u": "fl",
        "i": "fr",
        "j": "bl",
        "k": "br",
    }

    def __init__(self, client: RobotClient | None = None, speed: int = 70):
        self.client = client or RobotClient()
        self.speed = max(0, min(100, int(speed)))

    def set_speed(self, speed: int) -> None:
        self.speed = max(0, min(100, int(speed)))

    def command(self, key: str) -> None:
        """Send a movement command or stop for an unknown key."""
        command = self.COMMANDS.get(key.lower())
        if command:
            self.client.move(command, self.speed)
        else:
            self.client.stop()

    def stop(self) -> None:
        self.client.stop()
