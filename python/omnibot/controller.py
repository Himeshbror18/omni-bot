"""High-level keyboard controller for the Omni-Bot WeMos D1 R32."""

from .protocol import RobotClient


class Controller:
    """Translate human-friendly keyboard input into robot movement commands.

    The ESP32 firmware remains responsible for wheel mixing and motor output.
    This class only chooses the firmware movement command and speed.
    """

    # Matches /move?c=<command> in firmware/src/main.cpp.
    COMMANDS = {
        "w": "f",      # forward
        "s": "b",      # reverse
        "a": "l",      # strafe left
        "d": "r",      # strafe right
        "q": "ccw",    # rotate counter-clockwise
        "e": "cw",     # rotate clockwise
        "u": "fl",     # forward-left diagonal
        "i": "fr",     # forward-right diagonal
        "j": "bl",     # backward-left diagonal
        "k": "br",     # backward-right diagonal
    }

    def __init__(self, client: RobotClient | None = None, speed: int = 70):
        self.client = client or RobotClient()
        self.speed = max(0, min(100, int(speed)))

    def set_speed(self, speed: int) -> None:
        """Set movement speed as a percentage from 0 to 100."""
        self.speed = max(0, min(100, int(speed)))

    def command(self, key: str) -> None:
        """Send a mapped movement command; stop for an unmapped key."""
        command = self.COMMANDS.get(key.lower())
        if command:
            self.client.move(command, self.speed)
        else:
            self.client.stop()

    def stop(self) -> None:
        """Request an explicit robot stop."""
        self.client.stop()
