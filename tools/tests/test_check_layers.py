"""check_layers 的行为测试。"""

from __future__ import annotations

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
CHECKER = REPO_ROOT / "tools" / "check_layers.py"


class CheckLayersTest(unittest.TestCase):
    def run_checker(self, root: Path) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            [sys.executable, str(CHECKER), "--root", str(root), "--verbose"],
            capture_output=True,
            check=False,
            text=True,
            timeout=10,
        )

    @staticmethod
    def write_source(root: Path, relative: str, content: str) -> None:
        path = root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8")

    def test_empty_scan_fails(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            result = self.run_checker(Path(temp))

        self.assertEqual(2, result.returncode)
        self.assertIn("没有 .c/.h 文件", result.stderr)

    def test_legal_framework_source_passes(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            self.write_source(root, "framework/components/demo/demo.c", "#include <stdint.h>\n")
            result = self.run_checker(root)

        self.assertEqual(0, result.returncode, result.stdout + result.stderr)
        self.assertIn("1 个文件", result.stdout)

    def test_component_vendor_header_fails(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            self.write_source(
                root,
                "framework/components/demo/demo.c",
                '#include "stm32f4xx.h"\n',
            )
            result = self.run_checker(root)

        self.assertEqual(1, result.returncode)
        self.assertIn("framework/components/demo/demo.c:1", result.stdout)

    def test_port_vendor_header_passes(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            self.write_source(
                root,
                "framework/port/demo/demo.c",
                '#include "stm32f4xx.h"\n',
            )
            result = self.run_checker(root)

        self.assertEqual(0, result.returncode, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
