"""High-level keyboard controller for the Omni-Bot WeMos D1 R32."""

from .protocol import RobotClient


class Controller:
    """Translate keyboard input into safe robot movement commands.

    The ESP32 firmware remains responsible for wheel mixing and motor output.
    This class tracks movement-key state so unrelated keyboard events do not
    accidentally stop the robot.
    """

    COMMANDS = {
        "w": "f", "s": "b", "a": "l", "d": "r",
        "q": "ccw", "e": "cw",
        "u": "fl", "i": "fr", "j": "bl", "k": "br",
    }

    def __init__(self, client: RobotClient | None = None, speed: int = 70):
        self.client = client or RobotClient()
        self.speed = max(0, min(100, int(speed)))
        self._pressed: set[str] = set()
        self._active_key: str | None = None

    def set_speed(self, speed: int) -> None:
        """Set movement speed as a percentage from 0 to 100."""
        self.speed = max(0, min(100, int(speed)))

    def key_down(self, key: str) -> None:
        """Start movement for a mapped key; ignore unrelated keys."""
        key = key.lower()
        if key not in self.COMMANDS:
            return
        self._pressed.add(key)
        self._active_key = key
        self.client.move(self.COMMANDS[key], self.speed)

    def key_up(self, key: str) -> None:
        """Stop when the active movement key is released."""
        key = key.lower()
        self._pressed.discard(key)
        if key == self._active_key:
            self._active_key = next(iter(self._pressed), None)
            if self._active_key is None:
                self.client.stop()
            else:
                self.client.move(self.COMMANDS[self._active_key], self.speed)

    def command(self, key: str) -> None:
        """Backward-compatible one-shot command interface."""
        key = key.lower()
        if key in self.COMMANDS:
            self.key_down(key)
            self.key_up(key)

    def stop(self) -> None:
        """Explicitly stop the robot and clear local movement state."""
        self._pressed.clear()
        self._active_key = None
        self.client.stop()
