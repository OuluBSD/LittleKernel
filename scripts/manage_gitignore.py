#!/usr/bin/env python3
"""
Git Ignore Management Tool
This script helps maintain the .gitignore file to prevent build residues from dirtying the repository
"""

import os
import sys
from pathlib import Path

def ensure_gitignore_entries():
    """Ensure the .gitignore file has all necessary entries."""
    gitignore_path = Path('.gitignore')
    
    if not gitignore_path.exists():
        print(".gitignore file does not exist, creating it...")
        gitignore_path.write_text("")
    
    current_content = gitignore_path.read_text()
    
    # Entries to ensure are in the .gitignore
    required_entries = [
        "build/",
        "obj/",
        "iso/",
        "*.bin",
        "*.elf",
        "*.iso",
        "*.o",
        "*.obj",
        "*.out",
        "*.map",
        "*.dbg",
        "*.sym",
        ".vscode/",
        ".idea/",
        "*.swp",
        "*.swo",
        "*~",
        "#*",
        "*\\#",
        "littlekernel_venv/",
        "*.log",
        "*.lst",
        ".config.bak",
        "*.bak",
        "docs/*.tmp",
        "tmp/",
        "*.upp.backup",
        ".DS_Store",
        "Thumbs.db",
        "Makefile~",
        "compile_commands.json",
    ]
    
    missing_entries = []
    for entry in required_entries:
        if entry not in current_content:
            missing_entries.append(entry)
    
    if missing_entries:
        print(f"Adding {len(missing_entries)} missing entries to .gitignore:")
        for entry in missing_entries:
            print(f"  - {entry}")
        
        # Append missing entries to the file
        with open(gitignore_path, 'a') as f:
            f.write('\n# Added by git ignore management tool\n')
            for entry in missing_entries:
                f.write(f'{entry}\n')
    else:
        print("All required entries are already in .gitignore")

def cleanup_gitignore_duplicates():
    """Remove duplicate entries from .gitignore."""
    gitignore_path = Path('.gitignore')
    
    if not gitignore_path.exists():
        return
    
    lines = gitignore_path.read_text().splitlines()
    seen = set()
    unique_lines = []
    
    for line in lines:
        # Skip empty lines and comments when checking for duplicates
        if line.strip() and not line.startswith('#') and line in seen:
            print(f"Removing duplicate entry: {line}")
            continue
        seen.add(line)
        unique_lines.append(line)
    
    if len(unique_lines) != len(lines):
        # Write back the unique lines
        with open(gitignore_path, 'w') as f:
            f.write('\n'.join(unique_lines) + '\n')
        print(f"Cleaned up {len(lines) - len(unique_lines)} duplicate entries")

def scan_build_artifacts():
    """Scan for common build artifacts that should be ignored."""
    build_artifacts = []
    
    # Look for common build artifact patterns
    for root, dirs, files in os.walk('.'):
        # Skip .git directory
        if '.git' in root:
            continue
            
        for file in files:
            full_path = os.path.join(root, file)
            
            # Check for common artifact extensions
            if (file.endswith(('.bin', '.elf', '.iso', '.o', '.obj', '.out', '.map', '.dbg', '.sym', '.log', '.lst')) or
                (file.startswith('#') and file.endswith('#')) or
                file.endswith(('.swp', '.swo', '~')) or
                file == 'compile_commands.json'):
                
                # Get relative path
                rel_path = os.path.relpath(full_path, '.')
                
                # Skip if already in build/ directory
                if not rel_path.startswith('build/'):
                    build_artifacts.append(rel_path)
    
    if build_artifacts:
        print(f"Found {len(build_artifacts)} potential build artifacts that should be gitignored:")
        for artifact in build_artifacts:
            print(f"  - {artifact}")
        
        response = input("Add these to .gitignore? (y/n): ")
        if response.lower() == 'y':
            with open('.gitignore', 'a') as f:
                f.write('\n# Build artifacts found during scan\n')
                for artifact in build_artifacts:
                    f.write(f'{artifact}\n')
            print("Added artifacts to .gitignore")
    else:
        print("No new build artifacts found outside of build directory")

def main():
    """Main function to manage gitignore."""
    print("Git Ignore Management Tool")
    print("==========================")
    
    if len(sys.argv) < 2:
        print("Usage:")
        print("  python3 manage_gitignore.py ensure    # Ensure required entries are present")
        print("  python3 manage_gitignore.py cleanup   # Remove duplicate entries")
        print("  python3 manage_gitignore.py scan      # Scan for build artifacts")
        print("  python3 manage_gitignore.py all       # Run all operations")
        sys.exit(1)
    
    command = sys.argv[1]
    
    if command == "ensure":
        ensure_gitignore_entries()
    elif command == "cleanup":
        cleanup_gitignore_duplicates()
    elif command == "scan":
        scan_build_artifacts()
    elif command == "all":
        ensure_gitignore_entries()
        cleanup_gitignore_duplicates()
        scan_build_artifacts()
    else:
        print(f"Unknown command: {command}")
        sys.exit(1)

if __name__ == "__main__":
    main()