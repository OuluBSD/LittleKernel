#!/usr/bin/env python3
"""
Project File Synchronization Tool
This script synchronizes Ultimate++ project files (.upp) with Makefile
"""

import os
import sys
import json
import re
from pathlib import Path

def get_cpp_files(directory):
    """Get all C++ files in the directory recursively."""
    cpp_extensions = ['.cpp', '.c', '.cc', '.cxx']
    files = []
    
    for root, dirs, filenames in os.walk(directory):
        for filename in filenames:
            if any(filename.endswith(ext) for ext in cpp_extensions):
                # Convert to relative path from project root
                files.append(os.path.relpath(os.path.join(root, filename), "."))
    
    return files

def get_asm_files(directory):
    """Get all assembly files in the directory."""
    asm_extensions = ['.asm', '.s', '.S']
    files = []
    
    for root, dirs, filenames in os.walk(directory):
        for filename in filenames:
            if any(filename.endswith(ext) for ext in asm_extensions):
                # Convert to relative path from project root
                files.append(os.path.relpath(os.path.join(root, filename), "."))
    
    return files

def update_upp_project(upp_file, kernel_cpp_files, lib_cpp_files, asm_files):
    """Update the U++ project file with current file lists."""
    if not os.path.exists(upp_file):
        print(f"Warning: {upp_file} does not exist, creating it")
        # Create a basic U++ project structure
        project_content = {
            "name": "Kernel",
            "files": {
                "cpp": kernel_cpp_files,
                "asm": asm_files
            }
        }
        
        with open(upp_file, 'w') as f:
            json.dump(project_content, f, indent=2)
        return
    
    # Read existing project file and update
    with open(upp_file, 'r') as f:
        content = f.read()
    
    # This is a simplified approach since .upp files can be in different formats
    # For this implementation, we'll look for file sections and update them
    updated_lines = []
    in_kernel_files = False
    in_lib_files = False
    lines = content.splitlines()
    
    for line in lines:
        if 'Kernel.cpp' in line or 'Kernel.h' in line:
            in_kernel_files = True
        elif 'Library.cpp' in line or 'Library.h' in line:
            in_lib_files = True
        elif in_kernel_files and line.strip() == '':  # End of section
            # Insert updated kernel files
            for f in kernel_cpp_files:
                updated_lines.append(f"  {f},")
            in_kernel_files = False
        elif in_lib_files and line.strip() == '':  # End of section
            # Insert updated library files
            for f in lib_cpp_files:
                updated_lines.append(f"  {f},")
            in_lib_files = False
        else:
            updated_lines.append(line)
    
    with open(upp_file, 'w') as f:
        f.write('\n'.join(updated_lines))

def update_makefile(kernel_cpp_files, lib_cpp_files, asm_files):
    """Update the Makefile with current file lists."""
    # Read the current Makefile
    with open('Makefile', 'r') as f:
        content = f.read()
    
    # Update SOURCES lines with the current file lists
    # Define the source file variables
    kernel_sources_str = ' '.join(kernel_cpp_files)
    lib_sources_str = ' '.join(lib_cpp_files)
    asm_sources_str = ' '.join(asm_files)
    
    # Update SOURCES variable in Makefile
    sources_pattern = r'^SOURCES\s*:?=.*$'
    asm_sources_pattern = r'^ASM_SOURCES\s*:?=.*$'
    
    # Split content into lines for processing
    lines = content.splitlines()
    new_lines = []
    
    for line in lines:
        if line.startswith('SOURCES :='):
            # Build the new SOURCES line
            all_sources = kernel_cpp_files + lib_cpp_files
            line = f"SOURCES := {' '.join(all_sources)}"
        elif line.startswith('ASM_SOURCES :='):
            line = f"ASM_SOURCES := {' '.join(asm_files)}"
        elif line.startswith('KERNEL_SOURCES :='):
            line = f"KERNEL_SOURCES := {' '.join(kernel_cpp_files)}"
        elif line.startswith('LIB_SOURCES :='):
            line = f"LIB_SOURCES := {' '.join(lib_cpp_files)}"
        
        new_lines.append(line)
    
    # Write the updated content back to Makefile
    with open('Makefile', 'w') as f:
        f.write('\n'.join(new_lines))

