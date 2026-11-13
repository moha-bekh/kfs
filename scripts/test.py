"""Build a temporary debug ISO and check all suites in a separate QEMU guest.

Run inside the Linux VM. Existing interactive guests and builds are untouched.
"""

import pathlib
import re
import socket
import subprocess
import tempfile
import time

ROOT = pathlib.Path(__file__).resolve().parent.parent


def run_tests(directory):
    elf = directory / "build/kfs.elf"
    iso = directory / "tests.iso"
    build = subprocess.run(
        ["make", "debug-build", f"DEBUG_BUILD_DIR={directory / 'build'}",
         f"DEBUG_ISO={iso}"],
        cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
    )
    if build.returncode:
        print(build.stdout)
        raise SystemExit(build.returncode)
    subprocess.run(["grub-file", "--is-x86-multiboot2", str(elf)], check=True)

    # Use a separate port so an interactive GDB session can remain connected.
    with socket.socket() as listener:
        listener.bind(("127.0.0.1", 0))
        port = listener.getsockname()[1]
    guest = subprocess.Popen(
        ["qemu-system-i386", "-cdrom", str(iso), "-display", "none",
         "-monitor", "none", "-serial", "none", "-no-reboot", "-S",
         "-gdb", f"tcp:127.0.0.1:{port}"],
        stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, text=True,
    )
    try:
        time.sleep(0.5)
        if guest.poll() is not None:
            raise SystemExit(guest.stderr.read())
        # Stop after all results are initialized, before interactive polling.
        lines = (ROOT / "src/kernel/main.c").read_text().splitlines()
        line = next(i for i, text in enumerate(lines, 1)
                    if "uint8_t status" in text)
        expressions = [
            "set pagination off",
            f"target remote 127.0.0.1:{port}",
            f"break src/kernel/main.c:{line}",
            "continue",
            "print console_results",
            "print output_results",
            "print keyboard_results",
            "print console_results.all_passed && output_results.all_passed "
            "&& keyboard_results.all_passed",
        ]
        command = ["gdb", "-q", "-batch", str(elf)]
        for expression in expressions:
            command.extend(["-ex", expression])
        result = subprocess.run(command, cwd=ROOT, capture_output=True,
                                text=True, timeout=60)
        print(result.stdout)
        if result.stderr:
            print(result.stderr)
        if result.returncode or not re.search(r"^\$4 = 1\b", result.stdout, re.M):
            raise SystemExit("FAIL: kernel suites failed or did not complete")
        print("PASS: 62 scenarios in QEMU; Multiboot 2 validated")
    finally:
        guest.terminate()
        try:
            guest.wait(timeout=5)
        except subprocess.TimeoutExpired:
            guest.kill()
            guest.wait()
        guest.stderr.close()


if __name__ == "__main__":
    with tempfile.TemporaryDirectory(prefix="kfs-tests-") as temporary:
        run_tests(pathlib.Path(temporary))
