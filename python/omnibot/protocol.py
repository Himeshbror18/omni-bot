"""HTTP transport for the current Omni-Bot ESP32 firmware.

The endpoint names and parameters mirror firmware/src/main.cpp:
    /move?c=<command>&v=<0..100>
    /motor?i=<0..3>&v=<-100..100>
""" 

from dataclasses import dataclass
import requests


@dataclass
class RobotClient:
    base_url: str = "http://192.168.4.1"
    timeout: float = 0.35

    def _get(self, path: str, **params) -> None:
        requests.get(
            f"{self.base_url.rstrip('/')}{path}",
            params=params,
            timeout=self.timeout,
        ).raise_for_status()

    def move(self, command: str, speed: int = 70) -> None:
        speed = max(0, min(100, int(speed)))
        self._get("/move", c=command, v=speed)

    def stop(self) -> None:
        # The ESP32 treats command "s" as stop via its default branch.
        self._get("/move", c="s", v=0)

    def motor(self, index: int, value: int) -> None:
        if index not in range(4):
            raise ValueError("motor index must be 0..3")
        value = max(-100, min(100, int(value)))
        self._get("/motor", i=index, v=value)

    def close(self) -> None:
        """Best-effort stop before the client is discarded."""
        try:
            self.stop()
        except requests.RequestException:
            pass
