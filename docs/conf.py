import os
import sys
import subprocess

# Add project root to path for Python autodoc
sys.path.insert(0, os.path.abspath(".."))

project = "PolyScribe"
copyright = "2026, DracoRooks"
author = "DracoRooks"

# Extensions
extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.napoleon",  # Supports Google/NumPy style docstrings
    "myst_parser",          # Markdown support
    "breathe",              # C++ Doxygen integration
]

exclude_patterns = [
    "_build",
    "_doxygen",
    "venv",
    ".venv",
    "Thumbs.db",
    ".DS_Store",
]

# Source files
source_suffix = {
    ".rst": "restructuredtext",
    ".md": "markdown",
}
master_doc = "index"

# HTML Output Setup (Sphinx Book Theme)
html_theme = "sphinx_book_theme"
html_theme_options = {
    "repository_url": "https://github.com/DracoRooks/Scribe.git",
    "use_repository_button": True,
}

# Breathe Configuration (C++)
breathe_projects = {"PolyScribe": "./_doxygen/xml"}
breathe_default_project = "PolyScribe"

# Generate Doxygen XML on Read the Docs build
if os.environ.get("READTHEDOCS") == "True":
    subprocess.call("doxygen Doxyfile", shell=True)
