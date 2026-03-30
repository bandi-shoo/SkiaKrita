#!/usr/bin/env python3
"""
SkiaKrita UML Diagram Generator
================================
Recursively finds .puml files under  docs/uml/  and renders them
to SVG in  docs/uml/out/  using PlantUML.

Usage
-----
    python tools/uml/generate.py

PlantUML detection (checked in order)
-------------------------------------
1. PLANTUML_JAR  environment variable  →  java -jar $PLANTUML_JAR
2. tools/uml/plantuml.jar  (local copy) →  java -jar ...
3. 'plantuml' on PATH                   →  plantuml  (CLI)
"""

from __future__ import annotations

import os
import shutil
import subprocess
import sys
from pathlib import Path

# ── paths ────────────────────────────────────────────────────────
SCRIPT_DIR  = Path(__file__).resolve().parent
REPO_ROOT   = SCRIPT_DIR.parent.parent
UML_SRC_DIR = REPO_ROOT / "docs" / "uml"
UML_OUT_DIR = UML_SRC_DIR / "out"


def _find_plantuml() -> tuple[str, Path] | tuple[None, None]:
    """Return (mode, path) or (None, None)."""
    # 1. env var
    jar_env = os.environ.get("PLANTUML_JAR")
    if jar_env and Path(jar_env).is_file():
        return ("jar", Path(jar_env))

    # 2. local jar next to this script
    local_jar = SCRIPT_DIR / "plantuml.jar"
    if local_jar.is_file():
        return ("jar", local_jar)

    # 3. CLI on PATH
    cli = shutil.which("plantuml")
    if cli:
        return ("cli", Path(cli))

    return (None, None)


def _collect_puml_files() -> list[Path]:
    """Recursively gather *.puml under docs/uml/, excluding out/."""
    results: list[Path] = []
    for p in sorted(UML_SRC_DIR.rglob("*.puml")):
        try:
            p.relative_to(UML_OUT_DIR)
            continue  # inside out/ → skip
        except ValueError:
            pass
        results.append(p)
    return results


def _render(puml: Path, mode: str, tool: Path) -> bool:
    """Render a single .puml → SVG, preserving subdirectory structure."""
    rel = puml.relative_to(UML_SRC_DIR)
    out_dir = UML_OUT_DIR / rel.parent
    out_dir.mkdir(parents=True, exist_ok=True)

    if mode == "jar":
        cmd = ["java", "-jar", str(tool), "-tsvg", "-o", str(out_dir), str(puml)]
    else:
        cmd = [str(tool), "-tsvg", "-o", str(out_dir), str(puml)]

    print(f"  {rel}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"    ERROR: {result.stderr.strip()}", file=sys.stderr)
        return False
    return True


def main() -> None:
    print("SkiaKrita UML Generator")
    print("=" * 40)

    mode, tool = _find_plantuml()
    if mode is None:
        print(
            "ERROR: PlantUML not found.\n"
            "\n"
            "Install one of the following:\n"
            "  1) Set the PLANTUML_JAR environment variable to plantuml.jar\n"
            "  2) Copy plantuml.jar into tools/uml/\n"
            "  3) Install the PlantUML CLI  (e.g.  choco install plantuml)\n",
            file=sys.stderr,
        )
        sys.exit(1)

    print(f"PlantUML : {mode} → {tool}")

    puml_files = _collect_puml_files()
    if not puml_files:
        print("No .puml files found under docs/uml/.")
        return

    print(f"Files    : {len(puml_files)}")
    print(f"Output   : {UML_OUT_DIR}\n")

    UML_OUT_DIR.mkdir(parents=True, exist_ok=True)

    ok = 0
    fail = 0
    for p in puml_files:
        if _render(p, mode, tool):
            ok += 1
        else:
            fail += 1

    print(f"\nDone – {ok} succeeded, {fail} failed.")
    if fail:
        sys.exit(1)


if __name__ == "__main__":
    main()