def sync_projects():
    """Synchronize project files."""
    print("Synchronizing project files...")
    
    # Get all relevant files
    kernel_cpp_files = get_cpp_files('kernel/Kernel')
    lib_cpp_files = get_cpp_files('kernel/Library')
    asm_files = get_asm_files('kernel/Kernel')
    
    print(f"Found {len(kernel_cpp_files)} kernel C++ files")
    print(f"Found {len(lib_cpp_files)} library C++ files")
    print(f"Found {len(asm_files)} assembly files")
    
    # Update Makefile
    update_makefile(kernel_cpp_files, lib_cpp_files, asm_files)
    print("Makefile updated")
    
    # Update U++ project files
    kernel_upp = 'kernel/Kernel/Kernel.upp'
    lib_upp = 'kernel/Library/Library.upp'
    
    if os.path.exists(kernel_upp):
        update_upp_project(kernel_upp, kernel_cpp_files, [], asm_files)
        print("Kernel.upp updated")
    else:
        print(f"Warning: {kernel_upp} does not exist")
    
    if os.path.exists(lib_upp):
        update_upp_project(lib_upp, [], lib_cpp_files, [])
        print("Library.upp updated")
    else:
        print(f"Warning: {lib_upp} does not exist")
    
    print("Synchronization complete")

def add_file_to_projects(file_path):
    """Add a new file to all project files."""
    print(f"Adding file {file_path} to project files...")
    
    # Determine the project this file belongs to
    if 'kernel/Kernel' in file_path:
        project = 'kernel'
    elif 'kernel/Library' in file_path:
        project = 'library'
    else:
        print(f"Warning: File {file_path} is not in a known project directory")
        return
    
    # Update Makefile
    with open('Makefile', 'r') as f:
        content = f.read()
    
    lines = content.splitlines()
    new_lines = []
    
    for line in lines:
        if line.startswith('SOURCES :='):
            sources = line.split(':=', 1)[1].strip().split()
            if file_path not in sources:
                sources.append(file_path)
                line = f"SOURCES := {' '.join(sources)}"
        elif line.startswith('KERNEL_SOURCES :=') and project == 'kernel':
            sources = line.split(':=', 1)[1].strip().split()
            if file_path not in sources:
                sources.append(file_path)
                line = f"KERNEL_SOURCES := {' '.join(sources)}"
        elif line.startswith('LIB_SOURCES :=') and project == 'library':
            sources = line.split(':=', 1)[1].strip().split()
            if file_path not in sources:
                sources.append(file_path)
                line = f"LIB_SOURCES := {' '.join(sources)}"
        
        new_lines.append(line)
    
    with open('Makefile', 'w') as f:
        f.write('\n'.join(new_lines))
    
    print(f"Added {file_path} to Makefile")

def remove_file_from_projects(file_path):
    """Remove a file from all project files."""
    print(f"Removing file {file_path} from project files...")
    
    # Update Makefile
    with open('Makefile', 'r') as f:
        content = f.read()
    
    lines = content.splitlines()
    new_lines = []
    
    for line in lines:
        if line.startswith('SOURCES :='):
            sources = line.split(':=', 1)[1].strip().split()
            if file_path in sources:
                sources.remove(file_path)
                line = f"SOURCES := {' '.join(sources)}"
        elif line.startswith('KERNEL_SOURCES :='):
            sources = line.split(':=', 1)[1].strip().split()
            if file_path in sources:
                sources.remove(file_path)
                line = f"KERNEL_SOURCES := {' '.join(sources)}"
        elif line.startswith('LIB_SOURCES :='):
            sources = line.split(':=', 1)[1].strip().split()
            if file_path in sources:
                sources.remove(file_path)
                line = f"LIB_SOURCES := {' '.join(sources)}"
        elif line.startswith('ASM_SOURCES :='):
            sources = line.split(':=', 1)[1].strip().split()
            if file_path in sources:
                sources.remove(file_path)
                line = f"ASM_SOURCES := {' '.join(sources)}"
        
        new_lines.append(line)
    
    with open('Makefile', 'w') as f:
        f.write('\n'.join(new_lines))
    
    print(f"Removed {file_path} from Makefile")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage:")
        print("  python3 sync_project_files.py sync          # Synchronize all project files")
        print("  python3 sync_project_files.py add <file>    # Add a file to projects")
        print("  python3 sync_project_files.py remove <file> # Remove a file from projects")
        sys.exit(1)
    
    command = sys.argv[1]
    
    if command == "sync":
        sync_projects()
    elif command == "add" and len(sys.argv) == 3:
        add_file_to_projects(sys.argv[2])
    elif command == "remove" and len(sys.argv) == 3:
        remove_file_from_projects(sys.argv[2])
    else:
        print("Invalid command or arguments")
        sys.exit(1)