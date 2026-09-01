"""Minimal terminal controller for Omni-Bot."""

from .controller import Controller


def main() -> None:
    controller = Controller()
    print("Omni-Bot Python controller")
    print("W/A/S/D = drive, Q/E = rotate, X = stop, Ctrl+C = stop")

    try:
        while True:
            key = input("> ").strip()[:1].lower()
            if key == "x":
                controller.client.stop()
            elif key:
                controller.command(key)
    except KeyboardInterrupt:
        controller.client.stop()
        print("\nStopped.")


if __name__ == "__main__":
    main()
